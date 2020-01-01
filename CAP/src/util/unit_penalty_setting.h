#pragma once
#ifndef UNIT_PENALTY_SETTING_H
#define UNIT_PENALTY_SETTING_H
#include "../easylogging/easylogging++.h"

#include "cost_indicators.h"
#include <map>
#include <iostream>
#include <string>
#include <tuple>

namespace utils {
#define NAME_OF_VAR(variable) #variable


	//way 1
	class ALLUnitPenaltyStore {
	public:
		static ALLUnitPenaltyStore& getInstance() {
			static ALLUnitPenaltyStore instance;
			return instance;
		}
		static el::Logger* logger;

		void setEventPathCostTypePenaltyMap(const std::map<EventPathCostType, int>& eventPathCostTypePenalty) {
			_eventpath_costtype_to_penalty_map = eventPathCostTypePenalty;
		}
		void setCrewTypePenaltyMap(const std::map<CrewCostType, int>& crewCostTypePenalty) {
			_crew_costtype_to_penalty_map = crewCostTypePenalty;
		}

		const std::map<EventPathCostType, int>& getEventPathCostTypePenaltyMap() const { return _eventpath_costtype_to_penalty_map; }
		const std::map<CrewCostType, int>& getCrewCostTypePenaltyMap() const { return _crew_costtype_to_penalty_map; }

	private:
		ALLUnitPenaltyStore() {};
		std::map<EventPathCostType, int> _eventpath_costtype_to_penalty_map;
		std::map<CrewCostType, int> _crew_costtype_to_penalty_map;

	};
	// end way 1

#pragma region //way 2
	
	// 应该设为单例模式
	//class UnitPenaltyStore
	//{
	//public:		
	//	struct UnitPenaltyOfEventPathCostIndicators
	//	{
	//		friend UnitPenaltyStore;
	//	public:
	//		UnitPenaltyOfEventPathCostIndicators(int unit_penalty_num_event,
	//			int unit_penalty_num_dhd,
	//			int unit_penalty_num_overnight,
	//			int unit_penalty_num_idle_crew_count,
	//			int unit_penalty_time_fly,
	//			int unit_penalty_time_credit,
	//			int unit_penalty_time_long_stop) {

	//			UNIT_PENALTY_NUM_EVENT = unit_penalty_num_event;
	//			UNIT_PENALTY_NUM_DEADHEAD = unit_penalty_num_dhd;
	//			UNIT_PENALTY_NUM_OVERNIGHT = unit_penalty_num_overnight;
	//			UNIT_PENALTY_NUM_IDLE_CREW_COUNT = unit_penalty_num_idle_crew_count;
	//			UNIT_PENALTY_TIME_FLY = unit_penalty_time_fly;
	//			UNIT_PENALTY_TIME_CREDIT = unit_penalty_time_credit;
	//			UNIT_PENALTY_TIME_LONG_STOP = unit_penalty_time_long_stop;
	//		}
	//	private:
	//		int UNIT_PENALTY_NUM_EVENT; 
	//		int UNIT_PENALTY_NUM_DEADHEAD;
	//		int UNIT_PENALTY_NUM_OVERNIGHT;
	//		int UNIT_PENALTY_NUM_IDLE_CREW_COUNT;
	//		int UNIT_PENALTY_TIME_FLY;
	//		int UNIT_PENALTY_TIME_CREDIT;
	//		int UNIT_PENALTY_TIME_LONG_STOP;
	//	};
	//	struct UnitPenaltyOfCrewCostIndicatiors
	//	{
	//		friend UnitPenaltyStore;
	//	public:
	//		UnitPenaltyOfCrewCostIndicatiors(int unit_peanlty_deviation_fly_time,
	//			int unit_peanlty_deviation_credit_time,
	//			int unit_peanlty_deviation_overnight_num,
	//			int unit_peanlty_deviation_special_activity_num) {
	//		
	//			UNIT_PENALTY_DEVIATION_FLY_TIME = unit_peanlty_deviation_fly_time;
	//			UNIT_PENALTY_DEVIATION_CREDIT_TIME = unit_peanlty_deviation_credit_time;
	//			UNIT_PENALTY_DEVIATION_OVERNIGHT_NUM = unit_peanlty_deviation_overnight_num;
	//			UNIT_PENALTY_DEVIATION_SPECIAL_ACTIVITY_NUM = unit_peanlty_deviation_special_activity_num;
	//		}
	//	private:
	//		int UNIT_PENALTY_DEVIATION_FLY_TIME;
	//		int UNIT_PENALTY_DEVIATION_CREDIT_TIME;
	//		int UNIT_PENALTY_DEVIATION_OVERNIGHT_NUM;
	//		int UNIT_PENALTY_DEVIATION_SPECIAL_ACTIVITY_NUM;

