outfile <- commandArgs()[6]
for(i in c(100, 200, 500, 1000, 2000)){
	infile <- paste("facebook-links-t10-",i,sep="")
	infile <- paste(infile,".txt",sep="")
	data <- read.table(infile,h=F)
	for( j in 1:30){
		samples <- which(data[4]==i*j)
		if(length(samples) > 0){
			if(length(samples) == length(which(data[samples,5]==1))){
				result <- c(i,i*j,mean(data[samples,6]))
				print(result)
				write(result,file=outfile,ncol=3,append=T)
				break
			}
		}
	}
}
