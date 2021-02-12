Port Width Widening
===================

This example shows how HLS introduces the capability of resizing the port width of the kernel interface ports for better resource utilization maintaining the performance.

**KEY CONCEPTS:** Interface port width auto widening

**KEYWORDS:** m_axi_max_widen_bitwidth 

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/dot_product_1.cpp
   src/dot_product_2.cpp
   src/dot_product_3.cpp
   src/dot_product_4.cpp
   src/dot_product_5.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./port_width_widening <krnl_port_widen XCLBIN>

DETAILS
-------

This example introduces the capability of how Vitis HLS can configure
the size of kernel interface ports.

A few rules must be kept in mind by the user -

1. Pragma option value defined by user has higher priority than TCL.

2. The max_widen_bitwidth value should be in range [0, 1024], and it must be either 0 or a power of 2. If not satisfied, this value setting will be ignored.

3. If some ports are bundled together, one bundle name can have only one max_widen_bitwidth value. Therefore, if each port of a bundle has a different width, the maximum width under the bundle will be taken as the width for each of the ports.

Vitis kernel can have m_axi interface which will be used by host application to configure the kernel. We have 5 kernels here each having the port width set in a a different way -

1. KERNEL 1 - No pragmas, No tcl file, Number of iterations is a variable - The ports in this case have no fields specifying the number of bundles or port_width. The number of iterations are variable and passed from the host code. Therefore, default values for both will be taken for these i.e. 1 bundle with each port having a width of 64. 

.. code:: cpp

   void dot_product_1(const uint32_t *a, const uint32_t *b, uint64_t *res,
                      const int size, const int reps)

2. KERNEL 2 - No pragmas, No tcl file, Number of iterations is predefined in the kernel - The ports in this case have no fields specifying the number of bundles or port_width. The number of iterations are predefined in the kernel code. Therefore, default values for both will be taken for these i.e. 1 bundle with each port having a width of 512. 

.. code:: cpp

   #define DATA_WIDTH 16

   void dot_product_2(const uint32_t *a, const uint32_t *b, uint64_t *res,
                      const int size, const int reps)

3. KERNEL 3 - pragmas specifying bundle but not port width, No tcl file - The ports in this case have fields specifying the number of bundles but the port_width is still not set. Therefore, the number of bundles will be taken from the pragmas and port_width will be taken as default value. 

.. code:: cpp

   void dot_product_3(const uint32_t *a, const uint32_t *b, uint64_t *res,
                      const int size, const int reps) {

   #pragma HLS INTERFACE m_axi port=a bundle=gmem0
   #pragma HLS INTERFACE m_axi port=b bundle=gmem1
   #pragma HLS INTERFACE m_axi port=res bundle=gmem0

4. KERNEL 4 - pragmas specifying bundle allocation , tcl file specifying port width - The ports in this case have fields specifying the number of bundles. Therefore, the number of bundles will be taken from the pragmas and port_width will be taken fom the tcl file. 

.. code:: cpp

   void dot_product_4(const uint32_t *a, const uint32_t *b, uint64_t *res,
                      const int size, const int reps) {

   #pragma HLS INTERFACE m_axi port=a bundle=gmem0
   #pragma HLS INTERFACE m_axi port=b bundle=gmem1
   #pragma HLS INTERFACE m_axi port=res bundle=gmem0

The port_width will be set by the hls_config.tcl file.

.. code:: cpp

   config_interface -m_axi_max_widen_bitwidth 512

5. KERNEL 5 - pragmas specifying bundle allocation and port width - The ports in this case have fields specifying the number of bundles and port_width. Therefore, the number of bundles and port_width will be taken from the pragmas. 

.. code:: cpp

   void dot_product_5(const uint32_t *a, const uint32_t *b, uint64_t *res,
                      const int size, const int reps) {

   #pragma HLS INTERFACE m_axi port=a bundle=gmem0 max_widen_bitwidth=512
   #pragma HLS INTERFACE m_axi port=b bundle=gmem1 max_widen_bitwidth=256
   #pragma HLS INTERFACE m_axi port=res bundle=gmem0 

The interface size setting need to be specified in hls_config.tcl file.
We include this tcl file in our krnl_dot_product_4.cfg file and by using
the ``--config`` tag in the kernel compile stage we specify the m_axi
interface size.

Following is the content of cfg file

.. code:: cpp

   [advanced]
   prop=solution.hls_pre_tcl=hls_config.tcl

Below are the resource numbers while running the design on U200 platform:

============= =========== =========== ============= ============ ==== ==== ===
Design        port_size_a port_size_b port_size_res Bundle_Count BRAM LUT  DSP
============= =========== =========== ============= ============ ==== ==== ===
dot_product_1 64          64          64            1            2    2237 3 
dot_product_2 512         512         512           1            15   3665 48
dot_product_3 512         512         512           2            23   5319 48
dot_product_4 512         512         512           2            23   5316 48
dot_product_5 512         256         512           2            19   4939 48
============= =========== =========== ============= ============ ==== ==== ===

Following is the real log reported while running the design on U200 platform:

========================== =====================
Kernel(1000000 iterations) Wall-Clock Time (sec)
========================== =====================
dot_product_1              66.8994              
dot_product_2              2.57683              
dot_product_3              1.14736              
dot_product_4              1.14755              
dot_product_5              1.26024              
========================== =====================

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.