// social-networks.cpp : Defines the entry point for the console application.
// This file takes a transition matrix of a topology as an input and computes the variation distance.
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <set>
#include <iostream>
#include<fstream>
#include <math.h>
#include<sstream>
#include<hash_map>
#include<assert.h>

using namespace std;


/* Global variables */ 
// The following should be the user input

int num_nodes;		// number of initial nodes		
int num_misguided;	// number of misguided nodes
int attack_nodes;	// number of additional sybil nodes introduced

// i am not sure what the transform_num_walks and sybil_infer num_walks should be...tradeoff bw precision and computational complexity

int sybilinfer_num_walks=20;	
int honest_node_walks=10000;
int sybilinfer_walk_length;

int num_samples = 2;	// number of samples
int num_steps;		// number of steps per sample
int reset_sample; 	// reset sample state to initial honest node after obtaining reset_sample samples


int mo=10;	// connected core in the scale free topology
int m=5;	// number of edges for each new node
int max_friends=100;

double *degree;	// node degree
set<int> *p;	// maintains topology
int **walks;	// maintains random walk samples
int *misguided;	// misguided nodes


void generate_topology();	// generates scale free topology
void generate_social_topology();	// uses real social network topology
void attacker_strategy();	// inserts additional sybil identities
void generate_samples();	// generates random walks
void transform_topology();
double quick_prob(set<int> a, set<int> b,double f, double hh, double dd); // compute prob of cuts
int test(set<int> as, set<int> bs); // tests is a cut is an attack 

int honest_node;	// apriori honest node	
double attack_edges;	// number of  attack edges
char action;		// addition/ removal in sampling	
set <int> honest_state,dishonest_state, candidate_state, opp_state;	
double **index;//h,d
double diff; 
int change;	// node to be inserted/added in current state of sampler


string graph_file;

