#define _CRT_SECURE_NO_WARNINGS
#include"DataBaseClass.h"

MessageClass::MessageClass() :
	Data(),
	Type(UNKNOWN_TYPE)
{

}

MessageClass::MessageClass(const std::string& d, int t):
	Data(d),
	Type(t)
{
}

int MessageClass::GetType() {
	return Type;
}
std::string MessageClass::GetData() {
	return Data;
}

bool MessageClass::Empty() {
	return Data.empty();
}


void MessageClass::Read(std::ifstream& file) {
	std::string word;
	file >> word;		//get "msg"
	file >> word;		//get "type"
	int type;
	file >> type;		//get type
	file >> word;		//get "len"
	int len;
	file >> len;		//get len
	file >> word;		//get "text"
	file.get();
	std::string text;
	for (int i = 0; i < len; ++i) {
		text += (char)file.get();
	}
	this->Type = type;
	this->Data = text;
}

void MessageClass::Write(std::ofstream& file) {
	file << "msg type " << Type << " len " << Data.length() << " text " << Data;
}

DataBaseClass::DataBaseClass() {
	Upload(WAY_TO_DATA);
}


DataBaseClass::~DataBaseClass() {
	Write(WAY_TO_DATA);
}

std::queue<MessageClass>& DataBaseClass::GetQueueByName(const std::string& id) {
	return Data.find(id)->second;
}

std::map<std::string, std::queue<MessageClass>>::iterator  DataBaseClass::isExistQueue(const std::string& id) {
	return Data.find(id);
}
void DataBaseClass::AddQueue(const std::string& str) {
	Data.insert({ str, std::queue<MessageClass>() });
}

bool DataBaseClass::RemoveQueue(const std::string& qid) {
	auto q = isExistQueue(qid);
	if (q == Data.end()) {
		return false;
	}
	else {
		Data.erase(q);
		return true;
	}
}


void DataBaseClass::Upload(const std::string& way) {
	std::ifstream file(way);
	file.seekg(0, std::ios_base::end);
	unsigned int size = file.tellg();
	file.close();
	if (size == 0) {
		NewBase(way);
		Upload(way);
	}
	else {
		std::ifstream file(way);
		std::string word;
		file >> word;		//get "data"
		file >> word;		//get "queues"
		int q_count;
		file >> q_count;	//get queues count
		for (int i = 0; i < q_count; ++i) {
			ReadQueue(file);
		}
	}
	file.close();
}

void DataBaseClass::NewBase(const std::string& way) {
	std::ofstream file(way);
	file << "data" << std::endl;
	file << "queues " << 0 << std::endl;
	file.close();
}

void DataBaseClass::ReadQueue(std::ifstream& file) {
	std::string qid;
	std::string word;
	file >> word;		//get "id"
	file >> qid;		//get id
	file >> word;		//get "count"
	unsigned int count;
	file >> count;		//get count
	std::queue<MessageClass> res;
	for (int i = 0; i < count; ++i) {
		MessageClass msg;
		msg.Read(file);
		res.push(msg);
	}
	Data.insert(std::make_pair(qid, res));
}


void  DataBaseClass::Write(const std::string& way) {
	std::ofstream file(way);
	file.seekp(0, std::ios_base::end);
	unsigned int size = file.tellp();
	if (size != 0) {
		file.close();
		std::remove(way.c_str());
		file.open(way);
	}
	file << "data" << std::endl;
	file << "queues " << Data.size() << std::endl;
	for (auto it = Data.begin(); it != Data.end(); ++it) {
		file << "\t";
		WriteQueue(file, it->first, it->second);
		file << std::endl;
	}
	file.close();
}


void DataBaseClass::WriteQueue(std::ofstream& file,const std::string& qid, std::queue<MessageClass>& q) {
	file << "id " << qid << " count " << q.size() << std::endl;
	while (!q.empty()) {
		file << "\t";
		q.front().Write(file);
		q.pop();
		file << std::endl;
	}
}


std::map<std::string, std::queue<MessageClass>>::iterator DataBaseClass::End() {
	return Data.end();
}