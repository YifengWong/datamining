#pragma once
#include <fstream>

#include <Eigen\Dense>

using namespace std;
using namespace Eigen;

class LR {
public:
	LR(MatrixXd* trains, MatrixXd* trainsY, int dataDim);
	void setAlpha(double alpha);
	void setPrintIterVerify(bool flag);
	
	void runNormalIteration(int iterCount);
	void setTheta(MatrixXd* theta);
	MatrixXd* getTheta();
	void runSGD();
	double computeResult(MatrixXd* line);
	~LR();

private:
	MatrixXd* trains;
	MatrixXd* trainsY;
	MatrixXd* theta;
	MatrixXd* mean;
	MatrixXd* sd;
	int dataDim;
	double alpha;
	bool runFlag;
	bool printIterVerifyFlag;

	void printIterVerify(MatrixXd* iter);
};

