#include<iostream>
#include<string>
#include"ClientClass.h"

 int main() {
	setlocale(LC_ALL, "rus");
	ClientClass Client;
	Client.Connect("127.0.0.1:4040");
	Client.SendPost("12345", "Привет");
	std::cout << Client.RecievePost("12345") << std::endl;
	Client.SendPost("12345", "test4.txt");
	std::cout << Client.RecievePost("12345") << std::endl;
	system("pause");
	return 0;
}
