#pragma once
#ifndef PERIOD_H
#define PERIOD_H
#include "../structures/network/seg_network.h"
#include "../structures/network/crew_network.h"
#include "solution\crew_scheduling_solution.h"



class Period 
{
public:
	Period(Network::SegNetwork* segNet, Network::SegNodeSet* segNodeSetOfCurrPeriod, Network::CrewNodeSet* crewNodeSet);
	~Period();

	void addInitialSolutions(std::vector<CrewSchedulingSolution*>& solutions);


private:
	Network::SegNodeSet* _segNodeSet;
	Network::CrewNodeSet* _crewNodeSet;

	Network::SegNetwork* _segNet;

	std::vector<CrewSchedulingSolution*>* _init_slns;
};


#endif // !PERIOD_H
