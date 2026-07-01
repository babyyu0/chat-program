#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

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

	char recvBuffer[512];
	while (true) {
		// 1. 키보드로 메시지 입력 받기
		std::string sendMsg;
		std::cout << "메시지 입력 (quit 입력 시 종료): ";
		std::getline(std::cin, sendMsg);

		//'quit'을 입력하면 반복 종료
		if (sendMsg == "quit") {
			break;
		}

		// 2. 서버로 보내기 (말하기)
		send(clientSocket, sendMsg.c_str(), static_cast<int>(sendMsg.size()), 0);

		// 3. 서버가 돌려준 메아리 받기 (듣기)
		int bytesReceived = recv(clientSocket, recvBuffer, sizeof(recvBuffer) - 1, 0);
		if (bytesReceived <= 0) {
			std::cout << "Server disconnected." << std::endl;
			break;
		}
		recvBuffer[bytesReceived] = '\0';

		std::string recvMsg(recvBuffer);
		std::cout << "서버 응답: " << recvMsg << std::endl;
	}

	// 4. 정리
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}