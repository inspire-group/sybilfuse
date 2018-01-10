function [graph, indegree, outdegree] = scale_free(N, recp_prob, flag)
% Generate a scale-free directed graph structure using preferential attachment
% The graph is structured as follows:    
%   receiver sender..
% N - number of nodes in the generated graph
% recp_prob - reciprocate probability
% flag - +1 for benign, -1 for sybil

graph=[];
N0 = 10; % number of initial nodes
d = 4; % number of links originated from each newly added node

% Initialize N0 nodes, which form a clique.
for i=1:N0
    for j=1:i-1
        graph=[graph;i,j;j,i];
    end
end

indegree=zeros(N,1);
indegree(1:N0)=(N0-1)*ones(N0,1);
outdegree = zeros(N, 1);
outdegree(1:N0)=(N0-1)*ones(N0,1);

% For each new nodes
for i=N0+1:N
    % Launch d links to the existing nodes, the launch probability is
    % proportional to the indegree
    neigh = datasample(1:i-1, d, 'Replace', false, 'Weights', indegree(1:i-1));
    for k=1:d
        % Update the "receiver sender" link & receiver indegree
        graph = [graph; neigh(k), i];
        indegree(neigh(k)) = indegree(neigh(k)) + 1;
        outdegree(i) = outdegree(i) + 1;
        
        % Receiver follows back based on the recp_prob
        if rand(1) <= recp_prob
            graph = [graph; i, neigh(k)];
            indegree(i) = indegree(i) + 1;
            outdegree(neigh(k)) = outdegree(neigh(k)) + 1;
        end
    end    
end

% Change the nodes index according to flag
if flag == -1
   graph = graph * (-1);
end

end
