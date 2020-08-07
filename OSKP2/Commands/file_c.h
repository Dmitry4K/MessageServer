#pragma once
#include"../ServerClass.h"
#include"../DataBaseClass.h"
#include"../../MyFunctions/MyFunctions.h"
#include<fstream>

//file qid len name
struct FileCommand : MyCommandClass {
	FileCommand() {
		count = 3;
		name = "Getting File";
	}
	void execute(ServerClass* node) const override {
		node->DataMutex.lock();
		auto& Data = node->GetData();
		auto& qid = params[0];
		auto& filename = params[2];
		auto it_msg = Data.isExistQueue(qid);
		if (it_msg == Data.End()) {
			Data.AddQueue(qid);
		}
		std::remove(std::string(FOLDER + filename).c_str());
		Data.GetQueueByName(qid).push(MessageClass(filename, INGURED_TYPE));
		node->DataMutex.unlock();
	}
	void argument_parsing(std::istringstream& stream) override {
		long long file_size;
		std::string word;
		stream >> word;
		params.push_back(word);
		stream >> word;
		params.push_back(word);
		file_size = std::stoll(word);
		stream.get();
		word.clear();
		for (int i = 0; i < file_size - 1; ++i) {
			word += (char)stream.get();
		}
		stream.get();
		params.push_back(word);
	}
	void copy(MyCommandClass*& com) const override {
		com = new FileCommand();
	}
};
