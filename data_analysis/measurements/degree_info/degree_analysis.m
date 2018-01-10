% This script will perform some measurements & analysis on the twitter dataset
%% 1. CCDF of node degree
clear,clc
all = importdata('all_ratio');
benign = importdata('benign_ratio');
spam = importdata('spam_ratio');
all_degree = all(:,2);
benign_degree = benign(:,2);
spam_degree = spam(:,2);

h1 = ccdfplot(all_degree);
set(h1, 'Color', 'red');
% legend('All');
hold on;
h2 = ccdfplot(benign_degree);
set(h2, 'Color', 'blue');
% legend('Benign');
hold on;
h3 = ccdfplot(spam_degree);
set(h3, 'Color', 'green');
% legend('Spam');
title('CCDF of node degree');
xlabel('Number of neighbors (degree)');
ylabel('Frequency');
set(gca, 'XScale','log');
set(gca, 'YScale','log');

%% 2. Neighbor identity
benign_spam = benign(:,3); % a benign node who is connected with a spam neighbor
benign_benign = benign(:,4);
spam_spam = spam(:,3);
spam_benign = spam(:,4);

% CCDF of number of spam neighbors
h1 = ccdfplot(benign_spam);
set(h1, 'Color', 'red');
hold on;
h2 = ccdfplot(spam_spam);
set(h2, 'Color', 'blue');
title('CCDF of number of spam neighbors');
xlabel('Number of spam neighbors');
ylabel('Frequency');
set(gca, 'XScale','log');
set(gca, 'YScale','log');

% CCDF of number of benign neighbors
h1 = ccdfplot(benign_benign);
set(h1, 'Color', 'red');
hold on;
h2 = ccdfplot(spam_benign);
set(h2, 'Color', 'blue');
title('CCDF of number of benign neighbors');
xlabel('Number of benign neighbors');
ylabel('Frequency');
set(gca, 'XScale','log');
set(gca, 'YScale','log');

% Measurements on spammers' different strategies
scatter(spam_degree, spam_spam);
title('Scatter plot of spam to spam');
xlabel('Degree');
ylabel('Number of spam neighbors');
set(gca, 'XScale','log');
set(gca, 'YScale','log');

scatter(spam_degree, spam_benign);
title('Scatter plot of spam to benign');
xlabel('Degree');
ylabel('Number of benign neighbors');
set(gca, 'XScale','log');
set(gca, 'YScale','log');

%% Ratio of number of spam connected over number of benign connected
benign_ratio = benign(:,5);
spam_ratio = spam(:,5);

% For spam nodes
ind_spam_inf = find(spam_ratio == Inf); % n/0
ind_spam_nan = find(spam_spam == 0 & spam_benign == 0); % 0/0
length(ind_spam_inf)
length(ind_spam_nan)
max(spam_spam(ind_spam_inf))
min(spam_spam(ind_spam_inf))

% For benign nodes
ind_benign_inf = find(benign_ratio == Inf); % n/0
ind_benign_nan = find(benign_spam == 0 & benign_benign == 0); % 0/0
length(ind_benign_inf)
length(ind_benign_nan)
max(benign_spam(ind_benign_inf))
min(benign_spam(ind_benign_inf))

% For valid ratio scores
ind_spam = 1:length(spam_ratio);
ind_spam_temp = setdiff(ind_spam, ind_spam_inf);
ind_spam_valid = setdiff(ind_spam_temp, ind_spam_nan);
ind_benign = 1:length(benign_ratio);
ind_benign_temp = setdiff(ind_benign, ind_benign_inf);
ind_benign_valid = setdiff(ind_benign_temp, ind_benign_nan);

mean_spam_ratio_valid = mean(spam_ratio(ind_spam_valid));
mean_benign_ratio_valid = mean(benign_ratio(ind_benign_valid));
% mean_benign = mean(benign_ratio)
% mean_spam = mean(spam_ratio)

%% mean values
mean_spam_degree = mean(spam_degree)
mean_spam_spam = mean(spam_spam)
mean_spam_benign = mean(spam_benign)
mean_spam_ratio_valid

mean_benign_degree = mean(benign_degree)
mean_benign_spam = mean(benign_spam)
mean_benign_benign = mean(benign_benign)
mean_benign_ratio_valid
