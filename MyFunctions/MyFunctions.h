#pragma once
#include<utility>
#include<string>
#include<vector>
#include<mutex>

std::pair <std::string, std::string> SplitOnIpAndPort(const std::string& way);
bool isNumber(const std::string& s);
long long FileSize(const std::string& s);
std::string GetLastPartOfFileName(const std::string& filename);

//text q len <text>
std::string GenTextCommandForServer(const std::string& qid, const std::string& text);
//text type len <text>
std::string GenTextCommandForClient(const int& type,const std::string& text);
//recieve q
std::string GenReceiveCommandForServer(const std::string& qid);
//pack len ffilename len pack
std::string GenPackCommandForServer(const std::string& filename, const std::string& pack);
//file qid len name
std::string GenFileCommandForServer(const std::string& qid, const std::string& filename);
//endfile <len> <filename> <length>
std::string GenEndFileCommandForServer(const std::string& qid, const std::string filename, unsigned int length);

//pack len ffilename len pack
std::string GenPackCommandForClient(const std::string& filename, const std::string& pack);

std::string GenEndFileCommandForClinet();

void State(std::mutex& d, const std::string& text);

void ConnectionState(std::mutex& d, const std::string& text);

void CommandState(std::mutex& d, const std::string& command, const std::string& text);
void CommandSystemState(std::mutex& d, const std::string& text);