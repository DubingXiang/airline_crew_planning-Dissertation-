#include "crew_node_arc.h"
#include "../generic/csvClassesTransOptimizable.h"
#include "../crew_rules.h"


namespace Network {

	//void CrewNode::test() {}
	CrewNode::CrewNode() {}
	CrewNode::CrewNode(Opt_CREW* optCrew) {
		this->optCrew = optCrew;
		visited = false;
		setNodeType(NodeType::CREW);
	}
	CrewNode::~CrewNode() {
		optCrew = NULL;
		outArcSet.clear();
		//price = INT_MAX;	
	}
	void CrewNode::setNodeType(NodeType type) {
		nodeType = type;
	}
	void CrewNode::setPrice(double price) {
		this->price = price;
	}

	CrewArc::CrewArc(CrewNode* startNode, CrewNode* endNode) {
		this->startNode = startNode;
		this->endNode = endNode;
	}
	CrewArc::~CrewArc() {
		startNode = NULL;
		endNode = NULL;
		len = -1;
	}
	void CrewArc::setArcType(ArcType type) {
		arcType = type;
	}
	void CrewArc::setArcLen(int length) {
		len = length;
	}
}