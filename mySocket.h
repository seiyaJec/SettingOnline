#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>
#include <vector>

class MySocket
{
private:
	SOCKET sock;			//�\�P�b�g�ԍ�
	sockaddr_in addr;		//�\�P�b�g�ƃ����N����ip��|�[�g�ԍ�

	bool finishBind;		//�o�C���h������

public:
	//�R���X�g���N�^
	//param1: �\�P�b�g���i�[�ł���T�C�Y
	MySocket()
		:finishBind(false)
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
	SOCKET& Socket()
	{
		return sock;
	}

	//���M
	//param1: ����f�[�^�̃A�h���X
	//param2: ����f�[�^�̃T�C�Y
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


	//�f�X�g���N�^
	virtual ~MySocket() = 0;
};
MySocket::~MySocket()
{
	closesocket(sock);
}

//��M�p�\�P�b�g
class MSockReceive
{
	fd_set fds, readfds;
	std::vector<MySocket*> sockets;
	int timeCount;

public:
	enum ResultType
	{
		SUCCESS = 0,	//����
		NONSOCKET,		//�\�P�b�g����������Ă��Ȃ�
		TIMEOUT,		//�^�C���A�E�g
		FUNCERR,		//�O���֐��̃G���[
	};

	//�R���X�g���N�^
	MSockReceive()
	{
		FD_ZERO(&readfds);
	}

	//�\�P�b�g�̒ǉ�
	//param1: �ǉ�����\�P�b�g�̃|�C���^
	void AddSocket(MySocket* mysock_)
	{
		sockets.push_back(mysock_);
		FD_SET(mysock_->Socket(), &readfds);
	}

	//��M
	//param1: ��������
	int Receive(timeval& waitLimit_)
	{
		// �ǂݍ��ݗpfd_set�̏�����
		// select��������e���㏑�����Ă��܂��̂ŁA���񏉊������܂�
		memcpy(&fds, &readfds, sizeof(fd_set));

		// fds�ɐݒ肳�ꂽ�\�P�b�g���ǂݍ��݉\�ɂȂ�܂ő҂��܂�
		timeCount = select(0, &fds, NULL, NULL, &waitLimit_);

		// �^�C���A�E�g�̏ꍇ��select��0��Ԃ��܂�
		if (timeCount == 0) {
			return ResultType::TIMEOUT;
		}


		for (auto& socket : sockets)
		{
			// �\�P�b�g�ɓǂݍ��݉\�f�[�^������ꍇ
			if (FD_ISSET(socket->Socket(), &fds)) {
				// sock1����f�[�^����M���ĕ\�����܂�
				memset(socket->Buff(), 0, sizeof(buf));
				recv(sock1, buf, sizeof(buf), 0);
				printf("%s\n", buf);
			}
		}

	}
};