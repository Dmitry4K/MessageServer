#include"../ClientClass.h"
#include<sstream>

struct ClientTextCommand : MyCommandClass {
	ClientTextCommand() {
		count = 3;
	}
	void argument_parsing(MyCharStreamClass& stream) override {
		char* word;
		stream.GetWord(word);
		params.push_back(word);
		word = nullptr;
		stream.GetWord(word);
		params.push_back(word);
		int len = std::stoi(word);
		stream.Get();
		word = nullptr;
		stream.Get(word, len);
		stream.Get();
		params.push_back(word);
		word = nullptr;
	}
	void copy(MyCommandClass*& com) const override {
		com = new ClientTextCommand();
	}
	void execute(ClientClass* client) const override {

	}
};