X3 Network Traffic Generator (NTG)
==================================

Network traffic generator (NTG) enables X3 users to validate
functionality/performance using hardware emulation flow with ease.
X3 NTG provides user a way to emulate traffic for the X3 device. User has the
option to replay the PCAP or directly provide network data.
NTG also provides mechanism to incorporate delay between frames and handle meta
synchronization. It models the MAC/NIC behavior for emulation flow.
As part of X3 NTG various high level APIs are provided for users.

Setup Instructions
------------------

This library supports both Bash and Csh environments.

Supported Vitis Version: 2022.2

::

    1. source setenv.csh
    2. ./setup.sh

setup.sh creates a local directory "deps" in which relevent dependencies for X3_NTG will be installed 


X3 NTG API
----------


List of APIs available for user 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+---------------------------------+-------------------------+-------------------------+
|                                 | Transmit API            | Receive API             |
+---------------------------------+-------------------------+-------------------------+
| tx_in                           |  * enqueue              |                         |
|                                 |  * send                 |                         |
|                                 |  * generate_traffic     |                         |
+---------------------------------+-------------------------+-------------------------+
| tx_out                          |                         | * receive               |
|                                 |                         | * receive_traffic       |
+---------------------------------+-------------------------+-------------------------+
| rx_in                           |  * enqueue              |                         |
|                                 |  * send                 |                         |
|                                 |  * generate_traffic     |                         |
+---------------------------------+-------------------------+-------------------------+
| rx_out                          |                         | * receive               |
|                                 |                         | * receive_traffic       |
+---------------------------------+-------------------------+-------------------------+


* In order to send/receive individual packets, one can make use of ``enqueue``, ``send`` and ``receive`` APIs.
* Each individual packet received could be cross-cheked to verify the functionality of the application running on X3.
* If one want to replay and capture pcap files, ``generate_traffic`` and ``receive_traffic`` API can be used.
* For using generate_traffic and receive_traffic, a .txt file should be provided as argument.



Usage of each API is described as follows :
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+------------------+-----------------------------------------------------------------------------------------------------------------------+----------------------------------------------------+
| API name         |          Description                                                                                                  |   Example                                          |
+==================+=======================================================================================================================+====================================================+
| enqueue          | * This API will only enqueue the operation in the command queue but it will not execute until ``send`` API is invoked |  ::                                                |
|                  | * Each packet can be enqueued to any of the available four ports 0,1,2,3.                                             |                                                    |  
|                  |                                                                                                                       |      txSendObj = txin()                            |
|                  |                                                                                                                       |      ...                                           |
|                  |                                                                                                                       |      txSendObj.enqueue(pkt, 0, 1500)               |  
+------------------+-----------------------------------------------------------------------------------------------------------------------+----------------------------------------------------+
| send             | * This API will send all packets which were enqueued.                                                                 |  ::                                                |
|                  | * Multiple packets which were enqueued will be sent through respective port                                           |                                                    |
|                  |                                                                                                                       |      txSendObj = txin()                            |
|                  |                                                                                                                       |      txSendObj.enqueue(pkt, 0, 1500)               |
|                  |                                                                                                                       |      ...                                           |
|                  |                                                                                                                       |      txSendObj.send()                              |
|                  |                                                                                                                       |                                                    |
+------------------+-----------------------------------------------------------------------------------------------------------------------+----------------------------------------------------+
| receive          | * This API, when invoked, will receive packets arrived at the specified port                                          |  ::                                                |
|                  |                                                                                                                       |                                                    |
|                  |                                                                                                                       |      txReceiveObj = rxout()                        |
|                  |                                                                                                                       |      ...                                           |
|                  |                                                                                                                       |      frame = txReceiveObj.receive(0)               |
+------------------+-----------------------------------------------------------------------------------------------------------------------+----------------------------------------------------+
| generate_traffic | * This API will read the network frame information from PCAP file and handle them accordingly                         |  ::                                                |
|                  | * It internally calls ``enqueue``, ``send`` to send packets present in PCAP file.                                     |                                                    |
|                  | * It takes .txt file which has tuple of pcap file paths and port numbers.                                             |      rxSendObj = rxin()                            |
|                  |                                                                                                                       |      fileName = "path/to/txt/file"                 |
|                  |                                                                                                                       |      ...                                           |
|                  |                                                                                                                       |      rxin.generate_traffic(fileName)               |
+------------------+-----------------------------------------------------------------------------------------------------------------------+----------------------------------------------------+
| receive_traffic  | * This API is used for receiving traffic available at required port                                                   |  ::                                                |
|                  | * It also generates output in PCAP format and hence requires path where pcap file is to be created                    |                                                    |
|                  | * It also requires a list containing number of frames to be received at each individual port [0,1,2,3]                |      txReceieveObj = txout()                       |
|                  |                                                                                                                       |      fileName = "path/to/txt/file/to/be/created    |
|                  |                                                                                                                       |      recCnt = [20,30,10,20]                        |
|                  |                                                                                                                       |      ...                                           |
|                  |                                                                                                                       |      txReceiveObj.receive_traffic(fileName, recCnt)|
+------------------+-----------------------------------------------------------------------------------------------------------------------+----------------------------------------------------+

