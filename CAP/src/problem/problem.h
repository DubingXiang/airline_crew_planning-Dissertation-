#pragma once
#ifndef PROBLEM_H
#define PROBLEM_H
#include "../pch.h"
#include "../easylogging/easylogging++.h"


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
/**
 * @brief 将void*转化为具体的类型
 **/
class Concretizer {
	friend class ProblemBuilder;
	friend class Problem;
public:
	/*
	 * @brief 转换总体集合
	 * @detail 总体集合实质为vector<vector<void*>>，需要将其中每个元素vector<void*>中的元素void*转换为对应类型
	 * 转换后的对象集合分别存放于类的成员，所以实际上同一份数据用了两个对象来存，一个为void*类型，一个为实际具体类型
	 *
	 * @param std::map<std::string, std::vector<void*>>& dataSet key为类型名称，value为对应类型的对象集合
	 * @param const std::vector<std::string>& objNameSet 目标转换类型的名称集合
	 *
	 **/
	void concretizeAll(std::map<std::string, std::vector<void*>>& dataSet,
		const std::vector<std::string>& objNameSet);
	void clear();
	
private:
	void concretizeFlight(const std::vector<void*>& objSet, std::vector<Segment*>* segmentSet);
	void concretizeCrew(const std::vector<void*>& objSet, std::vector<CREW*>* crewSet);
	void concretizeBase(const std::vector<void*>& objSet, std::vector<BASE*>* baseSet);

	void concretizeCrewBase(const std::vector<void*>& objSet, std::vector<CREW_BASE*>* crewBaseSet);
	void concretizeCrewRank(const std::vector<void*>& objSet, std::vector<CREW_RANK*>* crewRankSet);
	void concretizeFlightComposition(const std::vector<void*>& objSet,
		std::vector<csvActivityComposition*>* fltCompositionSet);
	void concretizeComposition(const std::vector<void*>& objSet,
		std::vector<csvComposition*>* compositionSet);

	std::vector<Segment*> _segSet;
	std::vector<BASE*> _baseSet;
	std::vector<CREW*> _crewSet;
	std::vector<CREW_RANK*> _crew_rankSet;
	std::vector<CREW_BASE*> _crew_baseSet;
	std::vector<csvActivityComposition*> _fltCompositionSet;
	std::vector<csvComposition*> _compositionSet;
}; // end of Concretizer

class ProblemBuilder;
class Problem 
{
public:	
	class ProblemBuilder
	{
		friend class Problem;
	public:
		ProblemBuilder(Concretizer concretizer);
		
		~ProblemBuilder() {
			_segSet.clear();
			_crewSet.clear();
			_baseSet.clear();
			_crew_baseSet.clear();
			_crew_rankSet.clear();
			_fltCompositionSet.clear();
			_compositionSet.clear();
			_airportSet.clear();
		}

		/*
		 * @brief 按顺序执行初始化
		 **/
		Problem* build();		

	private:
		/*
		 * @brief 加载所有skill数据
		 * @attention TO BE FINISHED：当前就设置了一个高原机场"PlateauAirport"
		 **/
		void loadAllSkills();
		/*
		 * @brief 根据原始类型（Segment）对象创建计算所需类型（Opt_Segment）对象
		 * @detail 不改变原始类型对象，采用组合的方式，将原始类型对象作为对应新类型对象的成员
		 *
		 * @param std::vector<Opt_Segment*>* optSegSet 原始类型对象集合
		 * @attention 有动态内存分配(new Opt_Segment)操作
		 **/
		void createOptSegments();
		/*
		 * @brief 根据原始类型（CREW）对象创建计算所需类型（Opt_CREW）对象
		 * @detail 不改变原始类型对象，采用组合的方式，将原始类型对象作为对应新类型对象的成员
		 *
		 * @param std::vector<Opt_CREW*>* optCrewSet 原始类型对象集合
		 * @attention 有动态内存分配(new Opt_CREW)操作
		 **/
		void createOptCrews();
		/*
		 * @brief 赋值：Opt_Segment的配比要求
		 * @param std::vector<Opt_Segment*>* optSegSet
		 **/
		void matchOptSegmentAndComposition();
		/*
		 * @brief 赋值：每个Opt_Segment所需要的各个rank的人数
		 * @param std::vector<Opt_Segment*>* optSegSet
		**/
		void setRankToNumMapOfOptSegment();
		/*
		 * @brief 创建Opt_Crew的rankAry
		 * @param std::vector<Opt_CREW*>* optCrewSet
		 **/
		void matchOptCrewAndRankAry();
		/*
		 * @brief 创建Opt_Crew的baseAry
		 * @param std::vector<Opt_CREW*>* optCrewSet
		 **/
		void matchOptCrewAndBaseAry();
		/*
		 * @brief 初始化crew的skillsmap
		 * @detail set crew.skillMap[skill] = 0, foreach skill in skillSet
		 * @param std::vector<Opt_CREW*>* optCrewSet
		 * @attention 先检查crew的skillMap是否为初始状态，即为空
		 *
		 **/
		void initializeCrewSkills();
		/*
		 * @brief 赋值：Opt_Crew在集合optCrewSet中的下标
		 * @detail 全局的optCrewSet不允许改变，因而其中Opt_Crew的index也不会改变，便于查找
		 **/
		void setIndexOfCrew();
		/*
		 * @brief 对每个crew的rank按最新的时间排序
		 **/
		void sortCrewRank();

		void setCrewLatestRankAndPosition();

		/*
		 * @brief 赋值：所有airport的集合
		 **/
		void setAirportSet();

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

		/*以下集合存放void*转化为对应类型后的对象*/
		std::vector<Opt_CREW*> _optCrewSet;
		std::vector<Opt_Segment*> _optSegSet;


		std::set<std::string> _skillSet; //所有的crew skill
		std::vector<std::string> _airportSet;

	}; // end of ProblemBuilder

	Problem(const ProblemBuilder& builder); 
	
	static el::Logger* logger;	

	const std::vector<Segment*>& getSegmentSet() const { return _segSet; }
	const std::vector<CREW*>& getCrewSet() const { return _crewSet; }
	const std::vector<BASE*>& getBaseSet() const { return _baseSet; }
	const std::vector<CREW_BASE*>& getCrewBaseSet() const { return _crew_baseSet; }
	const std::vector<CREW_RANK*>& getCrewRankSet() const { return _crew_rankSet; }
	
	const std::vector<csvActivityComposition*>& getFltCompositonSet() const { return _fltCompositionSet; }
	const std::vector<csvComposition*>& getCompositionSet() const { return _compositionSet; }

	const std::set<std::string>& getSkillSet() const { return _skillSet; }
	const std::vector<std::string>& getAirptSet() const { return _airportSet; }

	const std::vector<Opt_Segment*>& getOptSegmentSet() const { return _optSegSet; }
	const std::vector<Opt_CREW*>& geOptCrewSet() const { return _optCrewSet; }
	
	
private:
	std::vector<Segment*> _segSet;	
	std::vector<CREW*> _crewSet;
	std::vector<BASE*> _baseSet;
	std::vector<CREW_BASE*> _crew_baseSet;
	std::vector<CREW_RANK*> _crew_rankSet;	
	std::vector<csvActivityComposition*> _fltCompositionSet;
	std::vector<csvComposition*> _compositionSet;

	std::set<std::string> _skillSet; //所有的crew skill
	std::vector<std::string> _airportSet;

	std::vector<Opt_Segment*> _optSegSet;
	std::vector<Opt_CREW*> _optCrewSet;

};


// end Problem





#endif // !PROBLEM_H