	//	};

	//	//std::tuple<>

	//	static UnitPenaltyStore& getInstance() {
	//		static UnitPenaltyStore instance;
	//		return instance;
	//	}
	//	static el::Logger* logger;
	//	const int kLen_Of_Unit_Penalty_ = 13;

	//	void setUnitPenaltyOfEventPathIndicators(const UnitPenaltyOfEventPathCostIndicators& unit_peanlty_eventpath_indicators);
	//	void setUnitPenaltyOfCrewIndicators(const UnitPenaltyOfCrewCostIndicatiors& unit_penalty_crew_indicators);
	//	// 目前setUnitPenaltyMap()依赖于类UnitPenaltyOfCrewCostIndicatiors，，依赖于两个其他set函数		
	//	

	//	//// 所有path在计算pathcost的时候，调用该函数，乘以path的各项成本指标
	//	//const UnitPenaltyOfEventPathCostIndicators& getUnitPeanltyOfEventPathIndicators() const { return _unit_penalty_eventpath; }
	//	//// // 所有crew在计算crewcost的时候，调用该函数，乘以crew的各项成本指标
	//	//const UnitPenaltyOfCrewCostIndicatiors& getUnitPeanltyOfCrewIndicators() const { return _unit_penalty_crew; }

	//	const std::map<std::string, int> getUnitPenaltyMap() const { return _indicator_to_unitpenalty_map; }
	//	void displayKeysOfUnitPenaltyMap();

	//private:
	//	UnitPenaltyStore();
	//	std::map<std::string, int> getAllPenaltyNameOfEventPath();
	//	std::map<std::string, int> getAllPenaltyNameOfCrew();
	//	void setUnitPenaltyMapPartOfEventPath();
	//	void setUnitPenaltyMapPartOfCrew();
	//	bool isCostIndicatorNameInMapKeys(const std::string& penaltyName);
	//	
	//	UnitPenaltyOfEventPathCostIndicators _unit_penalty_eventpath;
	//	UnitPenaltyOfCrewCostIndicatiors _unit_penalty_crew;
	//	
	//	std::map<std::string, int> _indicator_to_unitpenalty_map = { //< 存储了costindicator对应的unitpenalty
	//		//NAME_OF_VAR(EventPathCostIndicators::NUM_DEADHEAD),
	//		{"NUM_DEADHEAD", 0},
	//		{"NUM_EVENT", 0},
	//		{"NUM_IDLE_CREW_COUNT", 0 },
	//		{"NUM_OVERNIGHT", 0},
	//		{"TIME_CREDIT", 0 },
	//		{"TIME_FLY", 0},
	//		{"TIME_LONG_STOP", 0},
	//		{"DEVIATION_CREDIT_TIME", 0 },
	//		{"DEVIATION_FLY_TIME", 0},
	//		{"DEVIATION_OVERNIGHT_NUM", 0 },
	//		{"DEVIATION_SPECIAL_ACTIVITY_NUM", 0 }
	//	};
	//	
	//};
#pragma endregion

} // end namespace utils

#endif // !UNIT_PENALTY_SETTING_H
