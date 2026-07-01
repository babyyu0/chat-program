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
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		std::cout << "socket failed: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	// 2. 주소 설정 (내 번호 정하기)
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(9000);

	// 3. 바인드 (번호 부여)
	if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		std::cout << "bind failed: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// 4. 리슨 (받을 준비)
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "listen failed: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "Server is listening on port 9000..." << std::endl;

	// 5. 접속 수락 (전화 받기 — 올 때까지 여기서 대기)
	SOCKADDR_IN clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "accept failed: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "Client connected!" << std::endl;

	char buffer[512];
	while (true) {
		// 1. 클라이언트가 보낸 메시지 받기 (듣기)
		int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived <= 0) {
			std::cout << "Client disconnected." << std::endl;
			break;
		}
		buffer[bytesReceived] = '\0';

		std::string recvMsg(buffer);
		std::cout << "Received: " << recvMsg << std::endl;

		// 2. 받은 걸 그대로 돌려보내기 (말하기)
		send(clientSocket, buffer, static_cast<int>(recvMsg.size()), 0);
	}

	// 6. 정리
	closesocket(clientSocket);
	closesocket(listenSocket);
	WSACleanup();
	return 0;
}