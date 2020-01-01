#pragma once
#ifndef COST_INDICATORS_H
#define COST_INDICATORS_H

#include <vector>
#include <map>
#include <string>

namespace utils {
	
	enum EventPathCostType
	{
		NUM_EVENT = 0, //< Event是抽象后的基本单位，如duty，rest period
		NUM_DEADHEAD,
		NUM_OVERNIGHT,
		NUM_IDLE_CREW_COUNT, //< 闲置人次，path中segment的人员要求不同，造成部分人员闲置。
								//如seg1，2要求2人，seg3要求3人，则值为（3-2）+（3-2）= 2
		TIME_FLY,
		TIME_CREDIT,
		TIME_LONG_STOP
	};
	static std::vector<std::string> gAllEvenPathCostTypes = {
		"NUM_EVENT",
		"NUM_DEADHEAD",
		"NUM_OVERNIGHT",
		"NUM_IDLE_CREW_COUNT",		
		"TIME_FLY",
		"TIME_CREDIT",
		"TIME_LONG_STOP"
	};

	//enum SpecialCostIndicators
	//{
	//	RATIO_SKILL_INCOMPATIBILITY, //< 不兼容比例，如path中高原机场数量/普通机场数量
	//};

	// 基本是涉及均衡性的量
	enum CrewCostType {
		DEVIATION_FLY_TIME = 0,
		DEVIATION_CREDIT_TIME,
		DEVIATION_OVERNIGHT_NUM,
		DEVIATION_SPECIAL_ACTIVITY_NUM //<特殊活动，类似于crew飞过特殊航班（如经过高原机场）的数量，
	};
	static std::vector<std::string> gAllCrewCostTypes = {
		"DEVIATION_FLY_TIME",
		"DEVIATION_CREDIT_TIME",
		"DEVIATION_OVERNIGHT_NUM",
		"DEVIATION_SPECIAL_ACTIVITY_NUM"
	};
	
	class EventPathCostTypeAmount {
	public:
		EventPathCostTypeAmount() :_all_costtype_vec(&gAllEvenPathCostTypes) {
			for (int i = 0; i < _all_costtype_vec->size(); i++) {
				_eventpath_costtype_amount_map.insert(std::pair<EventPathCostType, int>((EventPathCostType)i, 0));
			}
		}
		std::map<EventPathCostType, int>& getAmountMap() { return _eventpath_costtype_amount_map; }

	private:
		const std::vector<std::string>* _all_costtype_vec;
		std::map<EventPathCostType, int> _eventpath_costtype_amount_map;
	};

	class CrewCostTypeAmount {
	public:
		CrewCostTypeAmount() :_all_costtype_vec(&gAllCrewCostTypes) {
			for (int i = 0; i < _all_costtype_vec->size(); i++) {
				_crew_costtype_amount_map.insert(std::pair<CrewCostType, int>((CrewCostType)i, 0));
			}
		}
		std::map<CrewCostType, int>& getAmountMap() { return	_crew_costtype_amount_map; }

	private:
		const std::vector<std::string>* _all_costtype_vec;
		std::map<CrewCostType, int> _crew_costtype_amount_map;
	};
	
	// end way 1

	//way 2
	struct EventPathCostIndicators
	{
		EventPathCostIndicators(int num_event,
			int num_dhd,
			int num_overnight,
			int num_idle_crew_count,
			int time_fly,
			int time_credit,
			int time_long_stop) {
		
			NUM_EVENT = num_event;
			NUM_DEADHEAD = num_dhd;
			NUM_OVERNIGHT = num_overnight;
			NUM_IDLE_CREW_COUNT = num_idle_crew_count;
			TIME_FLY = time_fly;
			TIME_CREDIT = time_credit;
			TIME_LONG_STOP = time_long_stop;
		}
		int NUM_EVENT = 1; //< Event是抽象后的基本单位，如duty，rest period
		int NUM_DEADHEAD;
		int NUM_OVERNIGHT;
		int NUM_IDLE_CREW_COUNT;//< 闲置人次，path中segment的人员要求不同，造成部分人员闲置。
								//如seg1，2要求2人，seg3要求3人，则值为（3-2）+（3-2）= 2
		int TIME_FLY;
		int TIME_CREDIT;
		int TIME_LONG_STOP;
	};	
	
	// 基本是涉及均衡性的量
	struct CrewCostIndicators {
		CrewCostIndicators(int deviation_fly_time,
			int deviation_credit_time,
			int deviation_overnight_num,
			int deviation_special_activity_num) {
			
			DEVIATION_FLY_TIME = deviation_fly_time;
			DEVIATION_CREDIT_TIME = deviation_credit_time;
			DEVIATION_OVERNIGHT_NUM = deviation_overnight_num;
			DEVIATION_SPECIAL_ACTIVITY_NUM = deviation_special_activity_num;
		}
		int DEVIATION_FLY_TIME;
		int DEVIATION_CREDIT_TIME;
		int DEVIATION_OVERNIGHT_NUM;
		int DEVIATION_SPECIAL_ACTIVITY_NUM; //<特殊活动，类似于crew飞过特殊航班（如经过高原机场）的数量，
	};
		
}

#endif // !COST_INDICATORS_H
