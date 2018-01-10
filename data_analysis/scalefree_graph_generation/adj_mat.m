function adj = adj_mat(graph, N_h, N_s)
% Compute the adjacency matrix of the directed graph
% Input: 
% - graph: network graph, directed (format: receiver--sender)
% - N_h: number of honest nodes
% - N_s: number of sybil nodes
% Output:
% - adj: adjacency matrix (adj(i, j) -> a link from j to i exists)

% Re-index the negative values of the graph
for i=1:length(graph(:, 1))
   if graph(i, 1) < 0
      graph(i, 1) = N_h - graph(i, 1); % -k, which denotes the kth node in the sybil region, changes to N_h + k 
   end
   if graph(i, 2) < 0
      graph(i, 2) = N_h - graph(i, 2); 
   end
end

% Construct the adjacency matrix
N = N_h + N_s;
adj = zeros(N, N);
for i=1:length(graph(:, 1))
   adj(graph(i, 1), graph(i, 2)) = 1;
end


end