#include "config-reader.h"

namespace utils 
{
	
	/*1.ConfigReader******************************************/
	ConfigReader::ConfigReader(WorkDirConfig* config){
		_work_dir_config = config;
	}
	ConfigReader::~ConfigReader() {
		_work_dir_config = nullptr;
	}

	void ConfigReader::readDefault() {
		this->_work_dir_config->setDefaultConfig();
	}

	// TODO
	void ConfigReader::read(const std::string& configFile) {}

	void ConfigReader::prepareWorkDir() {

		bool create_input_dir = DirectoryManager::createDir(_work_dir_config->getInputDataDir());
		bool create_output_dir = DirectoryManager::createDir(_work_dir_config->getOutputDataDir());

		std::string whole_horizion_dir_path = _work_dir_config->getOutputDataDir()			
			+ _work_dir_config->getFinalScheduleDirName()
			+ _work_dir_config->getFilePathSeperator();
		bool create_finalschedule_dir = DirectoryManager::createDir(whole_horizion_dir_path);
		std::string period_dir_path = _work_dir_config->getOutputDataDir()			
			+ _work_dir_config->getPeriodFileDirName()
			+ _work_dir_config->getFilePathSeperator();
		bool create_periodfile_dir = DirectoryManager::createDir(period_dir_path);

	}
	/*end 1.ConfigReader******************************************/

	/*2.WorkDirConfig******************************************/
	WorkDirConfig& WorkDirConfig::getInstance() {
		static WorkDirConfig m_instance;
		return m_instance;
	}
	void WorkDirConfig::setConfig(const char* filePathSeperator, const char* fileNameSeperator,
		const char* inputDataDir, const char* outputDataDir,
		const char* finalScheduleDirName,
		const char* outputPeriodDirName,
		const char* outputCplexDirName) {

		_filePathSeperator = filePathSeperator;
		_fileNameSeperator = fileNameSeperator;

		_inputDataDir = inputDataDir;
		_outputDataDir = outputDataDir;

		_outputFinalScheduleDirName = finalScheduleDirName;
		_outputPeriodFileDirName = outputPeriodDirName;
		_outputCplexFileDirName = outputCplexDirName;
	}

	void WorkDirConfig::setDefaultConfig() {
		_filePathSeperator = "/";
		_fileNameSeperator = "_";
		_inputDataDir = "data/input";
		_outputDataDir = "data/output";

		_outputFinalScheduleDirName = "whole_horizon";
		_outputPeriodFileDirName = "period";
		_outputCplexFileDirName = "cplex_file";
	}

	const std::string& WorkDirConfig::getFilePathSeperator()const { return _filePathSeperator; }
	const std::string& WorkDirConfig::getFileNameSeperator()const { return _fileNameSeperator; }
	const std::string& WorkDirConfig::getInputDataDir()const { return _inputDataDir; }
	const std::string& WorkDirConfig::getOutputDataDir()const { return _outputDataDir; }
	const std::string& WorkDirConfig::getFinalScheduleDirName()const { return _outputFinalScheduleDirName; }
	const std::string& WorkDirConfig::getPeriodFileDirName()const { return _outputPeriodFileDirName; }
	const std::string& WorkDirConfig::getCplexFileDirName()const { return _outputCplexFileDirName; }

	/*end 2.WorkDirConfig******************************************/

	/*3.DirectoryManager******************************************/
	bool DirectoryManager::createDir(const std::string& path) {
		if (path.empty()) {
			return false;
		}
		if (pathExists(path.c_str())) {
			return true;
		}

		int status = -1;

		char* curr_path = const_cast<char*>(path.c_str());
		std::string built_path = std::string();

		char* next_tok = nullptr;
		curr_path = strtok_s(curr_path, kFilePathSeperator_db, &next_tok);

		while (curr_path != nullptr) {
			built_path.append(curr_path);
			built_path.append(kFilePathSeperator_db);

			status = _mkdir(built_path.c_str());
			curr_path = strtok_s(nullptr, kFilePathSeperator_db, &next_tok);
		}

		if (status == -1) {
			printf("failed to create path [%s]\n", path);
			return false;
		}
		return true;
	}
	bool DirectoryManager::pathExists(const char* path) {
		if (path == nullptr) {
			return false;
		}

		DWORD fileType = GetFileAttributesA(path);
		if (fileType == INVALID_FILE_ATTRIBUTES) {
			return false;
		}

		return (fileType & FILE_ATTRIBUTE_DIRECTORY) == 0 ? false : true;
	}
	const char* DirectoryManager::kFilePathSeperator_db = "/";
	/*end 3.DirectoryManager******************************************/
}