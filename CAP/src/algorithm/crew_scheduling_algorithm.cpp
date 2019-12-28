#include "crew_scheduling_algorithm.h"



el::Logger* CrewSchedulingAlgorithm::logger = el::Loggers::getLogger("crewSchedulingAlgorithm");

CrewSchedulingAlgorithm::CrewSchedulingAlgorithm(const Problem& problem)
	:_problem(problem){
}

CrewSchedulingAlgorithm::~CrewSchedulingAlgorithm() {
}


void 
CrewSchedulingAlgorithm::setCurrApproachModule(CrewSchedulingAlgorithm::ApproachModuleName moduleName) {
	this->_selected_solving_approach_module_name = moduleName;
	switch (moduleName) {
	case COLUMN_GENERATION: 
		_selected_solving_approach_module = new CrewSchedulingColumnGenerationModule();
		break;
	//case 	LNS
	default:
		std::cerr << "error!:[wrong input appoach module name" << std::endl;
		break;
	}
}
std::vector<CrewSchedulingSolution*> 
CrewSchedulingAlgorithm::searchSolutions() {
			
	std::vector<CrewSchedulingSolution*> solutions =
		_selected_solving_approach_module->runAndGetSolution();
	
	
	return solutions;
}