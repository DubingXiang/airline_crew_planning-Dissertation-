#pragma once
#ifndef ALGORITHM_H_DB
#define ALGORITHM_H_DB

#include "../problem/problem.h"
#include "../problem/solution/crew_scheduling_solution.h"
#include "column_generation/column_generation_module.h"
#include "column_generation/column_generation.h"

#include "solving_approach_module.h"

#include "../easylogging/easylogging++.h"

#include <vector>

class CrewSchedulingAlgorithm
{
public:
	CrewSchedulingAlgorithm(const Problem& problem);
	~CrewSchedulingAlgorithm();

	enum ApproachModuleName
	{
		COLUMN_GENERATION = 0
	};

	static el::Logger* logger;

	void setCurrApproachModule(ApproachModuleName moduleName);
	std::vector<CrewSchedulingSolution*> searchSolutions();

	

private:
	
	//std::vector<CrewSchedulingSolution*> _solutions;
	CrewSchedulingSolution _best_solution;
	const Problem& _problem;

	/*std::map<std::string, ISolvingApproachModule*> _nameToApproachModuleMap = 
	{
		{"ColumnGeneration", new ColumnGeneration() }
	};*/
	
	std::string _selected_solving_approach_module_name;
	ISolvingApproachModule* _selected_solving_approach_module;
};



#endif // !ALGORITHM_H_DB
