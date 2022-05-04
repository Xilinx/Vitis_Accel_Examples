Validate Examples
==================================
The main purpose of validate kernels is to test basic HW functionality during bring up of a new platform or hardware board. The bandwidth kernels allow you to measure maximum bandwidth achievable for a given connection.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[bandwidth_test/][]|This Bandwidth Example test the maximum possible bandwidth between Kernel and Global Memory.|__Key__ __Concepts__<br> - [burst read and write](https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI-Burst-Transfers)<br> - Bandwidth<br>
[combine_bw_hm/][]|This is a simple design that verifies if the platform has basic functionalities It also tests the possible bandwidth between Kernel and Global Memory and validates direct host memory access from kernel.|
[hostmemory_test/][]|This host memory design explains how direct host memory access can be done by the kernel.|__Key__ __Concepts__<br> - host memory<br> - address translation unit<br>__Keywords__<br> - XCL_MEM_EXT_HOST_ONLY<br> - HOST[0]
[validate_aie_pl/][]|Xilinx AIE Validate example|
[validate_embedded/][]|This is a simple design that verifies if the platform has basic functionalities. It also tests the possible bandwidth between Kernel and Global Memory.|__Key__ __Concepts__<br> - Bandwidth<br>
[verify_test/][]|This is a simple design to verify that the platform has basic functionality.|
[ps_validate_test/][]|This is a simple PS kernel to verify that the platform has basic PS kernel functionality.|
[ps_bandwidth_test/][]|This is a PS kernel conrolling PL bandwidth kernel test.|
[ps_aie_gmio_test/][]|This is a PS kernel controlling AIE graph.|

[.]:.
[bandwidth_test/]:bandwidth_test/
[combine_bw_hm/]:combine_bw_hm/
[hostmemory_test/]:hostmemory_test/
[validate_aie_pl/]:validate_aie_pl/
[validate_embedded/]:validate_embedded/
[verify_test/]:verify_test/
[ps_validate_test/]:ps_validate_test/
[ps_bandwidth_test/]:ps_bandwidth_test/
[ps_aie_gmio_test/]:ps_aie_gmio_test/
