#include"../ClientClass.h"
#include<sstream>

struct ClientEndFileCommand : MyCommandClass {
	ClientEndFileCommand() {
		count = 1;
	}
	void argument_parsing(std::istringstream& stream) override {
		params.push_back("endfile");
	}
	void copy(MyCommandClass*& com) const override {
		com = new ClientEndFileCommand();
	}
	void execute(ClientClass* client) const override {

	}
};