/* Node class
author: Peng Gao
pgao@princeton.edu */

#ifndef NODE_H
#define NODE_H

#include <set>
//#include <vector>

using namespace std;

class Node{
	int index;
	int degree;
	int identity; // 0 for delete, 1 for act, -1 for suspend
	set<int> neighbors;

//	vector<int>neighbors;
public:
	Node();
	
	friend class Graph;
};

#endif

