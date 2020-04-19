#pragma once
#include<vector>
#include<queue>
class SocketMenedger {
	std::vector<int> ActiveSockets;
	std::queue<int> AlreadyUsed;
	int Capacity;
public:
	SocketMenedger(int c);
	int Exist(int);
	bool Add(int);
	bool Remove(int s);
	int Size();
	int operator[](int i) const;
};