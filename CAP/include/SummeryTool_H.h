//头文件包含<windows.h>
#include <windows.h>
#ifndef SummeryTool_H
#define SummeryTool_H

namespace Summery
{
	//基于QueryPerformanceFrequency，QueryPerformanceCounter
	//默认为微秒
	class StopWatch
	{
	public:
		StopWatch() : elapsed(0)
		{
			QueryPerformanceFrequency(&freq);
		}
		~StopWatch(){}

		void Start()
		{
			QueryPerformanceCounter(&begin_time);
		}
		void Stop()
		{
			LARGE_INTEGER end_time;
			QueryPerformanceCounter(&end_time);
			elapsed += (end_time.QuadPart - begin_time.QuadPart) * 1000000 / freq.QuadPart;
		}
		void Restart()
		{
			elapsed = 0;
			Start();
		}

		//微秒
		double Elapsed()
		{
			return static_cast<double>(elapsed);
		}
		//毫秒
		double Elapsed_ms()
		{
			return elapsed / 1000.0;
		}
		//秒
		double Elapsed_s()
		{
			return elapsed / 1000000.0;
		}

		void printElapsed(const std::string& unit = "s") {
			long long elap_time = elapsed;
			if (unit == "s") { elap_time = Elapsed_s(); }
			else if (unit == "ms") { elap_time = Elapsed_ms(); }
			else if (unit == "us") { elap_time = Elapsed(); }
			else { std::cerr << "ERROR: invalid input!!\n"; return; }
			std::cout << "ELAPSED TIME: < " << elap_time << " >" << unit << "\n";
		}
	private:
		LARGE_INTEGER freq;
		LARGE_INTEGER begin_time;
		long long elapsed;

	};
}
#endif // !SummeryTool_H

