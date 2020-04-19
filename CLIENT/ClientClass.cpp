#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<sys/stat.h>
#include"ClientClass.h"

ClientClass::ClientClass() {
	WSAData wData;
	if (WSAStartup(MAKEWORD(2, 2), &wData) == 0) {
		std::cout << "WSA Startup succes\n";
	}
	else {
		"WSA Startup Error!\n";
		exit(1);
	}
	SocketHandle = -1;
}

std::pair<std::string, std::string> GetPortAndIp(std::string& a) {
	std::string ip;
	std::string port;
	for (size_t i = 0; i < a.length(); ++i) {
		if (a[i] == ':') {
			ip = a.substr(0, i);
			port = a.substr(i + 1, a.npos);
			return std::make_pair(ip, port);
		}
	}
	throw std::exception("Invalid addres");
}

int ClientClass::Connect(const char* str) {
	std::string AdrString(str);
	SocketHandle = socket(AF_INET, SOCK_STREAM, NULL);
	if (SocketHandle == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}
	SOCKADDR_IN addr;
	auto IpAndPort = GetPortAndIp(AdrString);
	//std::cout << IpAndPort.first << ' ' << IpAndPort.second << std::endl;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(IpAndPort.first.c_str());
	addr.sin_port = htons(std::stoi(IpAndPort.second));
	return connect(SocketHandle, (sockaddr*)&addr, sizeof(addr));;
}

int ClientClass::Send(const char* msg) {
	return send(SocketHandle, msg, strlen(msg)+1, 0);
}

int ClientClass::Send(const char* b, int bytes) {
	return send(SocketHandle, b, bytes, 0);
}
char* ClientClass::Recieve() {
	if (SocketHandle == 0) {
		return nullptr;
	}
	char* msg = new char[CLIENT_CLASS_BUFFER_SIZE];
	recv(SocketHandle, msg, CLIENT_CLASS_BUFFER_SIZE, 0);
	return msg;
}

int ClientClass::GetSocket() {
	return SocketHandle;
}

ClientClass::ClientClass(const char* str) {
	SocketHandle = 0;
	ServerSocket = 0;
	Connect(str);
}
int ClientClass::Disconnect() {
	return shutdown(SocketHandle, SD_BOTH);
}
ClientClass::~ClientClass() {
	Disconnect();
	closesocket(SocketHandle);
	WSACleanup();
}
void ClientClass::SendPost(std::string id, std::string msg) {
	HANDLE F = CreateFile(msg.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_READ_ATTRIBUTES, NULL);
	rapidjson::Document doc;//отправляем файл с файлом
	auto& allocator = doc.GetAllocator();
	rapidjson::Value json_val;
	json_val.SetString("in");
	doc.SetObject().AddMember("mod", json_val, allocator);
	json_val.SetString(id.c_str(), allocator);
	doc.AddMember("destination", json_val, allocator);
	json_val.SetString(msg.c_str(), allocator);
	doc.AddMember("data", json_val, allocator);
	if (F != INVALID_HANDLE_VALUE) {
		json_val.SetInt(FILE_TYPE);
		doc.AddMember("type", json_val, allocator);
		
		long long psize = GetFileSize(F, NULL);
		long long pcount = psize / FILE_BUFFER_SIZE + (psize % FILE_BUFFER_SIZE) ? 1 : 0;
		json_val.SetInt64(pcount);
		doc.AddMember("pcount", json_val, allocator);
		json_val.SetInt64(psize);
		doc.AddMember("size", json_val, allocator);
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		doc.Accept(writer);
		Send(buffer.GetString());
		Sleep(100);
		Recieve(strlen("Start") + 1);
		DWORD readbytes;
		char file_buffer[FILE_BUFFER_SIZE];
		while (pcount > 0) {
			int read_size = (psize > FILE_BUFFER_SIZE) ? (psize-=FILE_BUFFER_SIZE, FILE_BUFFER_SIZE) : psize;
			ReadFile(F, file_buffer, read_size, &readbytes, NULL);
			Send(file_buffer, read_size);
			--pcount;
		}
		CloseHandle(F);
		Sleep(100);
	}
	else {
		json_val.SetInt(STRING_TYPE);
		doc.AddMember("type", json_val, allocator);
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		doc.Accept(writer);
		Send(buffer.GetString());
		Sleep(100);
		Recieve(strlen("Start") + 1);
	}
	
}
char* ClientClass::Recieve(int b) {
	if (SocketHandle == 0) {
		return nullptr;
	}
	char* msg = new char[b];
	recv(SocketHandle, msg, b, 0);
	return msg;
}

std::string ClientClass::RecievePost(std::string id) {
	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::Value json_val;
	json_val.SetString("get");
	doc.AddMember("mod", json_val, doc.GetAllocator());
	json_val.SetString(id.c_str(), doc.GetAllocator());
	doc.AddMember("destination", json_val, doc.GetAllocator());
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	Send(buffer.GetString());
	Sleep(100);
	Recieve(strlen("Start")+1);
	std::string ans(Recieve());
	rapidjson::StringStream s(ans.c_str());
	doc.ParseStream(s);
	int type = doc["type"].GetInt();
	if (type == STRING_TYPE) {
		std::string msg = doc["data"].GetString();
		Sleep(100);
		return msg;
	}
	else if (type == FILE_TYPE) {
		std::string msg = doc["data"].GetString();
		HANDLE f = CreateFile(msg.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		int pack_count = doc["pcount"].GetInt();
		unsigned long psize = doc["size"].GetInt64();
		DWORD writeBytes;
		while (pack_count > 0) {
			int read_size = (psize > FILE_BUFFER_SIZE) ? (psize -= FILE_BUFFER_SIZE, FILE_BUFFER_SIZE) : psize;
			char* pfile = Recieve(read_size);
			WriteFile(f, pfile, read_size, &writeBytes, NULL);
			delete[] pfile;
			--pack_count;
		}
		CloseHandle(f);
		Sleep(100);
		return msg;
		//получаем файл
	}
}