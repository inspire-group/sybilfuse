#!/bin/bash

for k in 100 200 500 1000 2000 
do
	for i in {5..20}
	#for i in {21..30}
	do
		for j in {1..5}
		do
			./sybilinfer 43953 $k $(($k*$i)) $j facebook-wall-format.txt 110 >> facebook-wall-$k.txt &	
		done
		wait
	done
done

