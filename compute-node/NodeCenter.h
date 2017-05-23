#pragma once
#include <Eigen\Dense>
using Eigen::MatrixXd;

#include "winsock2.h"

class NodeCenter {
private:
	WSADATA wsd;    //WSADATA变量  
	SOCKET server;    //服务器套接字  
	SOCKET* clients;    //客户端套接字  
	SOCKADDR_IN addrServ;;  //服务器地址
	int clientNum;

public:
	NodeCenter(int port, int clientNum);
	~NodeCenter();

	int start();
	void blockBroadcastMatrix(MatrixXd* mat);
	MatrixXd** blockRecvAllMatrix(int* count);
};

