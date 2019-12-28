#include "column_generation.h"
#include "../../../include/algorithm_xdb.h"



const std::string MIP_FILE_PATH = "../data/output/mip_files/";


CrewSchedulingColumnGenerationModule::CrewSchedulingColumnGenerationModule() {
}
CrewSchedulingColumnGenerationModule::~CrewSchedulingColumnGenerationModule() {
	releaseVector(_global_pool);
	delete _global_pool;
	delete _master;
	delete _sub_pro;

	/*for (size_t i = 0; i < _solution_pool.size(); i++) {
		delete _solution_pool[i];
	}*/
	releaseVector(&_solution_pool);

	
	std::cout << "column generation destruct function\n";
}

void CrewSchedulingColumnGenerationModule::init(/*ColumnPool& initialPool*/int curDay, std::vector<CrewGroup*>& initialGroups,
							CrewNetwork& crewNet,
							SegNetwork& segNet,
							const CrewRules& rules,
							const Penalty& penaltySetting) {	
	_cur_day_str = std::to_string(curDay);

	_global_pool = new ColumnPool();
	_crew_net = &crewNet;
	_seg_net = &segNet;
	_rules = &rules;
	_penalty = &penaltySetting;
	_crew_node_set = &crewNet.nodeSet;
	_seg_node_set = &segNet.nodeSet;

	 _sub_pro = new SubProblem(*_crew_net, *_seg_net, *_rules);
	 _sub_pro->setInitialGroups(initialGroups);
	 _master = new MasterProblem();	 

	_lb = 0;
	_ub = MAX;
	_max_num_iter = MAX_NUM_ITER;
}

