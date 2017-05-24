#include "ComputeCenter.h"
#include "../compute-node/NumParser.h"
#include <iostream>

using std::cout;
using std::endl;


void ComputeCenter::myBlockedRecv(SOCKET s, char * buf, int len, int flags) {
	int haveRecv = 0;
	while (haveRecv != len) {
		haveRecv += recv(s, buf + haveRecv, len - haveRecv, 0);
	}
}

ComputeCenter::ComputeCenter(int port, int clientNum) {
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(port);
	addrServ.sin_addr.s_addr = INADDR_ANY;
	this->clientNum = clientNum;
	clients = new SOCKET[clientNum];
}


ComputeCenter::~ComputeCenter() {
	closesocket(server);  //关闭套接字
	for (int i = 0; i < clientNum; i++) {
		closesocket(clients[i]);  //关闭套接字  
	}
	WSACleanup();   //释放套接字资源;
}

int ComputeCenter::start() {
	//初始化套结字动态库  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		cout << "WSAStartup failed!" << endl;
		return 1;
	}

	//创建套接字  
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server) {
		cout << "socket failed!" << endl;
		WSACleanup();//释放套接字资源;  
		return -1;
	}
	//绑定套接字  
	int retVal = bind(server, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal) {
		cout << "bind failed!" << endl;
		closesocket(server); //关闭套接字  
		WSACleanup();  //释放套接字资源;  
		return -1;
	}
	//开始监听
	retVal = listen(server, 1);
	if (SOCKET_ERROR == retVal) {
		cout << "listen failed!" << endl;
		closesocket(server); //关闭套接字  
		WSACleanup();  //释放套接字资源;  
		return -1;
	}
	int nNetTimeout = 100000; //100秒
							  //接受客户端请求
	for (int i = 0; i < clientNum; i++) {
		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);
		clients[i] = accept(server, (sockaddr FAR*)&addrClient, &addrClientlen);
		if (INVALID_SOCKET == clients[i]) {
			cout << "accept failed!" << endl;
			closesocket(server); //关闭套接字  
			WSACleanup();  //释放套接字资源;  
			return -1;
		}
		setsockopt(clients[i], SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
	}
}

SOCKET * ComputeCenter::getClient(int num) {
	if (num < clientNum) {
		return &(clients[num]);
	}
	return nullptr;
}

void ComputeCenter::blockSendMatrix(MatrixXd * mat, SOCKET * client) {
	char *buffer = new char[BUFFER_SIZE];
	int rows = mat->rows();
	int cols = mat->cols();

	memcpy(buffer, &rows, INT_LEN);
	memcpy(buffer + INT_LEN, &cols, INT_LEN);
	send(*client, buffer, INT_LEN * 2, 0);

	int numCount = 0;
	int allBytesCount = rows*cols*DBL_LEN;
	for (int i = 0; i < (allBytesCount / BUFFER_SIZE); i++) {
		for (int j = 0; j < BUFFER_SIZE; ) {
			memcpy(buffer + j, &(*mat)(numCount / cols, numCount % cols), DBL_LEN);
			j += DBL_LEN;
			numCount++;
		}
		send(*client, buffer, BUFFER_SIZE, 0);
	}
	int last = allBytesCount % BUFFER_SIZE;
	for (int j = 0; j < last; ) {
		memcpy(buffer + j, &(*mat)(numCount / cols, numCount % cols), DBL_LEN);
		j += DBL_LEN;
		numCount++;
	}
	send(*client, buffer, last, 0);
}

