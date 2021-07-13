K Means
=======

This is HLS C based K-Means clustering Implementation for Xilinx FPGA Devices. K-means clustering is a method of vector quantization, that is popular for cluster analysis in data mining. K-means clustering aims to partition n observations into k clusters in which each observation belongs to the cluster with the nearest mean, serving as a prototype of the cluster.

Command line argument flags:

-x  :    Used to specify kernel xclbin

-i  :    File containing data to be clustered

-c  :    Golden file for comparison

-n  :    Used to specify number of clusters

-o  :    Output cluster center coordinates(default=off)



**KEY CONCEPTS:** K-Means, `Multiple compute units <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#dqz1555367565037>`__

**KEYWORDS:** DATAFLOW, ARRAY PARTITION

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   vck
   u200_gen3x16
   u30
   nodma
   samsung

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

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
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./kmeans -x <krnl_kmeans XCLBIN> -i ./data/100 -c ./data/100.gold_c10 -n 10

DETAILS
-------

The application sets two compute units by default so that it will fit
all Xilinx Devices. For bigger Xilinx Devices, user can increase the
number of Compute units in krnl_kmeans.cfg, kmeans_config.h and
config.mk files for achieving better performance. It is tested upto 8
compute units for U200 device and got better results when compared to
single compute units, which shows that application is more compute bound
(not memory bound).

Following is the real log reported while running the design on U200
platform with 8 compute units:

::

   ------------------------------------------------------
     Performance Summary                                 
   ------------------------------------------------------
     Device Initialization      :    3224.1098 ms
     Buffer Allocation          :      33.4063 ms
   ------------------------------------------------------
     Compute Memberships        :   11490.2810 ms
     Update Delta               :    1387.0610 ms
     Update Centers             :    1745.7366 ms
     Update Clusters            :      41.0705 ms
     Total K-Means Compute Time :   15208.3733 ms
   ------------------------------------------------------

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.