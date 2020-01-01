#include "Seg_Path.h"
#include "../../../csv_impl.h"
#include "../../../crewDB_mine.h"

#include "../../problem/cost/event_path_costcalculator.h"
//#include "../network/seg_network.h"
//#include "crew_path.h"

using namespace Network;

//SegPath::SegPath(Penalty& penaltySetting) { 
//	_penalty_setting = &penaltySetting; 
//}
EventPath::EventPath() {	
}
EventPath::~EventPath() {
	startNode = NULL;
	endNode = NULL;

	_segNodeSequence.clear();
	//_crewGroup.clear();
}

double EventPath::getCost() const {
	return _total_cost;
}

void EventPath::init(/*const Penalty& penaltySetting*/) {	
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
		if (segnode->nodeType == NodeType::DEADHEAD) {
			++total_dhd;
			total_fly_min_dhd += segnode->taskMin;
		}
	}
	
	//total_fly_mint = end_label->accumuFlyMin;
	total_fly_mint = end_label->accumuFlyMin - total_fly_min_dhd; // CORRECTION-20190819
	total_credit_mint = end_label->accumuWorkMin;

}
void EventPath::computeCost(const std::map<utils::EventPathCostType, int>& eventPathCostTypePenalty) {
	////一些cost如dhd_cost可在搜索路时就求出来了
	//_penalty_setting = &penaltySetting;
	//_total_cost = _penalty_setting->penalty_dhd * total_dhd;
	_total_cost = EventPathCostCalculator::calCost(eventPathCostTypePenalty, 
		_costtype_to_amount.getAmountMap());

}

void EventPath::setSegIndexSet(const std::vector<Opt_Segment*>& curDayOptSegSet) {
	auto begin = curDayOptSegSet.begin();
	auto end = curDayOptSegSet.end();
	for (const auto& segnode : _segNodeSequence) {
		if (segnode->nodeType == NodeType::DEADHEAD) {
			continue;
		}
		auto pos = std::find(begin, end, segnode->optSegment);
		int index = pos - begin;
		optseg_id_sequence.emplace_back(index);
	}	
}
