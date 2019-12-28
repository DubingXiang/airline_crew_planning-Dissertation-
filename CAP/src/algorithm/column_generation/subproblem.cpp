#include "subproblem.h"
#include "..\..\..\include/algorithm_xdb.h"
#include "..\..\..\UtilFunc.h"




/*DEBUG 8-15 find uncover seg and possible crew*/
//const std::vector<long long> kUNCOVER_SEG_ID = { 54266023, 54253231, 54246335,54266024,54267951 };

const std::map<long long, std::vector<std::string> > kUNCOVER_SEG_POSSIBLE_CREW = {
			{54253231,{"000013","002117"}},
			{54253232,{"000009","001377"}},
			{54270605,{"000021","003116"}}
			};
const int kNUM_GROUPS_FOR_ONE_DUTY = 3;
const int kNUM_GROUPS_FOR_ONE_COMPOSITION = 1e5;

/*end! DEBUG 8-15 find uncover seg and possible crew*/

SubProblem::SubProblem(CrewNetwork& crewNet, SegNetwork& segNet, const CrewRules& rules) {
	_rules = &rules;
	_group_searcher.init(crewNet, rules);
	_seg_path_searcher.init(segNet, *segNet.resource, rules);

	//for debug 暂时不建立crewNet
	_crewnode_set = crewNet.nodeSet;
	_crewnode_set.erase(_crewnode_set.begin(), _crewnode_set.begin() + 2);
	
	// TODO:实际当作一个变量传进来即可，不然每次（天）都循环赋值
	for (const auto& crewnode : _crewnode_set) {
		_pos_crewnodes_map[crewnode->optCrew->getCurPosition()].emplace_back(crewnode);
		_rank_to_mean_fly_mint[crewnode->optCrew->getCurRank()] = 0;
		++_rank_to_num_crew_map[crewnode->optCrew->getCurRank()];
	}



}
SubProblem::~SubProblem() {

}


void SubProblem::setInitialGroups(std::vector<CrewGroup*>& initialGroups) {
	_group_searcher.getPathSet() = initialGroups;	
}

void SubProblem::findSegPaths() {
	_seg_path_searcher.search();	
	_cur_day_path_set = _seg_path_searcher.getPathSet();

}

void SubProblem::setPathStatus(const Penalty& penalty) {	
	for (auto& path : _cur_day_path_set) {
		path->computeCost(penalty);	
	}
	
	_seg_path_searcher.clear();
}

void SubProblem::setCurDaySegSet() {
	for (const auto& segpath : _cur_day_path_set) {
		std::vector<SegNode*>& segnode_set = segpath->getNodeSequence();
		for (const auto& segnode : segnode_set) {
			if (std::find(_cur_day_seg_set.begin(), _cur_day_seg_set.end(), segnode->optSegment) 
				== _cur_day_seg_set.end()) {
				_cur_day_seg_set.emplace_back(segnode->optSegment);
			}
		}
	}
}
void SubProblem::setIndexOfCurSeg() {
	for (auto& path : _cur_day_path_set) {
		path->setSegIndexSet(_cur_day_seg_set);
	}
}

void SubProblem::labelSpecialSegPath(std::vector<std::string>& specialAirports) {
	std::string dep_arp;
	std::string arv_arp;
	for (auto& segpath : _cur_day_path_set) {
		segpath->specialCredentials["SpecialAirport"] = 0;
		auto node_set = segpath->getNodeSequence();
		for (const auto& segnode : node_set) {
			dep_arp = segnode->depStation;
			arv_arp = segnode->arvStation;
			if (std::find(specialAirports.begin(), specialAirports.end(), dep_arp) != specialAirports.end()
				|| std::find(specialAirports.begin(), specialAirports.end(), arv_arp) != specialAirports.end()) {
				segpath->specialCredentials["SpecialAirport"] = +1;
			}
		}
	}
}
void SubProblem::groupingSegPathByComposition() {
	for (const auto& path : _cur_day_path_set) {
		_compo_mode_segpath_set[path->getCompoMode()->name].emplace_back(path);
	}
}


