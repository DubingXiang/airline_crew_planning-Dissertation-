#pragma once
#ifndef CONF_PARAM_H_
#define CONF_PARAM_H_

/*****************************************************************************
*  Open Mathmatical Optimization library                                     *
*  Copyright (C) 2019 Dubing.Xiang.                                          *
*                                                                            *
*                                                                            *
*  @file     conf-param.h                                                       *
*  @brief    读取相关常量等参数，便于控制流程                                                    *
*  Details.                                                                  *
*                                                                            *
*  @author   Dubing.Xiang                                                    *
*  @email    1059361402@qq.com                                               *
*  @version  1.0.0.1(版本号)                                                 *
*  @date     2019-12-18                                                      *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*****************************************************************************/
#include <map>
#include <string>
#include <iostream>
#include <io.h>
#include <direct.h>
#include <windows.h>

namespace util {
	
	class WorkDirConfig;
	class DirectoryManager;

	/**
	 * @class ConfigReader
	 * @brief 读取配置数据
	 *
	**/
	class ConfigReader {
	public:
		ConfigReader(WorkDirConfig* config);
		~ConfigReader();

		/*void read(const char* filePathSeperator, const char* fileNameSeperator,
			const char* inputDataDir, const char* outputDataDir,
			const char* finalScheduleDirName,
			const char* outputPeriodDirName,
			const char* outputCplexDirName) {
			this->_work_dir_config->setConfig(filePathSeperator, fileNameSeperator,
				inputDataDir, outputDataDir,
				finalScheduleDirName,
				outputPeriodDirName,
				outputCplexDirName);
		}*/


		void readDefault();
		

		/*
		 * @brief TODO
		 * @detail
		 *
		 * @param
		 *
		 * @return
		 *     @retval 返回值说明
		 * @attention
		 * @warning
		 * @exception
		 *
		 **/
		void read(const std::string& configFile);

		

		/*
		 * @brief 创建输入输出数据文件夹
		 * @detail
		 *
		 * @param
		 *
		 * @return
		 *     @retval 返回值说明
		 * @attention
		 * @warning
		 * @exception
		 *
		 **/
		void prepareWorkDir();


	private:
		WorkDirConfig* _work_dir_config = nullptr;
	};


	class WorkDirConfig {
		/*
			 * data
			 * --input
			 * --output
			 *   --whole_horizion
			 *     --schedule_whole_horizion.txt
			 *     --crew_workload_whole_horizion.txt
			 *   --period
			 *     --period_1
			 *       --schedule_period_1.txt
			 *       --crew_workload_period_1.txt
			 *       --cplex_file
			 *         --model_iter_1.lp
			 *         --soln_iter_1.sln
			 *     --period_2
			 *     --period_n
			 *
			 */
	public:
		
		static WorkDirConfig& getInstance();
		void setConfig(const char* filePathSeperator, const char* fileNameSeperator,
			const char* inputDataDir, const char* outputDataDir,
			const char* finalScheduleDirName,
			const char* outputPeriodDirName,
			const char* outputCplexDirName);
		void setDefaultConfig();

		const std::string& getFilePathSeperator()const;
		const std::string& getFileNameSeperator()const;
		const std::string& getInputDataDir()const;
		const std::string& getOutputDataDir()const;
		const std::string& getFinalScheduleDirName()const;
		const std::string& getPeriodFileDirName()const;
		const std::string& getCplexFileDirName()const;

		std::map<const std::string, const std::string> OutputFileInCalculatingProcess_Map;

	private:
		WorkDirConfig() {}; //私有构造函数
		//WorkDirConfig(const WorkDirConfig& other); //拷贝构造函数
		//WorkDirConfig& operator=(const WorkDirConfig &);
		//~WorkDirConfig() ;

		std::string _filePathSeperator = "/"; //< 文件路径分隔符，默认为"/"
		std::string _fileNameSeperator = "_"; //< 文件名分隔符，默认为"_"
		std::string _inputDataDir = "data/input"; //< 输入数据文件夹，相对路径，默认为"data/input"
		std::string _outputDataDir = "data/output"; //< 输出数据文件夹，相对路径，默认为"data/output"

		std::string _outputFinalScheduleDirName = "whole_horizon"; //< 最终输出计划文件的文件夹名称
		std::string _outputPeriodFileDirName = "period"; //< 每个优化period的输出数据文件夹名称
		std::string _outputCplexFileDirName = "cplex_file"; //< cplex相关的输出数据文件夹名称

	};


	/**
		 * @class DirectoryManager
		 * @brief 文件夹类
		 *
		 * 输入输出以及过程数据文件存放的文件夹
		 *
		**/
	class DirectoryManager
	{
	public:
		DirectoryManager() {};
		~DirectoryManager() {};
		static const char* kFilePathSeperator_db;

		static bool createDir(const std::string& path = "defaultFolder");

		static bool pathExists(const char* path);

	private:


	};
	


	/**
	 * @class 
	 * @brief 算例的参数 TODO
	 *
	 * 随机创建算例，比如随机选择部分crew有某种资质
	 *
	**/
	class ExampleConfig {


	};
}


#endif // !CONF_PARAM_H_
