/*
 Implementation of SybilFuse algorithm.

 The code was written by Neil Zhenqiang Gong, in September 2012, and modified by Peng Gao and Binghui Wang in 2016
 */

#include "graph.h"


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
#include <algorithm>
#include <math.h>
#include <sstream>
#include <iostream>
#include <iomanip>

#define GCC_VERSION (__GNUC__ * 10000 \
+ __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#if GCC_VERSION >= 40300
#include <tr1/unordered_map>
using namespace std::tr1;
#define hash_map unordered_map

#else
#include <unordered_map>
#endif
using namespace std;


// random generator function:
ptrdiff_t myrandom (ptrdiff_t i) { return rand()%i;}

// pointer object to it:
ptrdiff_t (*p_myrandom)(ptrdiff_t) = myrandom;

long randint(long min, long max){
    
    return long(rand() / (RAND_MAX + 0.0) * (max - min) + min);
    
}

// Main SybilFuse class
class SybilFuse {
public:
	// Social graph
	class graph g;

	// Fields specific to SF
	typedef unsigned long int vertex;
	typedef unsigned int sequential_id;
	
    //total number of nodes
    long N;  
    
	//adjacency list
    //weighted graph
	unordered_map<vertex,list<pair<vertex, double> > > network_map;
    
    //1 : weighted, 0: unweighted
    int weighted_graph;
    
	//messages, each message sum to 1
    unordered_map<string, double> messages_map;
    unordered_map<string, double> messages_map_new;
    
    
    //priors: initial probability of being benign for each node
    double *prior;

    //posteriors: posterior probability of being benign for each node  
    //during computation, post is treated as the normalized multiplication of incoming messages of each node.
    double *post;
    
    //network file
	char *network_file;
	
    //posterior file
    char *post_file;

    //prior file 
    char *prior_file;
    
    //labeled training set file
	char *train_set_file;

	// output json file (binary prediction)
	char *prediction_out_file;
    
	//parameters of PMR model when input is a set of labeled nodes, instead of priors
	double theta_pos; // positive nodes
    double theta_neg; // negative nodes
    double theta_unl; // unlabeled nodes
    
    //this weight is used when the input is an unweighted graph
    double weight;
    
    //number of maximum iterations
    double max_iter;
	

    SybilFuse(){
        
	}


	/* Store edges and weights in the network_map */
	void add_edge(vertex node1, vertex node2, double w){
		
		// no self loops
		if(node1==node2){
			return;
		}
		
		//add node2 to the adjacency list of node1
        pair <vertex, double> nei (node2, w);
		network_map[node1].push_back(nei);
	}


	/* Read the social graph */
    //the format for the social graph is
    //each line corresponds to an edge, e.g, 3 2 0.8
    //each edge in the graph appears twice, e.g.,
    //3 2 0.8
    //2 3 0.8
    void read_network(){
    	// Use the function in the Graph.cpp class to read the social network file
    	// Notice that the vertex IDs may not be consecutive.
    	g.read_social_topology(network_file);     // read in the social network topology
    	cout << "graph file read" << endl;

    	// Transform the network format in g into the format supported by SF
    	set<vertex>::iterator iter;  
    	set<vertex>::iterator iter2;
    	vertex node1, node2; 
    	vertex node1_id, node2_id;
    	double w;
    	for (auto iter = g.vertices.begin(); iter != g.vertices.end(); iter++) {
    		node1 = *iter;
    		for (auto iter2 = g.adjacency[node1].begin(); iter2 != g.adjacency[node1].end(); iter2++) {
    			node2 = *iter2;

    			// Map the non-consecutive vertex name to consecutive vertex id
    			node1_id = convert_id(node1);
    			node2_id = convert_id(node2);

				if (weighted_graph == 1) {
					w=(double)atof(strtok(NULL," \n\t\r")); // ignore this line for now
				}
				else{
					w = weight;
				}

				add_edge(node1_id, node2_id, w);
    		}
    	}
      
        //number of nodes in the graph
        N = network_map.size();
        
        //allocate space for final posterior scores
        post = (double*) malloc(sizeof(double)*(N));
        
        //allocate space for prior scores
        prior = (double*) malloc(sizeof(double)*(N));
    }

    // Convert non-consecutive vertex name to consecutive vertexID
    vertex convert_id(vertex name) {
    	return (vertex) g.v_to_id[name];
    }


