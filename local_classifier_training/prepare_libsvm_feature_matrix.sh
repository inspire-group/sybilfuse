# Libsvm installation & usage


# 1. Install Libsvm
# 2. Install eval tool (https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/eval/index.html). Use AUC for evaluation metric.
# 	Note that libsvm treats label +1 as positive examples in the FPR, FNR, etc. computation. In the Twitter dataset, 
# 	we also use +1 for benign nodes, and -1 for Sybil nodes. However, in security convention, Sybils are often treated
#	as positive examples.

# 3. Prepare input feature file

# Extract feature columns
cut -d " " -f1-1 Twitter_ccoeff.txt > node_id.txt

cut -d " " -f2-2 Twitter_ccoeff.txt > ccoeff_only.txt

cut -d " " -f2-2 Twitter_incoming.txt > in_ratio_only.txt

cut -d " " -f2-2 Twitter_outgoing.txt > out_ratio_only.txt

cut -d " " -f2-2 node_label.txt > label_only.txt

# Prepare the original feature matrix node_feature.txt
# format: id label in_ratio out_ratio ccoeff
paste -d " " node_id.txt label_only.txt in_ratio_only.txt out_ratio_only.txt ccoeff_only.txt > node_feature.txt


# Preapre libsvm format feature matrix node_feature_libsvm_a_s_d.txt
# format: label 1:in_ratio 2:out_ratio 3:ccoeff
awk '{printf("%d %d:%f %d:%f %d:%f\n", $2, 1, $3, 2, $4, 3, $5)}' node_feature.txt > node_feature_libsvm_a_s_d.txt

# Scale and subset to prepare features for libsvm training
# Scale features in [0,1]
# Need to local svm-scale path
./svm-scale -l 0 -u 1 -s scale_range node_feature_libsvm_a_s_d.txt > node_feature_libsvm_a_s_d.scale &

# Remove the deleted accounts (i.e., label = 0) from node_feature_libsvm_a_s_d.scale => node_feature_libsvm_a_s.scale
cat node_feature_libsvm_a_s_d.scale | awk '$1 != 0 { print $0 }' > node_feature_libsvm_a_s.scale

# Subset selection on node_feature_libsvm_a_s.scale for train & test datasets
# Need to local tools/subset.py path
# -s means stratified selection (i.e., keep the original class distributions)
python tools/subset.py -s 0 node_feature_libsvm_a_s.scale 100000 node_libsvm_train node_libsvm_test &


# 4. Grid search on parameters c & g
python libsvm-3.22/tools/grid.py node_libsvm_train
# best c=2048.0, g=0.0001220703125, rate=73.2769

# 5. Train a model file
.svm-train -c 2048 -g 0.0001220703125 -b 1 node_libsvm_train node_libsvm_train.model

# 6. Make predictions by enabling probabilistic outputs
# Uncomment predict(input,output); in svm-predict.cpp and comment binary_class_predict(input, output);
.svm-predict -b 1 node_feature_libsvm_a_s_d.scale node_klibsvm_train.model prediction_file