int main(int argc, char * argv[])
{
	

	if(argc==7){
		// usage : social-networks.exe num_nodes num_misguided attack_nodes random_no

		num_nodes=atoi(argv[1]);
		num_misguided=atoi(argv[2]);
		attack_nodes=atoi(argv[3]);
		srand(atoi(argv[4]));	// random seed
		graph_file=argv[5];
		sybilinfer_walk_length=atoi(argv[6]);
	}
	else{
		cout << " Usage : social-networks.exe num_nodes num_dishonest num_sybil random_seed graph_file walk_length" << endl;
		exit(1);
	}
	
	num_steps=5*(num_nodes+attack_nodes)*log(num_nodes+attack_nodes)/log(2);	
	reset_sample=num_samples;
	
	
	//sybilinfer_walk_length=(int)2*ceil(log((double)num_nodes+attack_nodes)/log((double)m));	// initial random walk length
	//sybilinfer_walk_length=100;	
	
	// allocate memory
	degree=new double[num_nodes+attack_nodes];	
	for(int i=0;i<num_nodes+attack_nodes;i++){
		degree[i]=0;
	}
	p= new set<int>[num_nodes+attack_nodes];

	walks=new int *[num_nodes+attack_nodes];
	for(int i=0;i<num_nodes+attack_nodes;i++){
		walks[i] = new int [sybilinfer_num_walks];
	}

	
	misguided = new int [num_misguided];
	index = new double *[num_nodes+attack_nodes];
	for(int i=0;i<num_nodes+attack_nodes;i++){
		index[i] = new double [2];
	}

	
	set<int>::iterator iter,iter2;


	int * array_honest = new int[num_nodes+attack_nodes];
	int * array_candidate = new int[num_nodes+attack_nodes];
	double *marginals = new double[num_nodes+attack_nodes];

	double count=0;
	for(int i=0;i<num_nodes+attack_nodes;i++){
		marginals[i]=0;
	}
	
	
	
	//generate_topology();		// generate scale free topology
	generate_social_topology();
	//cout << "topology generated" << endl;
	attacker_strategy();	// insert sybil identities
	//cout << "attacker strategy done" << endl;
	generate_samples();		// construct random walks
	//cout << "sample generation done" << endl;


	// initialize current state of sampler, candidate state : both in sets/arrays
	honest_state.clear();
	candidate_state.clear();
	honest_state.insert(honest_node);
	candidate_state.insert(honest_node);
	array_honest[honest_node]=1;
	array_candidate[honest_node]=1;

	for(int i=0;i<honest_node_walks;i++){
		int temp=honest_node;	// random walk is currently at temp
		for(int k=0;k<sybilinfer_walk_length;k++){
			double random_float = (double)(1.0*rand()/(RAND_MAX + 1.0));
			double sum=0;
			set<int>::iterator set_iter;
			for(set_iter=p[temp].begin();set_iter!=p[temp].end();set_iter++){
				if(degree[temp]>degree[*set_iter]){
					sum=sum+1/((double)degree[temp]);
				}
				else{
					sum=sum+1/((double)degree[*set_iter]);
				}
				if(random_float < sum){
					break;
				}
			}
			if(set_iter==p[temp].end()){
				// temp remains the same
			}
			else{
				//walks[i][j][k]=*set_iter;
				temp=*set_iter;
			}
		}
		honest_state.insert(temp);
		candidate_state.insert(temp);
		array_honest[temp]=1;
		array_candidate[temp]=1;

		
		
	}
	/*for(int i=0;i<sybilinfer_num_walks;i++){
		honest_state.insert(walks[honest_node][sybilinfer_walk_length-1]);
		candidate_state.insert(walks[honest_node][sybilinfer_walk_length-1]);
		array_honest[walks[honest_node][sybilinfer_walk_length-1]]=1;
		array_candidate[walks[honest_node][sybilinfer_walk_length-1]]=1;

	//	cout << " inserting " << honest_walks[i][walk_length-1] << endl;
	}*/

	/*cout<< " Scenario : " << endl;
	cout << " Number of nodes = " << num_nodes << endl;
	cout << " Number of dishonest nodes = " << num_misguided << endl;
	cout << " Number of sybil nodes = " << attack_nodes << endl;
	cout << " Number of attack edges = " << attack_edges << endl;
	cout << " Random walk length = " << walk_length << endl;
	cout << " Initial sample size = " << honest_state.size();
	cout << endl << endl;*/

	// initialized opposite state of current state/candidate state
	dishonest_state.clear();
	opp_state.clear();
	for(int i=0;i<num_nodes+attack_nodes;i++){
		if(honest_state.find(i)==honest_state.end()){
			dishonest_state.insert(i);
			opp_state.insert(i);
			array_honest[i]=0;
			array_candidate[i]=0;
		}
	}
		
	// correct = > set of honest nodes including misguided
	// correct_w => set of honest nodes without misguided nodes

	set<int> correct, incorrect, correct_w, incorrect_w;
	for(int i=0;i<num_nodes;i++){
		correct.insert(i);
		correct_w.insert(i);
	}
	for(int i=num_nodes;i<num_nodes+attack_nodes;i++){
		incorrect.insert(i);
		incorrect_w.insert(i);
	}
	for(int i=0;i<num_misguided;i++){
		correct_w.erase(misguided[i]);
		incorrect_w.insert(misguided[i]);
	}

	

	// lets compute hh, best hh
	double hh, new_hh, initial_hh;
	hh=0;
	for(iter=honest_state.begin();iter!=honest_state.end();iter++){	// consider random walks of honest nodes
		for(int z=0;z<sybilinfer_num_walks;z++){
			if(honest_state.find(walks[*iter][z])!=honest_state.end()){
				hh++;
			}
			
		}
	}
	initial_hh=hh;

	double w_hh=0;
	for(iter=correct_w.begin();iter!=correct_w.end();iter++){	// consider random walks of honest nodes
		for(int z=0;z<sybilinfer_num_walks;z++){
			if(correct_w.find(walks[*iter][z])!=correct_w.end()){
				w_hh++;
			}
			
		}
	}

	

	// lets compute dd, best dd
	double dd,new_dd,initial_dd;
	dd=0;
	for(iter=dishonest_state.begin();iter!=dishonest_state.end();iter++){
		for(int z=0;z<sybilinfer_num_walks;z++){
			if(dishonest_state.find(walks[*iter][z])!=dishonest_state.end()){
				dd++;
			}
		}
	}
	initial_dd=dd;

	double w_dd=0;
	for(iter=incorrect_w.begin();iter!=incorrect_w.end();iter++){
		for(int z=0;z<sybilinfer_num_walks;z++){
			if(incorrect_w.find(walks[*iter][z])!=incorrect_w.end()){
				w_dd++;
			}
		}
	}

	// index helps in updating of hh

	double **initial_index = new double * [num_nodes+attack_nodes];
	for(int i=0;i<num_nodes+attack_nodes;i++){
		index[i][0]=0;
		index[i][1]=0;

		initial_index[i] = new double[2];
		initial_index[i][0]=0;
		initial_index[i][1]=0;
	}
	for(int i=0;i<num_nodes+attack_nodes;i++){
		for(int z=0;z<sybilinfer_num_walks;z++){
			int end=walks[i][z];
			if(honest_state.find(i)!=honest_state.end()){
				index[end][0]++;
				initial_index[end][0]++;
			}
			else{
				index[end][1]++;
				initial_index[end][1]++;
			}
		}
	}
	

	/* Let us begin the sampling process */


	
	for(int q=0;q < num_steps*num_samples; q++){
		if((q>=20000 && q%num_steps==0)){	// we have got a sample...store information if it is an attack case
			
			//cout << endl << " Sample : " ;
			
			set<int> z;
			if(honest_state.find(honest_node)!=honest_state.end()){
				z=honest_state;
			}
			else{
				z=dishonest_state;
				//dishonest_state=honest_state;
				//honest_state=z;
			}

			//for(iter=honest_state.begin();iter!=honest_state.end();iter++){
			//	cout << *iter << " " ;
			//}
			//cout << endl;
						
			int eval=test(honest_state,dishonest_state);
			if(eval ==1){
				count++;
				//cout << "  => This Cut is an ATTACK CASE " << endl ;
				for(int i=0;i<num_nodes+attack_nodes;i++){
					if(z.find(i)!=z.end()){
						marginals[i]++;
					}
				}
			}
			else{
				//cout << " => This Cut is an HONEST CASE" << endl;
			}
			
			// i am not sure what the code below does, so i am going to comment it for now
			
			/*double c=0,d=0;
			for(int i=0;i<num_nodes+attack_nodes;i++){
				if(correct_w.find(i)!=correct_w.end()){
					if(z.find(i)!=z.end()){
						c++;
					}
				}
				else{
					if(z.find(i)!=z.end()){
						d++;
					}
				}
			}*/

			if(q % (num_steps*reset_sample) == 0){		// reset the sampler to start again
					honest_state.clear();
					candidate_state.clear();
					honest_state.insert(honest_node);
					candidate_state.insert(honest_node);
					array_honest[honest_node]=1;
					array_candidate[honest_node]=1;
					
					dishonest_state.clear();
					opp_state.clear();
					for(int i=0;i<num_nodes+attack_nodes;i++){
						if(honest_state.find(i)==honest_state.end()){
							dishonest_state.insert(i);
							opp_state.insert(i);
							array_honest[i]=0;
							array_candidate[i]=0;
						}	
						index[i][0]=initial_index[i][0];
						index[i][1]=initial_index[i][1];
					}
					hh=initial_hh;
					dd=initial_dd;
			}			
		}
		
		
		// swap a random node
		int val = (int)(((double)(num_nodes+attack_nodes))*rand()/(RAND_MAX + 1.0));
		if(array_honest[val]==1){
			action='r';
		}
		else{
			action ='a';
		}
				
		if(action=='a'){	// addition to the honest state
			
			change=val;
			
			candidate_state.insert(change);
			opp_state.erase(change);
			array_candidate[change]=1;

			
			new_hh=hh+index[change][0];
			new_dd=dd-index[change][1];
			for(int i=0;i<sybilinfer_num_walks;i++){
				int end=walks[change][i];
				if(array_candidate[end]==1){
					new_hh++;
				}
				else{
					new_dd--;
				}
			}
			
			// compute prob of new state

			double diff1=0;
		
			if(opp_state.size()==0){
				diff1=candidate_state.size()*sybilinfer_num_walks*log(1/((double)candidate_state.size()));// + a.size()*log(0.001);
			}
			else if(candidate_state.size()==0){
				diff1=opp_state.size()*sybilinfer_num_walks*log(1/((double)opp_state.size()));// +b.size()*log(0.999);
			}
			else{
				double hd=candidate_state.size()*sybilinfer_num_walks-new_hh;
				double dh=opp_state.size()*sybilinfer_num_walks-new_dd;
	 
				if(new_hh!=0){
					diff1=diff1+new_hh*log(new_hh/((new_hh+hd)*candidate_state.size()));
				}
				if(hd!=0){
					diff1=diff1+hd*log(hd/((new_hh+hd)*opp_state.size()));
				}
				if(new_dd!=0){
					diff1=diff1+new_dd*log(new_dd/((new_dd+dh)*opp_state.size()));
				}
				if(dh!=0){
					diff1=diff1+dh*log(dh/((dh+new_dd)*candidate_state.size()));
				}
			}

			// compute prob of old state
			double diff2=0;
		
			if(dishonest_state.size()==0){
				diff2=honest_state.size()*sybilinfer_num_walks*log(1/((double)honest_state.size()));// + a.size()*log(0.001);
			}
			else if(honest_state.size()==0){
				diff2=dishonest_state.size()*sybilinfer_num_walks*log(1/((double)dishonest_state.size()));// +b.size()*log(0.999);
			}
			else{
				double hd=honest_state.size()*sybilinfer_num_walks-hh;
				double dh=dishonest_state.size()*sybilinfer_num_walks-dd;
	 
				if(hh!=0){
					diff2=diff2+hh*log(hh/((hh+hd)*honest_state.size()));
				}
				if(hd!=0){
					diff2=diff2+hd*log(hd/((hh+hd)*dishonest_state.size()));
				}
				if(dd!=0){
					diff2=diff2+dd*log(dd/((dd+dh)*dishonest_state.size()));
				}
				if(dh!=0){
					diff2=diff2+dh*log(dh/((dh+dd)*honest_state.size()));
				}
			}

			diff=diff1-diff2;
		}
		else if (action=='r'){
			change=val;
			candidate_state.erase(change);
			opp_state.insert(change);
			array_candidate[change]=0;
			
			
			
			new_hh=hh-index[change][0];//have already removed myself to myself walks
			new_dd=dd+index[change][1];
			//cout << "subtract " << index[change][0] << endl;
			for(int i=0;i<sybilinfer_num_walks;i++){
				int end=walks[change][i];
				if(array_candidate[end]==1){
					new_hh--;
				}
				else{
					new_dd++;
				}
				
			}

			// compute prob of new state
			double diff1=0;
		
			if(opp_state.size()==0){
				diff1=candidate_state.size()*sybilinfer_num_walks*log(1/((double)candidate_state.size()));// + a.size()*log(0.001);
			}
			else if(candidate_state.size()==0){
				diff1=opp_state.size()*sybilinfer_num_walks*log(1/((double)opp_state.size()));// +b.size()*log(0.999);
			}
			else{
				double hd=candidate_state.size()*sybilinfer_num_walks-new_hh;
				double dh=opp_state.size()*sybilinfer_num_walks-new_dd;
	 
				if(new_hh!=0){
					diff1=diff1+new_hh*log(new_hh/((new_hh+hd)*candidate_state.size()));
				}
				if(hd!=0){
					diff1=diff1+hd*log(hd/((new_hh+hd)*opp_state.size()));
				}
				if(new_dd!=0){
					diff1=diff1+new_dd*log(new_dd/((new_dd+dh)*opp_state.size()));
				}
				if(dh!=0){
					diff1=diff1+dh*log(dh/((dh+new_dd)*candidate_state.size()));
				}
			}

			// compute prob of old state
			double diff2=0;
		
			if(dishonest_state.size()==0){
				diff2=honest_state.size()*sybilinfer_num_walks*log(1/((double)honest_state.size()));// + a.size()*log(0.001);
			}
			else if(honest_state.size()==0){
				diff2=dishonest_state.size()*sybilinfer_num_walks*log(1/((double)dishonest_state.size()));// +b.size()*log(0.999);
			}
			else{
				double hd=honest_state.size()*sybilinfer_num_walks-hh;
				double dh=dishonest_state.size()*sybilinfer_num_walks-dd;
	 
				if(hh!=0){
					diff2=diff2+hh*log(hh/((hh+hd)*honest_state.size()));
				}
				if(hd!=0){
					diff2=diff2+hd*log(hd/((hh+hd)*dishonest_state.size()));
				}
				if(dd!=0){
					diff2=diff2+dd*log(dd/((dd+dh)*dishonest_state.size()));
				}
				if(dh!=0){
					diff2=diff2+dh*log(dh/((dh+dd)*honest_state.size()));
				}
			}

			diff=diff1-diff2;
			
		}
		
			
		double random_val = (double)(1.0*rand()/(RAND_MAX + 1.0));
		

		if((diff > log(random_val) || random_val==0)){	// new state accepted, commit
			//cout << " next " << endl;

		
			hh=new_hh;
			dd=new_dd;
		
			// need to update the index
			if(action=='a'){
				//add node to honest state
				honest_state.insert(change);
				dishonest_state.erase(change);
				array_honest[change]=1;
				for(int i=0;i<sybilinfer_num_walks;i++){
					int end=walks[change][i];
					index[end][0]++;
					index[end][1]--;
				}
			//	cout << " inserting change into sample " << change << endl;
			}
			else if(action=='r'){
				// remove node from honest state
				honest_state.erase(change);
				dishonest_state.insert(change);
				array_honest[change]=0;
				for(int i=0;i<sybilinfer_num_walks;i++){
					int end=walks[change][i];
					index[end][0]--;
					index[end][1]++;
				}
			//	cout << " node removed " << change << endl;
			}
			
		}
		else {						// new state rejected. Undo 
			if(action=='a'){
				candidate_state.erase(change);
				opp_state.insert(change);
				array_candidate[change]=0;
			}
			else if (action =='r'){
				candidate_state.insert(change);
				opp_state.erase(change);
				array_candidate[change]=1;
			}
			
		}
		
	}

	// lets compute the final false positives/negatives
	

	double size=0;
	double fp=0,fn=0;
	for(iter=correct_w.begin();iter!=correct_w.end();iter++){
		marginals[*iter]=marginals[*iter]/count;
	//	cout << " Node " << i << " = " << marginals[i] << endl;
		if(marginals[*iter] < 0.5){
			fp++;
			//p[*iter].clear();	// part of new experimental code
		}
		else{
			size++;
			
		}

	}
	for(iter=incorrect_w.begin();iter!=incorrect_w.end();iter++){
		marginals[*iter]=marginals[*iter]/count;
	//	cout << " Node " << i << " = " << marginals[i] << endl;
		if(marginals[*iter] >0.5){
			fn++;
			size++;
		}
	}

	if(count >0){

		cout << num_nodes << " " << num_misguided << " " << attack_edges << " " << attack_nodes << " " << count << " " << fp  << " " << fn << endl;
	}
	else{
		cout << num_nodes<< " " << num_misguided << " " << attack_edges << " " << attack_nodes << " " << count << " " << "0" << " " << attack_nodes+num_misguided << endl;
	}
	
	return 0;
}





