Single AIE GMIO XRT 
-------------------

This simple design is to demonstrate about XRT API to support asynchronous GMIO transferring. Here we are having the support of xrt::aie::bo class async fuction for asynchronously transfering the input and output buffers. 

:: 

  xrt::aie::bo” class has “async” function to support async GMIO transferring.

XRT API is able to support graph run, RTP update, graph wait and GMIO transferring for now. 


**How to use the Feature**

        Control AIE graph, GMIO with XRT API only.

.. code:: c++

        auto device = xrt::device(1); //device index=0

        auto uuid = device.load_xclbin(xclbinFilename);
        
        //Create input buffer using xrt native api and allocate the size.

        auto din_buffer  = xrt::aie::bo (device, BLOCK_SIZE_in_Bytes, xrt::bo::flags::normal, /*memory group*/0);
        
        //map the buffer and get the buffer pointer.

        int* dinArray= din_buffer.map<int*>(); 

        auto dout_buffer  = xrt::aie::bo (device, BLOCK_SIZE_in_Bytes, xrt::bo::flags::normal, /*memory group*/0);

        int* doutArray= dout_buffer.map<int*>();

        //pre-processing

        for(int i=0;i<ITERATION*1024/4;i++){

        dinArray[i]=i; 

        }

        //Transfer input buffer asynchronously.

        din_buffer.async("gr.gmioIn",XCL_BO_SYNC_BO_GMIO_TO_AIE,BLOCK_SIZE_in_Bytes,/*offset*/0); 

        //Instantiate graph object using xrt api.

        auto ghdl=xrt::graph(device,uuid,"gr"); 

        ghdl.run(ITERATION); 

        dout_buffer.async("gr.gmioOut",XCL_BO_SYNC_BO_AIE_TO_GMIO,BLOCK_SIZE_in_Bytes,/*offset*/0);
 
        //wait to complete graph.

        ghdl.wait(); 



