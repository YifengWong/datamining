#pragma once
#include <Eigen\Dense>

using Eigen::MatrixXd;

#define DOUBLE_MIN_POSITIVE 1e-15

static MatrixXd* lrHFunc(MatrixXd* theta, MatrixXd* xi) {
	MatrixXd* mat = new MatrixXd((-1) * (*xi) * (*theta));
	for (int i = 0; i < mat->rows(); i++) {
		(*mat)(i, 0) = 1 / (1 + exp((*mat)(i, 0)));
		if ((*mat)(i, 0) == 0) (*mat)(i, 0) = DOUBLE_MIN_POSITIVE;
		else if ((*mat)(i, 0) == 1) (*mat)(i, 0) = 1 - DOUBLE_MIN_POSITIVE;
	}
	return mat;
}

static double lrComputeJTheta(Eigen::MatrixXd* dataLabels, MatrixXd* htheta) {
	double Jtheta = 0;
	for (int i = 0; i < dataLabels->rows(); i++) {
		Jtheta += ((*dataLabels)(i, 0) * log((*htheta)(i, 0)) + (1 - (*dataLabels)(i, 0))*(log(1 - (*htheta)(i, 0))));
	}
	Jtheta /= (-dataLabels->rows());
	return Jtheta;
}
