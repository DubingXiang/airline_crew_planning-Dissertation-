#pragma once
#ifndef CREW_NODE_ARC_H
#define CREW_NODE_ARC_H
#include "INetwork.h"
#include <vector>

class Opt_CREW;
class CrewRules;

namespace Network {
	class CrewNode;
	class CrewArc;
	using CrewNodeSet = std::vector<CrewNode*>;
	using CrewArcSet = std::vector<CrewArc*>;

	class CrewNode :public INode
	{
	public:
		CrewNode();
		CrewNode(Opt_CREW* optCrew);
		virtual ~CrewNode();
		virtual void setNodeType(NodeType type);

		void setPrice(double price);		
		CrewArcSet outArcSet;
		CrewArcSet inArcSet;

		Opt_CREW* optCrew;
		double price;

		//bool visited;
	};
	

	class CrewArc :public IArc
	{
	public:
		CrewArc(CrewNode* startNode, CrewNode* endNode);
		virtual ~CrewArc();
		virtual void setArcType(ArcType type);
		virtual void setArcLen(int length);

		CrewNode* startNode;
		CrewNode* endNode;
	};
}

#endif // !CREW_NODE_ARC_H


