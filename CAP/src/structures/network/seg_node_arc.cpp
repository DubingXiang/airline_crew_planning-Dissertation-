#include "seg_node_arc.h"
#include "../../../Segment_mine.h"
#include "../../../csv_impl.h"
#include "../generic/csvClassesTransOptimizable.h"

namespace Network {

	SegNode::SegNode() {
		optSegment = NULL;
		depStation = "";
		arvStation = "";
		startDtUtc = -1;
		startDtLoc = -1;
		endDtUtc = -1;
		endDtLoc = -1;
		taskMin = 0;

		visited = false;
	}
	SegNode::SegNode(Opt_Segment& optSeg) {
		optSegment = &optSeg;
		depStation = optSeg.getDepStation();
		arvStation = optSeg.getArrStation();
		startDtUtc = optSeg.getStartTimeUtcSch();
		startDtLoc = optSeg.getStartTimeLocSch();
		endDtUtc = optSeg.getEndTimeUtcSch();
		endDtLoc = optSeg.getEndTimeLocSch();
		taskMin = optSeg.getBlkMinutes();
		visited = false;
	}
	SegNode::~SegNode() {
		optSegment = NULL;
		//labelSet.clear();
		delete label;
		outArcSet.clear();
		inArcSet.clear();
	}
	void SegNode::setNodeType(NodeType type) {
		nodeType = type;
	}

	SegArc::SegArc(SegNode* startNode, SegNode* endNode) {
		this->startNode = startNode;
		this->endNode = endNode;
	}
	SegArc::~SegArc() {
		startNode = NULL;
		endNode = NULL;
		len = -1;
	}
	void SegArc::setArcType(ArcType type) {
		arcType = type;
	}
	void SegArc::setArcLen(int length) {
		len = length;
	}

}