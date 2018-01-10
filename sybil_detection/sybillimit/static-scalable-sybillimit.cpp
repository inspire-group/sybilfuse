/* A version of SybilLimit optimized for memory/performance scalability, and suitable for static social graphs */
/* Written by Prateek Mittal, using ideas in Bimal Vishwanath's code */ 

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <assert.h>
#include <string.h>
#include <tr1/unordered_map>
#include <algorithm>
#include <math.h>
#include <sstream>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) < (b)) ? (b) : (a))
#define debug
#define adjacency_format
//#define link_format

using namespace std;
using namespace std::tr1;

int randomInt(int range); // random integer between [0, range]

class graph{
	public:
		typedef unsigned int vertex;
		typedef pair<vertex, vertex> edge; 
		typedef unsigned int edge_label;
	

		set<vertex> vertices;	// maintain vertices
		int max_vertex_val; 	// maximum vertex value (useful for random vertex sampling)

		//set<pair<vertex,vertex> > edges;	// maintain edges 
		unordered_map<vertex,set<vertex> > adjacency; 	// friend list per vertex	
		unordered_map<vertex, unordered_map<vertex, unsigned int> > edge_map;
		unsigned int edge_counter;

		graph(){
			max_vertex_val=0;
		}
		void add_vertex(vertex node1){
			vertices.insert(node1);
		}
		void add_edge(vertex node1,vertex node2){
			// add edge node1, node2
			
			// ensure node1, node2 in the vertex set
			// can simply insert in vertex set
			// if they already exist, no harm
			vertices.insert(node1);	
			vertices.insert(node2);

			// no self loops
			if(node1==node2){
				return;
			}

			// what if this already exists? 
			//edge e1=make_pair(node1,node2);
			//edge e2=make_pair(node2,node1); 
			//if(edges.find(e2)==edges.end()){
			//	edges.insert(e1);
			//}		
						
			// add this in the adjacency list for both nodes 
			// graph is undirected
			if(adjacency[node1].find(node2)==adjacency[node1].end()){
				adjacency[node1].insert(node2);
				adjacency[node2].insert(node1);
				edge_map[node1][node2]=edge_counter;
				edge_counter++;
				edge_map[node2][node1]=edge_counter;
				edge_counter++;
			}
		}
		vertex randVertex(int seed){
			srand(seed);
			int random = randomInt(max_vertex_val);
			set<vertex>::iterator iter=vertices.lower_bound(random);
			if(iter==vertices.end()){
				iter=vertices.begin();
			}
			return *iter;
		}
		/*void delete_vertex(vertex node1){
			// remove all edges involving vertex
			set<vertex>::iterator iter;
			for(iter=adjacency[node1].begin();iter!=adjacency[node1].end();iter++){
				delete_edge(node1,*iter);
			}
			
			// remove adjacency for vertex
			adjacency[node1].clear();
			
			// remove from vertex set
			vertices.erase(node1);
		}
		void delete_edge(vertex node1,vertex node2){
			// delete edge node1-node2
			edges.erase(make_pair(node1,node2));
			edges.erase(make_pair(node2,node1));
			// only one of the above will succeed
			
			// remove edge from adjacency list
			adjacency[node1].erase(node2);
			adjacency[node2].erase(node1);

			// in case node 1 or node 2 have no edges left
			// then we could remove those vertices from the graph?
		}*/
		/*void print_graph(){
			set<vertex>::iterator iter;
			cout << "Vertices:" << endl;
			for(iter=vertices.begin();iter!=vertices.end();iter++){
				cout << *iter << endl;
			}
			
			set<pair<vertex,vertex> >::iterator edge_iter;
			cout << "Edges:" << endl;
			for(edge_iter=edges.begin();edge_iter!=edges.end();edge_iter++){
				cout << edge_iter->first << "---" << edge_iter->second << endl;
			}

			set<vertex>::iterator iter2;
			cout << "Adjacency" << endl;
			for(iter=vertices.begin();iter!=vertices.end();iter++){
				cout << *iter;
				for(iter2=adjacency[*iter].begin();iter2!=adjacency[*iter].end();iter2++){
					cout << " " << *iter2;
				}
				cout << endl;	
			}
		}*/
		/*edge_string get_string(edge e1){
			//string line;
			//stringstream ss_line(line);
			//ss_line << e1.first;
			//ss_line << " ";
			//ss_line << e1.second;
			//return ss_line.str();
			return edge_to_string_map[e1];
		}
		edge get_edge(edge_string line){
			stringstream ss_line(line);
			edge e1;
			ss_line >> e1.first;
			ss_line >> e1.second;
			return e1;
		}*/
};

