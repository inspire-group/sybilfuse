%%% Experiment in "Uncovering social network sybils in the wild"

%% load the data
cc = load('cc_sample_uniq');
[m, N] = size(cc);			% 19752, 2


%% Remove NaN and Inf value
ind = find(isfinite(cc(:, 3)) & ~isnan(cc(:, 3)));
cc_valid = cc(ind, :);
cc_benign = cc_valid(find(cc_valid(:, 2) == 1), 3);
cc_spam = cc_valid(find(cc_valid(:, 2) == -1), 3);

%% Box plot
x = [cc_benign; cc_spam];
g = [ones(size(cc_benign)); 2*ones(size(cc_spam))];
h = boxplot(x, g, 'notch', 'on', 'labels', {'benign', 'spam'});
title('Boxplot of clustering coefficients');
print(gcf, '-djpeg', 'box_ccoeff.jpg')
close;
clear h;

%% Scatter plot
plot(cc_benign, 0, 'ro', cc_spam, 0, 'b+')
title('Scatter plot of clustering coefficients');
legend('benign', 'spam')
print(gcf, '-djpeg', 'scatter_ccoeff.jpg')
close;


%% CDF plot
h1 = cdfplot(cc_benign);
set(h1, 'Color', 'red');
hold on;
h2 = cdfplot(cc_spam);
set(h2, 'Color', 'blue');
legend([h1 h2], 'benign', 'spam');
title('CDF plot of clustering coefficients');
xlabel('Clustering coefficient');
ylabel('CDF');
print(gcf, '-djpeg', 'cdf_ccoeff.jpg');
close;
clear h1 h2;










