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
	auto portmap = MyPortMapping::CreateAndInitialize();
	portmap->Add(55555);
	wcout << portmap->getLocal();
	//sock.SetPortNum(55555);
	//sock.SetReceive(48);
	//result = MySocket::Receive(20,0);
	//const char* data = sock.ReadReceive();
	//cout << data << "\nip:" << sock.GetSenderIP() << "\nport:" << ntohs(sock.GetSenderPORT());
#endif
	WSACleanup();
}