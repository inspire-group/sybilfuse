#include "graph.h"


/* Maintenance functions */
Graph::Graph(){}

Graph::~Graph(){
	while(!nodes.empty()){
		Node *node = nodes.back();
		nodes.pop_back();
		node->neighbors.clear();
		delete node;
	}
}

void Graph::insertNode(int index){
	Node *newNode = new Node;
	newNode ->index = index;
	nodes.push_back(newNode);
}

void Graph::initIdentity(int index, int identity){
	nodes[index]->identity = identity;
}

void Graph::insertNeighbor(int index1, int index2){
	Node* node1 = nodes[index1];
	node1->neighbors.insert(index2);
	node1->degree ++;
//	node1->neighbors.push_back(index2);
}

void Graph::initNumNodes(void){
	numNodes = nodes.size();
}

void Graph::initNumEdges(void){
	int val = 0;
	for (int i=0; i<nodes.size(); i++){
		val+=nodes[i]->degree;
	}
	val = val/2;
	numEdges = val;
}


/* Help functions (graph info) */
int Graph::getNumNodes(void){
	return numNodes;
}

int Graph::getNumEdges(void){
	return numEdges;
}

void Graph::printGraph(void){
	for (int i=0; i<numNodes; i++){
		cout << nodes[i]->index << " identity " << nodes[i]->identity << " degree " << nodes[i]->degree << ": ";
		for (set<int>::iterator it=nodes[i]->neighbors.begin(); it!=nodes[i]->neighbors.end(); it++){
			cout << " " << *it;
		}
		cout << endl;
	}
}

void Graph::printHighDegreeNodes(void){
	int index[20] = {0};

	for (int i=0; i<20; i++){
		for (int j=0; j<numNodes; j++){
			bool used = false;
			for (int m=0; m<=i; m++){
				if (j == index[m]){
					used = true;
					break;
				}
			}
			
			if (used == true){
				continue;
			}

			if (nodes[j]->degree >= nodes[index[i]]->degree){
				index[i] = j;
			}
		}
	}
	
	for (int i=0; i<20; i++){
		cout << "node " << index[i] << " degree " << nodes[index[i]]->degree << endl;
	}
}


int Graph::getNumSpamEdges(void){
	int count = 0;
	for (int i=0; i<numNodes; i++){
		for (set<int>::iterator it=nodes[i]->neighbors.begin(); it!=nodes[i]->neighbors.end(); it++){
			if (*it <= i){
				continue;
			}
			else{
				if((nodes[i]->identity == -1 && nodes[*it]->identity == 1) || (nodes[i]->identity == 1 && nodes[*it]->identity == -1)){
						count ++;
				}
				else{
					continue;
				}
			}
		}
	}

	return count;
}


int Graph::getNumBenignEdges(void){
	int count = 0;
	for (int i=0; i<numNodes; i++){
		for (set<int>::iterator it=nodes[i]->neighbors.begin(); it!=nodes[i]->neighbors.end(); it++){
			if (*it <= i){
				continue;
			}
			else{
				if(nodes[i]->identity == 1 && nodes[*it]->identity == 1){
						count ++;
				}
				else{
					continue;
				}
			}
		}
	}

	return count;
}


bool Graph::hasCommonNeighbors(int index1, int index2){
	int indexLarge;
	int indexSmall;
	if (nodes[index1]->degree <= nodes[index2]->degree){
		indexLarge = index2;
		indexSmall = index1;
	}
	else{
		indexLarge = index1;
		indexSmall = index2;
	}

	for (set<int>::iterator it=nodes[indexSmall]->neighbors.begin(); it!=nodes[indexSmall]->neighbors.end(); it++){
		if (nodes[indexLarge]->neighbors.find(*it)!=nodes[indexLarge]->neighbors.end()){
			return true;
		}
	}

	return false;
}


int Graph::getNumEdges_NoCN(void){
	int count = 0;
	for (int i=0; i<numNodes; i++){
		for (set<int>::iterator it=nodes[i]->neighbors.begin(); it!=nodes[i]->neighbors.end(); it++){
			if (*it <= i){
				continue;
			}
			else{
				if (!hasCommonNeighbors(i, *it)){
					count ++;
				}
				else{
					continue;
				}
			}
		}
	}

	return count;
}


int Graph::getNumSpamEdges_NoCN(void){
	int count = 0;
	for (int i=0; i<numNodes; i++){
		for (set<int>::iterator it=nodes[i]->neighbors.begin(); it!=nodes[i]->neighbors.end(); it++){
			if (*it <= i){
				continue;
			}
			else{
				if((nodes[i]->identity == -1 && nodes[*it]->identity == 1) || (nodes[i]->identity == 1 && nodes[*it]->identity == -1)){ // spam
					if (!hasCommonNeighbors(i, *it)){
						count ++;
					}
					else{
						continue;
					}
				}
				else{
					continue;
				}
			}
		}
	}

	return count;
}


