Port Width Widening
===================

This example shows how HLS introduces the capability of resizing the port width of the kernel interface ports for better resource utilization maintaining the performance.

**KEY CONCEPTS:** `Interface port width auto widening <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Automatic-Port-Width-Resizing>`__

**KEYWORDS:** m_axi_max_widen_bitwidth 

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc
 - Embedded ZCU102

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

1. KERNEL 1 - Default case (no explict settings) - By default, HLS gives single M_AXI interface to access all pointer arguments (i.e. a,b and res here) and default width would be the maximum width datatype (i.e. 64bit here due to uint64_t). 

.. code:: cpp

   void dot_product_1(const uint32_t *a, const uint32_t *b, uint64_t *res,
                      const int size, const int reps){
   loop_reps: for (int i = 0; i < reps; i++) {
    dot_product: for (int j = 0; j < size; j++) {
            res[j] = a[j] * b[j];
        }
    }
   }                      

2. KERNEL 2 - Auto port width widening when pipeline loop is fixed bound (i.e. DATA_WIDTH), HLS does auto port width widening when pipeline loop is fixed bound. Here pipeline loop dot_product_inner has fixed iteration of DATA_WIDTH, as a result, HLS is widening M_AXI port width to 512bit (Maximum). 

.. code:: cpp

   #define DATA_WIDTH 16
   void dot_product_2(const uint32_t *a, const uint32_t *b, uint64_t *res,
                      const int size, const int reps){
        dot_product_outer: for (int j = 0; j < size; j += DATA_WIDTH) {
        dot_product_inner: for (int k = 0; k < DATA_WIDTH; k++) {
                res[j + k] = a[j + k] * b[j + k];
            }
        }
    }   

3. KERNEL 3 - pragmas specifying multiple bundles to infer multiple M_AXI interfaces. Here we are providing gmem0 to pointer a (Read) and res (write) and gmem1 to pointer b(read). 

.. code:: cpp

   #define DATA_WIDTH 16
   void dot_product_3(const uint32_t *a, const uint32_t *b, uint64_t *res,
                      const int size, const int reps) {
   #pragma HLS INTERFACE m_axi port=a bundle=gmem0
   #pragma HLS INTERFACE m_axi port=b bundle=gmem1
   #pragma HLS INTERFACE m_axi port=res bundle=gmem0
   dot_product_outer: for (int j = 0; j < size; j += DATA_WIDTH) {
        dot_product_inner: for (int k = 0; k < DATA_WIDTH; k++) {
                res[j + k] = a[j + k] * b[j + k];
            }
        }
    }

4. KERNEL 4 - Along with pragma in kernel, user can explicitly provide port width in tcl file (hls_config.tcl) as specified below: 

.. code:: cpp

   config_interface -m_axi_max_widen_bitwidth 512


The interface size setting need to be specified in hls_config.tcl file. We included this tcl file in our krnl_dot_product_4.cfg file and by using
the ``--config`` tag in the kernel compile stage we specify the m_axi interface size.

Following is the content of krnl_dot_product_4.cfg file

.. code:: cpp

   [hls]
   pre_tcl=hls_config.tcl


5. KERNEL 5 - Interface pragma based port width allocation to each bundle. User can directly specifying portwidth to each M_AXI ports. Here user is setting 512 bit width to gmem0 and 256 bitwidth to gmem1. 

.. code:: cpp

   void dot_product_5(const uint32_t *a, const uint32_t *b, uint64_t *res,
                      const int size, const int reps) {

   #pragma HLS INTERFACE m_axi port=a bundle=gmem0 max_widen_bitwidth=512
   #pragma HLS INTERFACE m_axi port=b bundle=gmem1 max_widen_bitwidth=256
   #pragma HLS INTERFACE m_axi port=res bundle=gmem0 


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