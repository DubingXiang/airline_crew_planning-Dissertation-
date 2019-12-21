#include "problem.h"

#include "../../csv_impl.h";
#include "../../Segment_mine.h"
#include "../../crewDB_mine.h"
#include "../../UtilFunc.h"


#include "../structures/generic/csvClassesTransOptimizable.h"




void Concretizer::concretizeAll(std::map<std::string, std::vector<void*>>& dataSet,
	const std::vector<std::string>& objNameSet) {
	for (const auto& obj_name : objNameSet) {
		if (obj_name == "Flight") { concretizeFlight(dataSet[obj_name], &_segSet); }
		else if (obj_name == "Crew") { concretizeCrew(dataSet[obj_name], &_crewSet); }
		else if (obj_name == "Base") { concretizeBase(dataSet[obj_name], &_baseSet); }
		else if (obj_name == "CrewBase") { concretizeCrewBase(dataSet[obj_name], &_crew_baseSet); }
		else if (obj_name == "CrewRank") { concretizeCrewRank(dataSet[obj_name], &_crew_rankSet); }
		else if (obj_name == "FlightComposition") { concretizeFlightComposition(dataSet[obj_name], &_fltCompositionSet); }
		else if (obj_name == "Composition") { concretizeComposition(dataSet[obj_name], &_compositionSet); }
		else { std::cerr << "ERROR: invalid obj data name" << obj_name << std::endl; }
	}
}
void 
Concretizer::concretizeFlight(const std::vector<void*>& objSet, std::vector<Segment*>* segmentSet) {	
	segmentSet->resize(objSet.size());
	for (int i = 0; i < objSet.size(); i++) {
		(*segmentSet)[i] = (Segment*)objSet[i];
	}
}
void 
Concretizer::concretizeCrew(const std::vector<void*>& objSet, std::vector<CREW*>* crewSet) {
	crewSet->resize(objSet.size());
	for (int i = 0; i < objSet.size(); i++) {
		(*crewSet)[i] = (CREW*)objSet[i];
	}
}
void 
Concretizer::concretizeBase(const std::vector<void*>& objSet, std::vector<BASE*>* baseSet) {
	baseSet->resize(objSet.size());
	for (int i = 0; i < objSet.size(); i++) {
		(*baseSet)[i] = (BASE*)objSet[i];
	}
}
void Concretizer::concretizeCrewBase(const std::vector<void*>& objSet, 
	std::vector<CREW_BASE*>* crewBaseSet) {
	crewBaseSet->resize(objSet.size());
	for (int i = 0; i < objSet.size(); i++) {
		(*crewBaseSet)[i] = (CREW_BASE*)objSet[i];
	}
}
void
Concretizer::concretizeCrewRank(const std::vector<void*>& objSet, 
	std::vector<CREW_RANK*>* crewRankSet) {
	crewRankSet->resize(objSet.size());
	for (int i = 0; i < objSet.size(); i++) {
		(*crewRankSet)[i] = (CREW_RANK*)objSet[i];
	}
}
void 
Concretizer::concretizeFlightComposition(const std::vector<void*>& objSet,
	std::vector<csvActivityComposition*>* fltCompositionSet) {
	fltCompositionSet->resize(objSet.size());
	for (int i = 0; i < objSet.size(); i++) {
		(*fltCompositionSet)[i] = (csvActivityComposition*)objSet[i];
	}
}
void 
Concretizer::concretizeComposition(const std::vector<void*>& objSet,
	std::vector<csvComposition*>* compositionSet) {
	compositionSet->resize(objSet.size());
	for (int i = 0; i < objSet.size(); i++) {
		(*compositionSet)[i] = (csvComposition*)objSet[i];
	}
}

void Concretizer::clear() {
	_segSet.clear();
	_baseSet.clear();
	_crewSet.clear();
	_crew_rankSet.clear();
	_crew_baseSet.clear();
	_fltCompositionSet.clear();
	_compositionSet.clear();	
}

Problem::ProblemBuilder::ProblemBuilder(Concretizer concretizer) {
	_segSet = concretizer._segSet;
	_crewSet = concretizer._crewSet;
	_baseSet = concretizer._baseSet;
	_crew_baseSet = concretizer._crew_baseSet;
	_crew_rankSet = concretizer._crew_rankSet;
	_fltCompositionSet = concretizer._fltCompositionSet;
	_compositionSet = concretizer._compositionSet;
}