int Graph::getNumBenignEdges_NoCN(void){
	int count = 0;
	for (int i=0; i<numNodes; i++){
		for (set<int>::iterator it=nodes[i]->neighbors.begin(); it!=nodes[i]->neighbors.end(); it++){
			if (*it <= i){
				continue;
			}
			else{
				if(nodes[i]->identity == 1 && nodes[*it]->identity == 1){ // benign
					if (!hasCommonNeighbors(i, *it)){
						count ++;
					}
					else{
						continue;
					}
				}
				else{
					continue;
				}
			}
		}
	}

	return count;
}



/* Feature matrix output (w.r.t links)*/
/* All links */
void Graph::featureMat(string fileOut){
	ofstream myfile (fileOut.c_str());
	if (myfile.is_open()){
		for (int i=0; i<numNodes; i++){
			for (set<int>::iterator it=nodes[i]->neighbors.begin(); it!=nodes[i]->neighbors.end(); it++){
				if (*it <= i){
					continue;
				}
				else{
					//myfile << "link between " << i << " and " << *it << " "
					myfile << i << " " << *it << " ";
					if((nodes[i]->identity == -1 && nodes[*it]->identity == 1) || (nodes[i]->identity == 1 && nodes[*it]->identity == -1)){
						myfile << "-1" << " "; // benign <-> spam
					}
					else if(nodes[i]->identity == 1 && nodes[*it]->identity == 1) {
						myfile << "1" << " "; // benign <-> benign
					}
					else{
						myfile << "0" << " "; // spam <-> spam, X <-> deleted
					}


					similarity simScore = getSimilarityScore(i, *it);
					myfile << "1:" << simScore.commonNeighbors << " ";
					myfile << "2:" << simScore.saltonIndex << " ";
					myfile << "3:" << simScore.jaccardIndex << " ";
					myfile << "4:" << simScore.sorensenIndex << " ";
					myfile << "5:" << simScore.hubPromotedIndex << " ";
					myfile << "6:" << simScore.hubDepressedIndex << " ";
					myfile << "7:" << simScore.leichtHolmeNewmanIndex << " ";
					myfile << "8:" << simScore.adamicAdarIndex << " ";
					myfile << "9:" << simScore.preferentialAttachment << " ";
					myfile << "10:" << simScore.resourceAllocation << " \n";
				}
			}
		}
		myfile.close();
	}
}


/* All spam links */
void Graph::featureMatSpam(string fileOut1, string fileOut2){
	ofstream myfile1 (fileOut1.c_str());
	ofstream myfile2 (fileOut2.c_str());
	if (myfile1.is_open()){
		for (int i=0; i<numNodes; i++){
			for (set<int>::iterator it=nodes[i]->neighbors.begin(); it!=nodes[i]->neighbors.end(); it++){
				if (*it <= i){
					continue;
				}
				else{
					//myfile << "link between " << i << " and " << *it << " "
					if((nodes[i]->identity == -1 && nodes[*it]->identity == 1) || (nodes[i]->identity == 1 && nodes[*it]->identity == -1)){
						myfile1 << i << " " << *it << " \n";
						myfile2 << "-1" << " "; // spam link
					}
					else{
						continue; // do not include benign links
					}
	
					similarity simScore = getSimilarityScore(i, *it);
					myfile2 << "1:" << simScore.commonNeighbors << " ";
					myfile2 << "2:" << simScore.saltonIndex << " ";
					myfile2 << "3:" << simScore.jaccardIndex << " ";
					myfile2 << "4:" << simScore.sorensenIndex << " ";
					myfile2 << "5:" << simScore.hubPromotedIndex << " ";
					myfile2 << "6:" << simScore.hubDepressedIndex << " ";
					myfile2 << "7:" << simScore.leichtHolmeNewmanIndex << " ";
					myfile2 << "8:" << simScore.adamicAdarIndex << " ";
					myfile2 << "9:" << simScore.preferentialAttachment << " ";
					myfile2 << "10:" << simScore.resourceAllocation << " \n";
				}
			}
		}
		myfile1.close();
		myfile2.close();
	}
}


