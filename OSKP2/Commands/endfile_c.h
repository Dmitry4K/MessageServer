#pragma once
#include"../ServerClass.h"
#include"../DataBaseClass.h"
#include"../../MyFunctions/MyFunctions.h"
#include<fstream>
//endfile <len> <filename> <length>
struct EndFileCommand : MyCommandClass {
	EndFileCommand() {
		count = 3;
		name = "Sending EOF";
	}
	void execute(ServerClass* node) const override {

	}
	void argument_parsing(std::istringstream& stream) override {
		unsigned int file_size;
		std::string word;
		stream >> word;
		params.push_back(word);
		file_size = std::stoul(word);
		stream.get();
		word.clear();
		for (int i = 0; i < file_size - 1; ++i) {
			word += (char)stream.get();
		}
		stream.get();
		params.push_back(word);
		stream >> word;
		params.push_back(word);
	}
	void copy(MyCommandClass*& com) const override {
		com = new EndFileCommand();
	}
};
