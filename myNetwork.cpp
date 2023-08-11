#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include "nlohmann/json.hpp"
#include "portMapping_s.h"
#include "myNetwork.h"

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

void MyNetWork::Initialize()
{
	SOCKET sock1, sock2;						//ソケットを2つ作成
	struct sockaddr_in addr1, addr2;			//ipアドレスやポート番号を取得するための番号
	fd_set fds, readfds;						//ソケットの状態を読み取るための構造体
	char buf[2048];								//ソケットから受け取ったデータを格納する変数
	int n;										//nとは？？？？
	struct timeval tv;							//情報の受け取りがタイムアウトするまでの時間（秒とマイクロ秒の組み合わせ）
	WSADATA wsaData;							//WSAの設定に必要な構造体

	PortMapping* portmap = PortMapping::CreateAndInitialize();
	if (portmap == nullptr)
	{
		return -1;
	}

	int err = WSAStartup(MAKEWORD(2, 0), &wsaData);		//winsockを利用する際に必ず必要な初期化関数
	if (err != 0)
	{
		CheckWSAStartError(err);
		PortMapping::Finalize(portmap);
		return -2;
	}

	// 受信ソケットを2つ作ります
	sock1 = socket(AF_INET, SOCK_DGRAM, 0);		//AF_INET...IPv4,　SOCK_DGRAM...ソケットの種類　, 0...プロトコルの指定
	sock2 = socket(AF_INET, SOCK_DGRAM, 0);		//同じ

	addr1.sin_family = AF_INET;					//addr側の接続方法の設定　
	addr2.sin_family = AF_INET;					//同じ

	//InetPtonW(addr1.sin_family, TEXT("127.0.0.1"), &addr1.sin_addr.S_un.S_addr);		//ipアドレスの設定
	//InetPtonW(addr2.sin_family, TEXT("127.0.0.1"), &addr2.sin_addr.S_un.S_addr);		//同様

	// 2つの別々のポートで待つために別のポート番号をそれぞれ設定します
	addr1.sin_port = htons(54321);
	addr2.sin_port = htons(22222);

	addr1.sin_addr.S_un.S_addr = INADDR_ANY;
	addr2.sin_addr.S_un.S_addr = INADDR_ANY;

	// 2つの別々のポートで待つようにbindします
	bind(sock1, (sockaddr*)&addr1, sizeof(addr1));
	bind(sock2, (sockaddr*)&addr2, sizeof(addr2));

	// fd_setの初期化します
	FD_ZERO(&readfds);

	// selectで待つ読み込みソケットとしてsock1を登録します
	FD_SET(sock1, &readfds);
	// selectで待つ読み込みソケットとしてsock2を登録します
	FD_SET(sock2, &readfds);

	// 10秒でタイムアウトするようにします
	tv.tv_sec = 60;
	tv.tv_usec = 0;

	// このサンプルでは、10秒間データを受信しないとループを抜けます
	while (1) {
		// 読み込み用fd_setの初期化
		// selectが毎回内容を上書きしてしまうので、毎回初期化します
		memcpy(&fds, &readfds, sizeof(fd_set));

		// fdsに設定されたソケットが読み込み可能になるまで待ちます
		n = select(0, &fds, NULL, NULL, &tv);

		// タイムアウトの場合にselectは0を返します
		if (n == 0) {
			// ループから抜けます
			printf("timeout\n");
			break;
		}

		// sock1に読み込み可能データがある場合
		if (FD_ISSET(sock1, &fds)) {
			// sock1からデータを受信して表示します
			memset(buf, 0, sizeof(buf));
			recv(sock1, buf, sizeof(buf), 0);
			printf("%s\n", buf);
		}

		// sock2に読み込み可能データがある場合
		if (FD_ISSET(sock2, &fds)) {
			// sock2からデータを受信して表示します
			memset(buf, 0, sizeof(buf));
			recv(sock2, buf, sizeof(buf), 0);
			printf("%s\n", buf);
		}
	}

	closesocket(sock1);
	closesocket(sock2);

	WSACleanup();

	PortMapping::Finalize(portmap);

	return 0;
}