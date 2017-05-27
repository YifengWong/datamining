#include "ComputeNode.h"
#include <iostream>

#include "../datamining/LRFunction.h"

using namespace std;

int main() {

	ComputeNode* cn = new ComputeNode("127.0.0.1", 8888);
	cn->start();
	MatrixXd* trains = cn->blockedRecvMatrixXd();
	MatrixXd* trainsY = cn->blockedRecvMatrixXd();
	//cout << *m << endl;
	int count = 0;
	while (TRUE) {
		MatrixXd* theta = cn->blockedRecvMatrixXd();
		// TODO
		MatrixXd* hthetaX = lrHFunc(theta, trains);
		printf("%d %.15lf\n", ++count, lrComputeJTheta(trainsY, hthetaX));

		MatrixXd temp((*hthetaX) - (*trainsY));
		MatrixXd* iter = new MatrixXd((trains->transpose()) * (temp));
		cn->blockedSendMatrixXd(iter);
	}

	system("pause");
}