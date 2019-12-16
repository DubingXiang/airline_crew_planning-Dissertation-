#pragma once
#ifndef COLUMN_POOL_MANAGER
#define COLUMN_POOL_MANAGER
#include "column.h"
#include "ilcplex\ilocplex.h"
#include <vector>

class ColumnPoolManager
{
public:
	//! 未成为基变量的允许最大迭代次数
	//! 当某一列连续maxNbIterationsNonBasis次都没有成为基变量，则移除该列
	int maxNbIterationsNonBasis = 3;
	
	void recordColumnBecomeBasis(const IloCplex& cplex, const IloNumVarArray& decisionVars);
	//! 分别从globalPool中删除劣质列，并将对应的变量移除（或者fixed其值为0）
	void removeBadColumns(ColumnPool* globalPool, IloNumVarArray* decisionVars);


private:
	//! index = index of column in global_pool
	//! value = number of column not be a basis
	std::vector<int> _num_be_non_basis;


};

#endif // !COLUMN_POOL_MANAGER

