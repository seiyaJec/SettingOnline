#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>
#include <vector>

class MySocket
{
private:
	SOCKET sock;			//ソケット番号
	sockaddr_in addr;		//ソケットとリンクするipやポート番号

	bool finishBind;		//バインドしたか

public:
	//コンストラクタ
	//param1: ソケットが格納できるサイズ
	MySocket()
		:finishBind(false)
	{
		sock = socket(AF_INET, SOCK_DGRAM, 0);		//AF_INET...IPv4,　SOCK_DGRAM...ソケットの種類　, 0...プロトコルの指定
		addr.sin_family = AF_INET;
		addr.sin_port = htons(49152);
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
	}

	enum ResultType
	{ 
		SUCCESS = 0,	//成功
		FUNCERR,		//外部関数のエラー
		INVALIDNUM,		//無効な数字
		ALREADYBIND,	//バインド済みのため実行不可
	};

	//ポート番号の設定
	//param1: ポート番号
	//return: 実行結果（0なら成功）
	int SetPortNum(USHORT port_)
	{
		if (finishBind == true)
		{
			return ResultType::ALREADYBIND;
		}
			
		if (port_ < 49152 || port_ > 65535)
		{
			return ResultType::INVALIDNUM;
		}

		addr.sin_port = htons(port_);
		return ResultType::SUCCESS;
	}

	//ipアドレスの設定
	//param1: ipアドレス
	//return: 実行結果（0なら成功）
	int SetIpAdress(WCHAR* ip_)
	{
		if (finishBind == true)
		{
			return ResultType::ALREADYBIND;
		}

		auto result = InetPtonW(addr.sin_family, ip_, &addr.sin_addr.S_un.S_addr);
		

		//resultが1なら成功
		if (result == 1)
		{
			return ResultType::SUCCESS;
		}
		else
		{
			return ResultType::FUNCERR;
		}
	}

	//バインド
	//return: 実行結果（0なら成功）
	int Bind()
	{
		if (finishBind == true)
		{
			return ResultType::ALREADYBIND;
		}

		int result = bind(sock, (sockaddr*)&addr, sizeof(addr));

		if (result == 0)
		{
			return ResultType::SUCCESS;
		}
		else
		{
			return ResultType::FUNCERR;
		}
	}

	//ソケットの情報を受け取る
	//return: メンバのソケット
	SOCKET& Socket()
	{
		return sock;
	}

	//送信
	//param1: 送るデータのアドレス
	//param2: 送るデータのサイズ
	int Send(char* data_, size_t dataSize_)
	{
		int result = sendto(sock, data_, dataSize_, 0, (SOCKADDR*)&addr, sizeof(addr));
		if (result == SOCKET_ERROR)
		{
			return ResultType::FUNCERR;
		}
		else
		{
			return ResultType::SUCCESS;
		}
	}


	//デストラクタ
	virtual ~MySocket() = 0;
};
MySocket::~MySocket()
{
	closesocket(sock);
}

//受信用ソケット
class MSockReceive
{
	fd_set fds, readfds;
	std::vector<MySocket*> sockets;
	int timeCount;

public:
	enum ResultType
	{
		SUCCESS = 0,	//成功
		NONSOCKET,		//ソケットが準備されていない
		TIMEOUT,		//タイムアウト
		FUNCERR,		//外部関数のエラー
	};

	//コンストラクタ
	MSockReceive()
	{
		FD_ZERO(&readfds);
	}

	//ソケットの追加
	//param1: 追加するソケットのポインタ
	void AddSocket(MySocket* mysock_)
	{
		sockets.push_back(mysock_);
		FD_SET(mysock_->Socket(), &readfds);
	}

	//受信
	//param1: 制限時間
	int Receive(timeval& waitLimit_)
	{
		// 読み込み用fd_setの初期化
		// selectが毎回内容を上書きしてしまうので、毎回初期化します
		memcpy(&fds, &readfds, sizeof(fd_set));

		// fdsに設定されたソケットが読み込み可能になるまで待ちます
		timeCount = select(0, &fds, NULL, NULL, &waitLimit_);

		// タイムアウトの場合にselectは0を返します
		if (timeCount == 0) {
			return ResultType::TIMEOUT;
		}


		for (auto& socket : sockets)
		{
			// ソケットに読み込み可能データがある場合
			if (FD_ISSET(socket->Socket(), &fds)) {
				// sock1からデータを受信して表示します
				memset(socket->Buff(), 0, sizeof(buf));
				recv(sock1, buf, sizeof(buf), 0);
				printf("%s\n", buf);
			}
		}

	}
};