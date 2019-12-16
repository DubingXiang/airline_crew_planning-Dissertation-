#include "Seg_Path.h"

#include "..\..\..\csv_impl.h"
#include "..\..\..\crewDB_mine.h"

#include "..\generic\csvClassesTransOptimizable.h"

#include "seg_network.h"
#include "crew_path.h"

//SegPath::SegPath(Penalty& penaltySetting) { 
//	_penalty_setting = &penaltySetting; 
//}
SegPath::SegPath() {	
}
SegPath::~SegPath() {
	startNode = NULL;
	endNode = NULL;

	_segNodeSequence.clear();
	//_crewGroup.clear();
}

double SegPath::getCost() const {
	return _total_cost;
}

void SegPath::init(/*const Penalty& penaltySetting*/) {	
	_nbSegNodes = _segNodeSequence.size(); 
	startNode = _segNodeSequence.front();
	csvComposition* duty_compo = startNode->optSegment->getCompositon();
		
	for (size_t i = 1; i < _segNodeSequence.size(); i++) {		
		duty_compo = duty_compo->hirarchy > _segNodeSequence[i]->optSegment->getCompositon()->hirarchy
			? duty_compo : _segNodeSequence[i]->optSegment->getCompositon();		

		// TODO:计算特殊资质要求数量
	}
	_path_composition = duty_compo;
	_path_compoMode = &compoModeMap[_path_composition->nameDesc];	

	endNode = _segNodeSequence.back();
	Label* end_label = endNode->label;
		
	// dhd	
	/*total_dhd = startNode->nodeType == NodeType::dhd ? 1 : 0;
	if (_segNodeSequence.size() > 1) {
		total_dhd = endNode->nodeType == NodeType::dhd ? total_dhd + 1 : total_dhd;
	}*/
	// CORRECTION-20190819
	int total_fly_min_dhd = 0;
	total_dhd = 0;
	for (const auto& segnode : _segNodeSequence) {
		if (segnode->nodeType == NodeType::dhd) {
			++total_dhd;
			total_fly_min_dhd += segnode->taskMin;
		}
	}
	
	//total_fly_mint = end_label->accumuFlyMin;
	total_fly_mint = end_label->accumuFlyMin - total_fly_min_dhd; // CORRECTION-20190819
	total_credit_mint = end_label->accumuWorkMin;

}
void SegPath::computeCost(/*std::vector<costTuple>& costTuples*/const Penalty& penaltySetting) {
	//一些cost如dhd_cost可在搜索路时就求出来了
	_penalty_setting = &penaltySetting;

	_total_cost = _penalty_setting->penalty_dhd * total_dhd;
			   //+ _penalty_setting->prnalty_longtime_stop * total_long_stop_mint;
}

void SegPath::setSegIndexSet(const std::vector<Opt_Segment*>& curDayOptSegSet) {
	auto begin = curDayOptSegSet.begin();
	auto end = curDayOptSegSet.end();
	for (const auto& segnode : _segNodeSequence) {
		if (segnode->nodeType == NodeType::dhd) {
			continue;
		}
		auto pos = std::find(begin, end, segnode->optSegment);
		int index = pos - begin;
		optseg_id_sequence.emplace_back(index);
	}
	

}
