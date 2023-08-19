#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>
#include <map>
#include <string>
#pragma comment(lib, "ws2_32.lib")


class MySocket
{
private:
	SOCKET sock;			//�\�P�b�g�ԍ�
	SOCKADDR_IN addr;		//�\�P�b�g�ƃ����N����ip��|�[�g�ԍ�
	bool finishBind;		//�o�C���h������
	bool finishSetReceive;	//��M�p�̐ݒ肪�I��������
	int	 uKey;				//�\�P�b�g���ƂɊ��蓖�Ă��郆�j�[�N�L�[
	char* receiveBuff;		//��M�p�̃f�[�^�i�[�ϐ�
	size_t  receiveBuffSize;	//��M���Ɏ󂯎���ő�e��

	static bool*   finishInitialize;	//����������Ă��邩
	static fd_set* fds;					//��M�����f�[�^������ϐ�
	static fd_set* readfds;				//fds��������Ԃɖ߂����߂̕ϐ�
	static int*    activeSockets;		//���݉ғ����Ă���\�P�b�g�̐�
	static std::map<int, MySocket*>* receiveSockets;	//��M�p�̃\�P�b�g
	static int* uKeyManager;			//���j�[�N�L�[�̔ԍ��Ǘ�
	static size_t* buffMax;				//��M�p�e�ʂ̍ő�

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

		finishBind = true;
	}


	//�t�@�C���f�B�X�N���v�^�̏���������
	void InitializeFds()
	{
		if (finishInitialize == nullptr)
		{
			fds = new fd_set();
			readfds = new fd_set();
			//������
			FD_ZERO(readfds);

			//fds�̏����p�ϐ��̃C���X�^���X����
			finishInitialize = new bool(true);
			activeSockets = new int(0);

			//��M�p�̕ϐ��̃C���X�^���X����
			receiveSockets = new std::map<int, MySocket*>();
			uKeyManager = new int(0);
			buffMax = new size_t(0);
		}

		++(*activeSockets);
	}


	//�t�@�C���f�B�X�N���v�^�̏I������
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
	//�R���X�g���N�^
	MySocket()
		:finishBind(false)
		,finishSetReceive(false)
		,receiveBuff(nullptr)
		,receiveBuffSize(0)
	{
		InitializeFds();
		sock = socket(AF_INET, SOCK_DGRAM, 0);		//AF_INET...IPv4,�@SOCK_DGRAM...�\�P�b�g�̎�ށ@, 0...�v���g�R���̎w��
		addr.sin_family = AF_INET;
		addr.sin_port = htons(49152);
		addr.sin_addr.S_un.S_addr = INADDR_ANY;

		uKey = (*uKeyManager)++;
	}


	enum ResultType
	{ 
		SUCCESS = 0,	//����
		BINDERR,		//�o�C���h���̃G���[
		FUNCERR,		//�O���֐��̃G���[
		INVALIDNUM,		//�����Ȑ���
		ALREADYBIND,	//�o�C���h�ς݂̂��ߎ��s�s��
		ALREADYSETRECEIVE, //��M�ݒ�ς݂̂��ߎ��s�s��
		NONSOCKET,		//�\�P�b�g����������Ă��Ȃ�
		TIMEOUT,		//�^�C���A�E�g
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
	int SetIpAdress(const std::wstring& ip_)
	{
		if (finishBind == true)
		{
			return ResultType::ALREADYBIND;
		}

		auto result = InetPtonW(addr.sin_family, ip_.c_str(), &addr.sin_addr.S_un.S_addr);
		

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


	//�\�P�b�g�̏����󂯎��
	//return: �����o�̃\�P�b�g
	SOCKET& Socket()
	{
		return sock;
	}

	//���M
	//param1: ����f�[�^�̃A�h���X
	//param2: ����f�[�^�̃T�C�Y
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

	//��M�p�Ƃ��ēo�^
	//param1: ��M�ł���e��
	//return: ���s����
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


	//��M
	//param1: ��������
	//param2: �󂯎��f�[�^�̗e��
	//return: ���s����
	static int Receive(timeval& waitLimit_)
	{
		// �ǂݍ��ݗpfd_set�̏�����
		// select��������e���㏑�����Ă��܂��̂ŁA���񏉊������܂�
		memcpy(fds, readfds, sizeof(fd_set));

		// fds�ɐݒ肳�ꂽ�\�P�b�g���ǂݍ��݉\�ɂȂ�܂ő҂��܂�
		int result = select(0, fds, NULL, NULL, &waitLimit_);

		// �^�C���A�E�g�̏ꍇ
		if (result == 0) {
			return ResultType::TIMEOUT;
		}
		//���̑��G���[�̏ꍇ
		else if (result == SOCKET_ERROR)
		{
			return ResultType::FUNCERR;
		}

		//�\�P�b�g���Ƃɓǂݍ��݉\�ȃf�[�^���m�F
		for (auto& rsock : *receiveSockets)
		{
			if (FD_ISSET(rsock.second->sock, fds)) {
				memset(rsock.second->receiveBuff, 0, rsock.second->receiveBuffSize);
				recv(rsock.second->sock, rsock.second->receiveBuff, rsock.second->receiveBuffSize, 0);
			}
		}

		return ResultType::SUCCESS;
	}

	//��M�������e��ǂݍ���
	const char* ReadReceive()
	{
		return receiveBuff;
	}

	//�f�X�g���N�^
	~MySocket()
	{
		FinalizeFds();
		delete[] receiveBuff;
		closesocket(sock);
	}
};