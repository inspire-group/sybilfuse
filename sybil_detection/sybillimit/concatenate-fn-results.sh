#!/bin/bash

rm facebook-wall-t-fn.txt
rm facebook-links-t-fn.txt

for i in 10 50 100 200 300 400 500 1000 2000 3000 4000
do
Rscript fn-mean.r facebook-wall-fn-a"$i".txt  
cat temp  >> facebook-wall-t-fn.txt
done

for i in 10 50 100 200 300 400 500 1000 2000 4000 6000
do
Rscript fn-mean.r facebook-links-fn-a"$i".txt  
cat temp  >> facebook-links-t-fn.txt
done

