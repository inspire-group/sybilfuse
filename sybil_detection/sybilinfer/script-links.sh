#!/bin/bash

for k in 100 200 500 1000 2000 
do
	for i in {5..20}
	do
		for j in {1..5}
		do
			./sybilinfer 63392 $k $(($k*$i)) $j facebook-links-format.txt 70 >> facebook-links-$k.txt &	
		done
		wait
	done
done