void SubProblem::estimateMeanFlyMin() {
	//取segpath_set的中间80%，以及crew_set的中间80%，求飞时均值

	std::sort(_cur_day_path_set.begin(), _cur_day_path_set.end(),
		[](const SegPath* a, const SegPath* b) {return a->total_fly_mint > b->total_fly_mint; });
	int low = 0.1 * _cur_day_path_set.size();
	int high = 0.9 * _cur_day_path_set.size();

	int mean_segpath_fly_min = 0;
	for (size_t i = low; i < high; i++) {
		mean_segpath_fly_min += _cur_day_path_set[i]->total_fly_mint;
	}
	mean_segpath_fly_min /= (high - low + 1);

	//! changed 8-12-2019
	//! 不应该改变_crewnode_set的顺序，因为之后的对偶价格要对应 
	std::vector<CrewNode*> temp_crewnode_set(_crewnode_set);

	std::sort(temp_crewnode_set.begin(), temp_crewnode_set.end(),
		[](const CrewNode* a, const CrewNode* b) {return a->optCrew->workStatus->accumuFlyMin < b->optCrew->workStatus->accumuFlyMin; });
	low = 0.1 * (temp_crewnode_set.size());
	high = 0.9 * (temp_crewnode_set.size());
	int mean_crew_fly_min = 0;
	for (size_t i = low; i < high; i++) {
		//mean_crew_fly_min += temp_crewnode_set[i]->optCrew->workStatus->accumuFlyMin; //TODO:应该是total_fly_min  
		mean_crew_fly_min += temp_crewnode_set[i]->optCrew->workStatus->wholePlanTotalFlyMint;
	}
	mean_crew_fly_min /= (high - low + 1);

	_mean_fly_mint = mean_segpath_fly_min + mean_crew_fly_min;
}

void SubProblem::estimateMeanFlyMinPrecisly() {	
	std::map<std::string, int> rank_to_total_flymin;	
	for (const auto& seg : _cur_day_seg_set) {
		for (const auto& rank_num : seg->getRankToNumMap()) {
			rank_to_total_flymin[rank_num.first] += rank_num.second * seg->getBlkMinutes();			
		}
	}

	for (const auto& crew_node : _crewnode_set) {
		rank_to_total_flymin[crew_node->optCrew->getCurRank()] += crew_node->optCrew->workStatus->wholePlanTotalFlyMint;
	}

	for (const auto& rank_num : _rank_to_num_crew_map) {
		_rank_to_mean_fly_mint[rank_num.first] = rank_to_total_flymin[rank_num.first] / rank_num.second;
	}

}

void SubProblem::updateDuals(std::vector<double>& segCoverDuals, std::vector<double>& crewAssignDuals) {
	for (size_t i = 0; i < _cur_day_seg_set.size(); i++) {
		_cur_day_seg_set[i]->setDualPrice(segCoverDuals[i]);
	}
	for (auto& path : _cur_day_path_set) {
		path->total_dual_price = 0;
		auto node_of_path = path->getNodeSequence();
		for (auto& node : node_of_path) {
			if (node->nodeType == NodeType::dhd) {
				continue;
			}
			path->total_dual_price += node->optSegment->getDualPrice();
		}
	}

	for (size_t i = 0; i < _crewnode_set.size(); i++) {
		_crewnode_set[i]->setPrice(crewAssignDuals[i]);
	}
}
void SubProblem::findGroups() {
	_group_searcher.search();	
}

