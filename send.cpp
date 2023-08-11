#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string_view>
#pragma comment(lib, "ws2_32.lib")


int main() {
	WSADATA wsaData;
	WSACleanup();
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	const auto sock = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(54321);
	InetPtonW(addr.sin_family, TEXT("222.227.190.48"), &addr.sin_addr.S_un.S_addr);
	std::string sendtext;
	std::cout << "‘—M‚·‚é•¶Žš—ñ‚ð“ü—Í‚µ‚Ä‚­‚¾‚³‚¢F";
	std::cin >> sendtext;
	sendto(sock, sendtext.data(), sendtext.size() +1, 0, (SOCKADDR*)&addr, sizeof(addr));
	std::cout << sendtext.data();
	closesocket(sock);
	WSACleanup();
	return EXIT_SUCCESS;
}