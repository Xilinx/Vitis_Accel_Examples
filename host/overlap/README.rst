Overlap Host and Kernel (C)
===========================

This examples demonstrates techniques that allow user to overlap Host(CPU) and FPGA computation in an application. It will cover asynchronous operations and event object.

**KEY CONCEPTS:** OpenCL Host API, `Synchronize Host and FPGA <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Event-Synchronization>`__, `Asynchronous Processing <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Event-Synchronization>`__, `Events <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Overlapping-Data-Transfers-with-Kernel-Computation>`__, `Asynchronous memcpy <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Event-Synchronization>`__

**KEYWORDS:** `cl_event <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Event-Synchronization>`__, `cl::CommandQueue <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Command-Queues>`__, `CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Single-Out-of-Order-Command-Queue>`__, `enqueueMigrateMemObjects <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vector_addition.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./overlap <vector_addition XCLBIN>

DETAILS
-------

This example illustrates how host read and write tasks can be
synchronized with kernel computational tasks to achieve low latency in
cases where data is too large to fit on FPGA or it is being streamed
from a sensor or a network.

For example, the data is too big to fit in an FPGA or the data is being
streamed from a sensor or the network. In these situations data must be
transferred to the host memory to the FPGA before the computation can be
performed. Because PCIe is an full-duplex interconnect, you can transfer
data to and from the FPGA simultaneously. Xilinx FPGAs can also perform
computations during these data transfers. Performing all three of these
operations at the same time allows to keep the FPGA busy and take full
advantage of all of the hardware on your system.

In this example, it is described how to perform this using an out of
order command queue.

Many OpenCL commands are asynchronous. This means that whenever you call
an OpenCL function, the function will return before the operation has
completed. Asynchronous nature of OpenCL allows you to simultaneously
perform tasks on the host CPU as well as the FPGA.

Memory transfer operations are asynchronous when the blocking_read,
blocking_write parameters are set to CL_FALSE. These operations are
behaving on host memory so it is important to make sure that the command
has completed before that memory is used.

You can make sure an operation has completed by querying events returned
by these commands. Events are OpenCL objects that track the status of
operations. Event objects are created by kernel execution commands,
read, write, copy commands on memory objects or user events created
using clCreateUserEvent.

Events can be used to synchronize operations between the host thread and
the device or between two operations in the same context. You can also
use events to time a particular operation if the command queue was
created using the ``CL_QUEUE_PROFILING_ENABLE`` flag.

Most enqueuing commands return events by accepting a cl::Event pointer
as their last argument of the call. These events can be queried using
the cl::Event::wait() function to get the status of a particular
operation.

Many functions also accept event lists that can be used to enforce
ordering in an OpenCL context. These events lists are especially
important in the context of out of order command queues as they are the
only way specify dependency. Normal in-order command queues do not need
this because dependency is enforced in the order the operation was
enqueued.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.