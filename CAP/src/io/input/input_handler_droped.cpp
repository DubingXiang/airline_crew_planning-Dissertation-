#include "input_handle.h"

#include "../../../csv_impl.h";
#include "../../../Segment_mine.h"
#include "../../../crewDB_mine.h"
#include "../../../UtilFunc.h"


#include "../../structures/generic/csvClassesTransOptimizable.h"
#include "../../structures/crew_rules.h"



void InputHandler::transformInputObjSet(std::map<string, std::vector<void*>>& dataSet, const std::vector<string>& objNameSet) {
	for (const auto& obj_name : objNameSet) {
		if (obj_name == "Flight") { typeTrans(dataSet[obj_name], obj_name, &_segSet); }
		else if (obj_name == "Base") { typeTrans(dataSet[obj_name], obj_name, &_baseSet); }
		else if (obj_name == "Crew") { typeTrans(dataSet[obj_name], obj_name, &_crewSet); }
		else if (obj_name == "CrewRank") { typeTrans(dataSet[obj_name], obj_name, &_crew_rankSet); }
		else if (obj_name == "CrewBase") { typeTrans(dataSet[obj_name], obj_name, &_crew_baseSet); }
		else if (obj_name == "FlightComposition") { typeTrans(dataSet[obj_name], obj_name, &_fltCompositionSet); }
		else if (obj_name == "Composition") { typeTrans(dataSet[obj_name], obj_name, &_compositionSet); }
		else { std::cerr << "ERROR: invalid obj data name" << obj_name << std::endl; }
	}
}
void InputHandler::typeTrans(std::vector<void*>& objSet, const std::string& objName, void* outObjSet) {		
	if (objName == "Flight") {		
		std::vector<Segment*>* ref = (std::vector<Segment*>*)outObjSet;
		ref->resize(objSet.size());
		for (int i = 0; i < objSet.size(); i++) {
			(*ref)[i] = (Segment*)objSet[i];
		}
	}
	else if (objName == "Base") {
		std::vector<BASE*>* ref = (std::vector<BASE*>*)outObjSet;
		ref->resize(objSet.size());
		for (int i = 0; i < objSet.size(); i++) {
			(*ref)[i] = (BASE*)objSet[i];
		}
	}
	else if (objName == "Crew") {
		std::vector<CREW*>* ref = (std::vector<CREW*>*)outObjSet;
		ref->resize(objSet.size());
		for (int i = 0; i < objSet.size(); i++) {
			(*ref)[i] = (CREW*)objSet[i];
		}
	}
	else if (objName == "CrewRank") {
		std::vector<CREW_RANK*>* ref = (std::vector<CREW_RANK*>*)outObjSet;
		ref->resize(objSet.size());
		for (int i = 0; i < objSet.size(); i++) {
			(*ref)[i] = (CREW_RANK*)objSet[i];
		}
	}
	else if (objName == "CrewBase") {
		std::vector<CREW_BASE*>* ref = (std::vector<CREW_BASE*>*)outObjSet;
		ref->resize(objSet.size());
		for (int i = 0; i < objSet.size(); i++) {
			(*ref)[i] = (CREW_BASE*)objSet[i];
		}
	}
	else if (objName == "FlightComposition") {
		std::vector<csvActivityComposition*>* ref = (std::vector<csvActivityComposition*>*)outObjSet;
		ref->resize(objSet.size());
		for (int i = 0; i < objSet.size(); i++) {
			(*ref)[i] = (csvActivityComposition*)objSet[i];
		}
	}
	else if (objName == "Composition") {
		std::vector<csvComposition*>* ref = (std::vector<csvComposition*>*)outObjSet;
		ref->resize(objSet.size());
		for (int i = 0; i < objSet.size(); i++) {
			(*ref)[i] = (csvComposition*)objSet[i];
		}
	}
	else { std::cout << "ERROR: invalid objName " << objName << "\n"; }
}

void InputHandler::loadAllSkills() {
	_skillSet.insert("PlateauAirport");
}

void InputHandler::createOptSegments(/*std::vector<Opt_Segment*>* optSegSet*/) {
	for (const auto& seg : _segSet) {
		Opt_Segment* opt_seg = new Opt_Segment(seg);
		_optSegSet->emplace_back(opt_seg); 
	}
}
void InputHandler::createOptCrews(/*std::vector<Opt_CREW*>* optCrewSet*/) {
	for (const auto& crew : _crewSet) {
		Opt_CREW* opt_crew = new Opt_CREW(crew);
		_optCrewSet->emplace_back(opt_crew);
	}
}

