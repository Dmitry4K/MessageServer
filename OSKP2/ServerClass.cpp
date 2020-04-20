#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include "ServerClass.h"
bool isNumber(char* str) {
	for (size_t i = 0; i < strlen(str); ++i) {
		if (str[i] < '0' || str[i] > '9') {
			return false;
		}
	}
	return true;
}
ServerClass::ServerClass(int p) : Port(p), Executors(EXECUTOR_COUNT), ActiveSockets(ACTIVE_SOCKETS_COUNT){
	Start();
}

ServerClass::ServerClass(int argc, char* argv[]): Port(0), Executors(EXECUTOR_COUNT), ActiveSockets(ACTIVE_SOCKETS_COUNT) {
	if (argc == 1) {
		std::cout << "Wrong number of arguments!\nUse " << argv[0] << " [Port]\n";
		exit(1);
	}
	if (isNumber(argv[1])) {
		Port = std::atoi(argv[1]);
	}
	else {
		std::cout << "Wrong argument : Not number!\n";
		exit(1);
	}
	Start();
}
int ServerClass::GetPort() {
	return Port;
}


void ServerClass::Start() {
	Data = DataBaseClass(WAY_TO_DATA.c_str());
	ConsoleStart();
	WSAData wData;
	if (WSAStartup(MAKEWORD(2, 2), &wData) == 0) {
		std::cout << "WSA Startup succes\n";
	}
	else {
		"WSA Startup Error!\n";
		exit(1);
	}
	SocketHandle = socket(AF_INET, SOCK_STREAM, NULL);
	if (SocketHandle == SOCKET_ERROR) {
		std::cout << "Socket not created!\n";
		exit(1);
	}
	hostent* localHost = gethostbyname("");
	char* localIP = inet_ntoa(*(struct in_addr*) * localHost->h_addr_list);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(Port);
	if (bind(SocketHandle, (SOCKADDR*)&addr, sizeof(addr)) != SOCKET_ERROR) {
		std::cout << "Socket succed binded\n";
	}
	else {
		std::cout << "Socket bind error\n";
	}
	u_long arg = 1;
	ioctlsocket(SocketHandle, FIONBIO, &arg);
	if (listen(SocketHandle, SOMAXCONN) != SOCKET_ERROR) {
		std::cout << "Start listening at port : " << ntohs(addr.sin_port) << '\n';
	}
	for (size_t i = 0; i < Executors.size(); ++i) {
		Executors[i].first = 0;
	}
	ExecuteStarter = std::thread(ExecuteStarterFunction, this);
	Handler = std::thread(HandlerFunction, this);
	Accept();
}

void ServerClass::Close() {
	closesocket(SocketHandle);
	WSACleanup();
	std::cout << "Socket closed\n";
}

