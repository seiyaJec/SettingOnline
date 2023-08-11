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
	IUPnPNAT* nat;			//Nat���
	IStaticPortMappingCollection* maps;	//�|�[�g�}�b�s���O�̃��X�g
	std::wstring ip;		//�}�b�s���O������ip�A�h���X�i�ϊ��O�j
	BSTR localIp;			//�}�b�s���O������IP�A�h���X�i�ϊ���j
	BSTR udp;				//UDP��TCP�����w�肷�镶����
	IStaticPortMapping* map;	//�|�[�g�}�b�s���O�̏��
	BSTR name;				//���X�g�ɓo�^����閼�O
	int port;				//�}�b�s���O����IP�̂ǂ̃|�[�g�ɑ��M����邩
	static bool created;	//���łɐ�������Ă��邩

public:
	/// <summary>
	/// �|�[�g�}�b�s���O�̐����A������
	/// </summary>
	/// <param name="port_">�g�p����|�[�g�ԍ�</param>
	/// <param name="local_">�}�b�s���O���郍�[�J��ip�A�h���X</param>
	/// <returns>�������ꂽ�C���X�^���X�ւ̃|�C���^</returns>
	static PortMapping* CreateAndInitialize(int port_, std::wstring local_)
	{
		if (created == true)
		{
			return nullptr;
		}

		PortMapping* ins = new PortMapping();
		CoInitialize(nullptr);//com�̏�����
		CoCreateInstance(CLSID_UPnPNAT, nullptr, CLSCTX_ALL, IID_IUPnPNAT, reinterpret_cast<void**>(&(ins->nat)));	//�C���X�^���X�̎擾
		ins->nat->get_StaticPortMappingCollection(&(ins->maps));//�|�[�g�}�b�s���O�̃��X�g�擾
		if (!(ins->maps))return nullptr;
		ins->port = port_,
		ins->ip = local_;//�}�b�s���O���������[�J���A�h���X���w��
		ins->localIp = SysAllocString(ins->ip.c_str());
		ins->udp = SysAllocString(L"UDP");//UDP�ʐM���w�� TCP�̏ꍇ��TCP���w�肷��
		ins->name = SysAllocString(L"Test");//���X�g�ɓo�^����閼�O
		ins->maps->Add(ins->port, ins->udp, ins->port, ins->localIp, VARIANT_TRUE, ins->name, &(ins->map));//���X�g�ɓo�^

		return ins;
	}

	/// <summary>
	/// �I������
	/// </summary>
	/// <param name="ptr_">�C���X�^���X�ւ̃|�C���^</param>
	static void Finalize(PortMapping* ptr_)
	{
		delete ptr_;
	}

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~PortMapping()
	{
		maps->Remove(port, udp);//���X�g����폜
		SysFreeString(name);
		SysFreeString(udp);
		SysFreeString(localIp);
		map->Release();
		maps->Release();
		nat->Release();
		CoUninitialize();//���\�[�X�̊J���Ȃ�
	}
};
