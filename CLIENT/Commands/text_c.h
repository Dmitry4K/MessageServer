#include"../ClientClass.h"
#include<sstream>

struct ClientTextCommand : MyCommandClass {
	ClientTextCommand() {
		count = 3;
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
		for (int i = 0; i < len - 1; ++i) {
			word += (char)stream.get();
		}
		stream.get();
		params.push_back(word);
	}
	void copy(MyCommandClass*& com) const override {
		com = new ClientTextCommand();
	}
	void execute(ClientClass* client) const override {

	}
};