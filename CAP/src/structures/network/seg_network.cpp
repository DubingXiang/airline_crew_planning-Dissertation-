#include "Seg_Network.h"
#include "..\..\..\Segment_mine.h"
#include "..\..\..\csv_impl.h"
#include "..\generic\csvClassesTransOptimizable.h"
#include "INetwork.h"
#include "..\crew_rules.h"


SegNode::SegNode() {
	optSegment = NULL;
	depStation = "";
	arvStation = "";
	startDtUtc = -1;
	startDtLoc = -1;
	endDtUtc = -1;
	endDtLoc = -1;
	taskMin = 0;

	visited = false;
}
SegNode::SegNode(Opt_Segment& optSeg) {
	optSegment = &optSeg;
	depStation = optSeg.getDepStation();
	arvStation = optSeg.getArrStation();
	startDtUtc = optSeg.getStartTimeUtcSch();
	startDtLoc = optSeg.getStartTimeLocSch();
	endDtUtc = optSeg.getEndTimeUtcSch();
	endDtLoc = optSeg.getEndTimeLocSch();
	taskMin = optSeg.getBlkMinutes();
	visited = false;
}
SegNode::~SegNode() {
	optSegment = NULL;	
	//labelSet.clear();
	delete label;
	outArcSet.clear();
	inArcSet.clear();
}
void SegNode::setNodeType(NodeType type) {
	nodeType = type;
}


SegArc::SegArc(SegNode* startNode, SegNode* endNode) {
	this->startNode = startNode;
	this->endNode = endNode;
}
SegArc::~SegArc() {
	startNode = NULL;
	endNode = NULL;
	len = -1;
}
void SegArc::setArcType(ArcType type) {
	arcType = type;
}
void SegArc::setArcLen(int length) {
	len = length;
}


SegNetwork::SegNetwork(std::vector<Opt_Segment*>* optSegSet, std::vector<BASE*>* baseSet, CrewRules* rules) {
	_optSegSet = optSegSet;
	_baseSet = baseSet;
	_rules = rules;

	resource = new SegNode();
	sink = new SegNode();

	resource->optSegment = NULL;
	resource->depStation = "resource";
	resource->arvStation = "resource";
	resource->startDtUtc = 0;
	resource->startDtLoc = 0;
	resource->endDtUtc = 0;
	resource->endDtLoc = 0;
	resource->taskMin = 0;
	
	resource->setNodeType(NodeType::resource);
	resource->label = NULL;
	resource->visited = false;


};
SegNetwork::~SegNetwork() {
	resource = NULL;
	sink = NULL;
	
	_optSegSet = NULL;
	_baseSet = NULL;
	_rules = NULL;
	
	nodeSet.clear();
	arcSet.clear();
}

void SegNetwork::createNetwork() {
	createOptSegNodes();	
	std::cout << "number of nodes" << nodeSet.size() << "\n";
	createDeadheadNodes();
	std::cout << "number of nodes, include dhd-node" << nodeSet.size() << "\n";
	sortNodeSet();
	createBaseNodes();
	
	createOptSegArcs();
	std::cout << "number of arcs" << arcSet.size() << "\n";
	createBaseArcs();
	std::cout << "number of arcs, include base-arcs" << arcSet.size() << "\n";

	addLongArcs();
	std::cout << "number of arcs, after add long-arcs" << arcSet.size() << "\n";
	
	removeIsolatedNode();
	std::cout << "After removed Isolated Nodes\n";
	std::cout << "final number of arcs" << arcSet.size() << "\n";
	std::cout << "final number of arcs" << arcSet.size() << "\n";
}

void SegNetwork::updateResourceAndSink() {		
	resource->visited = false;	
	resource->outArcSet.clear();
	resource->outArcSet.shrink_to_fit();
	resource->inArcSet.clear();
	resource->inArcSet.shrink_to_fit();
	resource->label = new Label();
	resource->label->preLabel = new Label();
}
void SegNetwork::updateVirtualArcs(SegNodeSet& startNodeSet) {
	for (auto& segnode : startNodeSet) {
		SegArc* out_arc = new SegArc(resource, segnode);
		out_arc->setArcType(ArcType::resourceOut);
		out_arc->setArcLen(0);

		resource->outArcSet.emplace_back(out_arc);
		segnode->inArcSet.emplace_back(out_arc);
	}
}

void SegNetwork::createOptSegNodes() {
	for (const auto& seg : *_optSegSet) {
		SegNode* node = new SegNode(*seg);
		node->setNodeType(NodeType::seg);
		nodeSet.emplace_back(node);
	}
}
void SegNetwork::createDeadheadNodes() {
	for (const auto& seg : *_optSegSet) {
		SegNode* node = new SegNode(*seg);
		node->setNodeType(NodeType::dhd);
		nodeSet.emplace_back(node);
	}
}
void SegNetwork::sortNodeSet() {
	std::sort(nodeSet.begin(), nodeSet.end(),
		[](const SegNode* node1, const SegNode* node2) {return node1->startDtUtc < node2->startDtUtc; });
}