void CrewSchedulingColumnGenerationModule::solve() {
	int result = 0;	
	_sub_pro->findSegPaths();
	_sub_pro->setPathStatus(*_penalty);
	_sub_pro->setCurDaySegSet();
	_sub_pro->setIndexOfCurSeg();
	//_sub_pro->labelSpecialSegPath()
	_sub_pro->groupingSegPathByComposition();
	//_sub_pro->estimateMeanFlyMin();
	_sub_pro->estimateMeanFlyMinPrecisly(); //changed 20190822

	_sub_pro->addRestColumns();
	////create initial column pool
	//_sub_pro->matchGroupAndPath();	
	
	bool local_search = true;
	_global_pool->swap(_sub_pro->getCurLocalPool()); //sub_pro中的列池变为空

	_master->init(_global_pool, _sub_pro->getCurDaySegSet(), *_crew_node_set); //NOTE：后两个参数只需要传一次，改！
	_master->initSetting();
	_master->initParameters();
	
	_master->buildModel();
	for (size_t i = 1; i < _max_num_iter; i++) {				
		//debug
		std::cout << "iter: " << std::to_string(i) << "\n";
		
		//_master->exportModel(_cur_day_str, i);

		result = _master->solve();
		
		//_master->writeSoln(_cur_day_str, i);

		_lb = _master->getObjValue();
		std::cout << "--------------------------------------------the cur LB is " << _lb << "--------------------------------------------" << std::endl;

		//! TODO：一旦覆盖率达到，则无需再次检查，即isCaverageHigh()返回true后，就之后的迭代都不必再调用
		if (i % _frequency_solve_mip == 0 && isCoverageHigh() && result == 1) {
			
			////! 每隔_frequency_solve_mip次求整数解
			//solveMIP();
			//double obj_value_mip = _mip_cplex.getObjValue();
			//std::cout << "----------------------the cur UB is " << obj_value_mip << "----------------------" << std::endl;
			//Solution* new_soln = new Solution();
			//getFeasibleSolution(new_soln);
			//_solution_pool.emplace_back(new_soln);
			//if (obj_value_mip < _ub) {
			//	_ub = obj_value_mip;
			//	_best_solution_pool.clear();
			//	_best_solution_pool.emplace_back(new_soln);
			//}
			//else if (obj_value_mip == _ub) {
			//	_best_solution_pool.emplace_back(new_soln);
			//}
			//
			//_mip_model.end();
			//_mip_cplex.end();
		}

		_sub_pro->updateDuals(_master->getSegCoverDuals(), _master->getCrewAssignDuals());
		_sub_pro->getCurLocalPool().clear(); //将上次迭代的列清除
		
		if (local_search) {
			_sub_pro->findGroups();
			_sub_pro->matchGroupAndPath();
			ColumnPool& local_pool = _sub_pro->getCurLocalPool();
			std::cout << "-----find new columns " << local_pool.size() << "\n";
			//TODO: 实际是top k，后续优化
			std::sort(local_pool.begin(), local_pool.end(),
				[](const Column* a, const Column* b) {return a->reduced_cost < b->reduced_cost; });
			
			int num_saved_columns = min(1000, (int)local_pool.size());
			for (ColumnPool::iterator it = local_pool.begin() + num_saved_columns; it != local_pool.end();) {
				delete *it;
				it = local_pool.erase(it);
			}
		}
		if (_sub_pro->getCurLocalPool().size() == 0) {
			std::cout << "-----start accuraccy strategy" << _sub_pro->getCurLocalPool().size() << "\n";
			_sub_pro->findColumnSet(); //changed-20190821 
			std::cout << "-----accuraccy strategy find new columns " << _sub_pro->getCurLocalPool().size() << "\n";
			
			local_search = false;
		}
		

		if (_sub_pro->getCurLocalPool().size() == 0) {
			std::cout << "cur day's RMP met optimal \n";
			
			bool flag = false;
			double obj_value_opt;	
			if (!isFeasibelSoln(_master->getPathDvar())) {
				solveMIP();
				obj_value_opt = _mip_cplex.getObjValue();
				
			}
			else {
				_mip_cplex = _master->_cplex;
				obj_value_opt = _master->getObjValue();

				flag = true;
			}			
			std::cout << "--------------------------------------------the optimal obj value is " << obj_value_opt << "---------------------------------------" << std::endl;
			
			std::string opt_model_file(MIP_FILE_PATH);
			opt_model_file += "opt_mip_model_cur_day_" + _cur_day_str + ".lp";
			std::string opt_soln_file(MIP_FILE_PATH);
			opt_soln_file+= "opt_mip_soln_cur_day" + _cur_day_str + ".sln";

			//_mip_cplex.exportModel(opt_model_file.data());
			//_mip_cplex.writeSolution(opt_soln_file.data());


			/*output uncovered segments and rest-crews*/ 						
			/*std::string segment_crew_messege_file(MIP_FILE_PATH);
			segment_crew_messege_file += "segment_caverage_crew_assign_msg_cur_day" + _cur_day_str + ".txt";
			
			std::ofstream outf;
			outf.open(segment_crew_messege_file, std::ios::out);
			outf << "----uncovered segments\n";
			std::stringstream ss;
			
			IloNumVarArray& uncover_vars = _master->getUncoverDvar();
			for (size_t i = 0; i < uncover_vars.getSize(); i++) {
				if (std::abs(_master->_cplex.getValue(uncover_vars[i]) - 1) < 0.0001) {
					++_num_uncoverd_segments;
					ss << uncover_vars[i].getName() << "\n";
				}
			}
			outf << "total number of uncovered segments : " + std::to_string(_num_uncoverd_segments) + "\n";
			outf << ss.str();

			outf << "----rest crews\n";
			ss.str("");
			IloNumVarArray& col_vars = _master->getPathDvar();
			for (size_t i = 0; i < _crew_node_set->size(); i++) {
				if (std::abs(_master->_cplex.getValue(col_vars[i]) - 1) < 0.0001) {
					++_num_rest_crews;
					ss << col_vars[i].getName() << "\n";
				}
			}
			outf << "total number of rest crews: " + std::to_string(_num_rest_crews) + "\n";
			outf << ss.str();*/
			/*end! output uncovered segments and rest-crews*/


			CrewSchedulingSolution* opt_soln = new CrewSchedulingSolution();
			getFeasibleSolution(opt_soln);
			_solution_pool.emplace_back(opt_soln);
			if (obj_value_opt <= _ub) {
				_ub = obj_value_opt;
				_best_solution_pool.clear();
				_best_solution_pool.emplace_back(opt_soln);
			}
			/*_mip_model.end();			
			if (!flag) {
				_mip_cplex.end();
			}*/
			
			break;
		}
		
		_master->addNewColumns(_sub_pro->getCurLocalPool());
		
	}
}