void InputHandler::matchOptSegmentAndComposition(/*std::vector<Opt_Segment*>* optSegSet*/
	/*std::vector<csvActivityComposition*>* fltCompoSet,
	std::vector<csvComposition*>* compoSet*/) {
	for (auto& seg : *_optSegSet) {
		for (const auto& flt_compo : _fltCompositionSet) {
			if (flt_compo->activityId == seg->getDBId()) {				
				for (const auto& compo : _compositionSet) {
					if (compo->compId == flt_compo->compId) {
						seg->setCompositions(flt_compo, compo);
						break;
					}
				}
				break;
			}
		}
	}
}
void InputHandler::setRankToNumMapOfOptSegment(/*std::vector<Opt_Segment*>* optSegSet*/) {
	for (const auto& seg : *_optSegSet) {
		seg->setRankToNumMap();
	}
}


void InputHandler::matchOptCrewAndRank(std::vector<Opt_CREW*>* optCrewSet/*, std::vector<CREW_RANK*>* crewrankSet*/) {
	std::map<string, CrewRankAry*> id_rank;

	for (const auto& crewrank : _crew_rankSet) {
		if (id_rank.find(crewrank->idCrew) == id_rank.end()) {
			id_rank[crewrank->idCrew] = new CrewRankAry();
		}
		id_rank[crewrank->idCrew]->emplace_back(crewrank);
	}

	for (auto& crew : *optCrewSet) {
		crew->setCrewRankAry(id_rank[crew->getIdCrew()]);
	}
}
void InputHandler::matchOptCrewAndBase(std::vector<Opt_CREW*>* optCrewSet/*, std::vector<CREW_BASE*>* crewbaseSet*/) {
	std::map<string, CrewBaseAry*> id_base;
	for (auto& base : _crew_baseSet) {
		if (id_base.find(base->idCrew) == id_base.end()) {
			id_base[base->idCrew] = new CrewBaseAry();
		}
		id_base[base->idCrew]->emplace_back(base);
	}
	for (auto& crew : *optCrewSet) {
		crew->setCrewBaseAry(id_base[crew->getIdCrew()]);
	}
}
void InputHandler::initializeCrewSkills(std::vector<Opt_CREW*>* optCrewSet) {
	for (auto& optcrew : *optCrewSet) {
		_ASSERT(optcrew->getSkillMap().empty()); // TODO:test function "_ASSERT"
		optcrew->getSkillMap().clear();
	}
}

void InputHandler::setIndexOfCrew(std::vector<Opt_CREW*>* optCrewSet) {	
	for (size_t i = 0; i < optCrewSet->size(); i++) {
		(*optCrewSet)[i]->setIndex(i);
	}
}

void InputHandler::sortCrewRank(/*std::vector<CREW_RANK*>* crewrankAry*/) {	
	std::sort(_crew_rankSet.begin(), _crew_rankSet.end(),
		[](const CREW_RANK *a, const CREW_RANK *b) {return a->effUtc > b->effUtc; });

}
void InputHandler::setAirportSet(/*const std::vector<Segment*>& fltSet*/) {
	for (const auto& flt : _segSet) {
		if (std::find(_airportSet.begin(), _airportSet.end(), flt->getDepStation()) == _airportSet.end()) {
			_airportSet.emplace_back(flt->getDepStation());
		}
		if (std::find(_airportSet.begin(), _airportSet.end(), flt->getArrStation()) == _airportSet.end()) {
			_airportSet.emplace_back(flt->getArrStation());
		}
	}
}



#pragma region /**************random create examples**************/