void SegNetwork::createBaseNodes() {
	for (size_t i = 0; i < _baseSet->size(); i++) {
		SegNode* node_o = new SegNode();
		node_o->setNodeType(NodeType::baseO);
		node_o->depStation = (*_baseSet)[i]->base;
		node_o->arvStation = node_o->depStation;
		_base_nodeset.emplace_back(node_o);

		SegNode* node_d = new SegNode();
		node_d->setNodeType(NodeType::baseD);
		node_d->arvStation = (*_baseSet)[i]->base;
		node_d->depStation = node_d->arvStation;
		_base_nodeset.emplace_back(node_d);
	}
}


void SegNetwork::createOptSegArcs() {	
	int len = 0;
	SegNode *start_node, *end_node;
	const size_t kend = nodeSet.size();
	for (size_t i = 0; i < kend; i++) {
		start_node = nodeSet[i];
		// j=i+1 cause nodeSet sorted, so cur  node only connect with the node that behind it
		for (size_t j = i+1; j < kend; j++) {
			end_node = nodeSet[j];

			len = (end_node->startDtUtc - start_node->endDtUtc) / 60;
			//! if duration > maxTransMin, means no more end_node could connect with start_node cause nodeSet sorted
			// 时间间隔 > maxTransMin时，说明已经到达了start_node所能连接的最远的end_node
			if (len > _rules->horizon_rules->maxTransMin) {
				break;
			}

			if(start_node->arvStation == end_node->depStation
				&& len > 0
				&& (isSameFleetConnectble(*(start_node->optSegment), *(end_node->optSegment))
					|| isDiffFleetConnectble(*(start_node->optSegment), *(end_node->optSegment), _rules->horizon_rules->minTransMin))
				) {
				SegArc* seg_arc = new SegArc(start_node, end_node);
				seg_arc->setArcLen(len);
				seg_arc->setArcType(ArcType::connected);
								
				start_node->outArcSet.emplace_back(seg_arc);
				end_node->inArcSet.emplace_back(seg_arc);				
				arcSet.emplace_back(seg_arc);			
			}
		}
	}
}
bool SegNetwork::isSameFleetConnectble(const Opt_Segment& seg1, const Opt_Segment& seg2) {
	return seg1.getFleet() == seg2.getFleet();
}
bool SegNetwork::isDiffFleetConnectble(const Opt_Segment& seg1, const Opt_Segment& seg2, const int kMinTransMint) {
	return (seg1.getTailNum() != seg2.getTailNum()) 
		&& (seg2.getEndTimeUtcSch() - seg1.getStartTimeUtcSch() >= kMinTransMint * 60);
}
void SegNetwork::createBaseArcs() {
	for (auto& basenode : _base_nodeset) {
		for (auto& segnode : nodeSet) {
			if (basenode->arvStation == segnode->depStation
				&& basenode->nodeType == NodeType::baseO) {
				SegArc* out_arc = new SegArc(basenode, segnode);
				out_arc->setArcType(ArcType::baseOut);
				out_arc->setArcLen(0);
				
				basenode->outArcSet.emplace_back(out_arc);
				segnode->inArcSet.emplace_back(out_arc);
				arcSet.emplace_back(out_arc);
			}
			else if (basenode->depStation == segnode->arvStation
				&& basenode->nodeType == NodeType::baseD) {
				SegArc* in_arc = new SegArc(segnode, basenode);
				in_arc->setArcType(ArcType::baseIn);
				in_arc->setArcLen(0);

				segnode->outArcSet.emplace_back(in_arc);
				basenode->inArcSet.emplace_back(in_arc);				
				arcSet.emplace_back(in_arc);
			}
		}
	}
}

