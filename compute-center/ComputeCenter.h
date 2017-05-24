#pragma once
#include <Eigen\Dense>
using Eigen::MatrixXd;

#include "Winsock2.h"
#pragma comment(lib,"ws2_32.lib")

#define BUFFER_SIZE 1024
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

class ComputeCenter {
private:
	WSADATA wsd;    //WSADATA����  
	SOCKET server;    //�������׽���  
	SOCKET* clients;    //�ͻ����׽���  
	SOCKADDR_IN addrServ;;  //��������ַ
	int clientNum;

public:
	ComputeCenter(int port, int clientNum);
	~ComputeCenter();

	int start();
	SOCKET* getClient(int num);
	void blockSendMatrix(MatrixXd* mat, SOCKET* client);
	void blockBroadcastMatrix(MatrixXd* mat);
	MatrixXd** blockRecvAllMatrix(int singleRow, int singleCol);
};

