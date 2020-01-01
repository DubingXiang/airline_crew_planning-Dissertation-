#include "output_handler.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;
void OutputHandler::writeSchedule(const CrewSchedulingSolution& soln, const SegNodeSet& curDaySegSet, const std::string& schFile) {
	ofstream outf;
	outf.open(schFile, std::ios::out);
	
	map<long long, int> seg_covered; //seg_id 被cover, 则seg_covered[i]=1
	for (size_t i = 0; i < curDaySegSet.size(); i++) {
		if (curDaySegSet[i]->nodeType == NodeType::SEGMENT && curDaySegSet[i]->optSegment->getAssigned()) {
			seg_covered[curDaySegSet[i]->optSegment->getDBId()] = 1;
		}
		
	}

	for (int i = 0; i < scheduleHeader.size() - 1; i++) {
		outf << scheduleHeader[i] << "^";
	}
	outf << scheduleHeader.back() << "\n";

	const ColumnPool pool;// = soln.getColumnPool(); TODO:to be fixed
	for (const auto& col : pool) {
		if (col->type == Column::ColumnType::RELAX) {
			continue;
		}

		for (const auto& crew : col->_crewgroup->getCrewGroup()) {
			outf << "Crew: " << crew->getIdCrew() << " "
				<< "Rank: " << crew->getCurRank() << " "
				<< "Position: " << crew->getCurPosition() << "\n";
		}

		outf << "    ----duty:";
		EventPath* duty = col->_segpath;
		outf << "startDtLoc: " << utcToUtcString(duty->startNode->startDtLoc) << " "
			<< "endDtLoc: " << utcToUtcString(duty->endNode->endDtLoc) << " "
			<< "startStation: " << duty->startNode->depStation << " "
			<< "endStation: " << duty->endNode->arvStation << "\n";

		for (const auto& segnode : duty->getNodeSequence()) {
			outf << "        ----segment: ";
			outf << "id: " << segnode->optSegment->getDBId() << " "
				<< "fltNum: " << segnode->optSegment->getFlightNumber() << " "
				<< "depDtLoc: " << utcToUtcString(segnode->optSegment->getStartTimeLocSch()) << " "
				<< "arvDtLoc: " << utcToUtcString(segnode->optSegment->getEndTimeLocSch()) << " "
				<< "depArp: " << segnode->optSegment->getDepStation() << " "
				<< "arvArp: " << segnode->optSegment->getArrStation() << "\n";

			/*if (segnode->nodeType == NodeType::seg) {
				seg_covered[segnode->optSegment->getDBId()] = 1;
			}*/
		}
		outf << "\n";
	}

	outf << "----uncovered segments: ";
	stringstream ss;
	size_t count = 0;
	Opt_Segment* uncovered_seg;
	for (size_t i = 0; i < seg_covered.size(); i++) {
		uncovered_seg = curDaySegSet[i]->optSegment;
		if (curDaySegSet[i]->nodeType == NodeType::SEGMENT && seg_covered[uncovered_seg->getDBId()] != 1) {
			ss << "id: " << uncovered_seg->getDBId() << " "
				<< "fltNum: " << uncovered_seg->getFlightNumber() << " "
				<< "depDtLoc: " << utcToUtcString(uncovered_seg->getStartTimeLocSch()) << " "
				<< "arvDtLoc: " << utcToUtcString(uncovered_seg->getEndTimeLocSch()) << " "
				<< "depArp: " << uncovered_seg->getDepStation() << " "
				<< "arvArp: " << uncovered_seg->getArrStation() << "\n";

			++count;
		}
	}
	outf << "total number: " << count << "\n";
	outf << ss.str();

	outf.close();

}
void OutputHandler::writeCrewStatus(const CrewSchedulingSolution& soln, const std::string& statusFile) {
	ofstream outf;
	outf.open(statusFile, std::ios::out);
	
	for (int i = 0; i < crewStatusHeader.size() - 1; i++) {
		outf << crewStatusHeader[i] << "^";
	}
	outf << crewStatusHeader.back() << "\n";

	ColumnPool pool;//= soln.getColumnPool();
	int id = 1;
	for (const auto& col : pool) {
		if (col->type == Column::ColumnType::RELAX) {
			continue;
		}
		outf << "----Group" << id++ << "\n";
		for (const auto& crew : col->_crewgroup->getCrewGroup()) {
			CrewStatus* crew_status = crew->workStatus;
			
			outf << "    Crew: " << crew->getIdCrew() << "^"
				<< "Rank: " << crew->getCurRank() << "^"
				<< "Position: " << crew->getCurPosition() << "^ |";

			outf << to_string(crew_status->accumuFlyMin) << "^"
				<< to_string(crew_status->accumuCreditMin) << "^"
				<< to_string(crew_status->sevenDayTotalFlyMint) << "^"
				<< to_string(crew_status->wholePlanTotalCreditMint) << "\n";
		}
		
	}
	outf.close();
}
void OutputHandler::writeCrewStatus(const vector<Opt_CREW*>& crewSet, const char* statusFile) {
	ofstream outf;
	outf.open(statusFile, ios::app);

	stringstream ss;
	for (const auto& crew : crewSet) {
		// 输出宽度35个字符，左对齐，不足补空格，输出id		
		ss << setw(35) << setfill(' ') << left << crew->getIdCrew() << "| ";
	}
	ss << "\n";

	CrewStatus* status;
	//ss << "endDtLoc:";
	for (const auto& crew : crewSet) {
		status = crew->workStatus;
		ss << setw(35) << setfill(' ') << left << ("endDtLoc:" + utcToUtcString(status->endDtLoc)) << "| ";
	}
	ss << "\n";
	
	//ss << "endStation:";
	for (const auto& crew : crewSet) {
		status = crew->workStatus;
		if (status->restStation == "") {
			ss << setw(35) << setfill(' ') << left << "endStation:initial status" << "| ";
		}
		else {
			ss << setw(35) << setfill(' ') << left << ("endStation:" + status->restStation) << "| ";
		}
		
	}
	ss << "\n";

	//ss << "accumFlyMin:";
	for (const auto& crew : crewSet) {
		status = crew->workStatus;
		ss << setw(35) << setfill(' ') << left << ("accumFlyMin:" + to_string(status->accumuFlyMin)) << "| ";
	}
	ss << "\n";

	//ss << "accumCreditMin:";
	for (const auto& crew : crewSet) {
		status = crew->workStatus;
		ss << setw(35) << setfill(' ') << left << ("accumCreditMin:" + to_string(status->accumuCreditMin)) << "| ";
	}
	ss << "\n";
	for (int i = 0; i < crewSet.size(); i++) {
		ss << setw(35) << setfill('*') << left << "";
	}
	ss << "\n";
		
	outf << ss.str();
	outf.close();
}

