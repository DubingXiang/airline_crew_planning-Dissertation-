#pragma once
#ifndef PERIOD_H
#define PERIOD_H
#include "../algorithm/column_generation/column_generation.h"


class Period 
{
public:
	Period(SegNetwork* segNet, SegNodeSet* segNodeSetOfCurrPeriod, CrewNodeSet* crewNodeSet);
	~Period();

	void addInitialSolutions(std::vector<CrewSchedulingSolution*>& solutions);


private:
	SegNodeSet* _segNodeSet;
	CrewNodeSet* _crewNodeSet;

	SegNetwork* _segNet;

	std::vector<CrewSchedulingSolution*>* _init_slns;
};


#endif // !PERIOD_H
