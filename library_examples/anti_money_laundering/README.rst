Xilinx AML
==========

Anti Money Laundering checking against deny-lists

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u2_
   _u25_
   u30
   u50
   u200
   u280
   u250
   zc
   vck
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/BIC_ref_data.csv
   data/README.md
   data/entities.csv
   data/people.csv
   data/stopkeywords.csv
   data/txdata.csv
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./test_aml  -xclbin <aml XCLBIN> -d ./data -c 0

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.