#pragma once
#ifndef COST_PARAMETERS_H
#define COST_PARAMETERS_H
#include "../../util/cost_indicators.h"
#include "../../util/unit_penalty_setting.h"
#include <map>

//class Penalty
//{
//public:
//	Penalty(int penaltyFltTime,
//		int penaltyCreditTime,
//		int penaltyDhd,
//		int penaltyLongTimeStop,
//		int penaltySpecialCredential,
//		int penaltyBalanceFlyTime,
//		int penaltySegUncover) {
//		penalty_flt_time = penaltyFltTime;
//		penalty_credit_time = penaltyCreditTime;
//		penalty_dhd = penaltyDhd;
//		prnalty_longtime_stop = penaltyLongTimeStop;
//		penalty_special_credential = penaltySpecialCredential;
//		penalty_balance_flytime = penaltyBalanceFlyTime;
//		penalty_seg_uncover = penaltySegUncover;
//	}
//	int penalty_flt_time;
//	int penalty_credit_time;
//	int penalty_dhd;
//	int prnalty_longtime_stop;
//	int penalty_special_credential;
//	
//	int penalty_balance_flytime;
//	int penalty_seg_uncover;
//};


class CostTuple 
{
public:
	CostTuple() {
		_amount = 0;
		_unit_penalty = 0;
		_cost = 0;
	}
	CostTuple(double amount, double unitPenalty) {
		_amount = amount;
		_unit_penalty = unitPenalty;		
	}

	double getAmount() const { return _amount; }
	double getUnitPenalty() const { return _unit_penalty; }
	
	void setAmount(double amount) { _amount = amount; }
	void setUnitPenalty(double unitPenalty) { _unit_penalty = unitPenalty; }

	void calculateCost() { _cost = _amount * _unit_penalty; }
	double getCost() const { return _cost; }

protected:	
	double _amount;
	double _unit_penalty;
	double _cost;
};



#endif // !COST_PARAMETERS_H