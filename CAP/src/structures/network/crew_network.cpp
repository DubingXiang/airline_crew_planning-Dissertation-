#include "Crew_Network.h"
#include "../generic/csvClassesTransOptimizable.h"
#include "INetwork.h"
#include "../crew_rules.h"



CrewNode::CrewNode(Opt_CREW* optCrew) {
	this->optCrew = optCrew;
	visited = false;
	setNodeType(NodeType::crew);
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

CrewNetwork::~CrewNetwork() {
	delete resource;
	delete sink;
	resource = NULL;
	sink = NULL;
	
	_optCrewSet = NULL;
	_rules = NULL;

	nodeSet.clear();
	arcSet.clear();
}
void CrewNetwork::createNetwork() {
	setOptCrewNodeSet();
	createOptCrewArcs(); 
	std::cout << "number of crew arcs: " << arcSet.size() << "\n";
	createVirtualArcs();
	std::cout << "number of crew arcs, include virtrual arcs: " << arcSet.size() << "\n";
	
	nodeSet.insert(nodeSet.begin(), sink);
	nodeSet.insert(nodeSet.begin(), resource);

	std::cout << "total number of crew node: " << nodeSet.size() << "\n";
	

}
void CrewNetwork::rebuildNetwork() {
	
	for (size_t i = 0; i < nodeSet.size(); i++) {
		auto out_arcs = nodeSet[i]->outArcSet;
		auto in_arcs = nodeSet[i]->inArcSet;
		
		nodeSet[i]->inArcSet.clear();
		nodeSet[i]->outArcSet.clear();
	}
	nodeSet.erase(nodeSet.begin()); //erase resource
	nodeSet.erase(nodeSet.begin());//erase sink

	createOptCrewArcs();
	createVirtualArcs();
	nodeSet.insert(nodeSet.begin(), sink);
	nodeSet.insert(nodeSet.begin(), resource);
}

void CrewNetwork::setOptCrewNodeSet() {
	createOptCrewNodes();
	clusterCrewNode();
}
void CrewNetwork::createOptCrewNodes() {
	for (const auto& crew : *_optCrewSet) {
		CrewNode* node = new CrewNode(crew);
		node->setNodeType(NodeType::crew);
		nodeSet.emplace_back(node);
	}
}

void CrewNetwork::clusterCrewNode() {
	for (const auto& node : nodeSet) {		
		_pos_nodeset_map[node->optCrew->getCurRank()].emplace_back(node);
	}
}

// TODO: 应该可以多线程加速
void CrewNetwork::createOptCrewArcs() {
	std::vector<Opt_CREW*> opt_crew_set;
	
	// 法1.建立无向有环图
	/*for (auto& node1 : nodeSet) {
		for (auto& node2 : nodeSet) {
			opt_crew_set.emplace_back(node1->optCrew);
			opt_crew_set.emplace_back(node2->optCrew);
			if (!_rules->isFasibleCombination(opt_crew_set)) {
				continue;
			}
			CrewArc* arc = new CrewArc(node1, node2);
			arc->setArcType(ArcType::connect);
			arc->setArcLen(0);
			
			node1->outArcSet.emplace_back(arc);
			node2->inArcSet.emplace_back(arc);
			arcSet.emplace_back(arc);
		}
	}*/


	// 法2.建立有向无环图 按号位	
	std::vector<CrewNode*>* node_set;
	CrewNode *node1, *node2;
	for (auto& k_v : _pos_nodeset_map) {
		node_set = &k_v.second;
	
		int DEBUG_count_1 = 0;
		int DEBUG_count_2 = 0;

		for (size_t i = 0; i < node_set->size(); i++) {
			node1 = (*node_set)[i];
			for (size_t j = i+1; j < node_set->size(); j++) {
				node2 = (*node_set)[j];
				
				if (node1->optCrew->workStatus->restStation != node2->optCrew->workStatus->restStation) {
					continue;
				}

				DEBUG_count_1++;

				/*opt_crew_set.clear();
				opt_crew_set.emplace_back(node1->optCrew);
				opt_crew_set.emplace_back(node2->optCrew);
				if (!_rules->isFasibleCombination(opt_crew_set)) {
					continue;
				}*/
				DEBUG_count_2++;

				CrewArc* arc = new CrewArc(node1, node2);
				arc->setArcType(ArcType::connected);
				arc->setArcLen(0);

				node1->outArcSet.emplace_back(arc);
				node2->inArcSet.emplace_back(arc);
				arcSet.emplace_back(arc);
			}
		}

		std::cout << "到达判断搭配的次数count1 = " << DEBUG_count_1 << "\n";
		std::cout << "到达创建弧的次数count2 = " << DEBUG_count_2 << "\n";
	}

	//号位之间的弧
	//先只关注飞行员
	std::vector<CrewNode*>* cap_nodeset = &_pos_nodeset_map["CAP"];
	std::vector<CrewNode*>* fo_nodeset = &_pos_nodeset_map["FO"];

	for (auto& node1 : *cap_nodeset) {
		for (auto& node2 : *fo_nodeset) {
			if (node1->optCrew->workStatus->restStation != node2->optCrew->workStatus->restStation) {
				continue;
			}

			opt_crew_set.clear();
			opt_crew_set.emplace_back(node1->optCrew);
			opt_crew_set.emplace_back(node2->optCrew);
			/*if (!_rules->isFasibleCombination(opt_crew_set)) {
				continue;
			}*/
			CrewArc* arc = new CrewArc(node1, node2);
			arc->setArcType(ArcType::connected);
			arc->setArcLen(0);

			node1->outArcSet.emplace_back(arc);
			node2->inArcSet.emplace_back(arc);
			arcSet.emplace_back(arc);
		}
	}
} 
void CrewNetwork::createVirtualArcs() {		
	for (auto& crewnode : nodeSet) {		
		if (crewnode->optCrew->getCurRank() == "CAP") {
			CrewArc* out_arc = new CrewArc(resource, crewnode);
			out_arc->setArcType(ArcType::resourceOut);
			out_arc->setArcLen(0);

			resource->outArcSet.emplace_back(out_arc);
			crewnode->inArcSet.emplace_back(out_arc);
			arcSet.emplace_back(out_arc);
		}
		else if (crewnode->optCrew->getCurRank() == "FO") {
			CrewArc* in_arc = new CrewArc(crewnode, sink);
			in_arc->setArcType(ArcType::sinkIn); 
			in_arc->setArcLen(0);

			crewnode->outArcSet.emplace_back(in_arc);
			sink->inArcSet.emplace_back(in_arc);
			arcSet.emplace_back(in_arc);
		}

		
	}
}