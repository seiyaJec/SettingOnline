#include "mySocket.h"

bool* MySocket::finishInitialize = nullptr;	//����������Ă��邩
fd_set* MySocket::fds = nullptr;			//��M�����f�[�^������ϐ�
fd_set* MySocket::readfds = nullptr;		//fds��������Ԃɖ߂����߂̕ϐ�
int* MySocket::activeSockets = nullptr;		//���݉ғ����Ă���\�P�b�g�̐�
std::map<int, MySocket*>* MySocket::receiveSockets = nullptr;		//��M�p�̃\�P�b�g
int* MySocket::uKeyManager = nullptr;		//���j�[�N�L�[�̔ԍ��Ǘ�
size_t* MySocket::buffMax = nullptr;		//��M�p�e�ʂ̍ő�
