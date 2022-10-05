#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>

using namespace std;

double calcAgeLH(int, double, double);

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

	// Prior probabilities
	//------------------------------------------------
	// perished, survived
	double apriori[2] = { 0.0, 0.0 };

	// Sort rows by survival
	for (int i = 0; i < 800; i++) {
		if (!survived.at(i)) {
			apriori[0] = apriori[0] + 1.0;
		}
		else {
			apriori[1] = apriori[1] + 1.0;
		}
	}

	apriori[0] = apriori[0] / 800;
	apriori[1] = apriori[1] / 800;

	cout << "Prior probabilities" << endl;
	cout << "perished  survived" << endl;
	cout << apriori[0] << "      " << apriori[1] << endl << endl;
	//------------------------------------------------

	int survivedCount[2] = { 0, 0 };

	// Sort rows by survival
	for (int i = 0; i < 800; i++) {
		if (!survived.at(i)) {
			survivedCount[0]++;
		}
		else {
			survivedCount[1]++;
		}
	}

	// Qualitative likelihoods
	//---------------------------------------------------------------------------------------------------
	double pclassLH[2][3] = { 
		{ 0.0, 0.0, 0.0 }, 
		{ 0.0, 0.0, 0.0 } 
	};

	double sexLH[2][2] = { 
		{ 0.0, 0.0 }, 
		{ 0.0, 0.0 } 
	};

	// Sort rows by pclass and survival
	for (int i = 0; i < 800; i++) {
		if (pclass.at(i) == 1 && !survived.at(i)) {
			pclassLH[0][0] = pclassLH[0][0] + 1.0;
		}
		if (pclass.at(i) == 2 && !survived.at(i)) {
			pclassLH[0][1] = pclassLH[0][1] + 1.0;
		}
		if (pclass.at(i) == 3 && !survived.at(i)) {
			pclassLH[0][2] = pclassLH[0][2] + 1.0;
		}
		if (pclass.at(i) == 1 && survived.at(i)) {
			pclassLH[1][0] = pclassLH[1][0] + 1.0;
		}
		if (pclass.at(i) == 2 && survived.at(i)) {
			pclassLH[1][1] = pclassLH[1][1] + 1.0;
		}
		if (pclass.at(i) == 3 && survived.at(i)) {
			pclassLH[1][2] = pclassLH[1][2] + 1.0;
		}
	}

	// Divide by survivals for pclass likelihoods
	for (int i = 0; i < 3; i++) {
		pclassLH[0][i] = pclassLH[0][i] / survivedCount[0];
		pclassLH[1][i] = pclassLH[1][i] / survivedCount[1];
	}

	cout << "Pclass likelihoods" << endl;
	cout << "\t1st       2nd       3rd" << endl;
	cout << "0  " << pclassLH[0][0] << "  " << pclassLH[0][1] << "  " << pclassLH[0][2] << endl;
	cout << "1  " << pclassLH[1][0] << "  " << pclassLH[1][1] << "  " << pclassLH[1][2] << endl << endl;
	
	// Sort rows by sex and survival
	for (int i = 0; i < 800; i++) {
		if (sex.at(i) == 0 && !survived.at(i)) {
			sexLH[0][0] = sexLH[0][0] + 1.0;
		}
		if (sex.at(i) == 1 && !survived.at(i)) {
			sexLH[0][1] = sexLH[0][1] + 1.0;
		}
		if (sex.at(i) == 0 && survived.at(i)) {
			sexLH[1][0] = sexLH[1][0] + 1.0;
		}
		if (sex.at(i) == 1 && survived.at(i)) {
			sexLH[1][1] = sexLH[1][1] + 1.0;
		}
	}

	// Divide by survivals for sex likelihoods
	for (int i = 0; i < 2; i++) {
		sexLH[0][i] = sexLH[0][i] / survivedCount[0];
		sexLH[1][i] = sexLH[1][i] / survivedCount[1];
	}

	cout << "Sex likelihoods" << endl;
	cout << "     female      male" << endl;
	cout << "0  " << sexLH[0][0] << "  " << sexLH[0][1] << endl;
	cout << "1  " << sexLH[1][0] << "  " << sexLH[1][1] << endl << endl;
	//---------------------------------------------------------------------------------------------------

	// Prep for age likelihoods
	//-----------------------------------------------------------------------------------
	double ageMean[2] = { 0.0, 0.0 };
	double ageVar[2] = { 0.0, 0.0 };

	// Sort rows by survival and acculumate
	for (int i = 0; i < 800; i++) {
		if (!survived.at(i)) {
			ageMean[0] = ageMean[0] + age.at(i);
		}
		else {
			ageMean[1] = ageMean[1] + age.at(i);
		}
	}

	// Divide by survivals for means
	ageMean[0] = ageMean[0] / survivedCount[0];
	ageMean[1] = ageMean[1] / survivedCount[1];

	
	// Sort rows by survival and caluclate acculumated variance
	for (int i = 0; i < 800; i++) {
		if (!survived.at(i)) {
			ageVar[0] = ageVar[0] + ((ageMean[0] - age.at(i)) * (ageMean[0] - age.at(i)));
		}
		else {
			ageVar[1] = ageVar[1] + ((ageMean[1] - age.at(i)) * (ageMean[1] - age.at(i)));
		}
	}

	// Divide for variances
	ageVar[0] = ageVar[0] / survivedCount[0];
	ageVar[1] = ageVar[1] / survivedCount[1];

	cout << "Age means and variances" << endl;
	cout << "      Mean   Variance" << endl;
	cout << "0  " << ageMean[0] << "  " << ageVar[0] << endl;
	cout << "1  " << ageMean[1] << "  " << ageVar[1] << endl << endl;
	//-----------------------------------------------------------------------------------

	auto end = steady_clock::now();

	// Predict
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	double rawProb[246][2];
	
	for (int i = 0; i < 246; i++) {
		int p = pclass.at(i + 800);
		int s = sex.at(i + 800);
		int a = age.at(i + 800);

		double numS = pclassLH[1][p-1] + sexLH[1][s] + apriori[1] + calcAgeLH(a, ageMean[1], ageVar[1]);
		double numP = pclassLH[0][p-1] + sexLH[0][s] + apriori[0] + calcAgeLH(a, ageMean[0], ageVar[0]);
		double denom = pclassLH[1][p - 1] + sexLH[1][s] + apriori[1] + calcAgeLH(a, ageMean[1], ageVar[1]) + pclassLH[0][p - 1] + sexLH[0][s] + apriori[0] + calcAgeLH(a, ageMean[0], ageVar[0]);
	
		rawProb[i][0] = numP / denom;
		rawProb[i][1] = numS / denom;
	}

	cout << "Raw probabilities [1 -> 5]" << endl;
	cout << "     perished  survived" << endl;
	for (int i = 0; i < 5; i++) {
		cout << "[" << i + 1 << "]" << "  " << rawProb[i][0] << "  " << rawProb[i][1] << endl;
	}
	cout << endl;

	int classify[246];

	for (int i = 0; i < 246; i++) {
		if (rawProb[i][0] > rawProb[i][1]) {
			classify[i] = 0;
		}
		else {
			classify[i] = 1;
		}
	}

	int confusionMatrix[2][2] = { 
		{ 0, 0 },
		{ 0, 0 } 
	};

	for (int i = 0; i < 246; i++) {
		if (classify[i] == survived.at(i + 800) && classify[i] == 1) {
			confusionMatrix[0][0] = confusionMatrix[0][0] + 1;
		}
		else if (classify[i] != survived.at(i + 800) && classify[i] == 0) {
			confusionMatrix[0][1] = confusionMatrix[0][1] + 1;
		}
		else if (classify[i] != survived.at(i + 800) && classify[i] == 1) {
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
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	// Evaluation
	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	int runtime = duration_cast<chrono::microseconds>(end - start).count();

	cout << "Training time: " << runtime << " microseconds (" << static_cast<double>(runtime) / 1000.0 << " milliseconds)" << endl;
	cout << "Accuracy: " << static_cast<double>(confusionMatrix[0][0] + confusionMatrix[1][1]) / static_cast<double>(numObservations - 800) << endl;
	cout << "Sensitivity: " << static_cast<double>(confusionMatrix[0][0]) / static_cast<double>(confusionMatrix[0][0] + confusionMatrix[0][1]) << endl;
	cout << "Specificity: " << static_cast<double>(confusionMatrix[1][1]) / static_cast<double>(confusionMatrix[1][0] + confusionMatrix[1][1]) << endl << endl;
	//----------------------------------------------------------------------------------------------------------------------------------------------------------

	return 0;
}

double calcAgeLH(int age, double ageMean, double ageVar) {
	double pi = 2 * acos(0.0);
	return 1 / sqrt(2 * pi * ageVar) * exp(-((age - ageMean) * (age - ageMean)) / (2 * ageVar));
}