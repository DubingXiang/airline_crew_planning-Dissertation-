#include "../pch.h"
#include "crew_rules.h"
#include "generic/csvClassesTransOptimizable.h"
#include "../../UtilFunc.h"


void CrewRules::setHorizonRules(int minTransMin,
	int maxTransMin,
	int minOutRestMin,
	int maxOutRestMin,
	int minFlyMin,
	int maxFlyMin,
	int minDutyMin,
	int maxDutyMin)
{
	horizon_rules->minTransMin = minTransMin;
	horizon_rules->maxTransMin = maxTransMin;
	horizon_rules->minOutRestMin = minOutRestMin;
	horizon_rules->maxOutRestMin = maxOutRestMin;
	horizon_rules->minFlyMin = minFlyMin;
	horizon_rules->maxFlyMin = maxFlyMin;
	horizon_rules->minDutyMin = minDutyMin;
	horizon_rules->maxDutyMin = maxDutyMin;		
}
void CrewRules::setWeekPara(int maxCreditMinutes, int maxWeekFlyMinutes, int minDayOffMinutes, int maxDayOffMinutes/*allowOverCreditMinutes*/) {
	maxCreditMin = maxCreditMinutes;
	maxWeekFlyMin = maxWeekFlyMinutes;
	minDayOffMin = minDayOffMinutes;
	//allowOverCreditMin = allowOverCreditMinutes;
	maxDayOffMin = maxDayOffMinutes;
}

void CrewRules::addRankCombination(const std::string& rankCombination) {
	rankCombinationSet.emplace_back(rankCombination);
}

void CrewRules::setSeqMaps() {
	std::vector<std::string> positionSet(CAP_positions);
	positionSet.resize(CAP_positions.size() + FO_positions.size());
	std::copy(FO_positions.begin(), FO_positions.end(), positionSet.begin() + CAP_positions.size());
	for (const auto& pos : positionSet) {
		PosOrderSeqvec* posv = new PosOrderSeqvec;
		posv->position = pos;
		posv->orderSeqIdvec.clear();

		_pos_order_seqs[pos] = posv;
	}
	int numOrder = 4;//号位的数量
	for (int order = 0; order < numOrder; order++) {
		OrderPosSeqvec* opsv = new OrderPosSeqvec;
		opsv->order = order;
		opsv->posSeqIdvec.clear();

		_order_pos_seqs[order] = opsv;
	}

	
	const char* pos;
	const char* seq;
	std::vector<std::string> combi;
	for (int i = 0; i < rankCombinationSet.size(); i++) {
		seq = rankCombinationSet[i].c_str();
		combi.clear();
		split(seq, '-', combi);
		for (int order = 0; order < combi.size(); order++) {
			pos = combi[order].c_str();
			_pos_order_seqs[pos]->orderSeqIdvec[order].emplace_back(i);

			_order_pos_seqs[order]->posSeqIdvec[pos].emplace_back(i);
		}
	}
}
void CrewRules::sortSeqMaps() {
	for (auto& k_v : _pos_order_seqs) {
		for (auto& order_idSet : k_v.second->orderSeqIdvec) {
			std::sort(order_idSet.second.begin(), order_idSet.second.end());
		}
	}

	for (auto& k_v : _order_pos_seqs) {
		for (auto& pos_idSet : k_v.second->posSeqIdvec) {
			std::sort(pos_idSet.second.begin(), pos_idSet.second.end());
		}
	}
}

bool CrewRules::isFasibleCombination(const std::vector<Opt_CREW*>& crewComb) const {
	if (crewComb.size() == 1) {
		return	true;
	}
		
	std::vector<std::string> combi;
	for (size_t i = 0; i < crewComb.size(); i++) {		
		combi.emplace_back(crewComb[i]->getCurPosition());
	}
	//std::sort(combi.begin(), combi.end());
	//不断求交集
	seqIdVec intersection;
	seqIdVec *p_set1 = &_pos_order_seqs.at(combi[0])->orderSeqIdvec[0];
	seqIdVec *p_set2;
	for (int i = 1; i < combi.size(); i++) {
		p_set2 = &_pos_order_seqs.at(combi[i])->orderSeqIdvec[i];
		std::set_intersection(p_set1->begin(), p_set1->end(), p_set2->begin(), p_set2->end(),
			std::insert_iterator<seqIdVec>(intersection, intersection.begin()));
		if (intersection.empty()) { 
			return false; 
		}
		p_set1 = &intersection;
	}

	return intersection.size() != 0;
}