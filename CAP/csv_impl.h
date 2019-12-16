#ifndef CSV_IMP_H
#define CSV_IMP_H
/*
crew csv reader µœ÷£∫
Crew
Roster
Pairing
PairingDuty
PairingSegment
Flight
FlightComposition
FlightAttribute
AssignmentGroup
Assignment
Airport
Base
Composition
RankActing
Holiday
SystemParameter
*/
#include <iostream>
#include <chrono>
#include <vector>
#include "csv_reader.h"

//***********************************
// data class
//***********************************
struct csvWorkset {
	int id;
	string name;
	string type;
	string category;
	string division;
};
struct csvCity {
	string city;
	string cityName;
	string cityNativeName;
	string cityDesc;
	string country;
};
struct CsvEvaCrDefaultDutyTime{
	long id;
	string ddtAssignmentUnitQualifier;
	string ddtAssignmentUnitType;
	string ddtBaseAirportCode;
	int ddtDefaultDurationMins;
	string ddtDefaultLocalStrTime;
	time_t defaultLocalStrDt;
	time_t defaultLocalEndDt;
	int ddtRestTime;
};
struct csvFlightAttr {
	long long fltId;
	int attrId;
};
struct csvRankValue {
	long long activityId;
	string rank;
	int value;
};
struct csvImblanceFlight {
	int scenarioId;
	long long flightId;
	string user;
	time_t tmst;
};

struct csvFlightBlhHistory {
	long long id;
	string airline;
	string fleet;
	string depArp;
	string arvArp;
	int blkMin;
};
struct csvFlightAttribute {
	long long id;
	long long fltId;
	int schId;
	int attrId;
};
struct csvCommuteSchedule {
	long long id;
	int worksetId;
	string airline;
	time_t effDt;
	time_t expDt;
	string dow;
	string commuteType;
	string commuteNum;
	string depSta;
	int depTm;
	string arvSta;
	int arvTm;
	int durationMin;
};
struct csvOptimizationNecessity {
	int id;
	string airline;
	string category;
	string division;
	string componentType;
	int function;
	string name;
	string necessity;//'M'-Mandatory or 'O'-Optional
};

struct csvCrewOnFlight {
	long long fltId;
	string crewId;
	string actingRank;
	string assignment;
	string role;
	string subRole;
	long long pairingId;//for COP
	string pairingPrimeActivity;//for COP
	int seqOrder = 0;
};
struct csvActivityComposition {
	long long id;
	int scenarioId;
	long long activityId; // flt_id/ pairing_id
	int compId;
	short multipler;
	char division; //value:C,P,A

}; 
struct csvComposition {
	int compId;
	string airline;
	string name;
	string nameDesc;
	string division;
	int displayOrder;
	int hirarchy;
};
struct csvCompositionRank {
	int id;
	int compId;
	int rankId;
	int planValue;
	int planValueExtra;
};
struct csvSystemParam {
	string parmName;
	string parmValue;
	string parmSeparator;
	string idapp;
	string parmDesc;
};
struct csvRankActing {
	//id,rankId,actingRank,qual
	int id;
	string airline;
	string activeRank;
	string actingRank;
	string qual;
};

struct csvAttribute {
	int id;
	string airline;
	string code;
	string type;
	string name;
	string operation;
	bool isVisible;
};
struct csvRuleParam {
	int id;
	int ruleId;
	int phaseId;
	string paramNames;
	string paramValues;
};
struct ASSIGNMENT_GROUP_MAP {
	int      assignGrpId;
	int      assignId;
};
struct ASSIGNMENT_GROUP_NAME_MAP {
	string   grpName;
	string   name;
};

#define declareParser \
public:\
	string toCsv(vector<string>& headers, void* obj);\
	void fromCsv(vector<string>& headers, int index, char* value, void* obj);\
	void* createInstance();\
	void deleteInstance(void* obj);\
	vector<string>& getDefaultHeaders();\
	void init(vector<string>& headers);

//class ruleParser : public csvParser {
//	declareParser;
//};
//class ruleParamParser : public csvParser {
//	declareParser;
//};
//class cqfParser : public csvParser {
//	declareParser;
//};
//class cqfParamParser : public csvParser {
//	declareParser;
//};
////***********************************
//// csv parser implements
////***********************************
//class optimizationNecessityParser : public csvParser {
//	declareParser;
//};
//
//class commuteScheduleParser : public csvParser {
//	declareParser;
//};
//
//class flightBlhHistoryParser : public csvParser {
//	declareParser;
//};
//class imbalanceFlightParser : public csvParser {
//	declareParser;
//};
class flightParser : public csvParser {
	declareParser;
};
//
class flightCompositionParser : public csvParser {
	declareParser;
};
//class flightAttributeParser : public csvParser {
//	declareParser;
//};

