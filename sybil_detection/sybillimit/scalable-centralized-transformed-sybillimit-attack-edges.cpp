/* Graph transformation using random walks */
/* A central server is performing the random walks */
/* Written by Prateek Mittal*/ 

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
//#define debug
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
		int transform_wlen;

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
			if(max_vertex_val < node1){
				max_vertex_val = node1;
			}
			if(max_vertex_val <  node2){
				max_vertex_val = node2;
			}

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
		vertex randNeighbor(vertex current){
			//srand(seed);
			//int random = randomInt(max_vertex_val);
			//set<vertex>::iterator iter=vertices.lower_bound(random);
			set<vertex>::iterator iter;
			double sum=0;
			double rand_value = (double) ((float)1.0*rand()/(RAND_MAX+1.0));
			//cout << "Rand Value: " << rand_value << endl;
			for(iter=adjacency[current].begin();iter!=adjacency[current].end();iter++){
				sum=sum+1.0/(double)adjacency[current].size();
				//cout << "Current sum: " << sum << endl;
				if(sum >= rand_value){
					break;
				}
			}
			if(iter==adjacency[current].end()){
				iter=adjacency[current].begin();
			}
			vertex randN = *iter;
			//cout << *iter << endl;
			return randN;
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
		void print_graph(){
			set<vertex>::iterator iter;
		//	cout << "Vertices:" << endl;
		//	for(iter=vertices.begin();iter!=vertices.end();iter++){
		//		cout << *iter << endl;
		//	}
			
		//	set<pair<vertex,vertex> >::iterator edge_iter;
		//	cout << "Edges:" << endl;
		//	for(edge_iter=edges.begin();edge_iter!=edges.end();edge_iter++){
		//		cout << edge_iter->first << "---" << edge_iter->second << endl;
		//	}

			set<vertex>::iterator iter2;
		//	cout << "Adjacency" << endl;
			for(iter=vertices.begin();iter!=vertices.end();iter++){
				cout << *iter;
				for(iter2=adjacency[*iter].begin();iter2!=adjacency[*iter].end();iter2++){
					cout << " " << *iter2;
				}
				cout << endl;	
			}
		}
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
		void transform_graph(class sybillimit *g_new){		
			// perform random walks (along each edge) for each node
			// replace corresponding neighbor with terminus point of random walks
			set<vertex>::iterator iter,iter2;
			vertex prev_hop,cur_hop,next_hop;
			edge tail;
			int random,count_neighbors;
			unordered_map<vertex,vertex> map;		
			// should vertex selection be randomized? 
			int old_edges=0, new_edges=0;
			for(iter=vertices.begin();iter!=vertices.end();iter++){
				if(adjacency[*iter].size()==0){
					// isolated vertices remain isolated
					g_new -> vertices.insert(*iter);
					continue;
				}
				for(iter2=adjacency[*iter].begin();iter2!=adjacency[*iter].end();iter2++){
					// for each neighbor *iter2
					next_hop=*iter2;
					int num_loop=0;
					old_edges++;
					do{
						for(int j=1;j<transform_wlen;j++){
							prev_hop=cur_hop;
							cur_hop=next_hop;
							// perform a random walk from the current hop, and choose the next hop	
							next_hop=randNeighbor(cur_hop);
							//next_hop=map[prev_hop];	
					
						}
						num_loop++;
					}while((next_hop==*iter || g_new->adjacency[*iter].find(next_hop)!=g_new->adjacency[*iter].end()) && num_loop < 5);
					// store the result in a data structure
					// are we storing just the node or the edge?
					//tail = make_pair(cur_hop,next_hop);
					//tail_string = get_string(tail);
					//vertex_tails[*iter].push_back(edge_map[cur_hop][next_hop]);
					// lets not needlessly allocate memory for everyone
					
					// lets add this edge with probability 0.5 (otherwise the degree is going to be doubled)
					// special case for the first edge (want to make sure that atleast one edge in the graph)
					// if(g_new->adjacency[*iter].size()==0){
					// Bug: the above condition may not map to the first edge here
					// since adjacency size may be non zero is someone else setup an edge with me. 

					if(iter2==adjacency[*iter].begin()){
						
						g_new->add_edge(*iter,next_hop);
						g_new->add_edge(next_hop,*iter);
						new_edges++;	
					}
					else{
						double num_degree= (double)adjacency[*iter].size();
					 	double rand_value = (double) ((float)1.0*rand()/(RAND_MAX+1.0));
						if(rand_value <= (0.5*num_degree-1)/(num_degree-1)){
							g_new->add_edge(*iter,next_hop);
							g_new->add_edge(next_hop,*iter);
							new_edges++;
						}
		
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
			g_new-> max_vertex_val = max_vertex_val;
			//cout << "Graph transformed" << endl; 
			//cout << "# Old edges: " <<  old_edges/2.0 << endl; 
			//cout << "# New edges: " <<  new_edges << endl;
			//exit(1); 

		}
		set<vertex> compromise_nodes(int attack_edges){
			int cur_attack_edges=0;
			int new_attack_edges;
			set<vertex> malicious;	
			set<vertex>::iterator iter;
			double rand_seed = (int) ((float)vertices.size()*rand()/(RAND_MAX+1.0));
			while(cur_attack_edges < attack_edges){
				vertex new_vertex = randVertex(rand_seed++);
				if(malicious.find(new_vertex)!=malicious.end()){
					continue;
				}
				else{
					new_attack_edges=adjacency[new_vertex].size();
					for(iter=adjacency[new_vertex].begin(); iter!=adjacency[new_vertex].end();iter++){
						if(malicious.find(*iter)!=malicious.end()){
							new_attack_edges=new_attack_edges-2;
						}
					}
					if(cur_attack_edges+new_attack_edges <= attack_edges+20){
						malicious.insert(new_vertex);
						cur_attack_edges=cur_attack_edges+new_attack_edges;
					}
					else{
						continue;
					}
				}
			}
			return malicious;
		}
		void add_sybils(set<vertex> * malicious, int attack_edges, int sybils_per_edge){
			int m=5;
			int deg_count=0;
			set<vertex>::iterator iter,iter2;
			for(iter=malicious->begin();iter!=malicious->end();iter++){
				deg_count=deg_count+adjacency[*iter].size();
			}
			int max_honest_val=max_vertex_val;
			for(int num_sybils=1; num_sybils <= attack_edges*sybils_per_edge; num_sybils++){
				vertex cur_sybil = max_honest_val+num_sybils;
				// lets insert Sybil identities in a scale free manner
				for(int j=0;j<m;j++){
					// choose a random number less than node degreee
					int random_integer =int(deg_count*rand()/(RAND_MAX + 1.0));	// deg_count = sum of degrees of nodes already added.
					int sum=0;
					int bad_last;
					for(iter=malicious->begin();iter!=malicious->end();iter++){
						if(random_integer < sum){
							break;
						}
						sum=sum+adjacency[*iter].size(); 
						bad_last=*iter;
					}
					if(iter!=malicious->end()){
						add_edge(cur_sybil,*iter);
					}
					else{
						add_edge(cur_sybil,bad_last);
	
					}
				}
				malicious->insert(cur_sybil);
				deg_count=deg_count+2*m;
			}
		}
		
};

void readSocialTopology(char *graph_file);	// read in the social network topology

typedef unsigned int vertex;
typedef pair<vertex, vertex> edge;
class sybillimit g_original,g_transform;

int main(int argc, char *argv[]){
	int seed; // random seed for simulations
	int attack_edges;
	char graph_file[200];  // file name for social graph 
	
	int c;
        while ((c = getopt (argc, argv, "a:g:s:l:t:h:r:")) != -1){
		switch(c){
			case 'g': strncpy(graph_file,optarg,200);
				break;
			case 's':
				g_transform.num_instances=atoi(optarg);
				break;
			case 'l':
				g_original.wlen=atoi(optarg);
				g_transform.wlen=atoi(optarg);
				break;
			case 't':
				g_original.transform_wlen=atoi(optarg);
		
			case 'h':
				g_transform.balance_cond_h=atoi(optarg);
				break;
			case 'r':
				seed=atoi(optarg);
				break;
			case 'a':
				attack_edges=atoi(optarg);
				break;

			default:
				cout << "Usage: sybillimit.exe -g <graph-file> -a <attack edges> -r <rseed> " << endl;
				exit(EXIT_FAILURE);
		}
	}
	#ifdef debug
		cout << "Graph file: " << graph_file << endl;
		cout << "Attack edges: " << attack_edges << endl;
		cout << "Random seed for simulation: " << seed << endl; 
	#endif
	srand(seed);

	readSocialTopology(graph_file);	// read in the social network topology
	
	set<vertex>::iterator iter,iter2;
	for(int count=1; count <= 100; count++){
		// compromise nodes
		set<vertex> original_malicious=g_original.compromise_nodes(attack_edges);
		for(int sybils_per_edge=10; sybils_per_edge <= 10; sybils_per_edge=sybils_per_edge+10){
			class sybillimit * cur_g= new class sybillimit;
			*cur_g=g_original;
			set<vertex> malicious=original_malicious;
			// introduce varying number of Sybils in the system
			cur_g->add_sybils(&malicious,attack_edges,sybils_per_edge);
			// transform topology and compute attack edges
			// we need to vary the transform walk length as well.
			cout << count << " " << attack_edges << " " << sybils_per_edge << " ";
			for(int tlen=2;tlen<=20;tlen++){
				class sybillimit * cur_transform= new class sybillimit;
				cur_g->transform_wlen=tlen;
				cur_g->transform_graph(cur_transform);

				// compute attack edges in the transformed topology
				int transformed_attack_edges=0;
				for(iter=malicious.begin();iter!=malicious.end();iter++){
					for(iter2=cur_transform->adjacency[*iter].begin(); iter2!=cur_transform->adjacency[*iter].end();iter2++){
						if(malicious.find(*iter2)==malicious.end()){
							transformed_attack_edges++;
						}
					}
	
				}		
				cout << transformed_attack_edges << " ";
				delete(cur_transform);
			}
			cout << endl;
			delete(cur_g);
		}
	}	
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
		g_original.add_edge(node1,node2);
	#endif
	#ifdef adjacency_format
		stringstream ss(line);
		if(ss){	
			ss >> node1;
			if(node1 >= g_original.max_vertex_val){
				g_original.max_vertex_val=node1;
			}
			while(ss){	
				ss >> node2;
				g_original.add_edge(node1,node2);
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

