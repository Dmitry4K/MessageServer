#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>

#pragma comment (lib,"Ws2_32.lib")
#include <winsock2.h>
#include<Windows.h>
#include<mutex>
													//��������� �������
const int EMPTY = 0;
const int CONNECTED = 1;
const int BINDED = 2;
													//���������� ��� �������� ���������� ���������� winsock
static bool IS_WSA_STARTED = false;
static WSADATA W_DATA;

static int SOCKET_COUNT = 0;						//���-�� ��������� � �������� ����������� MySocketClass
class MySocketClass {		
private:
	std::string adr;								//������������ ������
	int handle = 0;									//handle, ������� �������� ��� ������ socket()
	int state = EMPTY;								//��������� ������
	//std::mutex door;								//����� �������� �������, ����� ������� ����� ������-����������
public:
	MySocketClass();
	~MySocketClass();
	const std::string& GetAdr() const;
	int GetSocketHandle() const;
	int Connect(const std::string& way);
	int Bind(const std::string& way);
	int Close();
													//������ �������� ���������
	int Send(int h, const char*, int len);
	int Send(const char*, int len);
	int Send(const char*);							//������ ��� ������������ �� ����� !
	int Send(const std::string& m);
	int Send(int h, const std::string& m);
													//������ ������ ���������
	int Recieve(int h, char*&);
	int Recieve(char*&);
	int Recieve(std::string& m);					//������ ��� ������������ �����
	int Recieve(int h, std::string& m);				//������ ��� ������������ �����

	int State() const ;
};