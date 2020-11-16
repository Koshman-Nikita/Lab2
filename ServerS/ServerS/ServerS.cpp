#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <deque>
#include <string>

using namespace std;

#pragma warning(disable: 4996)

SOCKET Connections[2];
int Counter = 0;

std::deque<std::string> Split(const std::string& str, int splitLength)
{
	int NumSubstrings = str.length() / splitLength;
	std::deque<std::string> ret;

	for (auto i = 0; i < NumSubstrings; i++)
	{
		ret.push_back(str.substr(i * splitLength, splitLength));
	}

	// If there are leftover characters, create a shorter item at the end.
	if (str.length() % splitLength != 0)
	{
		ret.push_back(str.substr(splitLength * NumSubstrings));
	}


	return ret;
}


void ClientHandler(int index) {
	char msg[256];
	while (true) {
		recv(Connections[index], msg, sizeof(msg), NULL);
		for (int i = 0; i < Counter; i++) {
			if (i != index) {
				continue;
			}

			auto splitted_parts = Split(msg, 5);

			while (!splitted_parts.empty()) {
				auto message = splitted_parts.front();
				splitted_parts.pop_front();
				send(Connections[i], message.data(), message.length(), NULL);
			}

		}
	}
}

int main(int argc, char* argv[]) {
	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	for (int i = 0; i < 2; i++) {
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "Client Connected!\n";
			char msg[256] = "Welcome to chat!";
			send(newConnection, msg, sizeof(msg), NULL);

			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
		}
	}


	system("pause");
	return 0;
}}