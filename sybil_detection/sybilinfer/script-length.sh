#!/bin/bash

for l in {6..10}
do
	#./a.out 43953 0 0 1 facebook-wall-format.txt $(($l*10)) >> facebook-wall-length.txt & 
	#./a.out 43953 0 0 1 facebook-wall-t3-r1.txt $(($l*10)) >> facebook-wall-t3-length.txt & 
	#./a.out 43953 0 0 1 facebook-wall-t5-r1.txt $(($l*10)) >> facebook-wall-t5-length.txt & 
	#./a.out 43953 0 0 1 facebook-wall-t10-r1.txt $(($l*10)) >> facebook-wall-t10-length.txt & 
	#./a.out 63392 0 0 1 facebook-links-format.txt $(($l*10)) >> facebook-links-length.txt & 
	#./a.out 63392 0 0 1 facebook-links-format.txt $(($l*10)) >> facebook-links-length.txt & 
	#./a.out 63392 0 0 1 facebook-links-format.txt $(($l*10)) 3 >> facebook-links-t3-length.txt & 
	./a.out 63392 0 0 1 facebook-links-format.txt $(($l*10)) 5 >> facebook-links-t5-length.txt & 
	./a.out 63392 0 0 1 facebook-links-format.txt $(($l*10)) 10 >> facebook-links-t10-length.txt & 
done

