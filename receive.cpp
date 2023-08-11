#include <natupnp.h>
#include <objbase.h>
#include <oleauto.h>
#include <winsock.h>
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"oleaut32.lib")
#include <string>
//#include <winsock2.h>
//#include <WS2tcpip.h>
//#include <memory>
//#pragma comment(lib, "ws2_32.lib")
#include "portMapping_s.h"



int main() {
	
	PortMapping* portmap = PortMapping::CreateAndInitialize();

	//ーーーーーーーーーーーーーーーーーーーー
	//受信処理
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	const auto sock = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(49460);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(sock, (sockaddr*)&addr, sizeof(addr));
	size_t buffSize = 65536;
	char* buffer = new char[buffSize];
	recv(sock, buffer, buffSize, 0);
	closesocket(sock);
	delete[] buffer;
	WSACleanup();
	return EXIT_SUCCESS;
	//ーーーーーーーーーーーーーーーーーーーーー

	PortMapping::Finalize(portmap);
}


