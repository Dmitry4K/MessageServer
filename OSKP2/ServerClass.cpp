#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<locale.h>
#include"../Additional/SimpleTimer.h"
#include "ServerClass.h"
#include "Commands/text_c.h"
#include "Commands/recieve_c.h"
#include "Commands/file_c.h"
#include "Commands/pack_c.h"
ServerClass::ServerClass() : 
	ActiveSockets(ACTIVE_SOCKETS_COUNT),
	Parser(CommandMap, CoutMutex),
	Threads(this)
{
	
	CommandMap["text"] = new TextCommand();
	CommandMap["recieve"] = new ReceiveCommand();
	CommandMap["pack"] = new PackCommand();
	CommandMap["file"] = new FileCommand();
}
ServerClass::ServerClass(const std::string& adr) : ServerClass(){
	if (HostSocket.Bind(adr) != 0) {
		CommandSystemState(CoutMutex, "Socket binding error. Closing...");
		HostSocket.Close();
		exit(1);
	}
	CommandSystemState(CoutMutex, "Success binding.");
	Start();
}

ServerClass::ServerClass(int argc, char* argv[]) : ServerClass(std::string(argv[1])){}


void ServerClass::Start() {
	CommandSystemState(CoutMutex, "Server's system thread starting...");
	ExecuteThreadState = ON;
	ReceiveThreadState = ON;
	AcceptThreadState = ON;

	AcceptThread = std::thread(AcceptFunction, this);
	ReceiveThread = std::thread(ReceiveFunction, this);
	ExecuteThread = std::thread(ExecuteFunction, this);

	//setlocale(LC_ALL, "rus");
	BlockThread();
}

void AcceptFunction(ServerClass* Server) {
	CommandSystemState(Server->CoutMutex, "Accept Incoming Connection thread started.");
	while (Server->AcceptThreadState == ON)
	{
		SOCKET acceptS;
		SOCKADDR_IN addr_c;
		int addrlen = sizeof(addr_c);
		acceptS = accept(Server->HostSocket.GetSocketHandle(), (struct sockaddr*) & addr_c, &addrlen);
		if (acceptS != INVALID_SOCKET) {
			///*
			Server->CoutMutex.lock();
			std::cout << "[Connection] : Client connected from "
				<< (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b1 + '0' - 48 << '.'
				<< (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b2 + '0' - 48 << '.'
				<< (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b3 + '0' - 48 << '.'
				<< (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b4 + '0' - 48 << ':'
				<< ntohs(addr_c.sin_port) <<" Handle : " <<acceptS<< std::endl;
			//*/
			Server->CoutMutex.unlock();
			Server->ActiveSockets.GetMutex().lock();
			Server->ActiveSockets.Add(acceptS);
			Server->ActiveSockets.GetMutex().unlock();
		}
		Sleep(DEFAULT_SLEEP_TIME);
	}
}

void ReceiveFunction(ServerClass* Server) {
	CommandSystemState(Server->CoutMutex, "Recieving Messages thread started.");
	char* msg;
	while (Server->ReceiveThreadState == ON) {
		Server->ActiveSockets.GetMutex().lock();
		for (size_t i = 0; i < Server->ActiveSockets.Size(); ++i) {
			int res;
			if ((res = Server->HostSocket.Recieve(Server->ActiveSockets[i], msg)) > 0) { 
				Server->Parser.Execute(msg,res, Server->Commands, Server->ActiveSockets[i]);//парсинг строки на комманды
				delete[] msg;
			}
		}
		Server->ActiveSockets.GetMutex().unlock();
		Sleep(DEFAULT_SLEEP_TIME);
	}
}
//*
void ExecuteFunction(ServerClass* Server) {
	CommandSystemState(Server->CoutMutex, "Execute thread started");
	std::vector<std::string> params;
	while (Server->ExecuteThreadState == ON) {
		Server->Commands.GetMutex().lock();
		if (!Server->Commands.empty()) {
			Server->Threads.Add(Server->Commands.front());
			Server->Commands.pop();
		}
		Server->Commands.GetMutex().unlock();
		Sleep(DEFAULT_SLEEP_TIME);
	}
}
void ServerClass::BlockThread() {
	std::cin.get();
	CommandSystemState(CoutMutex, "Exiting...");
}
ServerClass::~ServerClass() {
	ExecuteThreadState = OFF;
	ReceiveThreadState = OFF;
	AcceptThreadState = OFF;

	if (AcceptThread.joinable()) {
		AcceptThread.join();
	}
	if (ExecuteThread.joinable()) {
		ExecuteThread.join();
	}
	if (ReceiveThread.joinable()) {
		ReceiveThread.join();
	}
	HostSocket.Close();
}


MySocketClass& ServerClass::GetSocket() {
	return HostSocket;
}
DataBaseClass& ServerClass::GetData() {
	return Data;
}

void ThreadMenedgerClass::Add(MyCommandClass* command) {
	door.lock();
	///*
	if (!used.empty()) {
		*(container[used.front()].second) = ACTIVE;
		container[used.front()].first = std::thread(ThrMenExecuteFunction, command, container[used.front()].second, Server);
		used.pop();
	}
	else {
		int* state = new int(ACTIVE);
		container.push_back(std::make_pair(std::thread(ThrMenExecuteFunction, command, state, Server), state));
	}
	door.unlock();
}

void ThreadMenedgerClass::SetServer(ServerClass* Server) {
	state = ACTIVE;
	TimeChecker = std::thread(TimeCheckerFunction, this);
}

void ThrMenExecuteFunction(MyCommandClass* command, int* state, ServerClass* Server) {
	CommandState(Server->CoutMutex, command->name, command->socket,"The thread started");
	command->execute(Server);	//выполнить команду
	delete command;				//очищаем память
	*state = NOT_ACTIVE;
}

void TimeCheckerFunction(ThreadMenedgerClass* menedger) {
	Timer timer;
	timer.Start();
	while (menedger->state == ACTIVE) {
		if (timer.Time() > CHECK_TIME) {
			menedger->door.lock();
			for (int i = 0; i < (int)menedger->container.size(); ++i) {
				if (*(menedger->container[i].second) == NOT_ACTIVE) {
					*(menedger->container[i].second) = IN_USED;
					menedger->used.push(i);
					if (menedger->container[i].first.joinable()) {
						menedger->container[i].first.join();
					}
				}
			}
			menedger->door.unlock();
		}
	}
}

ThreadMenedgerClass::ThreadMenedgerClass() {}
ThreadMenedgerClass::ThreadMenedgerClass(ServerClass* S) : Server(S) {
	state = ACTIVE;
	TimeChecker = std::thread(TimeCheckerFunction, this);
}

ThreadMenedgerClass::~ThreadMenedgerClass() {
	state = NOT_ACTIVE;
	if (TimeChecker.joinable()) {
		TimeChecker.join();
	}
	for (int i = 0; i < (int)container.size(); ++i) {
		if (container[i].first.joinable()) {
			container[i].first.join();
		}
		delete container[i].second;
	}
}