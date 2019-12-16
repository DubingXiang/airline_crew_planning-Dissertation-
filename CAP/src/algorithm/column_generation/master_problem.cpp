#include "master_problem.h"
#include "../../../include/SummeryTool_H.h"
#include <assert.h>

static Summery::StopWatch STOP_WATCHER;

MasterProblem::MasterProblem() {
	_model = IloModel(_env);
	_cplex = IloCplex(_model);
	_obj = IloObjective(_env);
	_objValue = 0;	

	_constraints_all = IloRangeArray(_env);
	_constraints_segcover = IloRangeArray(_env);
	_constraints_crewassign = IloRangeArray(_env);
	/*_constraints_blance_u = IloRangeArray(_env);
	_constraints_blance_l = IloRangeArray(_env);*/
}
//void MasterProblem::sortByFlyTime(ColumnPool* columnpool_ptr, std::vector<Opt_CREW*>& crewNodeSet) {
//
//}

MasterProblem::~MasterProblem() {
	end();
	_env.end();

	std::cout << "masterproblem destruct function\n";
}
void MasterProblem::end() {
	_dvars_path.end();
	_dvars_uncover.end();
	/*_dvar_upper_flytime.end();
	_dvar_lower_flytime.end();*/
	/*_dvar_upper_seg_num.end();
	_dvar_lower_seg_num.end();*/

	_constraints_all.end();
	_constraints_segcover.end();
	_constraints_crewassign.end();
	/*_constraints_blance_u.end();
	_constraints_blance_l.end();*/
	_obj.end();
	
	_model.end();
	_cplex.end();
	
	
}

void MasterProblem::initSetting() {
	_cplex.setOut(_env.getNullStream());// 设置为不显示过程信息
	_cplex.setParam(IloCplex::Param::Output::WriteLevel, 1);
}


void MasterProblem::init(ColumnPool* columnpool_ptr, std::vector<Opt_Segment*>& segSet, std::vector<CrewNode*>& crewNodeSet) {
	global_pool = columnpool_ptr;
	_seg_set = &segSet;
	_crewnode_set = &crewNodeSet;

	_seg_num = segSet.size();
	_crew_num = crewNodeSet.size() - 2;
	
	_init_crew_fly_mint.reserve(_crew_num);	

	_dvars_path = IloNumVarArray(_env, global_pool->size(), 0.0001, 1.0001, IloNumVar::Type::Float);
	_dvars_uncover = IloNumVarArray(_env, _seg_num, 0, 1, IloNumVar::Type::Float);
	//_dvar_upper_flytime = IloNumVar(_env, 0, 10000, IloNumVar::Type::Float, "upper_fly_min");
	//_dvar_lower_flytime = IloNumVar(_env, 0, 10000, IloNumVar::Type::Float, "lower_fly_min"); //TODO：上下限具体取值需根据crew和path的飞时来确定
	//_dvar_upper_seg_num = IloNumVar(_env /*lower bound and upper bound*/);
	//_dvar_lower_seg_num = IloNumVar(_env /*lower bound and upper bound*/);


}
void MasterProblem::initParameters() {

	for (size_t i = 0; i < _crew_num; i++) {
		_init_crew_fly_mint.emplace_back((*_crewnode_set)[i + 2]->optCrew->workStatus->accumuFlyMin);
	}
}