void ServerClass::Accept() {
	while (!isExit)
	{
		SOCKET acceptS;
		SOCKADDR_IN addr_c;
		int addrlen = sizeof(addr_c);
		acceptS = accept(SocketHandle, (struct sockaddr*) & addr_c, &addrlen);
		if (acceptS != INVALID_SOCKET) {
			std::cout << "Client connected from "
				<< (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b1 + '0' - 48 << '.'
				<< (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b2 + '0' - 48 << '.'
				<< (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b3 + '0' - 48 << '.'
				<< (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b4 + '0' - 48 << ':'
				<< ntohs(addr_c.sin_port) << std::endl;
			ActiveSockets.Add(acceptS);
		}
		Sleep(50);
	}
}

int ServerClass::Recieve(int recvSocket, char** msg, int b) {
	*msg = new char[b];
	int res = recv(recvSocket, *msg, b, 0);
	if (res == SOCKET_ERROR) {
		delete[] * msg;
		*msg = nullptr;
	}
	return res;
}
int ServerClass::Send(int Socket, const char* msg) {
	return send(Socket, msg, strlen(msg)+1, 0);
}


int ServerClass::Send(int Socket, const char* b, int bytes) {
	return send(Socket, b, bytes, 0);
}

int ServerClass::Recieve(int recvSocket, char** msg) {
	*msg = new char[CLIENT_CLASS_BUFFER_SIZE]; 
	int res = recv(recvSocket, *msg, CLIENT_CLASS_BUFFER_SIZE, 0);
	if (res == SOCKET_ERROR) {
		delete[] *msg;
		*msg = nullptr;
	}
	return res;
}
int ServerClass::Recieve(char** msg) {
	*msg = new char[CLIENT_CLASS_BUFFER_SIZE];
	int res = recv(SocketHandle, *msg, CLIENT_CLASS_BUFFER_SIZE, 0);
	if (res == SOCKET_ERROR) {
		delete[] *msg;
		*msg = nullptr;
	}
	return res;
}

void HandlerFunction(ServerClass* Server) {
	char* msg = nullptr;
	while (!Server->isExit) {
		for (size_t i = 0; i < Server->ActiveSockets.Size(); ++i) {
			int res;
			if ((res = Server->Recieve(Server->ActiveSockets[i], &msg)) != SOCKET_ERROR && res != 0) {
				Server->HandleCommand.push({ Server->ActiveSockets[i], msg });
			}
			Sleep(100);
		}
	}
}
void ExecuteStarterFunction(ServerClass* Server) {
	while (!Server->IsExit()) {
		if (!Server->HandleCommand.empty()) {
			auto Command = Server->HandleCommand.front();
			Server->HandleCommand.pop();
			for (size_t i = 0; i < Server->Executors.size(); ++i) {
				if (Server->Executors[i].first == 0) {
					Server->Executors[i].first = 1;
					if (Server->Executors[i].second.joinable()) {
						Server->Executors[i].second.join();
					}
					Server->ActiveSockets.Remove(Command.first);
					Server->Executors[i].second = std::thread(ExecuteFunction, Server, Command.first, Command.second, std::ref(Server->Executors[i].first));
					break;
				}
			}
		}
	}
}

ServerClass::~ServerClass() {
	while (!HandleCommand.empty()) {
		delete[] HandleCommand.front().second;
	}
	if (Handler.joinable()) {
		Handler.join();
	}
	if (ExecuteStarter.joinable()) {
		ExecuteStarter.join();
	}
	if (Console.joinable()) {
		Console.join();
	}
	for (size_t i = 0; i < Executors.size(); ++i) {
		if (Executors[i].second.joinable()) {
			Executors[i].second.join();
		}
	}
}

bool ServerClass::IsExit() {
	return isExit;
}


void ExecuteFunction(ServerClass* Server, int sock, char* msg, int& res) {
	if (Server->Send(sock, "Start") != SOCKET_ERROR) {
		rapidjson::StringStream s(msg);
		rapidjson::Document doc;
		doc.ParseStream(s);
		std::string mod = doc["mod"].GetString();
		if (mod == "in") {
			int type = doc["type"].GetInt();
			if (type == MessageClass::STRING) {
				std::string id = doc["destination"].GetString();
				std::string data = doc["data"].GetString();
				int type = doc["type"].GetInt();
				Server->Data.AddMessageInQueue(id, MessageClass{ data , type });
			}
			else if (type == MessageClass::FILE) {
				std::string msg = doc["data"].GetString();
				HANDLE f = CreateFile(msg.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
				int pack_count = doc["pcount"].GetInt();
				long long fsize = doc["size"].GetInt64();
				DWORD writeBytes;
				while (pack_count > 0) {
					char* pfile = nullptr;		
					long long read_size = (fsize > ServerClass::FILE_BUFFER_SIZE) ? (fsize -= ServerClass::FILE_BUFFER_SIZE, ServerClass::FILE_BUFFER_SIZE) : fsize;
					while (Server->Recieve(sock, &pfile, read_size) == SOCKET_ERROR) {}
					WriteFile(f, pfile, read_size, &writeBytes, NULL);
					delete[] pfile;
					--pack_count;
				}
				std::string id = doc["destination"].GetString();
				Server->Data.AddMessageInQueue(id, MessageClass{ msg , type });
				CloseHandle(f);
			}
		}
		else if (mod == "get") {
			std::string from = doc["destination"].GetString(); 
			if (Server->Data.isExistQueue(from)) {
				MessageClass msg = Server->Data.GetFrontMessageInQueue(from);
				Server->Data.PopMessageInQueue(from);
				rapidjson::Document doc;
				rapidjson::Value json_val;
				json_val.SetString(msg.Data.c_str(), doc.GetAllocator());
				doc.SetObject().AddMember("data", json_val, doc.GetAllocator());
				if (msg.Type == MessageClass::STRING) {
					json_val.SetInt(msg.Type);
					doc.AddMember("type", json_val, doc.GetAllocator());
					rapidjson::StringBuffer buffer;
					rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
					doc.Accept(writer);
					std::string res = buffer.GetString();
					while (Server->Send(sock, res.c_str()) == SOCKET_ERROR) {}
				}
				else if (msg.Type == MessageClass::FILE) {
					//отправляем файл
					HANDLE f = CreateFile(msg.Data.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					json_val.SetInt(MessageClass::FILE);
					doc.AddMember("type", json_val, doc.GetAllocator());	
					DWORD fsize = GetFileSize(f, NULL);
					long long pcount = fsize / ServerClass::FILE_BUFFER_SIZE + (fsize % ServerClass::FILE_BUFFER_SIZE) ? 1 : 0;
					json_val.SetInt64(pcount);
					doc.AddMember("pcount", json_val, doc.GetAllocator());
					json_val.SetInt64(fsize);
					doc.AddMember("size", json_val, doc.GetAllocator());
					rapidjson::StringBuffer buffer;
					rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
					doc.Accept(writer);
					std::string r = buffer.GetString();
					while (Server->Send(sock, r.c_str()) == SOCKET_ERROR) {}
					DWORD readbytes;
					char file_buffer[ServerClass::FILE_BUFFER_SIZE];

					while (pcount > 0) {
						int read_size = (fsize > ServerClass::FILE_BUFFER_SIZE) ? (fsize -= ServerClass::FILE_BUFFER_SIZE, ServerClass::FILE_BUFFER_SIZE) : fsize;
						ReadFile(f, file_buffer, read_size, &readbytes, NULL);
						while (Server->Send(sock, file_buffer, read_size) != SOCKET_ERROR) {}
						--pcount;
					}
					CloseHandle(f);
				}
			}
			else {
				//такой очереди нет
				rapidjson::Document doc;
				rapidjson::Value json_val;
				json_val.SetString("", doc.GetAllocator());
				doc.SetObject().AddMember("data", json_val, doc.GetAllocator());
				json_val.SetInt(MessageClass::STRING);
				doc.AddMember("type", json_val, doc.GetAllocator());
				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				doc.Accept(writer);
				std::string res = buffer.GetString();
				while (Server->Send(sock, res.c_str()) == SOCKET_ERROR) {}
			}
		}
		Server->ActiveSockets.Add(sock);
		res = 0;
	}
	else {
		res = 2;
	}
}

void ServerClass::ConsoleStart() {
	//*
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	std::string ProcessName = "Console"; 
	char* procName = (char*)ProcessName.c_str();
	HANDLE pipe2Read, pipe2Write;//идентификатор объект
	if (CreatePipe(&pipe2Read, &pipe2Write, &sa, 0)) {
		std::cout << "Pipe to Conosole created\n";
	}
	else {
		std::cout << "Pipe to Console doesn't created!\n";
	}
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;
	StartupInfo.hStdOutput = pipe2Write;
	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));
	int process = CreateProcess(NULL,
		procName,
		NULL, NULL, true, //CREATE_NO_WINDOW| CREATE_SUSPENDED
		CREATE_NEW_CONSOLE,// CREATE_NEW_CONSOLE
		NULL, NULL,
		&StartupInfo,
		&ProcessInfo);
	//CloseHandle(pipe2Write);
	Console = std::thread(ConsoleFunction, this, pipe2Read);
	//*/
}


void ConsoleFunction(ServerClass* Server, HANDLE pipe) {
	DWORD readBytes = 0;
	int Command = -1;
	while (Command != 0) {
		ReadFile(pipe, &Command, sizeof(int), &readBytes, NULL);
	}
	std::cout << "Recieve the command 'Exit' from Console\n";
	Server->isExit = true;
}

long long FileSize(const char* f) {
	struct stat fi;
	stat(f, &fi);
	return fi.st_size;
}