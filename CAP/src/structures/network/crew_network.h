#pragma once
#ifndef CREW_NETWORK_H
#define CREW_NETWORK_H
#include "../../pch.h"
#include "INetwork.h"
#include "crew_node_arc.h"



class Opt_CREW;
class CrewRules;

namespace Network {	

	class CrewNetwork :public INetwork
	{
	public:
		CrewNetwork(std::vector<Opt_CREW*>* optCrewSet, CrewRules* rules) {
			_optCrewSet = optCrewSet;
			_rules = rules;

			resource = new CrewNode();
			sink = new CrewNode();
		}
		virtual ~CrewNetwork();
		virtual void createNetwork();
		//! 重建网络，每一天决策后，crew的所在地会变化，所以只有所在地相同的crew才能相连
		void rebuildNetwork();
		CrewNode* resource;
		CrewNode* sink;

		CrewNodeSet nodeSet;
		CrewArcSet arcSet;

	private:
		std::vector<Opt_CREW*>* _optCrewSet;
		CrewRules* _rules;

		std::map<std::string, std::vector<CrewNode*>> _pos_nodeset_map;
		void clusterCrewNode();

		void setOptCrewNodeSet();
		void createOptCrewNodes();

		void createOptCrewArcs();
		void createVirtualArcs();
	};
}

#endif // !CREW_NETWORK_H