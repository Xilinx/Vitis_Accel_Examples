Xilinx AML
==========

Anti Money Laundering checking against deny-lists

**EXCLUDED PLATFORMS:** 

 - Samsung U.2 SmartSSD
 - Alveo U25 SmartNIC
 - Alveo U30
 - Alveo U50
 - Alveo U200
 - Alveo U280
 - Alveo U250
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - All NoDMA Platforms, i.e u50 nodma etc

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