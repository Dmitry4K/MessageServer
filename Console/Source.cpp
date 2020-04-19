#include<iostream>
#include<Windows.h>
#include<string>

int main() {
	std::string str;
	HANDLE outH = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD writeBytes;
	while (std::getline(std::cin, str)) {
		if (str == "exit") {
			int num = 0;
			WriteFile(outH, &num, sizeof(int), &writeBytes, NULL);
			break;
		}
	}
	return 0;
}