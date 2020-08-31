Xilinx GZIP Application for u200 and u250 Platforms
===================================================

GZip application for DDR resides in `demos/gzip_app` directory.

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   vck
   u50
   u280

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/cmdlineparser.cpp
   src/cmdlineparser.h
   src/fixed_codes.hpp
   src/host.cpp
   src/huffman_decoder.hpp
   src/huffman_encoder.hpp
   src/huffman_treegen.hpp
   src/inflate.hpp
   src/logger.cpp
   src/logger.h
   src/lz_compress.hpp
   src/lz_decompress.hpp
   src/lz_optional.hpp
   src/mm2s.hpp
   src/s2mm.hpp
   src/stream_downsizer.hpp
   src/stream_upsizer.hpp
   src/xcl2.cpp
   src/xcl2.hpp
   src/xxhash.c
   src/xxhash.h
   src/zlib.cpp
   src/zlib.hpp
   src/zlib_dm_multibyte_rd.cpp
   src/zlib_dm_multibyte_rd.hpp
   src/zlib_dm_multibyte_wr.cpp
   src/zlib_dm_multibyte_wr.hpp
   src/zlib_huffman_enc_mm.cpp
   src/zlib_huffman_enc_mm.hpp
   src/zlib_lz77_compress_mm.cpp
   src/zlib_lz77_compress_mm.hpp
   src/zlib_parallelbyte_decompress_full.cpp
   src/zlib_parallelbyte_decompress_full.hpp
   src/zlib_specs.hpp
   src/zlib_tables.hpp
   src/zlib_treegen_mm.hpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./xil_gzip -sx <compress_decompress XCLBIN> -v ./sample.txt

