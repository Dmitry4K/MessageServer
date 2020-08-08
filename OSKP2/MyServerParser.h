#pragma once
#include<map>
#include<string>
#include<queue>
#include"../Additional/MyProtClasses.h"
struct MyCommandClass;
struct MyParserClass {
	const std::map<std::string, MyCommandClass*>& CommandMap;
	std::mutex& CoutMutex;
	void Execute(char*& src,int len, MyProtQueue<MyCommandClass*>& dest, int socket);
	MyParserClass(const std::map<std::string, MyCommandClass*>&, std::mutex&);
};