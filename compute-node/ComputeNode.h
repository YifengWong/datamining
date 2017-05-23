#pragma once
#include <Eigen\Dense>
using Eigen::MatrixXd;

#include "winsock2.h"

class ComputeNode {
private:
	WSADATA wsd; //WSADATA变量  
	SOCKET sHost; //服务器套接字  
	SOCKADDR_IN servAddr; //服务器地址

public:
	ComputeNode(const char* ip, int port);
	~ComputeNode();

	int start();
	MatrixXd* blockedRecvMatrixXd();
	void blockedSendMatrixXd(MatrixXd* mat);
};

