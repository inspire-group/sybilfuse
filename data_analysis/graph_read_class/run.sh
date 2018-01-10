#!/bin/bash
l=3	# 
n=5	# total number of nodes
s=$(($((n/l)) + 1))
i=0
OUTPREFIX="output"
while [ $i -lt $l ]
do
#	echo $i
	OUT="$OUTPREFIX$i"
#	echo $OUT
	START=$(($i * $s))
#	echo $START
	if [ $((($i + 1) * $s - 1)) -lt $(($n - 1)) ]
	then
		END=$((($i + 1) * $s - 1))	
	else
		END=$(($n - 1))
	fi
#	echo $END
				
	./test g2 g2_act g2_sus  $START $END $OUT &
#	./test data act suspend $START $END $OUT &
	let i++
done
