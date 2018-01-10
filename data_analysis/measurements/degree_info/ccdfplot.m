function [handleCDF,stats] = ccdfplot(x)
%CDFPLOT Display an empirical cumulative distribution function.
%   CDFPLOT(X) plots an empirical cumulative distribution function (CDF) 
%   of the observations in the data sample vector X. X may be a row or 
%   column vector, and represents a random sample of observations from 
%   some underlying distribution.
%
%   H = CDFPLOT(X) plots F(x), the empirical (or sample) CDF versus the
%   observations in X. The empirical CDF, F(x), is defined as follows:
%
%   F(x) = (Number of observations <= x)/(Total number of observations)
%
%   for all values in the sample vector X. If X contains missing data
%   indicated by NaN's (IEEE arithmetic representation for
%   Not-a-Number), the missing observations will be ignored.
%
%   H is the handle of the empirical CDF curve (a Handle Graphics 'line'
%   object). 
%
%   [H,STATS] = CDFPLOT(X) also returns a statistical summary structure
%   with the following fields:
%
%      STATS.min    = minimum value of the vector X.
%      STATS.max    = maximum value of the vector X.
%      STATS.mean   = sample mean of the vector X.
%      STATS.median = sample median (50th percentile) of the vector X.
%      STATS.std    = sample standard deviation of the vector X.
%
%   In addition to qualitative visual benefits, the empirical CDF is 
%   useful for general-purpose goodness-of-fit hypothesis testing, such 
%   as the Kolmogorov-Smirnov tests in which the test statistic is the 
%   largest deviation of the empirical CDF from a hypothesized theoretical 
%   CDF.
%
%   See also QQPLOT, KSTEST, KSTEST2, LILLIETEST.

% Copyright 1993-2011 The MathWorks, Inc.


% Get sample cdf, display error message if any
[yy,xx,~,~,eid] = cdfcalc(x);
if isequal(eid,'VectorRequired')
    error(message('stats:cdfplot:VectorRequired'));
elseif isequal(eid,'NotEnoughData')
    error(message('stats:cdfplot:NotEnoughData'));
end

% Create vectors for plotting
k = length(xx);
n = reshape(repmat(1:k, 2, 1), 2*k, 1);
xCDF    = [-Inf; xx(n); Inf];
yCDF    = [0; 0; yy(1+n)];

%
% Now plot the sample (empirical) CDF staircase.
%

hCDF = plot(xCDF , 1-yCDF); % we modify here to plot CCDF
if (nargout>0), handleCDF=hCDF; end
grid  ('on')
xlabel(getString(message('stats:cdfplot:LabelX')))
ylabel(getString(message('stats:cdfplot:LabelFx')))
title (getString(message('stats:cdfplot:Title')))

%
% Compute summary statistics if requested.
%

if nargout > 1
   stats.min    =  min(x);
   stats.max    =  max(x);
   stats.mean   =  mean(x);
   stats.median =  median(x);
   stats.std    =  std(x);
end