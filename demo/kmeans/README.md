K Means
======================

This is HLS C based K-Means clustering Implementation for Xilinx FPGA Devices. K-means clustering is a method of vector quantization, that is popular for cluster analysis in data mining. K-means clustering aims to partition n observations into k clusters in which each observation belongs to the cluster with the nearest mean, serving as a prototype of the cluster.

Command line argument flags:

-x  :    Used to specify kernel xclbin

-i  :    File containing data to be clustered

-c  :    Golden file for comparison

-n  :    Used to specify number of clusters

-o  :    Output cluster center coordinates(default=off)



***KEY CONCEPTS:*** K-Means, Multiple compute units

***KEYWORDS:*** DATAFLOW, ARRAY PARTITION

## EXCLUDED PLATFORMS
Platforms containing following strings in their names are not supported for this example :
```
zc
vck
samsung
```

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
data/100
data/100.gold_c10
data/100.gold_c5
src/fpga_kmeans.cpp
src/fpga_kmeans.h
src/host.cpp
src/kmeans.h
src/kmeans_clustering_cmodel.c
src/kmeans_config.h
src/krnl_kmeans.cpp
src/krnl_kmeans.h
src/timer.h
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./kmeans -x <krnl_kmeans XCLBIN> -i ./data/100 -c ./data/100.gold_c10 -n 10
```

##  COMMANDS FOR WINDOWS FLOW
Once the environment has been configured, run the following commands : 
```
cd cmake_build
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug -DXILINX_XRT=<set xilinx xrt path> -DOCL_ROOT=<set ocl root path>
cmake --build . --verbose --config Debug --target install

For Example : 
cd cmake_build
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug -DXILINX_XRT=C:\Xilinx\XRT -DOCL_ROOT=C:\Xilinx\XRT\ext
cmake --build . --verbose --config Debug --target install
```
