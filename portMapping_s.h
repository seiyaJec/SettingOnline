#pragma once
#include <string>
#include <natupnp.h>
#include <objbase.h>
#include <oleauto.h>
#include <winsock.h>
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"oleaut32.lib")
class PortMapping
{
	IUPnPNAT* nat;			//Nat情報
	IStaticPortMappingCollection* maps;	//ポートマッピングのリスト
	std::wstring ip;		//マッピングしたいipアドレス（変換前）
	BSTR localIp;			//マッピングしたいIPアドレス（変換後）
	BSTR udp;				//UDPかTCPかを指定する文字列
	IStaticPortMapping* map;	//ポートマッピングの情報
	BSTR name;				//リストに登録される名前
	int port;				//マッピングしたIPのどのポートに送信されるか
	static bool created;	//すでに生成されているか

public:
	/// <summary>
	/// ポートマッピングの生成、初期化
	/// </summary>
	/// <param name="port_">使用するポート番号</param>
	/// <param name="local_">マッピングするローカルipアドレス</param>
	/// <returns>生成されたインスタンスへのポインタ</returns>
	static PortMapping* CreateAndInitialize(int port_, std::wstring local_)
	{
		if (created == true)
		{
			return nullptr;
		}

		PortMapping* ins = new PortMapping();
		CoInitialize(nullptr);//comの初期化
		CoCreateInstance(CLSID_UPnPNAT, nullptr, CLSCTX_ALL, IID_IUPnPNAT, reinterpret_cast<void**>(&(ins->nat)));	//インスタンスの取得
		ins->nat->get_StaticPortMappingCollection(&(ins->maps));//ポートマッピングのリスト取得
		if (!(ins->maps))return nullptr;
		ins->port = port_,
		ins->ip = local_;//マッピングしたいローカルアドレスを指定
		ins->localIp = SysAllocString(ins->ip.c_str());
		ins->udp = SysAllocString(L"UDP");//UDP通信を指定 TCPの場合はTCPを指定する
		ins->name = SysAllocString(L"Test");//リストに登録される名前
		ins->maps->Add(ins->port, ins->udp, ins->port, ins->localIp, VARIANT_TRUE, ins->name, &(ins->map));//リストに登録

		return ins;
	}

	/// <summary>
	/// 終了処理
	/// </summary>
	/// <param name="ptr_">インスタンスへのポインタ</param>
	static void Finalize(PortMapping* ptr_)
	{
		delete ptr_;
	}

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PortMapping()
	{
		maps->Remove(port, udp);//リストから削除
		SysFreeString(name);
		SysFreeString(udp);
		SysFreeString(localIp);
		map->Release();
		maps->Release();
		nat->Release();
		CoUninitialize();//リソースの開放など
	}
};