//std::vector<Opt_CREW*>* InputHandler::getPilotSet(const std::vector<Opt_CREW*>& optCrewSet) {
//	std::vector<Opt_CREW*>* pilotSet = new std::vector<Opt_CREW*>();
//
//	for (const auto& crew : optCrewSet) {
//		if (crew->getDivision() == "P") {
//			/*for (auto iter = crew->rankAry->begin(); iter != crew->rankAry->end();) {
//				if ((*iter)->rank != "CAP" && (*iter)->rank != "FO") {
//					iter = crew->rankAry->erase(iter);
//				}
//				else {
//					iter++;
//				}
//			}*/
//			if (crew->getCurRank() != "CAP" && crew->getCurRank() != "FO") {
//				continue;
//			}
//
//			pilotSet->emplace_back(crew);
//		}
//	}
//
//	return pilotSet;
//}
//
//std::vector<string>& InputHandler::createSpecialArpSet() {
//	//std::vector<string>* specialArpSet = new std::vector<string>();
//	//srand(0);	
//	int index = 0;
//	size_t end = _airportSet.size();
//	for (int i = 0, num = 5; i < num; i++) {
//		index = rand() % end + i;
//		_special_airportSet.emplace_back(_airportSet[index]);
//	}
//	return _special_airportSet;
//}
//
//void InputHandler::randomSetCrewSkills(std::vector<Opt_CREW*>* optCrewSet, double percent) {
//	string skill("PlateauAirport");
//	int num_special_crew = 0;
//	size_t size = _crewSet.size();
//	std::cout << "-----------current skills is plateau airport-----------\n";
//	for (auto& crew : *optCrewSet) {		
//		//crew->SkillSet.clear();
//		if (rand() % 2 == 1) {
//			crew->getSkillMap()[skill] = 1;
//			++num_special_crew;
//
//			std::cout << "crew <" << crew->getIdCrew() << "> able to fly: " << skill << "\n";
//		}
//
//		if (num_special_crew / size >= percent) {
//			break;
//		}
//	}
//}
//
//void InputHandler::setRankCombination(CrewRules* rules) {
//	std::cout << "----------Rank Combinations----------\n";
//	std::stringstream combi;
//	#pragma region //搭配组合的全排列		
//	std::vector<std::string> CAP_permus2;
//	std::vector<std::string> FO_permus2;
//	/*1 1CAP-{1FO/2FO}*/
//	//1.1 1CAP1FO
//	auto set1 = &CAP_positions;
//	auto set2 = &FO_positions;
//	int num = 0;
//	while (num < 2) {
//		for (const auto& pos1 : *set1) {
//			for (const auto& pos2 : *set2) {
//				if ((pos1.find("K1") != std::string::npos && pos2.find("F6") != std::string::npos)
//					|| (pos1.find("F6") != std::string::npos && pos2.find("K1") != std::string::npos)) {
//					continue;
//				}
//
//				combi.str("");
//				combi << pos1 << "-";
//				combi << pos2;
//				
//				rules->addRankCombination(combi.str());
//			}
//		}
//		std::swap(set1, set2);
//		num++;
//	}
//	//1.2 1CAP2FO
//	//得到2FO的所有排列
//	for (int i = 0; i < FO_positions.size(); i++) {
//		for (int j = 0; j < FO_positions.size(); j++) {
//			combi.str("");
//			combi << FO_positions[i] << "-";
//			combi << FO_positions[j];
//			FO_permus2.emplace_back(combi.str());
//		}
//	}
//	//1CAP与2FO排列
//	set1 = &CAP_positions;
//	set2 = &FO_permus2;
//	num = 0;
//	while (num < 2) {
//		for (const auto& pos1 : *set1) {
//			for (const auto& pos2 : *set2) {
//				combi.str("");
//				combi << pos1 << "-";
//				combi << pos2;
//				rules->addRankCombination(combi.str());
//			}
//		}
//		std::swap(set1, set2);
//		num++;
//	}
//
//	/*2. 2CAP-{1FO/2FO}*/
//	for (int i = 0; i < CAP_positions.size(); i++) {
//		for (int j = 0; j < CAP_positions.size(); j++) {
//			if ((CAP_positions[i] == "C4" && CAP_positions[j] == "T3")
//				|| (CAP_positions[i] == "T3" && CAP_positions[j] == "C4")) {
//				continue;
//			}
//
//			combi.str("");
//			combi << CAP_positions[i] << "-";
//			combi << CAP_positions[j];
//			CAP_permus2.emplace_back(combi.str());
//		}
//	}
//	//2CAP1FO
//	set1 = &CAP_permus2;
//	set2 = &FO_positions;
//	num = 0;
//	while (num < 2) {
//		for (const auto& pos1 : *set1) {
//			for (const auto& pos2 : *set2) {
//				if ((pos1.find("C4") != std::string::npos && pos2.find("F6") != std::string::npos)
//					|| (pos1.find("F6") != std::string::npos && pos2.find("C4") != std::string::npos)) {
//					continue;
//				}
//
//				combi.str("");
//				combi << pos1 << "-";
//				combi << pos2;
//				rules->addRankCombination(combi.str());
//			}
//		}
//		std::swap(set1, set2);
//		num++;
//	}
//	//2FO...
//
//	/*3. 3CAP1FO*/
//	std::vector<std::string> CAP_permus3;
//	set1 = &CAP_permus2;
//	set2 = &CAP_positions;
//	num = 0;
//	while (num < 2) {
//		for (const auto& pos1 : *set1) {
//			for (const auto& pos2 : *set2) {
//				combi.str("");
//				combi << pos1 << "-";
//				combi << pos2;
//				CAP_permus3.emplace_back(combi.str());
//			}
//		}
//		std::swap(set1, set2);
//		num++;
//	}
//	set1 = &CAP_permus3;
//	set2 = &FO_positions;
//	num = 0;
//	while (num < 2) {
//		for (const auto& pos1 : *set1) {
//			for (const auto& pos2 : *set2) {
//				combi.str("");
//				combi << pos1 << "-";
//				combi << pos2;
//				rules->addRankCombination(combi.str());
//			}
//		}
//		std::swap(set1, set2);
//		num++;
//	}
//	#pragma endregion
//
//}
//
//void InputHandler::setRankCombination_OnePermutation(CrewRules* rules) {
//	std::cout << "----------Rank Combinations----------\n";
//	std::stringstream combi;
//	std::string compo_name;
//#pragma region //搭配组合的一个排列（按字典序）	
//	std::vector<std::string> CAP_permus2;
//	std::vector<std::string> FO_permus2;
//	/*1 1CAP-{1FO/2FO}*/
//	//1.1 1CAP1FO
//	compo_name = "1CAP1FO";
//	auto set1 = &CAP_positions;
//	auto set2 = &FO_positions;	
//	for (const auto& pos1 : *set1) {
//		for (const auto& pos2 : *set2) {
//			if (pos1.find("K1") != std::string::npos && pos2.find("F6") != std::string::npos) {
//				continue;
//			}
//
//			combi.str("");
//			combi << pos1 << "-";
//			combi << pos2;
//
//			//rules->addRankCombination(combi.str());
//			rules->compo_sequences_map[compo_name].emplace_back(combi.str());
//		}
//	}
//		
//	//1.2 1CAP2FO
//	//得到2FO的所有组合，每个组合用一个排列表示即可
//	for (int i = 0; i < FO_positions.size(); i++) {
//		for (int j = i; j < FO_positions.size(); j++) {
//			combi.str("");
//			combi << FO_positions[i] << "-";
//			combi << FO_positions[j];
//			FO_permus2.emplace_back(combi.str());
//		}
//	}
//	//1CAP与2FO排列
//	compo_name = "1CAP2FO";
//	set1 = &CAP_positions;
//	set2 = &FO_permus2;	
//	for (const auto& pos1 : *set1) {
//		for (const auto& pos2 : *set2) {
//			combi.str("");
//			combi << pos1 << "-";
//			combi << pos2;
//			//rules->addRankCombination(combi.str());
//			rules->compo_sequences_map[compo_name].emplace_back(combi.str());
//		}
//	}
//
//
//	/*2. 2CAP-{1FO/2FO}*/
//	for (int i = 0; i < CAP_positions.size(); i++) {
//		for (int j = i; j < CAP_positions.size(); j++) {
//			if (CAP_positions[i] == "C4" && CAP_positions[j] == "T3") {
//				continue;
//			}
//
//			combi.str("");
//			combi << CAP_positions[i] << "-";
//			combi << CAP_positions[j];
//			CAP_permus2.emplace_back(combi.str());
//		}
//	}
//	//2CAP1FO
//	compo_name = "2CAP1FO";
//	set1 = &CAP_permus2;
//	set2 = &FO_positions;	
//	for (const auto& pos1 : *set1) {
//		for (const auto& pos2 : *set2) {
//			if (pos1.find("C4") != std::string::npos && pos2.find("F6") != std::string::npos) {
//				continue;
//			}
//
//			combi.str("");
//			combi << pos1 << "-";
//			combi << pos2;
//			//rules->addRankCombination(combi.str());
//			rules->compo_sequences_map[compo_name].emplace_back(combi.str());
//		}
//	}
//	//2FO...
//
//	/*3. 3CAP1FO*/
//	std::vector<std::string> CAP_permus3;
//	set1 = &CAP_permus2;
//	set2 = &CAP_positions;		
//	for (const auto& pos1 : *set1) {
//		for (const auto& pos2 : *set2) {
//			combi.str("");
//			combi << pos1 << "-";
//			combi << pos2;
//			CAP_permus3.emplace_back(combi.str());
//		}
//	}
//	compo_name = "3CAP1FO";
//	set1 = &CAP_permus3;
//	set2 = &FO_positions;
//	for (const auto& pos1 : *set1) {
//		for (const auto& pos2 : *set2) {
//			combi.str("");
//			combi << pos1 << "-";
//			combi << pos2;
//			//rules->addRankCombination(combi.str());
//			rules->compo_sequences_map[compo_name].emplace_back(combi.str());
//		}
//	}
//
//#pragma endregion
//
//}

#pragma endregion
