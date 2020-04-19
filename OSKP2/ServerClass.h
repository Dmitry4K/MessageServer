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
#include<thread>
#include"DataBaseClass.h"
#include"SocketMenedger.h"
class ServerClass {
private:
	const std::string WAY_TO_DATA = "data.json";
	DataBaseClass Data;
	int Port;
	int SocketHandle;
	SocketMenedger ActiveSockets;
	std::queue<std::pair<int, char*>> HandleCommand;
	bool isExit = false;
	std::thread Handler;
	std::thread ExecuteStarter;
	std::thread Console;
	std::vector <std::pair<int, std::thread>> Executors;
	void Accept();
	void Close();
	void Start();

public:
	const static int CLIENT_CLASS_BUFFER_SIZE = 100;
	const static int EXECUTOR_COUNT = 10;
	const static int ACTIVE_SOCKETS_COUNT = 10;
	const static int FILE_BUFFER_SIZE = 10000;
	ServerClass(int argc, char* argv[]);
	ServerClass(int p);
	~ServerClass();

	int GetPort();
	int Send(int Socket, const char* b);
	int Send(int Socket, const char* b, int bytes);
	int Recieve(int recvSocket, char**);
	int Recieve(int recvSocket, char**, int b);
	int Recieve(char**);
	bool IsExit();
	void ConsoleStart();
	friend void HandlerFunction(ServerClass* Server);
	friend void ExecuteStarterFunction(ServerClass* Server);
	friend void ExecuteFunction(ServerClass* Server, int s, char* msg, int& res);
	friend void ConsoleFunction(ServerClass* Server, HANDLE pipe);
};
void HandlerFunction(ServerClass* Server);
void ExecuteStarterFunction(ServerClass* Server);
void ExecuteFunction(ServerClass* Server, int s, char* msg, int& res);
void ConsoleFunction(ServerClass* Server, HANDLE pipe);
