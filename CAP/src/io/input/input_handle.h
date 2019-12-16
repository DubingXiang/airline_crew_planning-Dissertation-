#pragma once
#ifndef INPUT_HANDLE_H
#define INPUT_HANDLE_H
#include "../../pch.h"

class CREW;
class CREW_RANK;
class CREW_BASE;
class Segment;
class BASE;
struct csvActivityComposition;
struct csvComposition;
class CrewRules;

class Opt_CREW;
class Opt_Segment;


class InputHandler
{
public:	
	InputHandler() {}
	~InputHandler() {
		_segSet.clear();
		_baseSet.clear();
		_crewSet.clear();
		_crew_rankSet.clear();
		_crew_baseSet.clear();
		_fltCompositionSet.clear();
		_compositionSet.clear();
		_airportSet.clear();
	}

	void inputData(std::map<std::string, std::vector<void*>>& dataSet, const std::vector<std::string>& objNameSet);
	void loadCrewRules(const std::string& rulesFile);
	void loadPenalties(const std::string& penaltiesFile);

	void matchOptSegmentSet(/*std::vector<Segment*>& segSet, */std::vector<Opt_Segment*>* optSegSet);
	void matchOptCrewSet(/*std::vector<CREW*>& crewSet, */std::vector<Opt_CREW*>* optCrewSet);

	void matchOptSegmentAndComposition(std::vector<Opt_Segment*>* optSegSet
		/*std::vector<csvActivityComposition*>* fltCompoSet,
		std::vector<csvComposition*>* compoSet*/);
	//! added 20190822
	void setRankToNumMapOfOptSegment(std::vector<Opt_Segment*>* optSegSet);
	void matchOptCrewAndRank(std::vector<Opt_CREW*>* optCrewSet/*, std::vector<CREW_RANK*>* crewrankSet*/);
	void matchOptCrewAndBase(std::vector<Opt_CREW*>* optCrewSet/*, std::vector<CREW_BASE*>* crewbaseSet*/);
	
	void setIndexOfCrew(std::vector<Opt_CREW*>* optCrewSet);

	void sortCrewRank(/*std::vector<CREW_RANK*>* crewrankAry*/);

	void setAirportSet(/*const std::vector<Segment*>& fltSet*/);
	std::vector<std::string>& createSpecialArpSet();
		
	std::vector<BASE*>& getBaseSet() { return _baseSet; }
	//先考虑飞行员
	std::vector<Opt_CREW*>* getPilotSet(const std::vector<Opt_CREW*>& optCrewSet);

	void randomSetCrewSkills(std::vector<Opt_CREW*>* optCrewSet, double percent = 0.3);
	void setRankCombination(CrewRules* rules);
	//! 按配比记录对应的号位搭配，用组合的一个排列来代表这个组合
	void setRankCombination_OnePermutation(CrewRules* rules);
	
private:
	void typeTrans(std::vector<void*>& objSet, const std::string& objName, void* outObjSet);

	std::vector<Segment*> _segSet;
	std::vector<BASE*> _baseSet;
	std::vector<CREW*> _crewSet;
	std::vector<CREW_RANK*> _crew_rankSet;
	std::vector<CREW_BASE*> _crew_baseSet;
	std::vector<csvActivityComposition*> _fltCompositionSet;
	std::vector<csvComposition*> _compositionSet;

	//CrewRules* _rules;

	std::vector<std::string> _airportSet;
	std::vector<std::string> _special_airportSet;
};

#endif // !INPUT_HANDLE_H
