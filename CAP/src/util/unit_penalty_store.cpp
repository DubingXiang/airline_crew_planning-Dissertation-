#include "unit_penalty_setting.h"

namespace utils {

	el::Logger* ALLUnitPenaltyStore::logger = el::Loggers::getLogger("ALLUnitPenaltyStore");



#pragma region //way 2

	//el::Logger* UnitPenaltyStore::logger = el::Loggers::getLogger("UnitPenaltyStore");

	//void UnitPenaltyStore::setUnitPenaltyOfEventPathIndicators(
	//	const UnitPenaltyOfEventPathCostIndicators& unit_peanlty_eventpath_indicators) {
	//	_unit_penalty_eventpath = unit_peanlty_eventpath_indicators;
	//	setUnitPenaltyMapPartOfEventPath();
	//}
	//void UnitPenaltyStore::setUnitPenaltyOfCrewIndicators(
	//	const UnitPenaltyOfCrewCostIndicatiors& unit_penalty_crew_indicators) {
	//	_unit_penalty_crew = unit_penalty_crew_indicators;
	//	setUnitPenaltyMapPartOfCrew();
	//}
	//void UnitPenaltyStore::setUnitPenaltyMapPartOfEventPath() {
	//	std::map<std::string, int> penalty_name_value = getAllPenaltyNameOfEventPath();
	//	for (const auto& name_value : penalty_name_value) {
	//		std::string indicator_name = name_value.first.substr(kLen_Of_Unit_Penalty_);
	//		if (!isCostIndicatorNameInMapKeys(indicator_name)) {
	//			logger->error("penalty字段[{0}]的名称与对应costindicator的名称不匹配\n", name_value.first);
	//			displayKeysOfUnitPenaltyMap();
	//			return;
	//		}
	//		_indicator_to_unitpenalty_map.at(indicator_name) = name_value.second;
	//	}
	//}
	//void UnitPenaltyStore::setUnitPenaltyMapPartOfCrew() {
	//	std::map<std::string, int> penalty_name_value = getAllPenaltyNameOfCrew();
	//	for (const auto& name_value : penalty_name_value) {
	//		std::string indicator_name = name_value.first.substr(kLen_Of_Unit_Penalty_);
	//		if (!isCostIndicatorNameInMapKeys(indicator_name)) {
	//			logger->error("penalty字段[{0}]的名称与对应costindicator的名称不匹配\n", name_value.first);
	//			displayKeysOfUnitPenaltyMap();
	//			return;
	//		}
	//		_indicator_to_unitpenalty_map.at(indicator_name) = name_value.second;
	//	}
	//}
	//std::map<std::string, int> UnitPenaltyStore::getAllPenaltyNameOfEventPath() {
	//	std::map<std::string, int> penalty_name_value;
	//	penalty_name_value.insert(std::pair<std::string, int>
	//		(NAME_OF_VAR(_unit_penalty_eventpath.UNIT_PENALTY_NUM_DEADHEAD),
	//		_unit_penalty_eventpath.UNIT_PENALTY_NUM_DEADHEAD));
	//	penalty_name_value.insert(std::pair<std::string, int>
	//		(NAME_OF_VAR(_unit_penalty_eventpath.UNIT_PENALTY_NUM_EVENT), 
	//		_unit_penalty_eventpath.UNIT_PENALTY_NUM_EVENT));
	//	penalty_name_value.insert(std::pair<std::string, int>
	//		(NAME_OF_VAR(_unit_penalty_eventpath.UNIT_PENALTY_NUM_IDLE_CREW_COUNT),
	//		_unit_penalty_eventpath.UNIT_PENALTY_NUM_IDLE_CREW_COUNT));
	//	penalty_name_value.insert(std::pair<std::string, int>
	//		(NAME_OF_VAR(_unit_penalty_eventpath.UNIT_PENALTY_NUM_OVERNIGHT),
	//		_unit_penalty_eventpath.UNIT_PENALTY_NUM_OVERNIGHT));
	//	penalty_name_value.insert(std::pair<std::string, int>
	//		(NAME_OF_VAR(_unit_penalty_eventpath.UNIT_PENALTY_TIME_CREDIT),
	//		_unit_penalty_eventpath.UNIT_PENALTY_TIME_CREDIT));
	//	penalty_name_value.insert(std::pair<std::string, int>
	//		(NAME_OF_VAR(_unit_penalty_eventpath.UNIT_PENALTY_TIME_FLY),
	//		_unit_penalty_eventpath.UNIT_PENALTY_TIME_FLY));
	//	penalty_name_value.insert(std::pair<std::string, int>
	//		(NAME_OF_VAR(_unit_penalty_eventpath.UNIT_PENALTY_TIME_LONG_STOP),
	//		_unit_penalty_eventpath.UNIT_PENALTY_TIME_LONG_STOP));
	//	
	//	return penalty_name_value;
	//}
	//std::map<std::string, int> UnitPenaltyStore::getAllPenaltyNameOfCrew() {
	//	std::map<std::string, int> penalty_name_value;
	//	penalty_name_value.insert(std::pair<std::string, int>
	//		(NAME_OF_VAR(_unit_penalty_crew.UNIT_PENALTY_DEVIATION_CREDIT_TIME),
	//		_unit_penalty_crew.UNIT_PENALTY_DEVIATION_CREDIT_TIME));
	//	penalty_name_value.insert(std::pair<std::string, int>
	//		(NAME_OF_VAR(_unit_penalty_crew.UNIT_PENALTY_DEVIATION_FLY_TIME),
	//		_unit_penalty_crew.UNIT_PENALTY_DEVIATION_FLY_TIME));
	//	penalty_name_value.insert(std::pair<std::string, int>
	//		(NAME_OF_VAR(_unit_penalty_crew.UNIT_PENALTY_DEVIATION_OVERNIGHT_NUM),
	//		_unit_penalty_crew.UNIT_PENALTY_DEVIATION_OVERNIGHT_NUM));
	//	penalty_name_value.insert(std::pair<std::string, int>
	//		(NAME_OF_VAR(_unit_penalty_crew.UNIT_PENALTY_DEVIATION_SPECIAL_ACTIVITY_NUM),
	//		_unit_penalty_crew.UNIT_PENALTY_DEVIATION_SPECIAL_ACTIVITY_NUM));

	//	return penalty_name_value;
	//}

	//void UnitPenaltyStore::displayKeysOfUnitPenaltyMap() {		
	//	logger->debug("penaltyMap的所有字段为：\n");
	//	for (auto iter = _indicator_to_unitpenalty_map.begin(); 
	//		iter != _indicator_to_unitpenalty_map.end(); 
	//		iter++) {
	//		logger->debug(iter->first);
	//	}
	//}

	//bool UnitPenaltyStore::isCostIndicatorNameInMapKeys(const std::string& costIndicatorrName) {		
	//	if (_indicator_to_unitpenalty_map.find(costIndicatorrName) == _indicator_to_unitpenalty_map.end()) {
	//		return false;
	//	}
	//	return true;
	//}
	//// end way 2

#pragma endregion

} // end namespace utils