double quick_prob(set<int> a, set<int> b, double f, double hh, double dd){

	// assigns probability to a cut. Pasted code inside sampler as an optimization. Hence, never used.
	double ans=0;
	set<int>::iterator iter, iter2;

	if(b.size()==0){
		return a.size()*sybilinfer_num_walks*log(1/((double)a.size()));// + a.size()*log(0.001);
	}
	if(a.size()==0){
		return b.size()*sybilinfer_num_walks*log(1/((double)b.size()));// +b.size()*log(0.999);
	}

	double hd=a.size()*sybilinfer_num_walks-hh;

	double dh=b.size()*sybilinfer_num_walks-dd;
	
	ans=0;
	if(hh!=0){
		ans=ans+hh*log(hh/((hh+hd)*a.size()));
	}
	if(hd!=0){
		ans=ans+hd*log(hd/((hh+hd)*b.size()));
	}
	if(dd!=0){
		ans=ans+dd*log(dd/((dd+dh)*b.size()));
	}
	if(dh!=0){
		ans=ans+dh*log(dh/((dh+dd)*a.size()));
	}

	return ans;// +a.size()*log(0.001)+b.size()*log(0.999);
}




void generate_topology(){
	/* Generating scale free topology */
	/* Start with a connected core */

	int * sample_scale_free = new int [mo*(mo-1)+num_nodes*2*m];
	int sample_length=0;

	for(int i=0;i<mo;i++){
		for(int j=0;j<mo;j++){
			if(i!=j){
				p[i].insert(j);
				degree[i]++;
				sample_scale_free[sample_length]=i;
				sample_length++;
			}
		}
	}
	/* Add remaining edges in proportion to the node degree */
	for(int i=mo;i<num_nodes;i++){
		for(int j=0;j<m;j++){
			// choose a random number less than node degreee
			int random_integer =int((double)(sample_length-1)*rand()/(RAND_MAX + 1.0));
			int k=sample_scale_free[random_integer];

			if(p[i].find(k)!=p[i].end()){	// duplicate detection
				j--;
			}
			else if(degree[k] > max_friends){
				j--;
			}
			else{
				p[i].insert(k);
				p[k].insert(i);
				degree[i]++;
				degree[k]++;
				sample_scale_free[sample_length]=i;
				sample_length++;
				sample_scale_free[sample_length]=k;
				sample_length++;
			}
		}
	}
	/*set<int>::iterator iter;
	for(int i=0;i<num_nodes;i++){
		cout << i << " ";
		for(iter=p[i].begin();iter!=p[i].end();iter++){
			cout << *iter << " ";
		}
		cout << endl;
	}*/
}

