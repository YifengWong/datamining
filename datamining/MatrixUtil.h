#pragma once
#include <fstream>
#include <Eigen\Dense>

using namespace std;
using namespace Eigen;

MatrixXd* computeMean(MatrixXd* data) {
	MatrixXd* mean = new MatrixXd(1, data->cols());
	mean->setZero();
	for (int i = 0; i < data->rows(); i++) {
		(*mean) += data->row(i);
	}
	(*mean) /= data->rows();
	return mean;
}

MatrixXd* computeStandardDeviation(MatrixXd* data, MatrixXd* mean) {
	MatrixXd* sd = new MatrixXd(1, data->cols());
	sd->setZero();
	for (int j = 0; j < data->cols(); j++) {
		for (int i = 0; i < data->rows(); i++) {
			double value = (*data)(i, j) - (*mean)(0, j);
			(*sd)(0, j) += value*value;
		}
		(*sd)(0, j) = sqrt((*sd)(0, j) / data->rows());
	}
	return sd;
}

MatrixXd* readFromMatrixFile(const char* filePath, int rows, int cols) {
	MatrixXd* data = new MatrixXd(rows, cols);
	ifstream matrixFile(filePath, ios_base::in);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrixFile >> (*data)(i, j);
		}
	}
	matrixFile.close();
	return data;
}

void writeMatrixToFile(const char* filePath, MatrixXd* data) {
	ofstream matrixFile(filePath, ios_base::out);
	matrixFile << (*data) << endl;
	matrixFile.close();
}