void SubProblem::matchGroupAndPath() {
	//_local_pool.clear();
	auto group_set = _group_searcher.getPathSet();
	std::string special = "SpecialAirport";
	//TODO:多线程加速
	std::string rest_station;
	for (auto& group : group_set) {
		//group->setBasicProperties();
		
		rest_station = group->getCrewGroup().front()->workStatus->restStation;

		int gap_crew_segpath = 0;		
		for (auto& segpath : _cur_day_path_set) {
			SegNode* start_node = segpath->startNode;
			SegNode* end_node = segpath->endNode;

			/*DEBUG 8-15 find uncover seg and possible crew*/
			for (const auto& crew : group->getCrewGroup()) {
				if (crew->getIdCrew() == "000013" || crew->getIdCrew() == "002117") {
					int tt = 0;
				}
			}
			
			SegNodeSet& segnode_set = segpath->getNodeSequence();
			for (size_t s = 0; s < segnode_set.size(); s++) {
				long long seg_id = segnode_set[s]->optSegment->getDBId();
				if (seg_id == 54253231) {
					int y = 0;
				}
			}
		
			
			
			/*bool flag = false;
			SegNodeSet& segnode_set = segpath->getNodeSequence();			
			for (size_t s = 0; s < segnode_set.size(); s++) {
				if (flag) {
					break;
				}
				long long seg_id = segnode_set[s]->optSegment->getDBId();				
				auto pos = kUNCOVER_SEG_POSSIBLE_CREW.find(seg_id);
				if (pos != kUNCOVER_SEG_POSSIBLE_CREW.end()) {
					for (const auto& crew : group->getCrewGroup()) {
						const std::vector<std::string>& possible_crews = kUNCOVER_SEG_POSSIBLE_CREW.at(seg_id);
						if (isContain(possible_crews, crew->getIdCrew())) {							
							std::cout << "segment " << seg_id << " crew " << crew->getIdCrew() << "\n";
							flag = true;
							break;
						}
					}
				}
			}*/
						
			/*end! DEBUG 8-15 find uncover seg and possible crew*/

			if (rest_station != "" && rest_station != start_node->depStation) {
				//=="",说明是初次迭代，不需要满足空间接续。不过实际上crew的初始状态中是有一个计划周期开始时的所在地的信息，这里先不考虑
				continue;
			}

			//检查配比是否符合
			if (group->compo_mode->name  != segpath->getCompoMode()->name) {
				continue;
			}

			//2.检查时间约束
			gap_crew_segpath = (start_node->startDtLoc - group->endDtLoc) / 60;
			if (gap_crew_segpath <= 0) {
				continue;
			}
			////限制duty和crew之间的间隔，实际上就是限制crew不能休息太久
			//else if (gap_crew_segpath > _rules->maxDayOffMin) {
			//	//break;
			//	continue; 
			//}
			//不能担当该duty，因为会超时.d越大，时长越长，所以不用再往下查找，直接break
			else if (0 < gap_crew_segpath && gap_crew_segpath < _rules->minDayOffMin) {
				//不需要day off，就是duty之间的正常接续
				//但预先判断一下若担当该duty，是否会超时
				if (group->max_credit_mint + gap_crew_segpath + segpath->total_credit_mint > _rules->maxCreditMin
					|| group->max_total_fly_mint + segpath->total_fly_mint > _rules->maxWeekFlyMin ) {
					
					continue;
				}
			}
							
			//3.检查完空间和时间，检查资质	
			if (segpath->specialCredentials[special] >= 1 &&
				group->spetialCredentials[special] != 1) { //duty需要特殊资质，而crew不具备
				continue;
			}			

			//均满足，该segPath和该crewGroup可以搭配
			// create new column
			//int balance_cost = calBalanceCost(segpath->total_fly_mint, *group);
			int balance_cost = calBalanceCostByVariance(segpath->total_fly_mint, *group); //changed 20190822
			int reduced_cost = segpath->getCost() + balance_cost - segpath->total_dual_price - group->getCost();
			if (reduced_cost <= -1e-8) {
				Column* col = new Column(); //TODO:8-15 取（reduced cost最小的）top k，利用大顶堆，只有小于堆顶才new
				col->_segpath = segpath;
				col->_crewgroup = group;
				col->cost += segpath->getCost() + balance_cost;
				col->reduced_cost = reduced_cost;

				_local_pool.emplace_back(col);
			}
			
		}
	}	
}

//! 1.按配比对duty分组
//！2.针对每个配比搜索group
//！3.将2中搜索到的group与对应的duty配对，得到新columns
//! 4.将新columns添加到local_pool中
void SubProblem::findColumnSet() {
	sort_pos_crew_set(); //对每种position的crew按price排序	
	for (const auto& k_v : _compo_mode_segpath_set) {
		std::string cur_compo_mode = k_v.first;
		const std::vector<SegPath*>& cur_duty_set = k_v.second;
		
		std::vector<CrewGroup*>* cur_group_set = new std::vector<CrewGroup*>();
		searchGroupByComposition(cur_compo_mode, cur_group_set); //得到crewGroup == partial_group_set
		//对搜索到的group赋值
		for (auto& group : *cur_group_set) {
			group->setCrewIndexSet();
			group->setCrewGroup();
			group->setBasicProperties();
			group->computeCost();
		}
		
		std::sort(cur_group_set->begin(), cur_group_set->end(),
			[](const CrewGroup* a, const CrewGroup* b) {return a->getCost() > b->getCost(); });
		// match duty and group
		// create new columns
		std::set<int> assigned_group_set; //分配了duty的group
		for (auto& duty : cur_duty_set) {
			int num_groups = kNUM_GROUPS_FOR_ONE_DUTY;			
			for (size_t i = 0; i < cur_group_set->size() && num_groups != 0; i++) {
				CrewGroup* group = (*cur_group_set)[i];
				if (!isMatchable(*group, *duty)) {
					continue;
				}				

				//int balance_cost = calBalanceCost(duty->total_fly_mint, *group);
				int balance_cost = calBalanceCostByVariance(duty->total_fly_mint, *group); //changed 20190822
				int reduced_cost = duty->getCost() + balance_cost - duty->total_dual_price - group->getCost();
				if (reduced_cost <= -1e-8) {
					Column* col = new Column(); //TODO:8-15 取（reduced cost最小的）top k，利用大顶堆，只有小于堆顶才new
					col->_segpath = duty;
					col->_crewgroup = group;
					col->cost += duty->getCost() + balance_cost;
					col->reduced_cost = reduced_cost;

					_local_pool.emplace_back(col);

					--num_groups;
					assigned_group_set.insert(i);
				}
			}
		}
		
		//释放未分配给任何duty的group		
		for (size_t i = 0; i < cur_group_set->size(); i++) {
			if (!std::binary_search(assigned_group_set.begin(), assigned_group_set.end(), i)) {				
				delete (*cur_group_set)[i];			
			}			
		}
		cur_group_set->clear();
		delete cur_group_set;
	}
}

