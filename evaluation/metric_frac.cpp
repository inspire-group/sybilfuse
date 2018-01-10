/*
 This program takes the input post.txt act.txt suspend.txt, rank the post and output the fraction of sybil nodes in the bottom 1000, 10000, 100000, 1000000 items.
*/


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <assert.h>
#include <string.h>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include <sstream>
#include <iostream>

using namespace std;

bool my_comparator ( const pair<double, int>& l, const pair<double, int>& r){ 
	return l.first < r.first; 
}


bool compare(const pair<int,double>&i, const pair<int,double>&j)
{
    return i.second < j.second;
}


vector<int> classify(vector<double> posteriors) {
	// Initialize prediction
	vector<int> predictions;
	for (int i = 0; i < posteriors.size(); i++){
		if (posteriors[i] < 0.5) {
			predictions.push_back(1); // sybil
		}
		else {
			predictions.push_back(0); // benign
		}
	}
	return predictions;
}

double* rankdata(vector<double> predictions){
	//tiedRank of predictions
	vector< pair<double, int> > new_predictions;
	
	vector<double>::iterator iter;
	int i=0;
	for (iter=predictions.begin(), i = 0; iter != predictions.end(); iter++, i++) {
		new_predictions.push_back(make_pair(*iter, i));
	}
	
	sort(new_predictions.begin(), new_predictions.end(), my_comparator);
	
	int *ivec;
	double *svec;
	
	ivec = (int*) malloc(sizeof(int) * new_predictions.size());
	svec = (double*) malloc(sizeof(double) * new_predictions.size());
	
	
	for (i=0; i < new_predictions.size(); i++) {
		ivec[i] = new_predictions[i].second;
		svec[i] = new_predictions[i].first;
	}
	
	double sumranks = 0, dupcount = 0, averank = 0;
	int n = new_predictions.size();
	
	double *newarray;
	newarray = (double*) malloc(sizeof(double) * new_predictions.size());
	
	for(i = 0; i < n; i++) {
		sumranks += i;
        dupcount += 1;
		if (i == n - 1 || svec[i] != svec[i + 1]){
			averank = sumranks / dupcount + 1;
			for (int j = i - dupcount + 1; j < i + 1; j++) {
				newarray[ivec[j]] = averank;
			}
			sumranks = 0;
            dupcount = 0;
		}
	}
	
	free(ivec);
	free(svec);
	
	return newarray;
}

double AUC(vector<double> posteriors, vector<int> labels){
	double auc = 0, num_benign = 0, num_sybil = 0;
	
	for (long i = 0; i < labels.size(); i++) {
		if (labels[i] == 0){
			num_benign += 1; // benign
		}
		else{
			num_sybil += 1;
		}
	}
	
	double *tiedRank;
	tiedRank = rankdata(posteriors);
	double sum_rank = 0.0;
	
	for (long i = 0; i < posteriors.size(); i++) {
		if (labels[i] == 0){
			sum_rank += tiedRank[i];
		}
	}
	auc = (sum_rank - (num_benign * num_benign + num_benign) / 2.0) / (num_benign * num_sybil);
	
	free(tiedRank);
	
	return auc;
}

vector<double> evaluate(vector<double> posteriors, vector<int> labels) {
	vector<double> results;
	vector<int> predictions = classify(posteriors);

	// tp, tn, fp, fn
	long tp = 0;
	long tn = 0;
	long fp = 0;
	long fn = 0;
	for (int i = 0; i < labels.size(); i++) {
		if (labels[i] == 0 && predictions[i] == 0) {
			tn ++;
		}
		else if (labels[i] == 0 && predictions[i] == 1) {
			fp ++;
		}
		else if (labels[i] == 1 && predictions[i] == 0) {
			fn ++;
		}
		else{
			tp ++;
		}
	}
	
	long total = tp+tn+fp+fn;
	double accuracy = (double)(tp + tn)/(double)(tp + tn + fp + fn);
	double auc = AUC(posteriors, labels);

	// store results;
	results.push_back(tp);
	results.push_back(tn);
	results.push_back(fp);
	results.push_back(fn);
	results.push_back(total);	
	results.push_back(accuracy);
	results.push_back(auc);

	return results;
}




