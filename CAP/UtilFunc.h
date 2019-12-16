#ifndef UTIL_FUNC_H
#define UTIL_FUNC_H
#include <vector>
#include <string>
#include <map>
using namespace std;

template<class T>
T* PI_NEW(int count) {
	T* p = new T[count];
	memset(p, 0, sizeof(T) * count);
	return p;
}

#define ClearVector(type, list) \
	for (int k = 0; k < list.size(); k++) { \
		type* r = (type*)list[k]; \
		delete r; list[k] = NULL; \
					} \
	list.clear();

#define PI_MALLOC(ptr,size) malloc(size); if (!ptr) {printf("error: malloc failed\n");} else { memset(ptr, 0, (size));}
#define PI_FREE(ptr) if (ptr) { free(ptr); ptr = NULL; }
#define PI_DELETE(ptr) if (ptr) {delete ptr; ptr = NULL;}
#define PI_DELETE_ARRAY(ptr)  if (ptr) { delete[] ptr; ptr = NULL; }
#define PI_CHECK_IS_NULL(ptr,msg) if (ptr) {cout << "ERROR: mem leak for " << msg << endl; throw "mem leak";}

long long getTimestampNano();

//从time_t获取对应日期是日期星期几
//return:  sun:0, mon:1, ...
int getWdayOfUtc(time_t utc);

//根据time_t计算对应日期的开始时间，即00:00
time_t getStartTimeOfDay(time_t utc);
time_t getLocalDayStartInUTC(time_t utc, int offsetHours);

time_t utcStrToUtc(char * buf);

void strToTm(char* buf, struct tm * m);
string tmToDtStr(struct tm * m);

string utcToMMDDHHmmString(time_t utc);
void utcToMMDDHHmmStr(time_t utc, char * buf, int bufsize);
void utcToYYYYMMDDHHmmss(time_t t, int& year, int& mon, int& date, int& HH, int& mm, int& ss);

//从utc time_t获取对应utc str 形如'yyyy-mm-dd HH:mm:ss' 时间字符串
void utcToUtcStr(time_t utc, char * buf, int bufsize);
string utcToUtcString(time_t utc);
string utcToUtcTzString(time_t utc); //fmt: 2016-1-1T23:59
//从utc time_t获取对应utc 形如'yyyy-mm-dd ' 日期字符串
void utcToUtcDtStr(time_t utc, char * buf, int bufsize);
string utcToUtcDtString(time_t utc);
//支持format: yyyy-mm-dd, yyyymmdd
string utcToUtcDtString(time_t utc, char * format); 
void utcToLocalDtStr(time_t utc, char * buf, int bufsize);
string utcToUtcHHMM(time_t t);

time_t utcToLocal(time_t utc);
time_t localToUtc(time_t loc);
time_t dateStrToTime(string str); //'20180101' --> 2018-1-1 00:00

//从YYYYMMDD HHmmss获取 time_t
time_t makeTimeT(int year, int mon, int day, int hour, int min, int sec);


//get time from 1970-1-1 08:00 in seconds
time_t makeUTC(int year, int mon, int day, int hour, int min, int sec);

string numberToTimeHHMM(int num);
void timeTmToStr(tm * input, char * buf, int bufsize);
int  dayOfYear(int month, int mday);

//get time from 1970-1-1 08:00 in seconds
time_t makeLoc(int year, int mon, int day, int hour, int min, int sec);
bool checkDateTimeStr(const char* str);

//计算time_t时间段对应天数
int getDaysByDuration(time_t utcBeg, time_t utcEnd);

//检查字符串是否符合格式 HH:mm
bool isHHmm(const char * str);
//01:30 --> 90, return -1 on err;
int hhmmToMinutes(const char * str);
//str "12:20" -> int 1220
int hhmmStrToHHmmInt(string& hhMmStr);
int hhmmStrToMinutes(string& hhMmStr);

//vector中是否包含item
bool isContains(vector<string>& list, string item);
bool isContains(vector<int>& list, int item);

//list equal
bool isEqualIntList(vector<int>& list1, vector<int>& list2);

//list join, default delim=' '
string joinStrList(vector<string>& list);
string joinStrList(vector<string>& list, string delim);
string joinIntList(vector<int>& list);
string joinIntList(vector<int>& list, string delim);
string joinMapToStr(map<string, int>& m);
//获取当年的结束时间 time_t
time_t getYearEnd(time_t time);
int isAlphaBeta(char ch);
int isDigit(char ch);
int retriveIntFromStr(string str, int defaultValue);

//将str中所有from替换为to，会修改输入参数str内容
void replaceAll(std::string& str, const std::string& from, const std::string& to);

//格式化path文件路径，合并相邻文件分隔符
void formatFilePath(std::string& path);

//字符编码转换
void utf8ToGbk(const char* utf8, char* gbkBuf, int gbkBufSize, int * realSize);
string UTF8ToGBK(const std::string& strUTF8);
void strToupper(char* buf, int size);
string strToUpper(string str);
string strToLower(string str);

string& trim(string &s);

bool strEndsWith(string& value, string ending);

void split(const std::string &s, char delim, std::vector<std::string> &elems);
void split(const char * input, char delim, vector<int> &result);

//获取list中不存在与compareTo的部分
void makeDifferentList(vector<long long>& list, vector<long long>& compareTo, vector<long long>& result);

size_t hashString(string& str);

size_t getThreadId();

string formatInt(int n);


//按username=xxxx方式获取参数值xxxx
string retriveStrArgInCmdline(int argc, char** argv, char* argName);
int retriveIntArgInCmdline(int argc, char** argv, char* argName);
char* findArgInCmdline(int argc, char** argv, char* argName);
bool checkArgInCmdLine(int argc, char** argv, char* argName);

bool isValueYesTrueOne(const char* value); //"TRUE"/"Yes"/"1" -> 1, else -> 0
int getNum(const char a[]);

#endif//UTIL_FUNC_H1