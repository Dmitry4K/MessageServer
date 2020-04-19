#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include<queue>
#include<map>
#include<string>
#include<rapidjson/writer.h>
#include<rapidjson/document.h>
#include<rapidjson/filewritestream.h>
#include<rapidjson/filereadstream.h>

struct MessageClass {
	const static int FILE = 0;
	const static int STRING = 1;
	std::string Data;
	int Type;
	MessageClass() {}
	MessageClass(std::string);
	MessageClass(std::string, int t);
};

class DataBaseClass {
private:
	std::string File;
	std::map<std::string, std::queue<MessageClass>> Data;
public:
	DataBaseClass();
	DataBaseClass(const char*);
	~DataBaseClass();
	std::queue<MessageClass>& GetQueueByName(const std::string&);
	void AddMessageInQueue(const std::string&, MessageClass);
	MessageClass GetFrontMessageInQueue(const std::string&);
	void PopMessageInQueue(const std::string&);
	bool isExistQueue(const std::string&);
	bool Upload(const char*);
	void Write(const char*);
	void NewBase(const char*);
	void AddQueue(const char*);
	bool RemoveQueue(const char*);
};
