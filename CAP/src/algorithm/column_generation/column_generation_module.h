#pragma once
#ifndef ABSTRACT_COLUMN_GENERATION_MODULE_H
#define ABSTRACT_COLUMN_GENERATION_MODULE_H
#include "../solving_approach_module.h"
#include "../../problem/problem.h"

class AbstractColumnGenerationModule :public ISolvingApproachModule
{
public:	
	std::vector<CrewSchedulingSolution*> runAndGetSolution() override;
	const std::string& getName() const override { return "ColumnGeneration"; }

protected:
	virtual void solveMasterProblem() = 0;
	virtual void solveSubProblem() = 0;
	virtual void updateDuals() = 0;
	virtual bool terminate() = 0;

	std::vector<CrewSchedulingSolution*> _solutions;

};

#endif // !ABSTRACT_COLUMN_GENERATION_MODULE_H
