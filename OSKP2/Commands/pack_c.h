#pragma once
#include"../ServerClass.h"
#include"../DataBaseClass.h"
#include"../../MyFunctions/MyFunctions.h"
#include<fstream>

//pack len filename len pack
struct PackCommand : MyCommandClass {
	PackCommand() {
		count = 4;
		name = "Pack";
	}
	void execute(ServerClass* node) const override {
		std::ofstream file(std::string(FOLDER + params[1]), std::ios::binary | std::ios::out | std::ios::app);
		file.seekp(std::ios_base::end);
		file.write(params[3], std::atoi(params[2]));
		file.close();
	}
	void argument_parsing(MyCharStreamClass& stream) override {
		long long file_size;
		char* word;
		stream.GetWord(word);
		params.push_back(word);
		file_size = std::stoll(word);
		stream.Get();
		word = nullptr;
		stream.Get(word, file_size);
		params.push_back(word);
		word = nullptr;
		stream.GetWord(word);
		params.push_back(word);
		file_size = std::stoul(word);
		word = nullptr;
		stream.Get();
		word = nullptr;
		stream.Get(word, file_size);
		params.push_back(word);
		word = nullptr;
	}
	void copy(MyCommandClass*& com) const override {
		com = new PackCommand();
	}
};
