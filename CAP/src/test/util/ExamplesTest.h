#ifndef EXAMPLES_TEST_H
#define EXAMPLES_TEST_H

#include "../test.h"

#include "../../util/examples.h"


using namespace util;
namespace Test {
	
	static const char* kFileName = "../test/tmp/fiels_utils_test";
	
	inline bool try_create_dir(const char* path, const char* normalized_path) {
		auto rv = DirectoryManager::createDir(path);
		assert(rv == true);

		return DirectoryManager::pathExists(normalized_path);
		
	}

	/*TEST(BaseTest, PathExsits) {
		EXPECT_TRUE(DirectoryManager::pathExists(kFileName));
	}*/
	/*TEST_F(BaseTest, CreateDir) {
		EXPECT_TRUE(try_create_dir("../test/tmp", "../test/tmp"));
		EXPECT_TRUE(try_create_dir(kFileName, "../test/tmp/fiels_utils_test"));
		
	}*/


	inline void TESTcreateDirectery() {
		char default_folder[_MAX_PATH];
		_getcwd(default_folder, _MAX_PATH);

		DirectoryManager::createDir();
		assert(_access(default_folder, 0) == 0);

		DirectoryManager::createDir("createDirecteryTest_Folder");
		std::string cur_folder(default_folder);
		cur_folder += "\\createDirecteryTest_Folder";
		assert(_access(cur_folder.c_str(), 0) == 0);
	}
	
		



}


#endif // !EXAMPLES_TEST_H


