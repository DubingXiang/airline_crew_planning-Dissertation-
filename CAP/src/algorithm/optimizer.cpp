#include "Optimizer.h"
#include "..\..\UtilFunc.h"
#include "..\structures\generic\csvClassesTransOptimizable.h"
#include "..\structures\crew_rules.h"
#include "..\structures\network\seg_network.h"
#include "..\structures\network\crew_network.h"
#include "..\structures\network\seg_path.h"
#include "..\structures\network\crew_path.h"
#include "..\algorithm\crewgroup_searcher.h"
#include "..\io\output\output_handler.h"
#include "..\structures\param_setting\cost_parameters.h"

//! for debug
#include "..\..\include\SummeryTool_H.h"
static Summery::StopWatch TIMER;

const char* CREW_STATUS_FILE = "../data/output/crew_status_record.txt";

//! end for debug
const int kSEVEN_DAYS = 7 * 24 * 60;

Optimizer::Optimizer() {	
	//_column_generation = new ColumnGeneration();
}
Optimizer::~Optimizer() {
	delete _crewNet;
	delete _segNet;	
}


void Optimizer::optimize() {
	// 1.initialize	
	_segNet->createNetwork();
	
	TIMER.Start();	
	_crewNet->createNetwork();
	TIMER.Stop();
	TIMER.printElapsed();

	OutputHandler output_handler;
	std::stringstream cur_day_sch_file;
	std::stringstream cur_day_crew_status_file;

	
	_begining_plan = getStartTimeOfDay(_segNet->nodeSet.front()->startDtLoc);
	for (auto& crew : _optCrewSet) {
		crew->workStatus->setInitStatus(_begining_plan, "");
	}

	clusterSegNode();
	//size_t length_plan = _day_segnode_map.size(); //TODO: 放在初始化的时候 //8-15
	
	size_t iter = 0;
	time_t start_cur_day;
	for (/*size_t iter = 0; iter < length_plan; iter++*/ //8-15
		std::map<time_t, SegNodeSet>::iterator it = _daytime_segnode_map.begin(); it != _daytime_segnode_map.end(); it++) {
		
		//setCurDayStartNodeSet(iter); //8-15
		_cur_day_segnode_set = &it->second;
		start_cur_day = it->first;
		
		_segNet->updateResourceAndSink();
		_segNet->updateVirtualArcs(*_cur_day_segnode_set);
		// 2.crew group searching
		CrewGroup initial_group;
		initialCrewGroup(initial_group);
		std::vector<CrewGroup*> initial_groups;
		initial_groups.emplace_back(&initial_group);
		// 3.seg path searching
		/*SubProblem initial_soln(*_crewNet, *_segNet, *_rules);
		initial_soln.findSegPaths();
		initial_soln.getCurLocalPool();*/
		//initial_soln
		// 4.
		ColumnGeneration* column_generation = new ColumnGeneration();
 		column_generation->init(iter, initial_groups, *_crewNet, *_segNet, *_rules, *_penalty);
		column_generation->solve();

		Solution* cur_day_soln = new Solution(column_generation->getBestSoln());
		soln_pool.emplace_back(cur_day_soln);
		//update crew's and seg's status according cur day's decision
		updateStatus(start_cur_day, *cur_day_soln);

		cur_day_sch_file.str("");
		cur_day_crew_status_file.str("");
		cur_day_sch_file << "../data/output/crew_schedule/schedule_day_" << std::to_string(iter + 1) << ".txt";
		cur_day_crew_status_file << "../data/output/crew_schedule/crew_status_day_" << std::to_string(iter + 1) << ".txt";
		output_handler.writeSchedule(*cur_day_soln, *_cur_day_segnode_set, cur_day_sch_file.str());
		output_handler.writeCrewStatus(*cur_day_soln, cur_day_crew_status_file.str());
		//记录整个周期crew的状态
		output_handler.writeCrewStatus(_optCrewSet, CREW_STATUS_FILE);

		delete column_generation;
		++iter;
	}


	//statistic
	std::string crew_statistic_csv = "../data/output/statistic files/crew_statistic.csv";
	std::string uncovered_flights_txt = "../data/output/statistic files/uncovered_flights.txt";
	output_handler.writeCrewStatistic(_optCrewSet, crew_statistic_csv.data());
	output_handler.writeUncoveredFlight(_segNet->nodeSet, uncovered_flights_txt.data());
	

}


void Optimizer::initialCrewGroup(CrewGroup& initGroup) {
	//random select a group according to rules.rankconbination
	auto combi = _rules->rankCombinationSet.front();
	std::vector<Opt_CREW*> crew_combi;
		
	/*for (const auto& crew1 : _optCrewSet) {
		for (const auto& crew2 : _optCrewSet) {
			if (crew1 != crew2) {
				crew_combi.emplace_back(crew1);
				crew_combi.emplace_back(crew2);
				if (_rules->isFasibleCombination(crew_combi)) {
					initGroup.getCrewGroup() = crew_combi;		
					return;
				}
			}			
		}		
	}*/
	for (size_t i = 2; i < _crewNet->nodeSet.size(); i++) {
		auto crew_node1 = _crewNet->nodeSet[i];
		for (size_t j = i+1; j < _crewNet->nodeSet.size(); j++) {
			auto crew_node2 = _crewNet->nodeSet[j];
			
			crew_combi.emplace_back(crew_node1->optCrew);
			crew_combi.emplace_back(crew_node2->optCrew);
			if (_rules->isFasibleCombination(crew_combi)) {
				initGroup.getNodeSequence().emplace_back(crew_node1);
				initGroup.getNodeSequence().emplace_back(crew_node2);

				initGroup.setCrewGroup();
				initGroup.setCrewIndexSet();
				initGroup.setBasicProperties();
				return;
			}
			
		}
	}

}

