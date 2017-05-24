#include "ComputeNode.h"
#include <iostream>

using namespace std;

int main() {

	ComputeNode* cn = new ComputeNode("127.0.0.1", 8888);
	cn->start();
	MatrixXd* m = cn->blockedRecvMatrixXd();
	cout << *m << endl;
	cn->blockedSendMatrixXd(m);
	delete cn;
	system("pause");
}