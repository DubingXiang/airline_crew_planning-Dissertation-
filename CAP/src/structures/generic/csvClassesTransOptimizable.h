/* transform input classes to the classes which used in the process of optimization
 * InputClasses corresponding to a optimizable classes named "Opt_InputClassName"
 * optimizable classes only include needed member of input classes
 */

#pragma once
#ifndef CSV_CLASSES_TRANS_OPTIMIZABLE
#define CSV_CLASSES_TRANS_OPTIMIZABLE
#include "../../pch.h"


class CREW;
class CREW_RANK;
class RANK;
class CREW_BASE;
class Segment;
class BASE;
struct csvActivityComposition;
struct csvComposition;

class CREW_RANK;
class CREW_BASE;
class CrewStatus;

typedef std::vector<CREW_RANK*> CrewRankAry;
typedef std::vector<CREW_BASE*> CrewBaseAry;


class Opt_CREW
{
public:
	Opt_CREW(CREW* crew);
	~Opt_CREW() {
		_crew = NULL;
		_rankAry = NULL;
		_baseAry = NULL;

		_skillMap.clear();
		_rankAry->clear();
		_baseAry->clear();

		delete workStatus;		
	}

	void setCrewRankAry(CrewRankAry* rankAry);
	void setCrewBaseAry(CrewBaseAry* baseAry);
	void setCurRank();
	void setCurPosition();	
	
	std::string& getIdCrew() { return _idCrew; }
	std::string& getDivision() { return _division; }
	std::string& getCurRank() { return _curRank; }
	std::string& getCurPosition() { return _curPosition; }
	std::map<std::string, int>& getSkillMap() { return _skillMap; }

	CrewRankAry& getRankAry() { return *_rankAry; }
	CrewBaseAry& getBaseAry() {return *_baseAry;	}

	void setDualPrice(double price) { _dual_price = price; }
	double getDualPrice() const { return _dual_price; }

	void setIndex(int index) { _index = index; }
	int getIndex() const { return _index; }

	CrewStatus* workStatus;

private:	
	CREW* _crew;
	CrewRankAry* _rankAry;
	CrewBaseAry* _baseAry;

	std::string _idCrew;
	std::string _division; //部门:飞行员；乘务员	
	std::string _curRank;
	std::string _curPosition;
	std::map<std::string, int> _skillMap;//特殊资质，此处先为可以飞特殊机场
	std::string _gender;
	std::string _nationality;

	double _dual_price;

	int _index; //crew在OptCrewSet中的下标

};

class Opt_Segment
{
public:
	Opt_Segment(Segment* seg);
	~Opt_Segment() {
		_seg = NULL;
		_fltCompo = NULL;
		_composition = NULL;
	}

	void setDualPrice(double price) { _dual_price = price; }
	double getDualPrice() const { return _dual_price; }

	void setIndex(int index) { _index = index; }
	int getIndex() const { return _index; }

	void setCompositions(csvActivityComposition* fltCompo, csvComposition* composition);
	csvActivityComposition* getFltComposition() const { return _fltCompo; }
	csvComposition* getCompositon()const { return _composition; }
	//! 
	void setRankToNumMap();
	std::map<std::string, int>& getRankToNumMap() { return _rank_to_num; };
	
	bool getAssigned();
	void setAssigned(bool flag);

	Segment& getSegment() const { return *_seg; }

	long long getDBId() const;
	std::string getDate() const;
	std::string getFlightNumber() const;
	std::string getDepStation() const;
	std::string getArrStation() const;
	int getBlkMinutes() const;
	time_t getStartTimeUtcSch() const;
	time_t getStartTimeLocSch() const;
	time_t getEndTimeUtcSch() const;
	time_t getEndTimeLocSch() const;
	std::string getDomIntType() const;
	std::string getFleet() const;
	std::string getTailNum() const;

private:
	Segment* _seg;

	long long _id;	
	std::string _fltDt;//航班所处date
	std::string	_fltNum;//航班号，实际机票上的航班号
	std::string _depArp;
	std::string _arvArp;	
	int	_schBlkMin;//计划航班飞行（block）时间，以min计
	time_t _schDepDtUtc;
	time_t _schDepDtLoc;
	time_t _schArvDtUtc;
	time_t _schArvDtLoc;	
	std::string _segType;//segment的类型：D-国内；I-国际；R-region，区域，比如台湾，香港。案例均为D
	std::string _fleet;
	std::string _tailNumber;//机尾号

	csvActivityComposition* _fltCompo;
	csvComposition* _composition;
	std::map<std::string, int> _rank_to_num; //该segment对应rank需要的人数

	bool _assigned;

	double _dual_price;
	int _index; //在OptSegSet中的下标
};


extern class Path;
class CrewStatus
{
public:
	CrewStatus() {};
	~CrewStatus() {
		_creditedDutySet.clear();
		_creditedDutySet.shrink_to_fit();
	}
	//
	void setInitStatus(const time_t initialTime, const std::string& initialStation);	

	bool getDayoff();
	//set _inDayoff to true
	void setDayoff(bool flag);	

	bool getAssigned();
	void setAssigned(bool flag);	
	//after assigned, _CreditedDutySet.push_back(param *duty)
	void addDuty(Path* duty);

	const std::vector<Path*>& getCreditDutySet() const;

	//reset every week
	int accumuFlyMin;
	//reset every day off
	//if it over 5 days, then must day off
	int accumuCreditMin;
	//最终day off的时长,[2 days, 3 days]
	int dayoffMin;
	//最后一个duty执行完的时间
	time_t endDtLoc;
	std::string restStation;

	//记录7天内的累计flymin
	int sevenDayTotalFlyMint;
	//为了记录7天内总飞时，
	//每7天刷新一次
	time_t dateLocFlyBegining;
	
	//统计指标

	//整个周期开始到当前的累计飞行时间
	int wholePlanTotalFlyMint;
	//整个周期开始执勤到当前的累计时间
	int wholePlanTotalCreditMint;	
	

private:

	bool _assigned;
	//当前处于day off状态	
	bool _inDayoff;
	//最终决定的担当的duty，会一直累加
	std::vector<Path*> _creditedDutySet;

};

#endif // !CSV_CLASSES_TRANS_OPTIMIZABLE