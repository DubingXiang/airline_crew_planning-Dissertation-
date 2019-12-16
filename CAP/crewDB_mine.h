#pragma once
#include <vector>
#include <map>
#include <string>
#include <time.h>
#ifndef CREWBD_MINE_H
#define CREWBD_MINE_H


class CREW_RANK;
class CREW_BASE;
typedef std::vector<CREW_RANK*> CrewRankAry;
typedef std::vector<CREW_BASE*> CrewBaseAry;

class CREW
{
public:
	CREW() {
		/*rankAry = new CREWRankAry();
		baseAry = new CrewBaseAry();
		workStatus = new CrewStatus();*/
	}
	~CREW() {
		/*delete rankAry;
		delete baseAry;
		delete workStatus;*/
	}

	int id;
	
	std::string idCrew;
	std::string division; //部门:飞行员；乘务员	
	std::string gender;	
	std::string nationality;
	std::string natinalIdentification;	
	
	/*CREWRankAry* rankAry;
	CrewBaseAry* baseAry;

	CrewStatus* workStatus;*/
};
class CREW_RANK
{
public:
	int id;
	std::string idCrew;
	std::string rank;
	time_t probationEndUtc;//试用期结束date
	time_t effUtc;
	time_t expUtc;
	std::string position;
	int preCumulatedExpDays;
	char* fleetSpecific;
	char* acType;

	std::map<std::string, int> SkillSet;//特殊资质，此处先为可以飞特殊机场
	
};
class RANK
{
public:
	int rankId;
	std::string airline;
	std::string rank;
	std::string division;
	int displayOrder;
	std::string description;
};
class RankCombination
{
public:
	long long id;
	long long rankCombCriteriaId;
	int options;
	int seqOrder;
	std::string rank;
	std::string positions;
	
};


class CREW_BASE
{
public:
	int id;
	std::string idCrew;
	std::string base;
	time_t effLoc;
	time_t expLoc;
	bool isPrime;
};

#endif // !CREWBD_MINE_H

