#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include<queue>
#include<map>
#include<string>
#include<list>

const int FILE_TYPE = 0;
const int STRING_TYPE = 1;
const int INGURED_TYPE = 2;
const int UNKNOWN_TYPE = -1;
const std::string WAY_TO_DATA = "D:\\dev\\OSKP2\\data.txt";
const std::string FOLDER = "D:\\dev\\OSKP2\\server_storage\\";

class MessageClass {
	std::string Data;
	int Type;
public:
	MessageClass();
	MessageClass(const std::string&, int t);

	bool Empty();
	int GetType();
	std::string GetData();
	void Write(std::ofstream& file);
	void Read(std::ifstream& file);
};

class DataBaseClass {
private:
	std::string File;
	std::map<std::string, std::queue<MessageClass>> Data;
	std::list<MessageClass*> Files;
	void ReadQueue(std::ifstream& file);
	void WriteQueue(std::ofstream& file, const std::string& qid, std::queue<MessageClass>& q);
public:
	DataBaseClass();
	~DataBaseClass();
	DataBaseClass(const DataBaseClass&) = delete;

	std::queue<MessageClass>& GetQueueByName(const std::string&);
	void AddQueue(const std::string&);
	std::map<std::string, std::queue<MessageClass>>::iterator isExistQueue(const std::string&);
	std::map<std::string, std::queue<MessageClass>>::iterator End();
	bool RemoveQueue(const std::string&);

	void Upload(const std::string&);
	void Write(const std::string&);
	void NewBase(const std::string&);
};
