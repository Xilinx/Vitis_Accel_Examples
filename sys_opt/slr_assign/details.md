SLR Assignment
===============

This example demonstrates how kernels can be assigned to different SLRs(Super Logic Region) during the kernel linking stage.

Actual kernel resource utilization values can be used to help distribute CUs across SLRs to reduce congestion in any one SLR. Xilinx recommends connecting to a DDR memory resource in the same SLR as the CU. This reduces competition for the limited `SLR-crossing` connection resources. In addition, connections between SLRs use super long line (SLL) routing resources, which incurs a greater delay than a standard intra-SLR routing.

It might be necessary to cross an SLR region to connect to a DDR resource in a different SLR. However, if both the and the directives are explicitly defined, the tools
`--sp`, `--slr` automatically add additional crossing logic to minimize the effect of the SLL delay, and facilitates better timing closure.

`V++` requires `--slr` option during the linking stage for explicit slr assignment of kernels.
```
--slr vmult_1:SLR0 --slr vadd_1:SLR1
```
