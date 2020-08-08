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
	void argument_parsing(MyCharStreamClass& stream) override {
		char* word;
		stream.GetWord(word);
		params.push_back(word);
		word = nullptr;
		stream.GetWord(word);
		params.push_back(word);
		int len = std::stoi(word);
		word = nullptr;
		stream.Get();
		stream.Get(word, len);
		stream.Get();
		params.push_back(word);
		word = nullptr;
	}
	void copy(MyCommandClass*& com) const override {
		com = new TextCommand();
	}
};