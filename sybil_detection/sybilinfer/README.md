There are two files in that directory: social-networks.cpp -- which runs sybilinfer on the original graph with the following parameters: 

./a.out num_nodes num_misguided num_sybil random_no graph_file sybilinfer_walk_length 

num_nodes -> number of nodes in the graph
num_misguided --> number of initial compromised nodes (that will help in inserting Sybil identities)
num_sybil --> these are number of Sybil nodes inserted by the set of compromised nodes mentioned previously
random_no --> random seed
graph_file --> path to social network (assumed undirected, connected graph) graph vertices are assumed labeled from 0 to num_nodes-1, file format is "vertex_no" followed by its neighbors, for example:

0 1 3 5
1 0 2
2 0 1
...

sybilinfer_walk_length --> length of random walks used in SybilInfer: this needs to be determined empirically. See scripts-*.sh for the parameters I used (for wall post graph, 110, after transformation -- 60) (for link graph, 60, after transformation -- 60) (lower walk length after transformation helps improved performance -- perhaps length required for your transformation algorithm can be even smaller!) (script-length.sh might be useful to figure this out)

There is another file, social-networks-transformation.cpp, which first transforms the graph and then runs SybilInfer -- you can simply replace the transformation function in that file with your own transformation function. This file takes another parameter at the end -- the random walk length used in the transformation. 

There are a bunch of *.sh and *.r files that might be useful to automate the experiments. For example, script-t2.sh runs the wall post graph experiments for t=2, while script-links-t5.sh runs the link graph experiments for t=5. 