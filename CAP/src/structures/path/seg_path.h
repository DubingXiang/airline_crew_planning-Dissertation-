#pragma once
#ifndef SEG_PATH_H
#define SEG_PATH_H
#include "../../pch.h"
#include "../../util/unit_penalty_setting.h"
#include "../generic/csvClassesTransOptimizable.h"
#include "../network/seg_node_arc.h"
#include "IPath.h"


const int INIT_PRICE = 1000 * 1000;// 目的是使初始的reduced cost为负， 需要和penalty的取值协调
struct csvComposition;
// 配比
struct compoMode
{
	std::string name;
	int totalNum;
	std::map<std::string, int> rankNum;
};
static std::map<std::string, compoMode> compoModeMap = { {"1CAP1FO", {"1CAP1FO", 2, {{"CAP",1}, {"FO",1}} } },
	{"1CAP2FO", {"1CAP2FO", 3, {{"CAP",1}, {"FO",2}}} },
	{"2CAP1FO", {"2CAP1FO", 3, {{"CAP",2}, {"FO",1}}} },
	{"3CAP1FO", {"3CAP1FO", 4, {{"CAP",3}, {"FO",1}} } } };


class EventPath :public IPath
{
public:			
	EventPath();
	virtual ~EventPath();		
	//! simple getter
	virtual double getCost() const;

	void init(/*const Penalty& penaltySetting*/);
	//! 根据路径内容计算各项cost
	//! 最后求和
	//! 1.fly_time
	//! 2.dhd
	//! 3.special_credential
	//void computeCost(const Penalty& penaltySetting);
	void computeCost(const std::map <utils::EventPathCostType, int > & eventPathCostTypePenalty);
	std::vector<Network::SegNode*>& getNodeSequence() { return _segNodeSequence; }
	

	//SegPathCost& getSegPathCost() { return _segPathCost; }

	/*8-5-2019*/
	//! parameter <optSegSet> 中的元素顺序在单天不会改变
	//！找到该条SegPath包含的Opt_Segment在<optSegSet>中的下标
	//! 若node是dhd，则下标设为-1，表示不存在
	void setSegIndexSet(const std::vector<Opt_Segment*>& curDayoptSegSet);
	
	const compoMode* getCompoMode() const { return _path_compoMode; }

	//for Cplex model
	std::vector<int> optseg_id_sequence;

	/*8-6-2019*/
	std::map<std::string, int> specialCredentials;
	Network::SegNode* startNode;
	Network::SegNode* endNode;

	int total_fly_mint;
	int total_credit_mint; //TODO：记得赋值
	int total_dhd;
	//int total_long_stop_mint;

	double total_dual_price = INIT_PRICE;
private:
	
	double _total_cost = 0;
	
	int _nbSegNodes;
	int _nbCrews;
	std::vector<Network::SegNode*> _segNodeSequence;		
	
	//const Penalty* _penalty_setting; //20191229 changed
	utils::EventPathCostTypeAmount _costtype_to_amount;

	// basic properties
	
	//一旦初始化便不会改变
	const csvComposition* _path_composition;
	//一旦初始化便不会改变
	const compoMode* _path_compoMode;

	std::map<std::string, int> _rank_to_num_map;

};

#endif // !SEG_PATH_H