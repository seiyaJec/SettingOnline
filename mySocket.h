#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>

class MySocket
{
	SOCKET sock;			//ソケット番号
	sockaddr_in addr;		//ソケットとリンクするipやポート番号
	size_t buffSize;		//ソケットに格納するデータのサイズ
	char* buff;				//データの中身

	bool finishBind;		//バインドしたか

public:
	//コンストラクタ
	//param1: ソケットが格納できるサイズ
	MySocket(size_t buffSize_)
		:buffSize(buffSize_)
		,buff(new char[buffSize_])
		,finishBind(false)
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
	SOCKET& getSocket()
	{
		return sock;
	}

	//ソケットの使用を終了する
	void Close()
	{
		closesocket(sock);
	}

	//デストラクタ
	virtual ~MySocket() = 0;

};


class MSock_Send : public MySocket
{
	MSock_Send(size_t buffSize_)
	{

	}
};


class MSock_Receive : public MySocket
{

};