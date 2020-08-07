#include<iostream>
#include<string>
#include"ClientClass.h"

const std::string test1 = "D:\\dev\\OSKP2\\Debug\\test1.txt";
const std::string test2 = "D:\\dev\\OSKP2\\Debug\\test2.txt";
const std::string test3 = "D:\\dev\\OSKP2\\Debug\\test3.txt";
const std::string test4 = "D:\\dev\\OSKP2\\Debug\\test4.7z";
const std::string test5 = "D:\\dev\\OSKP2\\Debug\\test5.docx";
const std::string image = "D:\\dev\\OSKP2\\Debug\\image.jpg";

int main() {
	std::string string;
	ClientClass Client("127.0.0.1:4040");
	std::cout << "Handle : " << Client.GetSocket().GetSocketHandle() << std::endl;
	//Client.SendText("1", "text");
	//Client.Receive("1", string);
	Client.SendFile("1", test3);
	Client.Receive("1", string);
	std::cout << string;
	system("pause");
	return 0;
}
