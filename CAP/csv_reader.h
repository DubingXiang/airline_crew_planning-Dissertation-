#ifndef CSV_READER_H
#define CSV_READER_H
#include <string>
#include <memory>
#include <vector>
#include <map>

using namespace std;

#define TableSeparator "------"

	//csvParser接口类
	class csvParser {
	public:
		virtual string toCsv(vector<string>& headers, void* obj) = 0;
		//virtual void fromCsv(vector<string>& headers, std::string line, void* obj) = 0;
		virtual void fromCsv(vector<string>& headers, int index, char* value, void* obj) = 0;
		virtual vector<string>& getDefaultHeaders() = 0;
		virtual void* createInstance() = 0;
		virtual void deleteInstance(void* obj) = 0;
		bool isAvailable() { return true; }
		virtual void init(vector<string>& headers) = 0;//derived by parsers
	};

extern long long dbgTestTimer;

#define SPLITER_BUF_SIZE 16 * 1024
#define SPLITER_WORD_SIZE 1024
	class csvSpliter {
	public:
		csvSpliter(char delim = '^');
		char * getNextWord(); //EOL: return null, Other: return word
		char * getBuf();      //for istream.getline()
		int count();
		void reset();         //clear buf/word
	private:
		char buf[SPLITER_BUF_SIZE];
		char word[SPLITER_WORD_SIZE];
		int index = 0;
		char delim;           //word separator
	};

	//--------------------------------
	//读写csv类
	//1 tableParserMap：tableName -> parser
	//2 getParser：根据tableName获取parser
	//3 createInstance：根据tableName调用parser创建数据实例
	//4 readMutiTableCsv：读取多表csv文件，表段落间按 -----划分
	//5 datas：保存读入数据存入，按tableName划分
	//6 读取格式
	//  ------ TableName1: ...
	//  tableHeader1, tableHeader2, ...
	//  ...
	//  ...
	//  ------ TableName2: ...
	//  tableHeaderA, tableHeaderB, ...
	//7 使用方法
	//  实现 csvParser类，关联对应数据类 
	//--------------------------------
	class csvReader {
	public:
		void readCsvAttribute(string filepath);
		void readMutiTableCsv(string filepath);
		void readMutiTableCsvByMemMap(string filepath);
		void write(ofstream& outfile, string tableName, vector<void*>& list, shared_ptr<csvParser> parser);
		virtual shared_ptr<csvParser> getParser(string tableName) = 0;
		virtual string getParserName(string tableName) = 0;
		map<string, shared_ptr<csvParser>> tableParserMap;
		map<string, vector<void*>> datas;
		void clear();
	private:
		string readLine(ifstream& in);
		void parseTableName(string& line);
		void parserLine(string& line);
		void parserLine(const char* line);
		string table;
		vector<string> headers;
		shared_ptr<csvParser> parser;
		csvSpliter spliter; //delim: ^
	};

#endif//CSV_READER_H