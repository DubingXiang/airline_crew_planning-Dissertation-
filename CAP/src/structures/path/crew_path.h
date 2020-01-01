#pragma once
#ifndef CREW_PATH_H
#define CREW_PATH_H
#include "../../pch.h"
#include "../../util/unit_penalty_setting.h"
#include "../network/crew_node_arc.h"
#include "IPath.h"
#include "seg_path.h"


struct CrewGroupPrice
{
	CrewGroupPrice() {
		fly_balance_price = 0;
		_total_price = 0;
	}
	
	void calTotalPrice() {		
		_total_price = fly_balance_price; //+..
	}
	double getTotalPrice() const {
		return _total_price;
	}
		
	double fly_balance_price;
	
private:
	double _total_price;

};

class CrewGroup :public IPath
{
public:
	virtual ~CrewGroup();

	virtual double getCost() const;
	
	//运算符重载
	bool operator < (const CrewGroup &b) const { return this->getCost() < b.getCost(); }
	bool operator > (const CrewGroup &b) const { return this->getCost() > b.getCost(); }


	void computeCost();
	std::vector<Network::CrewNode*>& getNodeSequence() { return _crewNodeSequence; }
	void setCrewGroup();	
	std::vector<Opt_CREW*>& getCrewGroup() { return _crewGroup; }

	void setCrewIndexSet(/*const std::vector<Opt_CREW*>& optCrewSet*/);
	std::vector<int> optcrew_id_set;

	/*8-6-2019*/
	
	//! 包括具体配比信息的确定
	void setBasicProperties();
	std::map<std::string, int> spetialCredentials; //只要有一个crew没有特殊资质，则整个group都没有	
	int max_total_fly_mint;
	int max_credit_mint;
	time_t endDtLoc;
	std::string curStation;

	const compoMode* compo_mode; // 在seg_path.h 中，TODO:应该独立出去 20191229


private:
	int _nbCrewNodes;
	std::vector<Network::CrewNode*> _crewNodeSequence;
	CrewGroupPrice _crewsPrice;
	utils::CrewCostTypeAmount _costtype_to_amount; 

	std::vector<Opt_CREW*> _crewGroup;

};

#endif // !CREW_PATH_H