#define _CRT_SECURE_NO_WARNINGS
#include"DataBaseClass.h"


DataBaseClass::DataBaseClass(const char* file) {
	if (!Upload(file)) {
		std::cout << "Creating New DataBase\n";
		NewBase(file);
		Upload(file);
	}
}


DataBaseClass::DataBaseClass() {

}
std::queue<MessageClass>& DataBaseClass::GetQueueByName(const std::string& id) {
	return Data.find(id)->second;
}
void DataBaseClass::AddMessageInQueue(const std::string& id, MessageClass msg) {
	if (isExistQueue(id)) {
		GetQueueByName(id).push(msg);
	}
	else {
		AddQueue(id.c_str());
		GetQueueByName(id).push(msg);
	}
}
MessageClass DataBaseClass::GetFrontMessageInQueue(const std::string& id) {
	if (isExistQueue(id)) {
		return GetQueueByName(id).front();
	}
	return MessageClass();
}
void DataBaseClass::PopMessageInQueue(const std::string& id) {
	if (isExistQueue(id)) {
		GetQueueByName(id).pop();
	}
}

bool DataBaseClass::isExistQueue(const std::string& id) {
	if (Data.find(id) == Data.end()) {
		return false;
	}
	else {
		return true;
	}
}

void DataBaseClass::NewBase(const char* str) {
	FILE* file = fopen(str, "wb");
	rapidjson::Document doc;
	auto& allocator = doc.GetAllocator();

	rapidjson::Document Vector;
	Vector.SetArray();
	doc.SetObject().AddMember("size", 0, allocator).AddMember("data", Vector, allocator);

	char writeBuffer[10000];
	rapidjson::FileWriteStream os(file, writeBuffer, sizeof(writeBuffer));
	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	doc.Accept(writer);
	fclose(file);
}

void DataBaseClass::AddQueue(const char* str) {
	Data.insert({ str, std::queue<MessageClass>() });
}
bool DataBaseClass::RemoveQueue(const char* str) {
	auto iter = Data.find(str);
	if (iter == Data.end()) {
		return false;
	}
	else {
		Data.erase(str);
		return true;
	}
}

bool DataBaseClass::Upload(const char* file) {
	FILE* f = fopen(file, "rb");
	if (!f) {
		std::cout << "Data is not opened!\n";
		//fclose(f);
		return false;
	}
	File = std::string(file);
	char readBuffer[10000];
	rapidjson::FileReadStream is(f, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;
	d.ParseStream(is);

	int size = d["size"].GetInt();
	auto Array = d["data"].GetArray();
	for (int i = 0; i < size; ++i) {
		auto Queue = Array[i].GetObject();
		std::string qName = Queue["name"].GetString();
		int qSize = Queue["size"].GetInt();
		auto qData = Queue["data"].GetArray();
		std::queue<MessageClass> q;
		for (int j = 0; j < qSize; ++j) {
			auto Msg = qData[j].GetObject();
			std::string data = Msg["data"].GetString();
			int type = Msg["type"].GetInt();
			q.push(MessageClass(data, type));
		}
		Data.insert({ qName, q });
	}
	std::cout << "Upload complete : uploaded " << Data.size() << " queues\n";
	fclose(f);
	return true;
}

void DataBaseClass::Write(const char* str) {
	FILE* file = fopen(str, "wb");
	rapidjson::Document doc;
	auto& allocator = doc.GetAllocator();

	rapidjson::Value json_val;
	json_val.SetInt(Data.size());
	doc.SetObject().AddMember("size", json_val, allocator);
	rapidjson::Document jsonData;
	jsonData.SetArray();
	for (auto iter = Data.begin(); iter != Data.end(); ++iter) {
		rapidjson::Document jsonQueue;
		json_val.SetString(iter->first.c_str(), allocator);
		jsonQueue.SetObject().AddMember("name", json_val, allocator);
		json_val.SetInt(iter->second.size());
		jsonQueue.AddMember("size", json_val, allocator);
		rapidjson::Document jsonDataQueue;
		jsonDataQueue.SetArray();
		int size = iter->second.size();
		for (int i = 0; i < size; ++i) {
			rapidjson::Document jsonMessage;
			json_val.SetString(iter->second.front().Data.c_str(), allocator);
			jsonMessage.SetObject().AddMember("data", json_val, allocator);
			json_val.SetInt(iter->second.front().Type);
			jsonMessage.AddMember("type", json_val, allocator);
			iter->second.pop();
			jsonDataQueue.PushBack(jsonMessage, allocator);
		}
		jsonQueue.AddMember("data", jsonDataQueue, allocator);
		jsonData.PushBack(jsonQueue, allocator);
	}


	doc.AddMember("data", jsonData, allocator);

	char writeBuffer[10000];
	rapidjson::FileWriteStream os(file, writeBuffer, sizeof(writeBuffer));
	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	doc.Accept(writer);
	fclose(file);
}

MessageClass::MessageClass(std::string m, int t) : Data(m), Type(t) {}
MessageClass::MessageClass(std::string m) : Data(m), Type(STRING) {};

DataBaseClass::~DataBaseClass() {
	if (!File.empty())
		Write(File.c_str());
}