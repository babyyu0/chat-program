#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

// 수신 전담 함수 : 이 함수가 별도 스레드에서 돌아감
void ReceiveThread(SOCKET sock) {
	char recvBuffer[512];
	while (true) {
		int bytesReceived = recv(sock, recvBuffer, sizeof(recvBuffer) - 1, 0);
		if (bytesReceived <= 0) {
			std::cout << "\n서버와 연결이 끊어졌습니다." << std::endl;
			break;
		}
		recvBuffer[bytesReceived] = '\0';

		std::string recvMsg(recvBuffer);
		std::cout << "\n[서버] " << recvMsg << std::endl;
	}
}

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "WSAStartup failed" << std::endl;
		return 1;
	}

	// 1. 소켓 생성 (전화기 들이기)
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "socket failed: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	// 2. 서버 주소 적기 (어디로 걸지)
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 서버 IP
	serverAddr.sin_port = htons(9000);                    // 서버 포트

	// 3. 전화 걸기
	if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		std::cout << "connect failed: " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "Client: connected to server!" << std::endl;

	// 수신 전담 스레드 시 작 (여기서 두 갈래로 나뉨)
	std::thread recvThread(ReceiveThread, clientSocket);

	// 메인 스레드는 입력 및 전송만 담당
	while (true) {
		std::string sendMsg;
		std::getline(std::cin, sendMsg);
		if (sendMsg == "quit") {
			break;
		}

		send(clientSocket, sendMsg.c_str(), static_cast<int>(sendMsg.size()), 0);
	}

	// 4. 정리
	closesocket(clientSocket);

	// 수신 스레드가 끝날 때까지 기다렸다 정리
	recvThread.join();

	WSACleanup();
	return 0;
}