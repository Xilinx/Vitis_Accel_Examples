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

`timescale 1ps/1ps

(* DowngradeIPIdentifiedWarnings="yes" *) 
module axi4stream_vip_v1_1_9_top #
  (
   parameter [31:0]  C_AXI4STREAM_SIGNAL_SET         = 32'h03,
   parameter integer C_AXI4STREAM_INTERFACE_MODE     = 1,  //master, slave and bypass
   parameter integer C_AXI4STREAM_DATA_WIDTH        = 8,
   parameter integer C_AXI4STREAM_USER_BITS_PER_BYTE = 0,
   parameter integer C_AXI4STREAM_ID_WIDTH          = 0,
   parameter integer C_AXI4STREAM_DEST_WIDTH        = 0,
   parameter integer C_AXI4STREAM_USER_WIDTH        = 0,
   parameter integer C_AXI4STREAM_HAS_ARESETN       = 1
   )
  (
   // System Signals
   input wire aclk,
   input wire aresetn,
   input wire aclken,

   // Slave side
   input  wire                                                                    s_axis_tvalid,
   output wire                                                                    s_axis_tready,
   input  wire [C_AXI4STREAM_DATA_WIDTH==0? 0:C_AXI4STREAM_DATA_WIDTH-1:0]        s_axis_tdata,
   input  wire [C_AXI4STREAM_DATA_WIDTH/8==0? 0:C_AXI4STREAM_DATA_WIDTH/8-1:0]    s_axis_tstrb,
   input  wire [C_AXI4STREAM_DATA_WIDTH/8==0? 0:C_AXI4STREAM_DATA_WIDTH/8-1:0]    s_axis_tkeep,
   input  wire                                                                    s_axis_tlast,
   input  wire [C_AXI4STREAM_ID_WIDTH==0? 0:C_AXI4STREAM_ID_WIDTH-1:0]            s_axis_tid,
   input  wire [C_AXI4STREAM_DEST_WIDTH==0? 0:C_AXI4STREAM_DEST_WIDTH-1:0]        s_axis_tdest,
   input  wire [C_AXI4STREAM_USER_WIDTH==0? 0:C_AXI4STREAM_USER_WIDTH-1:0]        s_axis_tuser,

   // Master side
   output wire                                                                    m_axis_tvalid,
   input  wire                                                                    m_axis_tready,
   output wire [C_AXI4STREAM_DATA_WIDTH==0? 0:C_AXI4STREAM_DATA_WIDTH-1:0]        m_axis_tdata,
   output wire [C_AXI4STREAM_DATA_WIDTH/8==0? 0:C_AXI4STREAM_DATA_WIDTH/8-1:0]    m_axis_tstrb,
   output wire [C_AXI4STREAM_DATA_WIDTH/8==0? 0:C_AXI4STREAM_DATA_WIDTH/8-1:0]    m_axis_tkeep,
   output wire                                                                    m_axis_tlast,
   output wire [C_AXI4STREAM_ID_WIDTH==0? 0:C_AXI4STREAM_ID_WIDTH-1:0]            m_axis_tid,
   output wire [C_AXI4STREAM_DEST_WIDTH==0? 0:C_AXI4STREAM_DEST_WIDTH-1:0]        m_axis_tdest,
   output wire [C_AXI4STREAM_USER_WIDTH==0? 0:C_AXI4STREAM_USER_WIDTH-1:0]        m_axis_tuser
     );

  /*NOTE:  
    C_AXI4STREAM_INTERFACE_MODE =0 -- MASTER MODE, 
    C_AXI4STREAM_INTERFACE_MODE =1 -- PASS-THROUGH MODE 
    C_AXI4STREAM_INTERFACE_MODE =2 -- SLAVE MODE
    Please refer xgui tcl and coreinfo.yml
    user can change PASS_THROUGH VIP to run time master mode or run time slave mode during the simulation 
  */

  /*master_mode means that either the dut is statically being configured to be in master mode
    or it statically being configured to be pass-through mode and switched to be in master mode
    in run time 
   
   slave mode means that either the dut is statically being configured to be in slave mode
   or it statically being configured to be pass-through mode and switched to be in slave mode
   in run time 

   pass-through mode means that either the dut is statically being configured to be in pass-through mode
   or it statically being configured to be pass-through mode and switched to be in master/slave mode and then 
   switch back to be in pass-through mode in run time
  */

  logic runtime_master =0;
  logic runtime_slave =0;

  wire run_slave_mode;
  wire run_master_mode;
  wire run_passth_mode;
  wire compile_master_mode;
  wire compile_slave_mode;
  wire master_mode;
  wire slave_mode;

  assign run_master_mode = (C_AXI4STREAM_INTERFACE_MODE ==1 && runtime_master ==1 &&runtime_slave ==0);
  assign run_slave_mode = C_AXI4STREAM_INTERFACE_MODE ==1 && runtime_slave ==1 && runtime_master ==0;
  assign run_passth_mode = (runtime_slave ==0 && runtime_master ==0);


  assign compile_master_mode = (C_AXI4STREAM_INTERFACE_MODE ==0 || C_AXI4STREAM_INTERFACE_MODE ==1 )&& run_passth_mode ;
  assign compile_slave_mode  = (C_AXI4STREAM_INTERFACE_MODE ==2 || C_AXI4STREAM_INTERFACE_MODE ==1) && run_passth_mode ;

  assign master_mode = compile_master_mode || run_master_mode; 
  assign slave_mode = compile_slave_mode || run_slave_mode;


  //input for slave side , z or input(mux) and then connect to interface
  assign IF.TVALID = slave_mode? s_axis_tvalid: 1'bz; 
  assign IF.TDATA  = slave_mode? s_axis_tdata : {C_AXI4STREAM_DATA_WIDTH==0? 1:C_AXI4STREAM_DATA_WIDTH{1'bz}};
  assign IF.TSTRB  = slave_mode? s_axis_tstrb : {(C_AXI4STREAM_DATA_WIDTH/8==0? 1:C_AXI4STREAM_DATA_WIDTH/8){1'bz}}; 
  assign IF.TKEEP  = slave_mode? s_axis_tkeep :  {(C_AXI4STREAM_DATA_WIDTH/8==0? 1: C_AXI4STREAM_DATA_WIDTH/8){1'bz}};
  assign IF.TLAST  = slave_mode? s_axis_tlast: 1'bz; 
  assign IF.TID    = slave_mode? s_axis_tid: {(C_AXI4STREAM_ID_WIDTH==0? 1: C_AXI4STREAM_ID_WIDTH){1'bz}}; 
  assign IF.TDEST  = slave_mode? s_axis_tdest:{(C_AXI4STREAM_DEST_WIDTH==0? 1: C_AXI4STREAM_DEST_WIDTH){1'bz}}; 
  assign IF.TUSER  = slave_mode? s_axis_tuser: {(C_AXI4STREAM_USER_WIDTH==0? 1: C_AXI4STREAM_USER_WIDTH){1'bz}}; 
  
  //output for slave side, 0 or interface signal(mux)
  assign s_axis_tready = slave_mode? IF.TREADY : {1'b0};

  //input for master side, z or input(mux) and then connect to interface
  assign IF.TREADY = master_mode? m_axis_tready :{1'bz}; 

  //output for master side, 0 or interface(mux)
  assign m_axis_tvalid = master_mode? IF.TVALID : {1'b0};
  assign m_axis_tdata  = master_mode? IF.TDATA : ({C_AXI4STREAM_DATA_WIDTH==0? 1:C_AXI4STREAM_DATA_WIDTH{1'b0}});
  assign m_axis_tstrb  = master_mode? IF.TSTRB : ({C_AXI4STREAM_DATA_WIDTH/8==0? 1:(C_AXI4STREAM_DATA_WIDTH/8){1'b0}});
  assign m_axis_tkeep  = master_mode? IF.TKEEP : ({C_AXI4STREAM_DATA_WIDTH/8==0? 1:(C_AXI4STREAM_DATA_WIDTH/8){1'b0}});
  assign m_axis_tlast  = master_mode? IF.TLAST : {1'b0};
  assign m_axis_tid    = master_mode? IF.TID : ({C_AXI4STREAM_ID_WIDTH==0? 1: C_AXI4STREAM_ID_WIDTH{1'b0}});
  assign m_axis_tdest  = master_mode? IF.TDEST : ({(C_AXI4STREAM_DEST_WIDTH==0? 1: C_AXI4STREAM_DEST_WIDTH){1'b0}});
  assign m_axis_tuser  = master_mode? IF.TUSER : ({(C_AXI4STREAM_USER_WIDTH==0? 1: C_AXI4STREAM_USER_WIDTH){1'b0}}); 

  axi4stream_vip_if #(
    .C_AXI4STREAM_SIGNAL_SET             (C_AXI4STREAM_SIGNAL_SET), 
    .C_AXI4STREAM_DEST_WIDTH             (C_AXI4STREAM_DEST_WIDTH), 
    .C_AXI4STREAM_DATA_WIDTH             (C_AXI4STREAM_DATA_WIDTH),
    .C_AXI4STREAM_ID_WIDTH               (C_AXI4STREAM_ID_WIDTH),
    .C_AXI4STREAM_USER_WIDTH             (C_AXI4STREAM_USER_WIDTH),
    .C_AXI4STREAM_USER_BITS_PER_BYTE     (C_AXI4STREAM_USER_BITS_PER_BYTE),
    .C_AXI4STREAM_HAS_ARESETN            (C_AXI4STREAM_HAS_ARESETN)
  ) IF (
    .ACLK                                (aclk), 
    .ARESET_N                            (aresetn),
    .ACLKEN                              (aclken)
  );  

  //synthesis translate_off
  initial begin
    $display("Xilinx AXI4STREAM VIP Found at Path: %m");
  end

  //set IF mode to be in the correct mode according to C_AXI_INTERFACE_MODE,Default is monitor mode  
  //so when it is in passthrough mode, nothing will be done. otherwise,stop simulation 
  generate
    initial begin
      if(C_AXI4STREAM_INTERFACE_MODE ==0) begin
        IF.set_intf_master;
      end else if(C_AXI4STREAM_INTERFACE_MODE ==2) begin
        IF.set_intf_slave;
      end else if(C_AXI4STREAM_INTERFACE_MODE ==1) begin
        $display("This AXI4STREAM VIP is in passthrough mode");
      end else begin
        $fatal(0,"This AXI4STREAM VIP's mode is out of range");
      end
    end  
  endgenerate

  /*
    Function: set_passthrough_mode
    Sets AXI4STREAM VIP passthrough into run time passthrough mode
  */
  function void set_passthrough_mode();
    if (C_AXI4STREAM_INTERFACE_MODE == 1) begin
      runtime_master = 0;
      runtime_slave = 0;   
      IF.set_intf_monitor;
    end else begin
      $fatal(0,"XilixAXISVIP: VIP was not initially configured as Pass-through. Cannot change mode.Delete non-Passthrough VIP's API call of set_passthrough_mode in the testbench. Refer PG277 section about Useful Coding Guidelines and Example for how to use master/slave/passthrough VIP");
    end
  endfunction : set_passthrough_mode

  /*
    Function: set_master_mode
    Sets AXI4STREAM VIP passthrough into run time master mode
  */
  function void set_master_mode();
    if (C_AXI4STREAM_INTERFACE_MODE == 1) begin
      runtime_master = 1;
      runtime_slave = 0;
      IF.set_intf_master;
    end else begin
      $fatal(0,"XilixAXISVIP: VIP was not initially configured as Pass-through. Cannot change mode.Delete non-Passthrough VIP's API call of set_passthrough_mode in the testbench. Refer PG277 section about Useful Coding Guidelines and Example for how to use master/slave/passthrough VIP");
    end

  endfunction : set_master_mode

  /*
    Function: set_slave_mode
    Sets AXI4STREAM VIP passthrough into run time slave mode
  */
  function void set_slave_mode();
    if (C_AXI4STREAM_INTERFACE_MODE == 1) begin
      runtime_master = 0;
      runtime_slave = 1;
      IF.set_intf_slave;
    end else begin
      $fatal(0,"XilixAXISVIP: VIP was not initially configured as Pass-through. Cannot change mode.Delete non-Passthrough VIP's API call of set_passthrough_mode in the testbench. Refer PG277 section about Useful Coding Guidelines and Example for how to use master/slave/passthrough VIP");
    end
  endfunction : set_slave_mode
  /*
   Function:  set_max_wait_cycles (not available in VIVADO Simulator)
   Sets max_wait_cycles of PC(ARM AXI4 Stream Protocol Checker) 
  */
  function void set_max_wait_cycles(input integer unsigned new_num);
    IF.PC.max_wait_cycles = new_num;
  endfunction

  /*
   Function: get_max_wait_cycles (not available in VIVADO Simulator)
   Returns max_wait_cycles of PC(ARM AXI4 Stream Protocol Checker)
  */
  function integer unsigned get_max_wait_cycles();
    return(IF.PC.max_wait_cycles);
  endfunction :get_max_wait_cycles

  /*
   Function: set_fatal_to_warnings (not available in VIVADO Simulator)
   Sets fatal_to_warnings of PC(ARM AXI4 Stream Protocol Checker) to be 1
  */
  function void set_fatal_to_warnings();
    IF.PC.fatal_to_warnings = 1;
  endfunction : set_fatal_to_warnings

  /*
   Function: clr_fatal_to_warnings (not available in VIVADO Simulator)
   Sets fatal_to_warnings of PC(ARM AXI4 Stream Protocol Checker) to be 0
  */
  function void clr_fatal_to_warnings();
    IF.PC.fatal_to_warnings = 0;
  endfunction : clr_fatal_to_warnings
  //synthesis translate_on

endmodule : axi4stream_vip_v1_1_9_top


