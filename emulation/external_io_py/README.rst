External IO Python based generator(HLS C/C++ Kernel)
====================================================

This example will demonstrate emulation of Xilinx Device's IO with a Python based external process.

**KEY CONCEPTS:** `Traffic Generator <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__, `Traffic generation using Python <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-Traffic-Generators-in-Python>`__

**KEYWORDS:** `sim_ipc_axis_master <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Adding-Traffic-Generators-to-Your-Design>`__, `sim_ipc_axis_slave <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Adding-Traffic-Generators-to-Your-Design>`__, `gt_master <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__, `gt_slave <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All Xilinx DMA Platforms
 - Versal VCK5000
 - AWS VU9P F1
 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/b_test_master.py
   src/b_test_slave.py
   src/host.cpp
   src/increment.cpp
   src/nb_test_master.py
   src/nb_test_slave.py
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./external_io_py <increment XCLBIN>

DETAILS
-------

Some user applications make use of IO ports in Vitis flow to drive streaming traffic in and out of the platform . This example emulates such designs in HW-Emulation by mimicing the IO port behavior as in HW.

In this example, Master and Slave Stream IO models will be available as prepacked xo's with different data width configuration. In order to instantiate the IP's corresponding to the provided xo, the xos are specified in the v++ command line 

::

   XOS = $(XILINX_VITIS)/data/emulation/XO/sim_ipc_axis_master_32.xo  $(XILINX_VITIS)/data/emulation/XO/sim_ipc_axis_slave_32.xo
   $(VPP) $(VPP_FLAGS) -l $(VPP_LDFLAGS) $(XOS) --temp_dir $(TEMP_DIR)  -o'$(LINK_OUTPUT)' $(+)

The Python-based Traffic Generator follows the following steps for blocking traffic generation, transportation and reception -

1. Instantiate AXI Master and Slave Utilities

.. code:: python

   master_util = ipc_axis_master_util("gt_master")
   slave_util = ipc_axis_slave_util("gt_slave")

2. Determine the number of transactions and create that many random packets 

.. code:: python

   #Create payload
   payload = xtlm_ipc.axi_stream_packet()
   #Generate Random Data length
   #Here we are generating 1-25 random integers. Each integer can have any value
   #We are multiplying by 4 as, each ineger will take 4 byes of memory 
   payload.data_length = 4*random.randint(1,25)
   
   rand_int = [] #A list containing random numbers
   
   for j in range(0, int(payload.data_length/4)):
       #Generate a random integer and append to list 
       rand_int.append(random.randint(0, 10000)) #We can choose any range. 
   
   #Convert generated random number to byte form (as supported by C style struct), 
   #More features packing are available at https://docs.python.org/3/library/struct.html
   #User is free to choose his own way of packing as long as it is in-line with C types
   format_string = "<"+str(int(payload.data_length/4))+"I" 
   payload.data = bytes(bytearray(struct.pack(format_string, *tuple(rand_int))))
   payload.tlast = True # to mark end of packet. This is user's choice based on application

3. Send the data over ipc socket. Here the function call is blocked until response is available.

.. code:: python

   master_util.b_transport(payload)

4. Sample the data available on the socket.

.. code:: python

   payload = slave_util.sample_transaction()

5. End the simulation using the following API.

.. code:: python

   master_util.end_of_simulation()

The Python-based Traffic Generator follows the following steps for Non-blocking traffic generation, transportation and reception -

1. Instantiate AXI Master and Slave Utilities

.. code:: python

   master_util = ipc_axis_master_nb_util("gt_master")
   slave_util = ipc_axis_slave_nb_util("gt_slave")

2. Determine the number of transactions and create that many random packets 

.. code:: python

   #Create payload
   payload = xtlm_ipc.axi_stream_packet()
   #Generate Random Data length
   #Here we are generating 1-25 random integers. Each integer can have any value
   #We are multiplying by 4 as, each ineger will take 4 byes of memory 
   payload.data_length = 4*random.randint(1,25)
   
   rand_int = [] #A list containing random numbers
   
   for j in range(0, int(payload.data_length/4)):
       #Generate a random integer and append to list 
       rand_int.append(random.randint(0, 10000)) #We can choose any range. 
   
   #Convert generated random number to byte form (as supported by C style struct), 
   #More features packing are available at https://docs.python.org/3/library/struct.html
   #User is free to choose his own way of packing as long as it is in-line with C types
   format_string = "<"+str(int(payload.data_length/4))+"I" 
   payload.data = bytes(bytearray(struct.pack(format_string, *tuple(rand_int))))
   payload.tlast = True # to mark end of packet. This is user's choice based on application

3. Send the data over ipc socket. Here the function returns immediately without waiting for a response.

.. code:: python

   master_util.nb_transport(payload)

4. Sample the data available on the socket.

.. code:: python

   payload = slave_util.nb_sample_transaction()

5. End the simulation using the following API.

.. code:: python

   master_util.end_of_simulation()

Connections to the Custom IP's specified in xo's are made using ``krnl_incr.cfg`` file as below:

::
   
   [connectivity]
   nk=sim_ipc_axis_master_32:1:gt_master
   nk=sim_ipc_axis_slave_32:1:gt_slave
   stream_connect=gt_master.M00_AXIS:increment_1.a
   stream_connect=increment_1.output:gt_slave.S00_AXIS


Note : xo instance name specified on v++ command line  becomes a key which will be used by the external process to refer to a particular stream port 

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.