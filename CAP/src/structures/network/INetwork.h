#pragma once
#ifndef INETWORK_H
#define INETWORK_H
#include <vector>

namespace Network 
{
	enum NodeType
	{
		SOURCE,
		SINK,
		BASE_S,
		BASE_T,
		SEGMENT,
		DEADHEAD,
		CREW
	};
	enum ArcType
	{
		SOURCE_LEVING,
		SINK_ENTERING,
		BASE_LEVING,
		BASE_ENTERING,
		CONNECTED,
		REST
	};
	class INode
	{
	public:
		
		//INode() {};
		virtual ~INode() {};

		virtual void setNodeType(NodeType type) = 0;

		NodeType nodeType;
		//double price;

		bool visited;
	};

	class IArc
	{
	public:
		
		//IArc() {};
		virtual ~IArc() {}

		virtual void setArcType(ArcType type) = 0;
		virtual void setArcLen(int type) = 0;
		int len;
		ArcType arcType;

		INode* startNode;
		INode* endNode;
	};

	class INetwork
	{
	public:
		//INetwork() {}
		virtual ~INetwork() {}

		virtual void createNetwork() = 0;
	};
}

#endif // !INETWORK_H

