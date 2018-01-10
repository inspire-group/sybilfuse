
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
#include <tr1/unordered_map>
#include <algorithm>
#include <math.h>
#include <sstream>
#include <iostream>

using namespace std;
using namespace std::tr1;

// random generator function:
ptrdiff_t myrandom (ptrdiff_t i) { return rand()%i;}

// pointer object to it:
ptrdiff_t (*p_myrandom)(ptrdiff_t) = myrandom;


bool my_comparator ( const pair<double, int>& l, const pair<double, int>& r)
{ 
	return l.first < r.first; 
}


long randint(long min, long max){
    
    return long(rand() / (RAND_MAX + 0.0) * (max - min) + min);
    
}

class Data{
public:
	typedef unsigned long int vertex;
	
	//adjacency list
	set<pair<vertex, vertex> > edges;
		
    unordered_map<vertex, list<vertex> > network_map;
    
    //network file
	char *network_file;
    char *honest_file;
	char *sybil_file;
        
    int* honest_set;
    int* sybil_set;
    
    	
    int total_nodes;
    
	Data(){
		
	}
    
    void add_edge(vertex node1, vertex node2){
		// add edge (node1, node2)
		
		// no self loops
		if(node1==node2){
			return;
		}
		
		//add node2 to the adjacency list of node1
		network_map[node1].push_back(node2);
	}

		
	/* Read in the social graph */
	void read_network(){
		ifstream in(network_file,ifstream::in);
		assert(in);
		
		string line;
		vertex node1,node2;
        
        //read in network
		while(getline(in,line) != NULL){
			node1=(vertex)atol(strtok((char *)line.c_str()," \n\t\r"));
			node2=(vertex)atol(strtok(NULL," \n\t\r"));
            add_edge(node1, node2);
                        
		}
		
		in.close();
		
        total_nodes = network_map.size();
        
        honest_set = (int*)malloc(sizeof(int) * total_nodes);
        sybil_set = (int*)malloc(sizeof(int) * total_nodes);
        
        for (int i = 0; i < total_nodes; i++) {
            honest_set[i] = 0;
            sybil_set[i] = 0;
        }
        ifstream honest_in(honest_file, ifstream::in);
		assert(honest_in);
		while(getline(honest_in,line)!=NULL){
			node1=(vertex)atol(line.c_str());
			honest_set[node1] = 1;
		}
        honest_in.close();
        
        ifstream sybil_in(sybil_file, ifstream::in);
		assert(sybil_in);
		while(getline(sybil_in,line)!=NULL){
			node1=(vertex)atol(line.c_str());
			sybil_set[node1] = 1;
		}
        sybil_in.close();

		
	}
	
	
    void stat(){
        
        //local homophily
        double* local_homo_honest = (double*)malloc(sizeof(double)*total_nodes);
        double* local_homo_sybil = (double*)malloc(sizeof(double)*total_nodes);
        
        int* attack_edge_honest = (int*)malloc(sizeof(int) * total_nodes);
        int* attack_edge_sybil = (int*)malloc(sizeof(int) * total_nodes);
        
        int* intra_deg_honest = (int*)malloc(sizeof(int) * total_nodes);
        int* intra_deg_sybil = (int*)malloc(sizeof(int) * total_nodes);
		
		int* deg_sybil = (int*)malloc(sizeof(int) * total_nodes);
		int* deg_honest = (int*)malloc(sizeof(int) * total_nodes);
        
        int same, diff;
        
        unordered_map<vertex, list<vertex> > :: iterator iter;
        list<vertex>::iterator iter_list;
        
        for (iter = network_map.begin(); iter != network_map.end(); iter++) {
            
            if (honest_set[iter->first] == 1) {
                //honest node
                same = 0;
                diff = 0;
                
                for (iter_list = network_map[iter->first].begin(); iter_list != network_map[iter->first].end(); iter_list++) {
                    if (honest_set[*iter_list] == 1) {
                        same += 1;
                    }
                    if (sybil_set[*iter_list] == 1) {
                        diff += 1;
                    }
                }
                
                local_homo_honest[iter->first] = (same + 0.0) / (same + diff);
                attack_edge_honest[iter->first] = diff;
                intra_deg_honest[iter->first] = same;
				
				deg_honest[iter->first] = (iter->second).size();
            }
            
            if (sybil_set[iter->first] == 1) {
                //sybil node
                same = 0;
                diff = 0;
                
                for (iter_list = network_map[iter->first].begin(); iter_list != network_map[iter->first].end(); iter_list++) {
                    if (honest_set[*iter_list] == 1) {
                        diff += 1;
                    }
                    if (sybil_set[*iter_list] == 1) {
                        same += 1;
                    }
                    
                    local_homo_sybil[iter->first] = (same + 0.0) / (same + diff);
                    attack_edge_sybil[iter->first] = diff;
                    intra_deg_sybil[iter->first] = same;
                }
                
				deg_sybil[iter->first] = (iter->second).size();
            }
        }
        
        //write data
        
        FILE* local_homo_honest_handler = fopen("../result/local_homo_honest.txt", "w");
        FILE* local_homo_sybil_handler = fopen("../result/local_homo_sybil.txt", "w");
        FILE* attack_edge_honest_handler = fopen("../result/attack_edge_honest.txt", "w");
        FILE* attack_edge_sybil_handler = fopen("../result/attack_edge_sybil.txt", "w");
        FILE* intra_deg_honest_handler = fopen("../result/intra_deg_honest.txt", "w");
        FILE* intra_deg_sybil_handler = fopen("../result/intra_deg_sybil.txt", "w");
		FILE* deg_sybil_handler = fopen("../result/deg_sybil.txt", "w");
		FILE* deg_honest_handler = fopen("../result/deg_honest.txt", "w");

        
        for (int i = 0;  i < total_nodes; i++ ) {
            
            if (honest_set[i] == 1) {
                fprintf(local_homo_honest_handler, "%d %f\n", i, local_homo_honest[i]);
                fprintf(attack_edge_honest_handler, "%d %d\n", i, attack_edge_honest[i]);
                fprintf(intra_deg_honest_handler, "%d %d\n", i, intra_deg_honest[i]);
				fprintf(deg_honest_handler, "%d %d\n", i, deg_honest[i]);
            }
            
            if (sybil_set[i] == 1) {
                fprintf(local_homo_sybil_handler, "%d %f\n", i, local_homo_sybil[i]);
                fprintf(attack_edge_sybil_handler, "%d %d\n", i, attack_edge_sybil[i]);
                fprintf(intra_deg_sybil_handler, "%d %d\n", i, intra_deg_sybil[i]);
				fprintf(deg_sybil_handler, "%d %d\n", i, deg_sybil[i]);
            }
        }
        
        fclose(local_homo_honest_handler);
        fclose(local_homo_sybil_handler);
        fclose(attack_edge_honest_handler);
        fclose(attack_edge_sybil_handler);
        fclose(intra_deg_honest_handler);
        fclose(intra_deg_sybil_handler);
		fclose(deg_honest_handler);
		fclose(deg_sybil_handler);
        
        
        //find the connected sybil region
        set<vertex> visited_nodes;
        list<unordered_map<vertex, set<vertex> > > components;
        vertex node;
        list<vertex> q;
        unordered_map<vertex, set<vertex> > c;
        
        for (int i = 0; i < total_nodes; i++) {
            if (sybil_set[i] == 0 || visited_nodes.find(i) != visited_nodes.end()) {
                continue;
            }
            c.clear();
            q.push_back(i);
            visited_nodes.insert(i);
            
            while (!q.empty()) {
                    
                node = q.front();
                q.pop_front();
                
                for (iter_list = network_map[node].begin(); iter_list != network_map[node].end(); iter_list++) {
                    if (sybil_set[*iter_list] == 1) {
                        //sybil node
                        if (visited_nodes.find(*iter_list) == visited_nodes.end()) {
                            q.push_back(*iter_list);
                            visited_nodes.insert(*iter_list);
                        }
                        
                        //add edge
                        c[node].insert(*iter_list);
                        c[*iter_list].insert(node);
                        
                    }
                }
                
            }
            
            components.push_back(c);
            
        }
		
		//component size distribution
		FILE* component_handler = fopen("../result/component-size-distribution.txt", "w");
        
        int max_c = 0;
        list<unordered_map<vertex, set<vertex> > > :: iterator iter_c;
        for (iter_c = components.begin(); iter_c != components.end(); iter_c++) {
			fprintf(component_handler, "%d ", (*iter_c).size());
            if ((*iter_c).size() > max_c) {
                max_c = (*iter_c).size();
                c = *iter_c;
            }
        }
        
        
        FILE* com_handler = fopen("../result/connected-sybil-region.txt", "w");
        
        unordered_map<vertex, set<vertex> > ::iterator iter_un;
        set<vertex>::iterator iter_set;
        
        int num_edges = 0;
        for (iter_un = c.begin(); iter_un != c.end(); iter_un++) {
            num_edges += (iter_un->second).size();
        }
        
        num_edges /= 2;
        fprintf(com_handler, "#num of nodes: %d\n", max_c);
        fprintf(com_handler, "#num of edges: %d\n", num_edges);
        fprintf(com_handler, "#num of components: %d\n", components.size());
        
        for (iter_un = c.begin(); iter_un != c.end(); iter_un++) {
            for (iter_set = (iter_un->second).begin(); iter_set != (iter_un->second).end(); iter_set++) {
                fprintf(com_handler, "%d %d\n", iter_un->first, *iter_set);
            }
        }
        
        fclose(com_handler);
		fclose(component_handler);
		
		

    }
    
    void parse_par(int argc, char **argv){
        
        //default setting
        network_file = "../data/net.txt";
        honest_file = "../data/active.txt";
        sybil_file = "../data/suspended.txt";
        
        
		
        int i = 1;
        
        while (i < argc) {
            if (strcmp(argv[i],"-net") == 0){
                network_file = argv[i + 1];
            }
			else if (strcmp(argv[i],"-h") == 0){
                honest_file = argv[i + 1];
            }
			else if (strcmp(argv[i],"-s") == 0){
                sybil_file = argv[i + 1];
            }			
            else{
                cout << "undefined inputs: " << argv[i] <<endl;
				exit(0);
            }
            
            i += 2;
        }
        
    }
	
};


int main (int argc, char **argv)
{	
		
	srand ( time(NULL) );
	Data data;
    
    data.parse_par(argc, argv);
	
	data.read_network();
    data.stat();
    
	return 0;
	
}