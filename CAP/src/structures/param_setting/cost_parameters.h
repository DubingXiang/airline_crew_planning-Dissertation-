#pragma once
#ifndef COST_PARAMETERS_H
#define COST_PARAMETERS_H

struct Penalty
{
	Penalty(int penaltyFltTime,
		int penaltyCreditTime,
		int penaltyDhd,
		int penaltyLongTimeStop,
		int penaltySpecialCredential,
		int penaltyBalanceFlyTime,
		int penaltySegUncover) {
		penalty_flt_time = penaltyFltTime;
		penalty_credit_time = penaltyCreditTime;
		penalty_dhd = penaltyDhd;
		prnalty_longtime_stop = penaltyLongTimeStop;
		penalty_special_credential = penaltySpecialCredential;
		penalty_balance_flytime = penaltyBalanceFlyTime;
		penalty_seg_uncover = penaltySegUncover;
	}
	int penalty_flt_time;
	int penalty_credit_time;
	int penalty_dhd;
	int prnalty_longtime_stop;
	int penalty_special_credential;
	
	int penalty_balance_flytime;
	int penalty_seg_uncover;
};

struct costTuple 
{
	costTuple() {
		unit = 0;
		penalty = 0;
		cost = 0;
	}
	costTuple(int unit, int penalty) {
		this->unit = unit;
		this->penalty = penalty;
		this->cost = unit * penalty;
	}
	int unit;
	int penalty;
	double cost;
};

class SegPathCost
{
public:
	SegPathCost() {}
	void calTotalCost() {
		total_cost = 0;		
		total_cost = /*fly_time_costtuple.cost +*/ dhd_costtuple.cost + special_credential_costtuple.cost;
	}
	double getTotalCost() const {
		return total_cost;
	}
	double total_cost;
	//double fly_time_cost;
	//double credit_time_cost;
	//double dhd_cost;
	//double longtime_stop_cost;
	////特殊要求（特殊机场，外籍机场...）
	////为了使这些航班集中在一条路上，该cost可以设为：固定cost(即惩罚) * （该path包含的总的航班数/特殊要求（如特殊航班）数量）(百分数表示)。当然，分母不为0
	//double special_credential_cost;
	
	//costTuple fly_time_costtuple;
	costTuple dhd_costtuple;
	costTuple special_credential_costtuple;

};

#endif // !COST_PARAMETERS_H