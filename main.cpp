#include <iostream>
#include <string>
#include "mySocket.h"
#include "MyPortMapping.h"

using namespace std;

//WSAStartUpのエラーを確認
void CheckWSAStartError(int err)
{
	switch (err) {
	case WSASYSNOTREADY:
		printf("WSASYSNOTREADY\n");
		break;
	case WSAVERNOTSUPPORTED:
		printf("WSAVERNOTSUPPORTED\n");
		break;
	case WSAEINPROGRESS:
		printf("WSAEINPROGRESS\n");
		break;
	case WSAEPROCLIM:
		printf("WSAEPROCLIM\n");
		break;
	case WSAEFAULT:
		printf("WSAEFAULT\n");
		break;
	}
}

int main()
{

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	MySocket sock;
#if false
	sock.SetIpAdress(L"222.227.190.48");
	sock.SetPortNum(55555);

	string text;
	cout << "送信する文字列を入力してください：";
	cin >> text;

	sock.Send(text.c_str(),text.size());
#else
	int result;
	auto portmap = MyPortMapping::CreateAndInitialize(55555, L"192.168.0.11");
	result = sock.SetPortNum(55555);
	result = sock.SetReceive(48);
	timeval timeout;
	timeout.tv_sec = 20;
	timeout.tv_usec = 0;
	result = MySocket::Receive(timeout);
	const char* data = sock.ReadReceive();
	cout << data;
#endif

	WSACleanup();
}