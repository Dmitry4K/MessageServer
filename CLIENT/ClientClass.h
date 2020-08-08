#pragma once
#include<iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib,"Ws2_32.lib")
#include <winsock2.h>
#include<Windows.h>
#include<string>
#include<utility>
#include<map>
#include"../Additional/SimpleTimer.h"
#include"../MySockets/MySockets.h"
#include"../Additional/MyProtClasses.h"
#include"../MyCharStream/MyCharStream.h"
#include"MyClientParser.h"
struct MyCommand;
const int ATTEMPT_COUNT = 4;
const int ATTEMPT_TIME = 1000;
const int ACTIVE_SOCKETS_COUNT = 10;
const int DEFAULT_SLEEP_TIME = 500;
const int OFF = 0;
const int ON = 1;
const int FILE_TYPE = 0;
const int STRING_TYPE = 1;
const std::string DEFAULT_FOLDER = "D:\\dev\\OSKP2\\client_storage\\";

const int PACK_SIZE = 10000;
struct MyCommandClass;
class ClientClass {
private:
	MySocketClass HostSocket;
	Timer MyTimer;
	std::map<std::string, MyCommandClass*> CommandMap;

	std::thread ReceiveThread;
	int ReceiveThreadState = OFF;
	MyParserClass Parser;
	std::queue<MyCommandClass*> Commands;
	int Send(const std::string& text);//CHAR*

	std::string Folder = DEFAULT_FOLDER;
	inline void GetPacks();
public:
	ClientClass(const std::string& adr);
	~ClientClass();

	void Start();
	int Connect(const std::string& adr);
	int Disconnect();

	const MySocketClass& GetSocket() const ;

	int SendText(const std::string& qid, const std::string& text);
	int SendFile(const std::string& qid, const std::string& file);
	int Receive(const std::string& qid, std::string& dest);
	const std::string& GetFolder() const;
	void SetFolder(const std::string& f);

	friend void ReceiveFunction(ClientClass* Server);
};
void ReceiveFunction(ClientClass* Server);

struct MyCommandClass {
	int count = 0;
	std::vector<char*> params;
	virtual void execute(ClientClass *node) const = 0;
	virtual void argument_parsing(MyCharStreamClass& stream) = 0;
	virtual void copy(MyCommandClass*&) const = 0;
	~MyCommandClass() {
		for (int i = 0; i < params.size(); ++i) {
			delete[] params[i];
		}
	}
};