void Optimizer::clusterSegNode() {	
	for (const auto& segnode : _segNet->nodeSet) {
		_day_segnode_map[segnode->optSegment->getDate()].emplace_back(segnode);
	
		_daytime_segnode_map[getStartTimeOfDay(segnode->optSegment->getStartTimeLocSch())].emplace_back(segnode);
	
	}
	
}
void Optimizer::setCurDayStartNodeSet(int curDay) {
	time_t cur_day = _begining_plan + curDay * _SECONDS_ONE_DAY;
	while (_daytime_segnode_map.find(cur_day) == _daytime_segnode_map.end()) {
		cur_day += _SECONDS_ONE_DAY;
	}
	_cur_day_segnode_set = &_daytime_segnode_map[cur_day];
}

//! 当前阶段的duty的departureDt和crew在当前阶段开始时的endDtLoc之间的时间间隔，需要判断该间隔是否满足了day off
//! 若该时间间隔超过day off，那么实际上，crew就是进行了一次day off
void Optimizer::updateStatus(const time_t startCurDay, Solution& soln) {
	ColumnPool& pool = soln.column_pool;
	time_t endCurDay = startCurDay + 86400 - 1;
	Column* col;
	for (size_t i = 0; i < pool.size(); i++) {
		col = pool[i];
		auto segpath = col->_segpath;
		
		auto segnode_set = segpath->getNodeSequence();
		for (auto& segnode : segnode_set) {
			segnode->optSegment->setAssigned(true);
		}

		time_t 	relax_end_loc = col->type == ColumnType::duty ?
						  segpath->startNode->startDtLoc : endCurDay;
		
		
		auto crew_set = col->_crewgroup->getCrewGroup();
		CrewStatus* status;
		for (auto& crew : crew_set) {
			status = crew->workStatus;
			status->setAssigned(true);
			
			status->accumuFlyMin += segpath->total_fly_mint;
			status->accumuCreditMin += segpath->total_credit_mint;
			status->sevenDayTotalFlyMint += segpath->total_fly_mint;
			
			status->wholePlanTotalFlyMint += segpath->total_fly_mint;
			status->wholePlanTotalCreditMint += segpath->total_credit_mint;

			int duration = relax_end_loc - status->endDtLoc;
			if (duration >= _rules->minDayOffMin*60) {
				status->accumuFlyMin = 0;
				status->accumuCreditMin = 0;
				status->wholePlanTotalCreditMint -= duration;
			}
			if (col->type == ColumnType::duty) {			
				status->endDtLoc = segpath->endNode->endDtLoc;
				status->restStation = segpath->endNode->arvStation;
			}
			if (endCurDay - status->dateLocFlyBegining >= kSEVEN_DAYS) {
				status->dateLocFlyBegining = endCurDay + 1;
				status->sevenDayTotalFlyMint = 0;
			}

		}

	}
}



/****************/
void Optimizer::loadData(std::map<std::string, std::vector<void*>>& dataSet, const std::vector<std::string>& objNameSet) {
	_inputHandler.transformInputObjSet(dataSet, objNameSet);
}

void Optimizer::loadCrewRules(CrewRules& rules) {
	_rules = &rules;
}
void Optimizer::loadPenaltySetting(const Penalty& penaltySeeting) {
	_penalty = &penaltySeeting;
}
void Optimizer::init() {
	_inputHandler.sortCrewRank();
	
	_inputHandler.createOptSegments(&_optSegSet);
	_inputHandler.matchOptSegmentAndComposition(&_optSegSet);
	_inputHandler.setRankToNumMapOfOptSegment(&_optSegSet);
	_inputHandler.createOptCrews(&_optCrewSet);	
	_inputHandler.matchOptCrewAndRank(&_optCrewSet);
	_inputHandler.matchOptCrewAndBase(&_optCrewSet);	
	
	
	for (auto& crew : _optCrewSet) {
		crew->setCurRank();
		crew->setCurPosition();		
	}

	_crewNet = new CrewNetwork(&_optCrewSet, _rules);
	_segNet = new SegNetwork(&_optSegSet, &_inputHandler.getBaseSet(), _rules);
	
}




void Optimizer::createCase() {
	_inputHandler.setAirportSet();
	_inputHandler.createSpecialArpSet();
	
	_inputHandler.randomSetCrewSkills(&_optCrewSet);
	_inputHandler.setRankCombination(_rules);
	_inputHandler.setRankCombination_OnePermutation(_rules);

	_optCrewSet = *_inputHandler.getPilotSet(_optCrewSet);

	_inputHandler.setIndexOfCrew(&_optCrewSet);
}