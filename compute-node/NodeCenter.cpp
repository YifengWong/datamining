#include "NodeCenter.h"
#include <iostream>

using std::cout;
using std::endl;


NodeCenter::NodeCenter(int port, int clientNum) {
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(port);
	addrServ.sin_addr.s_addr = INADDR_ANY;
	this->clientNum = clientNum;
	clients = new SOCKET[clientNum];
}


NodeCenter::~NodeCenter() {
	closesocket(server);  //�ر��׽���
	for (int i = 0; i < clientNum; i++) {
		closesocket(clients[i]);  //�ر��׽���  
	}
	WSACleanup();   //�ͷ��׽�����Դ;
}

int NodeCenter::start() {
	//��ʼ���׽��ֶ�̬��  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		cout << "WSAStartup failed!" << endl;
		return 1;
	}

	//�����׽���  
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server) {
		cout << "socket failed!" << endl;
		WSACleanup();//�ͷ��׽�����Դ;  
		return -1;
	}
	//���׽���  
	int retVal = bind(server, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal) {
		cout << "bind failed!" << endl;
		closesocket(server); //�ر��׽���  
		WSACleanup();  //�ͷ��׽�����Դ;  
		return -1;
	}
	//��ʼ����
	retVal = listen(server, 1);
	if (SOCKET_ERROR == retVal) {
		cout << "listen failed!" << endl;
		closesocket(server); //�ر��׽���  
		WSACleanup();  //�ͷ��׽�����Դ;  
		return -1;
	}

	//���ܿͻ�������
	for (int i = 0; i < clientNum; i++) {
		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);
		clients[i] = accept(server, (sockaddr FAR*)&addrClient, &addrClientlen);
		if (INVALID_SOCKET == clients[i]) {
			cout << "accept failed!" << endl;
			closesocket(server); //�ر��׽���  
			WSACleanup();  //�ͷ��׽�����Դ;  
			return -1;
		}
	}
}

void NodeCenter::blockBroadcastMatrix(MatrixXd * mat) {
	send(sClient, sendBuf, strlen(sendBuf), 0);
}

MatrixXd ** NodeCenter::blockRecvAllMatrix(int* count) {
	MatrixXd** mat = new MatrixXd*[10];

	int nNetTimeout = 100000; //100��
	setsockopt(clients[0], SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));

	recv(sClient, buf, BUF_SIZE, 0);
	return nullptr;
}
