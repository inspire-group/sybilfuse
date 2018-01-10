#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>
#include <time.h>


#include "node.h"
#include "node.C"
#include "graph.h"
#include "graph.C"

//#define NUM_NODES	5
using namespace std;

int main(int argc, char *argv[]){
	clock_t start, finish;
	double duration;
	start = clock();


	// -----------------------------------------------------------------
	// Graph initialization
	// -----------------------------------------------------------------
	Graph graph;
	
	// Initialize the index of nodes
	int numNodes;
	ifstream input1(argv[1]);
	input1 >> numNodes;
	for (int i=0; i<numNodes; i++){
		graph.insertNode(i);
	}

	// Initialize the neighbors of nodes
	int index1;
	int index2;
	while(true){
		input1 >> index1 >> index2;
		if (input1.eof()) break;
		graph.insertNeighbor(index1, index2);
	}
	input1.close();

	// Initialize the identities of nodes
	ifstream input2(argv[2]); //act
	int index_act;
	while(true){
		input2 >> index_act;
		if(input2.eof()) break;
		graph.initIdentity(index_act,1);
	}
	input2.close();

	ifstream input3(argv[3]); //suspend
	int index_suspend;
	while(true){
		input3 >> index_suspend;
		if(input3.eof()) break;
		graph.initIdentity(index_suspend,-1);
	}
	input3.close();

	graph.initNumNodes();
	graph.initNumEdges();
	
	// -----------------------------------------------------------------
	// Graph information printout
	// -----------------------------------------------------------------

	cout << "Graph sucess!\n";
	cout << "num nodes is " << graph.getNumNodes() << endl;
	cout << "num edges is " << graph.getNumEdges() << endl;
	// cout << "num spam edges is " << graph.getNumSpamEdges() << endl;
	// cout << "num benign edges is " << graph.getNumBenignEdges() << endl;
	// cout << "num no-CN edges is " << graph.getNumEdges_NoCN() << endl;
	// cout << "num no-CN spam edges is " << graph.getNumSpamEdges_NoCN() << endl;
	// cout << "num no-CN benign edges is " << graph.getNumBenignEdges_NoCN() << endl;
	// graph.printHighDegreeNodes();
	// graph.printGraph();


	// -----------------------------------------------------------------
	// Graph feature output
	// ----------------------------------------------------------------- 

	graph.featureMat(argv[4]);

/*
	int startNode = atoi(argv[4]);
	int endNode = atoi(argv[5]);
	graph.featureMatSpecify(startNode, endNode, argv[6], argv[7]);
*/

/*	int numSampledEdges = atoi(argv[4]);
	graph.featureMatSampled(numSampledEdges, argv[5], argv[6]);
*/

/*
	int numSampledBenignEdges = atoi(argv[4]);
	graph.featureMatBenignSampled(numSampledBenignEdges, argv[5], argv[6]);
	int numSampledSpamEdges = atoi(argv[7]);
	graph.featureMatSpamSampled(numSampledSpamEdges, argv[8], argv[9]);
*/
	// int numSpamEdges = atoi(argv[4]);
	// double ratio = atoi(argv[5]);
	// graph.featureMatRatioSampled(numSpamEdges, ratio, argv[6], argv[7], argv[8], argv[9]);


	// graph.featureMatRemoveSampled(atoi(argv[4]), argv[5], argv[6]);
	// graph.featureMatRatioRemoveSampled(atoi(numSpamEdges, ratio, argv[6], argv[7], argv[8], argv[9]);


	// -----------------------------------------------------------------
	// Other measurements
	// ----------------------------------------------------------------- 
	// graph.getDegree(argv[4], argv[5], argv[6]);
	// graph.getDegreeAndRatio(argv[4], argv[5], argv[6]);

	finish = clock();
	duration = (double)(finish-start)/CLOCKS_PER_SEC;
	cout << "duration is  " << duration << " seconds." << endl;

	return 0;
}
