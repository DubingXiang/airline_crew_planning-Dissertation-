#pragma once
#ifndef COST_INDICATOR_UNIT_PENALTY_PAIR_H
#define COST_INDICATOR_UNIT_PENALTY_PAIR_H
#include "../../util/cost_indicators.h"
#include "cost_tuple.h"


class CostIndicatorUnitPenaltyPair : public CostTuple {
public:
	CostIndicatorUnitPenaltyPair(utils::CostIndicators name) {
		_name = name;
	}

private:
	utils::CostIndicators _name;
};

#endif // !COST_INDICATOR_UNIT_PENALTY_PAIR_H
