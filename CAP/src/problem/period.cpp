#include "period.h"
#include "../algorithm/column_generation/column_generation.h"

using namespace Network;

Period::Period(SegNetwork* segNet, SegNodeSet* segNodeSetOfCurrPeriod, CrewNodeSet* crewNodeSet) {
	_segNet = segNet;
	_segNodeSet = segNodeSetOfCurrPeriod;
	_crewNodeSet = crewNodeSet;
}
Period::~Period() {
	_segNet = nullptr;
	_segNodeSet = nullptr;
	_crewNodeSet = nullptr;
}

void Period::addInitialSolutions(std::vector<CrewSchedulingSolution*>& solutions) {
	_init_slns = &solutions;
}