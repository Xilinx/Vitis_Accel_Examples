Concurrent Kernel execution (C)
===============================

This example illustrates two ways to implement concurrent kernel
execution. 1. Multiple command enqueues 2. Single command queue with out
of order execution

Design contains 3 kernels ``mscale``, ``madd`` and ``mmult``. ``madd``
needs to wait for ``mscale`` to complete but ``mmult`` can run
independently (no dependency).

``multiple_command_queues`` function creates two sequential command
queues, one for ``mscale and madd`` and other for ``mmult``.

.. code:: cpp

   cl::CommandQueue ordered_queue1(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
   cl::CommandQueue ordered_queue2(context, device, CL_QUEUE_PROFILING_ENABLE, &err);

Both Command queues are created as in-order execution. ``msacle`` and
``madd`` are enqueued in first command queue one after another. As
Command queue is ``in-order``, ``madd`` will only start when ``mscale``
will finish. So the order of execution is handled by command queue.

There is another approach of achieve the same using single out-of-order
command queue. ``out_of_order_queue`` function creates a single command
queue with out of order execution enabled which means that enqueues in
the command queue can go out of order (without waiting for a previous
execution to finish).

.. code:: cpp

   cl::CommandQueue ooo_queue(context, device,
             CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);

However, since we need that ``madd`` kernel only starts executing after
``mscale`` has finished, ``cl::event`` is used to wait for ``mscale`` to
finish execution.

.. code:: cpp

   cl::Event event;
   vector<cl::Event> kernel_wait_events
   kernel_wait_events.push_back(event);
   err = ooo_queue.enqueueNDRangeKernel(kernel_mscale, offset, global, local, nullptr, &event));

   err = ooo_queue.enqueueNDRangeKernel(kernel_madd, offset, global, local,
                                             &kernel_wait_events, // Event from previous call
                                             nullptr);