bool CrewSchedulingColumnGenerationModule::isCoverageHigh() {
	IloNumVarArray& vars_uncover = _master->getUncoverDvar();
	size_t vars_size = vars_uncover.getSize();
	int count = 0;
	for (size_t i = 0; i < vars_size; i++) {
		if (std::abs(_master->_cplex.getValue(vars_uncover[i]) - 1) <= 1e-4) {
			++count;
		}
	}

	return count >= vars_size * REQUIRE_CAVERAGE_RATE;
}

bool CrewSchedulingColumnGenerationModule::isFeasibelSoln(IloNumVarArray& dvars) {
	const double kEPSILON = 1e-8;	
	for (size_t i = 0; i < dvars.getSize(); i++) {		
		if (!(std::abs(_master->_cplex.getValue(dvars[i]) - 1) <= kEPSILON || std::abs(_master->_cplex.getValue(dvars[i]) - 0) <= kEPSILON)) {
			//std::cout << "<i: " << std::to_string(i) << ", value: " << _master->_cplex.getValue(dvars[i]) << ">\n";
			return false;
		}
	}

	return true;
}

int CrewSchedulingColumnGenerationModule::solveMIP() {
	
	IloEnv env = _master->_model.getEnv();
	_mip_model = IloModel(env);	
	_mip_model.add(_master->_model);
	
	
	
	for (int i = 0; i < _master->getPathDvar().getSize(); i++) {
		_master->getPathDvar()[i].setBounds(0, 1);
	}

	_mip_model.add(IloConversion(env, _master->getPathDvar(), IloNumVar::Type::Int));
	_mip_model.add(IloConversion(env, _master->getUncoverDvar(), IloNumVar::Type::Int));
	
	_mip_cplex = IloCplex(_mip_model);	
	


	int status = 1;

	if (!_mip_cplex.solve()) {
		env.error() << "!!!----------------------Failed to optimize MasterProblem----------------------" << std::endl;
		status = 0;
	}
	if (_mip_cplex.getStatus() == IloAlgorithm::Infeasible) {
		env.error() << "!!!----------------------The solution is infeasible----------------------" << std::endl;
		status = -1;
	}
	
	return status;
}
void CrewSchedulingColumnGenerationModule::getFeasibleSolution(CrewSchedulingSolution* soln) {
	IloNumArray values(_mip_cplex.getEnv());
	_mip_cplex.getValues(values, _master->getPathDvar());
	//std::vector<double> dvar_values(values.getSize());	
	std::cout << "feasible soln dvar vales \n";
	for (size_t i = 0; i < values.getSize(); i++) {
		//std::cout << "<i," << std::to_string(i) << ", value:" << values[i] << ">\n";
		if (values[i] > 0.99 /*&& (*_global_pool)[i]->type == ColumnType::duty*/) {
			soln->getColumnPool().emplace_back((*_global_pool)[i]);
			//std::cout << "<i," << std::to_string(i) << ", value:" << values[i] << ">\n";
		}
	}	
	soln->setTotalObjValue(_mip_cplex.getObjValue());
}


void CrewSchedulingColumnGenerationModule::solveMasterProblem() {
	std::cout << "still in the process of testing \
		\n this is the function " << __FUNCTION__ << std::endl;
}
void CrewSchedulingColumnGenerationModule::solveSubProblem() {
	std::cout << "still in the process of testing "
		<< "\nthis is the function" << __FUNCTION__ << std::endl;
}
void CrewSchedulingColumnGenerationModule::updateDuals() {
	std::cout << "still in the process of testing "
		<< "\nthis is the function" << __FUNCTION__ << std::endl;
}
bool CrewSchedulingColumnGenerationModule::terminate() {
	std::cout << "still in the process of testing "
		<< "\nthis is the function" << __FUNCTION__ << std::endl;
	return true;
}
