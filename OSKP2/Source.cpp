#include<iostream>
#include"ServerClass.h"


int main(int argc, char* argv[]) {
	//ServerClass Server(argc, argv);
	ServerClass Server("127.0.0.1:4040");
	return 0;
}