#!/bin/bash

count=0
g++ static-scalable-sybillimit.cpp -O3
for i in {1..20}
do
	./a.out -g ../facebook-wall.txt -s 1500 -l $i -h 4 -r 2 > facebook-wall-fp-l"$i".txt &
let count+=1
[[ $((count%10)) -eq 0 ]] && wait 
done

for i in {1..20}
do
	./a.out -g ../facebook-links.txt -s 3000 -l $i -h 4 -r 2 > facebook-links-fp-l"$i".txt & 
let count+=1
[[ $((count%10)) -eq 0 ]] && wait 
done