void SubProblem::sort_pos_crew_set() {
	for (auto& k_v : _pos_crewnodes_map) {
		std::sort(k_v.second.begin(), k_v.second.end(),
			[](const CrewNode* a, const CrewNode* b) {return a->price > b->price; });
	}
}

void SubProblem::searchGroupByComposition(const std::string compositionName, std::vector<CrewGroup*>* crewGroupSet) {	
	std::vector<CrewGroup*> cur_partial_group_set;
	std::vector<CrewGroup*> next_partial_group_set;

	const std::vector<std::string>& cur_sequnce_set = _rules->compo_sequences_map.at(compositionName);
	std::vector<std::string> pos_set;
	for (const auto& seq : cur_sequnce_set) {
		pos_set.clear();
		split(seq, '-', pos_set);

		// 如果该option中的某个号位不存在crew，则换下一个号位
		bool flag = false;
		for (const auto& pos : pos_set) {
			if (!isContainKey(_pos_crewnodes_map, pos)) {
				flag = true;
				break;
			}
		}
		if (flag) {
			continue;
		}		

		// 1.号位1的每个crew为一个group
		for (const auto& node : _pos_crewnodes_map[pos_set[0]]) {			
			CrewGroup* single_group = new CrewGroup();
			single_group->getNodeSequence().emplace_back(node);
			
			cur_partial_group_set.emplace_back(single_group);
		}

		size_t total_num_group = crewGroupSet->size();
		CrewNode *cur_node;
		std::vector<CrewNode*>* next_node_set;
		CrewStatus *cur_status, *next_status;
		size_t size = pos_set.size();
		size_t i = 1;
		while (i != size) {
			next_node_set = &_pos_crewnodes_map[pos_set[i]];
			for (auto& cur_group : cur_partial_group_set) {
				cur_node = cur_group->getNodeSequence().back();
				cur_status = cur_node->optCrew->workStatus;
				for (const auto& next_node : *next_node_set) {
					next_status = next_node->optCrew->workStatus;
					if (cur_node == next_node) { 
						continue;
					}
					if (cur_status->restStation == next_status->restStation
						|| (cur_status->restStation == "" || next_status->restStation == "")) {
						
						if (i + 1 == size && total_num_group + next_partial_group_set.size() >= kNUM_GROUPS_FOR_ONE_COMPOSITION) {
							goto here;
						}
						CrewGroup* next_group = new CrewGroup(*cur_group);
						next_group->getNodeSequence().emplace_back(next_node);

						next_partial_group_set.emplace_back(next_group);
					}
				}
				delete cur_group;
			}
			here:
			cur_partial_group_set.clear();
			cur_partial_group_set.shrink_to_fit();
			cur_partial_group_set.swap(next_partial_group_set);

			++i;
		}	
		
		// 循环结束，得到完整的group_set，赋给param <crewGroupSet>
		size_t needed_size = crewGroupSet->size() + cur_partial_group_set.size();
		crewGroupSet->reserve(needed_size);
		crewGroupSet->insert(crewGroupSet->end(), cur_partial_group_set.begin(), cur_partial_group_set.end());
		cur_partial_group_set.clear();
		cur_partial_group_set.shrink_to_fit();
	}
}

