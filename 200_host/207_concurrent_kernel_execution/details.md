Concurrent Kernel execution (C)
================================

This example illustrates two ways to implement concurrent kernel execution.
1. Multiple command enqueues
2. Single command queue with out of order execution

3 kernels `mscale`, `madd` and `mmult` are used for scaling, adding and multiplication operations on matrices respectively. Addition operation needs to wait for scaling to complete but multiplication is independent of both these operations.

`multiple_command_queues` function implements two command queues, one for scaling and addition and other for multiplication operation. `cl::event` objects are used to monitor the operations in the command queue.
```c++
cl::CommandQueue ordered_queue1(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
cl::CommandQueue ordered_queue2(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
```  

`out_of_order_queue` function implements a single command queue with out of order execution enabled which means that enqueues in the command queue can execute concurrently without waiting for a previous execution to finish.

```c++
OCL_CHECK(err, cl::CommandQueue ooo_queue(context, device,
          CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));
```          
However, since we need that addition kernel only starts executing after the scaling has finished, cl::event is used to wait for mscale to finish execution.
```c++
OCL_CHECK(err, err = ooo_queue.enqueueNDRangeKernel(kernel_madd, offset, global, local,
                                              &kernel_wait_events, // Event from previous call
                                              &ooo_events[1]));
```                                   

