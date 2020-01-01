#include "event_path_costcalculator.h"

double EventPathCostCalculator::calCost(std::map<utils::EventPathCostType, int> penaltyMap,
	std::map<utils::EventPathCostType, int> amountMap) {

	double total_cost = 0;
	for (const auto& type_amount : amountMap) {
		total_cost += type_amount.second * penaltyMap.at(type_amount.first);
	}
	return total_cost;
}