/* Nodes in (startNode, endNode) */
void Graph::featureMatSpecify(int startNode, int endNode, string fileOut1, string fileOut2){
	ofstream myfile1 (fileOut1.c_str());
	ofstream myfile2 (fileOut2.c_str());
	if (myfile1.is_open()){
		for (int i=startNode; i<=endNode; i++){ // largest value of endNode = numNodes-1
			for (set<int>::iterator it=nodes[i]->neighbors.begin(); it!=nodes[i]->neighbors.end(); it++){
				if (*it <= i){
					continue;
				}
				else{
					//myfile << "link between " << i << " and " << *it << " "
					if((nodes[i]->identity == -1 && nodes[*it]->identity == 1) || (nodes[i]->identity == 1 && nodes[*it]->identity == -1)){
						myfile1 << i << " " << *it << " \n";
						myfile2 << "-1" << " "; // spam link
					}
					else if (nodes[i]->identity == 1 && nodes[*it]->identity == 1){
						myfile1 << i << " " << *it << " \n";
						myfile2 << "+1" << " "; // benign link, links between active accounts
					}
					else{
						continue; // other links
					}
	
					similarity simScore = getSimilarityScore(i, *it);
					myfile2 << "1:" << simScore.commonNeighbors << " ";
					myfile2 << "2:" << simScore.saltonIndex << " ";
					myfile2 << "3:" << simScore.jaccardIndex << " ";
					myfile2 << "4:" << simScore.sorensenIndex << " ";
					myfile2 << "5:" << simScore.hubPromotedIndex << " ";
					myfile2 << "6:" << simScore.hubDepressedIndex << " ";
					myfile2 << "7:" << simScore.leichtHolmeNewmanIndex << " ";
					myfile2 << "8:" << simScore.adamicAdarIndex << " ";
					myfile2 << "9:" << simScore.preferentialAttachment << " ";
					myfile2 << "10:" << simScore.resourceAllocation << " \n";
				
				}
			}
		}
		myfile1.close();
		myfile2.close();
	}
}


/* Links sampling */
/* Randomly sample links */
void Graph::featureMatSampled(int numSampledEdges, string fileOut1, string fileOut2){
	ofstream myfile1 (fileOut1.c_str());
	ofstream myfile2 (fileOut2.c_str());
	if (myfile1.is_open()){
		int count = numSampledEdges;
		int index1;
		int index2;
		set<string> links; // set container to store sampled links
		srand((unsigned)time(NULL)); // RAND_MAX is 2147483647

		while (count > 0){
			// Generate random number
			double r = (double) rand() / RAND_MAX;
			
			// Determine index1
			double sum = 0;
			int i = 0;
			while (i < numNodes){
				if (sum > r){
					break;
				}
				else{
					sum += ((double) nodes[i]->degree) / (2 * numEdges);
					i++;
				}
			}
			index1 = i - 1;

			// Determine index2
			int maxNeighbor = nodes[index1]->degree;
			set<int>::iterator it;
			int stepCount = rand() % maxNeighbor;
			it = nodes[index1]->neighbors.begin();
			for (int j=0; j<stepCount; j++){
				it++;
			}
			index2 = *it;

			// Sort index1 & index2
			if (index1 > index2){
				int temp = index1;
				index1 = index2;
				index2 = temp;
			}

			// Check if sampled before
			string str = to_string(index1);
			str.append("+");
			str.append(to_string(index2));
			if (links.find(str) != links.end()){
				continue;
			}
			else{ // Sample out a new link, which may not specifically be a benign or spam link
				if((nodes[index1]->identity == -1 && nodes[index2]->identity == 1) || (nodes[index1]->identity == 1 && nodes[index2]->identity == -1)){
					links.insert(str); // a new spam link
					count --;

					myfile1 << index1 << " " << index2 << " \n";
					myfile2 << "-1" << " "; // spam link
				}
				else if (nodes[index1]->identity == 1 && nodes[index2]->identity == 1){
					links.insert(str); // a new benign link
					count --;

					myfile1 << index1 << " " << index2 << " \n";
					myfile2 << "+1" << " "; // benign link, links between active accounts
				}
				else{
					continue; // other links
				}

				similarity simScore = getSimilarityScore(index1, index2);
				myfile2 << "1:" << simScore.commonNeighbors << " ";
				myfile2 << "2:" << simScore.saltonIndex << " ";
				myfile2 << "3:" << simScore.jaccardIndex << " ";
				myfile2 << "4:" << simScore.sorensenIndex << " ";
				myfile2 << "5:" << simScore.hubPromotedIndex << " ";
				myfile2 << "6:" << simScore.hubDepressedIndex << " ";
				myfile2 << "7:" << simScore.leichtHolmeNewmanIndex << " ";
				myfile2 << "8:" << simScore.adamicAdarIndex << " ";
				myfile2 << "9:" << simScore.preferentialAttachment << " ";
				myfile2 << "10:" << simScore.resourceAllocation << " \n";
			}
		}
		myfile1.close();
		myfile2.close();
	}
}


