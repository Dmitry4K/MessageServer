#pragma once
#include"../ServerClass.h"
#include"../DataBaseClass.h"
#include"../../MyFunctions/MyFunctions.h"
#include"pack_c.h"
#include<fstream>

const int PACK_SIZE = 10000;

void SendText(MessageClass& Msg, int socket, ServerClass* node) {
	node->GetSocket().Send(socket, GenTextCommandForClient(Msg.GetType(), Msg.GetData()));
}

void SendFile(MessageClass& Msg, int socket, ServerClass* node) {
	long long file_size;
	if ((file_size = FileSize(FOLDER + Msg.GetData())) < 1) {
		return;
	}
	int res = -1;
	long long packs_count = file_size / PACK_SIZE;
	if (file_size % PACK_SIZE) {
		++packs_count;
	}
	//std::cout << "Packs : " << packs_count << std::endl;
	CommandState(node->CoutMutex, "File Sending", "Packs : " + std::to_string(packs_count));
	std::ifstream f(FOLDER + Msg.GetData(), std::ios::binary);
	std::string pack;
	char s;
	int pack_s = 0;
	for (long long i = 0; i < packs_count; ++i) {
		for (pack_s = 0; pack_s < PACK_SIZE; ++pack_s) {
			s = (char)f.get();
			if (!f.eof()) {
				pack += s;
			}
			else {
				break;
			}
		}
		int res = node->GetSocket().Send(socket, GenPackCommandForClient(GetLastPartOfFileName(Msg.GetData()), pack));
		if (res > 0) {
			//std::cout << "pack\n";
			CommandState(node->CoutMutex, "File Sending", "Pack [" + std::to_string(i+1) + "/" + std::to_string(packs_count) + "] sended. Size : " + std::to_string(pack_s) + " bytes");
		}
		pack.clear();
		Sleep(DEFAULT_SLEEP_TIME);
	}
	node->GetSocket().Send(socket, GenEndFileCommandForClinet());
}

struct ReceiveCommand : MyCommandClass {
	ReceiveCommand() {
		count = 1;
		name = "Request received";
	}
	void execute(ServerClass* node) const override {
		node->DataMutex.lock();
		auto Msg = node->GetData().GetQueueByName(params[0]).front();
		SendText(node->GetData().GetQueueByName(params[0]).front(), socket, node);
		if (Msg.GetType() == STRING_TYPE) {
		}
		else {
			SendFile(Msg, socket, node);
		}
		//Нужно определить, есть ли сообщение, файл или текст
		node->DataMutex.unlock();
	}
	void argument_parsing(MyCharStreamClass& stream) override {
		char * word;
		stream.GetWord(word);
		params.push_back(word);
		word = nullptr;
	}
	void copy(MyCommandClass*& com) const override {
		com = new ReceiveCommand();
	}
};
