#include "crewgroup_searcher.h"
#include "../structures/generic/csvClassesTransOptimizable.h"
#include "../structures/crew_rules.h"

#include "../structures/path/crew_path.h"

using namespace Network;

/*debug*/
const char* DEBUG_FILE_crew_group_searching = "../data/output/crew_group_searching.txt";
static std::ofstream DEBUG_outf;

/*end! debug*/

bool price_cmp(const CrewGroup* a, const CrewGroup* b) {
	return a->getCost() > b->getCost();
}

void GroupSearcher::init(CrewNetwork& crewNet, const CrewRules& rules) {
	_net = &crewNet;
	_s = crewNet.resource;
	_t = crewNet.sink;
	_rules = &rules;


	//added 8-13-2019
	for (size_t i = 2; i < _net->nodeSet.size(); i++) {
		_rank_nodes[_net->nodeSet[i]->optCrew->getCurRank()].emplace_back(_net->nodeSet[i]);
	}

};

void GroupSearcher::search() {
	_crewgroup_heap.clear();

	//searchByTopo();	
	//searchByTopo_test(); //tested
	//searchByBackpack(); //changed 8-14-2019 这个函数基本只会找到3个人的组合，且一般为2CAP1FO
	
	searchAccordingCompoMode();

	std::cout << "find groups: " << _crewgroup_heap.size() << "\n";

	int i = 0;
	for (auto group : _crewgroup_heap) {		
		group->setCrewIndexSet();
		group->setCrewGroup();				
		group->setBasicProperties();
	}

	//check group是否有重复的
	/*CrewGroup* group1;
	CrewGroup* group2;
	for (size_t i = 0; i < _crewgroup_heap.size(); i++) {
		group1 = _crewgroup_heap[i];
		for (size_t j = 0; j < _crewgroup_heap.size(); j++) {
			group2 = _crewgroup_heap[j];
			if (i != j) {
				long long hash1 = 1;
				for (auto node : group1->getNodeSequence()) {
					hash1 *= std::stoi(node->optCrew->getIdCrew());
				}
				hash1 *= 1000;
				long long hash2 = 1;
				for (auto node : group2->getNodeSequence()) {
					hash2 *= std::stoi(node->optCrew->getIdCrew());
				}
				hash2 *= 1000;

				if (hash1 == hash2) {
					std::cout << "same group " << i << " , " << j << "\n";
					_crewgroup_heap.erase(_crewgroup_heap.begin() + j);
					--j;					
				}
			}
		}
	}*/


}


bool GroupSearcher::checkCrewStatus(const Opt_CREW& optCrew) {
	auto status = optCrew.workStatus;
	return status->accumuFlyMin > _rules->maxWeekFlyMin || status->accumuCreditMin > _rules->maxCreditMin;
}