/* Randomly sample benign links */
void Graph::featureMatBenignSampled(int numSampledEdges, string fileOut1, string fileOut2){
	ofstream myfile1 (fileOut1.c_str());
	ofstream myfile2 (fileOut2.c_str());
	if (myfile1.is_open()){
		int count = numSampledEdges;
		int index1;
		int index2;
		set<string> links; // set container to store sampled links
		srand((unsigned)time(NULL)); // RAND_MAX is 2147483647

		while (count > 0){
			// Generate random number
			double r = (double) rand() / RAND_MAX;
			
			// Determine index1
			double sum = 0;
			int i = 0;
			while (i < numNodes){
				if (sum > r){
					break;
				}
				else{
					sum += ((double) nodes[i]->degree) / (2 * numEdges);
					i++;
				}
			}
			index1 = i - 1;

			// Determine index2
			int maxNeighbor = nodes[index1]->degree;
			set<int>::iterator it;
			int stepCount = rand() % maxNeighbor;
			it = nodes[index1]->neighbors.begin();
			for (int j=0; j<stepCount; j++){
				it++;
			}
			index2 = *it;

			// Sort index1 & index2
			if (index1 > index2){
				int temp = index1;
				index1 = index2;
				index2 = temp;
			}

			// Check if sampled before
			string str = to_string(index1);
			str.append("+");
			str.append(to_string(index2));
			if (links.find(str) != links.end()){
				continue;
			}
			else{ // a new link
				if (nodes[index1]->identity == 1 && nodes[index2]->identity == 1){ // a new benign link
					links.insert(str);
					count --;

					// Output the link feature matrix
					myfile1 << index1 << " " << index2 << " \n";
					myfile2 << "+1" << " "; // benign link

					similarity simScore = getSimilarityScore(index1, index2);
					myfile2 << "1:" << simScore.commonNeighbors << " ";
					myfile2 << "2:" << simScore.saltonIndex << " ";
					myfile2 << "3:" << simScore.jaccardIndex << " ";
					myfile2 << "4:" << simScore.sorensenIndex << " ";
					myfile2 << "5:" << simScore.hubPromotedIndex << " ";
					myfile2 << "6:" << simScore.hubDepressedIndex << " ";
					myfile2 << "7:" << simScore.leichtHolmeNewmanIndex << " ";
					myfile2 << "8:" << simScore.adamicAdarIndex << " ";
					myfile2 << "9:" << simScore.preferentialAttachment << " ";
					myfile2 << "10:" << simScore.resourceAllocation << " \n";
				}
				else{
					continue;
				}
			}
		}
		myfile1.close();
		myfile2.close();
	}
}


/* Randomly sample spam links */
void Graph::featureMatSpamSampled(int numSampledEdges, string fileOut1, string fileOut2){
	ofstream myfile1 (fileOut1.c_str());
	ofstream myfile2 (fileOut2.c_str());
	if (myfile1.is_open()){
		int count = numSampledEdges;
		int index1;
		int index2;
		set<string> links; // set container to store sampled links
		srand((unsigned)time(NULL)); // RAND_MAX is 2147483647

		while (count > 0){
			// Generate random number
			double r = (double) rand() / RAND_MAX;
			
			// Determine index1
			double sum = 0;
			int i = 0;
			while (i < numNodes){
				if (sum > r){
					break;
				}
				else{
					sum += ((double) nodes[i]->degree) / (2 * numEdges);
					i++;
				}
			}
			index1 = i - 1;

			// Determine index2
			int maxNeighbor = nodes[index1]->degree;
			set<int>::iterator it;
			int stepCount = rand() % maxNeighbor;
			it = nodes[index1]->neighbors.begin();
			for (int j=0; j<stepCount; j++){
				it++;
			}
			index2 = *it;

			// Sort index1 & index2
			if (index1 > index2){
				int temp = index1;
				index1 = index2;
				index2 = temp;
			}

			// Check if sampled before
			string str = to_string(index1);
			str.append("+");
			str.append(to_string(index2));
			if (links.find(str) != links.end()){
				continue;
			}
			else{ // a new link
				if ((nodes[index1]->identity == -1 && nodes[index2]->identity == 1) || (nodes[index1]->identity == 1 && nodes[index2]->identity == -1)){
					// spam link
					links.insert(str);
					count --;

					// Output the link feature matrix
					myfile1 << index1 << " " << index2 << " \n";
					myfile2 << "-1" << " "; // spam link

					similarity simScore = getSimilarityScore(index1, index2);
					myfile2 << "1:" << simScore.commonNeighbors << " ";
					myfile2 << "2:" << simScore.saltonIndex << " ";
					myfile2 << "3:" << simScore.jaccardIndex << " ";
					myfile2 << "4:" << simScore.sorensenIndex << " ";
					myfile2 << "5:" << simScore.hubPromotedIndex << " ";
					myfile2 << "6:" << simScore.hubDepressedIndex << " ";
					myfile2 << "7:" << simScore.leichtHolmeNewmanIndex << " ";
					myfile2 << "8:" << simScore.adamicAdarIndex << " ";
					myfile2 << "9:" << simScore.preferentialAttachment << " ";
					myfile2 << "10:" << simScore.resourceAllocation << " \n";

				}
				else{ // do not include benign link
					continue;
				}
			}
		}
		myfile1.close();
		myfile2.close();
	}
}

