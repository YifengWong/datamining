#pragma once
#include <cstring>
#include <Eigen\Dense>
using Eigen::MatrixXd;

#define DBL_LEN 8
#define INT_LEN 4

int static getInteger(char* bytes) {
	int num;
	memcpy(&num, bytes, INT_LEN);
	return num;
}

double static getDouble(char* bytes) {
	double num;
	memcpy(&num, bytes, DBL_LEN);
	return num;
}
