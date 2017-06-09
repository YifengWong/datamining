#include "ComputeCenter.h"
#include "../compute-node/NumParser.h"
#include <iostream>
#include <ctime>

#include "../datamining/LRFunction.h"

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

MatrixXd ** ComputeCenter::blockRecvAllMatrix() {
	clock_t start, stop;

	MatrixXd** mat = new MatrixXd*[clientNum];
	
	char *buffer = new char[BUFFER_SIZE];
	int rows;
	int cols;
	for (int i = 0; i < clientNum; i++) {
		myBlockedRecv(clients[i], buffer, INT_LEN * 2, 0);
		rows = getInteger(buffer);
		cols = getInteger(buffer + INT_LEN);

		mat[i] = new MatrixXd(rows, cols);
	}
	
	int allBytesCount = rows*cols*DBL_LEN;
	int* numCount = new int[clientNum] { 0 };
	for (int k = 0; k < (allBytesCount / BUFFER_SIZE); k++) {
		for (int i = 0; i < clientNum; i++) {
			myBlockedRecv(clients[i], buffer, BUFFER_SIZE, 0);
			if (k == 0 && i == 0) {
				start = clock();
			}
			
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
	stop = clock();
	printf("Use time %ld ms.\n", (stop - start));

	return mat;

}

void ComputeCenter::blockedSendStepRowsAndCols(int row, int col, SOCKET * client) {
	char *buffer = new char[BUFFER_SIZE];

	memcpy(buffer, &row, INT_LEN);
	memcpy(buffer + INT_LEN, &col, INT_LEN);
	send(*client, buffer, INT_LEN * 2, 0);
}

void ComputeCenter::blockedSendStepRow(double* nums, int colCount, SOCKET * client) {
	char *buffer = new char[BUFFER_SIZE];
	int numCount = 0;

	int allBytesCount = colCount*DBL_LEN;
	for (int i = 0; i < (allBytesCount / BUFFER_SIZE); i++) {
		for (int j = 0; j < BUFFER_SIZE; ) {
			memcpy(buffer + j, &(nums[numCount]), DBL_LEN);
			j += DBL_LEN;
			numCount++;
		}
		send(*client, buffer, BUFFER_SIZE, 0);
	}
	int last = allBytesCount % BUFFER_SIZE;
	for (int j = 0; j < last; ) {
		memcpy(buffer + j, &(nums[numCount]), DBL_LEN);
		j += DBL_LEN;
		numCount++;
	}
	send(*client, buffer, last, 0);
}

void ComputeCenter::setAllSampleRowsAndDim(int rows, int dim) {
	allSamplesRows = rows;
	dataDim = dim;

	int avgCount = rows / clientNum;
	int moreCount = rows % clientNum;
	clientTrainsCount = new int[clientNum];

	int i = 0;
	for (; i < moreCount; i++) {
		blockedSendStepRowsAndCols(avgCount+1, dim, getClient(i));
		clientTrainsCount[i] = avgCount + 1;
	}
	for (; i < clientNum; i++) {
		blockedSendStepRowsAndCols(avgCount, dim, getClient(i));
		clientTrainsCount[i] = avgCount;
	}
}

void ComputeCenter::readAndSendASampleRow(int no, double * sample, int cols) {
	blockedSendStepRow(sample, cols, getClient(no % clientNum));
}
// send sample
// loop :
// send theta
// Node: htheta ->send
// send temp
// Node: iter ->send
// iteration.

// iter and temp
MatrixXd * ComputeCenter::beginLRIterationAndGetTheta(double alpha, int iterCount, MatrixXd* trainsY) {
	int count = 0;
	MatrixXd * theta = new MatrixXd(dataDim, 1);
	theta->setZero();
	int trainsNum = trainsY->rows();

	for (int i = 0; i < clientNum; i++) {
		MatrixXd * labels = new MatrixXd(trainsY->block(0, 0, clientTrainsCount[i], 1));
		blockSendMatrix(labels, getClient(i));
	}

	while (iterCount--) {
		blockBroadcastMatrix(theta);
		MatrixXd** iter = blockRecvAllMatrix();
		MatrixXd newIter(dataDim, 1);
		newIter.setZero();
		for (int i = 0; i < clientNum; i++) {
			newIter += (*(iter[i]));
		}
		(*theta) -= (alpha * newIter / (double)trainsNum);
	}
	return theta;
}
