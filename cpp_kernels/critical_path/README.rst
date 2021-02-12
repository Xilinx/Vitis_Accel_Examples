Critical Path (C)
=================

This example shows a normal coding style which could lead to critical path issue and design will give degraded timing.  Example also contains better coding style which can improve design timing.

**KEY CONCEPTS:** Critical Path handling, Improve Timing

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/golden.bmp
   src/apply_watermark.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./critical_path -x <apply_watermark_GOOD XCLBIN> -i $(XF_PROJ_ROOT)/common/data/xilinx_img.bmp -c ./data/golden.bmp

DETAILS
-------

This example demonstrates the considerations in coding style to avoid
critical paths in kernels.

``Apply_watermark`` kernel is processing the image’s pixels concurrently
by using ``HLS UNROLL`` however, a global variable ``x`` is being
updated in every iteration which nullifies the speedup offered by
unrolling the loop and leads to a critical path.

.. code:: cpp

   watermark:
          for (int i = 0; i < DATA_SIZE; i++, x++) {
             #pragma HLS UNROLL
             if (x > width) {
                  x = x - width;
                  y += 1;
              }

              uint w_idy = y % WATERMARK_HEIGHT;
              uint w_idx = x % WATERMARK_WIDTH;
              tmp.data[i] = saturatedAdd(tmp.data[i], watermark[w_idy][w_idx]);
          }

Using local variables and just referencing the value of ``x`` in every
iteration and updating it outside the loop can remove this critical path
and thus improve the performance and timing of kernel execution.

.. code:: cpp

   for (int i = 0; i < DATA_SIZE; i++) {
              #pragma HLS UNROLL
               uint tmp_x = x + i;
               uint tmp_y = y;
               if (tmp_x > width) {
                   tmp_x = tmp_x - width;
                   tmp_y += 1;
               }

               uint w_idy = tmp_y % WATERMARK_HEIGHT;
               uint w_idx = tmp_x % WATERMARK_WIDTH;
               tmp.data[i] = saturatedAdd(tmp.data[i], watermark[w_idy][w_idx]);
           }
           x += DATA_SIZE;       

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.