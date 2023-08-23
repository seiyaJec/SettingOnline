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
		IStaticPortMapping* p_map;	//�|�[�g�}�b�s���O�̏��
		int port;
		BSTR ip;
		BSTR udp;				//UDP��TCP�����w�肷�镶����
		BSTR name;				//���X�g�ɓo�^����閼�O

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


	IUPnPNAT* nat;			//Nat���
	IStaticPortMappingCollection* maps;	//�|�[�g�}�b�s���O�̃��X�g
	std::vector<MyPortMap> map;
	std::wstring localIp;		//�}�b�s���O������ip�A�h���X�i�ϊ��O�j
	static bool* created;	//���łɐ�������Ă��邩

	MyPortMapping() {};

	//���[�J��IP�A�h���X�̎擾
	bool GetLocalIP()
	{
		//�z�X�g�����擾����
		char hostname[256];
		if (gethostname(hostname, sizeof(hostname)) != 0) 
		{
			return false;
		}

		//�z�X�g������IP�A�h���X���擾����
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

		//char����wchar_t�ɕϊ�
		size_t newsize = strlen(ip) + 1;
		wchar_t* wcstring = new wchar_t[newsize];
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, wcstring, newsize, ip, _TRUNCATE);

		//�����o�ɑ��
		localIp.insert(0, wcstring);

		delete[] ip;
		delete[] wcstring;
	}


public:
	/// <summary>
	/// �|�[�g�}�b�s���O�̐����A������
	/// </summary>
	/// <param name="port_">�g�p����|�[�g�ԍ�</param>
	/// <param name="local_">�}�b�s���O���郍�[�J��ip�A�h���X</param>
	/// <returns>�������ꂽ�C���X�^���X�ւ̃|�C���^</returns>
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

		CoInitialize(nullptr);//com�̏�����
		CoCreateInstance(CLSID_UPnPNAT, nullptr, CLSCTX_ALL, IID_IUPnPNAT, reinterpret_cast<void**>(&(ins->nat)));	//�C���X�^���X�̎擾
		ins->nat->get_StaticPortMappingCollection(&(ins->maps));//�|�[�g�}�b�s���O�̃��X�g�擾
		if (!(ins->maps))return nullptr;


		return ins;
	}

	/// <summary>
	/// �ǉ�
	/// </summary>
	/// <param name="port_">�}�b�s���O����|�[�g�ԍ�</param>
	void Add(int port_)
	{
		map.push_back(MyPortMap());
		auto& nm = map[map.size() - 1];
		nm.port = port_;
		nm.ip = SysAllocString(localIp.c_str());
		nm.udp = SysAllocString(L"UDP");//UDP�ʐM���w�� TCP�̏ꍇ��TCP���w�肷��
		nm.name = SysAllocString(L"PortMap");//���X�g�ɓo�^����閼�O
		maps->Add(nm.port, nm.udp, nm.port, nm.ip, VARIANT_TRUE, nm.name, &(map[map.size() - 1].p_map));//���X�g�ɓo�^
	}

	/// <summary>
	/// �I������
	/// </summary>
	/// <param name="ptr_">�C���X�^���X�ւ̃|�C���^</param>
	static void Finalize(MyPortMapping* ptr_)
	{
		delete ptr_;
	}

	const std::wstring& getLocal()
	{
		return localIp;
	}

	/// <summary>
	/// �f�X�g���N�^
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
		CoUninitialize();//���\�[�X�̊J���Ȃ�
	}
};
