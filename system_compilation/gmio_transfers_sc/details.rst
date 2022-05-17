GMIO Transfers System Compiler
==============================

This example demonstrates the Sequential & Randon data transfer between host and device compute units.

- ACCESS_PATTERN(<port>, <pattern>)
This macro is used to specify the data access pattern on a given CU port/memory interface. Based on the access pattern system compiler can generate an appropriate data mover plugin for the kernel. Where <port> is the kernel port and  <pattern> can be sequential/random.

- DATA_COPY(<port>, <port>[<num>])
Specifies the amount of data to be copied by the data mover at runtime between host and device for CU processing for given port. The "port" needs to be replaced by port name ( kernel function argument name). The "num" specifies the number of data elements to be transferred the size of data. The num can be a constant or it can be itself an expression in terms of scalar kernel arguments.

- ZERO_COPY(<port>, <port>[<num>])
Do not use a data mover plugin. The CU is assumed to be in direct connection with global DDR memory. It lets the kernel use a M-AXI interface to directly talk to the device DDR. The CU is responsible for any on-chip caching of data needed.  The "port" needs to be replaced by port name ( CU argument name). The "num" specifies the number of data elements to be transferred essentially the size of data. The "num" can be a constant or it can be itself an expression in terms of some scalar CU arguments.