void OutputHandler::writeCrewStatistic(const vector<Opt_CREW*>& crewSet, const char* statisticCsvFile) {	
	ofstream outf;
	outf.open(statisticCsvFile, std::ios::out);

	for (int i = 0; i < crewStatisticHeaders.size() - 1; i++) {
		outf << crewStatusHeader[i] << "^";
	}
	outf << crewStatusHeader.back() << "\n";
	
	CrewStatus* status;
	for (const auto& crew : crewSet) {
		status = crew->workStatus;
		outf << crew->getIdCrew() << ","
			<< crew->getCurRank() << ","
			<< crew->getCurPosition() << ","
			<< status->wholePlanTotalFlyMint << ","
			<< status->wholePlanTotalCreditMint << "\n";
	}

	outf.close();
}

void OutputHandler::writeUncoveredFlight(const SegNodeSet& allSegNodeSet, const char* statisticFile) {
	ofstream outf;
	outf.open(statisticFile, std::ios::out);
	
	stringstream ss;
	size_t num_uncovered_flight = 0;
	Opt_Segment* seg;
	for (const auto& seg_node : allSegNodeSet) {
		seg = seg_node->optSegment;
		if (seg_node->nodeType == NodeType::SEGMENT && seg->getAssigned() == false) {
			ss << seg->getDBId() << "^"
				<< seg->getTailNum() << "^"
				<< utcToUtcDtString(seg->getStartTimeLocSch()) << "^"
				<< utcToUtcDtString(seg->getEndTimeLocSch()) << "^"
				<< seg->getDepStation() << "^"
				<< seg->getArrStation() << "\n";

			++num_uncovered_flight;
		}
	}

	outf << "UNCOVERED FLIGHTS: " << num_uncovered_flight << "\n";
	for (int i = 0; i < crewStatisticHeaders.size() - 1; i++) {
		outf << crewStatusHeader[i] << "^";
	}
	outf << crewStatusHeader.back() << "\n";
	outf << ss.str();
	outf.close();
}
