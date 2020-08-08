#include"MyClientParser.h"
#include<iostream>
#include<string>
#include<sstream>
#include"ClientClass.h"
#include"../MyCharStream/MyCharStream.h"
MyParserClass::MyParserClass(const std::map<std::string, MyCommandClass*>& cm) : CommandMap(cm) {}

void MyParserClass::Execute(char*& src,int len, std::queue<MyCommandClass*>& dest) {
    char* p_command;
    MyCharStreamClass string_stream(src, len);      //потеря данных
    while (!string_stream.EOS()) {
        string_stream.GetWord(p_command);
        auto com = CommandMap.find(std::string(p_command));
        if (com != CommandMap.end()) {
            MyCommandClass* command_class = nullptr;
            com->second->copy(command_class);
            command_class->argument_parsing(string_stream);
            dest.push(command_class);
        }
        else {
           // std::cout << "Unknown command : " << command << std::endl;
        }
        delete[] p_command;
    }
}