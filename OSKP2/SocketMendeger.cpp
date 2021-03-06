#include"SocketMenedger.h"

SocketMenedger::SocketMenedger() : Capacity(0) {}

SocketMenedger::SocketMenedger(int p) : Capacity(p) {}

int SocketMenedger::isExist(int s) {
	for (int i = 0; i < ActiveSockets.size(); ++i) {
		if (ActiveSockets[i] == s) {
			return i;
		}
	}
	return 0;
}
bool SocketMenedger::Add(int s) {
	if (!AlreadyUsed.empty()) {
		ActiveSockets[AlreadyUsed.front()] = s;
		return true;
	}
	else {
		if (ActiveSockets.size() <= Capacity) {
			ActiveSockets.push_back(s);
			return true;
		}
		else {
			return false;
		}
	}
}
bool SocketMenedger::Remove(int s) {
	int res = isExist(s);
	if (res) {
		if (res == ActiveSockets.size() - 1) {
			ActiveSockets.pop_back();
			return true;
		}
		AlreadyUsed.push(res);
		ActiveSockets[res] = 0;
		return true;
	}
	else {
		return false;
	}
}
int SocketMenedger::operator[] (int i) const {
	return ActiveSockets[i];
}

size_t SocketMenedger::Size() {
	return ActiveSockets.size();
}


std::mutex& SocketMenedger::GetMutex(){
	return door;
}