#pragma once
#include<map>
#include<string>
#include<queue>
#include"../Additional/MyProtClasses.h"
struct MyCommandClass;
struct MyParserClass {
	const std::map<std::string, MyCommandClass*>& CommandMap;
	void Execute(char*& src, int len,std::queue<MyCommandClass*>& dest);
	MyParserClass(const std::map<std::string, MyCommandClass*>&);
};