bool SubProblem::isMatchable(CrewGroup& group, SegPath& segPath) {
	SegNode* start_node = segPath.startNode;
	SegNode* end_node = segPath.endNode;
	
	if (group.curStation != "" && group.curStation != start_node->depStation) {
		//=="",说明是初次迭代，不需要满足空间接续。不过实际上crew的初始状态中是有一个计划周期开始时的所在地的信息，这里先不考虑
		return false;
	}

	//检查配比是否符合
	if (group.compo_mode->name != segPath.getCompoMode()->name) {
		return false;
	}

	//2.检查时间约束
	int gap_crew_segpath = (start_node->startDtLoc - group.endDtLoc) / 60;
	if (gap_crew_segpath <= 0) {
		return false;
	}
	////限制duty和crew之间的间隔，实际上就是限制crew不能休息太久
	//else if (gap_crew_segpath > _rules->maxDayOffMin) {
	//	return false;
	//}
	//不能担当该duty，因为会超时
	else if (0 < gap_crew_segpath && gap_crew_segpath < _rules->minDayOffMin) {
		//不需要day off，就是duty之间的正常接续
		//但预先判断一下若担当该duty，是否会超时
		if (group.max_credit_mint + gap_crew_segpath + segPath.total_credit_mint > _rules->maxCreditMin
			|| group.max_total_fly_mint + segPath.total_fly_mint > _rules->maxWeekFlyMin) {
			return false;
		}
	}

	//3.检查完空间和时间，检查资质		
	for (const auto& k_v : segPath.specialCredentials) {
		if (k_v.second != 0 && group.spetialCredentials[k_v.first] != 1) {
			return false;
		}
	}

	return true;
}

void SubProblem::addRestColumns() {
	for (size_t i = 0; i < _crewnode_set.size(); i++) {
		Column* rest_col = new Column();
		rest_col->type = Column::ColumnType::RELAX;
		CrewGroup* single_crew = new CrewGroup();
		single_crew->getNodeSequence().emplace_back(_crewnode_set[i]);
		single_crew->setCrewIndexSet();
		single_crew->setCrewGroup();
		single_crew->setBasicProperties();
		single_crew->computeCost();

		SegPath* segpath = new SegPath();
		//segpath->_total_cost = 0;	
		segpath->total_fly_mint = 0;
		segpath->total_credit_mint = 0;
		segpath->total_dhd = 0;
		

		rest_col->_segpath = segpath;
		rest_col->_crewgroup = single_crew;
		rest_col->cost += segpath->getCost() + /*calBalanceCost(segpath->total_fly_mint, *single_crew)*/
			calBalanceCostByVariance(segpath->total_fly_mint, *single_crew); //changed 20190822;

		_local_pool.emplace_back(rest_col);
		
	}

}

//! 8-15 之前的计算方式不对，改为 若该group和duty匹配，那么得到该group匹配该duty后的飞时，用这个飞时减去均值才对，而不是用当前group的飞时减去均值
int SubProblem::calBalanceCost(const int dutyFlyMin, CrewGroup& crewGroup) {
	int total_deviation = 0;
	
	std::vector<Opt_CREW*>& crew_set = crewGroup.getCrewGroup();	
	int deviation_duty_mean = crew_set.size() * (dutyFlyMin - _mean_fly_mint);
	
	for (size_t i = 0; i < crew_set.size(); i++) {
		//total_deviation += crew_set[i]->workStatus->accumuFlyMin;
		total_deviation += crew_set[i]->workStatus->wholePlanTotalFlyMint;
	}
	total_deviation = std::abs(total_deviation + deviation_duty_mean);

	return total_deviation;
}

int SubProblem::calBalanceCostByVariance(const int dutyFlyMin, CrewGroup& crewGroup) {
	int total_cost = 0;
	std::map<std::string, int> rank_to_sum_square;
	std::map<std::string, int> rank_to_num;
	for (const auto& crew : crewGroup.getCrewGroup()) {
		rank_to_sum_square[crew->getCurRank()] += std::pow(crew->workStatus->wholePlanTotalFlyMint + dutyFlyMin - _rank_to_mean_fly_mint[crew->getCurRank()], 2);
		++rank_to_num[crew->getCurRank()];
	}
	for (const auto& rank_sum : rank_to_sum_square) {
		total_cost += /*std::sqrt(*/rank_sum.second / rank_to_num.at(rank_sum.first);
	}

	return total_cost;
}
