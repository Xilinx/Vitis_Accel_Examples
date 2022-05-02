Asynchronous XRT (XRT Native API's)
===================================

In this example we showcase asynchronous programming mechanism through the user-defined queues. The ``xrt::queue`` is lightweight, general-purpose queue implementation which is completely separated from core XRT native API data structures.

XRT queue implementation needs ``#include <experimental/xrt_queue.h`` to be added as the header file. The implementation also use C++17 features so the host code must be compiled with ``g++ -std=c++17``.

Executing multiple tasks through queue 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c++
      :number-lines: 84
      
        
          xrt::queue main_queue;
          xrt::queue queue_bo1;
          auto bo0_event = main_queue.enqueue([&bo0] {bo0.sync(XCL_BO_SYNC_BO_TO_DEVICE); });
          auto bo1_event = queue_bo1.enqueue([&bo1] {bo1.sync(XCL_BO_SYNC_BO_TO_DEVICE); });
          main_queue.enqueue(bo1_event); 
          main_queue.enqueue([&run] {run.start(); run.wait(); });
          auto bo_out_event = main_queue.enqueue([&bo_out] {bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE); });
          bo_out_event.wait();


In line number 86 and 87, ``bo0`` and ``bo1`` host-to-device data transfers are enqueued through two separate queues to achieve parallel transfers. To synchronize between these two queues, the returned event from the ``queue_bo1`` is enqueued in the ``main_queue``, similar to a task enqueue (line 88). As a result, any other task submitted after that event won't execute until the event is finished. So, in the above code example, subsequent task in the ``main_queue`` (such as kernel execution) would wait till the ``bo1_event`` is completed. By submitting an event returned from a ``queue::enqueue`` to another queue, we can synchronize among the queues.
