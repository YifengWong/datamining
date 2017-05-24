#pragma once
#include <Eigen\Dense>
using Eigen::MatrixXd;

#include "Winsock2.h"
#pragma comment(lib,"ws2_32.lib")

#define BUFFER_SIZE 1024
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1


class ComputeNode {
private:
	WSADATA wsd; //WSADATA����  
	SOCKET sHost; //�������׽���  
	SOCKADDR_IN servAddr; //��������ַ
	void static myBlockedRecv(SOCKET s, char* buf, int len, int flags);

public:
	ComputeNode(const char* ip, int port);
	~ComputeNode();

	int start();
	MatrixXd* blockedRecvMatrixXd();
	void blockedSendMatrixXd(MatrixXd* mat);
	void blockedSendStepRowsAndCols(int row, int col);
	void blockedSendStepRow(MatrixXd* mat);
};

