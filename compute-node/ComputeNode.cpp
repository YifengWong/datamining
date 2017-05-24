#include "ComputeNode.h"
#include "NumParser.h"
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
	char *buffer = new char[BUFFER_SIZE];
	recv(sHost, buffer, INT_LEN * 2, 0);
	int rows = getInteger(buffer);
	int cols = getInteger(buffer + 4);

	MatrixXd* mat = new MatrixXd(rows, cols);
	int allBytesCount = rows*cols*DBL_LEN;
	int numCount = 0;
	for (int i = 0; i < (allBytesCount / BUFFER_SIZE); i++) {
		recv(sHost, buffer, BUFFER_SIZE, 0);
		for (int j = 0; j < BUFFER_SIZE; ) {
			(*mat)(numCount / cols, numCount % cols) = getDouble(buffer + j);
			j += DBL_LEN;
			numCount++;
		}
	}
	int last = allBytesCount % BUFFER_SIZE;
	recv(sHost, buffer, last, 0);
	for (int j = 0; j < last; ) {
		(*mat)(numCount / cols, numCount % cols) = getDouble(buffer + j);
		j += DBL_LEN;
		numCount++;
	}

	return mat;
}

void ComputeNode::blockedSendMatrixXd(MatrixXd * mat) {
	char *buffer = new char[BUFFER_SIZE];
	int rows = mat->rows();
	int cols = mat->cols();

	memcpy(buffer, &rows, INT_LEN);
	memcpy(buffer+INT_LEN, &cols, INT_LEN);
	send(sHost, buffer, INT_LEN*2, 0);

	int numCount = 0;
	int allBytesCount = rows*cols*DBL_LEN;
	for (int i = 0; i < (allBytesCount / BUFFER_SIZE); i++) {
		for (int j = 0; j < BUFFER_SIZE; ) {
			memcpy(buffer+j, &(*mat)(numCount / cols, numCount % cols), DBL_LEN);
			j += DBL_LEN;
			numCount++;
		}
		send(sHost, buffer, BUFFER_SIZE, 0);
	}
	int last = allBytesCount % BUFFER_SIZE;
	for (int j = 0; j < last; ) {
		memcpy(buffer + j, &(*mat)(numCount / cols, numCount % cols), DBL_LEN);
		j += DBL_LEN;
		numCount++;
	}
	send(sHost, buffer, last, 0);

	//if (SOCKET_ERROR == retVal) {
	//	cout << "send failed!" << endl;
	//	closesocket(sHost); //关闭套接字  
	//	WSACleanup();       //释放套接字资源  
	//	return;
	//}
}
