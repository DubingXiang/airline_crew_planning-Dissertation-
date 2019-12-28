#pragma once
#ifndef CREW_RULES_H
#define CREW_RULES_H

#include "defines.h"
#include <unordered_set>

using namespace defines;

class Opt_CREW;

static std::vector<std::string> CAP_positions{ "C1","C2", "C4", "T1", "T2", "T3", "K1", "K2", "K3" };
static std::vector<std::string> FO_positions{ "F1", "F2", "F3", "F4", "F5", "F6", "J1", "J2" };


/**
 * @class PosOrderSeqvec
 * @brief 包含map，结构为 position-号位-option的index
 */
class PosOrderSeqvec
{
public:
	~PosOrderSeqvec() {
		for (auto& k_v : orderSeqIdvec) {
			k_v.second.clear();
			k_v.second.shrink_to_fit();
		}
		orderSeqIdvec.clear();
	}
	std::string position;
	std::map<int, seqIdVec> orderSeqIdvec;
};
/**
 * @class OrderPosSeqvec
 * @brief 包含map，结构为 号位-position-option的index
 */
class OrderPosSeqvec
{
public:
	~OrderPosSeqvec() {
		for (auto& k_v : posSeqIdvec) {
			k_v.second.clear();
			k_v.second.shrink_to_fit();
		}
		posSeqIdvec.clear();
	}
	int order;
	std::map<std::string, seqIdVec> posSeqIdvec;
};

/**
 * 组成单条duty时的规则
 */
typedef struct HorizontalRules
{
	int minTransMin;
	int maxTransMin;
	int minOutRestMin;
	int maxOutRestMin;
	int minFlyMin;
	int maxFlyMin;
	int minDutyMin;
	int maxDutyMin;

}HORIZON_RULES;

/**
 * @class CrewRules
 * @brief 乘务规则类
 * 
 * 存储规则，包括：1）组单条duty时的规则 2）"7天"规则 3）级别搭配
 * 判断级别搭配规则
 */
class CrewRules
{
public:
	CrewRules() {
		horizon_rules = new HORIZON_RULES();
	}
	void setHorizonRules(int minTransMin,
		int maxTransMin,
		int minOutRestMin,
		int maxOutRestMin,
		int minFlyMin,
		int maxFlyMin,
		int minDutyMin,
		int maxDutyMin);
	void setWeekPara(int maxCreditMinutes, int maxWeekFlyMinutes, int minDayOffMinutes, int maxDayOffMinutes/*allowOverCreditMinutes*/);
	/**
	 * @brief 添加合法搭配组合至搭配组合map
	 * @param 单个rankCombination，即option
	 */	
	void addRankCombination(const std::string& rankCombination);
	/**
	 * @brief 赋值存储option的map	 
	 */
	void setSeqMaps();
	/**
	 * @brief 按字典序升序排序	
	 */
	void sortSeqMaps();

	/**
	 * @brief 判断option是否合法
	 * @param 多个crew组成的option
	 */
	bool isFasibleCombination(const std::vector<Opt_CREW*>& crewComb) const;

	/// 组成单条duty须遵守的规则
	HORIZON_RULES* horizon_rules;
	/// 最大执勤时长，超过后必须day off
	int maxCreditMin;
	/// 最大周飞行时间，超过后必须day off
	int maxWeekFlyMin;	
	/// day off至少休息时间
	int minDayOffMin;
	/// day off至多休息时间
	int maxDayOffMin;
	/// 所有option的集合
	std::vector<std::string> rankCombinationSet;
	/// 按position-order-optionSet结构存储的map
	std::map<std::string, PosOrderSeqvec*> _pos_order_seqs;
	/// 按order-position-optionSet结构存储的map
	std::map<int, OrderPosSeqvec*> _order_pos_seqs;
	/// key <配比名称>; value <该配比下所有option>
	/// example: key "1CAP1FO"
	std::map<std::string, std::vector<std::string>> compo_sequences_map;
};

#endif // !CREW_RULES_H