//class segmentParser : public csvParser {
//	declareParser;
//};

//class dutyParser : public csvParser {
//	declareParser;
//};
//
//class pairingParser : public csvParser {
//	declareParser;
//	map<long long, string> pairingPreferenceMap;
//};
//class pairingCompositionParser : public csvParser {
//	declareParser;
//};
//
//class rosterParser : public csvParser {
//	declareParser;
//};

typedef void(*funcCsvSetter)(char*, void*);
//class mandayCcAmParser : public csvParser {
//	declareParser;
//	map<string, funcCsvSetter> headerSetters;
//};
//class mandayFdParser : public csvParser {
//	declareParser;
//};

class crewParser : public csvParser {
	declareParser;
}; 
class crewRankParser : public csvParser {
	declareParser;
};
class crewBaseParser : public csvParser {
	declareParser;
};
//class crewFleetParser : public csvParser {
//	declareParser;
//};
//class crewQualificationParser : public csvParser {
//	declareParser;
//};
//class crewPreferenceParser : public csvParser {
//	declareParser;
//};
//class crewStatusParser : public csvParser {
//	declareParser;
//};
//class crewTeamParser : public csvParser {
//	declareParser;
//};
//class crewEntitlementParser : public csvParser {
//	declareParser;
//};
//class crewProfileParser : public csvParser {
//	declareParser;
//};
//
//class attributeParser : public csvParser {
//	declareParser;
//};
//class fleetParser : public csvParser {
//	declareParser;
//};
class rankParser : public csvParser {
	declareParser;
};
class compositionParser : public csvParser {
	declareParser;
};
/*class compositionRankParser : public csvParser {
	declareParser;
};

class cityParser : public csvParser {
	declareParser;
};*/

class airportParser : public csvParser {
	declareParser;
};
//
//class briefingParser : public csvParser {
//	declareParser;
//};

class baseParser : public csvParser {
	declareParser;
};
//class rankActingParser : public csvParser {
//	declareParser;
//};

//class assignmentGroupParser : public csvParser {
//	declareParser;
//};
//class assignmentParser : public csvParser {
//	declareParser;
//};
//class assignmentGroupMapParser : public csvParser {
//	declareParser;
//};
//class assignmentOverlappableParser : public csvParser {
//	declareParser;
//};
//
//class portQualReqmntParser : public csvParser {
//	declareParser;
//};
//
//class crewOnFlightParser : public csvParser {
//	declareParser;
//};
//
//class holidayParser : public csvParser {
//	declareParser;
//};
//
//class sysParamParser : public csvParser {
//	declareParser;
//};
//
//class scenarioParser : public csvParser {
//	declareParser;
//};
//class worksetParser : public csvParser {
//	declareParser;
//};
//
//class rankValueParser : public csvParser {
//	declareParser;
//};
//
//class recencyParser : public csvParser {
//	declareParser;
//};
//
class stringParser : public csvParser {
	declareParser;
};
//
//class calculationMandayParser : public csvParser {
//	declareParser;
//};
//
//class routeActualRestParser : public csvParser {
//	declareParser;
//};
//
//class routeParser : public csvParser {
//	declareParser;
//};
//
//class rankCombinationCriteriaParser : public csvParser {
//	declareParser;
//};
//class rankCombinationParser : public csvParser {
//	declareParser;
//};

class defaultParser : public csvParser {
	declareParser;
};
//class teamParser : public csvParser {
//	declareParser;
//};
//class csvEvaCrDefaultDutyTimeParser : public csvParser {
//	declareParser;
//};
//class aircraftParser : public csvParser {
//	declareParser;
//};
//
//class voyageParser : public csvParser {
//	declareParser;
//};
//class voyageDetailParser : public csvParser {
//	declareParser;
//};
//class teachingParser : public csvParser {
//	declareParser;
//};
//class teachingDetailParser : public csvParser {
//	declareParser;
//};
//class rosterFlightParser : public csvParser {
//	declareParser;
//};
class crewCsvReader : public csvReader {
public:
	crewCsvReader();
	shared_ptr<csvParser> getParser(string tableName);
	string getParserName(string tableName);
};


extern long long split_cost;///TEST

#endif//CSV_IMP_H