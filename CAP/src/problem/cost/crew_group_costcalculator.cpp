#include "crew_group_costcalculator.h"

double CrewGroupCostCalculator::calCost(std::map<utils::CrewCostType, int> penaltyMap,
	std::map<utils::CrewCostType, int> amountMap) {

	double total_cost = 0;
	for (const auto& type_amount : amountMap) {
		total_cost += type_amount.second * penaltyMap.at(type_amount.first);
	}
	return total_cost;
}