K-Means
======================

This is OpenCL Based K-Means clustering Implementation for Xilinx FPGA 
Devices. K-means clustering is a method of vector quantization, that 
is popular for cluster analysis in data mining. K-means clustering 
aims to partition n observations into k clusters in which each 
observation belongs to the cluster with the nearest mean, serving as
a prototype of the cluster.

The application sets two compute units by default so that it will fit 
all Xilinx Devices. For bigger Xilinx Devices, user can increase the 
number of Compute units in Makefile and can get better performance.
It is tested upto 6 compute units for ku115 device and got 
approximately 6x improvement with respect to single compute units, which 
shows that application is more compute bound (not memory bound) and 
performance is directly proportional to number of compute units.

__Note__: This application is ported from Rodinia

(https://www.cs.virginia.edu/~skadron/wiki/rodinia/)

__Usage__

 ./kmeans -i input_file [-m max_nclusters ] [-n min_nclusters] [-g global_size] [-c golden_file] [options]

	 -i filename      : file containing data to be clustered
	 -m max_nclusters : maximum number of clusters allowed [default=5]
	 -n min_nclusters : minimum number of clusters allowed [default=5]
	 -g global_size   : Specify global size [default=1]
	 -c golden_file   : Golden File for result comparison
	 -b               : input file is in binary format
	 -o               : output cluster center coordinates [default=off]


For Setup instructions, please refer: [setup.md][]

[setup.md]: setup.md