/* Randomly sample benign & spam links with user specified ratio*/
void Graph::featureMatRatioSampled(int numSpamEdges, double ratio, string benignFileOut1, string benignFileOut2, string spamFileOut1, string spamFileOut2){
	ofstream benignFile1 (benignFileOut1.c_str());
	ofstream benignFile2 (benignFileOut2.c_str());
	ofstream spamFile1 (spamFileOut1.c_str());
	ofstream spamFile2 (spamFileOut2.c_str());

	if (benignFile1.is_open() || benignFile2.is_open()){
		int countSpam = numSpamEdges;
		int countBenign = (int)(numSpamEdges * ratio);
		int index1;
		int index2;
		set<string> benignLinks;
		set<string> spamLinks;
		srand((unsigned)time(NULL)); // RAND_MAX is 2147483647

		while (countBenign > 0 || countSpam > 0){
			double r = (double) rand() / RAND_MAX;
			
			// Determine index1
			double sum = 0;
			int i = 0;
			while (i < numNodes){
				if (sum > r){
					break;
				}
				else{
					sum += ((double) nodes[i]->degree) / (2 * numEdges);
					i++;
				}
			}
			index1 = i - 1;

			// Determine index2
			int maxNeighbor = nodes[index1]->degree;
			set<int>::iterator it;
			int stepCount = rand() % maxNeighbor;
			it = nodes[index1]->neighbors.begin();
			for (int j=0; j<stepCount; j++){
				it++;
			}
			index2 = *it;

			// Sort index1 & index2
			if (index1 > index2){
				int temp = index1;
				index1 = index2;
				index2 = temp;
			}

			// Check if sampled before
			string str = to_string(index1);
			str.append("+");
			str.append(to_string(index2));
			if (benignLinks.find(str) != benignLinks.end() || spamLinks.find(str) != spamLinks.end()){
				continue;
			}
			else{ // sample out a new link
				if ((nodes[index1]->identity == -1 && nodes[index2]->identity == 1) || (nodes[index1]->identity == 1 && nodes[index2]->identity == -1)){
					/* Spam link */
					if (countSpam > 0){
						spamLinks.insert(str);
						countSpam --;

						spamFile1 << index1 << " " << index2 << " \n";
						spamFile2 << "-1" << " "; // spam link
						similarity simScore = getSimilarityScore(index1, index2);
						spamFile2 << "1:" << simScore.commonNeighbors << " ";
						spamFile2 << "2:" << simScore.saltonIndex << " ";
						spamFile2 << "3:" << simScore.jaccardIndex << " ";
						spamFile2 << "4:" << simScore.sorensenIndex << " ";
						spamFile2 << "5:" << simScore.hubPromotedIndex << " ";
						spamFile2 << "6:" << simScore.hubDepressedIndex << " ";
						spamFile2 << "7:" << simScore.leichtHolmeNewmanIndex << " ";
						spamFile2 << "8:" << simScore.adamicAdarIndex << " ";
						spamFile2 << "9:" << simScore.preferentialAttachment << " ";
						spamFile2 << "10:" << simScore.resourceAllocation << " \n";
					}
					else{
						continue;
					}
				}
				else if (nodes[index1]->identity == 1 && nodes[index2]->identity == 1){
					/* Benign link */
					if (countBenign > 0){
						benignLinks.insert(str);
						countBenign --;

						benignFile1 << index1 << " " << index2 << " \n";
						benignFile2 << "+1" << " "; // benign link
						similarity simScore = getSimilarityScore(index1, index2);
						benignFile2 << "1:" << simScore.commonNeighbors << " ";
						benignFile2 << "2:" << simScore.saltonIndex << " ";
						benignFile2 << "3:" << simScore.jaccardIndex << " ";
						benignFile2 << "4:" << simScore.sorensenIndex << " ";
						benignFile2 << "5:" << simScore.hubPromotedIndex << " ";
						benignFile2 << "6:" << simScore.hubDepressedIndex << " ";
						benignFile2 << "7:" << simScore.leichtHolmeNewmanIndex << " ";
						benignFile2 << "8:" << simScore.adamicAdarIndex << " ";
						benignFile2 << "9:" << simScore.preferentialAttachment << " ";
						benignFile2 << "10:" << simScore.resourceAllocation << " \n";
					}
					else{
						continue;
					}
				}
				else{
					continue;
				}
			}
		}
		benignFile1.close();
		benignFile2.close();
		spamFile1.close();
		spamFile2.close();
	}
}