class sybillimit: public graph{
	public:
		int num_instances; 	// number of sybillimit instances
		int wlen;		// length of random walk
		int balance_cond_h;	// balance condition parameter

		// for each node, there are multiple Sybillimit permutations
		typedef unordered_map<vertex,vertex> permutation;
		unordered_map<vertex,vector<permutation> > permutation_map;	
		
		// each edge can maintain multiple registration
		// we need to know which instance to overwrite 	
		unordered_map<vertex,vector<edge_label> > vertex_tails;
		// each tail has a counter for the balance condition
		unordered_map<edge_label,int> verifier_tails_counter;

		//unordered_map<edge_string,multiset<vertex> > tails_registration;
		//unordered_map<edge_string,unordered_map<int,vertex> > tails_registration_instance;

		void generateRandomPermutations(){	// generate multiple permutation instances for each node in the system
			permutation_map.clear();
			set<vertex>::iterator iter;
			for(iter=vertices.begin();iter!=vertices.end();iter++){
				if(adjacency[*iter].size()==0){
					continue;
				}
				vector<vertex> adjacent_nodes,permuted_nodes;
				set<vertex>::iterator iter2;
				vertex temp;
				for(iter2=adjacency[*iter].begin();iter2!=adjacency[*iter].end();iter2++){
					adjacent_nodes.push_back(*iter2);
					permuted_nodes.push_back(*iter2);
				}
				//for(int i=0;i<num_instances;i++){	
					// need to instantiate a permutation now
					permutation table;
					for(int j=adjacent_nodes.size();j>=1;--j){
						int random=randomInt(j);
						table[adjacent_nodes[j-1]]=permuted_nodes[random];
						temp=permuted_nodes[j-1];
						permuted_nodes[j-1]=permuted_nodes[random];
						permuted_nodes[random]=temp;
					}
					permutation_map[*iter].push_back(table);
				//}
			}			
		}	
		void generateRandomRoutes(vertex verifier){		// generate multiple random route for each node

			set<vertex>::iterator iter,iter2;
			vertex prev_hop,cur_hop,next_hop;
			edge tail;
			//edge_string tail_string;
			int random,count_neighbors;
			unordered_map<vertex,vertex> map;		
			// should vertex selection be randomized? 
			for(int i=0;i<num_instances;i++){
				generateRandomPermutations();
				for(iter=vertices.begin();iter!=vertices.end();iter++){
					if(adjacency[*iter].size()==0){
						continue;
					}
					//for(int i=0;i<num_instances;i++){
					cur_hop=*iter;
					// lets select a random neighbor
					// next_hop;
					random=randomInt(adjacency[*iter].size());
					iter2=adjacency[*iter].begin();
					for(count_neighbors=0;count_neighbors<random;count_neighbors++){
						iter2++;
					}
					next_hop=*iter2;
					for(int j=1;j<wlen;j++){
						prev_hop=cur_hop;
						cur_hop=next_hop;
						map=permutation_map[cur_hop][0];
						next_hop=map[prev_hop];	
					//	next_hop=permutation_map[cur_hop][i][prev_hop];	
					}
					// store the result in a data structure
					// are we storing just the node or the edge?
					//tail = make_pair(cur_hop,next_hop);
					//tail_string = get_string(tail);
					vertex_tails[*iter].push_back(edge_map[cur_hop][next_hop]);
					// lets not needlessly allocate memory for everyone
					if(*iter==verifier){
						verifier_tails_counter[edge_map[cur_hop][next_hop]]=0;
					}
					//if someone else gets this tail, then i have wrong information
					//tails register public keys
					
					//if(tails_registration_instance[tail_string].find(i)!=tails_registration_instance[tail_string].end()){
						// delete old key
					//	vertex old = tails_registration_instance[tail_string].find(i)->second;
					//	set<vertex>::iterator old_iter= tails_registration[tail_string].find(old);
					//	tails_registration[tail_string].erase(old_iter);
					//}
					//tails_registration_instance[tail_string][i]=*iter;
					//tails_registration[tail_string].insert(*iter);
					
					// unique i per tail
					// need to find nodes in this structure efficiently too
				//}
				//assert(vertex_tails[*iter].size()==num_instances);
				}
			}
			permutation_map.clear();	
		}
		void generateSrcRandomRoutes(vertex source, bool verifier, int seed){
			set<vertex>::iterator iter,iter2;
			vertex prev_hop,cur_hop,next_hop;
			edge tail;
			int random,count_neighbors;
			unordered_map<vertex,vertex> map;		
			for(int i=0;i<num_instances;i++){
				//generateRandomPermutations();
				if(adjacency[source].size()==0){
					continue;
				}
				cur_hop=source;
				// lets select a random neighbor
				// next_hop;
				random=randomInt(adjacency[source].size());
				iter=adjacency[source].begin();
				for(count_neighbors=0;count_neighbors<random;count_neighbors++){
					iter++;
				}
				next_hop=*iter;
				for(int j=1;j<wlen;j++){
					prev_hop=cur_hop;
					cur_hop=next_hop;
					// we need to generate this mapping
					//map=permutation_map[cur_hop][0];	// lets generate these permutation mappings on the fly, so we don't have to store them.
					vector<vertex> adjacent_nodes,permuted_nodes;
					vertex temp;
					for(iter2=adjacency[cur_hop].begin();iter2!=adjacency[cur_hop].end();iter2++){
						adjacent_nodes.push_back(*iter2);
						permuted_nodes.push_back(*iter2);
					}
					// need to instantiate a permutation now
					srand(seed+i+cur_hop);
					for(int j=adjacent_nodes.size();j>=1;--j){
						int random=randomInt(j);
						//table[adjacent_nodes[j-1]]=permuted_nodes[random];
						if(adjacent_nodes[j-1]==prev_hop){
							next_hop=permuted_nodes[random];
							break;
						}
						temp=permuted_nodes[j-1];
						permuted_nodes[j-1]=permuted_nodes[random];
						permuted_nodes[random]=temp;
					}
				}
				vertex_tails[source].push_back(edge_map[cur_hop][next_hop]);
				// lets not needlessly allocate memory for everyone
				if(verifier){
					verifier_tails_counter[edge_map[cur_hop][next_hop]]=0;
				}
			}
		}
		void deleteSrcRandomRoutes(vertex source){
			vertex_tails[source].clear();	// save memory
		}