    /* Read priorfile or/and training file */
    void read_prior(){
        
        //initialize node priors as theta_unl
        vertex node;
        for (node = 0; node < N; node++) {
            prior[node] = theta_unl;
        }
        
        //reassign node priors if there exists the priorfile 
        if (prior_file != "") { // ignore this block for now

            ifstream in(prior_file,ifstream::in);
            assert(in);
            
            string line;
            
            double score;

            while(getline(in,line)){

                node=(vertex)atol(strtok((char *)line.c_str()," \n\t\r"));
                score=(double)atof(strtok(NULL," \n\t\r"));
                prior[node] = score;
            }
            in.close();
        }
        
        //reassign node priors for labeled benign (positive) nodes or/and Sybils (negative nodes) 
        if (train_set_file != ""){

            ifstream in(train_set_file,ifstream::in);
            assert(in);
            
            string line;
            
            //reading labeled benign nodes
            getline(in,line);
            istringstream pos_train_str(line);
            vertex sub;
            while (pos_train_str){
                pos_train_str >> sub;

                // Convert ID
                prior[convert_id(sub)] = theta_pos;    
            }
            
            //reading labeled Sybils
            getline(in,line);
            istringstream neg_train_str(line);
            while (neg_train_str){
                neg_train_str >> sub;
                prior[convert_id(sub)] = theta_neg;                   
            }
            
            in.close();
        }
        
    }


    string construct_id (vertex sender, vertex receiver){
        
        string id;
        ostringstream convert;
        
        convert << sender;
        convert << " ";
        convert << receiver;
        
        id = convert.str();
        
        return id;
        
    }


    /* Initialize all edges the messages */
	void initialize_messages(){
		
        vertex node, nei;
        list<pair<vertex, double> >::iterator iter;
        string id;
        
        for (node = 0; node < N; node++) {
            post[node] = 0.5;
            for (iter = network_map[node].begin(); iter != network_map[node].end(); iter++) {
                nei = (*iter).first;
                id = construct_id (nei, node);
                messages_map[id] = 0.5;
            }

        }
        
	}

	/* Send message from sender to receiver */
	double send_message(vertex sender, vertex receiver, double w){
		
        //calculate multiplication of messages sent to sender except the one from the receiver.
        double other_message[] = {1 - post[sender], post[sender]};
        double message_receiver_sender_pos;
        
        string id_receiver_sender = construct_id(receiver, sender);
        message_receiver_sender_pos = messages_map[id_receiver_sender];
        
        other_message[0] /= 1 - message_receiver_sender_pos;
        other_message[1] /= message_receiver_sender_pos;
        
        
		double message[] = {0.0, 0.0};
        
        int i, j;
        double node_potential;
        double edge_potential;
        
        //receiver's state
        for (i = 0; i < 2; i++) {
            //sender state
            for (j = 0; j < 2; j++) {
                
                node_potential = j == 1? prior[sender] : (1 - prior[sender]);
                edge_potential = i == j? w : 1 - w;
                
                message[i] += node_potential * edge_potential * other_message[j];
                
            }
            
        }
        
        double sum = message[0] + message[1];
        message[1] = message[1] / sum;
        
        if (message[1] < 1e-10) {
            message[1] = 1e-10;
        }
        
        if (message[1] > 1 - 1e-10) {
            message[1] = 1 - 1e-10;
        }
        
		return message[1];
        
	}


	/* Update posteriors in each iteration */
    void update_post(){

        double value[2], sum;
        list<pair<vertex, double> >::iterator iter;
        string id;
        vertex nei, node;
        
        unordered_map<vertex,list<pair<vertex, double> > >::iterator map_iter;
        
        for (map_iter = network_map.begin(); map_iter != network_map.end(); map_iter++) {
            node = map_iter->first;
            
            value[0] = 1;
            value[1] = 1;

            for (iter = (map_iter->second).begin(); iter != (map_iter->second).end(); iter++) {
                nei = (*iter).first;
                id = construct_id (nei, node);
                
                value[0] *= 1 - messages_map[id];
                value[1] *= messages_map[id];
                
                //normalize the values to avoid underflow or overflow
                sum = value[0] + value[1];
                value[0] /= sum;
                value[1] /= sum;
            }
            
            post[node] = value[1] / (value[1] + value[0]);
        }
        
    }
    

