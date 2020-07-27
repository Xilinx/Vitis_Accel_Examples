Loop Dependency Inter
=====================

This example demonstrates how extra information about dependency can be
specified for variables in consecutive loop iterations using
``#pragma HLS DEPENDENCE``. It allows the ``V++`` to recognize
dependencies and optimize the loop executions.

For example here ``linebuf`` has ``false inter dependency`` which means
each iteration of loop is independent for ``linebuf`` access.

``#pragma HLS DEPENDENCE`` allows compiler to ignore dependency of
linebuf and generate a pipeline with lower II count. If user does not
provide this pragma, by default compiler assumes dependency exists on
linebuf between iterations. As a result, compiler schedules unoptimized
design with larger II factor which in turn leads to slow performance.

.. code:: cpp

   #pragma HLS ARRAY_PARTITION variable = linebuf dim = 1 complete
   VConvH:
       for (int col = 0; col < height; ++col) {
       VConvW:
           for (int row = 0; row < width; ++row) {
              #pragma HLS DEPENDENCE variable=linebuf inter false
               int in_val = inStream.read();
               int out_val = 0;
           VConv:
               for (int i = 0; i < K; i++) {
                   int vwin_val = i < (K - 1) ? linebuf[i][row] : in_val;
                   out_val += vwin_val * vcoeff[i];

                   if (i > 0)
                       linebuf[i - 1][row] = vwin_val;
               }
               outStream << out_val;
           }
       }
   }
