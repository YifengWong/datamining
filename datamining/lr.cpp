#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <Eigen\Dense>


using namespace std;
using namespace Eigen;

#define DOUBLE_MIN_POSITIVE 1e-15

#define DATA_DIM 202
//#define DATA_DIM 5
#define LABEL_POS 202
//#define LABEL_POS 5
#define TRAIN_NUM 1866819
#define TEST_NUM  282796
#define RANGE_ONCE 10000

//#define TRAIN_FILE_PATH "E:\\DM_Data\\large-scale\\train_data.txt"
////#define TRAIN_FILE_PATH "E:\\DM_Data\\large-scale\\my_train.txt"
//#define THETA_FILE_PATH "G:\\Year3\\Year3-2\\DM\\homework02\\theta_V0.txt"
//#define TEST_FILE_PATH "E:\\DM_Data\\large-scale\\my_test_data.txt"
//#define RESULT_FILE_PATH "G:\\Year3\\Year3-2\\DM\\homework02\\result.txt"
//#define TRAIN_MEAN_FILE_PATH "G:\\Year3\\Year3-2\\DM\\homework02\\mean.txt"
//#define TRAIN_SD_FILE_PATH "G:\\Year3\\Year3-2\\DM\\homework02\\sd.txt"

#define TRAIN_FILE_PATH "E:\\data\\train_data.txt"
#define THETA_FILE_PATH "E:\\data\\theta_V0.txt"
#define TEST_FILE_PATH "E:\\data\\test_data.txt"
#define RESULT_FILE_PATH "E:\\data\\result.txt"
#define TRAIN_MEAN_FILE_PATH "E:\\data\\mean.txt"
#define TRAIN_SD_FILE_PATH "E:\\data\\sd.txt"

//double hfunc(int dim, double* theta, double* xi) {
//  double dot = 0;
//  for (int i = 0; i < dim; i++) {
//      dot += (theta[i] * xi[i]);
//  }
//  double s = exp(-dot);
//  double re = 1 / (1 + s);
//
//  if (re == 0) re = DOUBLE_MIN_POSITIVE;
//  else if (re == 1) re = 1 - DOUBLE_MIN_POSITIVE;
//  return re;
//}
//
//vector<double*>* readTrainFile(ifstream* file, int maxRow) {
//  string tempStr;
//  stringstream ss_stream;
//  vector<double*>* vec = new vector<double*>();
//  
//  for (int i = 0; i < maxRow && !file->eof(); i++) {
//      tempStr.clear();
//      ss_stream.clear();
//
//      double* line = new double[DATA_DIM+1] {0};
//      getline(*file, tempStr, '\n');
//      for (unsigned int i = 0; i < tempStr.length(); i++) if (tempStr[i] == ':') tempStr[i] = ' ';
//      
//      ss_stream << tempStr;
//      ss_stream >> line[LABEL_POS];
//      line[0] = 1;
//      while (!ss_stream.eof()) {
//          int pos;
//          double value;
//          ss_stream >> pos >> value;
//          line[pos] = value;
//      }
//      vec->push_back(line);
//  }
//  return vec;
//}
//
//void iteration(vector<double*>* trainData, double* theta, int iterNum) {
//  double Jtheta = 0;
//  double alpha = 0.0001;
//  double* hThetaX = new double[trainData->size()];
//
//  for (int k = 0; k < iterNum; k++) {
//      Jtheta = 0;
//      for (int i = 0; i < trainData->size(); i++) {
//          hThetaX[i] = hfunc(DATA_DIM, theta, trainData->at(i));
//          Jtheta += (trainData->at(i)[LABEL_POS] * log(hThetaX[i]) +
//              ((1 - trainData->at(i)[LABEL_POS]) * log(1 - hThetaX[i])));
//      }
//      Jtheta = - (Jtheta / trainData->size());
//      printf("%.15lf\n", Jtheta);
//
//      for (int j = 0; j < DATA_DIM; j++) {
//          double sum = 0;
//          for (int i = 0; i < trainData->size(); i++) {
//              sum += (hThetaX[i] - trainData->at(i)[LABEL_POS]) * trainData->at(i)[j];
//          }
//          theta[j] -= alpha * sum;
//      }
//  }
//}

