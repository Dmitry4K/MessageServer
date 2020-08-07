#pragma once
#include"../ServerClass.h"
//text q len <text>
struct TextCommand : MyCommandClass {
	TextCommand(){
		count = 3;
		name = "Adding new message";
	}
	void execute(ServerClass* node)const override {
		node->DataMutex.lock();
		if (node->GetData().isExistQueue(params[0]) == node->GetData().End()) {
			node->GetData().AddQueue(params[0]);
		}
		node->GetData().GetQueueByName(params[0]).push(MessageClass(params[2], STRING_TYPE));
		node->DataMutex.unlock();
	}
	void argument_parsing(std::istringstream& stream) override {
		std::string word;
		stream >> word;
		params.push_back(word);
		stream >> word;
		params.push_back(word);
		int len = std::stoi(word);
		stream.get();
		word.clear();
		for (int i = 0; i < len-1; ++i) {
			word += (char)stream.get();
		}
		stream.get();
		params.push_back(word);
	}
	void copy(MyCommandClass*& com) const override {
		com = new TextCommand();
	}
};