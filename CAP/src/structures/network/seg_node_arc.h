#pragma once
#ifndef SEGMENT_NODE_ARC_H
#define SEGMENT_NODE_ARC_H
#include "INetwork.h"

class BASE;
class CrewRules;
class Opt_Segment;

namespace Network {
	class SegNode;
	class SegArc;
	class Label;

	using SegNodeSet = std::vector<SegNode*>;
	using SegArcSet = std::vector<SegArc*>;
	/**
	 * @class SegNode
	 * @brief segment节点类 继承自INode
	 */
	class SegNode : public INode
	{
	public:
		SegNode();
		SegNode(Opt_Segment& optSeg);
		virtual ~SegNode();
		/**
		 * @brief 设置节点类型
		 * @param 节点类型
		 */
		virtual void setNodeType(NodeType type);

		SegArcSet outArcSet;
		SegArcSet inArcSet;

		Opt_Segment* optSegment;
		std::string depStation;
		std::string arvStation;
		//std::string name;
		time_t startDtUtc;
		time_t startDtLoc;
		time_t endDtUtc;
		time_t endDtLoc;
		int taskMin;


		//std::vector<Label*> labelSet;
		Label* label;

	};

	/**
	 * @class SegArc
	 * @brief segment相关的弧-继承自IArc
	 */
	class SegArc : public IArc
	{
	public:
		SegArc(SegNode* startNode, SegNode* endNode);
		virtual ~SegArc();
		/**
		 * @brief 设置弧的类型
		 * @param 弧的类型
		 */
		virtual void setArcType(ArcType type);
		/**
		 * @brief 设置弧的长度
		 * @param 弧的长度
		 */
		virtual void setArcLen(int length);

		SegNode* startNode;
		SegNode* endNode;
	};

	/**
	 * @class Label
	 * @brief 标号类-枚举duty时记录搜索信息
	 */
	class Label
	{
	public:
		Label() {
			accumuFlyMin = 0;
			accumuWorkMin = 0;
			accumuCost = 0;
			preLabel = NULL;
			preArc = NULL;
		}
		int accumuFlyMin;
		int accumuWorkMin;
		int accumuCost;
		Label* preLabel;
		SegArc* preArc;
	};

}// end namespace Network

#endif // !SEGMENT_NODE_ARC_H