/* Randomly sample links, without CN */
void Graph::featureMatRemoveSampled(int numSampledEdges, string fileOut1, string fileOut2){
	ofstream myfile1 (fileOut1.c_str());
	ofstream myfile2 (fileOut2.c_str());
	if (myfile1.is_open()){
		int count = numSampledEdges;
		int index1;
		int index2;
		set<string> links; // set container to store sampled links
		srand((unsigned)time(NULL)); // RAND_MAX is 2147483647

		while (count > 0){
			// Generate random number
			double r = (double) rand() / RAND_MAX;
			
			// Determine index1
			double sum = 0;
			int i = 0;
			while (i < numNodes){
				if (sum > r){
					break;
				}
				else{
					sum += ((double) nodes[i]->degree) / (2 * numEdges);
					i++;
				}
			}
			index1 = i - 1;

			// Determine index2
			int maxNeighbor = nodes[index1]->degree;
			set<int>::iterator it;
			int stepCount = rand() % maxNeighbor;
			it = nodes[index1]->neighbors.begin();
			for (int j=0; j<stepCount; j++){
				it++;
			}
			index2 = *it;

			// Sort index1 & index2
			if (index1 > index2){
				int temp = index1;
				index1 = index2;
				index2 = temp;
			}

			// Check if sampled before
			string str = to_string(index1);
			str.append("+");
			str.append(to_string(index2));
			if (links.find(str) != links.end()){
				continue;
			}
			else{ // Sample out a new link, which may not specifically be a benign or spam link
				if((nodes[index1]->identity == -1 && nodes[index2]->identity == 1) || (nodes[index1]->identity == 1 && nodes[index2]->identity == -1)){
					if (hasCommonNeighbors(index1, index2)){
						links.insert(str); // a new spam link
						count --;

						myfile1 << index1 << " " << index2 << " \n";
						myfile2 << "-1" << " "; // spam link
					}
					else{
						continue;
					}
				}
				else if (nodes[index1]->identity == 1 && nodes[index2]->identity == 1){
					if (hasCommonNeighbors(index1, index2)){
						links.insert(str); // a new benign link
						count --;

						myfile1 << index1 << " " << index2 << " \n";
						myfile2 << "+1" << " "; // benign link, links between active accounts
					}
					else{
						continue;
					}
				}
				else{
					continue; // other links
				}

				similarity simScore = getSimilarityScore(index1, index2);
				myfile2 << "1:" << simScore.commonNeighbors << " ";
				myfile2 << "2:" << simScore.saltonIndex << " ";
				myfile2 << "3:" << simScore.jaccardIndex << " ";
				myfile2 << "4:" << simScore.sorensenIndex << " ";
				myfile2 << "5:" << simScore.hubPromotedIndex << " ";
				myfile2 << "6:" << simScore.hubDepressedIndex << " ";
				myfile2 << "7:" << simScore.leichtHolmeNewmanIndex << " ";
				myfile2 << "8:" << simScore.adamicAdarIndex << " ";
				myfile2 << "9:" << simScore.preferentialAttachment << " ";
				myfile2 << "10:" << simScore.resourceAllocation << " \n";
			}
		}
		myfile1.close();
		myfile2.close();
	}
}