MatrixXd* trainData;
MatrixXd* trainLabels;


MatrixXd* hfuncMatrix(MatrixXd* theta, MatrixXd* xi) {
	//cout << theta->transpose() << endl;
	//cout << *xi << endl;
	MatrixXd* mat = new MatrixXd((-1) * (*xi) * (*theta));
	//cout << *mat << endl;
	for (int i = 0; i < mat->rows(); i++) {
		(*mat)(i, 0) = 1 / (1 + exp((*mat)(i, 0)));
		if ((*mat)(i, 0) == 0) (*mat)(i, 0) = DOUBLE_MIN_POSITIVE;
		else if ((*mat)(i, 0) == 1) (*mat)(i, 0) = 1 - DOUBLE_MIN_POSITIVE;
	}
	return mat;
}

double computeJThetaMatrix(MatrixXd* dataLabels, MatrixXd* htheta) {
	double Jtheta = 0;
	for (int i = 0; i < dataLabels->rows(); i++) {
		Jtheta += ((*dataLabels)(i, 0) * log((*htheta)(i, 0)) + (1 - (*dataLabels)(i, 0))*(log(1 - (*htheta)(i, 0))));
	}
	Jtheta /= (-dataLabels->rows());
	return Jtheta;
}

void dealTestAndWriteResultMatrix(ifstream* testFile, ofstream* resultFile, MatrixXd* theta, int testNum) {
	//cout << *theta << endl;
	string tempStr;
	stringstream ss_stream;

	while (testNum-- && !testFile->eof()) {
		MatrixXd line(1, DATA_DIM);
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
			line(0, pos) = value;
		}
		if ((line * (*theta))(0, 0) >= 0) (*resultFile) << 1 << endl;
		else (*resultFile) << 0 << endl;
	}
}


MatrixXd* readAndGetMean(ifstream* file, int dataNum) {
	MatrixXd* trainMean;
	trainMean = new MatrixXd(DATA_DIM, 1);
	trainMean->setZero();


	double label;
	int pos;
	double value;
	for (int i = 0; i < dataNum && !file->eof(); i++) {
		string tempStr;
		stringstream ss_stream;
		tempStr.clear();
		ss_stream.clear();

		getline(*file, tempStr, '\n');
		if (i % 100000 == 0) {
			cout << i << endl;
		}
		for (unsigned int k = 0; k < tempStr.length(); k++) if (tempStr[k] == ':') tempStr[k] = ' ';

		ss_stream << tempStr;
		ss_stream >> label;
		while (!ss_stream.eof()) {
			ss_stream >> pos >> value;
			(*trainMean)(pos, 0) += value;
		}
	}
	(*trainMean) /= dataNum;
	return trainMean;
}

MatrixXd* readAndGetSd(ifstream* file, int dataNum, MatrixXd* trainMean) {
	MatrixXd* trainSD;
	trainSD = new MatrixXd(DATA_DIM, 1);
	trainSD->setZero();

	for (int i = 0; i < dataNum && !file->eof(); i++) {
		if (i % 100000 == 0) cout << i << endl;
		string tempStr;
		stringstream ss_stream;
		tempStr.clear();
		ss_stream.clear();

		getline(*file, tempStr, '\n');
		for (unsigned int k = 0; k < tempStr.length(); k++) if (tempStr[k] == ':') tempStr[k] = ' ';

		ss_stream << tempStr;
		double label;
		ss_stream >> label;
		MatrixXd line(DATA_DIM, 1);
		line.setZero();
		while (!ss_stream.eof()) {
			int pos;
			double value;
			ss_stream >> pos >> value;
			line(pos, 0) = value;
			double m = (value - (*trainMean)(pos, 0));
		}
		line -= (*trainMean);

		for (int j = 0; j < DATA_DIM; j++) {
			(*trainSD)(j, 0) += line(j, 0) * line(j, 0);
		}
	}

	(*trainSD) /= dataNum;
	for (int i = 0; i < trainSD->rows(); i++) {
		(*trainSD)(i, 0) = sqrt((*trainSD)(i, 0));
	}
	return trainSD;
}