		bool verify(vertex verifier, vertex suspect){
			// need intersection condition and balance condition
			set<edge_label> v1,v2,intersection;
			vector<edge_label>::iterator edge_iter;
			/*cout << "verifier: " << verifier << endl;
			for(edge_iter=vertex_tails[verifier].begin();edge_iter!=vertex_tails[verifier].end();edge_iter++){
				cout << *edge_iter << endl;
			}
			cout << "suspect: " << suspect << endl;
			for(edge_iter=vertex_tails[suspect].begin();edge_iter!=vertex_tails[suspect].end();edge_iter++){
				cout << *edge_iter << endl;
			}*/

			copy(vertex_tails[verifier].begin(),vertex_tails[verifier].end(),inserter(v1,v1.end()));
			copy(vertex_tails[suspect].begin(),vertex_tails[suspect].end(),inserter(v2,v2.end()));
					
			set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),inserter(intersection,intersection.end()));
			//set_intersection(vertex_tails[verifier].begin(),vertex_tails[verifier].end(),vertex_tails[suspect].begin(),vertex_tails[suspect].end(),inserter(intersection,intersection.end()));
			//cout << intersection.size() << endl;
			//exit(1);
			//bool result=true;
			//return result;
			if(intersection.size()==0){	// lets check intersection condition
				return false;
			}
			
			//set<edge_string> intersection_set;
			//for(edge_iter=intersection.begin();edge_iter!=intersection.end();edge_iter++){
			//	intersection_set.insert(*edge_iter);
			//}
			// I guess we also need to check registration at the tails	
			// I could skip this for now, since I have global view so tails are trustworthy
			// Would be nice to eventually get this correct
			int counter_sum=0;
			
			edge_label cmin_label=*(intersection.begin());
			int cmin=verifier_tails_counter[cmin_label];

			unordered_map<edge_label,int>::iterator iter;
			bool check_if=false;
			for(iter=verifier_tails_counter.begin();iter!=verifier_tails_counter.end();iter++){
				counter_sum=counter_sum + iter->second;
				if(intersection.find(iter->first)==intersection.end()){
					continue;
				}
				if(iter->second <= cmin){
					check_if=true;
					cmin=iter->second;
					cmin_label=iter->first;
				}
			}
			if(!check_if){
				cout << "suspect  = " << suspect << endl;
			}
			assert(check_if);
			
			int r=verifier_tails_counter.size();
			double a=(double)(1.0+ counter_sum)/(double)r;
			double b=(double)balance_cond_h*(double)max(log((double)r),a);
			// should this be log2? 
			if(((double)cmin+1.0)>b){
				return false;
			}else{
				verifier_tails_counter[cmin_label]=verifier_tails_counter[cmin_label]+1;
				return true;
			}
			intersection.clear();
		}
		
		
};

