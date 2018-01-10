function graph = combination(graph_h, graph_s, N_attack, recp_prob_h2s)
% Combine graph_h with graph_s using N_attack edges. recp_prob_h2s refers
% to the reciprocate probability of benign nodes to sybil nodes.

%% Default: N edges are added randomly
graph = [graph_h; graph_s];
min_h = min(graph_h(:, 1));
max_h = max(graph_h(:, 1));
min_s = min(graph_s(:, 1));
max_s = max(graph_s(:, 1));

for i = 1:N_attack
    % select honest endnode
    ind_h = randi([min_h, max_h], 1, 1);
    
    % select sybil endnode
    ind_s = randi([min_s, max_s], 1, 1);
    
    % add attack edge from ind_s to ind_h
    graph = [graph; ind_h, ind_s];
    
    % add reciprocal edge from ind_h to ind_s based on recp_prob_h2s
    if rand(1) < recp_prob_h2s
        graph = [graph; ind_s, ind_h];
    end
end




end