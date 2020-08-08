#include"../ClientClass.h"
#include<sstream>

struct ClientEndFileCommand : MyCommandClass {
	ClientEndFileCommand() {
		count = 1;
	}
	void argument_parsing(MyCharStreamClass& stream) override {
		char* msg = new char[8];
		memcpy(msg, "endfile", 8);
		params.push_back(msg);
	}
	void copy(MyCommandClass*& com) const override {
		com = new ClientEndFileCommand();
	}
	void execute(ClientClass* client) const override {

	}
};