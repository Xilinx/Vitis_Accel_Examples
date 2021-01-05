HBM Bandwidth for large buffers using RAMA IP
=============================================

This is a HBM bandwidth example using a pseudo random 1024 bit data access pattern to mimic Ethereum Ethash workloads. The design contains 4 compute units of a kernel, 2 with and 2 without RAMA IP. To maximize bandwidth the pseudo channels are used in  P2P like configuration - See https://developer.xilinx.com/en/articles/maximizing-memory-bandwidth-with-vitis-and-xilinx-ultrascale-hbm-devices.html for more information on HBM memory access configurations. Each buffer is 1 GB large requiring 4 HBM banks. Since each CU requires 4 buffers, the .cfg file is allocating the buffers to all the 32 HBM banks. The host application runs the compute units concurrently to measure the overall bandwidth between kernel and HBM Memory. For applications requiring random data access, performance can vary significantly depending on the application requirements (for example, the ratio of read and write operations, minimum data word size, and memory size). The RAMA IP addresses such problems by significantly improving memory access efficiency in cases where the required memory exceeds 256 MB (one HBM pseudo-channel).

HBM is a high performance RAM interface for 3D-stacked DRAM. HBM can
provide very high bandwidth greater than **400 GB/s** with low power
consumption (HBM2 ~ 20W vs GDDR5 ~ 100W). These 32 memory resources
referenced as HBM [0:31] by v++ are accessed via 16 memory controllers.

HBM memory must be associated to respective kernel I/O ports using
``sp`` option. We need to add mapping between HBM memory and I/O ports
in ``krnl_vaddmul.cfg`` file

::

   sp=krnl_vaddmul_1.in1:HBM[0:3]
   sp=krnl_vaddmul_1.in2:HBM[4:7]
   sp=krnl_vaddmul_1.out_add:HBM[8:11]
   sp=krnl_vaddmul_1.out_mul:HBM[12:15]
   sp=krnl_vaddmul_2.in1:HBM[16:19]
   sp=krnl_vaddmul_2.in2:HBM[20:23]
   sp=krnl_vaddmul_2.out_add:HBM[24:27]
   sp=krnl_vaddmul_2.out_mul:HBM[28:31]
   sp=krnl_vaddmul_3.in1:HBM[0:3]
   sp=krnl_vaddmul_3.in2:HBM[4:7]
   sp=krnl_vaddmul_3.out_add:HBM[8:11]
   sp=krnl_vaddmul_3.out_mul:HBM[12:15]
   sp=krnl_vaddmul_4.in1:HBM[16:19]
   sp=krnl_vaddmul_4.in2:HBM[20:23]
   sp=krnl_vaddmul_4.out_add:HBM[24:27]
   sp=krnl_vaddmul_4.out_mul:HBM[28:31]
   nk=krnl_vaddmul:4

To improve the random access bandwidth, in ``krnl_vaddmul.cpp`` the
``latency`` and ``num_read_outstanding`` switches have been added to the
``HLS INTERFACE`` definition.

::

   void krnl_vaddmul(
       const v_dt *in1,             // Read-Only Vector 1
       const v_dt *in2,             // Read-Only Vector 2
       v_dt *out_add,               // Output Result for ADD
       v_dt *out_mul,               // Output Result for MUL
       const unsigned int size,     // Size in integer
       const unsigned int num_times // Running the same kernel operations num_times
       ) {
   #pragma HLS INTERFACE m_axi port = in1 offset = slave bundle = gmem0 latency = 300 num_read_outstanding=64
   #pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem1 latency = 300 num_read_outstanding=64

To further improve the random access bandwidth, the
``postSysLink.tcl`` file has been added to the Makefile. As can be seen below, the RAMA IP has only been added to the buffers of the first 2 CUs.

::

   hbm_memory_subsystem::ra_master_interface [get_bd_intf_pins /krnl_vaddmul_1/m_axi_gmem0] [get_bd_cells /hmss_0]
   hbm_memory_subsystem::ra_master_interface [get_bd_intf_pins /krnl_vaddmul_1/m_axi_gmem1] [get_bd_cells /hmss_0]
   hbm_memory_subsystem::ra_master_interface [get_bd_intf_pins /krnl_vaddmul_1/m_axi_gmem2] [get_bd_cells /hmss_0]
   hbm_memory_subsystem::ra_master_interface [get_bd_intf_pins /krnl_vaddmul_1/m_axi_gmem3] [get_bd_cells /hmss_0]
   hbm_memory_subsystem::ra_master_interface [get_bd_intf_pins /krnl_vaddmul_2/m_axi_gmem0] [get_bd_cells /hmss_0]
   hbm_memory_subsystem::ra_master_interface [get_bd_intf_pins /krnl_vaddmul_2/m_axi_gmem1] [get_bd_cells /hmss_0]
   hbm_memory_subsystem::ra_master_interface [get_bd_intf_pins /krnl_vaddmul_2/m_axi_gmem2] [get_bd_cells /hmss_0]
   hbm_memory_subsystem::ra_master_interface [get_bd_intf_pins /krnl_vaddmul_2/m_axi_gmem3] [get_bd_cells /hmss_0]

To see the benefit of HBM, user can look into the runtime logs and see
the overall throughput. Following is the real log reported comparing the throughput with and without RAMA IP while running
the design on U50 platform:

::

   Loading: './build_dir.hw.xilinx_u50_gen3x16_xdma_201920_3/krnl_vaddmul.xclbin'
   Trying to program device[0]: xilinx_u50_gen3x16_xdma_201920_3
   Device[0]: program successful!
   Creating a kernel [krnl_vaddmul:{krnl_vaddmul_1}] for CU(1)
   Creating a kernel [krnl_vaddmul:{krnl_vaddmul_2}] for CU(2)
   Creating a kernel [krnl_vaddmul:{krnl_vaddmul_3}] for CU(3)
   Creating a kernel [krnl_vaddmul:{krnl_vaddmul_4}] for CU(4)
   WITH RAMA IP -
   OVERALL THROUGHPUT = 97.0093 GB/s
   CHANNEL THROUGHPUT = 12.1262 GB/s
   WITHOUT RAMA IP -
   OVERALL THROUGHPUT = 8.25286 GB/s
   CHANNEL THROUGHPUT = 1.03161 GB/s
   TEST PASSED

