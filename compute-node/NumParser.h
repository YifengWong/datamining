#pragma once
#include <cstring>
#include <Eigen\Dense>
using Eigen::MatrixXd;

typedef char byte;
#define DOUBLE_LENGTH 8
#define INTEGER_LENGTH 4

int getInteger(byte* bytes, int from) {
	int num;
	memcpy(&num, bytes + from, INTEGER_LENGTH);
	return num;
}

double getDouble(byte* bytes, int from) {
	double num;
	memcpy(&num, bytes + from, DOUBLE_LENGTH);
	return num;
}

byte* writeDouble_8Bytes(double num, int *getLength) {
	byte* bytes = new byte[DOUBLE_LENGTH];
	memcpy(bytes, &num, DOUBLE_LENGTH);
	*getLength = DOUBLE_LENGTH;
	return bytes;
}

byte* writeInteger_4Bytes(int num, int *getLength) {
	byte* bytes = new byte[INTEGER_LENGTH];
	memcpy(bytes, &num, INTEGER_LENGTH);
	*getLength = INTEGER_LENGTH;
	return bytes;
}

// count+nums
byte* writeMatrixRow(MatrixXd* row, int *getLength) {
	int count = row->cols();
	*getLength = INTEGER_LENGTH + count * DOUBLE_LENGTH;
	byte* bytes = new byte[*getLength];

	int offset = 0;
	memcpy(bytes + offset, &count, INTEGER_LENGTH);
	offset += INTEGER_LENGTH;
	for (int i = 0; i < count; i++) {
		memcpy(bytes + offset, &((*row)(0, i)), DOUBLE_LENGTH);
		offset += DOUBLE_LENGTH;
	}

	return bytes;
}
