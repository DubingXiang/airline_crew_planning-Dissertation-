

整个程序要用到的主要基础集合：
SegmentSet, CrewSet, Opt_SegmentSet, Opt_CREWSet
SegNodeSet, SegArcSet
***************
Optimizer：提供解决问题的大纲，具体用什么方法，不管
	// 流程
	1.get_problem //获得问题
	2.solve_problem //解决问题
	3.output_solution //给出解决方案
	 

//interface OptimizationStrategy
	 
SequentialMethod //顺序优化
IntegrateMethod //一体化优化--不是算法，是方法
 --non rolling horizon //不滚动优化
 --RollingHorizon //滚动优化--不是算法，是一个框架
   --Period //时间段
 
 //单个时间段Period面临的问题也是一体化的问题，只不过是分解了的
*************** 
CSProblem
CSSolution
***************
CSAlgorithm // 算法抽象，提供算法大纲
	1. get_problem //获得问题
	2. search_solutions(){
		SolvingApproachModule.runAndGetSoln(){ //interface
			CGModule : SolvingApproachModule {
				member: 
					MasterProblemApproach
					SubProblemApproach
					override runAndGetSoln(){
						MasterProblemApproach.run()
						SubProblemApproach.run()
					}
			}
			
			TreeSearchModule : solvingMethodModule{ //求可行解
				
			}							
		}				
	}
	3.

interface AlgorithmInterface {
	1. get_problem
	2. 
	
}
	
CG //具体算法类--也是一个较大的框架
	1.MasterProblem
	2.SubProblem
TreeSearch //启发式搜索求整数解
NeighborhoodSearch //邻域搜索--可以用来改善可行解




-- Jscript算法结构
VehicleRoutingAlgorithm.SearchSolution()
--SearchStrategy.run(){
	for each SearchStrategyModule in Modules
		module .runAndGetSoln()
	
}

interface SearchStrategyModule

RuinAndRecreateModule implements SearchStrategyModule	
	--member: 
		InsertionStrategy
		RuinStrategy
		
	override runAndGetSoln(){
		RuinStrategy.ruin()
		InsertionStrategy.insert()
	}

AbstractRuinStrategy implements RuinStrategy

RuinRandom extends AbstractRuinStrategy
	

	
	
	
	