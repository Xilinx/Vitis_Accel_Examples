Multiple Process (C)
====================

This example will demonstrate how to run multiple processes to utilize multiple kernels simultaneously on an FPGA device. Multiple processes can share access to the same device provided each process uses the same xclbin. Processes share access to all device resources but there is no support for exclusive access to resources by any process.

**KEY CONCEPTS:** `Concurrent execution <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Task-Parallelism>`__, Multiple HLS kernels, Multiple Process Support

**KEYWORDS:** PID, fork, XCL_MULTIPROCESS_MODE, multiprocess

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_vadd.cpp
   src/krnl_vmul.cpp
   src/krnl_vsub.cpp
   src/multi_krnl.h
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./multiple_process <multi_krnl XCLBIN>

DETAILS
-------

This example will demonstrate how to run multiple processes to utilize
multiple kernels simultaneously on an FPGA device. Multiple processes
can share access to the same device provided each process uses the same
xclbin. Processes share access to all device resources but there is no
support for exclusive access to resources by any process.

If two or more processes execute the same kernel, then these processes
will acquire the kernel’s compute units and will be scheduled in a
first-come first-serve manner. All processes have the same priority in
Xilinx Runtime(XRT) Environment.

**PREREQUISITE**: Host is required to set the environment variable:

::

   XCL_MULTIPROCESS_MODE = 1

The example comprises of three different kernels for performing vector
addition, subtraction and multiplication. The host uses C provided
function ``fork()`` for invoking different child process for each of the
kernel to execute. The three child processes created can be identified
by their process ID using the function ``getpid()``. Similarly, if
required, parent process ID can also be achieved using the function
``getppid()``.

.. code:: c

        for(int i=0; i< num_of_child_process; i++) {
            if(fork() == 0) {
                printf("[CHILD] PID %d from [PARENT] PPID %d",getpid(),getppid());
                result = run_kernel(krnl_id);
            }
        }

For each of the process, the following tasks can be viewed along with
their PID: 1. Transfer the Input Data to Device 2. Launch Kernel 3.
Transfer the Output Data from Device 4. Checking the Output Data with
Golden Results

The flow of the above stated tasks will give the users an in-sight
information of how multiprocessing is performed by the Xilinx FPGA
device. Until all the child processes are finished, parent process
(host) waits for any further execution

**LIMITATION**: In Emulation flow, Debug and Profile will not function
correctly when multi-process has been enabled.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.