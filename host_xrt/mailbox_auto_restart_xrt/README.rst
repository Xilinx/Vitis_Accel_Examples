Mailbox and Auto-Restart (XRT Native API's)
===========================================

This is a simple example demonstrating the Mailbox and Auto-restart features for writing registers to and reading registers from a never-ending kernel.

**KEY CONCEPTS:** `Mailbox <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Using-the-Mailbox>`__, `Auto-restart <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Auto-Restarting-Mode>`__

**KEYWORDS:** `set_arg <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-XRT-Managed-Kernels>`__, get_arg, write(), read(), STABLE

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All NoDMA Platforms, i.e u50 nodma etc

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/mbox_autorestart.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./mailbox_auto_restart_xrt -x <mbox_autorestart XCLBIN>

DETAILS
-------

Auto-restarting kernels can run continuously after being started once by the host application. The main advantage of auto-restarting kernels is that they run semi-autonomously operating as data-driven kernels without the need for frequent interaction with the host application and for software control. But auto-restarting kernels also offer semi-synchronization through the mailbox, which is the ability to exchange data with the host application in an asynchronous, non-blocking, and safe way. 

The Auto-restart and mailbox features must be enabled at kernel compile time. Contents of ``runPre.tcl`` file is below:

::

   ::common::set_param hls.enable_hidden_option_error false
   config_interface -s_axilite_mailbox both
   config_interface -s_axilite_auto_restart_counter 1

Using the ``host.cpp``, the kernel is started in auto restart mode with initial scalar values. We also create a mailbox to pragmatically update the scalars.

.. code:: cpp

   auto mbox_autorestart_run = mbox_autorestart(xrt::autostart{0}, adder1, adder2);
   xrt::mailbox krnl_mbox(mbox_autorestart_run);

The kernel runs forever with the host updating the two scalar values in alternate iterations (The host reads the result in the other iterations). The same is achieved via updating the mailbox and requesting a sync to the hw for write whereas for read, the mailbox sync is requested first and then the values are fetched by the host.

.. code:: cpp

   if (cnt % 2 == 0) {
       krnl_mbox.set_arg(0, ++adder1); // update the mailbox
       krnl_mbox.set_arg(1, --adder2); // update the mailbox
   
       // write the mailbox content to hw, the write will not be picked
       // up until the next iteration of the pipeline .
       krnl_mbox.write(); // requests sync of mailbox to hw
   } else {
       krnl_mbox.read(); // requests sync of mailbox to hw
       auto mbox_add = krnl_mbox.get_arg(2);
       auto mbox_mult = krnl_mbox.get_arg(3);

The auto-restart iteration count set as 0 implies that there is no limit on the number of iterations to restart the kernel. The kernel thus have to be stopped manually by the host using ``mbox_autorestart_run.stop()``.
