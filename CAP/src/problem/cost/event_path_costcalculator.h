#pragma once
#ifndef COST_EVENTPATH_H
#define COST_EVENTPATH_H
#include "../../util/cost_indicators.h"

// 只允许静态使用
class EventPathCostCalculator
{
public:
	static double calCost(std::map<utils::EventPathCostType, int> penaltyMap, 
		std::map<utils::EventPathCostType, int> amountMap);
private:
	EventPathCostCalculator(); //只允许静态使用
};



//class EventPathCostCalculator
//{
//public:
//	EventPathCostCalculator() {}
//	void calTotalCost() {
//		total_cost = 0;
//		total_cost = /*fly_time_costtuple.cost +*/ dhd_costtuple.cost + special_credential_costtuple.cost;
//	}
//	double getTotalCost() const {
//		return total_cost;
//	}
//	double total_cost;
//	//double fly_time_cost;
//	//double credit_time_cost;
//	//double dhd_cost;
//	//double longtime_stop_cost;
//	////特殊要求（特殊机场，外籍机场...）
//	////为了使这些航班集中在一条路上，该cost可以设为：固定cost(即惩罚) * （该path包含的总的航班数/特殊要求（如特殊航班）数量）(百分数表示)。当然，分母不为0
//	//double special_credential_cost;
//	//costTuple fly_time_costtuple;
//	CostTuple dhd_costtuple;
//	CostTuple special_credential_costtuple;
//};

#endif // !COST_EVENTPATH_H
