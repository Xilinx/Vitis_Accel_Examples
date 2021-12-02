SystemC Vadd (C)
================

This is simple example of vector addition to describe usage of custom SystemC in Vitis Environment.

**KEY CONCEPTS:** `SystemC <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-SystemC-Models>`__

**KEYWORDS:** `create_sc_xo <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-SystemC-Models>`__, `xtlm_ap_ctrl_hs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Coding-a-SystemC-Model>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./systemc_vadd <vadd XCLBIN>

DETAILS
-------

There are three types of Vitis HLS kernels namely ap_ctrl_chain, ap_ctrl_hs and ap_ctrl_none. Corresponding to each of these HLS kernels user can write SystemC kernels using predefined constructs/macros. The difference b/w ap_ctrl_chain and ap_ctrl_hs is that user can start ap_ctrl_chain kernels repeatedly by writing to ap_continue bit. In this example there is ap_ctrl_hs kernel vadd which requires the XRT control to provide memory address and size of the data.


Writing ap_ctrl_hs SystemC kernel (vadd)
----------------------------------------

xtlm_ap_ctrl_hs base class
--------------------------

The Kernels which requires XRT control should be derived from the xsc::xtlm_ap_ctrl_hs base class. xtlm_ap_ctrl_hs models all the control registers and AXI lite control interface. This class also has the ap_rst_n and ap_clk interfaces which are required as part of kernel interface requirements.

.. code:: c++

      :number-lines: 1
           
           //! deriving from xtlm_ap_ctrl_hs
            class vadd : public xsc::xtlm_ap_ctrl_hs
                {
                public:
                    SC_HAS_PROCESS(vadd);
                    vadd(sc_module_name name, xsc::common_cpp::properties& _properties):
                    xsc::xtlm_ap_ctrl_hs(name)
                    {
                    .
                    .
                    }
                } 


Adding kernel interfaces
------------------------
Add the kernel interfaces by using below macros. The name of the interfaces used in the declaration will be reflected in component.xml which is generated from create_sc_xo command.
The same interfaces should be instantiated in the constructor of the model which is shown in below code snippet

.. code:: c++

        //Declare kernel interfaces
         DECLARE_XTLM_AXIMM_MASTER_IF(in1);
         DECLARE_XTLM_AXIMM_MASTER_IF(in2);
         DECLARE_XTLM_AXIMM_MASTER_IF(out_r);

        // Define interfaces inside constructor
        {
            DEFINE_XTLM_AXIMM_MASTER_IF(in1, 32);
            DEFINE_XTLM_AXIMM_MASTER_IF(in2, 32);
            DEFINE_XTLM_AXIMM_MASTER_IF(out_r, 32);
        }      

        

Bridge b/w Kernel arguments and XRT control
-------------------------------------------
If the kernel has the kernel arguments, the XRT needs to configure all these kernel arguments before starting the kernel. The first four 32 bit locations are reserved for predefined registers(CTRL, GIE, IE and IS) . From 0x10 onwards the addresses can be allocated to kernel arguments. If the kernel has arguments declare them with offset addresses(>=0x10) and its size. Each arguments should be placed at multiple of 0x8 (0x10, 0x18, 0x20 etc). Using this information the kernel.xml file is generated and which acts as bridge between XRT and SystemC PL kernel. Once the ap_start is triggered the user can get the arguments configured by XRT as shown below in code snippet.

.. code:: c++

        //! Below lines adds link b/w XRT and IP through kernel.xml
         ADD_MEMORY_IF_ARG(in1,   0x10, 0x8);
         ADD_MEMORY_IF_ARG(in2,   0x18, 0x8);
         ADD_MEMORY_IF_ARG(out_r, 0x20, 0x8);
         ADD_SCALAR_ARG(size,     0x28, 0x4); 
   
Reading and Writing through kernel interfaces 
---------------------------------------------
The actual computation of the kernel is executed in SystemC thread "main_thread". This thread waits until all kernel arguments are configured by XRT and start bit is set. Once thread comes out of wait(ev_ap_start), get the required kernel argument values. For each of the kernel interface types simple read/write API's are provided which takes char* data type. Depending on the interface size, number bytes are read/write from char* as shown in below code snippet. Once the kernel execution is completed ap_done sets the appropriate bits (ap_done/ap_ready) and raises interrupt if enabled.

.. code:: c++
    
          //! Reading/Writing through kernel interfaces
            void main_thread()
        {
            wait(ev_ap_start); //! Wait on ap_start event...

            //! Copy kernel args configured by host...
            uint64_t  in1_base_addr = kernel_args[0]; 
            uint64_t  in2_base_addr = kernel_args[1]; 
            uint64_t  out_r_base_addr = kernel_args[2]; 
            size = kernel_args[3];

            unsigned data1, data2, data_r;
            for(int i = 0; i < size; i++) {
                in1->read(in1_base_addr + (i*4), (unsigned char*)&data1);  //! Read from in1 interface
                in2->read(in2_base_addr + (i*4), (unsigned char*)&data2);  //! Read from in2 interface

                //! Add data1 & data2 and write back result
                data_r = data1 + data2;                //! Add
                out_r->write(out_r_base_addr + (i*4), (unsigned char*)&data_r); //! Write the result
            }

            ap_done(); //! completed Kernel computation...
        }

Putting together complete vadd SystemC PL kernel code
-----------------------------------------------------
Below are the required steps to write the SystemC PL kernel

.. code:: c++

        -  Include header files "xtlm_ap_ctrl.h" and "xtlm.h".
        -  Derive from predefined base class based on the kernel type(ap_ctrl_hs, ap_ctrl_none etc..). 
        -  Declare & define required kernel interfaces 
        -  Add required kernel arguments with right offset addresses and size
        -  Write the kernel body in main_thread

Generating XO's using SystemC PL kernel source
----------------------------------------------
Generating XO from source file has many intermediate steps like generating packaging script, package_xo script etc.. It is not required for the user to bother about intermediate files unless there are any issues. To generate XO use create_sc_xo command. create_sc_xo command takes the SystemC PL kenel source file as input and creates IP and generates the XO which can be used to link in Vitis. Once source files for vadd are ready use create_sc_xo command to generate XO's.

.. code:: c++

        #create_sc_xo command in Makefile
        vadd.xo: vadd.cpp
            create_sc_xo vadd.cpp

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.