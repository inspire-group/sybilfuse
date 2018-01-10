#!/usr/bin/python

import sys
import os
import random

# Take the active.txt, suspend.txt as inputs and generate randomly-sampled training & test files
# active.txt: active Twitter accounts
# suspend.txt: suspended Twitter accounts

def output_lsts_to_file(benign_lst, sybil_lst, outfile_path):
	outfile = open(outfile_path, "w")
	for i in benign_lst:
		outfile.write(str(i) + " ")
	outfile.write("\n")
	for i in sybil_lst:
		outfile.write(str(i) + " ")
	outfile.write("\n")
	outfile.close()

def output_lsts_to_file_v2(benign_lst, sybil_lst, del_lst, outfile_path):
	outfile = open(outfile_path, "w")
	for i in benign_lst:
		outfile.write(str(i) + " ")
	outfile.write("\n")
	for i in sybil_lst:
		outfile.write(str(i) + " ")
	outfile.write("\n")
	for i in del_lst:
		outfile.write(str(i) + " ")
	outfile.write("\n")
	outfile.close()


def generate_train_test_files(num_sample, benign_lst, sybil_lst, train_file_path, test_file_path):
	benign_train_lst = list()
	benign_test_lst = list()
	sybil_train_lst = list()
	sybil_test_lst = list()
	while len(benign_train_lst) < num_sample:
		index = random.randint(0, len(benign_lst)-1)
		benign_train_lst.append(benign_lst[index])
	while len(sybil_train_lst) < num_sample:
		index = random.randint(0, len(sybil_lst)-1)
		sybil_train_lst.append(sybil_lst[index])
	benign_test_lst = list(set(benign_lst)-set(benign_train_lst))
	sybil_test_lst = list(set(sybil_lst)-set(sybil_train_lst))
	output_lsts_to_file(benign_train_lst, sybil_train_lst, train_file_path)
	output_lsts_to_file(benign_test_lst, sybil_test_lst, test_file_path)



def main():
	random.seed(0)

	# Read inputs
	act_lst = list()
	sus_lst = list()
	del_lst = list()

	act_file = open("active.txt", "r") # may need to reconfigure the path
	sus_file = open("suspend.txt", "r")

	for line in act_file.readlines():
		act_lst.append(int(line))

	for line in sus_file.readlines():
		sus_lst.append(int(line))

	N = 21297772 # This is the number of nodes in the Twitter graph
	# for i in range(N):
	# 	if i not in act_lst and i not in sus_lst:
	# 		del_lst.append(i)

	all_lst = range(N)
	del_lst = list(set(all_lst)-set(act_lst)-set(sus_lst))		

	act_lst.sort()
	sus_lst.sort()
	del_lst.sort()

	output_lsts_to_file(act_lst, sus_lst, "Twitter_large_un_label_a_s.txt")
	output_lsts_to_file_v2(act_lst, sus_lst, del_lst, "Twitter_large_un_label_a_s_d.txt")

	# Generate train and test files
	num_sample = 3000
	generate_train_test_files(num_sample, act_lst, sus_lst, "train_a_s_3K.txt", "test_a_s_3K.txt")





if __name__ == "__main__":
	main()

