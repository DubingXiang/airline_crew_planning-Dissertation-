#pragma once
#ifndef SUBPROBLEM_H
#define SUBPROBLEM_H
#include "../../pch.h"
#include "../crewgroup_searcher.h"
#include "../segpath_searcher.h"
#include "../../structures/network/crew_network.h"
#include "../../structures/network/seg_network.h"
#include "../../structures/generic/csvClassesTransOptimizable.h"
#include "../../structures/crew_rules.h"
#include "column.h"
#include "../../structures/param_setting/cost_parameters.h"


/**
 * @class SubProblem
 * @brief 子问题求解类
 * 包括：
 * 1）枚举决策当天所有可行duty 
 * 2）CG迭代过程中为duty搜索可行的乘务组
 * 3）duty和乘务组二者组合，挑选reduced cost小于0的新列
 */
class SubProblem 
{
public:
	SubProblem(CrewNetwork& crewNet, SegNetwork& segNet, const CrewRules& rules);
	~SubProblem();
	/**
	 * @brief set初始乘务组
	 */
	void setInitialGroups(std::vector<CrewGroup*>& initialGroups);
	/**
	 * @brief 枚举所有可行duty
	 * 整个单天决策只执行一次
	 */	
	void findSegPaths();
	/**
	 * @brief 计算duty的成本
	 * @param 各项惩罚成本参数
	 * 
	 */
	void setPathStatus(const Penalty& penalty); //TODO：改名
	/**
	 * @brief 设置决策当天须覆盖的segment集合
	 * 先调用了私有函数得到当天的所有segPath
	 * 然后执行该函数
	 */		
	void setCurDaySegSet();
	/**
	 * @brief 设置决策当天须覆盖的segment的index
	 * 赋值每个segment在全局的segment集合中的位置，用下标表示	 
	 */
	void setIndexOfCurSeg();
	/**
	 * @brief 标记duty的特殊属性
	 * duty是否需要特殊资质的人员担当
	 */
	void labelSpecialSegPath(std::vector<std::string>& specialAirports);
	/**
	 * @brief 对duty按配比分类	 
	 */	
	void groupingSegPathByComposition();
	
	
	/*估值的功能不应该由subproblem实现，均值应该作为subproblem的一个参数*/	
	/**
	 * @brief 根据duty_set和crew_set对flymint的均值进行预估
	 */
	void estimateMeanFlyMin();
	/**
	 * @brief 不同rank有各自的估计均值, 根据duty_set和crew_set对flymint的均值进行预估
	 * 计算公式：mean_fly_min_rank[i] = sum(seg.fly_min * required_num_rank[i]) / total_num_rank[i]
	 */		
	void estimateMeanFlyMinPrecisly();
	/**
	 * @brief 获得估计的飞时均值
	 */
	int getMeanFlyMin() const { return _mean_fly_mint; }
	/**
	 * @brief 更新对偶价格
	 * @param segment覆盖约束对应的乘子
	 * @param crew指派约束对应的乘子
	 */
	void updateDuals(std::vector<double>& segCoverDuals, std::vector<double>& crewAssignDuals);
	/**
	 * @brief 搜索对偶乘子较大的乘务组
	 */
	void findGroups();
	
	/**
	 * @brief 获得估计的飞时均值	 
	 * create new columns, add to this->_local_pool
	 * match crew group and segment path
	 * to get columns that have small negetive reduced cost
	 * actually, is assign group for each segpath in _seg_path_searcher._segpath_set
	 */
	void matchGroupAndPath();	
	/**
	 * @brief 搜索crewGroup，并匹配Group与segpath得到新列
	 * TODO
	 */	
	void findColumnSet();
	
	/**
	 * @brief 为crew添加休息决策变量，相当于人工变量
	 *
	 */	
	void addRestColumns();

	/*output for master problem*/
	
	/**
	 * @brief 获得当前须覆盖的segment集合
	 * 只传递一次，主问题初始化的时候
	 */	
	std::vector<Opt_Segment*>& getCurDaySegSet() { return _cur_day_seg_set; }
	/**
	 * @brief 获得每次求解得到的新列
	 * 
	 */
	ColumnPool& getCurLocalPool() { return _local_pool; }

private:		
	/**
	 * @brief 计算均衡性成本-绝对值形式
	 * @param duty的飞时 
	 * @param 与duty配对的乘务组
	 * 根据预估的flymint均值，计算所有column的cost
	 * cost = sum(crewGroup's flyMin + dutyFlyMin - meanFlyMin)	
	 */
	int calBalanceCost(const int dutyFlyMin, CrewGroup& crewGroup);
	/**
	 * @brief 计算均衡性成本-方差形式
	 * @param duty的飞时
	 * @param 与duty配对的乘务组
	 * 根据预估的各rank的flymint均值，计算所有column的均衡cost
	 */	
	int calBalanceCostByVariance(const int dutyFlyMin, CrewGroup& crewGroup);
	/**
	 * @brief 计算均衡性成本-方差形式
	 * 按opt_crew的对偶价格降序排序
	 */	
	void sort_pos_crew_set();
	/**
	 * @brief 根据配比搜索合法的crewGroup
	 * @param 配比的名称信息，如"1CAP1FO"
	 * @param 输出参数 compositionName参数对应的合法乘务组集合
	 */	
	void searchGroupByComposition(const std::string compositionName, std::vector<CrewGroup*>* crewGroupSet);
	/**
	 * @brief 判断乘务组和duty是否可以配对
	 * @param 乘务组
	 * @param duty
	 * @return true if 满足约束:1）时间 2）空间 3）配比 4）特殊资质
	 */
	bool isMatchable(CrewGroup& group, SegPath& segPath);

	std::map<std::string, std::vector<CrewNode*>> _pos_crewnodes_map;
	std::map<std::string, int> _rank_to_num_crew_map;

	const CrewRules* _rules;
	std::vector<Opt_Segment*> _cur_day_seg_set;
	std::vector<CrewNode*> _crewnode_set; //TODO:未初始化
	ColumnPool _local_pool; //每次子问题得到的新列

	GroupSearcher _group_searcher;
	SegPathSearcher _seg_path_searcher;
	std::vector<SegPath*> _cur_day_path_set;
	
	int _mean_fly_mint;
	std::map<std::string, int> _rank_to_mean_fly_mint;
	/*std::vector<double> _seg_cover_duals;
	std::vector<double> _crew_assign_duals;
	std::vector<double> _balance_flytime_duals;
	std::vector<double> _balance_resttime_duals;*/

	std::map<std::string, std::vector<SegPath*>> _compo_mode_segpath_set;


};

#endif // !SUBPROBLEM_H

