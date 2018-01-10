file <- commandArgs()[6]
print(file)
data <- read.table(file)
median_vector <- apply(data,2,mean)
write(median_vector,file="temp",ncol=22)
