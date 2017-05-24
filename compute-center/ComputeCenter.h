#pragma once
#include <Eigen\Dense>
using Eigen::MatrixXd;

#include "Winsock2.h"
#pragma comment(lib,"ws2_32.lib")

#define BUFFER_SIZE 1024
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

class ComputeCenter {
private:
	WSADATA wsd;    //WSADATA变量  
	SOCKET server;    //服务器套接字  
	SOCKET* clients;    //客户端套接字  
	SOCKADDR_IN addrServ;;  //服务器地址
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

