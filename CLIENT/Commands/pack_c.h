#include"../ClientClass.h"
#include<sstream>
#include<fstream>

struct ClientPackCommand : MyCommandClass {
	ClientPackCommand() {
		count = 4;
	}
	void argument_parsing(MyCharStreamClass& stream) override {
		long long file_size;
		char* word;
		stream.GetWord(word);
		params.push_back(word);
		file_size = std::stoll(word);
		word = nullptr;
		stream.Get();
		stream.Get(word, file_size);
		//stream.get();
		params.push_back(word);
		word = nullptr;
		stream.GetWord(word);
		params.push_back(word);
		file_size = std::stoul(word);
		word = nullptr;
		stream.Get();
		stream.Get(word, file_size);
		params.push_back(word);
		word = nullptr;
	}
	void copy(MyCommandClass*& com) const override {
		com = new ClientPackCommand();
	}
	void execute(ClientClass* client) const override {
		std::ofstream file(std::string(client->GetFolder() + params[1]), std::ios::binary | std::ios::out | std::ios::app);
		file.seekp(std::ios_base::end);
		file.write(params[3], std::atoi(params[2]));
		file.close();
	}
};