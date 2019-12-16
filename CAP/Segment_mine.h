#include <vector>
#include <string>
#include <time.h>
#include "csv_impl.h"
//"..\csvReader\csv\csv_impl.h"
#ifndef SEGMENT_MINE_H
#define SEGMENT_MINE_H

class Segment
{
public:
	void setDBId(long long id);
	void setDate(std::string fltDt);
	void setFlightNumber(std::string fltNum);
	void setDepStation(std::string depArp);
	void setArrStation(std::string arvArp);
	void setBlkMinute(int schBlkMin);
	void setStartTimeUtcSch(time_t schDepDtUtc);
	void setStartTimeLocSch(time_t schDepDtLoc);
	void setEndTimeUtcSch(time_t schArvDtUtc);
	void setEndTimeLocSch(time_t schArvDtLoc);
	void setDomIntType(std::string segType);
	void setFleetCD(std::string fleetCD);
	void setTailNum(std::string tailNumber);

	long long getDBId();
	std::string getDate();
	std::string getFlightNumber();
	std::string getDepStation();
	std::string getArrStation();
	int getBlkMinutes();
	time_t getStartTimeUtcSch();
	time_t getStartTimeLocSch();
	time_t getEndTimeUtcSch();
	time_t getEndTimeLocSch();
	std::string getDomIntType();
	std::string getFleetCD();
	std::string getTailNum();
	

	/*csvActivityComposition* fltCompo;
	csvComposition* composition;
	bool assigned = false;*/

private:
	/*只考虑排环计划相关的属性*/	
	long long _id;
	//int schId;//案例中均相同	
	std::string _fltDt;//航班所处date
	std::string	_fltNum;//航班号，实际机票上的航班号
	std::string _depArp;
	std::string _arvArp;
	//int	airline;//航司//案例中均相同	
	int	_schBlkMin;//计划航班飞行（block）时间，以min计
	time_t _schDepDtUtc;
	time_t _schDepDtLoc;
	time_t _schArvDtUtc;
	time_t _schArvDtLoc;

	//int	actBlkMin;//实际航班飞行（block）时间，以min计
	//string segSeq;//一趟航班的segment的序列，因为航班可能经停。案例均为1
	std::string _segType;//segment的类型：D-国内；I-国际；R-region，区域，比如台湾，香港。案例均为D

	string _fleetCD;
	//string _aircraft;//和fleet基本等效，一般以fleet为准。仅关注tailNumer即可
	std::string _tailNumber;//机尾号
	
	//string setFltType;//客机，货机，做PO不用关注
	//string actAln;//实际的airline，不用关注
	//string actDepDtUtc;
	//string actDepDtLoc;
	//string actArvDtUtc;
	//string actArvDtLoc;
	//string actFltNum;//实际航班号，不用关注
	//string actDepArp;
	//string actArvArp;
	//string flightStatus;
	//string onwardFlt;
	//string acConfig;
	//string actTakeOffLoc;
	//string actTouchDownUtc;
	//string actTouchDownLoc;
	//string actTakeOffUtc;
	//int ver;//变化次数，不用关注
	//string serviceType;
	//string interfaceFltId;	
};

class Duty
{
public:	
	void setDutyId(long long id);
	void setPairingId(long long pairingId);
	void setDutySeq(int seq);
	void setTypeByStr(std::string type);
	void setDepStation(std::string startAirport);
	void setArrStation(std::string endAirport);
	void setRegion(std::string region);
	void setMinRest(int minRestMinutes);
	void setFlyMin(int fltMinutes);
	void setDutyMin(int dutyMinutes);
	void setStartTimeUtcSch(time_t schStartDtUtc);
	void setStartTimeLocSch(time_t schStartDtLoc);
	void setEndTimeUtcSch(time_t schEndDtUtc);
	void setEndTimeLocSch(time_t schEndDtLoc);

	long long getDutyId();
	long long getPairingId();
	int getDutySeq();
	std::string getTypeStr();
	std::string getDepStation();
	std::string getArrStation();
	std::string getRegion();
	int getMinRest();
	int getFlyMin();
	int getDutyMin();
	time_t getStartTimeUtcSch();
	time_t getStartTimeLocSch();
	time_t getEndTimeUtcSch();
	time_t getEndTimeLocSch();
private:
	long long _id;
	long long _pairingId;
	int _dutySeq;
	std::string _dutyType;
	/*int pickupMin;
	int dropoffMin;*/
	std::string _startArp;
	std::string _endArp;
	std::string _region;
	int _minRestMin;
	int _fltMin;
	int _dutyMin;
	//int creditedMin;

	time_t _schStartDtUtc;
	time_t _schStartDtLoc;
	time_t _schEndDtUtc;
	time_t _schEndDtLoc;

};
class DBAirport
{
public:
	int id;
	std::string airport;
	std::string airportName;
	std::string airportNativeName;
	std::string airportICAO;
	std::string airportABBR;
	std::string city;
	std::string cityName;
	std::string cityNativeName;
	std::string category;
	std::string dir;
	std::string zoneId;
	int utcOffset;
	std::string dstGrp;
	std::string country;
	std::string plateauType;
	std::string rnp;
	std::string cats;
};

class BASE
{
public:
	BASE() {};
	BASE(int id, std::string base, std::string name) :baseId(id), base(base), name(name) {};
	int baseId;
	std::string airline;
	std::string base;
	std::string name;
	int displayOrder;
	std::string isPrimeDisplayBase;
};

//class Pairing 
//{
//public:
//	Pairing()
//	{
//		fltVec_ptr = new std::vector<Segment*>();
//	}
//	~Pairing()
//	{
//		delete fltVec_ptr;
//		delete coverArrayFlts;
//	}
//
//	string toCsv();
//	void setValues();//TODO：计算pairing中duty的数量，即天数
//	string outFlts();
//	string id;
//	string base;
//	string endArp;
//	std::vector<Segment*>* fltVec_ptr;
//	/*props*/
//	int durationInDays = -1;//用于检查
//	int durationInHours;
//	int blockMint;
//	int flyMint;
//	//int duty
//	string schStartDtUtc;
//	string schEndDtUtc;
//	string schStartDtLoc;		
//	string schEndDtLoc;
//	/*for model*/
//	int* coverArrayFlts;	
//	//int fixedCost;//暂时用durationInDays表示
//private:
//	int _calDurationInDays();
//
//};

#endif // !SEGMENT_MINE_H
