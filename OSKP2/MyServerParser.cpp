#include"MyServerParser.h"
#include<iostream>
#include<string>
#include<sstream>
#include"ServerClass.h"
#include"../MyFunctions/MyFunctions.h"
#include"../MyCharStream/MyCharStream.h"
MyParserClass::MyParserClass(const std::map<std::string, MyCommandClass*>& cm, std::mutex& mutex) : CommandMap(cm), CoutMutex(mutex) {}

void MyParserClass::Execute(char*& src,int len, MyProtQueue<MyCommandClass*>& dest, int socket) {
    char* p_command;
    MyCharStreamClass string_stream(src,len);          //потеря данных
    while (!string_stream.EOS()) {
        string_stream.GetWord(p_command);
        auto com = CommandMap.find(std::string(p_command));
        if (com != CommandMap.end()) {
            MyCommandClass* command_class = nullptr;
            com->second->copy(command_class);
            CommandState(CoutMutex, com->second->name,socket,  "Parsing...");
            command_class->argument_parsing(string_stream);
            command_class->socket = socket;
            dest.push(command_class);
        }
        else {
            CommandState(CoutMutex, "Unknown command",socket, std::string(p_command));
        }
        delete[] p_command;
    }
}