Note :
.txt file used in pcap flow shoud be of following format. All four ports need not be specified.
  ::

           ./files/file0.pcap, 0
           ./files/file1.pcap, 1
           ./files/file2.pcap, 2
           ./files/file3.pcap, 3
 

Detailed Description of each API is provided below
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: python

    def enqueue(network_frame, ethernet_port_number, delay, frame_type)

     '''
     Parameters -
               network_frame - Input network frame
               ethernet_port - Ethernet port number
               delay - Inter frame delay value can be set in clock cycles (Default: 5 clock cycles)
               frame_type - This represents type of the frame (Good/Bad)
                            Valid Enum types: x3ntg_errcode.GOOD_FRAME, x3ntg_errcode.BAD_FRAME.
                                              By default it is set to x3ntg_errcode.GOOD_FRAME
      
     Returns - N/A
     '''
    
.. code:: python  

    def send()

    '''
    Returns - N/A
    '''
.. code:: python 
   
   def generate_traffic(list_pcap_ethernet_values, scaling_factor)

    '''
    Parameters -               
            list_pcap_ethernet_values - Input contains list of tuples <pcap_name, ethernet_number>
            interframe_delay_mode - x3ntg_iframe_delay.FIXED  -> fixed inter frame delay between frames is maintained
                                    x3ntg_iframe_delay.PCAP -> PCAP based inter frame delay is calculated based on EPOCH times in PCAP file
            tsfactor_delay <optional> - User can choose a fixed delay in case of FIXED mode (Default: 5 clock cycles)
                                        User can choose the delay time scaling factor in case of PCAP mode
                                        Example - If scaling factor is 100 then time gap in PCAP will be divided by 100
                                               and passed to simulation as the final delay.  
    Returns - N/A
    '''

.. code:: python
    
    def receive(port)

    '''
    Parameters:
            port: Input port number must be supplied

    Returns: This API returns a structure <frame, port, error_code>
             Here error code can be representing
             1. x3ntg_errcode.BAD_FRAME or x3ntg_errcode.WRONG_PARITY.
             2. In case of no errors by default it returns x3ntg_errcode.GOOD_FRAME

    '''

.. code:: python

    def receive_traffic(list_pcap_ethernet_values, list_receive_sizes_per_pcap)

    '''
    Parameters:  
            list_pcap_ethernet_values: Input contains list of tuples <pcap_name, ethernet_number>
            list_receive_sizes_per_pcap: For each output pcap file, receive_size is provided by user. 
                                         this API tries to ensures receive_size number of frames are dumped into output PCAP file.

    Returns: N/A
    '''



 
