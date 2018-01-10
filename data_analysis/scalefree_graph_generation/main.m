% Description: This program generates a directed social graph, constructed
% by a scale-free benign & sybil region, and user-specified number of
% attack edges. The adjacency matrix and transition probability matrix is
% also calculated.

% Note: This prorgam also works for the undirected graph, as long as each
% link occurs twice in the "graph". Each line of "graph" is formated as
% follows: [receiver1 sender1; receiver2 sender2; ...]

%% Initialization
clc
clear all;
close all;
N_h = 500;  % number of benign nodes
N_s = 300; % number of sybil nodes
N_attack = 20; % number of attack edges
recp_prob_h = 0.6; % reciprocate probability for benign region
recp_prob_s = 0.9; % reciprocate probability for sybil region
recp_prob_h2s = 0.2; % reciprocate probability for the benign node to sybil node


%% Construct the directed network graph
% benign region
graph_h = scale_free(N_h, recp_prob_h, 1); % positive index for benign nodes

% sybil region
graph_s = scale_free(N_s, recp_prob_s, -1); % negative index for sybil nodes

% combine two regions together
graph=combination(graph_h,graph_s,N_attack, recp_prob_h2s);


%% Compute adjacency matrix and TPM
adj = adj_mat(graph, N_h, N_s);
TPM = TPM_mat(graph, N_h, N_s, 1); % TPM_1 (1-hop)





