#include "csvClassesTransOptimizable.h"

#include "..\..\..\crewDB_mine.h"
#include "..\..\..\Segment_mine.h"

#include <cctype>

Opt_CREW::Opt_CREW(CREW* crew) {
	_crew = crew;

	workStatus = new CrewStatus();
	
	_idCrew = crew->idCrew;
	_division = crew->division;
	_gender = crew->gender;
	_nationality = crew->nationality;
}
void Opt_CREW::setCrewRankAry(CrewRankAry* rankAry) {
	_rankAry = rankAry;
}
void Opt_CREW::setCrewBaseAry(CrewBaseAry* baseAry) {
	_baseAry = baseAry;
}

//void Opt_CREW::setSkills(std::map<std::string, int>& skills) {
//	SkillSet = skills;
//}

void Opt_CREW::setCurRank() {
	_curRank = _rankAry->front()->rank;
}
void Opt_CREW::setCurPosition() {
	_curPosition = _rankAry->front()->position;
}


Opt_Segment::Opt_Segment(Segment* seg) {
	_seg = seg;
	
	_id = seg->getDBId();
	_fltDt = seg->getDate();
	_fltNum = seg->getFlightNumber();
	_depArp = seg->getDepStation();
	_arvArp = seg->getArrStation();
	_schBlkMin = seg->getBlkMinutes();
	_schDepDtUtc = seg->getStartTimeUtcSch();
	_schDepDtLoc = seg->getStartTimeLocSch();
	_schArvDtUtc = seg->getEndTimeUtcSch();
	_schArvDtLoc = seg->getEndTimeLocSch();
	_segType = seg->getDomIntType();
	_fleet = seg->getFleetCD();
	_tailNumber = seg->getTailNum();
	
	
	

	_assigned = false;
}
void Opt_Segment::setCompositions(csvActivityComposition* fltCompo, csvComposition* composition) {
	_fltCompo = fltCompo;
	_composition = composition;
}

void Opt_Segment::setRankToNumMap() {
	std::string compo_name = this->_composition->nameDesc;
	//根据配比得到具体rank所需人数
	size_t len = compo_name.size();
	std::vector<int> rank_start_end_indexs;	// 子字符串，即rank的起始index为一对	
	for (size_t pos = 0; pos != len; pos++) {
		while (pos != len && '0' <= compo_name[pos] && compo_name[pos] <= '9') {
			++pos;
		}
		rank_start_end_indexs.push_back(pos);
		while (pos != len && ('0' >= compo_name[pos] || compo_name[pos] >= '9')) {
			++pos;
		}
		rank_start_end_indexs.push_back(--pos);
	}

	std::vector<int> nums;
	std::vector<std::string> ranks;
	size_t pos = 0;
	size_t s;
	size_t e;
	for (size_t i = 0; i < rank_start_end_indexs.size(); i++) {
		std::string num_str = compo_name.substr(pos, rank_start_end_indexs[i] - pos);
		nums.push_back(std::stoi(num_str));
		s = rank_start_end_indexs[i];
		e = rank_start_end_indexs[++i];
		std::string rank = compo_name.substr(s, e - s + 1);
		ranks.push_back(rank);
		pos = rank_start_end_indexs[i] + 1;
		_rank_to_num[rank] = std::stoi(num_str);
	}

}
//int Opt_Segment::getNumOfRank(const std::string& rank) { return _rank_to_num[rank]; }

void Opt_Segment::setAssigned(bool flag) {
	_assigned = flag;
}
bool Opt_Segment::getAssigned() {
	return _assigned;
}


long long Opt_Segment::getDBId()  const { return _id; }
std::string Opt_Segment::getDate()  const { return _fltDt; }
std::string Opt_Segment::getFlightNumber() const { return _fltNum; }
std::string Opt_Segment::getDepStation()  const { return _depArp; }
std::string Opt_Segment::getArrStation()  const { return _arvArp; }
int Opt_Segment::getBlkMinutes()  const { return _schBlkMin; }
time_t Opt_Segment::getStartTimeUtcSch() const { return _schDepDtUtc; }
time_t Opt_Segment::getStartTimeLocSch()  const { return _schDepDtLoc; }
time_t Opt_Segment::getEndTimeUtcSch()  const { return _schArvDtUtc; }
time_t Opt_Segment::getEndTimeLocSch()  const { return _schArvDtLoc; }
std::string Opt_Segment::getDomIntType()  const { return _segType; }
std::string Opt_Segment::getFleet()  const { return _fleet; }
std::string Opt_Segment::getTailNum()  const { return _tailNumber; }


void CrewStatus::setInitStatus(const time_t initialTime, const std::string& initialStation) {
	endDtLoc = initialTime;
	restStation = initialStation;

	accumuFlyMin = 0;
	accumuCreditMin = 0;
	dayoffMin = 0;

	sevenDayTotalFlyMint = 0;
	dateLocFlyBegining = initialTime;
	
	wholePlanTotalFlyMint = 0;
	wholePlanTotalCreditMint = 0;

	_assigned = false;
}

bool CrewStatus::getDayoff() {
	return _inDayoff;
}
void CrewStatus::setDayoff(bool flag) {
	_inDayoff = flag;
}

bool CrewStatus::getAssigned() {
	return _assigned;
}
void CrewStatus::setAssigned(bool flag) {
	_assigned = flag;
}


void CrewStatus::addDuty(Path* duty) {
	_creditedDutySet.emplace_back(duty);
}
const std::vector<Path*>& CrewStatus::getCreditDutySet() const {
	return _creditedDutySet;
}