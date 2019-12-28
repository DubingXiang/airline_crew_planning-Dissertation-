#pragma once
#ifndef COLUMN_GENERATION_H
#define COLUMN_GENERATION_H
#include "../../pch.h"
#include "../../problem/solution/crew_scheduling_solution.h"

#include "master_problem.h"
#include "subproblem.h"
#include "column_generation_module.h"

#include "ilcplex/ilocplex.h"

const int MAX = 1000 * 1000 * 1000;
const int MAX_NUM_ITER = 300;
const int REQUIRE_CAVERAGE_RATE = 0.95; //最低要达到的覆盖率，当达到后，才能开始求整数解
const int FREQUENCY_SOLVE_MIP = 20; //当达到REQUIRE_CAVERAGE_RATE后，每FREQUENCY_SOLVE_MIP次迭代就求一次整数解


/**
 * @class ColumnGeneration
 * @brief 列生成算法类
 * 列生成算法整体框架，包括主问题与子问题的求解和二者的迭代
 */
class CrewSchedulingColumnGenerationModule :public AbstractColumnGenerationModule
{
public:
	CrewSchedulingColumnGenerationModule();
	~CrewSchedulingColumnGenerationModule();

	

	/**
	 * @brief 初始化决策当天的输入
	 * @param 当前决策所在天数
	 * @param 初始乘务组
	 * @param 乘务员网络	  
	 * @param segment网络
	 * @param 乘务规则
	 * @param 惩罚成本参数
	 */
	void init(/*ColumnPool& initialPool*/int curDay, std::vector<CrewGroup*>& initialGroups, 
			  CrewNetwork& crewNet, 
			  SegNetwork& segNet, 
			  const CrewRules& rules,
			  const Penalty& penaltySetting);
	/**
	 * @brief 整体求解框架
	 */
	void solve();
	/**
	 * @brief 得到当前决策阶段的最优解
	 */
	CrewSchedulingSolution getBestSoln() { return *_best_solution_pool.front(); }
	
private:

	void solveMasterProblem() override;
	void solveSubProblem() override;
	void updateDuals() override;
	bool terminate() override;

	/**
	 * @brief 判断覆盖率是否足够高
	  @return true if 覆盖率达到REQUIRE_CAVERAGE_RATE
	 */	 
	bool isCoverageHigh();
	/**
	 * @brief 判断LP是否就是整数解
	 * @return true if LP松弛解就是整数解
	 */	
	bool isFeasibelSoln(IloNumVarArray& dvars);
	/**
	 * @brief 判断LP是否就是整数解
	 * @return return 1 if solve mip succeed
	 */	
	int solveMIP();
	/**
	 * @brief 获得可行解
	 * @param 待赋值的解
	 * 将mip求完的可行解输出，赋值给soln
	 */
	void getFeasibleSolution(CrewSchedulingSolution* soln);

	std::string _cur_day_str;

	CrewNetwork* _crew_net;
	SegNetwork* _seg_net;
	const CrewRules* _rules;
	const Penalty* _penalty;
	std::vector<CrewNode*>* _crew_node_set;
	std::vector<SegNode*>* _seg_node_set;

	MasterProblem* _master;
	SubProblem* _sub_pro;
	IloModel _mip_model;
	IloCplex _mip_cplex;

	std::vector<CrewSchedulingSolution*> _solution_pool; //整数解解池
	std::vector<CrewSchedulingSolution*> _best_solution_pool; //最好解池

	ColumnPool* _global_pool;
	double _lb;
	double _ub;
	int _max_num_iter;
	int _frequency_solve_mip = FREQUENCY_SOLVE_MIP;


	int _num_uncoverd_segments;
	int _num_rest_crews;


};
#endif // !COLUMN_GENERATION_H