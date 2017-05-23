#pragma once
#include <Eigen\Dense>
using Eigen::MatrixXd;

#include "winsock2.h"

class NodeCenter {
private:
	WSADATA wsd;    //WSADATA����  
	SOCKET server;    //�������׽���  
	SOCKET* clients;    //�ͻ����׽���  
	SOCKADDR_IN addrServ;;  //��������ַ
	int clientNum;

public:
	NodeCenter(int port, int clientNum);
	~NodeCenter();

	int start();
	void blockBroadcastMatrix(MatrixXd* mat);
	MatrixXd** blockRecvAllMatrix(int* count);
};

