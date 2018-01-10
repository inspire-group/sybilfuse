#!/bin/bash

g++ scalable-centralized-transformed-sybillimit-attack-edges.cpp -O3

count=0
for i in 10 50 100 200 300 400 500 1000 2000 4000 6000
do
	./a.out -g ../facebook-links.txt -a $i -r 2 > facebook-links-fn-a"$i".txt &
let count+=1
#[[ $((count%11)) -eq 0 ]] && wait 
done



