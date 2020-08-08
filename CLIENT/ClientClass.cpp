#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<sys/stat.h>
#include"ClientClass.h"
#include"../MyFunctions/MyFunctions.h"
#include"Commands/text_c.h"
#include"Commands/endfile_c.h"
#include"Commands/pack_c.h"
#include<fstream>

ClientClass::ClientClass(const std::string& adr) : Parser(this->CommandMap){
	CommandMap["text"] = new ClientTextCommand();
	CommandMap["pack"] = new ClientPackCommand();
	CommandMap["endfile"] = new ClientEndFileCommand();
	if (HostSocket.Connect(adr) != 0) {
		HostSocket.Close();
		//exit(1);
	}
	Start();
}
ClientClass::~ClientClass() {
	shutdown(HostSocket.GetSocketHandle(), SD_BOTH);
	ReceiveThreadState = OFF;
	if (ReceiveThread.joinable()) {
		ReceiveThread.join();
	}
	HostSocket.Close();
}

void ReceiveFunction(ClientClass* Server) {
	char*  msg = nullptr;
	while (Server->ReceiveThreadState == ON) {
		Sleep(DEFAULT_SLEEP_TIME);
		int res;
		if ((res = Server->HostSocket.Recieve(msg)) > 0) {	//потеря данных!!!
			Server->Parser.Execute(msg,res, Server->Commands);//парсинг строки в очередь
			//std::cout << msg;
			delete[] msg;
		}
	}
	//std::cout << 0;
}

void ClientClass::Start() {
	ReceiveThreadState = ON;
	ReceiveThread = std::thread(ReceiveFunction, this);
}

const MySocketClass& ClientClass::GetSocket() const {
	return HostSocket;
}


int ClientClass::Connect(const std::string& adr) {
	Disconnect();
	return HostSocket.Connect(adr);
}
int ClientClass::Disconnect() {
	if (HostSocket.State() == CONNECTED) {
		return HostSocket.Close();
	}
	else if (HostSocket.State() == BINDED) {
		exit(1);
	}
	return 0;
}

//sendtext q len text
int ClientClass::SendText(const std::string& qid, const std::string& text) {
	return this->Send(GenTextCommandForServer(qid, text));
}
//receive q

void ClientClass::GetPacks() {
	int counts = DEFAULT_COUNT;
	while (counts > 0) {
		if (!Commands.empty()) {
			auto command = Commands.front();
			Commands.pop();
			if (strcmp(command->params[0], "endfile") == 0) {
				break;
			}
			command->execute(this);
			//std::cout << "pack\n";
			counts = DEFAULT_COUNT;
		}
		Sleep(DEFAULT_SLEEP_TIME);
		--counts;
	}
}

int ClientClass::Receive(const std::string& qid, std::string& dest) {
	int res = Send(GenReceiveCommandForServer(qid));
	if (res <= 0) {
		return res;
	}
	int count = DEFAULT_COUNT;
	MyCommandClass* command = nullptr;
	while (count > 0) {
		if (!Commands.empty()) {
			command = Commands.front();
			Commands.pop();
			dest = std::string(command->params[2]);
			if (command->params[0] == std::to_string(STRING_TYPE)) {
				//получили строку
			}
			else {
				//получаем файл
				std::remove(std::string(Folder + dest).c_str());
				GetPacks();
			}
			break;
		}
		Sleep(DEFAULT_SLEEP_TIME);
		--count;
	}
	return dest.length();
}
int ClientClass::Send(const std::string& text) {
	if (text.empty()) {
		return 0;
	}
	int res = -1;
	for (int i = 0; i < DEFAULT_COUNT; ++i) {
		if ((res = HostSocket.Send(text)) > 0) {
			break;
		}
		Sleep(DEFAULT_SLEEP_TIME);
	}
	return res;
}


int ClientClass::SendFile(const std::string& qid, const std::string& file) {
	long long file_size;
	if ((file_size = FileSize(file)) < 1) {
		return 0;
	}
	int res = -1;
	this->Send(GenFileCommandForServer(qid, GetLastPartOfFileName(file)));
	long long packs_count = file_size / PACK_SIZE;
	if (file_size % PACK_SIZE) {
		++packs_count;
	}
	std::ifstream f(file, std::ios::binary);
	std::string pack;
	char s;
	for (long long i = 0; i < packs_count; ++i) {
		for (int j = 0; j < PACK_SIZE; ++j) {
			s = (char)f.get();
			if (!f.eof()) {
				pack += s;
			}
			else {
				break;
			}
		}
		this->Send(GenPackCommandForServer(GetLastPartOfFileName(file), pack));
		pack.clear();
		Sleep(DEFAULT_SLEEP_TIME);
	}
	return 1;
}


const std::string& ClientClass::GetFolder() const {
	return Folder;
}


void ClientClass::SetFolder(const std::string& f) {
	Folder = f;
}

int ClientClass::State() {
	return HostSocket.State();
}