#include "graph.h"

void graph::add_vertex(vertex node){
    if(vertices.count(node) == 0){
	vertices.insert(node);
    sequential_id curr_id = id_to_v.size();
	id_to_v[curr_id] = node;
    v_to_id[node] = curr_id;
    }
}

vertex graph::add_random_vertex(){
    vertex v;
    do{
	v = rand();
    } while (vertices.count(v) == 1);
    add_vertex(v);
    return v;
}

void graph::add_edge(vertex node1,vertex node2){
    // add edge node1, node2

    // ensure node1, node2 in the vertex set
    // can simply insert in vertex set
    // if they already exist, no harm
    add_vertex(node1);
    add_vertex(node2);

    // no self loops
    if(node1==node2){
	return;
    }

    // add this in the adjacency list for both nodes
    // graph is undirected
    if(adjacency[node1].count(node2) == 0){
	adjacency[node1].insert(node2);
	adjacency[node2].insert(node1);
	/*
	edge_map[node1][node2]=edge_counter;
	edge_counter++;
	edge_map[node2][node1]=edge_counter;
	edge_counter++;
	*/
    }
}

bool graph::add_induced_edge(vertex node1,vertex node2){
    if(vertices.count(node1)==0 || vertices.count(node2)==0 || node1==node2){
	return false;
    } else {
	// add this in the adjacency list for both nodes
	// graph is undirected
	if(adjacency[node1].count(node2) == 0){
	    adjacency[node1].insert(node2);
	    adjacency[node2].insert(node1);
	    /*
	    edge_map[node1][node2]=edge_counter;
	    edge_counter++;
	    edge_map[node2][node1]=edge_counter;
	    edge_counter++;
	    */
	}
	return true;
    }
}


/*
vertex graph::randVertex(int seed){
    srand(seed);
    int random = randomInt(vertices.size());
    set<vertex>::iterator iter=vertices.lower_bound(random);
    if(iter==vertices.end()){
	iter=vertices.begin();
    }
    return *iter;
}
*/ 
void graph::print_graph(char *graph_out_file){
    ofstream out_file;
    out_file.open(graph_out_file,ofstream::out);
    assert(out_file);
    cout << "Test membership: " << vertices.count(0) << endl;

    set<vertex>::iterator iter;
    set<vertex>::iterator iter2;
    for(iter=vertices.begin();iter!=vertices.end();iter++){
	out_file << *iter;
	for(iter2=adjacency[*iter].begin();iter2!=adjacency[*iter].end();iter2++){
	    out_file << " " << *iter2;
	}
	out_file << endl;
    }
}

void graph::print_estimate(char *data_file){
    ofstream out_file;
    out_file.open(data_file,ofstream::out);
    assert(out_file);

    cout << "Number of vertices: " << vertices.size() << endl;

    set<vertex>::iterator iter;
    bool first_pass = true;
    for(iter=vertices.begin();iter!=vertices.end();iter++){
	if(first_pass){
	    out_file << "[" << endl;
	    first_pass = false;
	} else {
	    out_file << "," << endl;
	}
	out_file << "{" << endl;
	out_file << "\"user_id\": " << *iter << "," << endl;
	out_file << "\"sybil\": " << sybil_status[*iter]  << endl;
	out_file << "}";
    }

    out_file << endl << "]" << endl;
}



/* Read in the social graph */
void graph::read_social_topology(char *graph_file){
    ifstream i;
    i.open(graph_file,ifstream::in);
    assert(i);

    string line;
    vertex node1,node2;
    while(getline(i,line)){
	stringstream ss(line);
	if(ss){
	    ss >> node1;
	    while(ss){
		ss >> node2;
		add_edge(node1,node2);
	    }
	}
    }
    i.close();

}
