#include "segpath_searcher.h"
#include "../structures/crew_rules.h"

using namespace Network;
//SegPathSearcher::SegPathSearcher(SegNetwork& segNet, SegNode& s, CrewRules& rules) {
//	_net = &segNet;
//	_s = &s;
//	_rules = &rules;
//	for (const auto& arc : _net->arcSet) {
//		_visited_by_path[arc] = 0;
//	}
//}
void SegPathSearcher::init(SegNetwork& segNet, SegNode& s, const CrewRules& rules) {
	_net = &segNet;	
	_rules = &rules;
	_s = &s;

	for (const auto& arc : _net->arcSet) {
		_visited_by_path[arc] = 0;
	}
	for (auto& node : _net->nodeSet) {
		node->label = new Label(); //TODO:只需要对当天的新建就行了
		//node->label->preArc = node->inArcSet.front();

	}

	_s->visited = false;
	_s->label->preLabel = _s->label; //TODO: test
	for (auto& arc : _s->outArcSet) {
		arc->endNode->label->preLabel = _s->label;
		arc->endNode->label->preArc = arc;
	}
}
SegPathSearcher::~SegPathSearcher() {
	_net = NULL;
	_s = NULL;
	_rules = NULL;

	
}
void SegPathSearcher::clear() {
	for (auto node : _net->nodeSet) {
		delete node->label;
	}
}


void SegPathSearcher::reset(SegNode& s) {
	
}
void SegPathSearcher::search() {	
	//TODO: 还有网络中的，当前要做决策的天的点的预处理
	//initStartNode();

	_segpath_set.clear();

	enumerateByDFS();
	
}

void SegPathSearcher::initStartNode() {
	_s->visited = false;
	//_s->label = new Label();
	_s->label->preLabel = _s->label; //TODO: test
	for (auto& arc : _s->outArcSet) {
		arc->endNode->label->preLabel = _s->label;
		arc->endNode->label->preArc = arc;
	}
}

void SegPathSearcher::enumerateByDFS() {
	std::stack<SegNode*> node_stack;
	node_stack.push(_s);
	_s->visited = true;
	
	SegNode *next_node = NULL;	
	while (!node_stack.empty()) {
		SegNode *cur_node = node_stack.top();
		
		/*if (cur_node != _s) {
			std::cout << "cur node :" << cur_node->optSegment->getDBId() << ", type" << cur_node->nodeType << "\n";
		}*/
		
		auto out_arc_set = &cur_node->outArcSet;
		size_t arc_set_size = out_arc_set->size();		
		size_t i = 0;
		for (; i < arc_set_size; i++) {
			SegArc *cur_arc = (*out_arc_set)[i];
			next_node = cur_arc->endNode;
			
			/*if (next_node->optSegment != NULL) {
				std::cout << "next node :" << next_node->optSegment->getDBId() << ", type" << next_node->nodeType << "\n";
			}*/

			int visited = _visited_by_path[cur_arc];
			// next_node not visited and not been visited just recently
			if (!next_node->visited && visited == 0 && next_node->nodeType != NodeType::BASE_T) {
				extend(*cur_node->label, *cur_arc, next_node->label);
				updateLabel(*cur_node->label, *cur_arc, next_node->label);								
				node_stack.push(next_node);
				next_node->visited = true;
				break;
			}
		}
		if (i == arc_set_size) {
			for (i = 0; i < arc_set_size; i++) {				
				//next_node = cur_arc->endNode; //这句没意义吧
				_visited_by_path[(*out_arc_set)[i]] = 0;
			}
			cur_node->visited = false;
			_visited_by_path[cur_node->label->preArc] = 1;
			node_stack.pop();
			continue;
		}

		Label* end_label = next_node->label;
		_visited_by_path[end_label->preArc] = 1;
		//added 8-7-2019
		//prune dhd
		//1.dhd-dhd, cut, and don't create a new path
		if (next_node->nodeType == NodeType::DEADHEAD && cur_node->nodeType == NodeType::DEADHEAD) {
			next_node->visited = false;
			node_stack.pop();
		}
		//end! added		
		else if (isValidDuration(*end_label)) {			
			EventPath* new_path = backtrack(*end_label);
			new_path->init();
			_segpath_set.emplace_back(new_path);
			
			//added 8-7-2019
			//seg-dhd, cut, but creat a new path
			if (next_node->nodeType == NodeType::DEADHEAD && cur_node->nodeType == NodeType::SEGMENT) {
				next_node->visited = false;
				node_stack.pop();
			}
		}
		else { //  //if (isMetTermination(*end_label)) { //changed 8-7-2019 isMetTermination 应该正好是isValidDuration的反面，所以不需要再判断
			next_node->visited = false;
			node_stack.pop();
		}		
	}
}

void SegPathSearcher::extend(const Label& curLabel, const SegArc& curArc, Label* nextLabel) {
	nextLabel->accumuFlyMin = curLabel.accumuFlyMin + curArc.endNode->taskMin;
	nextLabel->accumuWorkMin = curArc.arcType != ArcType::BASE_LEVING
		? curLabel.accumuWorkMin + curArc.len + curArc.endNode->taskMin//equals flyMin + connect time(arc.len)
		: nextLabel->accumuFlyMin;		
}

//bool SegPathSearcher::isFeasible(const Label& curLabel, const SegArc& curArc, const Label& nextLabel) {
//	// 1.check dhd constraint
//	//if()
//	// 2.
//	return true;
//}

//bool SegPathSearcher::isValidDeahhead(Label& label) {
//	Label* temp_label = &label;
//	SegNode* end_node = temp_label->preArc->endNode;
//	
//	while (temp_label->preArc->startNode != _s) {
//		if (end_node->nodeType == NodeType::dhd) {
//			return false;
//		}
//		temp_label = temp_label->preLabel;
//		end_node = temp_label->preArc->endNode;
//	}
//
//	return true;
//}
void SegPathSearcher::updateLabel(Label& curLabel, SegArc& curArc, Label* nextLabel) {
	nextLabel->preLabel = &curLabel;
	nextLabel->preArc = &curArc;
}
bool SegPathSearcher::isValidDuration(const Label& curLabel) {
	if (curLabel.accumuFlyMin >= _rules->horizon_rules->minFlyMin && curLabel.accumuFlyMin <= _rules->horizon_rules->maxFlyMin
		&& curLabel.accumuWorkMin >= _rules->horizon_rules->minDutyMin && curLabel.accumuWorkMin <= _rules->horizon_rules->maxDutyMin) {
		return true;
	}

	return false;
}
bool SegPathSearcher::isMetTermination(const Label& curLabel) {
	return curLabel.accumuFlyMin > _rules->horizon_rules->maxFlyMin
		|| curLabel.accumuWorkMin > _rules->horizon_rules->maxDutyMin;
		//|| curLabel.preArc->endNode->nodeType == NodeType::dhd;	
}

EventPath* SegPathSearcher::backtrack(Label& endLabel) {
	EventPath* new_path = new EventPath();
	Label* label = &endLabel;
	SegArc* arc = endLabel.preArc;

	std::vector<SegNode*>& node_sequence = new_path->getNodeSequence();
	while (arc->startNode != _s) { // arc->startNode->nodeType!=NodeType::resource
		node_sequence.emplace_back(arc->endNode);
		
		label = label->preLabel;
		arc = label->preArc;
	}
	node_sequence.emplace_back(arc->endNode);
	std::reverse(node_sequence.begin(), node_sequence.end());

	return new_path;
}

