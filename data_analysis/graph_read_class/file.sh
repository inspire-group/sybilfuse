#!/bin/bash
l=3
i=0
while [ $i -lt $l ]
do
	cat "output$i"
	rm "output$i"
	let i++
done > output
