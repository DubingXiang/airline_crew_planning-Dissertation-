#pragma once
#ifndef SOLVING_APPROACH_MODULE_H
#define SOLVING_APPROACH_MODULE_H
#include "../problem/solution/crew_scheduling_solution.h"
#include <string>

class ISolvingApproachModule
{
public:
	virtual std::vector<CrewSchedulingSolution*> runAndGetSolution() = 0;
	virtual const std::string& getName() const = 0;

};

#endif // !SOLVING_APPROACH_MODULE_H
