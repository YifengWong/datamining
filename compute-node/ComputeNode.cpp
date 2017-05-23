#include "ComputeNode.h"
#include <iostream>

using std::cout;
using std::endl;


ComputeNode::ComputeNode(const char* ip, int port) {
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ip);
	servAddr.sin_port = htons(port);
}


ComputeNode::~ComputeNode() {
	closesocket(sHost); //关闭套接字  
	WSACleanup();       //释放套接字资源
}


int ComputeNode::start() {
	//初始化套结字动态库 
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	//创建套接字
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sHost) {
		cout << "socket failed!" << endl;
		WSACleanup();//释放套接字资源  
		return  -1;
	}

	int nServAddlen = sizeof(servAddr);
	int retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
	if (SOCKET_ERROR == retVal) {
		cout << "connect failed!" << endl;
		closesocket(sHost); //关闭套接字  
		WSACleanup();       //释放套接字资源  
		return -1;
	}
	return 0;
}

MatrixXd * ComputeNode::blockedRecvMatrixXd() {
	int nNetTimeout = 100000; //100秒
	setsockopt(sHost, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));

	recv(sHost, bufRecv, 5, 0);
	return nullptr;
}

void ComputeNode::blockedSendMatrixXd(MatrixXd * mat) {
	send(sHost, buf, strlen(buf), 0);
	//if (SOCKET_ERROR == retVal) {
	//	cout << "send failed!" << endl;
	//	closesocket(sHost); //关闭套接字  
	//	WSACleanup();       //释放套接字资源  
	//	return;
	//}
}