void generate_social_topology(){

	// use the real social network topology

	ifstream in;
	in.open(graph_file.c_str(),ifstream::in);
	if(in==NULL){
		printf("error opening file\n");
		exit(1);
	}

	long int i,j,index;
	char *token;
	const int max_count = 50000;
	//char name[max_count];
	string name;


	double line_count=0;
	while(!in.eof() && line_count < num_nodes){
		getline(in, name);
		stringstream ss(name);
		if(ss){	
			ss >> index;
			while(ss){	
				ss >> j;
				degree[index]++;
				p[index].insert(j);
			}
		}
		line_count++;
	}
	in.close();
}

void attacker_strategy(){
	/* Introduce Attackers into the matrix */

	/* Select misguided nodes and make edges */
	honest_node = (int) ((float)num_nodes*rand()/(RAND_MAX+1.0));
	//honest_node=90;
	//honest_node=0;
	
	int random_no;
	set<int> mis;
	set<int> bad;
	double deg_count=0;

	for(int i=0;i<num_misguided;i++){
		random_no = (int) ((float)num_nodes*rand()/(RAND_MAX+1.0));
		/* check that misguided node != apriori honest node, or its friends */
		if(mis.find(random_no)!=mis.end() || random_no==honest_node || p[honest_node].find(random_no)!=p[honest_node].end()){
			i--;
			continue;
		}
		else{
			misguided[i]=random_no;
			mis.insert(random_no);
			bad.insert(random_no);
			deg_count=deg_count +degree[random_no];
		}
	}

	set<int>::iterator iter,iter2,iter4;

	// compute number of attack edges
	attack_edges=deg_count;
	for(iter=mis.begin();iter!=mis.end();iter++){
		for(iter2=mis.begin();iter2!=mis.end();iter2++){
			if(p[*iter].find(*iter2)!=p[*iter].end()){
				attack_edges=attack_edges-2;
			}
		}
	}

	// insert sybil identities in a scale free manner

	for(int i=num_nodes;i<num_nodes+attack_nodes;i++){
		for(int j=0;j<m;j++){
			// choose a random number less than node degreee
			int random_integer =int(deg_count*rand()/(RAND_MAX + 1.0));	// deg_count = sum of degrees of nodes already added.
			int sum=0;
			set<int>::iterator bad_iter;
			int bad_last;
			for(bad_iter=bad.begin();bad_iter!=bad.end();bad_iter++){
				if(random_integer < sum){
					break;
				}
				sum=sum+degree[*bad_iter];
				bad_last=*bad_iter;
			}
			if(bad_iter!=bad.end()){
				p[i].insert(*bad_iter);
				p[*bad_iter].insert(i);
				degree[i]++;
				degree[*bad_iter]++;
			}
			else{
				p[i].insert(bad_last);
				p[bad_last].insert(i);
				degree[i]++;
				degree[bad_last]++;
			}
		}
		bad.insert(i);
		deg_count=deg_count+2*m;
	}
}