/* Randomly sample benign & spam links with user specified ratio, do not consider links with no common neighbors */
void Graph::featureMatRatioRemoveSampled(int numSpamEdges, double ratio, string benignFileOut1, string benignFileOut2, string spamFileOut1, string spamFileOut2){
	ofstream benignFile1 (benignFileOut1.c_str());
	ofstream benignFile2 (benignFileOut2.c_str());
	ofstream spamFile1 (spamFileOut1.c_str());
	ofstream spamFile2 (spamFileOut2.c_str());

	if (benignFile1.is_open() || benignFile2.is_open()){
		int countSpam = numSpamEdges;
		int countBenign = (int)(numSpamEdges * ratio);
		int index1;
		int index2;
		set<string> benignLinks;
		set<string> spamLinks;
		srand((unsigned)time(NULL)); // RAND_MAX is 2147483647

		while (countBenign > 0 || countSpam > 0){
			double r = (double) rand() / RAND_MAX;
			
			// Determine index1
			double sum = 0;
			int i = 0;
			while (i < numNodes){
				if (sum > r){
					break;
				}
				else{
					sum += ((double) nodes[i]->degree) / (2 * numEdges);
					i++;
				}
			}
			index1 = i - 1;

			// Determine index2
			int maxNeighbor = nodes[index1]->degree;
			set<int>::iterator it;
			int stepCount = rand() % maxNeighbor;
			it = nodes[index1]->neighbors.begin();
			for (int j=0; j<stepCount; j++){
				it++;
			}
			index2 = *it;

			// Sort index1 & index2
			if (index1 > index2){
				int temp = index1;
				index1 = index2;
				index2 = temp;
			}

			// Check if sampled before
			string str = to_string(index1);
			str.append("+");
			str.append(to_string(index2));
			if (benignLinks.find(str) != benignLinks.end() || spamLinks.find(str) != spamLinks.end()){
				continue;
			}
			else{ // sample out a new link
				if ((nodes[index1]->identity == -1 && nodes[index2]->identity == 1) || (nodes[index1]->identity == 1 && nodes[index2]->identity == -1)){
					/* Spam link */
					if (countSpam > 0){
						if (hasCommonNeighbors(index1, index2)){ // a non-zero CN spam link
							spamLinks.insert(str);
							countSpam --;

							similarity simScore = getSimilarityScore(index1, index2);
							spamFile1 << index1 << " " << index2 << " \n";
							spamFile2 << "-1" << " "; // spam link
							spamFile2 << "1:" << simScore.commonNeighbors << " ";
							spamFile2 << "2:" << simScore.saltonIndex << " ";
							spamFile2 << "3:" << simScore.jaccardIndex << " ";
							spamFile2 << "4:" << simScore.sorensenIndex << " ";
							spamFile2 << "5:" << simScore.hubPromotedIndex << " ";
							spamFile2 << "6:" << simScore.hubDepressedIndex << " ";
							spamFile2 << "7:" << simScore.leichtHolmeNewmanIndex << " ";
							spamFile2 << "8:" << simScore.adamicAdarIndex << " ";
							spamFile2 << "9:" << simScore.preferentialAttachment << " ";
							spamFile2 << "10:" << simScore.resourceAllocation << " \n";
						}
						else{ // no common neighbor
							continue;
						}
					}
					else{
						continue;
					}
				}
				else if (nodes[index1]->identity == 1 && nodes[index2]->identity == 1){
					/* Benign link */
					if (countBenign > 0){
						if (hasCommonNeighbors(index1, index2)){
							benignLinks.insert(str);
							countBenign --;

							similarity simScore = getSimilarityScore(index1, index2);
							benignFile1 << index1 << " " << index2 << " \n";
							benignFile2 << "+1" << " "; // benign link
							benignFile2 << "1:" << simScore.commonNeighbors << " ";
							benignFile2 << "2:" << simScore.saltonIndex << " ";
							benignFile2 << "3:" << simScore.jaccardIndex << " ";
							benignFile2 << "4:" << simScore.sorensenIndex << " ";
							benignFile2 << "5:" << simScore.hubPromotedIndex << " ";
							benignFile2 << "6:" << simScore.hubDepressedIndex << " ";
							benignFile2 << "7:" << simScore.leichtHolmeNewmanIndex << " ";
							benignFile2 << "8:" << simScore.adamicAdarIndex << " ";
							benignFile2 << "9:" << simScore.preferentialAttachment << " ";
							benignFile2 << "10:" << simScore.resourceAllocation << " \n";
						}
						else{
							continue;
						}
					}
					else{
						continue;
					}
				}
				else{
					continue;
				}
			}
		}
		benignFile1.close();
		benignFile2.close();
		spamFile1.close();
		spamFile2.close();
	}
}


