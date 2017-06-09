#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

#include <Eigen\Dense>

#include "LR.h"
#include "LRFunction.h"
#include "MatrixUtil.h"


using namespace std;
using namespace Eigen;

#define EIGEN_DONT_PARALLELIZE 

#define DATA_DIM 202
//#define DATA_DIM 5
#define LABEL_POS 202
//#define LABEL_POS 5
#define TRAIN_NUM 1866819
#define TEST_NUM  282796
#define RANGE_ONCE 10000

#define TRAIN_FILE_PATH "E:\\DM_Data\\large-scale\\train_data.txt"
#define THETA_FILE_PATH "G:\\Year3\\Year3-2\\DM\\homework02\\theta_V0.txt"
#define TEST_FILE_PATH "E:\\DM_Data\\large-scale\\my_test_data.txt"
#define RESULT_FILE_PATH "G:\\Year3\\Year3-2\\DM\\homework02\\result.txt"
#define TRAIN_MEAN_FILE_PATH "G:\\Year3\\Year3-2\\DM\\homework02\\mean.txt"
#define TRAIN_SD_FILE_PATH "G:\\Year3\\Year3-2\\DM\\homework02\\sd.txt"

//#define TRAIN_FILE_PATH "E:\\data\\train_data.txt"
//#define THETA_FILE_PATH "E:\\data\\theta_V0.txt"
//#define TEST_FILE_PATH "E:\\data\\my_test_data.txt"
//#define RESULT_FILE_PATH "E:\\data\\result.csv"
//#define TRAIN_MEAN_FILE_PATH "E:\\data\\mean.txt"
//#define TRAIN_SD_FILE_PATH "E:\\data\\sd.txt"



void dealTestAndWriteResultMatrix(ifstream* testFile, ofstream* resultFile, MatrixXd* theta, int testNum, MatrixXd* trainMean, MatrixXd* trainSD) {
	string tempStr;
	stringstream ss_stream;
	(*resultFile) << "id,label" << endl;
	while (testNum-- && !testFile->eof()) {
		MatrixXd line(1, DATA_DIM);
		line.setZero();
		tempStr.clear();
		ss_stream.clear();
		getline(*testFile, tempStr, '\n');

		for (unsigned int i = 0; i < tempStr.length(); i++) if (tempStr[i] == ':') tempStr[i] = ' ';
		ss_stream << tempStr;
		string id;
		ss_stream >> id;
		(*resultFile) << id << ",";
		line(0, 0) = 1;

		while (!ss_stream.eof()) {
			int pos;
			double value;
			ss_stream >> pos >> value;
			line(0, pos) = (value - (*trainMean)(pos, 0)) / (*trainSD)(pos, 0);
		}
		(*resultFile) << (line * (*theta))(0, 0) << endl;
	}
}

void readTrainFileMatrix(MatrixXd* trainData, MatrixXd* trainLabels, ifstream* file, int maxRow, MatrixXd* trainMean, MatrixXd* trainSD) {
	string tempStr;
	stringstream ss_stream;

	int i = 0;
	for (i = 0; i < maxRow && !file->eof(); i++) {
		tempStr.clear();
		ss_stream.clear();

		getline(*file, tempStr, '\n');
		for (unsigned int k = 0; k < tempStr.length(); k++) if (tempStr[k] == ':') tempStr[k] = ' ';

		ss_stream << tempStr;
		ss_stream >> (*trainLabels)(i, 0);
		(*trainData)(i, 0) = 1;
		while (!ss_stream.eof()) {
			int pos;
			double value;
			ss_stream >> pos >> value;
			(*trainData)(i, pos) = (value - (*trainMean)(pos, 0)) / (*trainSD)(pos, 0);
		}
	}
	if (i != maxRow) {
		trainData->conservativeResize(i, NoChange);
		trainLabels->conservativeResize(i, NoChange);
	}
}

void startDist() {
	// read trains
	// new LR,center,node

}

int main() {

	int trainNum = 500000;
	double alpha = 1.0;
	bool printIterVerifyFlag = false;
	int iterCount = 10;
	int testNum = TEST_NUM;
	int loopCount = 1;

	MatrixXd* trainMean = new MatrixXd(DATA_DIM, 1);
	trainMean->setZero();
	MatrixXd* trainSd = new MatrixXd(DATA_DIM, 1);
	trainSd->setOnes();

	cout << "Read the mean and sd file" << endl;
	ifstream meanFile(TRAIN_MEAN_FILE_PATH, ios_base::in);
	ifstream sdFile(TRAIN_SD_FILE_PATH, ios_base::in);
	for (int i = 0; i < DATA_DIM; i++) {
		meanFile >> (*trainMean)(i, 0);
		sdFile >> (*trainSd)(i, 0);
	}

	cout << "Read from train file" << endl;
	MatrixXd* trainData = new MatrixXd(trainNum, DATA_DIM);
	MatrixXd* trainLabels = new MatrixXd(trainNum, 1);
	trainData->setZero();
	ifstream trainFile(TRAIN_FILE_PATH, ios_base::in);
	readTrainFileMatrix(trainData, trainLabels, &trainFile, trainNum, trainMean, trainSd);
	trainFile.close();

	//cout << "Read from theta file" << endl;
	//MatrixXd* theta = readFromMatrixFile(THETA_FILE_PATH, DATA_DIM, 1);

	LR* lr = new LR(trainData, trainLabels, DATA_DIM);

	// more
	string path = "G:\\Year3\\Year3-2\\DM\\homework02\\";
	string thetaFilePath = "_theta.txt";
	string resultFilePath = "_result.txt";
	string num_str;
	stringstream ss;
	int count = 0;
	while (count < loopCount) {
		num_str.clear();
		ss.clear();
		ss << count;
		ss >> num_str;
		if (++count % 2 == 0) alpha *= 0.75;
		lr->setAlpha(alpha);
		//lr->setTheta(theta);
		lr->setPrintIterVerify(printIterVerifyFlag);
		clock_t start, stop;
		start = clock();
		lr->runNormalIteration(iterCount);
		stop = clock();
		printf("Use time %ld ms.\n", (stop - start));

		//cout << "Write theta to file." << endl;
		//writeMatrixToFile((path+num_str+thetaFilePath).c_str(), lr->getTheta());

		//cout << "Compute the test data and write" << endl;
		//ifstream testFile(TEST_FILE_PATH, ios_base::in);
		//ofstream resultFile((path + num_str + resultFilePath).c_str(), ios_base::out);
		//dealTestAndWriteResultMatrix(&testFile, &resultFile, lr->getTheta(), testNum, trainMean, trainSd);
		//testFile.close();
		//resultFile.close();
	}
	

	system("pause");
	return 0;
}