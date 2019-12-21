

整个程序要用到的主要基础集合：
SegmentSet, CrewSet, Opt_SegmentSet, Opt_CREWSet
SegNodeSet, SegArcSet


class Optimizer{
	function{
		loadData();
		loadPenaltySetting();
		loadCrewRules();
		
	}
	member{
		InputHandler _inputHandler; //
		CrewRules* _rules;
		const Penalty* _penalty;
	
		std::vector<Opt_Segment*> _optSegSet;
		std::vector<Opt_CREW*> _optCrewSet;

		SegNetwork* _segNet;
		CrewNetwork* _crewNet;	

		/*for calculate*/
		std::map<std::string, SegNodeSet> _day_segnode_map;
		std::map<time_t, SegNodeSet> _daytime_segnode_map;
	
		time_t _begining_plan;
		const int _SECONDS_ONE_DAY = 24 * 3600; //24 hour * 3600 secends
		SegNodeSet* _cur_day_segnode_set;


		std::map<std::string, std::vector<CrewNode*>> _pos_crewnodes; //position-vector<CrewNode*>
		
	}
	
}
