#include <stdlib.h>
#include <iostream>

#include "LR.h"
#include "LRFunction.h"

LR::LR(MatrixXd * trains, MatrixXd * trainsY, int dataDim) {
	this->trains = trains;
	this->trainsY = trainsY;
	this->theta = new MatrixXd(trains->cols(), 1);
	this->theta->setZero();
	this->mean = nullptr;
	this->sd = nullptr;
	this->dataDim = dataDim;
	alpha = 0;
	runFlag = false;
}


void LR::setAlpha(double alpha) {
	this->alpha = alpha;
}

void LR::setPrintIterVerify(bool flag) {
	this->printIterVerifyFlag = flag;
}

// TODO
void LR::runNormalIteration(int iterCount) {
	int count = 0;
	while (iterCount--) {

		// iter and temp
		MatrixXd* hthetaX = lrHFunc(theta, trains);
		printf("%d %.15lf\n", ++count, lrComputeJTheta(trainsY, hthetaX));
		MatrixXd temp((*hthetaX) - (*trainsY));
		temp /= trains->rows();
		MatrixXd iter((trains->transpose()) * (temp));// Add at here

		if (printIterVerifyFlag) printIterVerify(&iter);
		
		(*theta) -= (alpha * iter);
		delete hthetaX;
	}
}

void LR::setTheta(MatrixXd * theta) {
	if (this->theta != nullptr) {
		delete this->theta;
		this->theta = nullptr;
	}
	this->theta = theta;
}

MatrixXd * LR::getTheta() {
	return theta;
}


double LR::computeResult(MatrixXd * line) {
	return ((*line) * (*theta))(0, 0);
}

LR::~LR() {
	if (this->theta != nullptr) {
		delete this->theta;
	}
	this->theta = nullptr;
}

void LR::printIterVerify(MatrixXd * iter) {
	MatrixXd comp(dataDim, 1);
	comp.setZero();

	MatrixXd* perturb = new MatrixXd(dataDim, 1);
	perturb->setZero();
	for (int i = 0; i < dataDim; i++) {
		(*perturb)(i, 0) = 1e-4;
		MatrixXd* thetaA = new MatrixXd((*theta) + (*perturb));
		MatrixXd* thetaM = new MatrixXd((*theta) - (*perturb));

		MatrixXd* hthetaA = lrHFunc(thetaA, trains);
		MatrixXd* hthetaM = lrHFunc(thetaM, trains);

		double aJ = lrComputeJTheta(trainsY, hthetaA);
		double mJ = lrComputeJTheta(trainsY, hthetaM);

		comp(i, 0) = (aJ - mJ) / 2e-4;
		printf("%d %.15lf %.15lf\n", i, (*iter)(i, 0), comp(i, 0));
		delete hthetaM;
		delete hthetaA;
		delete thetaM;
		delete thetaA;
		(*perturb)(i, 0) = 0;
	}

	printf("norm diff: %.15lf\n", ((*iter) - comp).norm() / ((*iter) + comp).norm());
}