void generate_samples(){
	/* generate random walk samples */
	for(int i=0;i<num_nodes+attack_nodes;i++){
		for(int j=0;j<sybilinfer_num_walks;j++){
			int temp=i;	// random walk is currently at temp
			for(int k=0;k<sybilinfer_walk_length;k++){
				double random_float = (double)(1.0*rand()/(RAND_MAX + 1.0));
				double sum=0;
				set<int>::iterator set_iter;
				for(set_iter=p[temp].begin();set_iter!=p[temp].end();set_iter++){
					if(degree[temp]>degree[*set_iter]){
						sum=sum+1/((double)degree[temp]);
					}
					else{
						sum=sum+1/((double)degree[*set_iter]);
					}
					if(random_float < sum){
						break;
					}

				}
				if(set_iter==p[temp].end()){
					// temp remains the same
				}
				else{
					//walks[i][j][k]=*set_iter;
					temp=*set_iter;
				}
			}
			walks[i][j]=temp;
			
		}
		
	}

}

int test(set<int> as, set<int> bs){

	// function that tests if a cut is an attack cut or not
	// attack index set to 0.7
	
	// can this function be optimized any more?

	if(as.size()==0){
		return 0;
	}
	if(bs.size()==0){
		return 0;
	}
	
	double ans=0;
	set<int>::iterator iter, iter2;
	
	double num=0;
	if(as.size() < bs.size()){
		for(iter=as.begin();iter!=as.end();iter++){
			for(iter2=p[*iter].begin();iter2!=p[*iter].end();iter2++){
				if(bs.find(*iter2)!=bs.end()){
					// increment num
					if(degree[*iter]<degree[*iter2]){	
						num=num+1/((double)degree[*iter2]);
					}
					else{
						num=num+1/((double)degree[*iter]);
					}
				}
			}
		}
	}
	else{
		for(iter=bs.begin();iter!=bs.end();iter++){
			for(iter2=p[*iter].begin();iter2!=p[*iter].end();iter2++){
				if(as.find(*iter2)!=as.end()){
					// increment num
					if(degree[*iter]<degree[*iter2]){	
						num=num+1/((double)degree[*iter2]);
					}
					else{
						num=num+1/((double)degree[*iter]);
					}
				}
			}
		}
	}

	double cond=num/((double)as.size());	// f here is the conductance
	double back_cond = num/(double)bs.size();
	
	double a=(1-cond),b=cond,c=back_cond,d=(1-back_cond);
	double e=(1-cond),f=cond,g=back_cond,h=(1-back_cond);
	double i,j,k,l;

	for(int z=0;z<sybilinfer_walk_length-1;z++){
		i=e*e+b*g;
		j=a*f+b*h;
		k=c*e+d*g;
		l=c*f+d*h;
		a=i;
		b=j;
		c=k;
		d=l;
	}

	//cout << sybilinfer_walk_length << " " <<  (a/as.size()-1.0/(double)(num_nodes+attack_nodes))/(1.0/as.size()-1.0/(double)(num_nodes+attack_nodes)) << endl;
	//exit(1);
	if((a/as.size()-1.0/(double)(num_nodes+attack_nodes))/(1.0/as.size()-1.0/(double)(num_nodes+attack_nodes)) > 0.75){
		return 1;
	}
	else{
		return 0;
	}
	
}

