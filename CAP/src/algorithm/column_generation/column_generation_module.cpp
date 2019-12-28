#include "column_generation_module.h"

std::vector<CrewSchedulingSolution*>
AbstractColumnGenerationModule::runAndGetSolution() {

	while (!terminate()) {
		solveMasterProblem();
		solveSubProblem();
		updateDuals();
	}

	return _solutions;
}