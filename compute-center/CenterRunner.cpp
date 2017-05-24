#include "ComputeCenter.h"
#include <iostream>

using namespace std;

int main() {
	ComputeCenter* nc = new ComputeCenter(8888, 1);
	nc->start();
	int row = 1;
	int col = 202;
	MatrixXd* mat = new MatrixXd(row, col);
	cout << *mat << endl;
	nc->blockSendMatrix(mat, nc->getClient(0));
	MatrixXd** mats = nc->blockRecvAllMatrix(row, col);
	cout << *(mats[0]) << endl;
	
	delete nc;
	system("pause");
}