void MasterProblem::addObjFunction() {
	std::string name("");
	
	double total_cost = 0;
	IloExpr expr_xp(_env, 0);
	for (size_t i = 0; i < global_pool->size(); i++) {
		//expr_xp += cost[i] * _dvars_path[i];
		expr_xp += (*global_pool)[i]->cost * _dvars_path[i];

		total_cost += (*global_pool)[i]->cost;

		if ((*global_pool)[i]->type == ColumnType::duty) {
			_dvars_path[i].setName(std::to_string(i).data());
		}
		else {
			
			name += "relax_col of ";
			name += (*global_pool)[i]->_crewgroup->getCrewGroup().front()->getIdCrew();

			_dvars_path[i].setName(name.data());
			name.clear();
		}

	}
	name.clear();
	for (size_t i = 0; i < _seg_num; i++) {
		
		name = "uncover " + std::to_string(i);
		_dvars_uncover[i].setName(name.data());
		expr_xp += PENALTY_UNCOVER * _dvars_uncover[i];
	}

	//expr_xp +=  5 * (_dvar_upper_flytime - _dvar_lower_flytime); //added 8-7-2019 飞时均衡的惩罚值设为路径总成本的一半
	_obj = IloMinimize(_env, expr_xp);
	_model.add(_obj);
	expr_xp.end();
}
//void MasterProblem::addConstraints() {
//	
//	for (size_t i = 0; i < _seg_set->size(); i++) {
//		IloExpr ct_cover(_env);
//		for (size_t j = 0; j < global_pool->size(); j++) {
//			if (std::find((*global_pool)[j]->_segpath->getNodeSequence().begin(), (*global_pool)[j]->_segpath->getNodeSequence().end(), (*_seg_set)[i])
//				!= (*global_pool)[j]->_segpath->getNodeSequence().end()) {
//				ct_cover += _dvars_path[j];
//			}
//		}
//		_constraints_all.add(ct_cover + _dvars_uncover[i] == 1);
//		_constraints_segcover.add(ct_cover + _dvars_uncover[i] == 1);		
//		ct_cover.end();
//	}
//	_model.add(_constraints_segcover);
//
//	for (size_t i = 0; i < _crewnode_set->size(); i++) {
//		IloExpr ct_assign(_env);
//		IloExpr ct_balance_flytime(_env);
//		for (size_t j = 0; j < global_pool->size(); j++) {
//			if (std::find((*global_pool)[j]->_crewgroup->getNodeSequence().begin(), (*global_pool)[j]->_crewgroup->getNodeSequence().end(), _crewnode_set[i])
//				!= (*global_pool)[j]->_crewgroup->getNodeSequence().end()) {
//				ct_assign += _dvars_path[j];
//
//				ct_balance_flytime += (*global_pool)[j]->_segpath->getSegPathCost().fly_time_costtuple.unit * _dvars_path[j];
//			}
//		}
//		_constraints_all.add(ct_assign + _dvars_uncover[i] == 1);
//		_constraints_crewassign.add(ct_assign + _dvars_uncover[i] == 1);
//
//		_constraints_all.add(ct_balance_flytime - _dvar_upper_flytime <= 0);
//		_constraints_all.add(ct_balance_flytime - _dvar_lower_flytime >= 0);
//		_constraints_blance_u.add(ct_balance_flytime - _dvar_upper_flytime <= 0);
//		_constraints_blance_l.add(ct_balance_flytime - _dvar_lower_flytime >= 0);
//		
//		ct_assign.end();
//		ct_balance_flytime.end();
//	}
//	_model.add(_constraints_crewassign);
//	_model.add(_constraints_blance_u);
//	_model.add(_constraints_blance_l);
//}

void MasterProblem::addConstraints2() {	
	//size_t real_col_end = global_pool->size() - _crew_num; 
	//_crew_num个Column是休息column，但是在之后的迭代中，不断有新column添加，最好是把rest column放在最前面？
	//global最前面为rest column
	for (size_t i = 0; i < _seg_num; i++) {
		IloExpr ct_cover(_env);
		/*for (size_t j = 0; j < real_col_end; j++) {*/
		for (size_t j = _crew_num; j < global_pool->size(); j++) {
			auto id_seq = (*global_pool)[j]->_segpath->optseg_id_sequence;
			if (std::find(id_seq.begin(), id_seq.end(), i) != id_seq.end()) {
				ct_cover += _dvars_path[j];
			}
		}
		_constraints_all.add(ct_cover + _dvars_uncover[i] == 1);
		_constraints_segcover.add(ct_cover + _dvars_uncover[i] == 1);
		ct_cover.end();
	}
	_model.add(_constraints_segcover);
 	
	for (size_t i = 0; i < _crew_num; i++) { 
		IloExpr ct_assign(_env);
		//IloExpr ct_balance_flytime(_env);
		for (size_t j = 0; j < global_pool->size(); j++) {
			auto id_set = (*global_pool)[j]->_crewgroup->optcrew_id_set;
			if (std::find(id_set.begin(), id_set.end(), i) != id_set.end()) {
				ct_assign += _dvars_path[j];

				//ct_balance_flytime +=  _init_crew_fly_mint[i] + (*global_pool)[j]->_segpath->getSegPathCost().fly_time_costtuple.unit * _dvars_path[j];
			}
		}
		_constraints_all.add(ct_assign == 1);
		_constraints_crewassign.add(ct_assign == 1);

		/*_constraints_all.add(ct_balance_flytime - _dvar_upper_flytime <= 0);
		_constraints_all.add(ct_balance_flytime - _dvar_lower_flytime >= 0);
		_constraints_blance_u.add(ct_balance_flytime - _dvar_upper_flytime <= 0);
		_constraints_blance_l.add(ct_balance_flytime - _dvar_lower_flytime >= 0);*/

		ct_assign.end();
		//ct_balance_flytime.end();
	}
	_model.add(_constraints_crewassign);
	/*_model.add(_constraints_blance_u);
	_model.add(_constraints_blance_l);*/
}


void MasterProblem::buildModel() {
	addObjFunction();
	//addConstraints();
	addConstraints2();
}

void MasterProblem::exportModel(const std::string& curDayStr, int iter) {
	std::string master_file_name = "./data/output/lp_files/master_model_day_" + curDayStr + "_iter_" + std::to_string(iter) + ".lp";
		
	_cplex.exportModel(master_file_name.data());
}

int MasterProblem::solve() {
	int status = 1;

	if (!_cplex.solve()) {
		_env.error() << "!!!----------------------Failed to optimize MasterProblem----------------------" << std::endl;
		status = 0;
	}
	if (_cplex.getStatus() == IloAlgorithm::Infeasible) {
		_env.error() << "!!!----------------------The solution is infeasible----------------------" << std::endl;
		status = -1;
	}

	_objValue = _cplex.getObjValue();
	
	return status;
}

