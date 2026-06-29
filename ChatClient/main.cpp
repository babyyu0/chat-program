#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
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

	system("pause");

	// 4. 정리
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}