void ComputeCenter::blockBroadcastMatrix(MatrixXd * mat) {
	char *buffer = new char[BUFFER_SIZE];
	int rows = mat->rows();
	int cols = mat->cols();

	memcpy(buffer, &rows, INT_LEN);
	memcpy(buffer + INT_LEN, &cols, INT_LEN);
	for (int i = 0; i < clientNum; i++) {
		send(clients[i], buffer, INT_LEN * 2, 0);
	}

	int numCount = 0;
	int allBytesCount = rows*cols*DBL_LEN;
	for (int i = 0; i < (allBytesCount / BUFFER_SIZE); i++) {
		for (int j = 0; j < BUFFER_SIZE; ) {
			memcpy(buffer + j, &(*mat)(numCount / cols, numCount % cols), DBL_LEN);
			j += DBL_LEN;
			numCount++;
		}
		for (int i = 0; i < clientNum; i++) {
			send(clients[i], buffer, BUFFER_SIZE, 0);
		}
	}
	int last = allBytesCount % BUFFER_SIZE;
	for (int j = 0; j < last; ) {
		memcpy(buffer + j, &(*mat)(numCount / cols, numCount % cols), DBL_LEN);
		j += DBL_LEN;
		numCount++;
	}
	for (int i = 0; i < clientNum; i++) {
		send(clients[i], buffer, last, 0);
	}
}

MatrixXd ** ComputeCenter::blockRecvAllMatrix(int singleRow, int singleCol) {
	MatrixXd** mat = new MatrixXd*[clientNum];
	int allBytesCount = singleRow*singleCol*DBL_LEN;

	char *buffer = new char[BUFFER_SIZE];
	int rows;
	int cols;
	for (int i = 0; i < clientNum; i++) {
		myBlockedRecv(clients[i], buffer, INT_LEN * 2, 0);
		rows = getInteger(buffer);
		cols = getInteger(buffer + INT_LEN);
		if (rows != singleRow || cols != singleCol) return nullptr;

		mat[i] = new MatrixXd(singleRow, singleCol);
	}
	
	int* numCount = new int[clientNum] { 0 };
	for (int k = 0; k < (allBytesCount / BUFFER_SIZE); k++) {
		for (int i = 0; i < clientNum; i++) {
			myBlockedRecv(clients[i], buffer, BUFFER_SIZE, 0);
			for (int j = 0; j < BUFFER_SIZE; ) {
				(*mat[i])(numCount[i] / cols, numCount[i] % cols) = getDouble(buffer + j);
				j += DBL_LEN;
				numCount[i]++;
			}
		}
	}

	for (int i = 0; i < clientNum; i++) {
		int last = allBytesCount % BUFFER_SIZE;
		myBlockedRecv(clients[i], buffer, last, 0);
		for (int j = 0; j < last; ) {
			(*mat[i])(numCount[i] / cols, numCount[i] % cols) = getDouble(buffer + j);
			j += DBL_LEN;
			numCount[i]++;
		}
	}

	return mat;

}

void ComputeCenter::blockedSendStepRowsAndCols(int row, int col, SOCKET * client) {
	char *buffer = new char[BUFFER_SIZE];

	memcpy(buffer, &row, INT_LEN);
	memcpy(buffer + INT_LEN, &col, INT_LEN);
	send(*client, buffer, INT_LEN * 2, 0);
}

void ComputeCenter::blockedSendStepRow(MatrixXd * mat, SOCKET * client) {
	char *buffer = new char[BUFFER_SIZE];
	int numCount = 0;
	int rows = mat->rows();
	int cols = mat->cols();

	int allBytesCount = rows*cols*DBL_LEN;
	for (int i = 0; i < (allBytesCount / BUFFER_SIZE); i++) {
		for (int j = 0; j < BUFFER_SIZE; ) {
			memcpy(buffer + j, &(*mat)(numCount / cols, numCount % cols), DBL_LEN);
			j += DBL_LEN;
			numCount++;
		}
		send(*client, buffer, BUFFER_SIZE, 0);
	}
	int last = allBytesCount % BUFFER_SIZE;
	for (int j = 0; j < last; ) {
		memcpy(buffer + j, &(*mat)(numCount / cols, numCount % cols), DBL_LEN);
		j += DBL_LEN;
		numCount++;
	}
	send(*client, buffer, last, 0);
}
