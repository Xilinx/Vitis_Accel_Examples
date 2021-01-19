Kmeans
======

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
