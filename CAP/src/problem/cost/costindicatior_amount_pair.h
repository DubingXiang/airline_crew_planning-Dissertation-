#pragma once
#ifndef COST_INDICATOR_AMOUNT_PAIR_H
#define COST_INDICATOR_AMOUNT_PAIR_H
#include "../../util/cost_indicators.h"

/**
 * @brief pair<成本相关的指标，指标的量>
 * 涉及到成本的量，如deadhead次数，总值勤时间等
 **/
struct CostIndicatorAmountPair
{	
	utils::EventPathCostIndicators indicator;
	unsigned value;
};

#endif // !COST_INDICATOR_AMOUNT_PAIR_H
