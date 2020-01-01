#pragma once
#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include "../pch.h"

#include "../problem/problem.h"
#include "../structures/network/seg_network.h"
#include "../structures/network/crew_network.h"
#include "../algorithm/column_generation/column_generation.h"


class PosOrderSeqvec;
class OrderPosSeqvec;


//class CrewGroup;
//class EventPath;

//class GroupSearcher;

/**
 * @class Optimizer
 * @brief 优化框架类
 * 按天滚动优化的基本框架
 * 单天决策中调用Column Generation算法
 */

class Optimizer
{
public:
	Optimizer(const Problem* problem, const CrewRules* rules/*, const Penalty* penelty*/) :
		_problem(problem),
		_rules(rules)
		/*_penalty(penelty)*/ {
	
	}
	~Optimizer();

	/**
	 * @brief 优化算法的整体框架
	 * 1.Initialize: construct crew and segment network
	 * 2.SegpathSearching: search seg path, actually, enumerate all feasible path in a single day	 
	 * 3.CrewgroupSearching: search crew groups
	 * 4.MatchGroupAndPath: to get a match which has the smallest (negtive) reduced cost. actually, get multiple path(column)
	 * 5.MasterModelSolver: construct master model according to columns got in last process, and solve linear relaxatiion problem
	 * 6.DualInfomationPassing: uapate dual information of crew and seg network according the dual info got from last process
	 * 7.Repeat step 2-5 unless optimal solution found
	 */
	void optimize();

	static el::Logger* logger;

	/// 全局解池		
	std::vector<CrewSchedulingSolution*> soln_pool;
private:	
	/**
	 * @brief 选择初始乘务组
	 * @param 初始乘务组
	 * 采用启发式方法生成每天决策的初始乘务组，以生成初始解	 
	 */
	void initialCrewGroup(CrewGroup& initGroup);
	/**
	 * @brief 对segment按所在天数分类，存入map中	 	 
	 */
	void clusterSegNode();
	/**
	 * @brief 赋值，需要当天决策的所有segmentNode
	 * @param 当前决策天
	 * 
	 */
	void setCurDayStartNodeSet(int curDay);
	/**
	 * @brief 更新crew的状态，当天决策完后执行
	 * @param 当前天的起始时间
	 * @param 当前天的决策结果
	 */ 
	void updateStatus(const time_t startCurDay, CrewSchedulingSolution& soln);
	
	const Problem* _problem;
	const CrewRules* _rules;
	//const Penalty* _penalty; //changed 20191229
	
	std::vector<Opt_Segment*> _optSegSet;
	std::vector<Opt_CREW*> _optCrewSet;

	Network::SegNetwork* _segNet;
	Network::CrewNetwork* _crewNet;

	/*for calculate*/
	std::map<std::string, Network::SegNodeSet> _day_segnode_map;
	std::map<time_t, Network::SegNodeSet> _daytime_segnode_map;
	
	time_t _begining_plan;
	
	Network::SegNodeSet* _cur_day_segnode_set;

	// added-20190819

	std::map<std::string, std::vector<Network::CrewNode*>> _pos_crewnodes; //position-vector<CrewNode*>
};

#endif // !OPTIMIZER_H