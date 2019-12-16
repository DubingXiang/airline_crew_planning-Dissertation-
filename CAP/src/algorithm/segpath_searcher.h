#pragma once
#ifndef SEGPATH_SEARCHER_H
#define SEGPATH_SEARCHER_H
#include "../pch.h"



class SegNetwork;
class SegPath;
class SegNode;
class SegArc;
class Label;

class CrewRules;

class SegPathSearcher
{
public:
	//！之前写的PathFinder::createCurResource的功能应该交给其他模块处理。
	//！得到的_s是已经处理好的虚拟起点
	//SegPathSearcher(SegNetwork& segNet, SegNode& s, CrewRules& rules);
	~SegPathSearcher();
	
	void init(SegNetwork& segNet, SegNode& s, CrewRules& rules);
	void reset(SegNode& s);
	void search();
	std::vector<SegPath*>& getPathSet() { return _segpath_set; }
	void clear();

private:
	SegNetwork* _net;
	SegNode* _s;
	//SegNode* _t; _t is not necessary
	CrewRules* _rules;
	//! output all the feasible (single day) paths 
	std::vector<SegPath*> _segpath_set;

	/* For enumerate */


	std::map<SegArc*, int> _visited_by_path;		
	//! initialize _s
	void initStartNode();
	void enumerateByDFS();

	void extend(const Label& curLabel, const SegArc& curArc, Label* nextLabel);	
	void updateLabel(Label& curLabel, SegArc& curArc, Label* nextLabel);
	//！检查时长约束	
	bool isValidDuration(const Label& curLabel);
	//! 若达到终止条件，说明到当前节点的上一节点已经是最长的可行路径了。
	//！则无需再继续向下一层节点延伸，所以需要回溯至上一层
	bool isMetTermination(const Label& curLabel);
	
	////！检查路径是否满足dhd约束
	////! 主要是检查dhd是否仅出现在start_node or end_node
	//bool isValidDeahhead(Label& label);
		
	SegPath* backtrack(Label& endLabel);
};


#endif // !SEGPATH_SEARCHER_H