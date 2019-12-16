#pragma once
#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include "..\pch.h"
#include "..\io\input\input_handle.h"
#include "..\algorithm\column_generation\column_generation.h"


class CrewRules;
class PosOrderSeqvec;
class OrderPosSeqvec;

class Opt_Segment;
class Opt_CREW;

class SegNetwork;
class CrewNetwork;
class CrewGroup;
class SegPath;
class GroupSearcher;

/**
 * @class Optimizer
 * @brief 优化框架类
 * 按天滚动优化的基本框架
 * 单天决策中调用Column Generation算法
 */

class Optimizer
{
public:
	Optimizer();
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

	/*******input*******/
	/**
	 * @brief 加载数据
	 * @param 各个模块数据的集合
	 * @param 各个模块数据的名称
	 * 将csvReader已经读入的数据（void*型）转化为具体模块对象类型
	 */
	void loadData(std::map<std::string, std::vector<void*>>& dataSet, const std::vector<std::string>& objNameSet);
	/**
	 * @brief 读入乘务规则，赋给私有成员
	 * @param 乘务规则
	 */
	void loadCrewRules(CrewRules& rules);	
	/**
	 * @brief 读入惩罚成本参数值
	 * @param 惩罚成本参数类
	 */
	void loadPenaltySetting(const Penalty& penaltySeeting);
	/**
	 * @brief 初始化各个输入对象	 
	 */
	void init();
	/**
	 * @brief 测试函数，创建测试案例	 
	 */
	void createCase();
	/// 全局解池		
	std::vector<Solution*> soln_pool;
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
	void updateStatus(const time_t startCurDay, Solution& soln);

	InputHandler _inputHandler;
	CrewRules* _rules;
	const Penalty* _penalty;
	
	std::vector<Opt_Segment*> _optSegSet;
	std::vector<Opt_CREW*> _optCrewSet;

	SegNetwork* _segNet;
	CrewNetwork* _crewNet;	

	/*for calculate*/
	std::map<std::string, SegNodeSet> _day_segnode_map;
	std::map<time_t, SegNodeSet> _daytime_segnode_map;
	
	time_t _begining_plan;
	const int _SECONDS_ONE_DAY = 24 * 3600; //24 hour * 3600 secends
	SegNodeSet* _cur_day_segnode_set;

	// added-20190819

	std::map<std::string, std::vector<CrewNode*>> _pos_crewnodes; //position-vector<CrewNode*>
};

#endif // !OPTIMIZER_H