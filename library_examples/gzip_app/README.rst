Xilinx GZIP Application
=======================

GZip application for DDR resides in `library_examples/gzip_app` directory.

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u2_
   zc
   vck
   u30
   u50
   u280
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/cmdlineparser.cpp
   src/host.cpp
   src/logger.cpp
   src/xcl2.cpp
   src/xxhash.c
   src/xxhash.h
   src/zlib.cpp
   src/zlib_dm_multibyte_rd.cpp
   src/zlib_dm_multibyte_wr.cpp
   src/zlib_huffman_enc_mm.cpp
   src/zlib_lz77_compress_mm.cpp
   src/zlib_parallelbyte_decompress_full.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./xil_gzip -sx <compress_decompress XCLBIN> -v ./sample.txt

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.