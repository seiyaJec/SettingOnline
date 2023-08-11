#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include "nlohmann/json.hpp"
#include "portMapping_s.h"
#include "myNetwork.h"

//WSAStartUp�̃G���[���m�F
void CheckWSAStartError(int err)
{
	switch (err) {
	case WSASYSNOTREADY:
		printf("WSASYSNOTREADY\n");
		break;
	case WSAVERNOTSUPPORTED:
		printf("WSAVERNOTSUPPORTED\n");
		break;
	case WSAEINPROGRESS:
		printf("WSAEINPROGRESS\n");
		break;
	case WSAEPROCLIM:
		printf("WSAEPROCLIM\n");
		break;
	case WSAEFAULT:
		printf("WSAEFAULT\n");
		break;
	}
}

void MyNetWork::Initialize()
{
	SOCKET sock1, sock2;						//�\�P�b�g��2�쐬
	struct sockaddr_in addr1, addr2;			//ip�A�h���X��|�[�g�ԍ����擾���邽�߂̔ԍ�
	fd_set fds, readfds;						//�\�P�b�g�̏�Ԃ�ǂݎ�邽�߂̍\����
	char buf[2048];								//�\�P�b�g����󂯎�����f�[�^���i�[����ϐ�
	int n;										//n�Ƃ́H�H�H�H
	struct timeval tv;							//���̎󂯎�肪�^�C���A�E�g����܂ł̎��ԁi�b�ƃ}�C�N���b�̑g�ݍ��킹�j
	WSADATA wsaData;							//WSA�̐ݒ�ɕK�v�ȍ\����

	PortMapping* portmap = PortMapping::CreateAndInitialize();
	if (portmap == nullptr)
	{
		return -1;
	}

	int err = WSAStartup(MAKEWORD(2, 0), &wsaData);		//winsock�𗘗p����ۂɕK���K�v�ȏ������֐�
	if (err != 0)
	{
		CheckWSAStartError(err);
		PortMapping::Finalize(portmap);
		return -2;
	}

	// ��M�\�P�b�g��2���܂�
	sock1 = socket(AF_INET, SOCK_DGRAM, 0);		//AF_INET...IPv4,�@SOCK_DGRAM...�\�P�b�g�̎�ށ@, 0...�v���g�R���̎w��
	sock2 = socket(AF_INET, SOCK_DGRAM, 0);		//����

	addr1.sin_family = AF_INET;					//addr���̐ڑ����@�̐ݒ�@
	addr2.sin_family = AF_INET;					//����

	//InetPtonW(addr1.sin_family, TEXT("127.0.0.1"), &addr1.sin_addr.S_un.S_addr);		//ip�A�h���X�̐ݒ�
	//InetPtonW(addr2.sin_family, TEXT("127.0.0.1"), &addr2.sin_addr.S_un.S_addr);		//���l

	// 2�̕ʁX�̃|�[�g�ő҂��߂ɕʂ̃|�[�g�ԍ������ꂼ��ݒ肵�܂�
	addr1.sin_port = htons(54321);
	addr2.sin_port = htons(22222);

	addr1.sin_addr.S_un.S_addr = INADDR_ANY;
	addr2.sin_addr.S_un.S_addr = INADDR_ANY;

	// 2�̕ʁX�̃|�[�g�ő҂悤��bind���܂�
	bind(sock1, (sockaddr*)&addr1, sizeof(addr1));
	bind(sock2, (sockaddr*)&addr2, sizeof(addr2));

	// fd_set�̏��������܂�
	FD_ZERO(&readfds);

	// select�ő҂ǂݍ��݃\�P�b�g�Ƃ���sock1��o�^���܂�
	FD_SET(sock1, &readfds);
	// select�ő҂ǂݍ��݃\�P�b�g�Ƃ���sock2��o�^���܂�
	FD_SET(sock2, &readfds);

	// 10�b�Ń^�C���A�E�g����悤�ɂ��܂�
	tv.tv_sec = 60;
	tv.tv_usec = 0;

	// ���̃T���v���ł́A10�b�ԃf�[�^����M���Ȃ��ƃ��[�v�𔲂��܂�
	while (1) {
		// �ǂݍ��ݗpfd_set�̏�����
		// select��������e���㏑�����Ă��܂��̂ŁA���񏉊������܂�
		memcpy(&fds, &readfds, sizeof(fd_set));

		// fds�ɐݒ肳�ꂽ�\�P�b�g���ǂݍ��݉\�ɂȂ�܂ő҂��܂�
		n = select(0, &fds, NULL, NULL, &tv);

		// �^�C���A�E�g�̏ꍇ��select��0��Ԃ��܂�
		if (n == 0) {
			// ���[�v���甲���܂�
			printf("timeout\n");
			break;
		}

		// sock1�ɓǂݍ��݉\�f�[�^������ꍇ
		if (FD_ISSET(sock1, &fds)) {
			// sock1����f�[�^����M���ĕ\�����܂�
			memset(buf, 0, sizeof(buf));
			recv(sock1, buf, sizeof(buf), 0);
			printf("%s\n", buf);
		}

		// sock2�ɓǂݍ��݉\�f�[�^������ꍇ
		if (FD_ISSET(sock2, &fds)) {
			// sock2����f�[�^����M���ĕ\�����܂�
			memset(buf, 0, sizeof(buf));
			recv(sock2, buf, sizeof(buf), 0);
			printf("%s\n", buf);
		}
	}

	closesocket(sock1);
	closesocket(sock2);

	WSACleanup();

	PortMapping::Finalize(portmap);

	return 0;
}