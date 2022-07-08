System Deadlock Detection: Stream Kernel to Kernel Memory Mapped
================================================================

This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel design to demonstrate system level deadlock detection feature in HW Emulation.

**KEY CONCEPTS:** System Level Deadlock Detection

**KEYWORDS:** systemDeadlockDetection=enable

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - All Versal Platforms, i.e vck190 etc
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All NoDMA Platforms, i.e u50 nodma etc

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_stream_vadd.cpp
   src/krnl_stream_vmult.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./deadlock_streaming_k2k_mm <krnl_stream_vadd_vmult XCLBIN>

DETAILS
-------

This example demonstrates how a deadlock can occur by the interaction of two kernels (inter kernel) at system level. The prior detection at HW Emulation itself helps to debug the deadlock kernels prior to the HW flow. 

Input from the user
--------------------

The user needs to provide additional flags at v++ compile and v++ link as below: 

1. Compile the kernels to have local deadlock signals using advanced compile options inside file ``<krnl_compile.cfg>``

    .. code:: cpp
       param=compiler.deadlockDetection=true
  
In the Makefile, add the VPP flags 

    ::

        VPP_FLAGS_krnl_stream_vadd +=  --config krnl_compile.cfg
        VPP_FLAGS_krnl_stream_vmult +=  --config krnl_compile.cfg
        $(VPP) $(VPP_FLAGS) $(VPP_FLAGS_krnl_stream_vadd) -c -k krnl_stream_vadd 
        $(VPP) $(VPP_FLAGS) $(VPP_FLAGS_krnl_stream_vmult) -c -k krnl_stream_vmult

2. Link the kernels to enable system level deadlock detection using advanced linker option inside file ``<krnl_link.cfg>``

      .. code:: cpp
         [advanced]
         param=compiler.systemDeadlockDetection=enable 

In the Makefile, add the VPP flags at v++ link 

    ::
    
        VPP_LDFLAGS_krnl_stream_vadd_vmult += --config ./krnl_stream_vadd_vmult.cfg --config krnl_link.cfg
        $(VPP) $(VPP_FLAGS) -l $(VPP_LDFLAGS) --temp_dir $(TEMP_DIR) $(VPP_LDFLAGS_krnl_stream_vadd_vmult) -o'$(LINK_OUTPUT)   

Output
-------

At runtime, user can see below deadlock detection message once deadlock is detected between kernels - 
   ``[RUNTIME_ERROR]:Deadlock Detected between PL Kernels in the design and hardware emulation cannot progress further. Please terminate the simulation manually and debug the kernels``

* Note: This example is set up to terminate the simulation automatically from host code itself post deadlock detection runtime error.

Runtime Step: Deadlock Monitor 
------------------------------

This example is run with `debug_mode=batch` and generates waveform database file (.wdb) that can be opened in vivado to analyse the deadlock signals. 

* Upon opening the waveform, you will find "CU stalls" object for each kernel functions. 
* Users can see the stall signals (in orange) generated from streams of the kernels responsible for the deadlock in the design.    

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.