#pragma once
#ifndef MASTER_PROBLEM_H
#define MASTER_PROBLEM_H
#include "column.h"
#include "..\..\structures\network\crew_network.h"
#include "..\..\structures\network\seg_network.h"
#include "..\..\structures\generic\csvClassesTransOptimizable.h"
#include "ilcplex\ilocplex.h"
#include <memory>


const int PENALTY_UNCOVER = 200000;

/**
 * @class MasterProblem 
 * @brief 主问题-指派问题
 * 
 */
class MasterProblem 
{
public:
	MasterProblem();		
	~MasterProblem();

	/**	 
	 * @brief 设置基本参数
	 *
	 */
	void initSetting();
	/**
     * @brief 初始化输入参数     
     * @param 初始的列池
	 * @param 要覆盖的segment集合
	 * @param 要指派的crew集合
     */
	void init(ColumnPool* columnpool_ptr, std::vector<Opt_Segment*>& segSet, std::vector<CrewNode*>& crewNodeSet);
	/**
	 * @brief 记录crew决策前的初始飞时
	 *
	 */	
	void initParameters();	
	/**
	 * @brief 调用cplex建立模型
	 */
	void buildModel();
	/**
	 * @brief 调用cplex输出模型文件
	 * @param 当前决策天的id
	 * @param 当前所处迭代次数
	 */
	void exportModel(const std::string& curDayStr, int iter);
	/**
	 * @brief 调用cplex求解模型
	 */
	int solve();
	/**
	 * @brief 调用cplex输出解文件
	 * @param 当前决策天的id
	 * @param 当前所处迭代次数
	 */
	void writeSoln(const std::string& curDayStr, int iter);
	/**
	 * @brief 获得目标函数值
	 */
	double getObjValue() const { return _objValue; }
	/**
	 * @brief 获得segment对偶价格
	 */
	std::vector<double>& getSegCoverDuals();
	/**
	 * @brief 获得crew对偶价格
	 */
	std::vector<double>& getCrewAssignDuals();
	/**
	 * @brief 获得路径决策变量的集合
	 */
	IloNumVarArray& getPathDvar() { return _dvars_path; }
	/**
	 * @brief 获得segment未覆盖决策变量的集合
	 */
	IloNumVarArray& getUncoverDvar() { return _dvars_uncover; }

	/**
	 * @brief 添加新列到模型中
	 * @param 新列集合
	 */
	void addNewColumns(ColumnPool& columnpool_ptr);

	ColumnPool* global_pool; //主问题空间的列集合
	IloModel _model;
	IloCplex _cplex;
	
private:
	/**
	 * @brief end all except _env
	 */	
	void end();
	/**
	 * @brief 添加目标函数至模型
	 * 包括：1）duty的最小成本总和 2）均衡性目标-最小方差
	 */
	void addObjFunction();
	/**
	 * @brief 添加约束至模型
	 * 包括：1）segment覆盖约束 2）crew指派约束
	 */
	void addConstraints2();
	
	IloEnv _env;
	
	IloObjective _obj;
	double _objValue;
	IloNumVarArray _dvars_path; //路径决策变量
	IloNumVarArray _dvars_uncover; //路径决策变量

	/*IloNumVar _dvar_upper_flytime;
	IloNumVar _dvar_lower_flytime;*/
	/*IloNumVar _dvar_upper_seg_num;
	IloNumVar _dvar_lower_seg_num;*/

	IloRangeArray _constraints_all;
	IloRangeArray _constraints_segcover;
	IloRangeArray _constraints_crewassign;
	/*IloRangeArray _constraints_blance_u;
	IloRangeArray _constraints_blance_l;*/

	std::map<int, IloRange> _seg_index_cover_constraint;
	std::map<int, IloRange> _crew_index_assign_constraint;

	//std::vector<double> cost;
	std::vector<int> _init_crew_fly_mint;

	std::vector<Opt_Segment*>* _seg_set;
	std::vector<CrewNode*>* _crewnode_set;
	size_t _seg_num;
	size_t _crew_num;

	std::vector<double> seg_cover_duals;
	std::vector<double> crew_assign_duals;
};


#endif // !MASTER_PROBLEM_H