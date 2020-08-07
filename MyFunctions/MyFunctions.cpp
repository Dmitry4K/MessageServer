#include"MyFunctions.h"
#include<iostream>
#include<fstream>


std::pair <std::string, std::string> SplitOnIpAndPort(const std::string& way) {
	long long split_point = -1;
	for (long long i = 0; i < (long long)way.length(); ++i) {
		if (way[i] == ':') {
			split_point = i;
			break;
		}
	}
	return split_point != -1 ? std::make_pair<std::string, std::string>(way.substr(0, split_point), way.substr(split_point + 1)) : std::make_pair<std::string, std::string>("", "");
}

bool isNumber(const std::string& s) {
	for (size_t i = 0; i < s.length(); ++i) {
		if (s[i] < '0' || s[i] > '9') {
			return false;
		}
	}
	return true;
}


long long FileSize(const std::string& s) {
	std::ifstream file(s, std::ios::binary);
	file.seekg(0, std::ios_base::end);
	long long size = file.tellg();
	file.close();
	return size;
}

std::string GetLastPartOfFileName(const std::string& filename) {
	std::string::const_iterator it = filename.begin();
	std::string::const_iterator start_point = filename.begin();
	while (it != filename.end()) {
		if (*it == '\\') {
			start_point = it + 1;
		}
		++it;
	}
	return std::string(start_point, filename.end());
}

//text q len <text>
std::string GenTextCommandForServer(const std::string& qid, const std::string& text) {
	return "text " + qid + ' ' + std::to_string(text.length() + 1) + ' ' + text;
}
//text type len <text>
std::string GenTextCommandForClient(const int& type, const std::string& text) {
	return "text " + std::to_string(type) + ' ' + std::to_string(text.length() + 1) + ' ' + text;
}

//recieve q
std::string GenReceiveCommandForServer(const std::string& qid) {
	return "recieve " + qid;
}


//pack len filename len pack
std::string GenPackCommandForServer(const std::string& filename, const std::string& pack) {
	return "pack " + std::to_string(filename.length()) + ' ' + filename + ' ' + std::to_string(pack.length()) + ' ' + pack;
}
//file qid len name
std::string GenFileCommandForServer(const std::string& qid, const std::string& filename) {
	return "file " + qid + ' ' + std::to_string(filename.length() + 1) + ' ' + filename;
}
//endfile <len> <filename> <length>
std::string GenEndFileCommandForServer(const std::string& qid, const std::string filename, unsigned int length) {
	return "endfile " + std::to_string(filename.length() + 1) + ' ' + filename + ' '+ std::to_string(length);
}


std::string GenPackCommandForClient(const std::string& filename, const std::string& pack) {
	return GenPackCommandForServer(filename, pack);
}
std::string GenEndFileCommandForClinet() {
	return "endfile";
}



void State(std::mutex& d, const std::string& text) {
	d.lock();
	//std::cout << "---------------------------------------------------------------------------------------\n";
	std::cout << text << std::endl;
	d.unlock();
}

void ConnectionState(std::mutex& d, const std::string& text) {
	State(d, "[Connection] : " + text);
}

void CommandState(std::mutex& d, const std::string& command, const std::string& text) {
	State(d, "[" + command+ "] : " + text);
}


void CommandSystemState(std::mutex& d, const std::string& text) {
	State(d, "[SYSTEM] " + text);
}