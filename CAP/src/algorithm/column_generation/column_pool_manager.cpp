#include "column_pool_manager.h"
#include <memory>

//! 若找不到判断变量是否是基变量的方法
//! 按reduced cost来挑选劣质列
void ColumnPoolManager::recordColumnBecomeBasis(const IloCplex& cplex, const IloNumVarArray& decisionVars) {
	for (size_t i = 0; i < _num_be_non_basis.size(); i++) {
		if (cplex.getBasisStatus(decisionVars[i]) != IloCplex::BasisStatus::Basic) { //NotABasicStatus
			_num_be_non_basis[i] += 1;
		}
	}
}

void ColumnPoolManager::removeBadColumns(ColumnPool* globalPool, IloNumVarArray* decisionVars) {
	assert(globalPool->size() == decisionVars->getSize());

	ColumnPool::iterator pool_begin = globalPool->begin();		
	//int count = 0;
	for (size_t i = 0; i < _num_be_non_basis.size(); i++) {		
		if (_num_be_non_basis[i] >= maxNbIterationsNonBasis) {
			// remove column i from global pool			
			//globalPool->erase(pool_begin + i - count);
			//++count;
			
			// set var = 0
			(*decisionVars)[i].setBounds(0.0, 0.0);
		}
	}
	// TODO: reset num_be_non_basis
	//_num_be_non_basis

}