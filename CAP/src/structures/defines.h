#pragma once
#ifndef DEFINES_H_
#define DEFINES_H_
#include <string>
#include <vector>
#include <map>

namespace defines 
{
	using Skill = std::string;

	using seqIdVec = std::vector<int>; ///< 存放seq在_rankCombinationSet中的id

	const int kSECONDS_ONE_DAY = 24 * 3600; //24 hour * 3600 secends
	
}



#endif // !DEFINES_H_
