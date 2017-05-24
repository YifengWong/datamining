#include "ComputeCenter.h"
#include <iostream>

using namespace std;



int main() {
	ComputeCenter* nc = new ComputeCenter(8888, 2);
	nc->start();
	int row = 5;
	int col = 5;
	MatrixXd* mat = new MatrixXd(row, col);

	cout << *mat << endl;
	//nc->blockBroadcastMatrix(mat);
	nc->blockedSendStepRowsAndCols(row * 2, col, nc->getClient(0));
	nc->blockedSendStepRow(mat, nc->getClient(0));
	nc->blockedSendStepRow(mat, nc->getClient(0));

	nc->blockedSendStepRowsAndCols(row * 2, col, nc->getClient(1));
	nc->blockedSendStepRow(mat, nc->getClient(1));
	nc->blockedSendStepRow(mat, nc->getClient(1));

	MatrixXd** mats = nc->blockRecvAllMatrix(row*2, col);
	cout << *(mats[0]) << endl;
	
	delete nc;
	system("pause");
}