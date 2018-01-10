function scale_free_plot(graph, N, indegree, outdegree)
% Get degree information for graph (bi-directional)
% N = size(unique(graph(:, 1)));
% degree = zeros(N, 1);
% for i=1:N
%     degree(i) = numel(find(graph(:, 1) == i));
% end

uniq_degree_in = unique(indegree);
count_uniq_in = histc(indegree, uniq_degree_in);
count_freq_in = count_uniq_in/numel(indegree);

uniq_degree_out = unique(outdegree);
count_uniq_out = histc(outdegree, uniq_degree_out);
count_freq_out = count_uniq_out/numel(outdegree);

% Generate histogram
subplot(1, 2, 1);
loglog(uniq_degree_in, count_freq_in, 'bo')
xlabel('indegree');
ylabel('fraction of nodes');
xlim([min(indegree), max(indegree)]);

subplot(1, 2, 2);
loglog(uniq_degree_out, count_freq_out, 'bo')
xlabel('outdegree');
ylabel('fraction of nodes');
xlim([min(outdegree), max(outdegree)]);


end