void SegNetwork::addLongArcs() {
	// 1.outdegree little
	SegNode *cur, *next;	
	int len = 0;
	const size_t end = nodeSet.size();
	for (size_t i = 0; i < end; i++) {
		cur = nodeSet[i];
		if (cur->outArcSet.size() > 1) {
			continue;
		}
		
		for (size_t j = i+1; j < end; j++) {
			next = nodeSet[j];

			len = (next->startDtUtc - cur->endDtUtc) / 60;
			if (len > _rules->horizon_rules->maxOutRestMin) {
				break;
			}
			//! else if len <= maxOutRestMin
			if (next->depStation == cur->arvStation
				&& _rules->horizon_rules->maxTransMin < len) {
				SegArc* seg_arc = new SegArc(cur, next);
				seg_arc->setArcLen(len);
				seg_arc->setArcType(ArcType::connected);

				cur->outArcSet.emplace_back(seg_arc);
				next->inArcSet.emplace_back(seg_arc);
				arcSet.emplace_back(seg_arc);
			}

		}
	}

	// 2.indgree little
	std::sort(nodeSet.begin(), nodeSet.end(),
		[](const SegNode *a, const SegNode *b) { return a->endDtUtc < b->endDtUtc; });
	SegNode* pre;
	for (size_t i = 0; i < end; i++) {
		cur = nodeSet[i];
		if (cur->inArcSet.size() > 1) {
			continue;
		}

		for (size_t j = i; j > 0; j--) {
			pre = nodeSet[j];

			len = (cur->startDtUtc - pre->endDtUtc) / 60;
			if (len > _rules->horizon_rules->maxOutRestMin) {
				break;
			}
			if (pre->arvStation == cur->depStation
				&& _rules->horizon_rules->maxTransMin < len) {
				SegArc* seg_arc = new SegArc(pre, cur);
				seg_arc->setArcLen(len);
				seg_arc->setArcType(ArcType::connected);

				pre->outArcSet.emplace_back(seg_arc);
				cur->inArcSet.emplace_back(seg_arc);
				arcSet.emplace_back(seg_arc);
			}
		}
	}
}

void SegNetwork::removeIsolatedNode() {
	//首先要找到计划周期的第一天和最后一天
	//因为这两天的node可以没有入弧/出弧
	std::ofstream isolatedNodeFile;
	isolatedNodeFile.open("isolated_flights_in_network_output.csv", std::ios::out);

	sortNodeSet();
	std::string first_date = nodeSet.front()->optSegment->getDate();
	std::string last_date = nodeSet.back()->optSegment->getDate();
	//find start and end copy index
	std::vector<SegNode*>::iterator start_it = nodeSet.begin();
	std::vector<SegNode*>::iterator end_it = nodeSet.end();
	for (; start_it != end_it; start_it++) {
		if ((*start_it)->optSegment->getDate() != first_date) {
			break;
		}
	}
	for (--end_it; end_it != nodeSet.begin(); end_it--) {
		if ((*end_it)->optSegment->getDate() != last_date) {
			break;
		}
	}
	//copy node in [start_it, end_it+1)
	std::vector<SegNode*> middle_node_set(start_it, end_it+1);
	nodeSet.erase(start_it, end_it + 1);
	
	flightParser flt_csv_parser;
	std::vector<string> headers = flt_csv_parser.getDefaultHeaders();
	for (const auto& col : headers) {
		isolatedNodeFile << col << ",";
	}
	isolatedNodeFile << "\n";

	size_t begin = 0;
	SegNode* cur;
	bool finished;
	do {
		finished = true;
		for (size_t i = begin; i < middle_node_set.size(); i++) {
			cur = middle_node_set[i];


			if (cur->optSegment->getDBId() == 54265954 || cur->optSegment->getDBId() == 54265916) {
				int ttt = 0;
			}



			if (cur->outArcSet.size() == 0) {
				if (cur->nodeType == NodeType::seg) {
					isolatedNodeFile << flt_csv_parser.toCsv(headers, &cur->optSegment->getSegment());
				}
				
				// 对出弧为0的点和与其连接的点，删掉之间的弧
				for (auto arc = cur->inArcSet.begin(); arc != cur->inArcSet.end();) {
					SegArcSet& out_arc_vec = (*arc)->startNode->outArcSet;
					for (auto outarc = out_arc_vec.begin(); outarc != out_arc_vec.end();) {
						if (*outarc == *arc) {
							out_arc_vec.erase(outarc);
							break;
						}
						++outarc;
					}
					auto pos = std::find(arcSet.begin(), arcSet.end(), *arc);
					arcSet.erase(pos);//从弧集中也得删除
					arc = cur->inArcSet.erase(arc);
				}
				middle_node_set.erase(middle_node_set.begin() + i--);
				finished = false;
			}
			else if (cur->inArcSet.size() == 0) {
				if (cur->nodeType == NodeType::seg) {
					isolatedNodeFile << flt_csv_parser.toCsv(headers, &cur->optSegment->getSegment());
				}
				
				for (auto arc = cur->outArcSet.begin(); arc != cur->outArcSet.end();) {
					SegArcSet& in_arc_vec = (*arc)->endNode->inArcSet;
					for (auto inarc = in_arc_vec.begin(); inarc != in_arc_vec.end();) {
						if (*inarc == *arc) {
							in_arc_vec.erase(inarc);
							break;
						}
						++inarc;
					}
					auto pos = std::find(arcSet.begin(), arcSet.end(), *arc);
					arcSet.erase(pos);
					arc = cur->outArcSet.erase(arc);
				}
				middle_node_set.erase(middle_node_set.begin() + i--);
				finished = false;
			}
		}

	} while (!finished);
	// 复位
	nodeSet.insert(nodeSet.end(), middle_node_set.begin(), middle_node_set.end());
	sortNodeSet();
	isolatedNodeFile.close();
}