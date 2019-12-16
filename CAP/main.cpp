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

#include "src\structures\crew_rules.h"
#include "src\structures\param_setting\cost_parameters.h"
#include "src\io\input\input_handle.h"
#include "src\algorithm\optimizer.h"
#include "include\SummeryTool_H.h"
#include "crewDB_mine.h"
#include "csv_reader.h"
#include "csv_impl.h"



#pragma comment(lib, "lib\\debug\\csvReader_mdd.lib")

const vector<string> outDetailFilesName = {
	"day_iter_CG_LP_.lp", "day_iter_CG_LP_.sln", "day_schedule_flt_only_.txt", "day_schedule_crew_only.csv"
}

using namespace std;
using namespace Summery;
int main() {

	srand(0);
	crewCsvReader crew_csv_reader;
	vector<string> objNameSet = { "Flight","Base","Crew","CrewRank","CrewBase", "FlightComposition", "Composition" };
	string input_dir = "data/input/";
	string output_dir = "data/output/";

	


	crew_csv_reader.readMutiTableCsv(input_dir + "ro_input_931.txt");
	auto allTable = crew_csv_reader.datas;

	CrewRules rules;
	rules.setHorizonRules(15, 600, 100, 720, 80, 400, 80, 600);
	rules.setWeekPara(7200, 1200, 2160, 4320);
	Penalty penalty(1, 1, 1000, 1, 1, 1, 1);

	StopWatch sw;
	sw.Start();
	Optimizer opt;
	opt.loadData(allTable, objNameSet);
	opt.loadCrewRules(rules);
	opt.loadPenaltySetting(penalty);
	opt.init();
	sw.Stop();
	cout << "init spend time: " << sw.Elapsed_s() << " s\n";

	opt.createCase();
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
