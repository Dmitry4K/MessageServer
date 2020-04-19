#pragma once
#include<iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib,"Ws2_32.lib")
#include <winsock2.h>
#include<Windows.h>
#include<string>
#include<utility>
#include<rapidjson/writer.h>
#include<rapidjson/document.h>
#include<rapidjson/filewritestream.h>
#include<rapidjson/filereadstream.h>
#include<rapidjson/stringbuffer.h>
class ClientClass {
private:
	int SocketHandle = 0;
	int ServerSocket = 0;
public:
	const static int CLIENT_CLASS_ERROR_SOCKET = SOCKET_ERROR;
	const static int CLIENT_CLASS_BUFFER_SIZE = 100;
	const static int FILE_TYPE = 0;
	const static int STRING_TYPE = 1;
	const static int FILE_BUFFER_SIZE = 10000;
	ClientClass();
	ClientClass(const char* str);
	~ClientClass();
	int GetSocket();
	int Connect(const char* str);
	int Disconnect();
	int Send(const char* b);
	int Send(const char* b, int bytes);
	char* Recieve();
	char* Recieve(int b);
	std::string RecievePost(std::string id);
	void SendPost(std::string id, std::string msg);
};