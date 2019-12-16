#pragma once
#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H
#include "..\..\..\UtilFunc.h"
#include "..\..\algorithm\column_generation\column_generation.h"
#include <iostream>
#include <sstream>
#include <fstream>

const std::vector<string> scheduleHeader = { "crewID","rank","position", "startDtLoc", "endDtLoc","dutys content<startDt-endDt-<fltId-fltNum>" };
const std::vector<string> crewStatusHeader = { "crewID","rank","position", "accumFltMin","accumCreditMin","totalFlyMin","totalCreditMint" };
const std::vector<std::string> crewStatisticHeaders = { "crewID","rank","position","wholePlanTotalFlyMint", "wholePlanTotalCreditMint" };
const std::vector<std::string> segmentStatisticHeaders = { "fltId", "fltNum", "startDtLoc","endDtLoc","depArp","arvArp" };

class OutputHandler 
{
public:
	//! 记录单天的计划
	//！包括为覆盖的班
	void writeSchedule(const Solution& soln, const SegNodeSet& curDaySegSet, const std::string& schFile);
	//! 记录单天crew的状态
	//! 以乘务组为单位
	void writeCrewStatus(const Solution& soln, const std::string& statusFile);
	//! 记录整个周期每个crew的状态
	void writeCrewStatus(const vector<Opt_CREW*>& crewSet, const char* statusFile);
	
	//! 输出crew的统计指标
	//! 总飞时；总执勤时间
	void writeCrewStatistic(const vector<Opt_CREW*>& crewSet, const char* statisticCsvFile);
	//! 每个航班的覆盖情况(主要输出未覆盖的航班)
	//! 覆盖率
	void writeUncoveredFlight(const SegNodeSet& allSegNodeSet, const char* statisticFile);
};

#endif // !OUTPUT_HANDLER_H

