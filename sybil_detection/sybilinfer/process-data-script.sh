for i in 3 5 10
do
	for j in {1..5}
	do
		./a.out -g ../facebook-wall.txt -t $i -r $j > facebook-wall-t$i-r$j.txt 
		./a.out -g ../facebook-links.txt -t $i -r $j > facebook-links-t$i-r$j.txt 
	done
done