void readTrainFileMatrix(ifstream* file, int maxRow, MatrixXd* trainMean, MatrixXd* trainSD) {
	string tempStr;
	stringstream ss_stream;
	trainData = new MatrixXd(maxRow, DATA_DIM);
	trainData->setZero();
	trainLabels = new MatrixXd(maxRow, 1);

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

void iterationMatrix(MatrixXd* trains, MatrixXd* trainsY, MatrixXd* theta, int iterCount) {
	double alpha = 0.9;
	while (iterCount--) {
		MatrixXd* hthetaX = hfuncMatrix(theta, trains);
		printf("%.15lf\n", computeJThetaMatrix(trainsY, hthetaX));
		MatrixXd temp((*hthetaX) - (*trainsY));
		temp /= trains->rows();
		MatrixXd iter((trains->transpose()) * (temp));
		//MatrixXd comp(DATA_DIM, 1);
		//comp.setZero();
		//
		//MatrixXd* perturb = new MatrixXd(DATA_DIM, 1);
		//perturb->setZero();
		//for (int i = 0; i < DATA_DIM; i++) {
		//  //cout << (*theta)(i, 0) << endl;
		//  (*perturb)(i, 0) = 1e-4;
		//  MatrixXd* thetaA = new MatrixXd((*theta) + (*perturb));
		//  MatrixXd* thetaM = new MatrixXd((*theta) - (*perturb));

		//  MatrixXd* hthetaA = hfuncMatrix(thetaA, trains);
		//  MatrixXd* hthetaM = hfuncMatrix(thetaM, trains);
		//  
		//  double aJ = computeJThetaMatrix(trainsY, hthetaA);
		//  double mJ = computeJThetaMatrix(trainsY, hthetaM);
		//  
		//  comp(i, 0) = (aJ - mJ) / 2e-4;
		//  //cout << *trains << endl;
		//  //cout << ((*hthetaX) - (*trainsY)).transpose() << endl;
		//  printf("%d %.15lf %.15lf\n", i, iter(i, 0), comp(i, 0));
		//  delete hthetaM;
		//  delete hthetaA;
		//  delete thetaM;
		//  delete thetaA;
		//  (*perturb)(i, 0) = 0;
		//}
		//
		//printf("%.15lf\n", (iter - comp).norm() / (iter + comp).norm());
		(*theta) -= (alpha * iter);
		delete hthetaX;

		cout << endl;
	}
}

void runNormal(int trainNum, int iterCount, int testNum) {
	cout << "Read the mean and sd file" << endl;
	MatrixXd* trainMean = new MatrixXd(DATA_DIM, 1);
	MatrixXd* trainSd = new MatrixXd(DATA_DIM, 1);
	ifstream meanFile(TRAIN_MEAN_FILE_PATH, ios_base::in);
	ifstream sdFile(TRAIN_SD_FILE_PATH, ios_base::in);
	for (int i = 0; i < DATA_DIM; i++) {
		meanFile >> (*trainMean)(i, 0);
		sdFile >> (*trainSd)(i, 0);
	}

	cout << "Read from train file" << endl;
	ifstream trainFile(TRAIN_FILE_PATH, ios_base::in);
	readTrainFileMatrix(&trainFile, trainNum, trainMean, trainSd);
	trainFile.close();

	cout << "Begin iteration" << endl;
	MatrixXd* theta = new MatrixXd(DATA_DIM, 1);
	theta->setZero();
	iterationMatrix(trainData, trainLabels, theta, iterCount);

	cout << "Compute the test data and write" << endl;
	ifstream testFile(TEST_FILE_PATH, ios_base::in);
	ofstream resultFile(RESULT_FILE_PATH, ios_base::out);
	dealTestAndWriteResultMatrix(&testFile, &resultFile, theta, testNum);
	testFile.close();
	resultFile.close();
}

void iterationSGD(MatrixXd* trains, MatrixXd* trainsY, MatrixXd* theta, int iterNum);

void runSGD();

int main() {
	runNormal(10000, 1000, 0);

	system("pause");
	return 0;
}