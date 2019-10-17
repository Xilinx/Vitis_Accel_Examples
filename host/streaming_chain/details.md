Stream Chain Matrix Multiplication (C)
======================================
This example focuses on using the ap_ctrl_chain that implements a set of
block-level control ports to start the design operation, continue operation, and
indicate when the design is idle, done, and ready for new input data. The
ap_ctrl_chain interface mode is similar to ap_ctrl_hs but provides an additional input signal 
ap_continue to apply back pressure. Xilinx recommends using the ap_ctrl_chain block-level I/O protocol when 
chaining Vivado HLS blocks together.

In this example, we are having a series of Vivado HLS blocks chained together to
perform this operation. For declaring this protocol, the kernel interface should
contain this special pragma:

```c++
#pragma HLS INTERFACE s_axilite port = return bundle = control
#pragma HLS INTERFACE ap_ctrl_chain port = return bundle = control
```
