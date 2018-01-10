function TPM = TPM_mat(graph, N_h, N_s, l)
% Compute the transition probability matrix of the directed graph
% Input: 
% - graph: network graph, bi-direction
% - N_h: number of honest nodes
% - N_s: number of sybil nodes
% - l: number of hops in random walk
% Output:
% - TPM: transition probability matrix (TPM(i, j) -> a probability flow from j to i exists)

% Re-index the negative values of the graph
for i=1:length(graph(:, 1))
   if graph(i, 1) < 0
      graph(i, 1) = N_h - graph(i, 1); % -k, which denotes the kth node in the sybil region, changes to N_h + k 
   end
   if graph(i, 2) < 0
      graph(i, 2) = N_h - graph(i, 2); 
   end
end

% Construct the transition probability matrix
N = N_h + N_s;
TPM = zeros(N, N);
for i=1:length(graph(:, 1))
   curr_degree = numel(find(graph(:, 2) == graph(i, 2)));
   if curr_degree ~= 0
       TPM(graph(i, 1), graph(i, 2)) = 1/curr_degree;
   end
end

% Power iteration for l random walks
TPM = TPM^l;


end