bool GroupSearcher::searchByTopo() {
	if (_topo_order.empty()) {
		setTopoOrder();
	}
	_crewgroup_heap.reserve(20);
	std::make_heap(_crewgroup_heap.begin(), _crewgroup_heap.end());

	for (const auto& node : _net->nodeSet) {
		_cost_map[node] = _NEGATIVE_INF;
		_prev_map[node] = NULL;
		
		_node_labels[node] = {};
	}
	_cost_map[_s] = 0;
	_prev_map[_s] = _s;
	crewLabel* label_of_s = new crewLabel;
	label_of_s->cur_price = 0;
	label_of_s->cur_node_sequence.emplace_back(_s);
	_node_labels[_s].emplace_back(label_of_s);
	
	DEBUG_outf.open(DEBUG_FILE_crew_group_searching, std::ios::app);

	std::vector<Opt_CREW*> combi;
	CrewNode* cur;
	CrewNode* next;
	int i = _topo_order.size();
	while (!_topo_order.empty()) {
		cur = _topo_order.front(); 
		_topo_order.pop();
		
		--i;
		std::cout << "number of node still not started" << std::to_string(i) << "\n";
		DEBUG_outf << "number of node rest " << std::to_string(i) << " over: \n";
		if (i > 0 && i % 10 == 1) {
			DEBUG_outf.close();
			DEBUG_outf.open(DEBUG_FILE_crew_group_searching, std::ios::app);
		}
		
		
		if (cur->optCrew != NULL) {
			DEBUG_outf << "start cur: ";
			DEBUG_outf << cur->optCrew->getIdCrew() << " -> ";
		}
		else {
			DEBUG_outf << "start cur = ssssss ";
		}
		

		if (_cost_map[cur] != _NEGATIVE_INF) {
			for (auto& arc : cur->outArcSet) {
				next = arc->endNode;

				DEBUG_outf << next->optCrew->getIdCrew() << " -> ";

				if (next == _t) { 					
					for (const auto& label : _node_labels[cur]) {
						CrewGroup* group = new CrewGroup();
						group->getNodeSequence() = label->cur_node_sequence;
						group->computeCost();
						
						if (group->getCost() < _crewgroup_heap.front()->getCost()) {
							delete group;
							continue;
						}

						DEBUG_outf << "\n **********one negative-reduced-cost-Group**********\n";
						for (const auto& node : group->getNodeSequence()) {
							DEBUG_outf << node->optCrew->getIdCrew() << "--";
						}
						DEBUG_outf << "\n **********OVER one negative-reduced-cost-Group**********\n";

						_crewgroup_heap.emplace_back(group);
						std::push_heap(_crewgroup_heap.begin(), _crewgroup_heap.end());											
					}

					continue;
				}
				//next != t				
				for (crewLabel* cur_label : _node_labels[cur]) {					
					crewLabel* new_label = new crewLabel;
					//extend
					new_label->cur_node_sequence = cur_label->cur_node_sequence;
					new_label->cur_price = cur_label->cur_price;
					//update
					new_label->cur_node_sequence.emplace_back(next);
					new_label->cur_price += arc->endNode->price;

					combi.clear();
					for (auto& node : new_label->cur_node_sequence) {
						if (node != _s && node != _t) {
							combi.emplace_back(node->optCrew);
						}						
					}

					if (new_label->cur_node_sequence.size() > _max_num_path /*|| !_rules->isFasibleCombination(combi)*/) {
						delete new_label;
						continue;
					}

					if (_cost_map[next] < _cost_map[cur] + cur->price + next->price) {
						_cost_map[next] = _cost_map[cur] + cur->price + next->price;

						_node_labels[next].emplace_back(new_label);
					}
										
				}
								
			}			
			
			for (size_t i = 0; i < _node_labels[cur].size(); i++) {
				delete _node_labels[cur][i];
			}
			_node_labels[cur].clear();
			_node_labels[cur].shrink_to_fit();
		}
	}


	DEBUG_outf << "\n----------FINISHED ONE SEARCH----------\n";
	DEBUG_outf.close();
	return true;
}
bool GroupSearcher::searchByTopo_test() {
	if (_topo_order.empty()) {
		setTopoOrder();
	}
	_crewgroup_heap.reserve(20);
	CrewGroup* dummy_group = new CrewGroup();
	_crewgroup_heap.push_back(dummy_group);
	std::make_heap(_crewgroup_heap.begin(), _crewgroup_heap.end());

	for (const auto& node : _net->nodeSet) {
		_cost_map[node] = _NEGATIVE_INF;
		_prev_map[node] = NULL;

		_node_label[node] = NULL;
	}
	_cost_map[_s] = 0;
	_prev_map[_s] = _s;
	crewLabel* label_of_s = new crewLabel;
	label_of_s->cur_price = 0;
	label_of_s->cur_node_sequence.emplace_back(_s);
	_node_label[_s] = label_of_s;

	//DEBUG_outf.open(DEBUG_FILE_crew_group_searching, std::ios::app);

	std::vector<Opt_CREW*> combi;
	CrewNode* cur;
	CrewNode* next;
	int i = _topo_order.size();
	while (!_topo_order.empty()) {
		cur = _topo_order.front();
		_topo_order.pop();

		--i;
		std::cout << "number of node still not started" << std::to_string(i) << "\n";
		//DEBUG_outf << "number of node rest " << std::to_string(i) << " over: \n";
		/*if (i > 0 && i % 10 == 1) {
			DEBUG_outf.close();
			DEBUG_outf.open(DEBUG_FILE_crew_group_searching, std::ios::app);
		}*/

		/*if (cur->optCrew != NULL) {
			DEBUG_outf << "start cur: ";
			DEBUG_outf << cur->optCrew->getIdCrew() << " -> ";
		}
		else {
			DEBUG_outf << "start cur: ssssss ";
		}*/


		if (_cost_map[cur] != _NEGATIVE_INF) {
			for (auto& arc : cur->outArcSet) {
				next = arc->endNode;

				/*if (next->optCrew != NULL) {
					DEBUG_outf << next->optCrew->getIdCrew() << " -> ";
				}*/
				
				if (next == _t) {

					CrewGroup* group = new CrewGroup();

					group->getNodeSequence() = _node_label[cur]->cur_node_sequence;
					group->getNodeSequence().erase(group->getNodeSequence().begin());
					group->computeCost();

					if (group->getCost() < _crewgroup_heap.front()->getCost()) {
						delete group;
						continue;
					}

					/*DEBUG_outf << "\n **********one negative-reduced-cost-Group**********\n";
					for (const auto& node : group->getNodeSequence()) {
						DEBUG_outf << node->optCrew->getIdCrew() << "--";
					}
					DEBUG_outf << "\n **********OVER one negative-reduced-cost-Group**********\n";*/

					_crewgroup_heap.emplace_back(group);
					std::push_heap(_crewgroup_heap.begin(), _crewgroup_heap.end());
					
					continue;
				}
				//next != t				
				crewLabel* new_label = new crewLabel;
				//extend
				new_label->cur_node_sequence = _node_label[cur]->cur_node_sequence;
				new_label->cur_price = _node_label[cur]->cur_price;
				//update
				new_label->cur_node_sequence.emplace_back(next);
				new_label->cur_price += arc->endNode->price;

				combi.clear();
				for (auto& node : new_label->cur_node_sequence) {
					if (node != _s && node != _t) {
						combi.emplace_back(node->optCrew);
					}
				}
				if (new_label->cur_node_sequence.size() > _max_num_path || !_rules->isFasibleCombination(combi)) {
					delete new_label;
					continue;
				}

				if (_cost_map[next] < _cost_map[cur] /*+ cur->price*/ + next->price) {
					_cost_map[next] = _cost_map[cur] /*+ cur->price*/ + next->price;

					_node_label[next] = new_label;
				}
			}
			
			delete _node_label[cur];				
		}
	}
	
	//删除dummy_group
	for (auto it = _crewgroup_heap.rbegin(); it != _crewgroup_heap.rend(); it++) {
		if ((*it)->getNodeSequence().size() == 0) {
			_crewgroup_heap.erase((++it).base());
			break;
		}
	}
	
	/*DEBUG_outf << "\n----------FINISHED ONE SEARCH----------\n";
	DEBUG_outf.close();*/
	return true;
}
void GroupSearcher::setTopoOrder() {
	
	std::queue<CrewNode*> temp_queue;
	size_t count = 0;
	//initialize
	for (auto& node : _net->nodeSet) {
		count = node->inArcSet.size();
		_node_indegree[node] = count;
		
		if (count == 0) {
			temp_queue.push(node);
		}
	}
	
	count = 0;
	CrewNode* node;
	while (!temp_queue.empty()) {
		node = temp_queue.front(); temp_queue.pop();
		_topo_order.push(node);
		for (auto& arc : node->outArcSet) {
			if (--_node_indegree[arc->endNode] == 0) {
				temp_queue.push(arc->endNode);
			}
		}
		++count;
	}
	if (count != _net->nodeSet.size()) {
		throw "the network is cyclic!!";
	}	
}

