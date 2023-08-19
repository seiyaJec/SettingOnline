#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>
#include <map>
#include <string>
#pragma comment(lib, "ws2_32.lib")


class MySocket
{
private:
	SOCKET sock;			//ソケット番号
	SOCKADDR_IN addr;		//ソケットとリンクするipやポート番号
	bool finishBind;		//バインドしたか
	bool finishSetReceive;	//受信用の設定が終了したか
	int	 uKey;				//ソケットごとに割り当てられるユニークキー
	char* receiveBuff;		//受信用のデータ格納変数
	size_t  receiveBuffSize;	//受信時に受け取れる最大容量

	static bool*   finishInitialize;	//初期化されているか
	static fd_set* fds;					//受信したデータを入れる変数
	static fd_set* readfds;				//fdsを初期状態に戻すための変数
	static int*    activeSockets;		//現在稼働しているソケットの数
	static std::map<int, MySocket*>* receiveSockets;	//受信用のソケット
	static int* uKeyManager;			//ユニークキーの番号管理
	static size_t* buffMax;				//受信用容量の最大

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

		finishBind = true;
	}


	//ファイルディスクリプタの初期化処理
	void InitializeFds()
	{
		if (finishInitialize == nullptr)
		{
			fds = new fd_set();
			readfds = new fd_set();
			//初期化
			FD_ZERO(readfds);

			//fdsの処理用変数のインスタンス生成
			finishInitialize = new bool(true);
			activeSockets = new int(0);

			//受信用の変数のインスタンス生成
			receiveSockets = new std::map<int, MySocket*>();
			uKeyManager = new int(0);
			buffMax = new size_t(0);
		}

		++(*activeSockets);
	}


	//ファイルディスクリプタの終了処理
	void FinalizeFds()
	{
		FD_CLR(sock, readfds);

		--(*activeSockets);

		if (*activeSockets <= 0)
		{
			delete fds;
			delete readfds;
			delete finishInitialize;
			delete activeSockets;
			delete receiveSockets;
			delete uKeyManager;
			delete buffMax;
		}
	}

public:
	//コンストラクタ
	MySocket()
		:finishBind(false)
		,finishSetReceive(false)
		,receiveBuff(nullptr)
		,receiveBuffSize(0)
	{
		InitializeFds();
		sock = socket(AF_INET, SOCK_DGRAM, 0);		//AF_INET...IPv4,　SOCK_DGRAM...ソケットの種類　, 0...プロトコルの指定
		addr.sin_family = AF_INET;
		addr.sin_port = htons(49152);
		addr.sin_addr.S_un.S_addr = INADDR_ANY;

		uKey = (*uKeyManager)++;
	}


	enum ResultType
	{ 
		SUCCESS = 0,	//成功
		BINDERR,		//バインド時のエラー
		FUNCERR,		//外部関数のエラー
		INVALIDNUM,		//無効な数字
		ALREADYBIND,	//バインド済みのため実行不可
		ALREADYSETRECEIVE, //受信設定済みのため実行不可
		NONSOCKET,		//ソケットが準備されていない
		TIMEOUT,		//タイムアウト
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
	int SetIpAdress(const std::wstring& ip_)
	{
		if (finishBind == true)
		{
			return ResultType::ALREADYBIND;
		}

		auto result = InetPtonW(addr.sin_family, ip_.c_str(), &addr.sin_addr.S_un.S_addr);
		

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


	//ソケットの情報を受け取る
	//return: メンバのソケット
	SOCKET& Socket()
	{
		return sock;
	}

	//送信
	//param1: 送るデータのアドレス
	//param2: 送るデータのサイズ
	int Send(const char* data_, size_t dataSize_)
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

	//受信用として登録
	//param1: 受信できる容量
	//return: 実行結果
	int SetReceive(size_t bufferSize_)
	{
		if (finishBind == false)
		{
			if (this->Bind() != ResultType::SUCCESS)
			{
				return ResultType::BINDERR;
			}
		}
		else if (finishSetReceive == true)
		{
			return ResultType::ALREADYSETRECEIVE;
		}

		FD_SET(sock, readfds);
		(*receiveSockets)[uKey] = this;
		receiveBuff = new char[bufferSize_];
		receiveBuffSize = bufferSize_;

		*buffMax = max(*buffMax, bufferSize_);

		finishSetReceive = true;

		return ResultType::SUCCESS;
	}


	//受信
	//param1: 制限時間
	//param2: 受け取るデータの容量
	//return: 実行結果
	static int Receive(timeval& waitLimit_)
	{
		// 読み込み用fd_setの初期化
		// selectが毎回内容を上書きしてしまうので、毎回初期化します
		memcpy(fds, readfds, sizeof(fd_set));

		// fdsに設定されたソケットが読み込み可能になるまで待ちます
		int result = select(0, fds, NULL, NULL, &waitLimit_);

		// タイムアウトの場合
		if (result == 0) {
			return ResultType::TIMEOUT;
		}
		//その他エラーの場合
		else if (result == SOCKET_ERROR)
		{
			return ResultType::FUNCERR;
		}

		//ソケットごとに読み込み可能なデータを確認
		for (auto& rsock : *receiveSockets)
		{
			if (FD_ISSET(rsock.second->sock, fds)) {
				memset(rsock.second->receiveBuff, 0, rsock.second->receiveBuffSize);
				recv(rsock.second->sock, rsock.second->receiveBuff, rsock.second->receiveBuffSize, 0);
			}
		}

		return ResultType::SUCCESS;
	}

	//受信した内容を読み込む
	const char* ReadReceive()
	{
		return receiveBuff;
	}

	//デストラクタ
	~MySocket()
	{
		FinalizeFds();
		delete[] receiveBuff;
		closesocket(sock);
	}
};