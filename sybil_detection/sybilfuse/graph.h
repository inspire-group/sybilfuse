#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <assert.h>
#include <string.h>
//#include <tr1/unordered_map>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include <sstream>
#include <getopt.h>


//#define min(a,b) (((a) < (b)) ? (a) : (b))
//#define max(a,b) (((a) < (b)) ? (b) : (a))
//#define debug
#define adjacency_format
//#define link_format

using namespace std;
//using namespace std::tr1;

typedef unsigned int vertex;
typedef unsigned int sequential_id;
typedef pair<vertex, vertex> edge; 
typedef unsigned int edge_label;

class graph{
public:
    set<vertex> vertices;	// maintain vertices
    unordered_map<vertex, bool> sybil_status;
    unordered_map<sequential_id, vertex> id_to_v;       // for random sampling of vertices
    unordered_map<vertex, sequential_id> v_to_id; // non-consecutive name to consecutive id

    unordered_map<vertex, set<vertex> > adjacency; 	// friend list per vertex	
    unordered_map<vertex, unordered_map<vertex, unsigned int> > edge_map;
    unsigned int edge_counter;


    
    void add_vertex(vertex node);
    vertex add_random_vertex();
    void add_edge(vertex node1,vertex node2);
    bool add_induced_edge(vertex node1,vertex node2);
//    vertex randVertex(int seed);

    void print_graph(char *graph_out_file);
    void print_ground_truth(char *data_file);
    void print_estimate(char *data_file);
    void read_social_topology(char *graph_file);
};