/* main function */
int main (int argc, char **argv)
{	
	// input files
	// string post_file = argv[1];
	string benign_file = "/data1/Large_Twitter/Old_Undirected/act-mapped.txt";
	string sybil_file = "/data1/Large_Twitter/Old_Undirected/suspend-mapped.txt";


	// vector<long> all_nodes;
	vector<long> benign_nodes;
	vector<long> sybil_nodes;
	vector<double> post;

	ifstream input1(argv[1]);
	long node_ind;
	double post_val;
	double tmp;
	while(true){
		input1 >> node_ind >> post_val >> tmp;
		if (input1.eof()) break;
		// all_nodes.push_back(node_ind);
		post.push_back(post_val);
	}
	input1.close();

	ifstream input2(benign_file.c_str());
	while(true){
		input2 >> node_ind;
		if (input2.eof()) break;
		benign_nodes.push_back(node_ind);
	}
	input2.close();

	ifstream input3(sybil_file.c_str());
	while(true){
		input3 >> node_ind;
		if (input3.eof()) break;
		sybil_nodes.push_back(node_ind);
	}
	input3.close();


	// sanity check
	// for (int i = 0; i < post.size(); i++){
	// 	cout << post[i] << endl;
	// }



	// process
	vector< pair<int, double> > res;

	vector<int> labels; // only record benign & sybil, do not consider other nodes
	vector<double> posteriors;
	for (long i = 0; i < benign_nodes.size(); i++) {
		res.push_back(make_pair(0, post[benign_nodes[i]]));
	}
	for (long i = 0; i < sybil_nodes.size(); i++) {
		res.push_back(make_pair(1, post[sybil_nodes[i]]));
	}

	// // evaluate
	// // vector<double> results = evaluate(posteriors, labels);

	sort(res.begin(), res.end(), compare);
	cout << "res size: " << res.size() << endl;
	cout << "sort done. " << endl;

	vector< pair<int, int> > frac_results;

	int sybil_count = 0;
	int benign_count = 0;


	for (int i = 0; i < res.size(); i++) {
		if (res[i].first == 1) { // sybil node
			sybil_count ++;
		}
		else{
			benign_count ++;
		}

		if (i == 999 || i == 9999 || i == 49999 || i == 99999 || i == 999999 || i == 9999999) {
			frac_results.push_back(make_pair(sybil_count, benign_count));
		}
	}

	cout << "frac_results size: " << frac_results.size() << endl;

	cout << "first 1000 (sybil benign): " << frac_results[0].first << " " << frac_results[0].second << endl;
	cout << "first 10000 (sybil benign): " << frac_results[1].first << " " << frac_results[1].second << endl;
	cout << "first 50000 (sybil benign): " << frac_results[2].first << " " << frac_results[2].second << endl;
	cout << "first 100000 (sybil benign): " << frac_results[3].first << " " << frac_results[3].second << endl;
	cout << "first 1000000 (sybil benign): " << frac_results[4].first << " " << frac_results[4].second << endl;
	cout << "first 10000000 (sybil benign): " << frac_results[5].first << " " << frac_results[5].second << endl;


	// for (int i = 0; i < res.size(); i++) {
	// 	cout << res[i].first << " " << res[i].second << endl;

	// }




	// display
	// cout << "tp: " << results[0] << endl;
	// cout << "tn: " << results[1] << endl;
	// cout << "fp: " << results[2] << endl;
	// cout << "fn: " << results[3] << endl;
	// cout << "total: " << results[4] << endl;
	// cout << "accuracy: " << results[5] << endl;
	// cout << "auc: " << results[6] << endl;


    return 0;
}