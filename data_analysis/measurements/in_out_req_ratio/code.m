%%% Experiment in "Uncovering social network sybils in the wild"

%% load the data
req_info = load('req_info');
[m, N] = size(req_info);			% 41652230, 7

%% Extract fields
num_bi_edges = req_info(:, 5);
in_req_accepted_ratio = req_info(:, 6);
out_req_accepted_ratio = req_info(:, 7);

%% Some info
numel(find(req_info(:, 2) == 1))	% benign number = 36959205
numel(find(req_info(:, 2) == -1))	% spam number = 205292

numel(find(req_info(:, 3) == 0))	% 1548949
numel(find(req_info(:, 4) == 0))	% 5963082
numel(find(req_info(:, 3) == 0 & req_info(:, 4) == 0))
numel(find(req_info(:, 5) == 0))	% 19071811


%% CDF plot of incoming/outgoing requests accepted ratio
% Preprocessing, do not consider nan value
ind_in_noNa_benign = find((req_info(:, 3) ~= 0) & (req_info(:, 2) == 1));
ind_out_noNa_benign = find((req_info(:, 4) ~= 0) & (req_info(:, 2) == 1));
ind_in_noNa_spam = find((req_info(:, 3) ~= 0) & (req_info(:, 2) == -1));
ind_out_noNa_spam = find((req_info(:, 4) ~= 0) & (req_info(:, 2) == -1));

% CDF of incoming request accepted ratio
h1 = cdfplot(req_info(ind_in_noNa_benign, 6));
set(h1, 'Color', 'red');
hold on;
h2 = cdfplot(req_info(ind_in_noNa_spam, 6));
set(h2, 'Color', 'blue');
legend([h1 h2], 'benign', 'spam');
title('Ratio of accepted incoming friend requests');
xlabel('Ratio of Accepted Incoming Requests');
ylabel('CDF');
print(gcf, '-djpeg', 'in_req_accepted_ratio.jpg');
close;
clear h1 h2;

% CDF of outgoing request accepted ratio
h1 = cdfplot(req_info(ind_out_noNa_benign, 7));
set(h1, 'Color', 'red');
hold on;
h2 = cdfplot(req_info(ind_out_noNa_spam, 7));
set(h2, 'Color', 'blue');
legend([h1 h2], 'benign', 'spam');
title('Ratio of accepted outgoing friend requests');
xlabel('Ratio of Accepted Outgoing Requests');
ylabel('CDF');
print(gcf, '-djpeg', 'out_req_accepted_ratio.jpg');
close;
clear h1 h2;

%% Scatter plot of in vs. out ratio for benign & spam
ind_noNa_benign = find((req_info(:, 3) ~= 0) & (req_info(:, 4) ~= 0) & (req_info(:, 2) == 1));
ind_noNa_spam = find((req_info(:, 3) ~= 0) & (req_info(:, 4) ~= 0) & (req_info(:, 2) == -1));

scatter(req_info(ind_noNa_benign, 6), req_info(ind_noNa_benign, 7), 'r.');
hold on;
scatter(req_info(ind_noNa_spam, 6), req_info(ind_noNa_spam, 7), 'b.');
legend('benign', 'spam');
title('Scatter of incoming request accepted ratio vs. outgoing request accepted ratio');
xlabel('Ratio of Incoming Accepted Requests');
ylabel('Ratio of Outgoing Accepted Requests');
print(gcf, '-djpeg', 'scatter_in_and_out_ratio.jpg');
close;











