Copy Buffer XRT (XRT native API's)
==================================

This Copy Buffer example demonstrate how one buffer can be copied from another buffer.

**KEY CONCEPTS:** Copy Buffer

**KEYWORDS:** copy

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

   ./copy_buffer_xrt -x <vector_addition XCLBIN>

DETAILS
-------

This example illustrates the use of ``Copyr`` API for deep copy between 
the two buffer objects if the platform supports a deep-copy. If deep copy
is not supported by the platform the data transfer happens by shallow copy.

Contents of ``bo_a`` are copied into ``bo_b``.

.. code:: cpp

   dst_buffer.copy(src_buffer, copy_size_in_bytes);

The API xrt::bo::copy also has overloaded version to provide a different
offset than 0 for both the source and the destination buffer.


For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.