void readSocialTopology(char *graph_file);	// read in the social network topology

typedef unsigned int vertex;
typedef pair<vertex, vertex> edge;
class sybillimit g;

int main(int argc, char *argv[]){
	int seed; // random seed for simulations
	char graph_file[200];  // file name for social graph 
	
	int c;
        while ((c = getopt (argc, argv, "g:s:l:h:r:")) != -1){
		switch(c){
			case 'g': strncpy(graph_file,optarg,200);
				break;
			case 's':
				g.num_instances=atoi(optarg);
				break;
			case 'l':
				g.wlen=atoi(optarg);
				break;
			case 'h':
				g.balance_cond_h=atoi(optarg);
				break;
			case 'r':
				seed=atoi(optarg);
				break;
			default:
				cout << "Usage: sybillimit.exe -g <graph-file> -s <num-s-instances> -l <walk-lengh> -h <balance-cond-hval> -r <rseed> " << endl;
				exit(EXIT_FAILURE);
		}
	}
	#ifdef debug
		cout << "Graph file: " << graph_file << endl;
		cout << "Number of SybilLimit instances: " << g.num_instances << endl;
		cout << "Length of random walks: " << g.wlen << endl;
		cout << "Balance condition h parameter: " << g.balance_cond_h << endl;
		cout << "Random seed for simulation: " << seed << endl; 
	#endif
	assert(g.num_instances > 0);
	assert(g.wlen >0 && g.wlen <= 100);
	srand(seed);

	/*vector<string> s1,s2;
	s1.push_back("1 2");
	s1.push_back("2 3");
	s2.push_back("3 4");
	s2.push_back("1 2");
	set<string> s3,s4;
	copy(s1.begin(),s1.end(),inserter(s3,s3.end()));
	copy(s2.begin(),s2.end(),inserter(s4,s4.end()));
	
	set<string> intersection;
	set_intersection(s3.begin(),s3.end(),s4.begin(),s4.end(),inserter(intersection,intersection.end()));
	// problem: this is set intersection, and not vector intersection
	cout << (int)intersection.size() << endl;
	exit(1);*/
	
	readSocialTopology(graph_file);	// read in the social network topology
	//g.print_graph();
	//g.generateRandomPermutations();	// generate multiple permutation instances for each node in the system
	//cout << "generated_permutations" << endl;

	set<vertex>::iterator iter;
	vertex verifier;
	/*int cur_count=0;
	int rand_count=randomInt(g.vertices.size());
	for(iter=g.vertices.begin();iter!=g.vertices.end();iter++){
		if(rand_count==cur_count){
			verifier=*iter;
			break;
		}
		cur_count++;
	}*/	
	//iter=g.vertices.begin();
	//verifier=*iter;

	//g.generateRandomRoutes(verifier);	// generate multiple random route for each node
	//cout << "generated random routes" << endl;
	/*unordered_map<vertex,vertex>::iterator map_iter;
	vertex v1=*(g.vertices.begin());
	v1=1;	
	for(int i=0;i<g.num_instances;i++){
		for(map_iter=g.permutation_map[v1][i].begin();map_iter!=g.permutation_map[v1][i].end();map_iter++){
			cout << map_iter->first << " ---> " << map_iter->second << endl;
		}
		cout << endl << endl;
	}*/

	// now we need to add functionality for node addition/edge addition in the system
	// need to study what changes? 

	int count=0;
	int num_suspects=0;
	int max_suspects=100;
	int max_verifiers=1000;
	for(int num_verifiers=0;num_verifiers<max_verifiers;num_verifiers++){	
		verifier=g.randVertex(seed*num_verifiers);
		cout << "Verifier: " << verifier << endl;

		//int max_suspects=g.vertices.size();
		g.generateSrcRandomRoutes(verifier,true,verifier);
		//for(iter=g.vertices.begin();iter!=g.vertices.end();iter++){
		for(int i=0;i<max_suspects;i++){
			num_suspects++;
			cout << "Validating suspect number: " << num_suspects << endl;
			//vertex suspect=*iter;
			vertex suspect = g.randVertex(i+seed+(i+1)*seed);
			if(verifier==suspect){
				count++;
				continue;
			}
			g.generateSrcRandomRoutes(suspect,false,suspect);
			bool result=g.verify(verifier,suspect);
			g.deleteSrcRandomRoutes(suspect);
			if(result){
				count++;
			}
			else{
				cout << "FP: " <<  suspect << endl;
			}
			//if(num_suspects==max_suspects){
			//	break;
			//}
		}
	}
	cout << "Number of verified nodes: " << count << endl;
	cout << "False positive percentage: " << (100*(max_suspects*max_verifiers-count))/(double)(max_suspects*max_verifiers) << endl;
	// verification code --intersection condition and balance condition
	// what i want to do is introduce an edge in the system, and see how many new tails do I get?
	// then for some time, I will have access to all the tails. 
	// More generally, I will obtain new identities over time, so I can "burn" old identities
	
	// algo will be: add a new edge (either via old node or new node in the system)
	// all num_instances' permutations will change: probability that attacker's tail is impacted given it traverses the node: 1/d? 
	// num_instances/d new tails (around 100 new tails)

	return 0;
}

/* Read in the social graph */
void readSocialTopology(char *graph_file){
	ifstream in(graph_file,ifstream::in);
	assert(in);

	string line;
	vertex node1,node2;
	while(getline(in,line)!=NULL){
	#ifdef link_format
		node1=(unsigned long int)atol(strtok((char *)line.c_str()," \n\t\r"));
		node2=(unsigned long int)atol(strtok(NULL," \n\t\r"));
		g.add_edge(node1,node2);
	#endif
	#ifdef adjacency_format
		stringstream ss(line);
		if(ss){	
			ss >> node1;
			if(node1 >= g.max_vertex_val){
				g.max_vertex_val=node1;
			}
			while(ss){	
				ss >> node2;
				g.add_edge(node1,node2);
			}
		}
	#endif
	}
	in.close();

}

int randomInt(int range){  // random integer between [0,range)
	int random_no = (int) ((float)range*rand()/(RAND_MAX+1.0));
	return random_no;
}

