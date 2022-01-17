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
`ifndef _AXI4STREAM_VIP_IF_SV_
`define _AXI4STREAM_VIP_IF_SV_
`timescale 1ps/1ps

   `define ARESET_XCHECK "ARESET_XCHECK: ARESET_N can't be X/Z after 1 cycle of clock was detected."
   `define ARESET_XCHECK_SOL "To downgrade, use <hierarchy_path to VIP>.IF.set_enable_xchecks_to_warn(), or filter using clk_enable_xchecks()."
  `define XILINX_RESET_PULSE "XILINX_RESET_PULSE_WIDTH: Holding AXI ARESETN asserted for 16 cycles of the slowest AXI clock is generally a sufficient reset pulse width for Xilinx IP. --UG1037."
  `define XILINX_RESET_PULSE_SOL "To downgrade, use <hierarchy_path to VIP>.IF.set_xilinx_reset_check_to_warn(), or filter using clr_xilinx_reset_check()."
  `define XILINX_TKEEP_SPARSE_HEAD_TAIL_ZERO "XILINX_TKEEP_SPARSE: TKEEP sparse pattern is being set to XIL_AXI4STREAM_TKEEP_ALLOW_HEAD_TAIL_ZERO, but non_head, non_tail zeros was detected."
  `define XILINX_TKEEP_SPARSE_NO_NULL_BYTES "XILINX_TKEEP_SPARSE: TKEEP sparse pattern is being set to XIL_AXI4STREAM_TKEEP_ALLOW_NO_NULL_BYTES but zero was detected."
  `define XILINX_TKEEP_SPARSE_HEAD_ZERO "XILINX_TKEEP_SPARSE: TKEEP sparse pattern is being set to XIL_AXI4STREAM_TKEEP_ALLOW_HEAD_ZERO but non_head zero was detected."
  `define XILINX_TKEEP_SPARSE_TAIL_ZERO "XILINX_TKEEP_SPARSE: TKEEP sparse pattern is being set to XIL_AXI4STREAM_TKEEP_ALLOW_TAIL_ZERO but non_tail zero was detected."
  `define XILINX_TKEEP_SPARSE_SOL "To downgrade, use <hierarchy_path to VIP>.IF.set_xilinx_tkeep_check_to_warn(), or filter using clr_xilinx_tkeep_check()."

  
 //SEV_NUM is in this order to match legacy APIs
