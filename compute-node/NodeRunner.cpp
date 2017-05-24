#include "ComputeNode.h"
#include <iostream>

#include "../datamining/LRFunction.h"

using namespace std;

int main() {

	ComputeNode* cn = new ComputeNode("127.0.0.1", 8888);
	cn->start();
	MatrixXd* samples = cn->blockedRecvMatrixXd();
	//cout << *m << endl;
	while (TRUE) {
		MatrixXd* theta = cn->blockedRecvMatrixXd();
		// TODO
	}
	cn->blockedSendMatrixXd(samples);
	delete cn;
	system("pause");
}