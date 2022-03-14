External IO C++ based generator
===============================

Some user applications make use of IO ports in Vitis flow to drive streaming traffic in and out of the platform . This example emulates such designs in HW-Emulation by mimicing the IO port behavior as in HW.

In this example, Master and Slave Stream IO models will be available as prepacked xo's with different data width configuration. In order to instantiate the IP's corresponding to the provided xo, the xos are specified in the v++ command line 

::

   XOS = $(XILINX_VITIS)/data/emulation/XO/sim_ipc_axis_master_32.xo  $(XILINX_VITIS)/data/emulation/XO/sim_ipc_axis_slave_32.xo
   $(VPP) $(VPP_FLAGS) -l $(VPP_LDFLAGS) $(XOS) --temp_dir $(TEMP_DIR)  -o'$(LINK_OUTPUT)' $(+)

The C++ based Traffic Generator follows the following steps for blocking traffic generation, transportation and reception -

1. Instantiate IPC socket with name matching in IPI diagram

.. code:: cpp

   xtlm_ipc::axis_initiator_socket_util<xtlm_ipc::BLOCKING> socket_util("gt_master");
   xtlm_ipc::axis_target_socket_util<xtlm_ipc::BLOCKING> socket_util("gt_slave");

2. Determine the number of transactions and create that many random packets 

.. code:: cpp

   std::vector<char> data = generate_data();
   
   //! Set packet attributes...
   packet.set_data(data.data(), data.size());
   packet.set_data_length(data.size());
   packet.set_tlast(1);

3. Send the data over ipc socket. Here the function call is blocked until response is available.

.. code:: cpp

   socket_util.transport(packet);

4. Sample the data available on the socket.

.. code:: cpp

   socket_util.sample_transaction(packet)

5. End the simulation using the following API.

.. code:: cpp

   socket_util.end_of_simulation();

The C++ based Traffic Generator follows the following steps for Non-blocking traffic generation, transportation and reception -

1. Instantiate IPC socket with name matching in IPI diagram

.. code:: cpp

   xtlm_ipc::axis_initiator_socket_util<xtlm_ipc::NON_BLOCKING> socket_util("gt_master");
   xtlm_ipc::axis_target_socket_util<xtlm_ipc::NON_BLOCKING> socket_util("gt_slave");

2. Determine the number of transactions and create that many random data points 

.. code:: cpp

   //! Initialize vector size...
   std::vector<char> v_data(data_length);
   
   //! Fill the data in vector...
   std::generate(v_data.begin(), v_data.end(), []() { return std::rand() % 256; });

3. Send the data over ipc socket. Here the function returns immediately without waiting for a response.

.. code:: cpp

   socket_util.transport(data.data(), data.size());

4. Sample the data available on the socket.

.. code:: cpp

   socket_util.sample_transaction(data);

5. End the simulation using the following API.

.. code:: cpp

   socket_util.end_of_simulation();

Connections to the Custom IP's specified in xo's are made using ``krnl_incr.cfg`` file as below:

::
   
   [connectivity]
   nk=sim_ipc_axis_master_32:1:gt_master
   nk=sim_ipc_axis_slave_32:1:gt_slave
   stream_connect=gt_master.M00_AXIS:increment_1.a
   stream_connect=increment_1.output:gt_slave.S00_AXIS


Note : xo instance name specified on v++ command line  becomes a key which will be used by the external process to refer to a particular stream port 