void MasterProblem::writeSoln(const std::string& curDayStr, int iter) {
	std::string LPsoln_file_name  = "./data/output/lp_files/LP_soln_day_" + curDayStr + "_iter_" + std::to_string(iter) + ".sln";
		
	_cplex.writeSolution(LPsoln_file_name.data());
}

std::vector<double>& MasterProblem::getSegCoverDuals() {
	assert(_constraints_segcover.getSize() == _seg_set->size());

	//IloNumArray seg_cover_duals(_env, _constraints_segcover.getSize());		
	//_cplex.getDuals(seg_cover_duals, _constraints_segcover);
	seg_cover_duals.clear();
	seg_cover_duals.resize(_constraints_segcover.getSize());
	for (size_t i = 0; i < _constraints_segcover.getSize(); i++) {
		seg_cover_duals[i] = _cplex.getDual(_constraints_segcover[i]);
	}
	
	return seg_cover_duals;
}
std::vector<double>& MasterProblem::getCrewAssignDuals() {
	assert(_constraints_crewassign.getSize() == _crew_num);

	/*IloNumArray crew_assign_duals(_env, _constraints_crewassign.getSize());
	_cplex.getDuals(crew_assign_duals, _constraints_crewassign);*/

	crew_assign_duals.clear();
	crew_assign_duals.resize(_constraints_crewassign.getSize());
	for (size_t i = 0; i < _constraints_crewassign.getSize(); i++) {
		crew_assign_duals[i] = _cplex.getDual(_constraints_crewassign[i]);
	}

	return crew_assign_duals;

}

void MasterProblem::addNewColumns(ColumnPool& newPool) {
	//global_pool->column_pool.insert(global_pool->column_pool.end(), columnpool_ptr->column_pool.begin(), columnpool_ptr->column_pool.end());
	
	int report_frequncy = 10000;

	//std::stringstream ct_str;
	int index = global_pool->size();
	IloNumVarArray add_vars(_env);
	IloNumArray add_vars_cost(_env);
	
	STOP_WATCHER.Start();
	for (size_t i = 0; i < newPool.size(); i++) {
		
		if (i > 0 && i%report_frequncy == 0) {
			STOP_WATCHER.Stop();
			std::cout << "added " << i << " columns\n";
			STOP_WATCHER.printElapsed();
			STOP_WATCHER.Restart();
		}

		global_pool->emplace_back(newPool[i]); //NOTE:改变了CG中的globalPool

		IloNumVar col_var = IloNumVar(_env, 0.0001, 1.0001, IloNumVar::Type::Float);		
		col_var.setName(std::to_string(index++).data());
		
		_dvars_path.add(col_var);
		// obj	
		add_vars.add(col_var);
		add_vars_cost.add(newPool[i]->cost);

		// seg cover constraint		
		auto seg_id_set = newPool[i]->_segpath->optseg_id_sequence; //NOTE：用直接保存optSeg在curSegSet中的index代替了指针查找
		int ct_id = 0;
		for (size_t s = 0; s < seg_id_set.size(); s++) {			
			ct_id = seg_id_set[s];			
			_constraints_segcover[ct_id].setLinearCoef(col_var, 1);			
		}

		/*ct_str.str("");
		ct_str << _constraints_segcover[ct_id];
		ct_str << "\n";*/		
		
		// crew assign constraint
		auto crewnode_id_set = newPool[i]->_crewgroup->optcrew_id_set;
		ct_id = 0;
		for (size_t c = 0; c < crewnode_id_set.size(); c++) {			
			ct_id = crewnode_id_set[c];			
			_constraints_crewassign[ct_id].setLinearCoef(col_var, 1);							
		}

		//ct_str << _constraints_crewassign[ct_id];

		//debug
		/*if (newPool.size() > 1000) {
			std::cout << "seg ct " << ct_str.str() << "\n";
		}*/
		
	}

	/*std::stringstream obj_str;
	obj_str << _obj;
	std::cout << "before obj" << obj_str.str() << "\n";*/
	
	//obj_str.str("");
	
	
	_obj.setLinearCoefs(add_vars, add_vars_cost); 
	/*obj_str << _obj;
	std::cout << "after obj" << obj_str.str() << "\n";*/
	
	add_vars.end();
	add_vars_cost.end();
	
}
//void MasterProblem::reset() {
//	end();
//
//	_model = IloModel(_env);
//	_cplex = IloCplex(_model);
//	_obj = IloObjective(_env);
//	_objValue = 0;
//
//	_constraints_all = IloRangeArray(_env);
//
//	// decision varibles initialization
//	_dvars_path = IloNumVarArray(_env /*, num of column pool*/);
//	_dvar_upper_flytime = IloNumVar(_env /*lower bound and upper bound*/);
//	_dvar_lower_flytime = IloNumVar(_env /*lower bound and upper bound*/);
//	//_dvar_upper_seg_num = IloNumVar(_env /*lower bound and upper bound*/);
//	//_dvar_lower_seg_num = IloNumVar(_env /*lower bound and upper bound*/);
//}
