#pragma once
#include<vector>
#include<queue>
#include<mutex>
class SocketMenedger {
	std::vector<int> ActiveSockets;
	std::queue<int> AlreadyUsed;
	std::mutex door;
	int Capacity;
public:
	SocketMenedger(int c);
	SocketMenedger();
	int isExist(int);
	bool Add(int);
	bool Remove(int s);
	size_t Size();
	int operator[](int i) const;
	std::mutex& GetMutex();
};