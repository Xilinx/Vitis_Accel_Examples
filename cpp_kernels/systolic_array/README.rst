Systolic Array (C)
==================

This is a simple example of matrix multiplication (Row x Col) to help developers learn systolic array based algorithm design. Note : Systolic array based algorithm design is well suited for FPGA.

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - Embedded ZCU104
 - Embedded ZC702
 - All NoDMA Platforms, i.e u50 nodma etc

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/mmult.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./systolic_array <mmult XCLBIN>

DETAILS
-------

This example demonstrates how ``Systolic array algorithm`` can be used
in FPGAs to perform matrix operations efficiently.

Matrix multiplication is performed in the ``mmult`` kernel. Local
matrices A and B are partitioned using ``#pragma HLS ARRAY_PARTITION``
along rows and columns respectively for computation of each element of
the matrix C requires a row of matrix A and a column of matrix B. Matrix
C is partitioned completely.

.. code:: cpp

     int localA[MAX_SIZE][MAX_SIZE];
     #pragma HLS ARRAY_PARTITION variable=localA dim=1 complete

     int localB[MAX_SIZE][MAX_SIZE];
     #pragma HLS ARRAY_PARTITION variable=localB dim=2 complete

     int localC[MAX_SIZE][MAX_SIZE];
     #pragma HLS ARRAY_PARTITION variable = localC dim = 0 complete

This allows the multiply and accumulate operations to be performed in
parallel thus speeding up the operation as innermost loop is unrolled to
provide for the pipelining in the outer loops. However, the number of
these parallel operations is limited by the number of DSPs available in
the FPGA.

.. code:: cpp

   systolic1:
       for (int k = 0; k < a_col; k++) {
          #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
       systolic2:
           for (int i = 0; i < MAX_SIZE; i++) {
           systolic3:
               for (int j = 0; j < MAX_SIZE; j++) {
                   int last = (k == 0) ? 0 : localC[i][j];
                   int a_val = (i < a_row && k < a_col) ? localA[i][k] : 0;
                   int b_val = (k < b_row && j < b_col) ? localB[k][j] : 0;
                   int result = last + a_val * b_val;

                   localC[i][j] = result;
               }
           }
       }

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.