    /* Compute the final posteriors */
    void posterior(){
        
        double value[] = {0.0, 0.0};
        
        
        for (vertex node = 0; node < N; node++){
            value[0] = (1 - prior[node]) * (1 - post[node]);
            value[1] = prior[node] * post[node];
            
            //probability of being benign
            post[node] = value[1] / (value[0] + value[1]);
            
        }
        
    }

    /* Loopy belief propagation */
    //use iterative method to propagate messages
    //flooding method's performance is not as good as iterative method    
	void lbp(){
		
		//initialize messages
		initialize_messages();
    
		vector<vertex> ordering;
		for (vertex i = 0; i < N; i++) {
			ordering.push_back(i);
		}
		
		
		vector<vertex>::iterator iter_order;
		vertex node;
        double message;
        list<pair<vertex, double> >::iterator nei_iter;
        string id;
        
        double sum;
        
        int iter = 1;
		while (iter <= max_iter) {

            //random ordering
            random_shuffle(ordering.begin(), ordering.end(), p_myrandom);
           
			for (iter_order = ordering.begin(); iter_order != ordering.end(); iter_order++) {
				node = *iter_order;
                
				//collect messages for node from all of its neighbors
				for (nei_iter = network_map[node].begin(); nei_iter != network_map[node].end(); nei_iter++) {
                        
                    message = send_message((*nei_iter).first, node, (*nei_iter).second);
                   
                    //update message_map
                    id = construct_id((*nei_iter).first, node);
                    messages_map_new[id] = message;
                }
                
            }

            messages_map.clear();
            messages_map = messages_map_new;
            
            //update post
            update_post();
			
			iter += 1;
			
		}
		
        //calculate the final posteriors
        posterior();
	}


	/* Write final posteriors to the output file */
    void write_posterior(){
        
        ofstream out(post_file, ofstream::out);
        
        for (vertex i = 0; i < N; i++) { // need to change this...
            out << i << " " << setprecision(10) << post[i] << endl;
        }
        
        out.close();
        
    }

    /* Write final binary prediction to the  */
    void write_prediction() {
    	int prediction;
    	for (sequential_id i = 0; i < N; i++) {
    		prediction = (post[i] >= 0.5) ? 0 : 1;

    		// Update the records in g
    		g.sybil_status[g.id_to_v[i]] = prediction;
    	}

    	// Print the prediction to the output file
		g.print_estimate(prediction_out_file);
    }


    void parse_par(int argc, char **argv){
        
        //default setting
        network_file = "";
    
        train_set_file = "";
        post_file = "";
        prior_file = "";

        max_iter = 10;
        
        theta_pos = 0.9;
        theta_neg = 0.1;
        theta_unl = 0.5;
        
        weight = 0.9;
        
        //by default, unweighted graph
        weighted_graph = 0;
        
        int i = 1;
        
        while (i < argc) {
            
            if (strcmp(argv[i],"-graphfile") == 0){
                network_file = argv[i + 1];
            }
            else if (strcmp(argv[i],"-trainfile") == 0){
                train_set_file = argv[i + 1];
            }
            else if (strcmp(argv[i],"-priorfile") == 0){
                prior_file = argv[i + 1];
            }
            else if (strcmp(argv[i], "-postfile") == 0){
                post_file = argv[i + 1];
            }
            else if (strcmp(argv[i],"-mIter") == 0){
                max_iter = atoi(argv[i + 1]);
            }
            else if (strcmp(argv[i],"-tp") == 0){
                theta_pos = atof(argv[i + 1]);
            }
            else if (strcmp(argv[i],"-tn") == 0){
                theta_neg = atof(argv[i + 1]);
            }
            else if (strcmp(argv[i],"-tu") == 0){
                theta_unl = atof(argv[i + 1]);
            }
            else if (strcmp(argv[i],"-wei") == 0){
                weight = atof(argv[i + 1]);
            }
            else if (strcmp(argv[i],"-wg") == 0){
                weighted_graph = atoi(argv[i + 1]);
            }
            else if (strcmp(argv[i],"-dataout") == 0) {
            	prediction_out_file = argv[i + 1];
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
    
    SybilFuse sf;
    
    sf.parse_par(argc, argv);
    
    sf.read_network();
    
    sf.read_prior();
    
    sf.lbp();
        
    // sf.write_posterior();

    sf.write_prediction();


    return 0;
}


