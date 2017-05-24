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

//char* writeDouble_8Bytes(double num, int *getLength) {
//	char* bytes = new char[DBL_LEN];
//	memcpy(bytes, &num, DBL_LEN);
//	*getLength = DBL_LEN;
//	return bytes;
//}
//
//char* writeInteger_4Bytes(int num, int *getLength) {
//	char* bytes = new char[INT_LEN];
//	memcpy(bytes, &num, INT_LEN);
//	*getLength = INT_LEN;
//	return bytes;
//}
//
//// count+nums
//char* writeMatrixRow(MatrixXd* row, int *getLength) {
//	int count = row->cols();
//	*getLength = INT_LEN + count * DBL_LEN;
//	char* bytes = new char[*getLength];
//
//	int offset = 0;
//	memcpy(bytes + offset, &count, INT_LEN);
//	offset += INT_LEN;
//	for (int i = 0; i < count; i++) {
//		memcpy(bytes + offset, &((*row)(0, i)), DBL_LEN);
//		offset += DBL_LEN;
//	}
//
//	return bytes;
//}
