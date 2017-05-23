#pragma once
#include <Eigen\Dense>
using Eigen::MatrixXd;

#include "winsock2.h"

class ComputeNode {
private:
	WSADATA wsd; //WSADATA����  
	SOCKET sHost; //�������׽���  
	SOCKADDR_IN servAddr; //��������ַ

public:
	ComputeNode(const char* ip, int port);
	~ComputeNode();

	int start();
	MatrixXd* blockedRecvMatrixXd();
	void blockedSendMatrixXd(MatrixXd* mat);
};

