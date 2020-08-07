#include"../ClientClass.h"
#include<sstream>
#include<fstream>

struct ClientPackCommand : MyCommandClass {
	ClientPackCommand() {
		count = 4;
	}
	void argument_parsing(std::istringstream& stream) override {
		long long file_size;
		std::string word;
		stream >> word;
		params.push_back(word);
		file_size = std::stoll(word);
		stream.get();
		word.clear();
		for (int i = 0; i < file_size; ++i) {
			word += (char)stream.get();
		}
		//stream.get();
		params.push_back(word);
		stream >> word;
		params.push_back(word);
		file_size = std::stoul(word);
		stream.get();
		word.clear();
		for (int i = 0; i < file_size; ++i) {
			word += (char)stream.get();
		}
		params.push_back(word);
	}
	void copy(MyCommandClass*& com) const override {
		com = new ClientPackCommand();
	}
	void execute(ClientClass* client) const override {
		std::ofstream file(std::string(client->GetFolder() + params[1]), std::ios::binary | std::ios::out | std::ios::app);
		file.seekp(std::ios_base::end);
		file << params[3];
		file.close();
	}
};