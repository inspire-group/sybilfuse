#!/bin/bash

#for k in 100 200 500 1000 2000 
for k in 500 1000 2000 
do
	for i in {5..20}
	#for i in {21..30}
	do
		for j in {1..5}
		do
			./a.out 63392 $k $(($k*$i)) $j facebook-links-format.txt 60 10 >> facebook-links-t10-$k.txt &	
		done
		wait
	done
done

