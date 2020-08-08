#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>

#pragma comment (lib,"Ws2_32.lib")
#include <winsock2.h>
#include<Windows.h>
#include<string>
#include<utility>
#include<vector>
#include<queue>
#include<mutex>
#include<thread>
#include<sstream>
#include"DataBaseClass.h"
#include"SocketMenedger.h"
#include"MyServerParser.h"
#include"../MySockets/MySockets.h"
#include"../Additional/MyProtClasses.h"
#include"../MyCharStream/MyCharStream.h"


const int ACTIVE_SOCKETS_COUNT = 200;
const int DEFAULT_SLEEP_TIME = 10;	//100
const int DEFAULT_COUNT = 1000;	//10
const int OFF = 0;
const int ON = 1;

struct MyCommandClass;
struct ServerClass;

const int CHECK_TIME = 10000;
const int ACTIVE = 1;
const int NOT_ACTIVE = 0;
const int IN_USED = 2;

struct ThreadMenedgerClass {
private:
	ServerClass* Server = nullptr;
	std::vector<std::pair<std::thread, int*>> container;
	std::queue<int> used;
	std::mutex door;
	int state = NOT_ACTIVE;
	std::thread TimeChecker;
	friend void ThrMenExecuteFunction(MyCommandClass*, int*, ServerClass* Server);
	friend void TimeCheckerFunction(ThreadMenedgerClass* menedger);
public:
	void SetServer(ServerClass* Server);
	ThreadMenedgerClass();
	ThreadMenedgerClass(ServerClass* Server);
	~ThreadMenedgerClass();
	void Add(MyCommandClass*);
};

void ThrMenExecuteFunction(MyCommandClass*, int*, ServerClass* Server);
void TimeCheckerFunction(ThreadMenedgerClass* menedger);

struct ServerClass {
private:
	MySocketClass HostSocket;
	SocketMenedger ActiveSockets;
	DataBaseClass Data;
	MyParserClass Parser;
	ThreadMenedgerClass Threads;
	std::map<std::string, MyCommandClass*> CommandMap;

	std::thread ExecuteThread;
	std::thread ReceiveThread;
	std::thread AcceptThread;

	int ExecuteThreadState = OFF;
	int ReceiveThreadState = OFF;
	int AcceptThreadState = OFF;

	MyProtQueue<MyCommandClass*> Commands;
	void BlockThread();
public:
	std::mutex DataMutex;
	std::mutex CoutMutex;
	ServerClass(int argc, char* argv[]);
	ServerClass(const std::string& adr);
	ServerClass();
	~ServerClass();

	void Start();	
	MySocketClass& GetSocket();
	DataBaseClass& GetData();

	friend void ReceiveFunction(ServerClass* Server);
	friend void ExecuteFunction(ServerClass* Server);
	friend void AcceptFunction(ServerClass* Server);
	friend MyCommandClass;
};
void ReceiveFunction(ServerClass* Server);
void ExecuteFunction(ServerClass* Server);
void AcceptFunction(ServerClass* Server);


struct MyCommandClass {
	int count = 0;
	std::vector<char*> params;
	std::string name;
	int socket = 0;
	virtual void execute(ServerClass* node) const = 0;
	virtual void argument_parsing(MyCharStreamClass& stream) = 0;
	virtual void copy(MyCommandClass*&) const = 0;
	~MyCommandClass() {
		for (int i = 0; i < params.size(); ++i) {
			delete[] params[i];
		}
	}
}; 