`define REPORT_MACRO(MSG,SOL,SEV_NUM) \
  if(SEV_NUM ==2'b01) $display("WARNING: %0t %m : %s %s",$realtime,MSG,SOL); \
  else if (SEV_NUM ==2'b10) $display("ERROR: %0t %m  : %s %s",$realtime,MSG,SOL); \
 
  
import axi4stream_vip_pkg::*;

interface axi4stream_vip_if  #(
      xil_axi4stream_sigset_t C_AXI4STREAM_SIGNAL_SET         = 8'h0, 
      int                     C_AXI4STREAM_DEST_WIDTH         = 32, 
                              C_AXI4STREAM_DATA_WIDTH         = 64, 
                              C_AXI4STREAM_ID_WIDTH           = 4, 
                              C_AXI4STREAM_USER_WIDTH         = 0,
                              C_AXI4STREAM_USER_BITS_PER_BYTE = 0,
                              C_AXI4STREAM_HAS_ARESETN        = 1)
  (input bit ACLK, ACLKEN, ARESET_N);
  parameter time C_HOLD_TIME      = 1ps;
  parameter integer C_MAXWAITS    = 16000;
  wire  TREADY;
  wire  TVALID;
  wire  TLAST;
  wire [((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DEST] == 0 ) ? 0 : C_AXI4STREAM_DEST_WIDTH   - 1) : 0] TDEST;
  wire [((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_ID  ] == 0 ) ? 0 : C_AXI4STREAM_ID_WIDTH     - 1) : 0] TID;
  wire [((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER] == 0 ) ? 0 : C_AXI4STREAM_USER_WIDTH   - 1) : 0] TUSER;
  wire [((C_AXI4STREAM_DATA_WIDTH == 0 ) ? 0 : (C_AXI4STREAM_DATA_WIDTH - 1)) :   0] TDATA;
  wire [((C_AXI4STREAM_DATA_WIDTH == 0 ) ? 0 : ((C_AXI4STREAM_DATA_WIDTH/8)-1)) : 0] TKEEP;
  wire [((C_AXI4STREAM_DATA_WIDTH == 0 ) ? 0 : ((C_AXI4STREAM_DATA_WIDTH/8)-1)) : 0] TSTRB;

  logic  ready = 1'b0;
  logic  valid = 1'b0;
  logic  last;
  logic [((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DEST] == 0 ) ? 0 : C_AXI4STREAM_DEST_WIDTH   - 1) : 0] dest;
  logic [((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_ID  ] == 0 ) ? 0 : C_AXI4STREAM_ID_WIDTH     - 1) : 0] id;
  logic [((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER] == 0 ) ? 0 : C_AXI4STREAM_USER_WIDTH   - 1) : 0] user;
  logic [(C_AXI4STREAM_DATA_WIDTH   - 1) : 0] data;
  logic [((C_AXI4STREAM_DATA_WIDTH/8)-1) : 0] keep;
  logic [((C_AXI4STREAM_DATA_WIDTH/8)-1) : 0] strb;

  logic                                   intf_is_master = 0;
  logic                                   intf_is_slave  = 0;
  logic [1:0]                             xilinx_reset_check_enable = 2'b10;
  logic [1:0]                             enable_xchecks = 2'b10;
  logic                                   fake_reset;
  logic                                   real_reset;
  logic                                   reset_for_assert;
  logic [1:0]                             xilinx_tkeep_check_enable = 2'b00;
  typedef struct {bit tkeep_carry =0;
                  bit tkeep_first =0;
                  integer unsigned tkeep_toggle_cnt =0;
                 } tkeep_chk_struct;
  tkeep_chk_struct                        tkeep_chk_s[integer];
  integer unsigned                        report_tkeep_sparse_err_cnt =0;
  integer unsigned                        report_areset_xcheck_err_cnt =0;
  integer unsigned                        report_reset_pulse_err_cnt =0;
  xil_axi4stream_tkeep_sparse_pattern_t   tkeep_sparse_pattern = XIL_AXI4STREAM_TKEEP_ALLOW_HEAD_TAIL_ZERO;       
  int                                     reset_cnt;

  assign TVALID =                                               intf_is_master ? valid : 'z;
  assign TREADY = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_READY]) ? (intf_is_slave  ? ready : 'z) : '1;
  assign TDATA  = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DATA])  ? (intf_is_master ? data  : 'z) : '0;
  assign TSTRB  = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DATA])  ? (intf_is_master ? strb  : 'z) : '1;
  assign TKEEP  = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_KEEP])  ? (intf_is_master ? keep  : 'z) : '1;
  assign TUSER  = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER])  ? (intf_is_master ? user  : 'z) : '0;
  assign TLAST  = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_LAST])  ? (intf_is_master ? last  : 'z) : '0;
  assign TID    = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_ID])    ? (intf_is_master ? id    : 'z) : '0;
  assign TDEST  = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DEST])  ? (intf_is_master ? dest  : 'z) : '0;

  wire  TREADY_internal;
  wire  TLAST_internal;


  wire [((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DEST] == 0 ) ? 0 : C_AXI4STREAM_DEST_WIDTH   - 1) : 0] TDEST_internal;
  wire [((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_ID  ] == 0 ) ? 0 : C_AXI4STREAM_ID_WIDTH     - 1) : 0] TID_internal;
  wire [((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER] == 0 ) ? 0 : C_AXI4STREAM_USER_WIDTH   - 1) : 0] TUSER_internal;
  wire [(C_AXI4STREAM_DATA_WIDTH   - 1) : 0] TDATA_internal;
  wire [((C_AXI4STREAM_DATA_WIDTH/8)-1) : 0] TKEEP_internal;
  wire [((C_AXI4STREAM_DATA_WIDTH/8)-1) : 0] TSTRB_internal;

  assign TREADY_internal  = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_READY]== 0) ? 1'b1 : TREADY;
  assign TLAST_internal   = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_LAST] == 0) ? 1'b1 : TLAST;
  assign TDEST_internal   = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DEST] == 0) ? 1'bz : TDEST;
  assign TID_internal     = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_ID]   == 0) ? 1'bz : TID;
  assign TUSER_internal   = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER] == 0) ? 1'bz : TUSER;
  assign TDATA_internal   = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DATA] == 0) ? {C_AXI4STREAM_DATA_WIDTH{1'bz}} : TDATA;
  assign TKEEP_internal   = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_KEEP] == 0) ? {(C_AXI4STREAM_DATA_WIDTH/8){1'bz}} : TKEEP;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //When there isn't a STRB but there is KEEP then STRB must track KEEP
  assign TSTRB_internal   = (C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_STRB] == 0) ? TKEEP_internal : TSTRB;
  integer unsigned    beat_count = 0;
  integer unsigned    last_count = 0;
  wire ACLK_internal = (ACLKEN == 1'b0) ? 1'b0 : ACLK;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  axi4stream_vip_axi4streampc #(
    .DEST_WIDTH          ((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DEST] == 0 ) ? 0 : C_AXI4STREAM_DEST_WIDTH   ),
    .DATA_WIDTH_BYTES    ((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DATA] == 0 ) ? 0 : C_AXI4STREAM_DATA_WIDTH/8 ),
    .ID_WIDTH            ((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_ID  ] == 0 ) ? 0 : C_AXI4STREAM_ID_WIDTH     ),
    .USER_BITS_PER_BYTE  ((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER] == 0 ) ? 0 : C_AXI4STREAM_USER_BITS_PER_BYTE   ),
    .USER_WIDTH          ((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER] == 0 ) ? 0 : C_AXI4STREAM_USER_WIDTH   ),
    .MAXWAITS            ( C_MAXWAITS ),
    .RecommendOn         ( 1  ),
    .RecMaxWaitOn        ( 0  ),
    .HAS_ARESETN         ( C_AXI4STREAM_HAS_ARESETN)
  ) PC (
    .ACLK               (ACLK             ),
    .ACLKEN             (ACLKEN           ),
    .ARESETn            (ARESET_N         ),
    .TVALID             (TVALID           ),
    .TREADY             (TREADY_internal  ),
    .TDATA              (TDATA_internal[((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DATA] == 0 ) ? 1 : C_AXI4STREAM_DATA_WIDTH)-1:0]),
    .TSTRB              (TSTRB_internal   ),
    .TKEEP              (TKEEP_internal   ),
    .TDEST              (TDEST_internal   ),
    .TID                (TID_internal     ),
    .TLAST              (TLAST_internal   ),
    .TUSER              (TUSER_internal   )
  );
  
  /*
    Function: set_intf_slave
    Sets interface to slave mode
  */
  function void set_intf_slave();
    intf_is_master = 0;
    intf_is_slave = 1;
  endfunction : set_intf_slave

  /*
    Function: set_intf_master
    Sets interface to master mode
  */
  function void set_intf_master();
    intf_is_master = 1;
    intf_is_slave = 0;
  endfunction : set_intf_master

  /*
    Function: set_intf_monitor
    Sets interface to monitor mode
  */
  function void set_intf_monitor();
    intf_is_master = 0;
    intf_is_slave = 0;
  endfunction : set_intf_monitor

  
  /*
  *  Function: set_xilinx_reset_check
  *  Sets xilinx_reset_check_enable to turn on error message when there is a violation of rule which is
  *  holding AXI ARESETN asserted for 16 cycles of the slowest AXI clock is generally a sufficient reset
  *  pulse width for Xilinx IP--UG1037
  */
  function void set_xilinx_reset_check();
    xilinx_reset_check_enable = 2'b10;
  endfunction

  /*
  * Function: set_xilinx_reset_check_to_warn
  * Sets xilinx_reset_check_enable to turn on warning message when there is a violation of rule which is
  * holding AXI ARESETN asserted for 16 cycles of the slowest AXI clock is generally a sufficient reset
  * pulse width for Xilinx IP--UG1037
  */
  function void set_xilinx_reset_check_to_warn();
    xilinx_reset_check_enable = 2'b01;
  endfunction

  /*
  * Function: clr_xilinx_reset_check
  * Sets xilinx_reset_check_enable to turn off warning/error message when there is a violation of rule which is
  * holding AXI ARESETN asserted for 16 cycles of the slowest AXI clock is generally a sufficient reset
  * pulse width for Xilinx IP--UG1037
  */
  function void clr_xilinx_reset_check();
    xilinx_reset_check_enable = 2'b00;
  endfunction

  /*
  *  Function: set_xilinx_tkeep_check
  *  Sets xilinx_tkeep_check_enable to turn on error message when sparse tkeep is being detected 
  *  in the transaction.
  */
  function void set_xilinx_tkeep_check();
    xilinx_tkeep_check_enable = 2'b10;
  endfunction

  /*
  * Function: set_xilinx_tkeep_check_to_warn
  * Sets xilinx_tkeep_check_enable to turn off warning/error message when sparse tkeep is 
  * being detected in the transaction.
  */
  function void set_xilinx_tkeep_check_to_warn();
    xilinx_tkeep_check_enable = 2'b01;
  endfunction

  /*
  * Function: clr_xilinx_tkeep_check
  * Sets xilinx_tkeep_check_enable to turn off warning/error message when sparse tkeep is 
  * being detected in the transaction.
  */
  function void clr_xilinx_tkeep_check();
    xilinx_tkeep_check_enable = 2'b00;
  endfunction

  /*
  * Function: set_enable_xchecks
  * Sets enable_xchecks to turn on error message when reset signal is unknown after 1 cycle of clock.
  */
  function void set_enable_xchecks();
    enable_xchecks = 2'b10;
  endfunction
  
  /*
  * Function: set_enable_xchecks_to_warn
  * Sets enable_xchecks to downgrade/upgrade into warning message when reset signal is unknown after 1 cycle of clock.
  */
  function void set_enable_xchecks_to_warn();
    enable_xchecks = 2'b01;
  endfunction       
  
  /*
  * Function: clr_enable_xchecks
  * Sets enable_xchecks to downgrade error/warning message into info message when reset signal is unknown after 1 cycle of clock.
  */
  function void clr_enable_xchecks();
    enable_xchecks = 2'b00;
  endfunction

  /*
  Function: set_tkeep_sparse_pattern
  * Sets tkeep_sparse_pattern 
  */
  function void set_tkeep_sparse_pattern(xil_axi4stream_tkeep_sparse_pattern_t pattern);
    tkeep_sparse_pattern = pattern;
    report_tkeep_sparse_err_cnt =0;
  endfunction 
 
  /*
  Function: get_tkeep_sparse_pattern
  * Gets tkeep_sparse_pattern 
  */
  function xil_axi4stream_tkeep_sparse_pattern_t get_tkeep_sparse_pattern();
    return(tkeep_sparse_pattern);
  endfunction

  assign real_reset = !ARESET_N;
  assign reset_for_assert = fake_reset & real_reset;
  initial begin
    fake_reset =0;
    @(posedge ACLK);
    fake_reset =1;
  end

  //sample tkeep into a queue till tlast is high, then do tkeep sparse check
  function void tkeep_sample_check(int id, logic[(C_AXI4STREAM_DATA_WIDTH/8)-1:0] tkeep, bit tlast,xil_axi4stream_tkeep_sparse_pattern_t sparse_pattern);
    //when id doesn't exist in the assoicate array of tkeep struct,
    // initialize tkeep_carry, tkeep_first and tkeep_toggle_cnt
    if(!tkeep_chk_s.exists(id)) begin
      tkeep_chk_s[id].tkeep_carry = tkeep[0];
      tkeep_chk_s[id].tkeep_first = tkeep[0];
      tkeep_chk_s[id].tkeep_toggle_cnt =0;
    end  

    // calculate the togggle cnt if tkeep has more than 1 bit
    if(C_AXI4STREAM_DATA_WIDTH/8>1) begin  
      for(int i =0; i<C_AXI4STREAM_DATA_WIDTH/8-1; i++) begin
        if(tkeep[i+1] == ~tkeep[i]) begin
          tkeep_chk_s[id].tkeep_toggle_cnt +=1;
        end
      end
    end  
    
    // check carry on tkeep bit(last tkeep bit) from previous tkeep with current first tkeep bit
    if(tkeep_chk_s[id].tkeep_carry == ~ tkeep[0] )begin
      tkeep_chk_s[id].tkeep_toggle_cnt +=1;
    end  
    
    // replace tkeep_carry[id] with current last tkeep bit
    tkeep_chk_s[id].tkeep_carry = tkeep[C_AXI4STREAM_DATA_WIDTH/8-1];

    //start to check tkeep of a packet or a transfer
    check_tkeep_sparse(sparse_pattern,tkeep_chk_s[id]);

    //delete the check struct information of this ID when tlast is one
    if(tlast ==1) begin
      tkeep_chk_s.delete(id);
    end  
  endfunction  :tkeep_sample_check

  //check the pattern of a stack of tkeep according to its sparse pattern
  function void check_tkeep_sparse(xil_axi4stream_tkeep_sparse_pattern_t sparse, tkeep_chk_struct tkeep_chk);
    // if no toggle, error if tkeep_chk.tkeep_toggle_cnt>0 
    if(sparse == XIL_AXI4STREAM_TKEEP_NO_NULL_BYTES) begin
      if(tkeep_chk.tkeep_toggle_cnt > 0 ) begin
        `REPORT_MACRO(`XILINX_TKEEP_SPARSE_NO_NULL_BYTES,`XILINX_TKEEP_SPARSE_SOL,xilinx_tkeep_check_enable)
        report_tkeep_sparse_err_cnt +=1;
      end 
    // if allow head zero, error if tkeep_chk.tkeep_toggle_cnt >1 or tkeep_chk.tkeep_toggle_cnt =1 but with head start with 1
    end else if(sparse == XIL_AXI4STREAM_TKEEP_ALLOW_HEAD_ZERO) begin
      if(tkeep_chk.tkeep_toggle_cnt > 1 || (tkeep_chk.tkeep_toggle_cnt ==1 && tkeep_chk.tkeep_first ==1) ) begin
        `REPORT_MACRO(`XILINX_TKEEP_SPARSE_HEAD_ZERO,`XILINX_TKEEP_SPARSE_SOL,xilinx_tkeep_check_enable)
        report_tkeep_sparse_err_cnt +=1;
      end
    // if allow tail zero, error if tkeep_chk.tkeep_toggle_cnt>1 or tkeep_chk.tkeep_toggle_cnt=1 but with head start with 0  
    end else if(sparse == XIL_AXI4STREAM_TKEEP_ALLOW_TAIL_ZERO) begin
      if(tkeep_chk.tkeep_toggle_cnt >1 || (tkeep_chk.tkeep_toggle_cnt ==1 && tkeep_chk.tkeep_first ==0)) begin
        `REPORT_MACRO(`XILINX_TKEEP_SPARSE_TAIL_ZERO,`XILINX_TKEEP_SPARSE_SOL,xilinx_tkeep_check_enable)
        report_tkeep_sparse_err_cnt +=1;
      end  
    // if allow both head and tail zero, error if tkeep_chk.tkeep_toggle_cnt > 2 or tkeep_chk.tkeep_toggle_cnt=2 but with head start with 1  
    end else begin
      if(tkeep_chk.tkeep_toggle_cnt >2 || (tkeep_chk.tkeep_toggle_cnt ==2 && tkeep_chk.tkeep_first ==1)) begin
        `REPORT_MACRO(`XILINX_TKEEP_SPARSE_HEAD_TAIL_ZERO,`XILINX_TKEEP_SPARSE_SOL,xilinx_tkeep_check_enable)
        report_tkeep_sparse_err_cnt +=1;
      end  
    end
  endfunction  : check_tkeep_sparse


  always @(posedge ACLK) begin
    if(TVALID&TREADY &&(report_tkeep_sparse_err_cnt==0)) begin
      tkeep_sample_check(TID_internal,TKEEP_internal,TLAST_internal,tkeep_sparse_pattern);
    end
 end   

     
  
   always @(posedge ACLK) begin
     if ($isunknown(ARESET_N) && (report_areset_xcheck_err_cnt==0)) begin
       `REPORT_MACRO(`ARESET_XCHECK,`ARESET_XCHECK_SOL,enable_xchecks)
       report_areset_xcheck_err_cnt +=1;
     end  
   end
  

  always @(posedge ACLK) begin
    if(reset_for_assert) begin
      reset_cnt =0;
      while(reset_for_assert) begin
        @(posedge ACLK);
        reset_cnt = reset_cnt +1;
      end
      if((reset_cnt <16) &&(report_reset_pulse_err_cnt ==0)) begin
        `REPORT_MACRO(`XILINX_RESET_PULSE,`XILINX_RESET_PULSE_SOL,xilinx_reset_check_enable)
        report_reset_pulse_err_cnt +=1;
      end
    end  
  end



  always @(posedge ACLK) begin
    if (~ARESET_N) begin
      beat_count = 0;
      last_count = 0;
    end else if (ACLKEN) begin
      if ((TVALID == 1) && (TREADY_internal == 1)) begin
        beat_count++;
        if ((C_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_LAST] == 1) && (TLAST_internal == 1)) begin
          last_count++;
        end
      end
    end
  end

  default clocking cb @(posedge ACLK_internal);
    default input #1step output #C_HOLD_TIME;
    input   ARESET_N;
    input   ACLKEN;
    inout   TVALID;
    inout   TREADY;
    inout   TDATA;
    inout   TSTRB;
    inout   TKEEP;
    inout   TDEST;
    inout   TID;
    inout   TUSER;
    inout   TLAST;
  endclocking : cb

endinterface : axi4stream_vip_if

`endif