Problem* Problem::ProblemBuilder::build() {
	sortCrewRank();
	
	createOptSegments();
	matchOptSegmentAndComposition();
	setRankToNumMapOfOptSegment();

	createOptCrews();
	matchOptCrewAndRankAry();
	matchOptCrewAndBaseAry();

	return new Problem(*this);
}


void Problem::ProblemBuilder::loadAllSkills() {
	_skillSet.insert("PlateauAirport");
}

void Problem::ProblemBuilder::createOptSegments() {
	for (const auto& seg : _segSet) {
		Opt_Segment* opt_seg = new Opt_Segment(seg);
		_optSegSet.emplace_back(opt_seg);
	}
}
void Problem::ProblemBuilder::createOptCrews() {
	for (const auto& crew : _crewSet) {
		Opt_CREW* opt_crew = new Opt_CREW(crew);
		_optCrewSet.emplace_back(opt_crew);
	}
}

void Problem::ProblemBuilder::matchOptSegmentAndComposition() {
	for (auto& seg : _optSegSet) {
		for (const auto& flt_compo : _fltCompositionSet) {
			if (flt_compo->activityId == seg->getDBId()) {
				for (const auto& compo : _compositionSet) {
					if (compo->compId == flt_compo->compId) {
						seg->setCompositions(flt_compo, compo);
						break;
					}
				}
				break;
			}
		}
	}
}
void Problem::ProblemBuilder::setRankToNumMapOfOptSegment() {
	for (const auto& seg : _optSegSet) {
		seg->setRankToNumMap();
	}
}


void Problem::ProblemBuilder::matchOptCrewAndRankAry() {
	std::map<string, CrewRankAry*> id_rank;

	for (const auto& crewrank : _crew_rankSet) {
		if (id_rank.find(crewrank->idCrew) == id_rank.end()) {
			id_rank[crewrank->idCrew] = new CrewRankAry();
		}
		id_rank[crewrank->idCrew]->emplace_back(crewrank);
	}

	for (auto& crew : _optCrewSet) {
		crew->setCrewRankAry(id_rank[crew->getIdCrew()]);
	}
}
void Problem::ProblemBuilder::matchOptCrewAndBaseAry() {
	std::map<string, CrewBaseAry*> id_base;
	for (auto& base : _crew_baseSet) {
		if (id_base.find(base->idCrew) == id_base.end()) {
			id_base[base->idCrew] = new CrewBaseAry();
		}
		id_base[base->idCrew]->emplace_back(base);
	}
	for (auto& crew : _optCrewSet) {
		crew->setCrewBaseAry(id_base[crew->getIdCrew()]);
	}
}
void Problem::ProblemBuilder::initializeCrewSkills() {
	for (auto& optcrew : _optCrewSet) {
		_ASSERT(optcrew->getSkillMap().empty()); // TODO:test function "_ASSERT"
		optcrew->getSkillMap().clear();
	}
}

void Problem::ProblemBuilder::setIndexOfCrew() {
	for (size_t i = 0; i < _optCrewSet.size(); i++) {
		_optCrewSet[i]->setIndex(i);
	}
}

void Problem::ProblemBuilder::sortCrewRank() {
	std::sort(_crew_rankSet.begin(), _crew_rankSet.end(),
		[](const CREW_RANK *a, const CREW_RANK *b) {return a->effUtc > b->effUtc; });

}
void Problem::ProblemBuilder::setCrewLatestRankAndPosition() {
	for (auto& crew : _optCrewSet) {
		crew->setCurRank();
		crew->setCurPosition();
	}
}

void Problem::ProblemBuilder::setAirportSet() {
	for (const auto& flt : _segSet) {
		if (std::find(_airportSet.begin(), _airportSet.end(), flt->getDepStation()) == _airportSet.end()) {
			_airportSet.emplace_back(flt->getDepStation());
		}
		if (std::find(_airportSet.begin(), _airportSet.end(), flt->getArrStation()) == _airportSet.end()) {
			_airportSet.emplace_back(flt->getArrStation());
		}
	}
}


Problem::Problem(const ProblemBuilder& builder) {
	this->_segSet = builder._segSet;
	this->_crewSet = builder._crewSet;
	this->_baseSet = builder._baseSet;
	this->_crew_baseSet = builder._crew_baseSet;
	this->_crew_rankSet = builder._crew_rankSet;
	this->_fltCompositionSet = builder._fltCompositionSet;
	this->_compositionSet = builder._compositionSet;

	this->_skillSet = builder._skillSet;
	this->_airportSet = builder._airportSet;

	this->_optSegSet = builder._optSegSet;
	this->_optCrewSet = builder._optCrewSet;

	logger->info("setup problem:{0}", this);
}