void GroupSearcher::sort_rank_nodes() {
	for (auto& k_v : _rank_nodes) {
		std::sort(k_v.second.begin(), k_v.second.end(),
			[](const CrewNode* a, const CrewNode* b) {return a->price > b->price; });
	}
}
bool GroupSearcher::searchByBackpack() {	
	sort_rank_nodes();

	CrewNodeSet& cap_set = _rank_nodes["CAP"];
	CrewNodeSet& fo_set = _rank_nodes["FO"];	
	CrewNodeSet combine_set(cap_set);
	combine_set.insert(combine_set.end(), fo_set.begin(), fo_set.end());
	
	_crewgroup_heap.reserve(50);
	CrewGroup* dummy_group = new CrewGroup();
	_crewgroup_heap.push_back(dummy_group);
	std::make_heap(_crewgroup_heap.begin(), _crewgroup_heap.end(), price_cmp);

	for (const auto& node : _net->nodeSet) {
		_cost_map[node] = _NEGATIVE_INF;
		_prev_map[node] = NULL;

		_node_label[node] = NULL;
	}
	//! attention
	// 起点变成每个cap
	for (size_t i = 0; i < cap_set.size(); i++) {
		_cost_map[cap_set[i]];
		_prev_map[cap_set[i]] = cap_set[i];
		crewLabel* label_of_s = new crewLabel;
		label_of_s->cur_price = 0;
		label_of_s->cur_node_sequence.emplace_back(cap_set[i]);
		_node_label[cap_set[i]] = label_of_s;
	}

	std::vector<Opt_CREW*> combi;
	CrewNode* cur;
	CrewNode* next;

	for (size_t i = 0; i < cap_set.size(); i++) {
		cur = cap_set[i];
		for (size_t j = i + 1; j < combine_set.size(); j++) {
			next = combine_set[j];			
			
			if ((cur->optCrew->workStatus->restStation != next->optCrew->workStatus->restStation)
				&& (cur->optCrew->workStatus->restStation != "" && next->optCrew->workStatus->restStation != "")) {			
				continue;
			}

			crewLabel* new_label = new crewLabel;
			//extend
			new_label->cur_node_sequence = _node_label[cur]->cur_node_sequence;			
			new_label->cur_node_sequence.emplace_back(next);
			
			combi.clear();
			for (auto& node : new_label->cur_node_sequence) {
				if (node != _s && node != _t) {
					combi.emplace_back(node->optCrew);
				}
			}
			if (new_label->cur_node_sequence.size() >= _max_num_path || !_rules->isFasibleCombination(combi)) {
				delete new_label;
				continue;
			}
			bool relaxed = false;
			if (_cost_map[next] < _cost_map[cur] /*+ cur->price*/ + next->price) {
				_cost_map[next] = _cost_map[cur] /*+ cur->price*/ + next->price;

				_node_label[next] = new_label;
				relaxed = true;
			}

			if (next->optCrew->getCurRank() == "FO" && relaxed == true) {
				CrewGroup* group = new CrewGroup();

				group->getNodeSequence() = _node_label[next]->cur_node_sequence;
				group->computeCost();

				if (group->getCost() < _crewgroup_heap.front()->getCost()) {
					delete group;
					continue;
				}
				/*DEBUG_outf << "\n **********one negative-reduced-cost-Group**********\n";
				for (const auto& node : group->getNodeSequence()) {
					DEBUG_outf << node->optCrew->getIdCrew() << "--";
				}
				DEBUG_outf << "\n **********OVER one negative-reduced-cost-Group**********\n";*/
				_crewgroup_heap.emplace_back(group);
				std::push_heap(_crewgroup_heap.begin(), _crewgroup_heap.end());
				if (_crewgroup_heap.size() > 2) {
					std::pop_heap(_crewgroup_heap.begin(), _crewgroup_heap.end());
					_crewgroup_heap.pop_back();
				}

				continue;

			}

		}
		
		delete _node_label[cur];
	}

	
	/*DEBUG_outf << "\n----------FINISHED ONE SEARCH----------\n";
	DEBUG_outf.close();*/
	return true;
}

