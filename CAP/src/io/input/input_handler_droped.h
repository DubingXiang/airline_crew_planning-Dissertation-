#pragma once
#ifndef INPUT_HANDLE_H
#define INPUT_HANDLE_H
#include "../../pch.h"

class CREW;
class CREW_RANK;
class CREW_BASE;
class Segment;
class BASE;
struct csvActivityComposition;
struct csvComposition;
class CrewRules;

class Opt_CREW;
class Opt_Segment;


class InputHandler
{
public:	
	InputHandler() {}
	~InputHandler() {
		_segSet.clear();
		_baseSet.clear();
		_crewSet.clear();
		_crew_rankSet.clear();
		_crew_baseSet.clear();
		_fltCompositionSet.clear();
		_compositionSet.clear();
		_airportSet.clear();
	}

	void init(std::map<std::string, std::vector<void*>>& dataSet,
		const std::vector<std::string>& objNameSet);
	/*
	 * @brief 转换总体集合
	 * @detail 总体集合实质为vector<vector<void*>>，需要将其中每个元素vector<void*>中的元素void*转换为对应类型
	 * 转换后的对象集合分别存放于类的成员，所以实际上同一份数据用了两个对象来存，一个为void*类型，一个为实际具体类型
	 *
	 * @param std::map<std::string, std::vector<void*>>& dataSet key为类型名称，value为对应类型的对象集合
	 * @param const std::vector<std::string>& objNameSet 目标转换类型的名称集合	 
	 *
	 **/
	void transformInputObjSet(std::map<std::string, std::vector<void*>>& dataSet, 
		const std::vector<std::string>& objNameSet);

	/*
	 * @brief 加载所有skill数据
	 * @detail
	 *
	 * @attention TO BE FINISHED：当前就设置了一个高原机场"PlateauAirport"
	 *
	 **/
	void loadAllSkills();

	/*
	 * @brief 根据原始类型（Segment）对象创建计算所需类型（Opt_Segment）对象
	 * @detail 不改变原始类型对象，采用组合的方式，将原始类型对象作为对应新类型对象的成员
	 *
	 * @param std::vector<Opt_Segment*>* optSegSet 原始类型对象集合	 
	 * @attention 有动态内存分配(new Opt_Segment)操作	 
	 **/
	void createOptSegments(/*std::vector<Opt_Segment*>* optSegSet*/);
	/*
	 * @brief 根据原始类型（CREW）对象创建计算所需类型（Opt_CREW）对象
	 * @detail 不改变原始类型对象，采用组合的方式，将原始类型对象作为对应新类型对象的成员
	 *
	 * @param std::vector<Opt_CREW*>* optCrewSet 原始类型对象集合
	 * @attention 有动态内存分配(new Opt_CREW)操作
	 **/
	void createOptCrews(/*std::vector<Opt_CREW*>* optCrewSet*/);

	/*
	 * @brief 赋值：Opt_Segment的配比要求
	 * @detail
	 *
	 * @param std::vector<Opt_Segment*>* optSegSet
	 *
	 **/
	void matchOptSegmentAndComposition(/*std::vector<Opt_Segment*>* optSegSet*/
		/*std::vector<csvActivityComposition*>* fltCompoSet,
		std::vector<csvComposition*>* compoSet*/);
	
	/*
	 * @brief 赋值：每个Opt_Segment所需要的各个rank的人数
	 * @detail
	 *
	 * @param std::vector<Opt_Segment*>* optSegSet
	 *
	 **/
	void setRankToNumMapOfOptSegment(/*std::vector<Opt_Segment*>* optSegSet*/);

	/*
	 * @brief 创建Opt_Crew的rankAry
	 * @detail
	 *
	 * @param std::vector<Opt_CREW*>* optCrewSet
	 *
	 **/
	void matchOptCrewAndRank(/*std::vector<Opt_CREW*>* optCrewSet*/);
	/*
	 * @brief 创建Opt_Crew的baseAry
	 * @detail
	 *
	 * @param std::vector<Opt_CREW*>* optCrewSet
	 *
	 **/
	void matchOptCrewAndBase(/*std::vector<Opt_CREW*>* optCrewSet*/);

	/*
	 * @brief 初始化crew的skillsmap
	 * @detail set crew.skillMap[skill] = 0, foreach skill in skillSet
	 *
	 * @param std::vector<Opt_CREW*>* optCrewSet
	 *
	 * @attention 先检查crew的skillMap是否为初始状态，即为空
	 *
	 **/
	void initializeCrewSkills(/*std::vector<Opt_CREW*>* optCrewSet*/);

	/*
	 * @brief 赋值：Opt_Crew在集合optCrewSet中的下标
	 * @detail 全局的optCrewSet不允许改变，因而其中Opt_Crew的index也不会改变，便于查找
	 *
	 * @param
	 *
	 **/
	void setIndexOfCrew(/*std::vector<Opt_CREW*>* optCrewSet*/);

	/*
	 * @brief 对每个crew的rank按最新的时间排序
	 * @detail
	 *
	 * @param
	 *
	 **/
	void sortCrewRank(/*std::vector<CREW_RANK*>* crewrankAry*/);

	/*
	 * @brief 赋值：所有airport的集合
	 * @detail
	 *
	 * @param
	 *
	 **/
	void setAirportSet(/*const std::vector<Segment*>& fltSet*/);
	
	std::vector<BASE*>& getBaseSet() { return _baseSet; }
	
	/***TODO 这部分应该移除，不属于InputHandler的功能*******************/
	////先考虑飞行员
	//std::vector<Opt_CREW*>* getPilotSet(const std::vector<Opt_CREW*>& optCrewSet);
	//std::vector<std::string>& createSpecialArpSet();
	//void randomSetCrewSkills(std::vector<Opt_CREW*>* optCrewSet, double percent = 0.3);
	//// 得到号位的全排列
	//void setRankCombination(CrewRules* rules);
	////! 按配比记录对应的号位搭配，用组合的一个排列来代表这个组合
	//void setRankCombination_OnePermutation(CrewRules* rules);
	
private:

	/*
	 * @brief 将void*转换为具体类型
	 * @detail
	 *
	 * @param std::vector<void*>& objSet void*对象集合
	 * @param const std::string& objName 目标转换类型
	 * @param void* outObjSet 输出参数，转换后的目标类型对象集合
	 *
	 **/
	void typeTrans(std::vector<void*>& objSet, const std::string& objName, void* outObjSet);

	/*以下集合存放void*转化为对应类型后的对象
	 * 生命周期为：直至整个程序结束
	 */

	std::vector<Segment*> _segSet;
	std::vector<BASE*> _baseSet;
	std::vector<CREW*> _crewSet;
	std::vector<CREW_RANK*> _crew_rankSet;
	std::vector<CREW_BASE*> _crew_baseSet;
	std::vector<csvActivityComposition*> _fltCompositionSet;
	std::vector<csvComposition*> _compositionSet;
	
	/*end 以下集合存放void*转化为对应类型后的对象*/

	std::set<std::string> _skillSet; //所有的crew skill
	std::vector<std::string> _airportSet;
	//std::vector<std::string> _special_airportSet;


	
};

#endif // !INPUT_HANDLE_H
