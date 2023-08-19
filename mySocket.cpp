#include "mySocket.h"

bool* MySocket::finishInitialize = nullptr;	//初期化されているか
fd_set* MySocket::fds = nullptr;			//受信したデータを入れる変数
fd_set* MySocket::readfds = nullptr;		//fdsを初期状態に戻すための変数
int* MySocket::activeSockets = nullptr;		//現在稼働しているソケットの数
std::map<int, MySocket*>* MySocket::receiveSockets = nullptr;		//受信用のソケット
int* MySocket::uKeyManager = nullptr;		//ユニークキーの番号管理
size_t* MySocket::buffMax = nullptr;		//受信用容量の最大