void GroupSearcher::searchAccordingCompoMode() {
	//sort_rank_nodes();
	for (auto& k_v : _rank_nodes) {
		std::sort(k_v.second.begin(), k_v.second.end(),
			[](const CrewNode* a, const CrewNode* b) {return a->price < b->price; }); //此处升序排序
	}
	CrewNodeSet& cap_set = _rank_nodes["CAP"];
	CrewNodeSet& fo_set = _rank_nodes["FO"];
	size_t cap_size = cap_set.size();
	size_t fo_size = fo_set.size();
	
	size_t max_number = 100; //每种配比，最多找max_number组
	std::vector<Opt_CREW*> combi;
	
	CrewNode* cap1;
	CrewNode* fo1;
	
	// TODO: 只需要一次初始化，新建为类成员存储
	std::vector<double> cap_price(cap_size);
	std::vector<double> fo_price(fo_size);
	for (const auto& cap : cap_set) {
		cap_price.push_back(cap->price);
	}
	for (const auto& fo : fo_set) {
		fo_price.push_back(fo->price);
	}

	// 1. 1CAP1FO
	// tuple (sum, (i, j)) 
	std::priority_queue<std::pair<double, std::pair<int, int>>> pq;
	std::set<std::pair<int, int> > my_set;
	pq.push(std::make_pair(cap_price[cap_size-1] + fo_price[fo_size-1],
						   std::make_pair(cap_size - 1, fo_size - 1)));
	

	my_set.insert(std::make_pair(cap_size - 1, fo_size - 1));
	//iter to k
	for (size_t count = 0; count < max_number; count++) {
		// tuple format (sum, (i, j)). 
		std::pair<double, std::pair<int, int>> temp;
		temp = pq.top();
		pq.pop();

		int i = temp.second.first;
		int j = temp.second.second;

		double sum = cap_price[i - 1] + fo_price[j];

		// insert (A[i - 1] + B[j], (i - 1, j))  
		// into max heap. 
		std::pair<int, int> temp1 = std::make_pair(i - 1, j);

		// insert only if the pair (i - 1, j) is  
		// not already present inside the map i.e. 
		// no repeating pair should be present inside  
		// the heap. 
		if (my_set.find(temp1) == my_set.end()) {
			pq.push(make_pair(sum, temp1));
			my_set.insert(temp1);
		}

		// insert (A[i] + B[j - 1], (i, j - 1))  
		// into max heap. 
		sum = cap_price[i] + fo_price[j - 1];
		temp1 = std::make_pair(i, j - 1);

		// insert only if the pair (i, j - 1) 
		// is not present inside the heap. 
		if (my_set.find(temp1) == my_set.end()) {
			pq.push(make_pair(sum, temp1));
			my_set.insert(temp1);
		}
	}

	// create group from my_set
	for (const auto& two_pair : my_set) {
		cap1 = cap_set[two_pair.first];
		fo1 = fo_set[two_pair.second];
		if ((cap1->optCrew->workStatus->restStation != fo1->optCrew->workStatus->restStation)
			&& (cap1->optCrew->workStatus->restStation != "" && fo1->optCrew->workStatus->restStation != "")) {
			continue;
		}		
		
		combi.clear();
		combi.emplace_back(cap1->optCrew);
		combi.emplace_back(fo1->optCrew);
		if (_rules->isFasibleCombination(combi)) {
			CrewGroup* group = new CrewGroup();
			group->getNodeSequence() = { cap1, fo1 };
			group->computeCost();

			_crewgroup_heap.emplace_back(group);
		}
	}

	// 2. 1CAP2FO
	// 利用1CAP1FO的结果，与1FO组合
	size_t count = 0;
	bool flag = false;
	CrewNode* fo2;
	for (const auto& two_pair : my_set) {
		if (flag == true) {
			break;
		}

		cap1 = cap_set[two_pair.first];
		fo1 = fo_set[two_pair.second];
		for (size_t k = 0; k < fo_size; k++) {
			if (k == two_pair.second) {
				continue;
			}
			fo2 = fo_set[k];
			if ((fo1->optCrew->workStatus->restStation != fo2->optCrew->workStatus->restStation)
				&& (fo1->optCrew->workStatus->restStation != "" && fo2->optCrew->workStatus->restStation != "")) {
				continue;
			}


			combi.clear();
			combi.emplace_back(cap1->optCrew);
			combi.emplace_back(fo1->optCrew);
			combi.emplace_back(fo2->optCrew);
			if (_rules->isFasibleCombination(combi)) {
				CrewGroup* group = new CrewGroup();
				group->getNodeSequence() = { cap1,fo1,fo2 };
				group->computeCost();
				
				_crewgroup_heap.emplace_back(group);

				if (++count >= max_number) {
					flag = true;
					break;
				}
			}
		}
	}
	fo2 = NULL;

	// 3. 2CAP1FO
	// 利用1CAP1FO的结果，与1CAP组合
	count = 0;
	flag = false;
	CrewNode* cap2;
	for (const auto& two_pair : my_set) {
		if (flag == true) {
			break;
		}

		cap1 = cap_set[two_pair.first];
		fo1 = fo_set[two_pair.second];
		for (size_t k = 0; k < cap_size; k++) {
			if (k == two_pair.first) {
				continue;
			}
			cap2 = cap_set[k];
			if ((cap1->optCrew->workStatus->restStation != cap2->optCrew->workStatus->restStation)
				&& (cap1->optCrew->workStatus->restStation != "" && cap2->optCrew->workStatus->restStation != "")) {
				continue;
			}

			combi.clear();
			combi.emplace_back(cap1->optCrew);
			combi.emplace_back(fo1->optCrew);
			combi.emplace_back(cap2->optCrew);
			if (_rules->isFasibleCombination(combi)) {
				CrewGroup* group = new CrewGroup();
				group->getNodeSequence() = { cap1,fo1,cap2 };
				group->computeCost();

				_crewgroup_heap.emplace_back(group);

				if (++count >= max_number) {
					flag = true;
					break;
				}
			}
		}
	}

}




bool GroupSearcher::searchBySPFA() {
	return true;
}

//bool PathSearcher::searchByDijk() {
//	_prior_queue.push(&prioNode(_s, 0));
//	//init
//	INode* cur;
//	INode* next;
//	while (!_prior_queue.empty()) {
//		cur = _prior_queue.top()->inode; _prior_queue.pop();
//		if (cur->visited) {
//			continue;
//		}
//		cur->visited = true;
//		for (auto& arc : cur->outArcSet) {
//			next = arc->endNode;
//			//因为求最长路
//			//所以此处松弛为：经过点cur到达next的cost > 到达next，当前的cost
//			if (!next->visited && _cost_map[next] < _cost_map[cur] + next->price) {
//				_cost_map[next] = _cost_map[cur] + next->price;
//												
//				_prev_map[next] = cur;
//
//				_prior_queue.push(&prioNode(next, _cost_map[next]));
//			}
//
//		}
//	}
//
//}


