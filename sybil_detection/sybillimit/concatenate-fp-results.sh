#!/bin/bash

rm facebook-wall-fp.txt
rm facebook-wall-t5-fp.txt
rm facebook-wall-t10-fp.txt
rm facebook-links-fp.txt
rm facebook-links-t5-fp.txt
rm facebook-links-t10-fp.txt

for i in {1..20}
do
fp=`tail -1 facebook-wall-fp-l"$i".txt | awk '{print $4}'` 
echo $i $fp >> facebook-wall-fp.txt
done

for i in {1..20}
do
fp=`tail -1 facebook-wall-t5-fp-l"$i".txt | awk '{print $4}'` 
echo $i $fp >> facebook-wall-t5-fp.txt
done

for i in {1..20}
do
fp=`tail -1 facebook-wall-t10-fp-l"$i".txt | awk '{print $4}'` 
echo $i $fp >> facebook-wall-t10-fp.txt
done


for i in {1..20}
do
fp=`tail -1 facebook-links-fp-l"$i".txt | awk '{print $4}'` 
echo $i $fp >> facebook-links-fp.txt
done

for i in {1..20}
do
fp=`tail -1 facebook-links-t5-fp-l"$i".txt | awk '{print $4}'` 
echo $i $fp >> facebook-links-t5-fp.txt
done

for i in {1..20}
do
fp=`tail -1 facebook-links-t10-fp-l"$i".txt | awk '{print $4}'` 
echo $i $fp >> facebook-links-t10-fp.txt
done
