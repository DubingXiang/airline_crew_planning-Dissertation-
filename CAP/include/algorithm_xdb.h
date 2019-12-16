#pragma once
#ifndef ALGORITHM_XDB_H
#define ALGORITHM_XDB_H
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <string>

//! return true if <vec> contains <elem>
template<class T>
bool isContain(const std::vector<T>& vec, T elem) {
	if (std::find(vec.begin(), vec.end(), elem) == vec.end()) {
		return false;
	}
	return true;
}
//! return true if <vec> contains <elem>
template<class Key, class Value>
bool isContainKey(const std::map<Key, Value>& map, Key key) {
	if (map.find(key) == map.end()) {
		return false;
	}
	return true;
}

//! return false if <vec> not contains <elem> 
template<class T>
bool removeElem(std::vector<T>* vecPtr, T elem) {
	std::vector<T>::iterator it = vecPtr->begin();
	for (; it != vecPtr->end();) {
		if (*it == elem) {
			it = vecPtr->erase(it);
		}
		else {
			++it;
		}
	}

	if (it == vecPtr->end()) {
		return false;
	}
	return true;
}

//! release vector which store pointers of T
template <class T>
void releaseVector(std::vector<T*>* vecPtr) {
	std::vector<T*>::iterator it = vecPtr->begin();
	for (; it != vecPtr->end(); ++it) {
		if (*it != NULL) {
			delete *it;
			*it = NULL;
		}		
	}
	vecPtr->clear();
}


//void split_db(const std::string& str, const std::string& delimeters, std::vector<std::string>* outVec) {
//	if (str == "")
//		return;
//	//在字符串末尾也加入分隔符，方便截取最后一段
//	std::string strs = str + delimeters;
//	size_t pos = strs.find(delimeters);
//
//	while (pos != strs.npos) {
//		std::string temp = strs.substr(0, pos);
//		outVec->push_back(temp);
//		//去掉已分割的字符串,在剩下的字符串中进行分割
//		strs = strs.substr(pos + 1, strs.size());
//		pos = strs.find(delimeters);
//	}
//}




#endif // !ALGORITHM_XDB_H