/* Feature computation functions */
/* Similarity scores based on local information */
similarity Graph::getSimilarityScore(int index1, int index2){
	similarity simScore;
	double commonNeighbors = 0;
	double saltonIndex = 0;
	double jaccardIndex = 0;
	double sorensenIndex = 0;
	double hubPromotedIndex = 0;
	double hubDepressedIndex = 0;
	double leichtHolmeNewmanIndex = 0;
	double adamicAdarIndex = 0;
	double preferentialAttachment = 0;
	double resourceAllocation = 0;

	int indexLarge;
	int indexSmall;
	if (nodes[index1]->degree <= nodes[index2]->degree){
		indexLarge = index2;
		indexSmall = index1;
	}
	else{
		indexLarge = index1;
		indexSmall = index2;
	}

	for (set<int>::iterator it=nodes[indexSmall]->neighbors.begin(); it!=nodes[indexSmall]->neighbors.end(); it++){
		if (nodes[indexLarge]->neighbors.find(*it)!=nodes[indexLarge]->neighbors.end()){
			commonNeighbors += 1;
			adamicAdarIndex += 1/log((double)nodes[*it]->degree);
			resourceAllocation += 1 / ((double)nodes[*it]->degree);
		}
	}

	saltonIndex = commonNeighbors/sqrt((nodes[indexSmall]->degree) * (nodes[indexLarge]->degree));
	jaccardIndex = commonNeighbors/((nodes[indexSmall]->degree) + (nodes[indexLarge]->degree) - commonNeighbors);
	sorensenIndex = commonNeighbors*2/((nodes[indexSmall]->degree) + (nodes[indexLarge]->degree));
	hubPromotedIndex = commonNeighbors/min(nodes[indexSmall]->degree, nodes[indexLarge]->degree);
	hubDepressedIndex = commonNeighbors/max(nodes[indexSmall]->degree, nodes[indexLarge]->degree);
	leichtHolmeNewmanIndex = commonNeighbors/((nodes[indexSmall]->degree) * (nodes[indexLarge]->degree));
	preferentialAttachment = (nodes[indexSmall]->degree) * (nodes[indexLarge]->degree);


	simScore.commonNeighbors = commonNeighbors;
	simScore.saltonIndex = saltonIndex;
	simScore.jaccardIndex = jaccardIndex;
	simScore.sorensenIndex = sorensenIndex;
	simScore.hubPromotedIndex = hubPromotedIndex;
	simScore.hubDepressedIndex = hubDepressedIndex;
	simScore.leichtHolmeNewmanIndex = leichtHolmeNewmanIndex;
	simScore.adamicAdarIndex = adamicAdarIndex;
	simScore.preferentialAttachment = preferentialAttachment;
	simScore.resourceAllocation = resourceAllocation;

	return simScore;
}


/* Other measurements */
/* Output three files containing degree info of all nodes, benign nodes, spam nodes */
void Graph::getDegree(string allOut, string benignOut, string spamOut){
	ofstream allFile (allOut.c_str());
	ofstream benignFile (benignOut.c_str());
	ofstream spamFile (spamOut.c_str());
	
	if (allFile.is_open() && benignFile.is_open() && spamFile.is_open()){
		for (int i=0; i<numNodes; i++){
			allFile << i << " " << nodes[i]->degree << " \n";
			if (nodes[i]->identity == 1){ // benign
				benignFile << i << " " << nodes[i]->degree << " \n";
			}
			else if (nodes[i]->identity == -1){ // spam
				spamFile << i << " " << nodes[i]->degree << " \n";
			}
		}
		allFile.close();
		benignFile.close();
		spamFile.close();
	}
}

/* Nodes degree info plus the ratio of spam over benign of neighbors */
void Graph::getDegreeAndRatio(string allOut, string benignOut, string spamOut){
	ofstream allFile (allOut.c_str());
	ofstream benignFile (benignOut.c_str());
	ofstream spamFile (spamOut.c_str());
	
	if (allFile.is_open() && benignFile.is_open() && spamFile.is_open()){
		for (int i=0; i<numNodes; i++){
			int benignCount = 0;
			int spamCount = 0;
			double ratio = 0;
			
			/* Compute ratio */
			for (set<int>::iterator it=nodes[i]->neighbors.begin(); it!=nodes[i]->neighbors.end(); it++){
				if (nodes[*it]->identity == 1){ // benign neighbor
					benignCount ++;
				}
				else if (nodes[*it]->identity == -1){ // spam neighbor
					spamCount ++;
				}
			}
			
			ratio = (double)spamCount / benignCount; 
			/* Some exceptions: 
				0/0 -> -nan
			 	n/0 -> inf
			*/

			/* Output to files */
			allFile << i << " " << nodes[i]->degree << " " << spamCount << " " << benignCount << " " << ratio << " \n";
			if (nodes[i]->identity == 1){ // benign
				benignFile << i << " " << nodes[i]->degree << " " << spamCount << " " << benignCount << " " << ratio << " \n";
			}
			else if (nodes[i]->identity == -1){ // spam
				spamFile << i << " " << nodes[i]->degree << " " << spamCount << " " << benignCount << " " << ratio << " \n";
			}
		}
		allFile.close();
		benignFile.close();
		spamFile.close();
	}
}