#pragma once
#define _WINSOCKAPI_
#include <string>
#include <memory>
#include <vector>
#include <natupnp.h>
#include <objbase.h>
#include <oleauto.h>
#include <WS2tcpip.h>
#include <winsock.h>
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"oleaut32.lib")
class MyPortMapping
{
	class MyPortMap
	{
	public:
		IStaticPortMapping* p_map;	//ポートマッピングの情報
		int port;
		BSTR ip;
		BSTR udp;				//UDPかTCPかを指定する文字列
		BSTR name;				//リストに登録される名前

		~MyPortMap()
		{
			if (p_map != nullptr)
			{
				SysFreeString(name);
				SysFreeString(udp);
				SysFreeString(ip);
				p_map->Release();
			}
		}
	};


	IUPnPNAT* nat;			//Nat情報
	IStaticPortMappingCollection* maps;	//ポートマッピングのリスト
	std::vector<MyPortMap> map;
	std::wstring localIp;		//マッピングしたいipアドレス（変換前）
	static bool* created;	//すでに生成されているか

	MyPortMapping() {};

	//ローカルIPアドレスの取得
	bool GetLocalIP()
	{
		//ホスト名を取得する
		char hostname[256];
		if (gethostname(hostname, sizeof(hostname)) != 0) 
		{
			return false;
		}

		//ホスト名からIPアドレスを取得する
		addrinfo hint;
		memset(&hint, 0, sizeof(hint));
		hint.ai_socktype = SOCK_STREAM;
		hint.ai_family = AF_INET;
		addrinfo* getip;
		if (getaddrinfo(hostname, NULL, &hint, &getip) != 0)
		{
			return false;
		}
		char* ip = new char[100];
		in_addr addr;
		addr.S_un = ((struct sockaddr_in*)(getip->ai_addr))->sin_addr.S_un;
		inet_ntop(AF_INET, &(addr), ip, 100);
		freeaddrinfo(getip);

		//charからwchar_tに変換
		size_t newsize = strlen(ip) + 1;
		wchar_t* wcstring = new wchar_t[newsize];
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, wcstring, newsize, ip, _TRUNCATE);

		//メンバに代入
		localIp.insert(0, wcstring);

		delete[] ip;
		delete[] wcstring;
	}


public:
	/// <summary>
	/// ポートマッピングの生成、初期化
	/// </summary>
	/// <param name="port_">使用するポート番号</param>
	/// <param name="local_">マッピングするローカルipアドレス</param>
	/// <returns>生成されたインスタンスへのポインタ</returns>
	static std::shared_ptr<MyPortMapping> CreateAndInitialize()
	{
		if (created != nullptr)
		{
			return nullptr;
		}

		auto ins = std::shared_ptr<MyPortMapping>(new MyPortMapping());
		if (ins->GetLocalIP() == false) 
		{
			return nullptr;
		}
		created = new bool(true);

		CoInitialize(nullptr);//comの初期化
		CoCreateInstance(CLSID_UPnPNAT, nullptr, CLSCTX_ALL, IID_IUPnPNAT, reinterpret_cast<void**>(&(ins->nat)));	//インスタンスの取得
		ins->nat->get_StaticPortMappingCollection(&(ins->maps));//ポートマッピングのリスト取得
		if (!(ins->maps))return nullptr;


		return ins;
	}

	/// <summary>
	/// 追加
	/// </summary>
	/// <param name="port_">マッピングするポート番号</param>
	void Add(int port_)
	{
		map.push_back(MyPortMap());
		auto& nm = map[map.size() - 1];
		nm.port = port_;
		nm.ip = SysAllocString(localIp.c_str());
		nm.udp = SysAllocString(L"UDP");//UDP通信を指定 TCPの場合はTCPを指定する
		nm.name = SysAllocString(L"PortMap");//リストに登録される名前
		maps->Add(nm.port, nm.udp, nm.port, nm.ip, VARIANT_TRUE, nm.name, &(map[map.size() - 1].p_map));//リストに登録
	}

	/// <summary>
	/// 終了処理
	/// </summary>
	/// <param name="ptr_">インスタンスへのポインタ</param>
	static void Finalize(MyPortMapping* ptr_)
	{
		delete ptr_;
	}

	const std::wstring& getLocal()
	{
		return localIp;
	}

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MyPortMapping()
	{
		for (auto& mapi : map)
		{
			maps->Remove(mapi.port, mapi.udp);
		}
		maps->Release();
		nat->Release();
		delete created;
		CoUninitialize();//リソースの開放など
	}
};
