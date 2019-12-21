/*!
 * \file main.cpp
 * \brief program entrance
 *
 * -read input data from csv files
 * -load setting
 * -start optimization
 *
 * \author dubing xiang
 * \version 0.1.0
 * \date 20190823
 */
#include "src\pch.h"

#include "crewDB_mine.h"
#include "csv_reader.h"
#include "csv_impl.h"

#include "include/SummeryTool_H.h"

#include "src/structures/crew_rules.h"
#include "src/problem/problem.h"
#include "src/structures/param_setting/cost_parameters.h"
#include "src/algorithm/optimizer.h"

#include "config-reader.h"
#include "src/util/examples.h"

#pragma comment(lib, "lib\\debug\\csvReader_mdd.lib")


#define ELPP_NO_DEFAULT_LOG_FILE //禁用默认日志文件
#include "src/easylogging/easylogging++.h"
INITIALIZE_EASYLOGGINGPP // easylogging宏定义


#define EXPECT_EQ_DB(expression) assert(expression) == true;

using namespace std;
using namespace Summery;
using namespace util;

int main(int argc, char** argv) {	

	/*el::Configurations conf("src/log/my_log.conf");
	el::Loggers::reconfigureAllLoggers(conf);*/
	
	/*el::Configurations default_conf;
	default_conf.setToDefault();*/
	

	WorkDirConfig dir_config = WorkDirConfig::getInstance();
	ConfigReader config_reader(&dir_config);
	config_reader.readDefault();
	config_reader.prepareWorkDir();
	// tested OK


	int current_location = 0;


	srand(0);
	crewCsvReader crew_csv_reader;
	vector<string> objNameSet = { "Flight","Base","Crew","CrewRank","CrewBase", "FlightComposition", "Composition" };		

	crew_csv_reader.readMutiTableCsv(dir_config.getInputDataDir() + "ro_input_931.txt");
	auto allTable = crew_csv_reader.datas;

	CrewRules rules;
	rules.setHorizonRules(15, 600, 100, 720, 80, 400, 80, 600);
	rules.setWeekPara(7200, 1200, 2160, 4320);
	Penalty penalty(1, 1, 1000, 1, 1, 1, 1);

	StopWatch sw;
	sw.Start();
	
	Concretizer concretizer;
	concretizer.concretizeAll(allTable, objNameSet);
	Problem::ProblemBuilder builder(concretizer);
	Problem* problem = builder.build();
	
	Example random_example;
	set<string> plateau_airport_set;
	plateau_airport_set = random_example.randomCreatePlateauAirptSet(problem->getAirptSet(), 10);
	random_example.randomSetCrewSkillls(const_cast<vector<Opt_CREW*>*>(&problem->geOptCrewSet()),
		problem->getSkillSet(), 0.1);
	random_example.randomSetRankCombination(&rules);
	
	Optimizer opt(problem, &rules, &penalty);
	/*
	opt.loadProblem(*problem);
	opt.loadCrewRules(rules);
	opt.loadPenaltySetting(penalty);
	opt.init(); 
	*/
	sw.Stop();
	cout << "init spend time: " << sw.Elapsed_s() << " s\n";
	
	sw.Restart();
	rules.setSeqMaps();
	sw.Stop();
	cout << "set all permutations of combinations spend time: " << sw.Elapsed_s() << " s\n";
	rules.sortSeqMaps();

	sw.Restart();
	opt.optimize();
	sw.Stop();
	cout << "optimize spend time: " << sw.Elapsed_s() << " s\n";
	cout << "hello world\n";
	system("pause");
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
