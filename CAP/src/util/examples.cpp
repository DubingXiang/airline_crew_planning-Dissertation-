#include "examples.h"
#include "../structures/generic/csvClassesTransOptimizable.h"


namespace util 
{
	std::set<std::string> Example::randomCreatePlateauAirptSet(const std::vector<std::string>& airportSet,
		const int nbOfPlateauAirport) {
		size_t size = airportSet.size();
		int rand_index = 0;
		std::set<std::string> plateau_airports;
		for (size_t i = 0; i < nbOfPlateauAirport; i++) {
			rand_index = rand() % size + 1;
			plateau_airports.insert(airportSet[rand_index]);
		}

		return plateau_airports;
	}
	void Example::randomSetCrewSkillls(std::vector<Opt_CREW*>* optCrewSet, 
		const std::set<defines::Skill>& skillSet, 
		const double percent) {
		size_t size = optCrewSet->size();
		int nb_skill_crew = size * percent;		
		for (const auto& skill : skillSet) {			
			for (int i = 0; i < nb_skill_crew; ++i) {
				int rand_index = rand() % size + 1;
				(*optCrewSet)[rand_index]->getSkillMap()[skill] = 1;								
			}
		}
	}
	void Example::randomSetRankCombination(CrewRules* rules) {
		printf("*****************set rank composition(only by decending permutation)*****************\n");
		std::stringstream combi;
		std::string compo_name;
	#pragma region //搭配组合的一个排列（按字典序）	
		std::vector<std::string> CAP_permus2;
		std::vector<std::string> FO_permus2;
		/*1 1CAP-{1FO/2FO}*/
		//1.1 1CAP1FO
		compo_name = "1CAP1FO";
		auto set1 = &CAP_positions;
		auto set2 = &FO_positions;
		for (const auto& pos1 : *set1) {
			for (const auto& pos2 : *set2) {
				if (pos1.find("K1") != std::string::npos && pos2.find("F6") != std::string::npos) {
					continue;
				}
				combi.str("");
				combi << pos1 << "-";
				combi << pos2;				
				rules->compo_sequences_map[compo_name].emplace_back(combi.str());
			}
		}

		//1.2 1CAP2FO
		//得到2FO的所有组合，每个组合用一个排列表示即可
		for (int i = 0; i < FO_positions.size(); i++) {
			for (int j = i; j < FO_positions.size(); j++) {
				combi.str("");
				combi << FO_positions[i] << "-";
				combi << FO_positions[j];
				FO_permus2.emplace_back(combi.str());
			}
		}
		//1CAP与2FO排列
		compo_name = "1CAP2FO";
		set1 = &CAP_positions;
		set2 = &FO_permus2;
		for (const auto& pos1 : *set1) {
			for (const auto& pos2 : *set2) {
				combi.str("");
				combi << pos1 << "-";
				combi << pos2;
				rules->compo_sequences_map[compo_name].emplace_back(combi.str());
			}
		}
		/*2. 2CAP-{1FO/2FO}*/
		for (int i = 0; i < CAP_positions.size(); i++) {
			for (int j = i; j < CAP_positions.size(); j++) {
				if (CAP_positions[i] == "C4" && CAP_positions[j] == "T3") {
					continue;
				}
				combi.str("");
				combi << CAP_positions[i] << "-";
				combi << CAP_positions[j];
				CAP_permus2.emplace_back(combi.str());
			}
		}
		//2CAP1FO
		compo_name = "2CAP1FO";
		set1 = &CAP_permus2;
		set2 = &FO_positions;
		for (const auto& pos1 : *set1) {
			for (const auto& pos2 : *set2) {
				if (pos1.find("C4") != std::string::npos && pos2.find("F6") != std::string::npos) {
					continue;
				}
				combi.str("");
				combi << pos1 << "-";
				combi << pos2;				
				rules->compo_sequences_map[compo_name].emplace_back(combi.str());
			}
		}
		//2FO...

		/*3. 3CAP1FO*/
		std::vector<std::string> CAP_permus3;
		set1 = &CAP_permus2;
		set2 = &CAP_positions;
		for (const auto& pos1 : *set1) {
			for (const auto& pos2 : *set2) {
				combi.str("");
				combi << pos1 << "-";
				combi << pos2;
				CAP_permus3.emplace_back(combi.str());
			}
		}
		compo_name = "3CAP1FO";
		set1 = &CAP_permus3;
		set2 = &FO_positions;
		for (const auto& pos1 : *set1) {
			for (const auto& pos2 : *set2) {
				combi.str("");
				combi << pos1 << "-";
				combi << pos2;
				rules->compo_sequences_map[compo_name].emplace_back(combi.str());
			}
		}

	#pragma endregion
	}
}
