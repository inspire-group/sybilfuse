/* Graph class
author: Peng Gao
pgao@princeton.edu */

#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <string>

#include "node.h"

using namespace std;



/* Similarity scores based on local information */
struct similarity{
	double commonNeighbors;
	double saltonIndex;
	double jaccardIndex;
	double sorensenIndex;
	double hubPromotedIndex;
	double hubDepressedIndex;
	double leichtHolmeNewmanIndex;
	double adamicAdarIndex;
	double preferentialAttachment;
	double resourceAllocation;
};


class Graph{
	vector<Node*> nodes;
	int numNodes;
	int numEdges;

public:
	/* Maintenance functions */
	Graph(); // constructor
	~Graph(); // destructor
	void insertNode(int index);	// insert a node into the graph
	void initIdentity(int index, int identity);	// initialize identity of a node
	void insertNeighbor(int index1, int index2); // initialize set of neighbors of a node
	void initNumNodes(void); // initialize numNodes
	void initNumEdges(void); // initialize numEdges

	
	/* Help functions (graph info) */
	int getNumNodes(void); // number of nodes
	int getNumEdges(void); // number of undirected edges
	void printGraph(void); // graph print out
	void printHighDegreeNodes(void); // 30 highest degree nodes
	int getNumSpamEdges(void); // num of spam edges
	int getNumBenignEdges(void); // num of benign edges
	bool hasCommonNeighbors(int index1, int index2); // check whether two nodes has common neighbors
	int getNumEdges_NoCN(void); // num of no-common-neighbor edges
	int getNumSpamEdges_NoCN(void); // num of no-common-neighbor spam edges 
	int getNumBenignEdges_NoCN(void); // num of no-common-neighbor benign edges


	/* Feature computation functions */
	/* Similarity scores based on local information */
	similarity getSimilarityScore(int index1, int index2);

	/* Feature matrix output (w.r.t links)*/
	// void featureMat(string fileOut1, string fileOut2);	// all links
	void featureMat(string fileOut);
	void featureMatSpam(string fileOut1, string fileOut2); // all spam links
	void featureMatSpecify(int startNode, int endNode, string fileOut1, string fileOut2); // for the nodes in [startNode, endNode]
	
	/* Links sampling */
	void featureMatSampled(int numSampledEdges, string fileOut1, string fileOut2); // randomly sample links
	void featureMatBenignSampled(int numSampledEdges, string fileOut1, string fileOut2); // randomly sample benign links
	void featureMatSpamSampled(int numSampledEdges, string fileOut1, string fileOut2); // randomly sample spam links
	void featureMatRatioSampled(int numSpamEdges, double ratio, string benignFileOut1, string benignFileOut2, string spamFileOut1, string spamFileOut2); // randomly sample benign & spam links at certain ratio


	void featureMatRemoveSampled(int numSampledEdges, string fileOut1, string fileOut2); // randomly sample links, without CN
	void featureMatRatioRemoveSampled(int numSpamEdges, double ratio, string benignFileOut1, string benignFileOut2, string spamFileOut1, string spamFileOut2); // randomly sample benign & spam links at certain ratio, without CN

	
	/* Other measurements */
	void getDegree(string allOut, string benignOut, string spamOut); // output three files containing degree info of all nodes, benign nodes, spam nodes
	void getDegreeAndRatio(string allOut, string benignOut, string spamOut); // Nodes degree info plus the ratio of spam over benign of neighbors
	

	
};






#endif


