#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
using namespace std;

int read_csv(vector<double>&, vector<double>&);
void print_stats(vector<double>, int, double[], int);
double sum(vector<double>, int, double[], int);
double mean(vector<double>, int, double[], int);
double median(vector<double>, int);
void range(vector<double>, int, double []);
double covar(vector<double>, vector<double>, int, double[]);
double cor(vector<double>, vector<double>, int, double, double, double);

int main(int argc, char** argv) {
	const int MAX_LEN = 1000;
	vector<double> rm(MAX_LEN);
	vector<double> medv(MAX_LEN);	
	int numObservations = read_csv(rm, medv); // read file into vectors

	// exit on error to read file
	if (numObservations == -1) return 1;

	// stored calculations for repeat references
	double calc[5];

	// print stats
	cout << "Number of records: " << numObservations << endl;
	cout << "\nStats for rm" << endl;
	print_stats(rm, numObservations, calc, 0);
	cout << "\nStats for medv" << endl;
	print_stats(medv, numObservations, calc, 1);
	cout << "\n Covariance = " << covar(rm, medv, numObservations, calc) << endl;
	cout << "\n Correlation = " << cor(rm, medv, numObservations, calc[2], calc[3], calc[4]) << endl;

	cout << "\nProgram terminated." << endl;
	return 0;
}

int read_csv(vector<double>& rm, vector<double>& medv) {
	ifstream inFS;
	string line;
	string rm_in, medv_in;

	// open file
	cout << "Opening file Boston.csv." << endl;
	inFS.open("Boston.csv");
	if (!inFS.is_open()) {
		cout << "Could not open file Boston.csv." << endl;
		return -1;
	}

	// read and echo heading
	cout << "Reading line 1" << endl;
	getline(inFS, line);
	cout << "heading: " << line << endl;

	// fill vectors
	int numObservations = 0;
	while (inFS.good()) {
		getline(inFS, rm_in, ',');
		getline(inFS, medv_in, '\n');

		rm.at(numObservations) = stof(rm_in);
		medv.at(numObservations) = stof(medv_in);

		numObservations++;
	}

	// resize vectors
	rm.resize(numObservations);
	medv.resize(numObservations);
	cout << "new length " << rm.size() << endl;

	// close file
	cout << "Closing file Boston.csv." << endl;
	inFS.close();

	return numObservations;
}

void print_stats(vector<double> stat, int n, double calc[], int col) {
	// sort vector
	sort(stat.begin(), stat.end());
	
	// find min and max of range
	double min_max[2];
	range(stat, n, min_max);

	// print stats based on vector
	cout << " Sum = " << sum(stat, n, calc, col) << endl;
	cout << " Mean = " << mean(stat, n, calc, col) << endl;
	cout << " Median = " << median(stat, n) << endl;
	cout << " Range = " << "[" << min_max[0] << ", " << min_max[1] << "]" << endl;
}

double sum(vector<double> stat, int n, double calc[], int col) {
	double sum = 0.0;
	for (int i = 0; i < n; i++) {
		sum += stat[i];
	}

	// save calculation
	calc[col] = sum; 
	return sum;
}

double mean(vector<double> stat, int n, double calc[], int col) {
	// save calculation
	calc[col + 2] = calc[col] / n;
	return calc[col + 2];
}

double median(vector<double> stat, int n) {
	// even number of observations
	if (n % 2 == 0) {
		return (stat[(n / 2) - 1] + stat[n / 2]) / 2;
	}
	// odd number of observations
	else {
		return stat[n / 2];
	}
}

void range(vector<double> stat, int n, double min_max[]) {
	min_max[0] = stat[0];
	min_max[1] = stat[n - 1];
}

double covar(vector<double> rm, vector<double> medv, int n, double calc[]) {
	// expected value per vector
	double rm_ev = calc[2];
	double medv_ev = calc[3];
	
	// sum product
	double sprod = 0.0;

	for (int i = 0; i < n; i++) {
		sprod += (rm[i] - rm_ev) * (medv[i] - medv_ev);
	}

	// save calculation
	calc[4] = sprod / (n - 1);
	return sprod / (n - 1);
}

double cor(vector<double> rm, vector<double> medv, int n, double rm_ev, double medv_ev, double covar) {
	// sum product per vector
	double rm_sprod = 0.0, medv_sprod = 0.0;

	for (int i = 0; i < n; i++) {
		rm_sprod += (rm[i] - rm_ev)  * (rm[i] - rm_ev);
		medv_sprod += (medv[i] - medv_ev) * (medv[i] - medv_ev);
	}

	rm_sprod = rm_sprod / (n - 1);
	medv_sprod = medv_sprod / (n - 1);

	return covar / sqrt(rm_sprod * medv_sprod);
}