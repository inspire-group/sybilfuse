#!/bin/bash

echo "Using t="$1" transformation graphs"

count=0
g++ static-scalable-sybillimit.cpp -O3
#for i in {1..20}
#do
#	./a.out -g ../facebook-wall-t"$1" -s 1500 -l $i -h 4 -r 2 > facebook-wall-t"$1"-fp-l"$i".txt &
#let count+=1
#[[ $((count%10)) -eq 0 ]] && wait 
#done

for i in {1..20}
do
	./a.out -g ../facebook-links-t"$1" -s 3000 -l $i -h 4 -r 2 > facebook-links-t"$1"-fp-l"$i".txt & 
let count+=1
[[ $((count%10)) -eq 0 ]] && wait 
done


