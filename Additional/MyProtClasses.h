#pragma once

#include<queue>
#include<mutex>

template<class T>
class MyProtQueue {
private:
	std::queue<T> c;
	std::mutex d;
public:
	T front() {
		return c.front();
	}
	void push(const T e) {
		c.push(e);
	}
	void pop() {
		c.pop();
	}
	bool empty() {
		return c.empty();
	}
	std::mutex& GetMutex() {
		return d;
	}
};