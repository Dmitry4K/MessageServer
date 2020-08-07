#include"MyServerParser.h"
#include<iostream>
#include<string>
#include<sstream>
#include"ServerClass.h"
#include"../MyFunctions/MyFunctions.h"
MyParserClass::MyParserClass(const std::map<std::string, MyCommandClass*>& cm, std::mutex& mutex) : CommandMap(cm), CoutMutex(mutex) {}

void MyParserClass::Execute(const std::string& src, MyProtQueue<MyCommandClass*>& dest, int socket) {
    std::string command;
    std::istringstream string_stream(src);
    while (!string_stream.eof()) {
        string_stream >> command;
        auto com = CommandMap.find(command);
        if (com != CommandMap.end()) {
            MyCommandClass* command_class = nullptr;
            com->second->copy(command_class);
            CommandState(CoutMutex, com->second->name, "Parsing...");
            command_class->argument_parsing(string_stream);
            command_class->socket = socket;
            dest.push(command_class);
        }
        else {
            CommandState(CoutMutex, "Unknown command", command);
        }
        command.clear();
    }
}