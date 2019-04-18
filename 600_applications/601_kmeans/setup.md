K-Means
======================

This file contains the following sections:

1. SUPPORTED PLATFORMS
2. DESIGN FILES
3. COMMAND LINE ARGUMENTS


## 1. SUPPORTED PLATFORMS
Board | Device Name | Software Version
------|-------------|-----------------
Xilinx Alveo U250|xilinx_u250_xdma_201820_1|SDx 2018.3
Xilinx Virtex UltraScale+ VCU1525|xilinx_vcu1525_dynamic|SDx 2018.3
Xilinx Alveo U200|xilinx_u200_xdma_201820_1|SDx 2018.3


## 2. DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
data/100
data/100.gold_c5
src/cluster.c
src/fpga_kmeans.cpp
src/fpga_kmeans.h
src/host.cpp
src/kmeans.cl
src/kmeans.h
src/kmeans_clustering_cmodel.c
src/rmse.c
```

## 3. COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host_kmeans -x <kmeans XCLBIN> -i ./data/100 -c ./data/100.gold_c5 -m 5 -n 5 -g 2
```

