#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>

using namespace std;

double sigmoid(double);

int main() {
	using namespace std::chrono;

	string line;
	ifstream inFS;
	string index_in, pclass_in, survived_in, sex_in, age_in;
	int numObservations = 0;

	const int MAX_LEN = 1500;
	vector<int> index(MAX_LEN);
	vector<int> pclass(MAX_LEN);
	vector<int> survived(MAX_LEN);
	vector<int> sex(MAX_LEN);
	vector<int> age(MAX_LEN);

	// Import dataset
	//------------------------------------------------------------------------------ 
	// open the file
	inFS.open("titanic_project.csv");
	if (!inFS.is_open()) {
		cout << "Could not open file titanic_project.csv." << endl;
		return 1;
	}

	cout << "The file is opened." << endl;

	// ignore the first line
	getline(inFS, line);

	cout << line << endl;

	// read the data and save it in vectors
	while (inFS.good()) {
		getline(inFS, index_in, ',');
		getline(inFS, pclass_in, ',');
		getline(inFS, survived_in, ',');
		getline(inFS, sex_in, ',');
		getline(inFS, age_in, '\n');

		index_in.erase(remove(index_in.begin(),index_in.end(),'"'),index_in.end());

		index.at(numObservations) = stoi(index_in);
		pclass.at(numObservations) = stoi(pclass_in);
		survived.at(numObservations) = stoi(survived_in);
		sex.at(numObservations) = stoi(sex_in);
		age.at(numObservations) = stoi(age_in);

		numObservations++;
	}

	cout << "The data has been read from the file." << endl;

	// resize the vectors
	index.resize(numObservations);
	pclass.resize(numObservations);
	survived.resize(numObservations);
	sex.resize(numObservations);
	age.resize(numObservations);

	// close the file
	inFS.close();

	cout << "The file is closed." << endl << endl;
	//------------------------------------------------------------------------------ 

	auto start = steady_clock::now();

	// Train
	//--------------------------------------------------------------------------------------------
	double learningRate = 0.001;
	double weights[2] = { 1.0, 1.0 };
	double dataMatrix[800][2];
	double dataTranspose[2][800];
	vector<double> probVector(800);
	vector<double> modelError(800);

	for (int i = 0; i < 800; i++) {
		dataMatrix[i][0] = 1.0;
		dataMatrix[i][1] = static_cast<double>(sex.at(i));
		dataTranspose[0][i] = learningRate * dataMatrix[i][0];
		dataTranspose[1][i] = learningRate * dataMatrix[i][1];
	}

	for (int i = 0; i < 500000; i++) {
		for (int j = 0; j < 800; j++) {
			probVector.at(j) = (dataMatrix[j][0] * weights[0]) + (dataMatrix[j][1] * weights[1]);
			probVector.at(j) = sigmoid(probVector.at(j));
			modelError.at(j) = survived.at(j) - probVector.at(j);
			weights[0] = weights[0] + (dataTranspose[0][j] * modelError.at(j));
			weights[1] = weights[1] + (dataTranspose[1][j] * modelError.at(j));
		}
	}

	cout << "Weights" << endl;
	cout << "w0: " << weights[0] << endl;
	cout << "w1: " << weights[1] << endl << endl;
	//--------------------------------------------------------------------------------------------

	auto end = steady_clock::now();

	// Predict
	//----------------------------------------------------------------------------------------
	double testMatrix[246][2];
	vector<double> predicted(246);
	vector<double> probabilities(246);
	vector<int> predictions(246);

	for (int i = 0; i < 246; i++) {
		testMatrix[i][0] = 1.0;
		testMatrix[i][1] = static_cast<double>(sex.at(i + 800));
	}

	for (int i = 0; i < 246; i++) {
		predicted.at(i) = (testMatrix[i][0] * weights[0]) + (testMatrix[i][1] * weights[1]);
		probabilities.at(i) = exp(predicted.at(i)) * sigmoid(-predicted.at(i));
		
		if (probabilities.at(i) < 0.5) {
			predictions.at(i) = 0;
		}
		else {
			predictions.at(i) = 1;
		}
	}

	int confusionMatrix[2][2] = { 
		{ 0, 0 },
		{ 0, 0 } 
	};

	for (int i = 0; i < 246; i++) {
		if (predictions.at(i) == survived.at(i + 800) && predictions.at(i) == 1) {
			confusionMatrix[0][0] = confusionMatrix[0][0] + 1;
		}
		else if (predictions.at(i) != survived.at(i + 800) && predictions.at(i) == 0) {
			confusionMatrix[0][1] = confusionMatrix[0][1] + 1;
		}
		else if (predictions.at(i) != survived.at(i + 800) && predictions.at(i) == 1) {
			confusionMatrix[1][0] = confusionMatrix[1][0] + 1;
		}
		else {
			confusionMatrix[1][1] = confusionMatrix[1][1] + 1;
		}
	}

	cout << "Confusion matrix" << endl;
	cout << "    pP  pN" << endl;
	cout << "aP  " << confusionMatrix[0][0] << "  " << confusionMatrix[0][1] << endl;
	cout << "aN  " << confusionMatrix[1][0] << "  " << confusionMatrix[1][1] << endl << endl;
	//----------------------------------------------------------------------------------------

	// Evaluation
	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	int runtime = duration_cast<chrono::milliseconds>(end - start).count();

	cout << "Training time: " << runtime << " milliseconds (" << static_cast<double>(runtime) / 1000.0 << " seconds)" << endl;
	cout << "Accuracy: " << static_cast<double>(confusionMatrix[0][0] + confusionMatrix[1][1]) / static_cast<double>(numObservations - 800) << endl;
	cout << "Sensitivity: " << static_cast<double>(confusionMatrix[0][0]) / static_cast<double>(confusionMatrix[0][0] + confusionMatrix[0][1]) << endl;
	cout << "Specificity: " << static_cast<double>(confusionMatrix[1][1]) / static_cast<double>(confusionMatrix[1][0] + confusionMatrix[1][1]) << endl << endl;
	//----------------------------------------------------------------------------------------------------------------------------------------------------------

	return 0;
}

double sigmoid(double z) {
	return 1 / (1 + exp(-z));
}