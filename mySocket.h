#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>

class MySocket
{
	SOCKET sock;			//�\�P�b�g�ԍ�
	sockaddr_in addr;		//�\�P�b�g�ƃ����N����ip��|�[�g�ԍ�
	size_t buffSize;		//�\�P�b�g�Ɋi�[����f�[�^�̃T�C�Y
	char* buff;				//�f�[�^�̒��g

	bool finishBind;		//�o�C���h������

public:
	//�R���X�g���N�^
	//param1: �\�P�b�g���i�[�ł���T�C�Y
	MySocket(size_t buffSize_)
		:buffSize(buffSize_)
		,buff(new char[buffSize_])
		,finishBind(false)
	{
		sock = socket(AF_INET, SOCK_DGRAM, 0);		//AF_INET...IPv4,�@SOCK_DGRAM...�\�P�b�g�̎�ށ@, 0...�v���g�R���̎w��
		addr.sin_family = AF_INET;
		addr.sin_port = htons(49152);
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
	}

	enum ResultType
	{ 
		SUCCESS = 0,	//����
		FUNCERR,		//�O���֐��̃G���[
		INVALIDNUM,		//�����Ȑ���
		ALREADYBIND,	//�o�C���h�ς݂̂��ߎ��s�s��
	};

	//�|�[�g�ԍ��̐ݒ�
	//param1: �|�[�g�ԍ�
	//return: ���s���ʁi0�Ȃ琬���j
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

	//ip�A�h���X�̐ݒ�
	//param1: ip�A�h���X
	//return: ���s���ʁi0�Ȃ琬���j
	int SetIpAdress(WCHAR* ip_)
	{
		if (finishBind == true)
		{
			return ResultType::ALREADYBIND;
		}

		auto result = InetPtonW(addr.sin_family, ip_, &addr.sin_addr.S_un.S_addr);
		

		//result��1�Ȃ琬��
		if (result == 1)
		{
			return ResultType::SUCCESS;
		}
		else
		{
			return ResultType::FUNCERR;
		}
	}

	//�o�C���h
	//return: ���s���ʁi0�Ȃ琬���j
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

	//�\�P�b�g�̏����󂯎��
	//return: �����o�̃\�P�b�g
	SOCKET& getSocket()
	{
		return sock;
	}

	//�\�P�b�g�̎g�p���I������
	void Close()
	{
		closesocket(sock);
	}

	//�f�X�g���N�^
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