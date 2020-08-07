#include"MyClientParser.h"
#include<iostream>
#include<string>
#include<sstream>
#include"ClientClass.h"
MyParserClass::MyParserClass(const std::map<std::string, MyCommandClass*>& cm) : CommandMap(cm) {}

void MyParserClass::Execute(const std::string& src, std::queue<MyCommandClass*>& dest) {
    std::string command;
    std::istringstream string_stream(src);
    while (!string_stream.eof()) {
        string_stream >> command;
        auto com = CommandMap.find(command);
        if (com != CommandMap.end()) {
            MyCommandClass* command_class = nullptr;
            com->second->copy(command_class);
            command_class->argument_parsing(string_stream);
            dest.push(command_class);
        }
        else {
           // std::cout << "Unknown command : " << command << std::endl;
        }
        command.clear();
    }
}