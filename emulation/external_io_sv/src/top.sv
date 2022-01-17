/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

`timescale 1ns / 1ps

import axi4stream_vip_pkg::*;
import ex_sim_axi4stream_vip_mst_0_pkg::*;
import ex_sim_axi4stream_vip_slv_0_pkg::*;

module top(
  );

  // Error count to check how many comparison failed
  xil_axi4stream_uint                            error_cnt = 0; 
  // Comparison count to check how many comparsion happened
  xil_axi4stream_uint                            comparison_cnt = 0;
 
  event      checkCompleteEvent;


  // Monitor transaction from master VIP
  axi4stream_monitor_transaction                 mst_monitor_transaction;
  // Monitor transaction queue for master VIP 
  axi4stream_monitor_transaction                 master_moniter_transaction_queue[$];
  // Size of master_moniter_transaction_queue
  xil_axi4stream_uint                           master_moniter_transaction_queue_size =0;
  // Scoreboard transaction from master monitor transaction queue
  axi4stream_monitor_transaction                 mst_scb_transaction;
  // Monitor transaction for slave VIP
  axi4stream_monitor_transaction                 slv_monitor_transaction;
  // Monitor transaction queue for slave VIP
  axi4stream_monitor_transaction                 slave_moniter_transaction_queue[$];
  // Size of slave_moniter_transaction_queue
  xil_axi4stream_uint                            slave_moniter_transaction_queue_size =0;
  // Scoreboard transaction from slave monitor transaction queue
  axi4stream_monitor_transaction                 slv_scb_transaction;

  // Master VIP agent verbosity level
  xil_axi4stream_uint                           mst_agent_verbosity = 0;
  // Slave VIP agent verbosity level
  xil_axi4stream_uint                           slv_agent_verbosity = 0;

  //instantiating vip agents
  ex_sim_axi4stream_vip_mst_0_mst_t                              mst_agent;
  ex_sim_axi4stream_vip_slv_0_slv_t                              slv_agent;



  //DUT INSTANTIATION ; CLOCKING AND RESET


  // Clock signal
  bit                                     clock;
  // Reset signal
  bit                                     reset;

  // instantiate bd
  des DUT(
    .axis_aresetn(reset),
    .axis_aclk(clock)
  );

  initial begin
    clock <= 1'b0;
    reset <= 1'b0;
    #10 reset <= 1'b1;
  end
  
  always #10 clock <= ~clock;


  initial begin

    mst_agent = new("master vip agent",DUT.axi4stream_vip_mst.inst.IF);
    slv_agent = new("slave vip agent",DUT.axi4stream_vip_slv.inst.IF);
    $timeformat (-12, 1, " ps", 1);


    mst_agent.vif_proxy.set_dummy_drive_type(XIL_AXI4STREAM_VIF_DRIVE_NONE);
    slv_agent.vif_proxy.set_dummy_drive_type(XIL_AXI4STREAM_VIF_DRIVE_NONE);

    mst_agent.set_agent_tag("Master VIP");
    slv_agent.set_agent_tag("Slave VIP");

    // set print out verbosity level.
    mst_agent.set_verbosity(mst_agent_verbosity);
    slv_agent.set_verbosity(slv_agent_verbosity);
 
    mst_agent.start_master();
    slv_agent.start_slave();

    /***************************************************************************************************
    * Fork off the process
    * Master VIP create transaction
    * Slave VIP create TREADY if it is on
    * Passthrough VIP starts to monitor 
    ***************************************************************************************************/

    fork
      begin
        for(int i = 0; i < 10;i++) begin
          mst_gen_transaction();
        end  
        $display("Looped master to slave transfers example with randomization completes");
      end
      begin
        slv_gen_tready();
      end
    join_any
    #1ns;
    
    $display("waiting for check completion event"); 
    @(checkCompleteEvent);
    $display("check completed"); 
     
    if(error_cnt ==0) begin
      $display("EXAMPLE TEST DONE : Test Completed Successfully");
    end else begin  
      $display("EXAMPLE TEST DONE ",$sformatf("Test Failed: %d Comparison Failed", error_cnt));
    end 
    $finish;

  end


  task slv_gen_tready();
    axi4stream_ready_gen                           ready_gen;
    ready_gen = slv_agent.driver.create_ready("ready_gen");
    ready_gen.set_ready_policy(XIL_AXI4STREAM_READY_GEN_OSC);
    ready_gen.set_low_time(2);
    ready_gen.set_high_time(6);
    slv_agent.driver.send_tready(ready_gen);
  endtask :slv_gen_tready

  task mst_gen_transaction();
    axi4stream_transaction                         wr_transaction; 
    wr_transaction = mst_agent.driver.create_transaction("Master VIP write transaction");
    wr_transaction.set_xfer_alignment(XIL_AXI4STREAM_XFER_RANDOM);
    WR_TRANSACTION_FAIL: assert(wr_transaction.randomize());
    $display("Transaction sent to Vitis Apllication", wr_transaction.convert2string());
    wr_transaction.set_last(1);  
    mst_agent.driver.send(wr_transaction);
  endtask

  task automatic send_a_packet(xil_axi4stream_uint num_words);
    xil_axi4stream_uint                     total_transfer;
    bit[4*8-1:0]              data_beat;
    bit[4-1:0]            keep_beat;
    bit[31:0]                               payload; 
    bit [7:0]                               byte_mem[xil_axi4stream_ulong];
    axi4stream_transaction                  wr_transaction; 
    axi4stream_transaction                  wr_transactionc;    
  
    
    if((num_words*4)%(4 ) !=0) begin
      $display("Warning, stream VIP TDATA_WIDTH is %d byte, the number of words you wants to pass in a whole packet is %d, the last transfer will be filled with 0",4,num_words );
    end
    
    for(xil_axi4stream_uint word_cnt=0; word_cnt<num_words; word_cnt++) begin
      payload = word_cnt;
      for(xil_axi4stream_uint byte_cnt=0; byte_cnt<4; byte_cnt++) begin
        byte_mem[word_cnt*4+byte_cnt] = payload[byte_cnt*8+:8];
      end  
    end  
   
    if( (num_words*4)%(4 ) !=0) begin
      total_transfer = (num_words*4)/(4 ) +1;
    end else begin
      total_transfer = (num_words*4)/(4 ) ;
    end
  
    for(xil_axi4stream_uint i=0; i<total_transfer; i++) begin
      for(xil_axi4stream_uint byte_cnt=0; byte_cnt<(4); byte_cnt++) begin
        data_beat[byte_cnt*8+:8] = byte_mem[byte_cnt+i*(4)];
      end  
      wr_transaction = mst_agent.driver.create_transaction("Master VIP write transaction");
      wr_transactionc = mst_agent.driver.create_transaction("Master VIP write transactionc");
      wr_transaction.set_driver_return_item_policy(XIL_AXI4STREAM_AT_ACCEPT_RETURN );
      SEND_PACKET_FAILURE: assert(wr_transaction.randomize());
      wr_transaction.set_data_beat(data_beat);
      wr_transaction.set_last(0);
      if(i == total_transfer-1) begin
        wr_transaction.set_last(1);  
      end     
      mst_agent.driver.send(wr_transaction);
      mst_agent.driver.seq_item_port.get_next_rsp(wr_transactionc);
    end
  endtask  :send_a_packet


  /***************************************************************************************************
  * Get monitor transaction from master VIP monitor analysis port
  * Put the transactin into master monitor transaction queue 
  ***************************************************************************************************/
  initial begin
    forever begin
      mst_agent.monitor.item_collected_port.get(mst_monitor_transaction);
      master_moniter_transaction_queue.push_back(mst_monitor_transaction);
      master_moniter_transaction_queue_size++;
    end  
  end 

  /***************************************************************************************************
  * Get monitor transaction from slave VIP monitor analysis port
  * Put the transactin into slave monitor transaction queue 
  ***************************************************************************************************/
  initial begin
    forever begin
      slv_agent.monitor.item_collected_port.get(slv_monitor_transaction);
      slave_moniter_transaction_queue.push_back(slv_monitor_transaction);
      slave_moniter_transaction_queue_size++;
    end
  end


  always @(comparison_cnt) begin
      //$display("Number of comparisons =",comparison_cnt); 
      if(comparison_cnt == 10) begin
        -> checkCompleteEvent;
      end 
  end


  initial begin
   xil_axi4stream_data_byte _data[];
   int unsigned _word;
   forever begin
      wait (master_moniter_transaction_queue_size>0 ) begin
        mst_scb_transaction = master_moniter_transaction_queue.pop_front;
        _data = new[4];
        mst_scb_transaction.get_data(_data);
        _word=0; 
        foreach(_data[i]) begin
          _word += _data[i] << 8*i;
        end
        //_word += 32'h01010101;
        _word += 32'h1;
        foreach(_data[i]) begin
          _data[i] = (_word >> 8*i) & 8'hFF;
        end
        mst_scb_transaction.set_data(_data);
        $display("Expected payload \n", mst_scb_transaction.convert2string());
        master_moniter_transaction_queue_size--;
        wait (slave_moniter_transaction_queue_size>0 ) begin
          slv_scb_transaction = slave_moniter_transaction_queue.pop_front;
          slave_moniter_transaction_queue_size--;
          $display("Received payload \n", slv_scb_transaction.convert2string());
          if (slv_scb_transaction.do_compare(mst_scb_transaction) == 0) begin
            $display("ERROR:  Master VIP against Slave VIP scoreboard Compare failed");
            error_cnt++;
          end else begin
            $display("SUCCESS: %d, Master VIP against Slave VIP scoreboard Compare passed", comparison_cnt+1);
          end
          comparison_cnt++;
        end  
      end 
    end
  end

endmodule

