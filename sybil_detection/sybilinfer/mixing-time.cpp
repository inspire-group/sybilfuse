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
		vertex randVertex(){
			// Commenting since we are not running Sybillimit
			//srand(seed);
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

		/*void compute_transient_dist(vertex initiator){

			// If we want to run this code for very large graphs, it would be better to have node vertices be consecutive, and use vertex label as index into an array. Currently, I am using unordered_map, which should be goodenough for small to moderate scale topologies 

			unordered_map<vertex,long double> prob_vector;
			unordered_map<vertex,long double> new_prob_vector;
			
			set<vertex>::iterator iter,iter2;
	
 		       	for(iter=vertices.begin();iter!=vertices.end();iter++){
       		        	prob_vector[*iter]=0;
    		        	new_prob_vector[*iter]=0;
         		}
           		prob_vector[initiator]=1;
		
			for(int l=0;l<max_walk_length;l++){
     		        	// now, we need to multiply the prob vector with the transition probability$
       		                // row into column multiplication -- for each column, we have a value. 
      		        	for(iter=vertices.begin();iter!=vertices.end();iter++){
        		        	// need to multiple the prob vector with the i'th column. 
                       		        // the i'th column can be computed using p[i]
              		                new_prob_vector[*iter]=0;
					for(iter2=adjacency[*iter].begin();iter2!=adjacency[*iter].end();iter2++){
                               		         // here, we will compute the result for new_prob_vector[i]
                   		                 // lets first consider normal random walks
                                       		 new_prob_vector[*iter] = new_prob_vector[*iter] + prob_vector[*iter2]*(1.0/((double)adjacency[*iter2].size()));
                                	}
                        	}
                        	for(iter=vertices.begin();iter!=vertices.end();iter++){
                               		prob_vector[*iter]=new_prob_vector[*iter];
                        	}
			}
		}*/
	
		
};

class transform: public graph{
	public:
		void transform_graph(class transform *g_new){		
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
						next_hop=*iter2;
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
				}
			}
			g_new-> max_vertex_val = max_vertex_val;
			//cout << "Graph transformed" << endl; 
			//cout << "# Old edges: " <<  old_edges/2.0 << endl; 
			//cout << "# New edges: " <<  new_edges << endl;
			//exit(1); 

		}
};

void readSocialTopology(char *graph_file);	// read in the social network topology

typedef unsigned int vertex;
typedef pair<vertex, vertex> edge;
class transform g_original;

int main(int argc, char *argv[]){
	int seed; // random seed for simulations
	char graph_file[200];  // file name for social graph 
	
	int c;
        while ((c = getopt (argc, argv, "g:t:r:")) != -1){
		switch(c){
			case 'g': strncpy(graph_file,optarg,200);
				break;
			case 't': g_original.transform_wlen=atoi(optarg);
				break;
			case 'r':
				seed=atoi(optarg);
				break;
			default:
				cout << "Usage: ./a.out -g <graph-file> -t <transform_walk_length> -r <rseed> " << endl;
				exit(EXIT_FAILURE);
		}
	}
	#ifdef debug
		cout << "Graph file: " << graph_file << endl;
		cout << "Transform_walk_length:" << g_original.transform_wlen << endl;
		cout << "Random seed for simulation: " << seed << endl; 
	#endif
	srand(seed);

	readSocialTopology(graph_file);	// read in the social network topology

	vertex rand_vertex;
	set<vertex>::iterator iter,iter2;

	int original_edges=0;
	for(iter=g_original.vertices.begin();iter!=g_original.vertices.end();iter++){
		original_edges=original_edges+g_original.adjacency[*iter].size();
	}
	original_edges=original_edges/2;
	
	int num_samples=1000;
	const int max_walk_length=50;
	int num_nodes=g_original.vertices.size();		
	const int max_graphs=1;

	unordered_map<vertex,long double> prob_vector;
	unordered_map<vertex,long double> new_prob_vector;
	unordered_map<vertex,long double>::iterator miter;

	double result[max_walk_length][max_graphs];
	int bad_samples =0;
	for(int num_graph=0; num_graph < max_graphs; num_graph++){
		
		class transform * g_transform = new class transform;
		g_original.transform_graph(g_transform);
		
		int transformed_edges=0;
		for(iter=g_transform->vertices.begin();iter!=g_transform->vertices.end();iter++){
			transformed_edges = transformed_edges + g_transform->adjacency[*iter].size();
		}
		transformed_edges=transformed_edges/2.0;

		double worst[max_walk_length];
		for(int l=0; l< max_walk_length;l++){
			worst[l]=0;
		}
	        for(int sample=0;sample<num_samples;sample++){
			//cout << num_graph << " " << sample << endl;
			prob_vector.clear();
			new_prob_vector.clear();
			
                	// select a random node
			vertex start = g_original.randVertex();
			prob_vector[start] = 1;
		
			double sample_worst[max_walk_length];
			for(int l=0; l< max_walk_length;l++){
				sample_worst[l]=0;
			}

			for(int l=0;l<max_walk_length;l++){
				new_prob_vector.clear();
				for(miter=prob_vector.begin();miter!=prob_vector.end();miter++){
					for(iter=g_transform->adjacency[miter->first].begin();iter!=g_transform->adjacency[miter->first].end();iter++){
						new_prob_vector[*iter]=new_prob_vector[*iter]+prob_vector[miter->first]*(1.0/((double)g_transform->adjacency[miter->first].size()));
					}
				}
				prob_vector.clear();
				prob_vector=new_prob_vector;

				// now we can compute and record the total variation distance
				long double total_variation_distance=0;
				long double stationary_prob_vertex;
				long double diff;
				
				for(iter=g_transform->vertices.begin();iter!=g_transform->vertices.end();iter++){
					stationary_prob_vertex = (double)g_transform->adjacency[*iter].size()/((double)transformed_edges*2);	
					if( stationary_prob_vertex > prob_vector[*iter]){
       	                         	//total_variation_distance=total_variation_distance+0.5*(stationary_prob_vertex-prob_vector[*iter]);
						diff=stationary_prob_vertex-prob_vector[*iter];
					}else{
						//total_variation_distance=total_variation_distance+0.5*(prob_vector[*iter]-stationary_prob_vertex);
						diff=prob_vector[*iter]-stationary_prob_vertex;
					}
					if(diff > total_variation_distance){
						total_variation_distance=diff;
					}
       		                 }
				sample_worst[l]=total_variation_distance;
			}
			double count=0;
			for(iter=g_transform->vertices.begin();iter!=g_transform->vertices.end();iter++){
				if(prob_vector[*iter] > 0){
					count++;
				}
			}
			// if this was a good sample, then..
			if(count > 1000){
				for(int l=0; l< max_walk_length; l++){
					if(worst[l] < sample_worst[l]){
						worst[l] = sample_worst[l];
					}
				}
			}
			else{
				bad_samples++;
			}

		}
		for(int l=0; l < max_walk_length; l++){
			result[l][num_graph]=worst[l]; 
		}
		delete(g_transform);
	}
	
	for(int l=0; l< max_walk_length; l++){	
		cout << l << " " << bad_samples;
		for(int num_graph=0; num_graph < max_graphs; num_graph++){
			cout << " " << result[l][num_graph]; 
		}
		cout << endl;
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

