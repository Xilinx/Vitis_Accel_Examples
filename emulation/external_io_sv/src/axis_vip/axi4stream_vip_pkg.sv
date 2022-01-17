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
`ifndef _AXI4STREAM_VIP_PKG_SV_
`define _AXI4STREAM_VIP_PKG_SV_

`timescale 1ps/1ps

package axi4stream_vip_pkg;

  import xil_common_vip_pkg::*;
  `include "xil_common_vip_macros.svh"
`define XIL_AXI4STREAM_PARAM_DECL #(xil_axi4stream_sigset_t C_XIL_AXI4STREAM_SIGNAL_SET=3, int C_XIL_AXI4STREAM_DEST_WIDTH=32, C_XIL_AXI4STREAM_DATA_WIDTH=64, C_XIL_AXI4STREAM_ID_WIDTH = 4, C_XIL_AXI4STREAM_USER_WIDTH=0, C_XIL_AXI4STREAM_USER_BITS_PER_BYTE=0,C_XIL_AXI4STREAM_HAS_ARESETN=0)
`define XIL_AXI4STREAM_PARAM_ORDER #(C_XIL_AXI4STREAM_SIGNAL_SET,C_XIL_AXI4STREAM_DEST_WIDTH,C_XIL_AXI4STREAM_DATA_WIDTH,C_XIL_AXI4STREAM_ID_WIDTH,C_XIL_AXI4STREAM_USER_WIDTH,C_XIL_AXI4STREAM_USER_BITS_PER_BYTE,C_XIL_AXI4STREAM_HAS_ARESETN)

/*
  Type: xil_axi4stream_uint
  Xilinx AXI4STREAM Interger unsigned data type
  |typedef integer unsigned  xil_axi4stream_uint;
*/
typedef integer unsigned  xil_axi4stream_uint;

/*
  Type: xil_axi4stream_ulong 
   Xilinx AXI4STREAM longint unsigned data type
   |typedef longint unsigned  xil_axi4stream_ulong;
*/
typedef longint unsigned  xil_axi4stream_ulong;

/*
  Type: xil_axi4stream_long
   Xilinx AXI4STREAM longint data type
   |typedef longint           xil_axi4stream_long;
*/
typedef longint           xil_axi4stream_long;

/*
  Type: xil_axi4stream_data_byte
   Xilinx AXI4STREAM data byte
   |typedef logic [7:0]       xil_axi4stream_data_byte;
*/
typedef logic [7:0]       xil_axi4stream_data_byte;

/*
  Type: xil_axi4stream_strb
   Xilinx AXI4STREAM strobe bit
   |typedef bit               xil_axi4stream_strb;
*/
typedef bit               xil_axi4stream_strb;

parameter XIL_AXI4STREAM_MAX_DATA_WIDTH               = 4096;
parameter XIL_AXI4STREAM_MAX_ID_WIDTH                 = 32;
parameter XIL_AXI4STREAM_USER_BEAT_WIDTH              = 4096;
parameter XIL_AXI4STREAM_USER_ELEMENT_WIDTH           = 32;
parameter XIL_AXI4STREAM_VERBOSITY_NONE               = 0;
parameter XIL_AXI4STREAM_VERBOSITY_FULL               = 400;
typedef logic [XIL_AXI4STREAM_USER_BEAT_WIDTH-1:0]    xil_axi4stream_user_beat;
typedef logic [XIL_AXI4STREAM_MAX_DATA_WIDTH-1:0]     xil_axi4stream_data_beat;
typedef bit   [XIL_AXI4STREAM_MAX_DATA_WIDTH/8-1:0]   xil_axi4stream_strb_beat;
typedef xil_axi4stream_uint                           xil_axi4stream_user_element;

/*
  Type: xil_axi4stream_driver_return_policy_t

  Policies of the transaction to inform the driver whether to return the transaction back to the issuer
  Depending on the policy the driver will return the transaction a different points in the life of the 
  transaction.

  Enum: xil_axi4stream_driver_return_policy_t
    XIL_AXI4STREAM_NO_RETURN        - Do not return the transaction back to the user environment
    XIL_AXI4STREAM_AT_ASSERT_RETURN - Return the transaction back to the user environment after
                                      it TVALID is asserted, but before it is accepted.
    XIL_AXI4STREAM_AT_ACCEPT_RETURN - Return the transaction back to the user environment
                                      when it has been accepted by the slave.
*/

typedef enum {
  XIL_AXI4STREAM_NO_RETURN,
  XIL_AXI4STREAM_AT_ASSERT_RETURN,
  XIL_AXI4STREAM_AT_ACCEPT_RETURN
  } xil_axi4stream_driver_return_policy_t;

/*
  Type: xil_axi4stream_ready_gen_policy_t

  Collection of policies that describe how the xREADY signals will behave. These policies can 
  introduce backpressure into the system to find design faults.

  
  Enum: xil_axi4stream_ready_gen_policy_t

    XIL_AXI4STREAM_READY_GEN_NO_BACKPRESSURE   - Ready stays asserted and will not change. The driver
                                                 will still check for policy changes.
    XIL_AXI4STREAM_READY_GEN_SINGLE            - Ready stays 0 for low_time clock cycles and then
                                                 drives 1 until one ready/valid handshake occurs,
                                                 the policy repeats until the channel is given
                                                 different policy.
   XIL_AXI4STREAM_READY_GEN_EVENTS             - Ready stays 0 for low_time clock cycles and then
                                                 drives 1 until event_count ready/valid handshakes
                                                 occur,the policy repeats until the channel is given
                                                 different policy.
   XIL_AXI4STREAM_READY_GEN_OSC                - Ready stays 0 for low_time and then goes to 1 and      
                                                 stays 1 for high_time,the policy repeats until the
                                                 channel is given different policy.
   XIL_AXI4STREAM_READY_GEN_RANDOM             - This policy generate random ready policy and uses
                                                 min/max pair of low_time, high_time and event_count to
                                                 generate low_time, high_time and event_count.
   XIL_AXI4STREAM_READY_GEN_AFTER_VALID_SINGLE - This policy is active when VALID is detected to be
                                                 asserted, Ready stays 0 for low_time clock cycles and
                                                 then drives 1 until one ready/valid handshake occurs,
                                                 the policy repeats until the channel is given
                                                 different policy.
   XIL_AXI4STREAM_READY_GEN_AFTER_VALID_EVENTS - This policy is active when VALID is detected to be
                                                 asserted, Ready stays 0 for low_time clock cycles and
                                                 then drives 1 until event_count ready/valid handshake
                                                 occurs,the policy repeats until the channel is given
                                                 different policy.
   XIL_AXI4STREAM_READY_GEN_AFTER_VALID_OSC    - This policy is active when VALID is detected to be
                                                 asserted, Ready stays 0 for low_time and then goes to
                                                1 and  stays 1 for high_time,the policy repeats until
                                                the channel is given different policy.
*/

typedef enum {
  XIL_AXI4STREAM_READY_GEN_NO_BACKPRESSURE,
  XIL_AXI4STREAM_READY_GEN_SINGLE,
  XIL_AXI4STREAM_READY_GEN_EVENTS,
  XIL_AXI4STREAM_READY_GEN_OSC,
  XIL_AXI4STREAM_READY_GEN_RANDOM,
  XIL_AXI4STREAM_READY_GEN_AFTER_VALID_SINGLE,
  XIL_AXI4STREAM_READY_GEN_AFTER_VALID_EVENTS,
  XIL_AXI4STREAM_READY_GEN_AFTER_VALID_OSC
  } xil_axi4stream_ready_gen_policy_t;

/*  
  Type: xil_axi4stream_ready_rand_policy_t

  Collection of policies that describe how the xREADY signals will behave when xil_axi4stream_ready_gen_policy is XIL_AXI4STREAM_READY_GEN_RANDOM. 


  Enum:  xil_axi4stream_ready_rand_policy_t
     XIL_AXI4STREAM_READY_RAND_SINGLE             - Ready stays 0 for low_time clock cycles and then
                                                  drives 1 until one ready/valid handshake occurs,
                                                  the policy repeats until the channel is given
                                                  different policy.
     XIL_AXI4STREAM_READY_RAND_EVENTS             - Ready stays 0 for low_time clock cycles and then
                                                  drives 1 until event_count ready/valid handshakes
                                                  occur,the policy repeats until the channel is given
                                                  different policy.
     XIL_AXI4STREAM_READY_RAND_OSC                - Ready stays 0 for low_time and then goes to 1 and      
                                                  stays 1 for high_time,the policy repeats until the
                                                  channel is given different policy.
     XIL_AXI4STREAM_READY_RAND_AFTER_VALID_SINGLE - This policy is active when VALID is detected to be
                                                  asserted, Ready stays 0 for low_time clock cycles and
                                                  then drives 1 until one ready/valid handshake occurs,
                                                  the policy repeats until the channel is given
                                                  different policy.
     XIL_AXI4STREAM_READY_RAND_AFTER_VALID_EVENTS - This policy is active when VALID is detected to be
                                                  asserted, Ready stays 0 for low_time clock cycles and
                                                  then drives 1 until event_count ready/valid handshake
                                                  occurs,the policy repeats until the channel is given
                                                  different policy.
     XIL_AXI4STREAM_READY_RAND_AFTER_VALID_OSC    - This policy is active when VALID is detected to be
                                                  asserted, Ready stays 0 for low_time and then goes to
                                                  1 and  stays 1 for high_time,the policy repeats until
                                                  the channel is given different policy.
*/
typedef enum {
  XIL_AXI4STREAM_READY_RAND_SINGLE,
  XIL_AXI4STREAM_READY_RAND_OSC,
  XIL_AXI4STREAM_READY_RAND_EVENTS,
  XIL_AXI4STREAM_READY_RAND_AFTER_VALID_SINGLE,
  XIL_AXI4STREAM_READY_RAND_AFTER_VALID_OSC,
  XIL_AXI4STREAM_READY_RAND_AFTER_VALID_EVENTS
} xil_axi4stream_ready_rand_policy_t;

/*
  Type: xil_axi4stream_boolean_t
  AXI4STREAM Boolean data type

  Enum: xil_axi4stream_boolean_t

  XIL_AXI4STREAM_TRUE  - Boolean TRUE
  XIL_AXI4STREAM_FALSE - Boolean FALSE
*/
typedef enum bit {
  XIL_AXI4STREAM_TRUE  = 1,
  XIL_AXI4STREAM_FALSE = 0
  } xil_axi4stream_boolean_t;

/*
  Type: xil_axi4stream_vif_dummy_drive_t
  When VIF driver is in idle mode, possible signals that can be driven on VIF

  Enum: xil_axi4stream_vif_dummy_drive_t

  XIL_AXI4STREAM_VIF_DRIVE_NONE  - VIF drive zero in dummy mode
  XIL_AXI4STREAM_VIF_DRIVE_X     - VIF drive X in dummy mode
  XIL_AXI4STREAM_VIF_DRIVE_NOISE - VIF drive noise in dummy mode
  XIL_AXI4STREAM_VIF_DRIVE_Z     - VIF drive Z in dummy mode
*/

typedef enum {
  XIL_AXI4STREAM_VIF_DRIVE_NONE,
  XIL_AXI4STREAM_VIF_DRIVE_X,
  XIL_AXI4STREAM_VIF_DRIVE_NOISE
  } xil_axi4stream_vif_dummy_drive_t;

/*
  Type: xil_axi4stream_xfer_alignment_t
  Transaction types which can be all valid byte, all null bytes, all position bytes or a combination  of these three 

  Enum: xil_axi4stream_xfer_alignment_t

  XIL_AXI4STREAM_XFER_ALL_SET       -  All bytes are valid bytes
  XIL_AXI4STREAM_XFER_ALL_NULLBYTE  -  All bytes are null Bytes
  XIL_AXI4STREAM_XFER_ALL_POSBYTE   -  All bytes are position bytes
  XIL_AXI4STREAM_XFER_RANDOM        -  Bytes are combination of valid, null and position bytes 
*/
typedef enum {
  XIL_AXI4STREAM_XFER_ALL_SET,
  XIL_AXI4STREAM_XFER_ALL_NULLBYTE,
  XIL_AXI4STREAM_XFER_ALL_POSBYTE,
  XIL_AXI4STREAM_XFER_RANDOM
  } xil_axi4stream_xfer_alignment_t;

/*
  Enum: xil_axi4stream_xfer_delay_insertion_policy_t

  XIL_AXI4STREAM_DELAY_INSERTION_ALWAYS          - Always insert data delay
   XIL_AXI4STREAM_DELAY_INSERTION_FROM_IDLE      - Insert data delay only from idle
*/
typedef enum {
  XIL_AXI4STREAM_DELAY_INSERTION_ALWAYS,
  XIL_AXI4STREAM_DELAY_INSERTION_FROM_IDLE
  } xil_axi4stream_xfer_delay_insertion_policy_t;

/*
  Enum: xil_axi4stream_trans_state_t
  XIL_AXI4STREAM_TRANS_STATE_NEW               - Transaction state is new
  XIL_AXI4STREAM_TRANS_STATE_ACTIVE            - Transaction state is active
  XIL_AXI4STREAM_TRANS_STATE_COMPLETED         - Transaction state is completed
  XIL_AXI4STREAM_TRANS_STATE_KILLED            - Transaction state is killed
  XIL_AXI4STREAM_TRANS_STATE_COMPLETED_SLAVE   - Transaction state is slave completed
  XIL_AXI4STREAM_TRANS_STATE_COMPLETED_MASTER  - Transaction state is master completed 
*/
typedef enum {
  XIL_AXI4STREAM_TRANS_STATE_NEW,
  XIL_AXI4STREAM_TRANS_STATE_ACTIVE,
  XIL_AXI4STREAM_TRANS_STATE_COMPLETED,
  XIL_AXI4STREAM_TRANS_STATE_KILLED,
  XIL_AXI4STREAM_TRANS_STATE_COMPLETED_SLAVE,
  XIL_AXI4STREAM_TRANS_STATE_COMPLETED_MASTER
  } xil_axi4stream_trans_state_t;

/*
  Type:xil_axi4stream_sigset_t 
   Xilinx AXI4STREAM sigset data type
   |typedef bit[7:0] xil_axi4stream_sigset_t;
*/
typedef bit[7:0] xil_axi4stream_sigset_t;


/*
  Enum: xil_axi4stream_tkeep_sparse_pattern_t
  XIL_AXI4STREAM_TKEEP_NO_TOGGLE            - TKEEP must be either all 0's or all 1's, no toggle of 1 and 0.
  XIL_AXI4STREAM_TKEEP_ALLOW_HEAD_ZERO      - consective 0's at the head of TKEEP, other bits are 1's. it also includes XIL_AXI4STREAM_TKEEP_NO_TOGGLE 
  XIL_AXI4STREAM_TKEEP_ALLOW_TAIL_ZERO      - consective 0's at the tail of TKEEP, other bits are 1's. it also includes XIL_AXI4STREAM_TKEEP_NO_TOGGLE   
  XIL_AXI4STREAM_TKEEP_ALLOW_HEAD_TAIL_ZERO - consective 0's at the head or/and consective 0's at the tail of TKEEP,other bits are 1's. it also includes XIL_AXI4STREAM_TKEEP_NO_TOGGLE
*/
typedef enum {
  XIL_AXI4STREAM_TKEEP_NO_NULL_BYTES,
  XIL_AXI4STREAM_TKEEP_ALLOW_HEAD_ZERO, 
  XIL_AXI4STREAM_TKEEP_ALLOW_TAIL_ZERO, 
  XIL_AXI4STREAM_TKEEP_ALLOW_HEAD_TAIL_ZERO
  } xil_axi4stream_tkeep_sparse_pattern_t;

// Bit offset constants for the AXIS Signal Set
parameter XIL_AXI4STREAM_SIGSET_POS_READY     = 0;
parameter XIL_AXI4STREAM_SIGSET_POS_DATA      = 1;
parameter XIL_AXI4STREAM_SIGSET_POS_STRB      = 2;
parameter XIL_AXI4STREAM_SIGSET_POS_KEEP      = 3;
parameter XIL_AXI4STREAM_SIGSET_POS_LAST      = 4;
parameter XIL_AXI4STREAM_SIGSET_POS_ID        = 5;
parameter XIL_AXI4STREAM_SIGSET_POS_DEST      = 6;
parameter XIL_AXI4STREAM_SIGSET_POS_USER      = 7;

// Field enable constants for AXIS Signal Set
parameter XIL_AXI4STREAM_SIGSET_READY     = 8'b00000001;
parameter XIL_AXI4STREAM_SIGSET_DATA      = 8'b00000010;
parameter XIL_AXI4STREAM_SIGSET_STRB      = 8'b00000100;
parameter XIL_AXI4STREAM_SIGSET_KEEP      = 8'b00001000;
parameter XIL_AXI4STREAM_SIGSET_LAST      = 8'b00010000;
parameter XIL_AXI4STREAM_SIGSET_ID        = 8'b00100000;
parameter XIL_AXI4STREAM_SIGSET_DEST      = 8'b01000000;
parameter XIL_AXI4STREAM_SIGSET_USER      = 8'b10000000;
parameter XIL_AXI4STREAM_SIGSET_ALL       = 8'b11111111;

/*
  Class: axi4stream_transaction
  AXI4STREAM Transaction object.
*/
class axi4stream_transaction extends xil_sequence_item;
  static xil_axi4stream_uint                              s_cmd_id = 0;
  xil_axi4stream_uint                                     cmd_id = 0;

  ///////////////////////////////////////////////////////////////////////////
  //Payload
  rand xil_axi4stream_uint                                 dest = 'hdeadbeef;
  rand xil_axi4stream_uint                                 id = 'h12345678;
  protected rand xil_axi4stream_data_byte                  data[];
  protected rand xil_axi4stream_uint                       user[];
  protected rand xil_axi4stream_strb                       strb[];
  protected rand xil_axi4stream_strb                       keep[];
  rand bit                                                 last = 1;

  ///////////////////////////////////////////////////////////////////////////
  //Configurable signals and widths.
  protected xil_axi4stream_sigset_t                        signal_set = XIL_AXI4STREAM_SIGSET_READY | XIL_AXI4STREAM_SIGSET_DATA;
  protected xil_axi4stream_uint                            dest_width = 4;
  protected xil_axi4stream_uint                            id_width = 2;
  protected xil_axi4stream_uint                            data_width = 32;
  protected xil_axi4stream_uint                            user_width = 32;
  protected xil_axi4stream_uint                            user_bits_per_byte = 8;

  ///////////////////////////////////////////////////////////////////////////
  //State information
  protected xil_axi4stream_driver_return_policy_t          driver_return_item = XIL_AXI4STREAM_NO_RETURN;

  ///////////////////////////////////////////////////////////////////////////
  // Delays
  rand xil_axi4stream_uint                                  delay = 20;
  protected xil_axi4stream_xfer_delay_insertion_policy_t    delay_policy = XIL_AXI4STREAM_DELAY_INSERTION_ALWAYS;

  ///////////////////////////////////////////////////////////////////////////
  //Delay ranges for constraints
  protected xil_axi4stream_uint                            min_delay = 0;
  protected xil_axi4stream_uint                            max_delay = 20;
  protected xil_axi4stream_xfer_alignment_t                xfer_alignment = XIL_AXI4STREAM_XFER_ALL_SET;

  ///////////////////////////////////////////////////////////////////////////
  // constructor ////////////////////////////////////////////////////////////
  /*
    Function: new
    Constructor to create a new transaction.
  */
  function new(input string name="unnamed_axi4stream_transaction",
                     xil_axi4stream_sigset_t set      = XIL_AXI4STREAM_SIGSET_READY | XIL_AXI4STREAM_SIGSET_DATA,
                     xil_axi4stream_uint  dw          = 32,
                     xil_axi4stream_uint  uw          = 32,
                     xil_axi4stream_uint  bpb         = 8,
                     xil_axi4stream_uint  idw         = 2,
                     xil_axi4stream_uint  destw       = 4
  );
    super.new(name);
    cmd_id = s_cmd_id;
    s_cmd_id++;
    this.set_signal_set(set);
    this.set_data_width(dw);
    this.set_id_width(idw);
    this.set_dest_width(destw);
    this.set_user_width(uw);
    this.set_user_bits_per_byte(bpb);
    this.resize_payload_arrays();
  endfunction : new

  /*
    Function: copy
    Copies the contents of the input transaction to the current transaction
  */
  function void copy(axi4stream_transaction rhs);
    this.set_signal_set(rhs.get_signal_set());
    this.set_data_width(rhs.get_data_width());
    this.set_id_width(rhs.get_id_width());
    this.set_dest_width(rhs.get_dest_width());
    this.set_user_width(rhs.get_user_width());
    this.set_user_bits_per_byte(rhs.get_user_bits_per_byte());
    this.resize_payload_arrays();
    if (this.signal_set[XIL_AXI4STREAM_SIGSET_POS_DATA] == 1) begin
      this.set_data_beat(rhs.get_data_beat());
    end  
    if (this.signal_set[XIL_AXI4STREAM_SIGSET_POS_USER] == 1) begin
      this.set_user_beat(rhs.get_user_beat());
    end  
    if (this.signal_set[XIL_AXI4STREAM_SIGSET_POS_STRB] == 1) begin
      this.set_strb_beat(rhs.get_strb_beat());
    end
    if (this.signal_set[XIL_AXI4STREAM_SIGSET_POS_KEEP] == 1) begin
      this.set_keep_beat(rhs.get_keep_beat());
    end
    if (this.signal_set[XIL_AXI4STREAM_SIGSET_POS_ID] == 1) begin
      this.set_id(rhs.get_id());
    end
    if (this.signal_set[XIL_AXI4STREAM_SIGSET_POS_DEST] == 1) begin
      this.set_dest(rhs.get_dest());
    end 
    if (this.signal_set[XIL_AXI4STREAM_SIGSET_POS_LAST] == 1) begin
      this.set_last(rhs.get_last());
    end
    this.set_driver_return_item_policy(rhs.get_driver_return_item_policy());
    this.set_delay(rhs.get_delay());
    this.set_xfer_alignment(rhs.get_xfer_alignment());
    rhs.get_delay_range(this.min_delay, this.max_delay);
    this.set_delay_policy(rhs.get_delay_policy());
  endfunction : copy

  virtual function string get_type_name();
    return "XIL_AXI4STREAM_TRANSACTION";
  endfunction : get_type_name

  /*
    Function: my_clone
    Returns a clones of the contents of the transaction.
  */
  virtual function axi4stream_transaction my_clone ();
    axi4stream_transaction           my_obj;
    my_obj = new( this.get_name(),
                  this.get_signal_set(),
                  this.get_data_width(),
                  this.get_user_width(),
                  this.get_user_bits_per_byte(),
                  this.get_id_width(),
                  this.get_dest_width());

    my_obj.set_id_info(this);
    my_obj.copy(this);
    return(my_obj);
  endfunction

  //////////////////////////////////////////////////////////////////////////
  // Trans field accessors
  /*
    Function: get_id
    Returns the value of TID of the transaction.
  */
  virtual function xil_axi4stream_uint get_id ();
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_ID) == 1) begin
      return(this.id);
    end else begin
      return(0);
    end
  endfunction : get_id

  /*
    Function: set_id
    Sets the value of TID of the transaction.
  */
  virtual function void set_id (input xil_axi4stream_uint new_id);
    xil_axi4stream_uint mask;
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_ID) == 1) begin
      mask = (1 << this.get_id_width())-1;
      if ((new_id & ~mask) > 0) begin
        `xil_warning(get_name(), $sformatf("Attempted to send ID (0x%x) which is wider than id_width (%d). Truncating ID to (0x%0x)", new_id, this.get_id_width(), new_id & mask))
        new_id &= mask;
      end

      this.id = new_id;
    end else begin
      if(new_id !=0) begin
        `xil_error(get_name(), $sformatf("Attempted to set ID (0x%x) when the interface has no ID signals. Nothing set.", new_id))
      end  
    end
  endfunction

  /*
    Function: get_dest
    Returns the value of TDEST of the transaction.
  */
  function xil_axi4stream_uint get_dest ();
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_DEST) == 1) begin
      return(this.dest);
    end else begin
      return(0);
    end
  endfunction : get_dest

  /*
    Function: set_dest
    Sets the value of TDEST of the transaction.
  */
  virtual function void set_dest (input xil_axi4stream_uint new_dest);
    xil_axi4stream_uint mask;
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_DEST) == 1) begin
      mask = (1 << this.get_dest_width())-1;
      if ((new_dest & ~mask) > 0) begin
        `xil_warning(get_name(), $sformatf("Attempted to send DEST (0x%x) which is wider than dest_width (%d). Truncating DEST to (0x%0x)", new_dest, this.get_dest_width(), new_dest & mask))
        new_dest &= mask;
      end
      this.dest = new_dest;
    end else begin
      if(new_dest !=0) begin
        `xil_error(get_name(), $sformatf("Attempted to set DEST (0x%x) when the interface has no DEST signals. Nothing set.", new_dest))
      end  
    end
  endfunction : set_dest

  /*
    Function: get_last
    Returns the value of TLAST of the transaction. This transaction will return 1 when the TLAST signal is not enabled.
  */
  function bit get_last ();
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_LAST) == 1) begin
      return(this.last);
    end else begin
      return(1);
    end
  endfunction : get_last

  /*
    Function: set_last
    Sets the value of TLAST of the transaction.
  */
  virtual function void set_last (input bit new_last);
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_LAST) == 1) begin
      this.last = new_last;
    end else begin
      if(new_last !=1) begin
        `xil_error(get_name(), $sformatf("Attempted to set LAST (0x%x) when the interface has no LAST signal. Nothing set.", new_last))
      end  
    end
  endfunction : set_last

  /*
    Function: get_strb
    Returns the unpacked version of TSTRB of the transaction.
  */
  function void get_strb(inout xil_axi4stream_strb ext_strb [] );
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) begin
      if (ext_strb.size() != this.strb.size()) begin
        `xil_fatal(this.get_name(), $sformatf("Attempt to read STRB into container array with different size. Transactions strb size (%d), container size (%d)", this.strb.size(), ext_strb.size()))
      end

      for (xil_axi4stream_uint i = 0; i < ext_strb.size(); i++) begin
        ext_strb[i] = this.strb[i];
      end
    end else begin
      for (xil_axi4stream_uint i = 0; i < ext_strb.size(); i++) begin
        ext_strb[i] = 0;
      end
    end
  endfunction : get_strb

  /*
    Function: set_strb
    Sets the value of TSTRB of the transaction using an unpacked array.
  */
  virtual function void set_strb (input xil_axi4stream_strb updated []);
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) begin
      if (updated.size() != this.strb.size()) begin
        `xil_fatal(this.get_name(), $sformatf("Attempt to assign STRB arrays with mismatched size. Transaction size (%3d) vs container (%3d).", this.strb.size(), updated.size()))
      end

      for (xil_axi4stream_uint i = 0; i < updated.size(); i++) begin
        this.strb[i] = updated[i];
      end
    end else begin
      `xil_error(get_name(), $sformatf("Attempted to set STRB when the interface has no STRB signals. Nothing set."))
    end
  endfunction : set_strb

  /*
    Function: get_strb_beat
    Returns the beat type version of TSTRB of the transaction.
  */
  function xil_axi4stream_strb_beat get_strb_beat();
    xil_axi4stream_strb_beat tmp;
    tmp = 'h0;
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) begin
      if (this.get_data_width()/8 > this.strb.size()) begin
        `xil_warning(this.get_name(), $sformatf("Transaction's interal strb size (0x%x) is smaller than data_width (0x%x).", this.strb.size(), this.get_data_width()/8))
      end else if (this.get_data_width()/8 < this.strb.size()) begin
        `xil_warning(this.get_name(), $sformatf("Transaction's interal strb size (0x%x) is larger than data_width (0x%x). STRB's truncated", this.strb.size(), this.get_data_width()/8))
      end
      for (xil_axi4stream_uint i = 0; i < this.strb.size(); i++) begin
        tmp[i] = this.strb[i];
      end
    end
    return(tmp);
  endfunction : get_strb_beat

  /*
    Function: set_strb_beat
    Sets the value of TSTRB of the transaction using a beat type.
  */
  virtual function void set_strb_beat (input xil_axi4stream_strb_beat updated);
    xil_axi4stream_strb_beat mask;
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) begin
      mask = (1 << (this.get_data_width()/8))-1;
      if ((updated & ~mask) > 0) begin
        `xil_warning(get_name(), $sformatf("Attempted to send STRB (0x%x) which is wider than data_width/8 (%d). Truncating STRB to (0x%0x)", updated, this.get_data_width()/8, updated & mask))
        updated &= mask;
      end
      this.resize_payload_arrays();

      for (xil_axi4stream_uint i = 0; i < (this.get_data_width()/8); i++) begin
        this.strb[i] = updated[i+:1];
      end
    end else begin
      `xil_error(get_name(), $sformatf("Attempted to set STRB (0x%x) when the interface has no STRB signals. Nothing set.", updated))
    end
  endfunction : set_strb_beat

  /*
    Function: get_keep
    Returns the unpacked version of TKEEP of the transaction.
  */
  function void get_keep(inout xil_axi4stream_strb ext_keep [] );
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1) begin
      if (ext_keep.size() != this.keep.size()) begin
        `xil_fatal(this.get_name(), $sformatf("Attempt to read KEEP into container array with different size. Transaction KEEP size (%3d), container size (%3d)", this.keep.size(), ext_keep.size()))
      end

      for (xil_axi4stream_uint i = 0; i < ext_keep.size(); i++) begin
        ext_keep[i] = this.keep[i];
      end
    end else begin
      for (xil_axi4stream_uint i = 0; i < ext_keep.size(); i++) begin
        ext_keep[i] = 0;
      end
    end
  endfunction : get_keep

  /*
    Function: set_keep
    Sets the value of TKEEP of the transaction using an unpacked array.
  */
  virtual function void set_keep (input xil_axi4stream_strb updated []);
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1) begin
      if (updated.size() != this.keep.size()) begin
        `xil_fatal(this.get_name(), $sformatf("Attempt to assign KEEP arrays with mismatched size. Transaction size (%3d), container size(%3d).", this.keep.size(), updated.size()))
      end

      for (xil_axi4stream_uint i = 0; i < updated.size(); i++) begin
        this.keep[i] = updated[i];
      end
    end else begin
      `xil_error(get_name(), $sformatf("Attempted to set KEEP when the interface has no KEEP signals. Nothing set."))
    end
  endfunction : set_keep

  /*
    Function: get_keep_beat
    Returns the beat type version of TKEEP of the transaction.
  */
  function xil_axi4stream_strb_beat get_keep_beat();
    xil_axi4stream_strb_beat tmp;
    tmp = 'h0;
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1) begin
      if (this.get_data_width()/8 > this.keep.size()) begin
        `xil_warning(this.get_name(), $sformatf("Transaction's interal keep size (0x%x) is smaller than data_width (0x%x).", this.keep.size(), this.get_data_width()/8))
      end else if (this.get_data_width()/8 < this.keep.size()) begin
        `xil_warning(this.get_name(), $sformatf("Transaction's interal keep size (0x%x) is larger than data_width (0x%x). KEEP's truncated", this.keep.size(), this.get_data_width()/8))
      end
      for (xil_axi4stream_uint i = 0; i < this.keep.size(); i++) begin
        tmp[i] = this.keep[i];
      end
    end
    return(tmp);
  endfunction : get_keep_beat

  /*
    Function: set_keep_beat
    Sets the value of TKEEP of the transaction using a beat type.
  */
  virtual function void set_keep_beat (input xil_axi4stream_strb_beat updated);
    xil_axi4stream_strb_beat mask;
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1) begin
      mask = (1 << (this.get_data_width()/8))-1;
      if ((updated & ~mask) > 0) begin
        `xil_warning(get_name(), $sformatf("Attempted to send KEEP (0x%x) which is wider than data_width/8 (%d). Truncating KEEP to (0x%0x)", updated, this.get_data_width()/8, updated & mask))
        updated &= mask;
      end
      this.resize_payload_arrays();

      for (xil_axi4stream_uint i = 0; i < (this.get_data_width()/8); i++) begin
        this.keep[i] = updated[i+:1];
      end
    end else begin
      `xil_error(get_name(), $sformatf("Attempted to set KEEP (0x%x) when the interface has no KEEP signals. Nothing set.", updated))
    end
  endfunction : set_keep_beat

  /*
    Function: get_data
    Returns the unpacked version of TDATA of the transaction.
  */
  virtual function void get_data(inout xil_axi4stream_data_byte ext_data []);
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_DATA) == 1) begin
      if (ext_data.size() != this.data.size()) begin
        `xil_fatal(this.get_name(), $sformatf("Attempt to read DATA into container array with different size. Transaction DATA size (%3d), container size (%3d)", this.data.size(), ext_data.size()))
      end

      for (xil_axi4stream_uint i = 0; i < ext_data.size(); i++) begin
        ext_data[i] = this.data[i];
      end
    end else begin
      for (xil_axi4stream_uint i = 0; i < ext_data.size(); i++) begin
        ext_data[i] = 0;
      end
    end
  endfunction : get_data

  /*
    Function: set_data
    Sets the value of TDATA of the transaction using an unpacked array.
  */
  virtual function void set_data(input xil_axi4stream_data_byte updated[]);
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_DATA) == 1) begin
      if (updated.size() != this.data.size()) begin
        `xil_fatal(this.get_name(), $sformatf("set_data: Attempt to assign DATA arrays with mismatched size. Transaction size: (%3d), container size: (%3d).", this.data.size(), updated.size()))
      end

      for (xil_axi4stream_uint i = 0; i < updated.size(); i++) begin
        this.data[i]=updated[i];
      end
    end else begin
      `xil_error(get_name(), $sformatf("Attempted to set DATA when the interface has no DATA signals. Nothing set."))
    end
  endfunction : set_data


  /*
    Function: get_data_beat
    Returns the beat type version of TDATA of the transaction.
  */
  function xil_axi4stream_data_beat get_data_beat();
    xil_axi4stream_data_beat tmp;
    tmp = 'h0;
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_DATA) == 1) begin
      if (this.get_data_width() > (this.data.size() * 8)) begin
        `xil_warning(this.get_name(), $sformatf("Transaction's interal DATA size (0x%x) is smaller than data_width (0x%x).", (this.data.size() * 8), this.get_data_width()))
      end else if (this.get_data_width() < (this.data.size() * 8)) begin
        `xil_warning(this.get_name(), $sformatf("Transaction's interal DATA size (0x%x) is larger than data_width (0x%x). DATA's truncated", this.data.size(), this.get_data_width()))
      end
      for (xil_axi4stream_uint i = 0; i < this.data.size(); i++) begin
        tmp[i*8+:8] = this.data[i];
      end
    end
    return(tmp);
  endfunction : get_data_beat

  /*
    Function: set_data_beat
    Sets the value of TDATA of the transaction using a beat type.
  */
  function void set_data_beat(input xil_axi4stream_data_beat updated);
    xil_axi4stream_data_beat mask;
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_DATA) == 1) begin
      mask = (1 << (this.get_data_width()))-1;
      if ((updated & ~mask) > 0) begin
        `xil_warning(get_name(), $sformatf("Attempted to send DATA (0x%x) which is wider than data_width (%d). Truncating DATA to (0x%0x)", updated, this.get_data_width(), updated & mask))
        updated &= mask;
      end
      this.resize_payload_arrays();
      for (xil_axi4stream_uint i = 0; i < this.data.size(); i++) begin
        this.data[i] = updated[i*8+:8];
      end
    end
  endfunction : set_data_beat

  ///////////////////////////////////////////////////////////////////////////
  //USER bit configuration
  /*
    Function: set_user_beat
    Sets the value of TUSER of the transaction using a beat type.
  */
  virtual function void set_user_beat(input xil_axi4stream_user_beat updated);
    xil_axi4stream_uint         max_elements;
    xil_axi4stream_user_beat    in_masked;
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_USER)) begin
      this.resize_payload_arrays();
      max_elements = (this.get_user_width() + XIL_AXI4STREAM_USER_ELEMENT_WIDTH - 1)/XIL_AXI4STREAM_USER_ELEMENT_WIDTH;
      in_masked = updated & ((1 << this.get_user_width()) - 1);
      for (xil_axi4stream_uint i = 0; i < max_elements; i++) begin
        this.user[i] = in_masked[XIL_AXI4STREAM_USER_ELEMENT_WIDTH*i+:XIL_AXI4STREAM_USER_ELEMENT_WIDTH];
      end
    end
  endfunction : set_user_beat

  /*
    Function: get_user_beat
    Returns the beat type version of TUSER of the transaction.
  */
  virtual function xil_axi4stream_user_beat get_user_beat();
    xil_axi4stream_uint         max_elements;
    xil_axi4stream_user_beat    tmp;
    tmp = 'h0;
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_USER)) begin
      max_elements = (this.get_user_width() + XIL_AXI4STREAM_USER_ELEMENT_WIDTH - 1)/XIL_AXI4STREAM_USER_ELEMENT_WIDTH;
      for (xil_axi4stream_uint i = 0; i < max_elements; i++) begin
        tmp[i*XIL_AXI4STREAM_USER_ELEMENT_WIDTH+:XIL_AXI4STREAM_USER_ELEMENT_WIDTH] = this.user[i];
      end
    end
    return(tmp);
  endfunction : get_user_beat

  //////////////////////////////////////////////////////////////////////////
  // Width / signalset accessors
  /*
    Function: get_signal_set
    Returns the current settings of the interface.
  */
  virtual function xil_axi4stream_sigset_t get_signal_set();
    return(this.signal_set);
  endfunction

  /*
    Function: set_signal_set
    Sets the value of signal set. This value MUST match the interface properties.
  */
  virtual function void set_signal_set(input xil_axi4stream_sigset_t updated);
    //////////////////////////////////////////////////////////////////////////
    //When updating the signal_set values should be zeroed out
    if (updated[XIL_AXI4STREAM_SIGSET_POS_DATA ] == 0) begin
      this.data.delete();
    end
    if (updated[XIL_AXI4STREAM_SIGSET_POS_STRB ] == 0) begin
      this.strb.delete();
    end
    if (updated[XIL_AXI4STREAM_SIGSET_POS_KEEP ] == 0) begin
      this.keep.delete();
    end
    if (updated[XIL_AXI4STREAM_SIGSET_POS_LAST ] == 0) begin
      this.set_last(1);
    end
    if (updated[XIL_AXI4STREAM_SIGSET_POS_ID   ] == 0) begin
      this.set_id(0);
    end
    if (updated[XIL_AXI4STREAM_SIGSET_POS_DEST ] == 0) begin
      this.set_dest(0);
    end
    if (updated[XIL_AXI4STREAM_SIGSET_POS_USER ] == 0) begin
      this.user.delete();
    end
    this.signal_set = updated;
  endfunction

  protected virtual function bit is_signal_set_pos_set(input xil_axi4stream_uint pos);
    return(this.signal_set[pos]);
  endfunction : is_signal_set_pos_set

  /*
    Function: get_data_width
    Returns the DATA width of the transaction.
  */
  virtual function xil_axi4stream_uint get_data_width();
    return(this.data_width);
  endfunction

  /*
    Function: resize_payload_arrays
    Creates/Adjusts the different internal data structures.
  */
  virtual function void resize_payload_arrays();
    xil_axi4stream_uint         max_elements;
    // (Re-)size the data byte array
    if ((this.data.size() == 0) || (this.data.size() != (this.data_width/8))) begin
      this.data = new[this.data_width/8](this.data);
    end

    if ((this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) && ((this.strb.size() == 0) || (this.strb.size() != this.data_width/8))) begin
      this.strb = new[this.data_width/8](this.strb);
    end

    if ((this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1) && ((this.keep.size() == 0) || (this.keep.size() != this.data_width/8))) begin
      this.keep = new[this.data_width/8](this.keep);
    end

    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_USER) == 1) begin
      if ((this.get_user_bits_per_byte() > 0) && (this.get_user_width() != (this.get_user_bits_per_byte() * (this.get_data_width/8)))) begin
        `xil_fatal(this.get_name(), $sformatf("Attempt to resize USER field (incorrectly sized). TUSER_WIDTH: (%d) < Bits-per-byte (%d) * Interface width in Bytes (%d)", this.get_user_width(), this.get_user_bits_per_byte(), (this.get_data_width/8)))
      end
      max_elements = (this.get_user_width() + XIL_AXI4STREAM_USER_ELEMENT_WIDTH - 1)/XIL_AXI4STREAM_USER_ELEMENT_WIDTH;
      if ((this.user.size() == 0) || (this.user.size() != max_elements)) begin
        this.user = new[max_elements] (this.user);
      end
    end
  endfunction : resize_payload_arrays

  /*
    Function: set_data_width
    Sets the value of DATA width for the transaction.
  */
  virtual function void set_data_width(input xil_axi4stream_uint         updated);
    xil_axi4stream_data_byte new_data[];

    if (updated % 8) begin
      `xil_fatal(this.get_name(), $sformatf("Can't assign data width (%d) to a non-integer multiple of 8", updated))
    end
    this.data_width = updated;
  endfunction

  /*
    Function: get_dest_width
    Returns the DEST width of the transaction.
  */
  virtual function xil_axi4stream_uint get_dest_width();
    get_dest_width = this.dest_width;
  endfunction

  /*
    Function: set_dest_width
    Sets the value of DEST width for the transaction.
  */
  virtual function void set_dest_width(input xil_axi4stream_uint         updated);
    if (updated > XIL_AXI4STREAM_MAX_ID_WIDTH) begin
      `xil_fatal(this.get_name(), $sformatf("Attempted to set DEST width (0x%0x) greater than AXI4STREAM specification", updated))
    end
    this.dest_width = updated;
  endfunction : set_dest_width

  /*
    Function: get_id_width
    Returns the ID width of the transaction.
  */
  virtual function xil_axi4stream_uint get_id_width();
    return(this.id_width);
  endfunction : get_id_width

  /*
    Function: set_id_width
    Sets the value of ID width for the transaction.
  */
  virtual function void set_id_width(input xil_axi4stream_uint         updated);
    if (updated > XIL_AXI4STREAM_MAX_ID_WIDTH) begin
      `xil_fatal(this.get_name(), $sformatf("Attempted to set ID width (0x%0x) greater than AXI4STREAM specification (0x%x)", updated,XIL_AXI4STREAM_MAX_ID_WIDTH))
    end
    this.id_width = updated;
  endfunction : set_id_width

  /*
    Function: get_user_width
    Returns the USER width of the transaction.
  */
  virtual function xil_axi4stream_uint get_user_width();
    return(this.user_width);
  endfunction : get_user_width

  /*
    Function: set_user_width
    Sets the value of USER width for the transaction.
  */
  virtual function void set_user_width(input xil_axi4stream_uint         updated);
    if (updated > XIL_AXI4STREAM_MAX_DATA_WIDTH) begin
      `xil_fatal(this.get_name(), $sformatf("Attempted to set USER width (0x%0x) greater than AXI4STREAM specification", updated))
    end
    this.user_width = updated;
  endfunction : set_user_width

  /*
    Function: set_user_bits_per_byte
    Sets the value of the user bits per byte for the transaction.
  */
  virtual function void set_user_bits_per_byte(input xil_axi4stream_uint         updated);
    if (this.user_width % updated) begin
      `xil_fatal(this.get_name(), $sformatf("Attempt to set user bits-per-byte value (%2d) to non-integer divisor of current user_width (%2d).", this.user_width, updated))
    end

    this.user_bits_per_byte = updated;
  endfunction : set_user_bits_per_byte

  /*
    Function: get_user_bits_per_byte
    Returns the value of the user bits per byte of the transaction.
  */
  virtual function xil_axi4stream_uint get_user_bits_per_byte();
    return(this.user_bits_per_byte);
  endfunction : get_user_bits_per_byte

  /*
    Function: set_driver_return_item
    Sets the driver_return_item property to XIL_AXI4STREAM_AT_ACCEPT_RETURN. 
  */
  virtual function void set_driver_return_item ();
    this.driver_return_item = XIL_AXI4STREAM_AT_ACCEPT_RETURN;
  endfunction

  /*
    Function: set_driver_return_item_policy
    Sets the driver_return_item property of the transaction.
  */
  virtual function void set_driver_return_item_policy (input xil_axi4stream_driver_return_policy_t set);
    this.driver_return_item = set;
  endfunction

  /*
    Function: get_driver_return_item_policy
    Returns the value of driver_return_item property of the transaction.
  */
  virtual function xil_axi4stream_driver_return_policy_t get_driver_return_item_policy ();
    return(this.driver_return_item);
  endfunction

  /*
   Function: clr_driver_return_item 
   Set driver return item policy to be XIL_AXI4STREAM_NO_RETURN
  */
  virtual function void clr_driver_return_item ();
    this.driver_return_item = XIL_AXI4STREAM_NO_RETURN;
  endfunction

  /*
   Function: get_cmd_id
   Returns cmd_id of the transaction
  */
  virtual function xil_axi4stream_uint get_cmd_id();
    return(this.cmd_id);
  endfunction

  /*
   Function:  set_xfer_alignment
   Sets xfer_alignment of the transaction
  */
  virtual function void set_xfer_alignment (input xil_axi4stream_xfer_alignment_t set);
    this.xfer_alignment = set;
  endfunction

  /*
   Function:  get_xfer_alignment
   Returns xfer_alignment of the transaction
  */
  virtual function xil_axi4stream_xfer_alignment_t get_xfer_alignment ();
    return(this.xfer_alignment);
  endfunction

  //////////////////////////////////////////////////////////////////////////
  //
  protected function string get_user_format(
    input xil_axi4stream_uint  width
  );
    return($sformatf("%%%0dx", (width+3)/4));
  endfunction : get_user_format

  /*
   Function:  convert2string
   Returns a form of string for axi4stream transaction  
  */
  virtual function string convert2string();
    string sdata;
    string sstrb;
    string sresp;
    string sout;
    string ret;
    xil_axi4stream_uint         number_bytes;
    xil_axi4stream_user_beat    ubeat = 'h0;
    xil_axi4stream_user_beat    umask = 'h0;

    ret = $sformatf(     "-------------------------------------------------------");
    ret = $sformatf("%s\nNAME                               VALUE", ret);
    ret = $sformatf("%s\n-------------------------------------------------------", ret);
    ret = $sformatf("%s\nXIL_AXI4STREAM_TRANSACTION", ret);
    ret = $sformatf("%s\n  SIGNAL_SET                     0b%b", ret, this.signal_set);

    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_DEST) == 1) begin
      if (this.get_dest_width() > 0) begin
        ret = $sformatf("%s\n  DEST                           0x%x", ret, this.dest);
      end
    end

    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_ID) == 1) begin
      if (this.get_id_width() > 0) begin
        ret = $sformatf("%s\n  ID                            0x%x", ret, this.id);
      end
    end

    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_USER) == 1) begin
      if (this.get_user_bits_per_byte() == 0 && this.get_user_width() > 0) begin
        ret = $sformatf("%s\n  USER                         0x%x", ret, this.get_user_beat());
      end
    end

    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_LAST) == 1) begin
      ret = $sformatf("%s\n  LAST                           0x%x", ret, this.last);
    end

    if (data_width > 0) begin
      ret = $sformatf("%s\n  PAYLOAD                        %0d", ret, data.size());
      if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_USER) == 1) begin
        ubeat = this.get_user_beat();
        if (this.get_user_bits_per_byte() > 0) begin
          umask = (1 << this.get_user_bits_per_byte()) - 1;
        end
      end
      for (xil_axi4stream_uint i=0; i < data.size(); i++) begin
        if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_DATA) == 1) begin
          sdata = $sformatf ("                data: %x ", data[i]);
        end

        if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) begin
          sdata = $sformatf ("%s strb: %b ", sdata, this.strb[i]);
        end

        if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1) begin
          sdata = $sformatf ("%s keep: %b ", sdata, this.keep[i]);
        end

        if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_USER) == 1) begin
          if (this.get_user_bits_per_byte() > 0) begin
            sdata = $sformatf("%s U:(0x%s)", sdata, $sformatf(this.get_user_format(this.get_user_bits_per_byte()), ubeat & umask));
            ubeat = ubeat >> this.get_user_bits_per_byte();
          end
        end
        ret = $sformatf("%s\n    Byte[%3d]:   %s", ret, i, sdata);
      end
    end
    ret = $sformatf("%s\n-------------------------------------------------------", ret);
    return(ret);
  endfunction : convert2string


  /*
   Function:  do_compare
   Compare two transactions based on the signal sets.
  */
  virtual function bit do_compare (
    xil_object  rhs
  );
    bit compare_result;
    axi4stream_transaction      rhs_;
    xil_axi4stream_user_beat    rhsbeat;
    xil_axi4stream_user_beat    lhsbeat;

    compare_result = super.do_compare(rhs);
    $cast(rhs_,rhs);
    ///////////////////////////////////////////////////////////////////////////
    //Compare signalsets
    if (this.get_signal_set() != rhs_.get_signal_set()) begin
      `xil_warning(this.get_name(), $sformatf("SIGNAL_SET: Miscompare: lhs: 0x%x rhs: 0x%x", this.get_signal_set(), rhs_.get_signal_set()))
      compare_result = 0;
    end
    ///////////////////////////////////////////////////////////////////////////
    //For each byte in the payload which is valid.
    for (xil_axi4stream_uint i = 0; i < this.get_data_width()/8; i++) begin
      bit comp_byte = 1;
      if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1) begin
        if (this.keep[i] != rhs_.keep[i]) begin
            `xil_warning(this.get_name(), $sformatf("KEEP: Miscompare: byte %d  lhs: 0x%x rhs: 0x%x", i, this.keep[i], rhs_.keep[i]))
            compare_result = 0;
        end
        comp_byte = this.keep[i];
      end
      if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) begin
        if (comp_byte == 1) begin
          if (this.strb[i] != rhs_.strb[i]) begin
              `xil_warning(this.get_name(), $sformatf("STRB: Miscompare: byte %d  lhs: 0x%x rhs: 0x%x", i, this.strb[i], rhs_.strb[i]))
              compare_result = 0;
          end
          comp_byte = this.strb[i];
        end
      end
      ///////////////////////////////////////////////////////////////////////////
      // No data comparison for POS or NULL bytes...
      if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_DATA) == 1) begin
        ///////////////////////////////////////////////////////////////////////////
        //If there are no KEEP's or STRB's but there is data, compare data
        if (this.data[i] != rhs_.data[i]) begin
          `xil_warning(this.get_name(), $sformatf("DATA: Miscompare: byte %d  lhs: 0x%x rhs: 0x%x", i, this.data[i], rhs_.data[i]))
          compare_result = 0;
        end
      end
    end
    ///////////////////////////////////////////////////////////////////////////
    //Compare USER item
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_USER) == 1) begin
      rhsbeat = rhs_.get_user_beat();
      lhsbeat = this.get_user_beat();
      if (rhs_.get_user_width() != this.get_user_width()) begin
        `xil_warning(this.get_name(), $sformatf("USER WIDTH: Miscompare: lhs: 0x%x rhs: 0x%x", this.get_user_width(), rhs_.get_user_width()))
        compare_result = 0;
      end
      if (lhsbeat != rhsbeat) begin
        `xil_warning(this.get_name(), $sformatf("USER: Miscompare: lhs: 0x%s rhs: 0x%s", 
          $sformatf(this.get_user_format(this.get_user_width()), lhsbeat), 
          $sformatf(this.get_user_format(rhs_.get_user_width()), rhsbeat)
          ))
        compare_result = 0;
      end
    end
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_LAST) == 1) begin
      if (this.last !== rhs_.last) begin
        `xil_warning(this.get_name(), $sformatf("LAST Miscompare: lhs: %b rhs: %b", this.last, rhs_.last))
        compare_result = 0;
      end
    end
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_DEST) == 1) begin
      if (this.dest !== rhs_.dest) begin
        `xil_warning(this.get_name(), $sformatf("DEST Miscompare: lhs: %b rhs: %b", this.dest, rhs_.dest))
        compare_result = 0;
      end
    end
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_ID) == 1) begin
      if (this.id !== rhs_.id) begin
        `xil_warning(this.get_name(), $sformatf("ID Miscompare: lhs: 0x%x rhs: 0x%x", this.id, rhs_.id))
        compare_result = 0;
      end
    end

    if (compare_result == 0) begin
      `xil_warning(this.get_name(), $sformatf("LHS: %s != RHS: %s", this.get_name(), rhs_.get_name()))
    end

    return(compare_result);
  endfunction

  /*
   Function:  get_delay
   Returns delay of the transaction
  */
  virtual function xil_axi4stream_uint get_delay();
    return(this.delay);
  endfunction

  /*
   Function: set_delay
   Sets delay of the transaction
  */
  virtual function void set_delay(input xil_axi4stream_uint updated);
    this.delay = updated;
  endfunction

  /*
   Function:  get_delay_policy
   Returns delay_policy of the transaction
  */
  virtual function xil_axi4stream_xfer_delay_insertion_policy_t get_delay_policy();
    return(this.delay_policy);
  endfunction

  /*
   Function:  set_delay_policy
   Sets delay_policy of the transaction
  */
  virtual function void set_delay_policy(input xil_axi4stream_xfer_delay_insertion_policy_t  updated);
    this.delay_policy = updated;
  endfunction

  ///////////////////////////////////////////////////////////////////////////
  //Ranges
  /*
   Function: get_delay_range
   Returns min_delay and max_delay of the transaction
  */
  function void get_delay_range(output xil_axi4stream_uint         min,
                                output xil_axi4stream_uint         max);
    min = this.min_delay;
    max = this.max_delay;
  endfunction

   /*
   Function: set_delay_range
   Sets min_delay and max_delay of the transaction
  */
  virtual function void set_delay_range(input xil_axi4stream_uint         min,
                                        input xil_axi4stream_uint         max);
    if (min > max) begin
      `xil_fatal(this.get_name(), $sformatf("DELAY: Attempted to set the max (%d) value lower than the min (%d)", max, min))
    end
    this.min_delay = min;
    this.max_delay = max;
  endfunction

  /*
   Function: set_strb_array_all_disabled
   Sets all strobe bits to 0 when HAS_STRB is on
  */
  virtual function void set_strb_array_all_disabled();
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) begin
      for (xil_axi4stream_uint i = 0; i < this.strb.size(); i++) begin
        this.strb[i] = 0;
      end
    end
  endfunction : set_strb_array_all_disabled

   /*
   Function: set_strb_array_all_enabled
   Sets all strobe bits to 1 when HAS_STRB is on
  */
  virtual function void set_strb_array_all_enabled();
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) begin
      for (xil_axi4stream_uint i = 0; i < this.strb.size(); i++) begin
        this.strb[i] = 1;
      end
    end
  endfunction : set_strb_array_all_enabled

  /*
   Function: set_keep_array_all_disabled
   Sets all keep bits to 0 when HAS_KEEP is on
  */
  virtual function void set_keep_array_all_disabled();
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1) begin
      for (xil_axi4stream_uint i = 0; i < this.keep.size(); i++) begin
        this.keep[i] = 0;
      end
    end
  endfunction : set_keep_array_all_disabled

  /*
   Function: set_keep_array_all_enabled
   Sets all keep bits to 1 when HAS_KEEP is on
  */
  virtual function void set_keep_array_all_enabled();
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1) begin
      for (xil_axi4stream_uint i = 0; i < this.keep.size(); i++) begin
        this.keep[i] = 1;
      end
    end
  endfunction : set_keep_array_all_enabled

  ///////////////////////////////////////////////////////////////////////////
  //General constraints
  ///////////////////////////////////////////////////////////////////////////
  //Constrain the size of the arrays based on the data width
  constraint c_array_size {
    data.size() == (data_width / 8);
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) { strb.size() == (data_width / 8);} else {strb.size() == 1;}
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1) { keep.size() == (data_width / 8);} else {keep.size() == 1;}
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_USER) == 1) {
      if (user_bits_per_byte > 0) {
        user.size() == ((user_bits_per_byte * (data_width/8)) + XIL_AXI4STREAM_USER_ELEMENT_WIDTH - 1) / XIL_AXI4STREAM_USER_ELEMENT_WIDTH;
      } else {
        user.size() == (user_width + XIL_AXI4STREAM_USER_ELEMENT_WIDTH - 1) / XIL_AXI4STREAM_USER_ELEMENT_WIDTH;
      }
    } else {
      user.size() == 0;
    }
  }

  constraint c_strb_setting {
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) {
      if (xfer_alignment == XIL_AXI4STREAM_XFER_ALL_SET) {
        foreach (strb[i]) {
          strb[i] == 1;
        }
      } else if (xfer_alignment == XIL_AXI4STREAM_XFER_ALL_POSBYTE ||
                 xfer_alignment == XIL_AXI4STREAM_XFER_ALL_NULLBYTE) {
        foreach (strb[i]) {
          strb[i] == 0;
        }
      }
    }
  }

  constraint c_keep_setting {
    if (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1) {
      if ((xfer_alignment == XIL_AXI4STREAM_XFER_ALL_SET) || (xfer_alignment == XIL_AXI4STREAM_XFER_ALL_POSBYTE)) {
        foreach (keep[i]) {
          keep[i] == 1;
        }
      } else if (xfer_alignment == XIL_AXI4STREAM_XFER_ALL_NULLBYTE) {
        foreach (keep[i]) {
          keep[i] == 0;
        }
      }
    }
  }

  constraint c_delay {
    delay inside {[min_delay:max_delay]};
  }

  constraint c_id_max_value { id inside {[0: (1 << id_width)-1]}; }
  constraint c_dest_max_value { dest inside {[0: (1 << dest_width)-1]}; }

  /*
   Function: post_randomize
   Sets final value of strobe of the transaction
  */
  function void post_randomize();
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //clean up the alignment of strb and keeps
    if ((this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_STRB) == 1) && (this.is_signal_set_pos_set(XIL_AXI4STREAM_SIGSET_POS_KEEP) == 1)) begin
      foreach (keep[i]) begin
        if ((keep[i] == 0) && (strb[i] == 1)) begin
          strb[i] = 0;
        end
      end
    end
   endfunction : post_randomize

endclass : axi4stream_transaction

 /*
  Class: axi4stream_monitor_transaction
  AXI4STREAM Monitor Transaction Object
*/
class axi4stream_monitor_transaction extends axi4stream_transaction;
  time                      ready_assert_time = 0;
  time                      valid_assert_time = 0;
  time                      accepted_time = 0;
  xil_axi4stream_ulong      accepted_cycle = 0;
  xil_axi4stream_ulong      ready_assert_cycle = 0;
  xil_axi4stream_ulong      valid_assert_cycle = 0;
  xil_axi4stream_long       accepted_cycles = 0;
  xil_axi4stream_ulong      backpressure = 0;
  protected xil_axi4stream_boolean_t  monitor_print_times = XIL_AXI4STREAM_FALSE;

  ///////////////////////////////////////////////////////////////////////////
  // constructor
  /*
    Function: new
    Constructor to create a new monitor transaction.
  */
  function new(input string name="axi4stream_monitor_transaction",
                    xil_axi4stream_sigset_t set=XIL_AXI4STREAM_SIGSET_ALL,
                    xil_axi4stream_uint  dw= 32,
                    xil_axi4stream_uint  uw=32,
                    xil_axi4stream_uint  bpb=8,
                    xil_axi4stream_uint  idw=2,
                    xil_axi4stream_uint  destw=4
  );
    super.new(name,
               set,
               dw,
               uw,
               bpb,
               idw,
               destw
             );
  endfunction : new

   /*
    Function:  set_monitor_print_times
    Sets monitor_print_times to be TRUE
  */
  virtual function void set_monitor_print_times();
    monitor_print_times = XIL_AXI4STREAM_TRUE;
  endfunction

  /*
    Function:  clr_monitor_print_times
    Sets monitor_print_times to be FALSE
  */
  virtual function void clr_monitor_print_times();
    monitor_print_times = XIL_AXI4STREAM_FALSE;
  endfunction

  /*
    Function: get_monitor_print_times
    Returns monitor_print_times
  */
  virtual function xil_axi4stream_boolean_t get_monitor_print_times();
    return(monitor_print_times);
  endfunction

  /*
    Function:  copy
    Copies the contents of the input monitor transaction to the current monitor transaction
  */
  function void copy(axi4stream_monitor_transaction rhs);
    super.copy(rhs);
    this.ready_assert_time    = rhs.ready_assert_time ;
    this.valid_assert_time    = rhs.valid_assert_time ;
    this.accepted_time        = rhs.accepted_time     ;
    this.accepted_cycle       = rhs.accepted_cycle    ;
    this.ready_assert_cycle   = rhs.ready_assert_cycle;
    this.valid_assert_cycle   = rhs.valid_assert_cycle;
    this.accepted_cycles      = rhs.accepted_cycles   ;
    this.backpressure         = rhs.backpressure      ;
    this.monitor_print_times  = rhs.monitor_print_times;
  endfunction : copy

  /*
    Function: my_clone
    Clones the current transaction and returns a handle to the new transaction.
  */
  virtual function axi4stream_monitor_transaction my_clone ();
    axi4stream_monitor_transaction           my_obj;
    my_obj = new(this.get_name(),
               this.get_signal_set(),
               this.get_data_width(),
               this.get_user_width(),
               this.get_user_bits_per_byte(),
               this.get_id_width(),
               this.get_dest_width()
     );
    my_obj.set_id_info(this);
    my_obj.copy(this);
    return(my_obj);
  endfunction : my_clone

  /*
    Function:  get_type_name
    Returns string  XIL_AXI4STREAM_MONITOR_TRANSACTION
  */
  virtual function string get_type_name();
    return "XIL_AXI4STREAM_MONITOR_TRANSACTION";
  endfunction : get_type_name

  /*
   Function: convert2string
   Returns a form of string of axi4stream transaction if get_monitor_print_times is false, else
   returns a form of string of ready_assert_time,valid_assert_time,accepted_cycles,backpressure
  */
  virtual function string convert2string();
    string sout;

    ///////////////////////////////////////////////////////////////////////////
    if (this.get_monitor_print_times() != XIL_AXI4STREAM_TRUE) begin
      return(super.convert2string());
    end else begin
      sout = "";
      sout = $sformatf("%s\nready_assert_time   : %t", sout, ready_assert_time);
      sout = $sformatf("%s\nvalid_assert_time   : %t", sout, valid_assert_time);
      sout = $sformatf("%s\nready_assert_cycle  : %d", sout, ready_assert_cycle);
      sout = $sformatf("%s\nvalid_assert_cycle  : %d", sout, valid_assert_cycle);
      sout = $sformatf("%s\naccepted_cycles     : %d", sout, accepted_cycles);
      sout = $sformatf("%s\nbackpressure        : %d", sout, backpressure);
      return(sout);
    end
  endfunction : convert2string


endclass : axi4stream_monitor_transaction

/*
  Class: axi4stream_scoreboard_transaction
  AXI4STREAM Scoreboard Transaction Object
*/
class axi4stream_scoreboard_transaction extends axi4stream_monitor_transaction;

  ///////////////////////////////////////////////////////////////////////////
  // constructor
  /*
    Function: new
    Constructor to create a new scoreboard transaction.
  */
  function new(input string name="axi4stream_monitor_transaction",
                    xil_axi4stream_sigset_t set=XIL_AXI4STREAM_SIGSET_ALL,
                    xil_axi4stream_uint  dw= 32,
                    xil_axi4stream_uint  uw=32,
                    xil_axi4stream_uint  bpb=8,
                    xil_axi4stream_uint  idw=2,
                    xil_axi4stream_uint  destw=4
  );
    super.new(name,
             set,
             dw,
             uw,
             bpb,
             idw,
             destw
             );
  endfunction : new

  /*
   Function: get_type_name
   Returns type name "AXI4STREAM_SCOREBOARD_TRANSACTION"
  */
  virtual function string get_type_name();
    return "AXI4STREAM_SCOREBOARD_TRANSACTION";
  endfunction : get_type_name

endclass : axi4stream_scoreboard_transaction

typedef axi4stream_transaction                                       axi4stream_transaction_t;
typedef axi4stream_monitor_transaction                               axi4stream_monitor_transaction_t;

/*
  Class: axi4stream_vif_proxy
  AXI4STREAM VIF Proxy Object
*/
class axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_DECL extends xil_component;
  ///////////////////////////////////////////////////////////////////////////
  // AXI4STREAM interface -- call assign_vi to connect
  virtual interface axi4stream_vip_if `XIL_AXI4STREAM_PARAM_ORDER m_vif;

  protected int                                   verbosity = XIL_AXI4STREAM_VERBOSITY_NONE;
  protected xil_axi4stream_vif_dummy_drive_t      dummy_drive_type = XIL_AXI4STREAM_VIF_DRIVE_NOISE;
  protected int                                   no_insert_x_when_keep_low = 0;
  time                                            hold_time = 1ps;
  protected xil_axi4stream_ulong                  clk_edge_counter = 0;
  protected time                                  clk_edge_time = 0;

  rand bit [((C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DEST] == 0) ? 1 : (((C_XIL_AXI4STREAM_DEST_WIDTH == 0) ? 1 : C_XIL_AXI4STREAM_DEST_WIDTH)-1))   : 0 ]  xdest;
  rand bit [((C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_ID  ] == 0) ? 1 : (((C_XIL_AXI4STREAM_ID_WIDTH   == 0) ? 1 : C_XIL_AXI4STREAM_ID_WIDTH  )-1))     : 0 ]  xid;
  rand bit [((C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER] == 0) ? 1 : (((C_XIL_AXI4STREAM_USER_WIDTH == 0) ? 1 : C_XIL_AXI4STREAM_USER_WIDTH)-1))   : 0 ]  xuser;
  rand bit [((C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DATA] == 0) ? 1 : (C_XIL_AXI4STREAM_DATA_WIDTH-1))   : 0 ]  xdata;
  rand bit [((C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_STRB] == 0) ? 1 : (C_XIL_AXI4STREAM_DATA_WIDTH/8)-1) : 0 ]  xstrb;
  rand bit [((C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_KEEP] == 0) ? 1 : (C_XIL_AXI4STREAM_DATA_WIDTH/8)-1) : 0 ]  xkeep;
  rand bit xlast;

  /*
   Function:  wait_aclks
   Waits specified amount of posedge aclk occur
  */
  virtual task wait_aclks(input xil_axi4stream_uint cnt);
    if (cnt > 0) begin
      repeat (cnt) wait_posedge_aclk;
    end
  endtask

  /*
   Function: wait_posedge_aclks
   Waits posedge of ACLK of m_vif 
  */
  virtual task wait_posedge_aclk();
    @(m_vif.cb iff (m_vif.cb.ACLKEN == 1));
  endtask

  /*
   Function: wait_posedge_aclk_with_hold
   Waits posedge of ACLK of m_vif and then wait hold_time 
  */
  virtual task wait_posedge_aclk_with_hold();
    @(m_vif.cb iff (m_vif.cb.ACLKEN == 1));
    #hold_time;
  endtask

  /*
   Function: wait_negedge_aclk
   Waits negedge of ACLK of m_vif
  */
  virtual task wait_negedge_aclk();
    @(negedge m_vif.ACLK iff (m_vif.cb.ACLKEN));
  endtask

  /*
   Function: wait_areset_deassert
   Waits areset to be deasserted
  */
  virtual task wait_areset_deassert();
    if (m_vif.ARESET_N == 1'b0) begin
      @(posedge m_vif.cb.ARESET_N);
    end
  endtask

  /*
   Function: get_drive_x
   Returns 1 if dummy_drive_type is XIL_AXI4STREAM_VIF_DRIVE_X, else returns 0
  */
  virtual function xil_axi4stream_boolean_t get_drive_x();
    return((this.dummy_drive_type == XIL_AXI4STREAM_VIF_DRIVE_X) ? XIL_AXI4STREAM_TRUE : XIL_AXI4STREAM_FALSE);
  endfunction

  /*
   Function:  cheap_random
   Generate simplified randomization of xid,xdest and xlast of VIF when user define XIL_DO_NOT_USE_ADV_RANDOMIZATION
  */
  virtual function void cheap_random();
    xid = $urandom();
    xdest = $urandom();
    xlast = bit'($urandom());
  endfunction : cheap_random

  /*
    Function: get_dummy_drive_type
    Get Dummy Drive Type of m_vif
  */
  virtual function xil_axi4stream_vif_dummy_drive_t get_dummy_drive_type();
    xil_axi4stream_vif_dummy_drive_t temp;
    xil_axi4stream_uint              choice;
    if (this.dummy_drive_type == XIL_AXI4STREAM_VIF_DRIVE_NOISE) begin
      `ifndef XIL_DO_NOT_USE_ADV_RANDOMIZATION
        randcase
          60 : temp = XIL_AXI4STREAM_VIF_DRIVE_NOISE;
          40 : temp = XIL_AXI4STREAM_VIF_DRIVE_X;
        endcase
      `else
        choice = $urandom() % 2;
        case(choice)
          0 : temp = XIL_AXI4STREAM_VIF_DRIVE_NOISE;
          1 : temp = XIL_AXI4STREAM_VIF_DRIVE_X;
        endcase
      `endif
      return(temp);
    end else begin
      return(this.dummy_drive_type);
    end
  endfunction

  /*
   Function: wait_valid_asserted(
   Waits TVALID of m_vif asserted
  */
  virtual task wait_valid_asserted();
    wait(m_vif.cb.TVALID && m_vif.cb.ACLKEN);
  endtask
  /*
   Function: new
   Constructor to create a new axi4stream vif proxy
  */
  function new(input string name="unnamed_axi4stream_vif_mem_proxy");
    super.new(name);
  endfunction : new

  /*
    Function: run_phase
    Starts control processes for operation
  */
  task run_phase();
    if (this.get_is_active() == XIL_AXI4STREAM_FALSE) begin
      this.set_is_active();
      forever begin : CLK
        this.wait_posedge_aclk();
        this.clk_edge_counter++;
        this.clk_edge_time = $time;
      end
    end
  endtask : run_phase

  /*
   Function: get_current_clk_count 
   Returns clk_edge_counter
  */
  function xil_axi4stream_ulong get_current_clk_count();
    return(this.clk_edge_counter);
  endfunction : get_current_clk_count

  /*
   Function: get_current_edge_time
   Returns clk_edge_time
  */
  function time get_current_edge_time();
    return(this.clk_edge_time);
  endfunction : get_current_edge_time

  /*
  Function: set_drive_x
  Sets Dummy Drive type to be XIL_AXI4STREAM_VIF_DRIVE_X
  */
  virtual function void set_drive_x();
    this.dummy_drive_type = XIL_AXI4STREAM_VIF_DRIVE_X;
  endfunction : set_drive_x

 /*
  Function: set_dummy_drive_type
  Sets Dummy Drive Type
 */
  virtual function void set_dummy_drive_type(input xil_axi4stream_vif_dummy_drive_t inp);
    this.dummy_drive_type = inp;
  endfunction : set_dummy_drive_type

  /*
   Function: assign_vi
   Assigns m_vif
  */
  function void assign_vi (virtual interface axi4stream_vip_if `XIL_AXI4STREAM_PARAM_ORDER vif) ;
    this.m_vif = vif ;
  endfunction :assign_vi

  /*
   Function: put_noise
   Puts noise on bus
  */
  virtual function void put_noise();
    `ifndef XIL_DO_NOT_USE_ADV_RANDOMIZATION
      VIF_DRIVE_NOISE_RAND_FAILED: assert(this.randomize());
    `else
      this.cheap_random();
    `endif
    case(this.get_dummy_drive_type())
      XIL_AXI4STREAM_VIF_DRIVE_NONE : begin
        m_vif.data <= (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DATA] == 0) ? {C_XIL_AXI4STREAM_DATA_WIDTH   {1'bz}} : {(C_XIL_AXI4STREAM_DATA_WIDTH   ){1'b0}};
        m_vif.strb <= (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_STRB] == 0) ? {C_XIL_AXI4STREAM_DATA_WIDTH/8 {1'bz}} : {(C_XIL_AXI4STREAM_DATA_WIDTH/8 ){1'b0}};
        m_vif.keep <= (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_KEEP] == 0) ? {C_XIL_AXI4STREAM_DATA_WIDTH/8 {1'bz}} : {(C_XIL_AXI4STREAM_DATA_WIDTH/8 ){1'b0}};
        m_vif.dest <= (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DEST] == 0) ? {((C_XIL_AXI4STREAM_DEST_WIDTH == 0) ? 1 : C_XIL_AXI4STREAM_DEST_WIDTH){1'bz}} : {((C_XIL_AXI4STREAM_DEST_WIDTH == 0) ? 1 : C_XIL_AXI4STREAM_DEST_WIDTH){1'b0}};
        m_vif.id   <= (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_ID  ] == 0) ? {((C_XIL_AXI4STREAM_ID_WIDTH   == 0) ? 1 : C_XIL_AXI4STREAM_ID_WIDTH  ){1'bz}} : {((C_XIL_AXI4STREAM_ID_WIDTH   == 0) ? 1 : C_XIL_AXI4STREAM_ID_WIDTH  ){1'b0}};
        m_vif.user <= (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER] == 0) ? {((C_XIL_AXI4STREAM_USER_WIDTH == 0) ? 1 : C_XIL_AXI4STREAM_USER_WIDTH){1'bz}} : {((C_XIL_AXI4STREAM_USER_WIDTH == 0) ? 1 : C_XIL_AXI4STREAM_USER_WIDTH){1'b0}};
        m_vif.last <= (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_LAST] == 0) ? 1'bz : 1'b0;
      end
      XIL_AXI4STREAM_VIF_DRIVE_X : begin
        m_vif.data <= {(C_XIL_AXI4STREAM_DATA_WIDTH   ){1'bx}};
        m_vif.strb <= {(C_XIL_AXI4STREAM_DATA_WIDTH/8 ){1'bx}};
        m_vif.keep <= {(C_XIL_AXI4STREAM_DATA_WIDTH/8 ){1'bx}};
        m_vif.dest <= {((C_XIL_AXI4STREAM_DEST_WIDTH == 0) ? 1 : C_XIL_AXI4STREAM_DEST_WIDTH){1'bx}};
        m_vif.id   <= {((C_XIL_AXI4STREAM_ID_WIDTH   == 0) ? 1 : C_XIL_AXI4STREAM_ID_WIDTH  ){1'bx}};
        m_vif.user <= {((C_XIL_AXI4STREAM_USER_WIDTH == 0) ? 1 : C_XIL_AXI4STREAM_USER_WIDTH){1'bx}};
        m_vif.last <= 1'bx;
      end
      XIL_AXI4STREAM_VIF_DRIVE_NOISE : begin
        m_vif.data <= xdata;
        m_vif.strb <= xstrb;
        m_vif.keep <= xkeep;
        m_vif.dest <= xdest;
        m_vif.id   <= xid;
        m_vif.user <= xuser;
        m_vif.last <= xlast;
      end
    endcase
  endfunction :put_noise

  /*
   Function: reset
   Sets TVALID to be 0 and puts noise onto bus
  */
  virtual function void reset();
    clr_valid();
    put_noise();
  endfunction : reset

  /*
   Function: put_transaction
   Puts transaction information onto bus
  */
  virtual function void put_transaction(input axi4stream_transaction trans);
    xil_axi4stream_uint             user_arr [];

    reg [((C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DATA] == 0) ? 1 : (C_XIL_AXI4STREAM_DATA_WIDTH)-1)   : 0 ]   data;
    reg [((C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_STRB] == 0) ? 1 : (C_XIL_AXI4STREAM_DATA_WIDTH/8)-1) : 0 ]   strb;
    reg [((C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_KEEP] == 0) ? 1 : (C_XIL_AXI4STREAM_DATA_WIDTH/8)-1) : 0 ]   keep;
    reg [((C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER] == 0) ? 1 : (C_XIL_AXI4STREAM_USER_WIDTH)-1)   : 0 ]   user;

    put_noise();

    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DEST]) begin
      m_vif.dest   <= trans.get_dest();
    end

    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_ID]) begin
      m_vif.id   <= trans.get_id();
    end

    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_LAST]) begin
      m_vif.last   <= trans.get_last();
    end

    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_STRB]) begin
      strb = trans.get_strb_beat();
      m_vif.strb <= strb;
    end

    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_KEEP]) begin
      keep = trans.get_keep_beat();
      m_vif.keep <= keep;
    end

    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DATA]) begin
      data = trans.get_data_beat();

      for (int j=0; j<C_XIL_AXI4STREAM_DATA_WIDTH/8;j++) begin
        if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_STRB]) begin
          if (strb[j] == 1'b0) begin
            data[(((j+1)*8)-1) -: 8] = ~data[(((j+1)*8)-1) -: 8];
          end
        end else if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_KEEP]) begin
          if ((no_insert_x_when_keep_low == 0) && (keep[j] == 1'b0)) begin
            data[(((j+1)*8)-1) -: 8] = 8'hxx;
          end
        end
      end
      m_vif.data <= data;
    end

    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER]) begin
      user = trans.get_user_beat();
      m_vif.user <= user;
    end

  endfunction

  /*
   Function: get_transaction
   Collects m_vif information and returns it to transaction class
  */
  virtual function axi4stream_monitor_transaction get_transaction();
    axi4stream_monitor_transaction trans;

    trans = new("AXI4STREAM_MON",
      C_XIL_AXI4STREAM_SIGNAL_SET,
      C_XIL_AXI4STREAM_DATA_WIDTH,
      C_XIL_AXI4STREAM_USER_WIDTH,
      C_XIL_AXI4STREAM_USER_BITS_PER_BYTE,
      C_XIL_AXI4STREAM_ID_WIDTH,
      C_XIL_AXI4STREAM_DEST_WIDTH);
    trans.set_name($sformatf("%s_%x", trans.get_name(),trans.get_cmd_id()));
    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DEST] == 1) begin
      trans.set_dest(m_vif.cb.TDEST);
    end
    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_ID  ] == 1) begin
      trans.set_id(m_vif.cb.TID);
    end
    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_LAST] == 1) begin
      trans.set_last(m_vif.cb.TLAST);
    end
    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_DATA] == 1) begin
      trans.set_data_beat(m_vif.cb.TDATA);
    end
    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_STRB] == 1) begin
      trans.set_strb_beat(m_vif.cb.TSTRB);
    end
    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_KEEP] == 1) begin
      trans.set_keep_beat(m_vif.cb.TKEEP);
    end
    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_USER] == 1) begin
      trans.set_user_beat(m_vif.cb.TUSER);
    end
    trans.accepted_time = this.get_current_edge_time();
    trans.accepted_cycle = this.get_current_clk_count();
    return(trans);
  endfunction

  /*
   Function: set_valid
   Sets valid of m_vif to be 1
  */
  virtual function void set_valid();
    m_vif.valid <= 1'b1;
  endfunction

  /*
   Function: clr_valid
   Sets valid of m_vif to be 0
  */
  virtual function void clr_valid();
    m_vif.valid <= 1'b0;
  endfunction

  /*
   Function: set_ready
   Sets ready of m_vif to be 1
  */
  virtual function void set_ready();
    m_vif.ready <= 1'b1;
  endfunction

  /*
   Function: clr_ready
   Sets ready of m_vif to be 0
  */
  virtual function void clr_ready();
    m_vif.ready <= 1'b0;
  endfunction


  ///////////////////////////////////////////////////////////////////////////////////
  //Live monitoring
  /*
   Function: is_live_valid_asserted
   Returns 1 if TVALID of m_vif is 1, else returns 0
  */
  virtual function bit is_live_valid_asserted();
    return(m_vif.TVALID == 1);
  endfunction


  /*
   Function: wait_live_valid_asserted
   Wait TVALID of m_vif is 1
  */
  virtual task wait_live_valid_asserted();
    if(m_vif.TVALID ==1'b0) begin
      @(posedge m_vif.TVALID );
    end  
  endtask
  /*
   Function: is_ready_asserted
   If HAS_TRAEADY is on, returns 1 if TREADY of clock block in m_vif is 1, else returns 0
   If HAS_TRAEADY is off, returns 1.
  */
  virtual function bit is_ready_asserted();
    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_READY] == 1) begin
      return(m_vif.cb.TREADY == 1);
    end else begin
      return 1'b1;
    end
  endfunction

  /*
   Function: is_valid_asserted
   Returns 1 if TVALID of clock block in m_vif is 1, else returns 0
  */
  virtual function bit is_valid_asserted();
    return(m_vif.cb.TVALID == 1);
  endfunction

  /*
   Function: is_areset_asserted
   Returns 1 if ARESET_N of clock block in m_vif is 0, else returns 0
  */
  virtual function bit is_areset_asserted();
    return(m_vif.cb.ARESET_N == 0);
  endfunction

  /*
   Function: wait_areset_asserted
   Waits negedge of ARSET_N of m_vif
  */
  virtual task wait_areset_asserted();
    if (m_vif.ARESET_N == 1'b1) begin
      @(negedge m_vif.ARESET_N);
    end
  endtask

  /*
   Function: wait_valid_sampled
   Waits till TVALID is sampled
  */
  virtual task wait_valid_sampled();
    @(m_vif.cb iff ((m_vif.cb.TVALID == 1) && (m_vif.cb.ACLKEN == 1)));
  endtask

  /*
   Function: wait_tx_accepted
   Waits till TREADY/TAVLID handshake occurs
  */
  virtual task wait_tx_accepted();
    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_READY] == 1) begin
      @(m_vif.cb iff ((m_vif.cb.TREADY == 1) && ( m_vif.cb.TVALID == 1) && (m_vif.cb.ARESET_N == 1) && (m_vif.cb.ACLKEN == 1)));
    end else begin
      @(m_vif.cb iff (( m_vif.cb.TVALID == 1) && (m_vif.cb.ARESET_N == 1) && (m_vif.cb.ACLKEN == 1)));
    end
  endtask

  /*
   Function: is_tx_accepted
   Returns 1 if TREADY/TVALID handshake occurs, otherwise returns 0
  */
  virtual function bit is_tx_accepted();
    return((m_vif.cb.TREADY == 1) && ( m_vif.cb.TVALID == 1) && (m_vif.cb.ACLKEN == 1));
  endfunction
endclass : axi4stream_vif_proxy

/*
  Class: axi4stream_monitor
  When active the AXI4STREAM Monitor will record transactions that are presented on the the virtual interface.
*/

class axi4stream_monitor `XIL_AXI4STREAM_PARAM_DECL extends xil_monitor;
  ///////////////////////////////////////////////////////////////////////////
  // The following two bits are used to control whether checks and coverage are
  // done both in the monitor class and the interface.
  bit                                                     checks_enable = 1;
  xil_analysis_port #(axi4stream_monitor_transaction)     item_collected_port;

  protected time                                          ready_assert_time = 0;
  protected time                                          valid_assert_time = 0;
  protected xil_axi4stream_ulong                          ready_assert_cycle = 0;
  protected xil_axi4stream_ulong                          valid_assert_cycle = 0;

  ///////////////////////////////////////////////////////////////////////////
  // AXI4STREAM interface
  axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER vif_proxy;

  ///////////////////////////////////////////////////////////////////////////
  // new - constructor
  /*
   Function: new
   Constructor to create a new axi4stream monitor object
  */
  function new (input string name);
    super.new(name);
    this.item_collected_port = new($sformatf("%s_item_collected_port", name));
    this.item_collected_port.set_enabled();
    this.set_tag("XIL_AXI4STREAM_MONITOR");
  endfunction : new

  /*
    Function: set_vif
    Assigns the virtual interface of the driver.
  */
  function void set_vif(axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER vif);
    this.vif_proxy = vif;
  endfunction : set_vif

  ///////////////////////////////////////////////////////////////////////////
  // run phase
  /*
    Function: run_phase
    Start control processes for operation
  */
  virtual task run_phase();
    if (!this.get_is_active()) begin
      this.set_is_active();
      this.stop_triggered_event = 0;
      if (this.vif_proxy == null) begin
        `xil_fatal(this.get_tag(), $sformatf("Attempted to start component (%s) without assigned virtual interface. Interfaces must be assigned before starting the component.", this.get_name()))
      end
      ///////////////////////////////////////////////////////////////////////////
      //Start the clock counters
      fork
        this.vif_proxy.run_phase();
      join_none
      this.vif_proxy.wait_posedge_aclk();
      this.vif_proxy.wait_areset_deassert();
      while (this.stop_triggered_event == 0) begin
        fork
          begin : ACTIVE
            `xil_info(TAG, "run()",verbosity)
            this.run_active();
          end
          begin : RESET_SERVICE
            this.vif_proxy.wait_areset_asserted();
          end
          begin : STOP
            @(posedge this.stop_triggered_event);
            `xil_info(this.get_tag(), "Stop event triggered. All traffic is being terminated.",this.verbosity)
          end
        join_any
        disable fork;
        if (this.stop_triggered_event == 0) begin
          `xil_info(this.get_tag(), $sformatf("RESET DETECTED"),this.verbosity)
          vif_proxy.wait_areset_deassert();
          `xil_info(this.get_tag(), $sformatf("RESET Released"),this.verbosity)
        end
      end
      this.clr_is_active();
    end
  endtask : run_phase

  ///////////////////////////////////////////////////////////////////////////
  //Stop the active processes.
  /*
    Function: stop_phase
    Stops all control processes.
  */
  virtual task stop_phase();
    this.stop_triggered_event = 1;
  endtask : stop_phase

  ///////////////////////////////////////////////////////////////////////////
  //Fork off the active processes.
  protected task run_active();
    fork
      this.watch_and_write();
      this.trap_events();
    join
  endtask

  ///////////////////////////////////////////////////////////////////////////
  //
  protected task watch_and_write ();
    axi4stream_monitor_transaction  vif_trans;

    forever begin : WRITE_TRANS
      ///////////////////////////////////////////////////////////////////////////
      // Wait for a grant then push the timestamps into the command...
      this.vif_proxy.wait_tx_accepted();
      #1ps;
      vif_trans = this.vif_proxy.get_transaction();

      ///////////////////////////////////////////////////////////////////////////
      // Update the timestamps
      vif_trans.set_name($sformatf("MON_%0d", vif_trans.get_cmd_id()));
      vif_trans.ready_assert_time = this.ready_assert_time;
      vif_trans.valid_assert_time = this.valid_assert_time;
      vif_trans.ready_assert_cycle = this.ready_assert_cycle;
      vif_trans.valid_assert_cycle = this.valid_assert_cycle;

      if (this.valid_assert_cycle > this.ready_assert_cycle) begin
        vif_trans.accepted_cycles = -1 * (this.valid_assert_cycle - this.ready_assert_cycle);
      end else begin
        vif_trans.accepted_cycles =      (this.ready_assert_cycle - this.valid_assert_cycle);
      end

      if (vif_trans.accepted_cycles > 0) begin
        vif_trans.backpressure = vif_trans.accepted_cycles - 1;
      end else begin
        vif_trans.backpressure = 0;
      end

      this.ready_assert_time = 0;
      this.valid_assert_time = 0;
      this.ready_assert_cycle = 0;
      this.valid_assert_cycle = 0;

      this.item_collected_port.write(vif_trans);
      `xil_info(TAG, $sformatf("Sent to scoreboard!\n%s", vif_trans.convert2string()), verbosity)
    end
  endtask : watch_and_write

  ///////////////////////////////////////////////////////////////////////////
  //Event handling
  protected task trap_events ();
    forever begin : TRAP
      vif_proxy.wait_posedge_aclk();
      #0ps;

      ///////////////////////////////////////////////////////////////////////////
      //Assertion Times for Handshakes
      if (vif_proxy.is_ready_asserted()) begin
        if (this.ready_assert_time == 0) begin
          this.ready_assert_time = this.vif_proxy.get_current_edge_time();
          this.ready_assert_cycle = this.vif_proxy.get_current_clk_count();
        end
      end else begin
        this.ready_assert_time = 0;
        this.ready_assert_cycle = 0;
      end
      if (vif_proxy.is_valid_asserted()) begin
        if (this.valid_assert_time == 0) begin
          this.valid_assert_time = this.vif_proxy.get_current_edge_time();
          this.valid_assert_cycle = this.vif_proxy.get_current_clk_count();
        end
      end else begin
        this.valid_assert_time = 0;
        this.valid_assert_cycle = 0;
      end
    end
  endtask

endclass : axi4stream_monitor

//Ready generation class
/*
  Class: axi4stream_ready_gen
  AXI4Stream Ready generation object
*/
class axi4stream_ready_gen extends xil_sequence_item;
  protected xil_axi4stream_uint               max_low_time = 5;
  protected xil_axi4stream_uint               min_low_time = 0;
  protected xil_axi4stream_uint               low_time = 2;
  rand xil_axi4stream_uint                    rand_low_time = 2;

  protected xil_axi4stream_uint               max_high_time = 5;
  protected xil_axi4stream_uint               min_high_time = 1;
  protected xil_axi4stream_uint               high_time = 5;
  rand xil_axi4stream_uint                    rand_high_time = 5;

  protected xil_axi4stream_uint               max_event_count = 1;
  protected xil_axi4stream_uint               min_event_count = 1;
  protected xil_axi4stream_uint               event_count = 1;
  rand xil_axi4stream_uint                    rand_event_count = 1;

  protected xil_axi4stream_uint               event_cycle_count_reset = 2000;
  protected xil_axi4stream_ready_gen_policy_t ready_policy = XIL_AXI4STREAM_READY_GEN_SINGLE;
  rand xil_axi4stream_ready_rand_policy_t     ready_rand_policy = XIL_AXI4STREAM_READY_RAND_SINGLE;
  protected xil_axi4stream_boolean_t          use_variable_ranges = XIL_AXI4STREAM_FALSE;

  ///////////////////////////////////////////////////////////////////////////
  //Constructor
  /*
   Function: new
   Constructor to create an new axi4stream ready gen object
  */
  function new(input string name="unnamed_axi4stream_ready_gen");
    super.new(name);
  endfunction : new

  /*
   Function: reset_to_defaults
   Reset all variables in ready generation to default value
  */
  virtual function void reset_to_defaults();
    this.max_low_time = 5;
    this.min_low_time = 0;

    this.max_high_time = 5;
    this.min_high_time = 1;

    this.max_event_count = 1;
    this.min_event_count = 1;

    this.event_cycle_count_reset = 2000;
    this.ready_policy = XIL_AXI4STREAM_READY_GEN_SINGLE;
    this.ready_rand_policy = XIL_AXI4STREAM_READY_RAND_SINGLE;
    this.event_count = 1;
    this.high_time = 5;
    this.low_time = 2;
    this.rand_event_count = 1;
    this.rand_high_time = 5;
    this.rand_low_time = 1;
    this.use_variable_ranges = XIL_AXI4STREAM_FALSE;
  endfunction : reset_to_defaults

  /*
    Function:  copy
    Copies the contents of the input ready generation to the current ready generation
  */
  function void copy(axi4stream_ready_gen rhs);
    this.max_low_time             =  rhs.max_low_time           ;
    this.min_low_time             =  rhs.min_low_time           ;
    this.max_high_time            =  rhs.max_high_time          ;
    this.min_high_time            =  rhs.min_high_time          ;
    this.max_event_count          =  rhs.max_event_count        ;
    this.min_event_count          =  rhs.min_event_count        ;
    this.event_cycle_count_reset  =  rhs.event_cycle_count_reset;
    this.ready_policy             =  rhs.ready_policy           ;
    this.low_time                 =  rhs.low_time               ;
    this.high_time                =  rhs.high_time              ;
    this.event_count              =  rhs.event_count            ;
    this.ready_rand_policy        =  rhs.ready_rand_policy      ;
    this.rand_event_count         =  rhs.rand_event_count       ;
    this.rand_high_time           =  rhs.rand_high_time         ;
    this.rand_low_time            =  rhs.rand_low_time          ;
    if (rhs.get_use_variable_ranges() == XIL_AXI4STREAM_TRUE) begin
      this.set_use_variable_ranges();
    end else begin
      this.clr_use_variable_ranges();
    end
  endfunction : copy

  /*
    Function: my_clone
    Clones the current ready generation and returns a handle to the new generation
  */
  virtual function axi4stream_ready_gen my_clone ();
    axi4stream_ready_gen           my_obj;
    my_obj = new( this.get_name());

    my_obj.set_id_info(this);
    my_obj.copy(this);
    return(my_obj);
  endfunction : my_clone

  /*
   Function: convert2string
   Returns  a form of string of ready object
  */
  virtual function string convert2string();
    string ret = "";
    ret = $sformatf("%s\n-------------------------------------------------------", ret);
    ret = $sformatf("%s\nNAME                               VALUE", ret);
    ret = $sformatf("%s\n-------------------------------------------------------", ret);
    ret = $sformatf("%s\nReady_gen               ", ret);
    ret = $sformatf("%s\n  Policy                   %s", ret, this.ready_policy.name());
    if (this.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_RANDOM) begin
      ret = $sformatf("%s\n  Random Policy            %s", ret, this.ready_rand_policy.name());
    end
    ret = $sformatf("%s\n  Use Variable Ranges        %s", ret, this.use_variable_ranges.name());
    ret = $sformatf("%s\n  Max Low Time               %d", ret, this.max_low_time);
    ret = $sformatf("%s\n  Min Low Time               %d", ret, this.min_low_time);
    ret = $sformatf("%s\n  Max High Time              %d", ret, this.max_high_time);
    ret = $sformatf("%s\n  Min High Time              %d", ret, this.min_high_time);
    ret = $sformatf("%s\n  Max Event Count            %d", ret, this.max_event_count);
    ret = $sformatf("%s\n  Min Event Count            %d", ret, this.min_event_count);
    ret = $sformatf("%s\n  Event Cycle Count Reset    %d", ret, this.event_cycle_count_reset);
    ret = $sformatf("%s\n  Low Time                   %d", ret, this.get_low_time());
    ret = $sformatf("%s\n  High Time                  %d", ret, this.get_high_time());
    ret = $sformatf("%s\n  Event Count                %d", ret, this.get_event_count());
    ret = $sformatf("%s\n-------------------------------------------------------", ret);
    return(ret);
  endfunction : convert2string


  /*
    Function: set_use_variable_ranges
    Sets the use of the variable ranges when the policy of ready generation is not RANDOM
  */
  virtual function void set_use_variable_ranges();
    this.use_variable_ranges = XIL_AXI4STREAM_TRUE;
  endfunction : set_use_variable_ranges

  /*
    Function: clr_use_variable_ranges
    Clears the use of the variable ranges when the policy of ready generation is not RANDOM
  */
  virtual function void clr_use_variable_ranges();
    this.use_variable_ranges = XIL_AXI4STREAM_FALSE;
  endfunction : clr_use_variable_ranges

  /*
    Function: get_use_variable_ranges
    Returns the current state of the variable range use feature.
  */
  virtual function xil_axi4stream_boolean_t get_use_variable_ranges();
    return(this.use_variable_ranges);
  endfunction :get_use_variable_ranges

  /*
    Function: set_ready_policy
    Sets the policy of ready generation
  */
  virtual function void set_ready_policy(input xil_axi4stream_ready_gen_policy_t value);
    this.ready_policy = value;
    if (value == XIL_AXI4STREAM_READY_GEN_NO_BACKPRESSURE) begin
      this.set_low_time(0);
      this.set_low_time_range(0,0);
      this.set_high_time(1);
      this.set_high_time_range(1,1);
      this.clr_use_variable_ranges();
    end
  endfunction

  /*
    Function: get_ready_policy
    Returns the current ready generation policy
  */
  virtual function xil_axi4stream_ready_gen_policy_t get_ready_policy();
    return(this.ready_policy);
  endfunction

  /*
   Function: set_event_cycle_count_reset
   Set event_cycle_count_reset value of ready generation
  */
  virtual function void set_event_cycle_count_reset(input xil_axi4stream_uint value);
    this.event_cycle_count_reset = value;
  endfunction

  /*
   Function: get_event_cycle_count_reset
   Returns the current event_cycle_count_reset
  */
  virtual function xil_axi4stream_uint get_event_cycle_count_reset();
    get_event_cycle_count_reset = this.event_cycle_count_reset;
  endfunction

  /*
    Function: get_low_time_range
    Returns min_low_time and max_low_time of the current ready generation
  */
  virtual function void get_low_time_range(output xil_axi4stream_uint min,
                          output xil_axi4stream_uint max);
    min = this.min_low_time;
    max = this.max_low_time;
  endfunction

  /*
    Function: set_low_time_range
    Sets min_low_time and max_low_time of the current ready generation
  */
  virtual function void set_low_time_range(input xil_axi4stream_uint min,
                                   input xil_axi4stream_uint max);
    if (min > max) begin
      `xil_fatal(this.get_name(), $sformatf("LOW_TIME: Attempted to set the max (%d) value lower than the min (%d)", max, min))
    end
    this.min_low_time = min;
    this.max_low_time = max;
  endfunction

  /*
    Function: set_low_time
    Sets low_time of the current ready generation
  */
  virtual function void set_low_time(input xil_axi4stream_uint value);
    if ((this.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_NO_BACKPRESSURE) && (value > 0)) begin
      `xil_fatal(this.get_name(), $sformatf("LOW_TIME: Attempted to set the low time(%d) when policy is %s", value, this.ready_policy.name()))
    end else begin
      this.low_time = value;
    end
  endfunction

  /*
    Function: get_low_time
    Returns low time of the current ready generation
  */
  virtual function xil_axi4stream_uint get_low_time();
    if (this.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_NO_BACKPRESSURE) begin
      return(0);
    end else if ((this.get_use_variable_ranges() == XIL_AXI4STREAM_TRUE) || (this.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_RANDOM)) begin
      return (this.rand_low_time);
    end else begin
      return (this.low_time);
    end
  endfunction

  /*
    Function: get_high_time_range
    Returns min_high_time and max_high_time of the current ready generation
  */
  virtual function void get_high_time_range(output xil_axi4stream_uint min,
                           output xil_axi4stream_uint max);
    min = this.min_high_time;
    max = this.max_high_time;
  endfunction

  /*
    Function: set_high_time_range
    Sets min_high_time and max_high_time of the current ready generation
  */
  virtual function void set_high_time_range(input xil_axi4stream_uint min,
                                   input xil_axi4stream_uint max);
    if (min <1) begin
      `xil_fatal(this.get_name(), $sformatf("HIGH_TIME: Attempted to set the min(%d) value smaller than 1",min))
    end else if (min > max) begin
      `xil_fatal(this.get_name(), $sformatf("HIGH_TIME: Attempted to set the max (%d) value lower than the min (%d)", max, min))
    end
    this.min_high_time = min;
    this.max_high_time = max;
  endfunction

  /*
    Function: set_high_time
    Sets high_time of the current ready generation
  */
  virtual function void set_high_time(input xil_axi4stream_uint value);  
    if (value <1) begin
      `xil_fatal(this.get_name(), $sformatf("HIGH_TIME: Attempted to set high time (%d) smaller than 1",value))
    end else if ((this.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_NO_BACKPRESSURE) && (value > 1)) begin
      `xil_warning(this.get_name(), $sformatf("HIGH_TIME: Attempted to set the high time(%d) when policy is %s", value, this.ready_policy.name()))
      this.high_time = value;
    end else begin  
      this.high_time = value;
    end  
  endfunction

  /*
    Function: get_high_time
    Returns high time of the current ready generation
  */
  virtual function xil_axi4stream_uint get_high_time();
    if (this.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_NO_BACKPRESSURE) begin
      return(1);
    end else if ((this.get_use_variable_ranges() == XIL_AXI4STREAM_TRUE) || (this.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_RANDOM)) begin
      return (this.rand_high_time);
    end else begin
      return (this.high_time);
    end
  endfunction

  /*
    Function: get_event_count_range
    Returns min_event_count and max_event_count of the current ready generation
  */
  virtual function void get_event_count_range(output xil_axi4stream_uint min,
                             output xil_axi4stream_uint max);
    min = this.min_event_count;
    max = this.max_event_count;
  endfunction

   /*
    Function: set_event_count_range
    Sets min_event_count and max_event_count of the current ready generation
  */
  virtual function void set_event_count_range(input xil_axi4stream_uint min,
                                      input xil_axi4stream_uint max);
    if (min > max) begin
      `xil_fatal(this.get_name(), $sformatf("EVENT_COUNT: Attempted to set the max (%d) value lower than the min (%d)", max, min))
    end
    this.min_event_count = min;
    this.max_event_count = max;
  endfunction

  /*
    Function: get_event_count
    Returns event_count of the current ready generation
  */
  virtual function xil_axi4stream_uint get_event_count();
    if ((this.get_use_variable_ranges() == XIL_AXI4STREAM_TRUE) || (this.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_RANDOM)) begin
      return (this.rand_event_count);
    end else begin
      return (this.event_count);
    end
  endfunction : get_event_count

  /*
    Function: set_event_count
    Sets the number of events that ready stays at high
  */
  virtual function void set_event_count(input xil_axi4stream_uint in);
    this.event_count = in;
  endfunction : set_event_count

  /*
   Function: get_ready_rand_policy
   Returns ready_rand_policy of the ready generation
  */
  virtual function xil_axi4stream_ready_rand_policy_t get_ready_rand_policy();
    return(this.ready_rand_policy);
  endfunction : get_ready_rand_policy

  constraint c_low_time { rand_low_time inside {[min_low_time:max_low_time]};}
  constraint c_high_time { rand_high_time inside {[min_high_time:max_high_time]};}
  constraint c_event_count { rand_event_count inside {[min_event_count:max_event_count]};}

  constraint c_rand_policy {
    ready_rand_policy dist {
      XIL_AXI4STREAM_READY_RAND_SINGLE              :/ 30,
      XIL_AXI4STREAM_READY_RAND_EVENTS              :/ 10,
      XIL_AXI4STREAM_READY_RAND_OSC                 :/ 15,
      XIL_AXI4STREAM_READY_RAND_AFTER_VALID_SINGLE  :/ 20,
      XIL_AXI4STREAM_READY_RAND_AFTER_VALID_EVENTS  :/ 10,
      XIL_AXI4STREAM_READY_RAND_AFTER_VALID_OSC     :/ 15
    };
  }

  /*
   Function: cheap_random
   Generate simplified randomization of ready class when user defines XIL_DO_NOT_USE_ADV_RANDOMIZATION
  */
  virtual function void cheap_random();
    xil_axi4stream_uint dice;
    if ((this.get_use_variable_ranges() == XIL_AXI4STREAM_TRUE) || (this.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_RANDOM)) begin
      if (this.max_event_count == this.min_event_count) begin
        this.event_count = this.min_event_count;
      end else begin
        this.event_count = ($urandom() % (this.max_event_count - this.min_event_count)) + this.min_event_count;
      end
      if (this.max_low_time == this.min_low_time) begin
        this.low_time = this.min_low_time;
      end else begin
        this.low_time = ($urandom() % (this.max_low_time - this.min_low_time)) + this.min_low_time;
      end
      if (this.max_low_time == this.min_low_time) begin
        this.high_time = this.min_high_time;
      end else begin
        this.high_time = ($urandom() % (this.max_high_time - this.min_high_time)) + this.min_high_time;
      end
      
      dice = ($urandom() % 6);
      case (dice)
        0: this.ready_rand_policy = XIL_AXI4STREAM_READY_RAND_SINGLE;
        1: this.ready_rand_policy = XIL_AXI4STREAM_READY_RAND_OSC;
        2: this.ready_rand_policy = XIL_AXI4STREAM_READY_RAND_AFTER_VALID_SINGLE;
        3: this.ready_rand_policy = XIL_AXI4STREAM_READY_RAND_AFTER_VALID_OSC;
        4: this.ready_rand_policy = XIL_AXI4STREAM_READY_RAND_EVENTS;
        5: this.ready_rand_policy = XIL_AXI4STREAM_READY_RAND_AFTER_VALID_EVENTS;
      endcase
    end
  endfunction : cheap_random

endclass : axi4stream_ready_gen

typedef axi4stream_ready_gen         axi4stream_ready_gen_t;

/*
  Class: axi4stream_mst_driver 
  AXI4STREAM Master Driver Object
*/
class axi4stream_mst_driver `XIL_AXI4STREAM_PARAM_DECL extends xil_driver #(axi4stream_transaction,axi4stream_transaction);
  ///////////////////////////////////////////////////////////////////////////
  // user configurable parameters
  protected xil_axi4stream_uint                     forward_progress_timeout_value = 50000;
  protected xil_axi4stream_boolean_t                transfer_active = XIL_AXI4STREAM_FALSE;
  axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER  vif_proxy;

  ///////////////////////////////////////////////////////////////////////////
  // internal events
  /*
   Function: new
   Constructor to create a new axi4stream master driver
  */
  function new(input string                     name = "unnamed_axi4stream_mst_driver");
    super.new(name);
    this.stop_triggered_event = 0;
    this.set_tag("XIL_AXI4STREAM_MST_DRIVER");
  endfunction

  /*
    Function: set_vif
    Assigns the virtual interface of the driver.
  */
  function void set_vif(axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER vif);
    this.vif_proxy = vif;
  endfunction : set_vif

  /*
   Function: set_forward_progress_timeout_value
   Sets the number of cycles that the driver will wait until it will flag a watch dog error.
   Setting this to a very large value will cause a hung simulation to continue for a longer time.
   Setting this to a very small number may not allow the slave to respond.
  */
  function void set_forward_progress_timeout_value(input xil_axi4stream_uint value);
    this.forward_progress_timeout_value = value;
  endfunction : set_forward_progress_timeout_value

  /*
   Function: get_forward_progress_timeout_value
   Returns the current value of the forward_progress_timeout_value.
  */
  function xil_axi4stream_uint get_forward_progress_timeout_value();
    return(this.forward_progress_timeout_value);
  endfunction : get_forward_progress_timeout_value

  /*
    Function: is_driver_idle
    When the driver is actively processing a transaction this function will return FALSE.
  */
  function xil_axi4stream_boolean_t is_driver_idle();
    return((this.transfer_active == XIL_AXI4STREAM_TRUE) ? XIL_AXI4STREAM_FALSE : XIL_AXI4STREAM_TRUE);
  endfunction : is_driver_idle

  ///////////////////////////////////////////////////////////////////////////
  // Main entry point.
  /*
    Function: run_phase
    Start control processes for operation
  */
  task run_phase();
    if (!this.get_is_active()) begin
      this.set_is_active();
      this.stop_triggered_event = 0;
      if (this.vif_proxy == null) begin
        `xil_fatal(this.get_tag(), $sformatf("Attempted to start component (%s) without assigned virtual interface. Virtual interfaces must be assigned before starting the component.", this.get_name()))
      end
      vif_proxy.reset();
      vif_proxy.wait_posedge_aclk();
      vif_proxy.wait_areset_deassert();
      while (this.stop_triggered_event == 0) begin
        fork
          begin : ACTIVE
            `xil_info(TAG, "run()", verbosity)
            this.run_active();
          end
          begin : RESET_SERVICE
            vif_proxy.wait_areset_asserted();
          end
          begin : STOP
            @(posedge this.stop_triggered_event);
            `xil_info(this.get_tag(), "Stop event triggered. All traffic is being terminated.",this.verbosity)
          end
        join_any
        disable fork;
        vif_proxy.reset();
        if (this.stop_triggered_event == 0) begin
          `xil_info(this.get_tag(), $sformatf("RESET DETECTED"),this.verbosity)
          this.vif_proxy.wait_areset_deassert();
          `xil_info(this.get_tag(), $sformatf("RESET Released"),this.verbosity)
        end
      end
      this.clr_is_active();
    end else begin
      `xil_warning(TAG, $sformatf("Component (%s) is already in the active state.", this.get_name()))
    end
  endtask

  //Stop the active processes.
  /*
    Function: stop_phase
    Stops all control processes.
  */
  virtual task stop_phase();
    this.stop_triggered_event = 1;
  endtask : stop_phase

  ///////////////////////////////////////////////////////////////////////////
  //Fork off the active processes.
  protected task run_active();
    fork
      this.get_and_drive();
    join
  endtask

  ///////////////////////////////////////////////////////////////////////////
  //Interaction thread with the sequencer.
  protected task get_and_drive();
    axi4stream_transaction            next_item;
    axi4stream_transaction            item;
    xil_axi4stream_uint               backpressure_cycle_count = 0;
    xil_axi4stream_uint               actual_delay_cycles = 0;
    time                              put_transaction_time, finish_transaction_time;

    ///////////////////////////////////////////////////////////////////////////
    //Continuous process getting sequence items from the sequencer and
    //assigning them to the bus via the VIF
    vif_proxy.wait_posedge_aclk_with_hold();

    forever begin : GET_AND_DRIVE
      this.vif_proxy.clr_valid();
      next_item = null;
      while (next_item == null) begin
        this.seq_item_port.try_next_item(next_item);
        if (next_item == null) begin
          vif_proxy.wait_posedge_aclk_with_hold();
          this.vif_proxy.put_noise();
        end
      end

      item = next_item.my_clone();
      if (this.seq_item_port.item_done()) begin
        `xil_fatal(this.get_tag(), $sformatf("Internal transaction QUEUE accounting has failed. Please contact Xilinx Support.\nTransaction: %s", next_item.convert2string()))
      end

      this.transfer_active = XIL_AXI4STREAM_TRUE;

      // Depending on the delay policy, adjust the transfer delay
      // to compensate for any backpressure observed for the prior transfer...
      if (item.get_delay() > 0) begin

        actual_delay_cycles = item.get_delay();

        if (item.get_delay_policy() == XIL_AXI4STREAM_DELAY_INSERTION_FROM_IDLE) begin
          if(actual_delay_cycles < backpressure_cycle_count) begin
            actual_delay_cycles =0;
          end else begin  
            actual_delay_cycles = actual_delay_cycles - backpressure_cycle_count;
          end  
        end
       
        

        for (xil_axi4stream_uint acnt = 0; acnt < actual_delay_cycles; acnt++) begin
          fork
            vif_proxy.wait_posedge_aclk_with_hold();
            vif_proxy.put_noise();
          join
        end
      end

      backpressure_cycle_count = 0;

      fork
        begin : DRIVER_PUT_TRANSACTION
          vif_proxy.put_transaction(item);
          `xil_info(TAG, "Put transaction:", verbosity)
          `xil_info(TAG, $sformatf("%s", item.convert2string()), verbosity)
          vif_proxy.set_valid();

          if (item.get_driver_return_item_policy() == XIL_AXI4STREAM_AT_ASSERT_RETURN) begin
            return_item_to_sequence(item);
          end

          vif_proxy.wait_tx_accepted();
        end // block: DRIVER_PUT_TRANSACTION

        begin : DRIVER_COUNT_BACKPRESSURE
          forever begin
            vif_proxy.wait_posedge_aclk();
            backpressure_cycle_count++;
          end

        end
      join_any
      disable fork;

      if (backpressure_cycle_count > 0) begin
        `xil_info(TAG, $sformatf("Backpressure detected: %3d cycles", backpressure_cycle_count), verbosity)
      end

      if (item.get_driver_return_item_policy() == XIL_AXI4STREAM_AT_ACCEPT_RETURN) begin
        return_item_to_sequence(item);
      end

      transfer_active = XIL_AXI4STREAM_FALSE;
      item = null;
      #this.vif_proxy.hold_time;
    end
  endtask

  protected task return_item_to_sequence(input axi4stream_transaction transfer);
    axi4stream_transaction transfer_clone;
    xil_object      cloned;
    cloned = transfer.my_clone();
    $cast(transfer_clone, cloned);
    if (this.seq_item_port.put_rsp(transfer_clone)) begin
      `xil_fatal(this.get_tag(), $sformatf("The limit of the number of response transaction has been exceeded using (seq_item_port). Trying to put_rsp:\n%s", transfer_clone.convert2string()))
    end
  endtask

  protected task forward_progress_watchdog();
    forever begin : PROGRESS
      ///////////////////////////////////////////////////////////////////////////
      //Wait until there is at least one transaction to be processed. This is here
      // to cover the case where there are not sequence items for the read channel
      vif_proxy.wait_valid_asserted();

      fork
        begin : CHANNEL
          vif_proxy.wait_tx_accepted();
        end
        ///////////////////////////////////////////////////////////////////////////
        //Timeout if there is no accepted beats within forward_progress_timeout_value cycles
        begin : WATCHDOG
          vif_proxy.wait_aclks(this.get_forward_progress_timeout_value());
          `xil_fatal(TAG, $sformatf("%m : TVALID watchdog triggered, no forward progess detected. TVALID is asserted for (%d) cycles and no TREADY has been detected. To increase this timeout use: set_forward_progress_timeout_value", this.get_forward_progress_timeout_value()))
        end
      join_any
      disable fork;
      vif_proxy.wait_aclks(10);
    end
  endtask : forward_progress_watchdog

  /*
    Function: send
    Send axi4stream transaction object to the driver 
  */
  task send(input axi4stream_transaction t );
    if(this.get_is_active()) begin
      if (this.seq_item_port.put_item(t)) begin
        `xil_fatal(this.get_tag(), $sformatf("The limit of the number of outstanding has been exceeded using (seq_item_port). Trying to send:\n%s", t.convert2string()))
      end
      seq_item_port.wait_for_item_done();
    end else begin
      `xil_fatal(this.get_tag(), $sformatf("%m : Component (%s) is not active. Trying to send transaction: %s", this.get_name(), t.convert2string()))
    end
  endtask  : send

  /*
   Function: create_transaction
   Returns an axi4stream transaction that has been "newed"
  */
   virtual function axi4stream_transaction create_transaction (string name = "unnamed_transaction");
    axi4stream_transaction    item = new(name,
      C_XIL_AXI4STREAM_SIGNAL_SET,
      C_XIL_AXI4STREAM_DATA_WIDTH,
      C_XIL_AXI4STREAM_USER_WIDTH,
      C_XIL_AXI4STREAM_USER_BITS_PER_BYTE,
      C_XIL_AXI4STREAM_ID_WIDTH,
      C_XIL_AXI4STREAM_DEST_WIDTH);
    return(item);
  endfunction :create_transaction

endclass : axi4stream_mst_driver

/*
  Class: axi4stream_slv_driver
  AXI4STREAM Slave Driver Object
*/
class axi4stream_slv_driver `XIL_AXI4STREAM_PARAM_DECL extends xil_driver #(axi4stream_ready_gen,axi4stream_ready_gen);
  protected axi4stream_ready_gen_t        ready_gen;
  protected axi4stream_ready_gen_t        ready_gen_q[$];
  event                                   ready_popped;
  protected time                          debug_transition_gap = 0ps;

  ///////////////////////////////////////////////////////////////////////////
  // AXI4STREAM interface
  axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER vif_proxy;

  /*
   Function: new
   Constructor to create a new axi4stream slave driver object
  */
  function new(input string                     name = "unnamed_axi4stream_slv_driver");
    super.new(name);
    this.stop_triggered_event = 0;
    this.ready_gen = new("slv_ready_gen");
    this.set_tag("XIL_AXI4STREAM_SLV_DRIVER");
  endfunction

  /*
    Function: set_vif
    Assigns the virtual interface of the driver.
  */
  function void set_vif(axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER vif);
    this.vif_proxy = vif;
  endfunction : set_vif

  ///////////////////////////////////////////////////////////////////////////
  // Main definition enabling all the channels.
  /*
    Function: run_phase
    Start control processes for operation
  */
  virtual task run_phase();
    if (!this.get_is_active()) begin
      this.set_is_active();
      this.stop_triggered_event = 0;
      if (this.vif_proxy == null) begin
        `xil_fatal(this.get_tag(), $sformatf("Attempted to start component (%s) without first assigning a virtual interface", this.get_name()))
      end
      this.vif_proxy.clr_ready();
      this.vif_proxy.wait_posedge_aclk();
      this.vif_proxy.wait_areset_deassert();

      while (this.stop_triggered_event == 0) begin
        fork
          begin : ACTIVE
            `xil_info(TAG, "run()", verbosity)
            this.run_active();
          end
          begin : RESET_SERVICE
            this.vif_proxy.wait_areset_asserted();
          end
          begin : STOP
            @(posedge this.stop_triggered_event);
            `xil_info(this.get_tag(), "Stop event triggered. All traffic is being terminated.",this.verbosity)
          end
        join_any
        disable fork;

        `xil_info(TAG, $sformatf("RESET DETECTED"),XIL_VERBOSITY_NONE)
        this.vif_proxy.clr_ready();
        if (this.stop_triggered_event == 0) begin
          this.vif_proxy.wait_areset_deassert();
          `xil_info(TAG, $sformatf("RESET Released"),XIL_VERBOSITY_NONE)
        end  
      end
      this.clr_is_active();
    end else begin
      `xil_warning(TAG, $sformatf("Component (%s) is already in the active state.", this.get_name()))
    end
  endtask : run_phase

  ///////////////////////////////////////////////////////////////////////////
  //Stop the active processes.
  /*
    Function: stop_phase
    Stops all control processes.
  */
  virtual task stop_phase();
    this.stop_triggered_event = 1;
  endtask : stop_phase

  ///////////////////////////////////////////////////////////////////////////
  //Fork off the active processes.
  protected task run_active();
    fork
      get_next_ready_item();
      ready_generation();
    join
  endtask

  protected task get_next_ready_item();
    axi4stream_ready_gen            next_item;
    axi4stream_ready_gen            cloned;
    forever begin : GNI
      ///////////////////////////////////////////////////////////////////////////
      //Blocking GET
      this.seq_item_port.get_next_item(next_item);

      `xil_info(TAG, "get_next_ready_gen:",verbosity)
      `xil_info(TAG, $sformatf("%s", next_item.convert2string()), verbosity)
      cloned = next_item.my_clone();
      ready_gen_q.push_back(cloned);
      if (this.seq_item_port.item_done()) begin
        `xil_fatal(this.get_tag(), $sformatf("Internal ready QUEUE accounting has failed. Please contact Xilinx Support.\nTransaction: %s", next_item.convert2string()))
      end
      @(this.ready_popped);
    end : GNI
  endtask : get_next_ready_item

  protected task ready_policy_generation();
    xil_axi4stream_uint event_counter;
    forever begin : READY_POLICY_GENERATION

      ////////////////////////////////////////////////////////////////////////////
      // If there is an entry in the queue then pop it off
      if (ready_gen_q.size() > 0) begin
        this.ready_gen = ready_gen_q.pop_front();
        `xil_info(TAG,"Changing ready_gen policy:",verbosity)
        `xil_info(TAG, $sformatf("%s", ready_gen.convert2string()), verbosity)
        -> this.ready_popped;
      end
      ///////////////////////////////////////////////////////////////////////////
      //Get random values
      `ifndef XIL_DO_NOT_USE_ADV_RANDOMIZATION
        XIL_AXI4STREAM_SLV_POLICY_GEN_RAND_FAILED: assert(this.ready_gen.randomize());
      `else
        this.ready_gen.cheap_random();
      `endif

      ///////////////////////////////////////////////////////////////////////////
      //If the ready algorithm is to wait until after VALID has been asserted, then
      // check to see if VALID is asserted, if not wait until it is sampled asserted.
      if ((this.ready_gen.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_AFTER_VALID_SINGLE) ||
          (this.ready_gen.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_AFTER_VALID_EVENTS) ||
          (this.ready_gen.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_AFTER_VALID_OSC) ||
          ((this.ready_gen.get_ready_policy() == XIL_AXI4STREAM_READY_GEN_RANDOM) && (
            (this.ready_gen.get_ready_rand_policy() == XIL_AXI4STREAM_READY_RAND_AFTER_VALID_EVENTS) ||
            (this.ready_gen.get_ready_rand_policy() == XIL_AXI4STREAM_READY_RAND_AFTER_VALID_SINGLE) ||
            (this.ready_gen.get_ready_rand_policy() == XIL_AXI4STREAM_READY_RAND_AFTER_VALID_OSC)))) begin
        this.vif_proxy.clr_ready();
        this.vif_proxy.wait_negedge_aclk();
        ///////////////////////////////////////////////////////////////////////////
        //If the valid is not asserted then wait until the posedge
        if (vif_proxy.is_live_valid_asserted() == 0) begin
          this.vif_proxy.wait_live_valid_asserted();
        end
      end

      ///////////////////////////////////////////////////////////////////////////
      //Wait low time then assert for one handshake
      `xil_info(TAG, $sformatf("get_low_time %3d", this.ready_gen.get_low_time()), verbosity)
      if (this.ready_gen.get_low_time() > 0) begin
        this.vif_proxy.clr_ready();
        this.vif_proxy.wait_aclks(this.ready_gen.get_low_time());
        #vif_proxy.hold_time;
      end
      event_counter = 0;
      this.vif_proxy.set_ready();
      case (this.ready_gen.get_ready_policy())
        XIL_AXI4STREAM_READY_GEN_AFTER_VALID_EVENTS,
        XIL_AXI4STREAM_READY_GEN_EVENTS: begin
          fork
            begin
              while (this.ready_gen.get_event_count() > event_counter) begin
                this.vif_proxy.wait_tx_accepted();
                event_counter++;
              end
            end
            begin
              forever begin
                this.vif_proxy.wait_aclks(this.ready_gen.get_event_cycle_count_reset());
                event_counter = 0;
              end
            end
          join_any
          disable fork;
        end
        XIL_AXI4STREAM_READY_GEN_AFTER_VALID_OSC,
        XIL_AXI4STREAM_READY_GEN_OSC: begin
          this.vif_proxy.wait_aclks(this.ready_gen.get_high_time());
        end
        XIL_AXI4STREAM_READY_GEN_RANDOM: begin
          case(this.ready_gen.get_ready_rand_policy())
            XIL_AXI4STREAM_READY_RAND_AFTER_VALID_OSC,
            XIL_AXI4STREAM_READY_RAND_OSC: begin
              this.vif_proxy.wait_aclks(this.ready_gen.get_high_time());
            end
            XIL_AXI4STREAM_READY_RAND_AFTER_VALID_EVENTS,
            XIL_AXI4STREAM_READY_RAND_EVENTS: begin
              fork
                begin
                  while (this.ready_gen.get_event_count() > event_counter) begin
                    this.vif_proxy.wait_tx_accepted();
                    event_counter++;
                  end
                end
                begin
                  forever begin
                    this.vif_proxy.wait_aclks(this.ready_gen.get_event_cycle_count_reset());
                    event_counter = 0;
                  end
                end
              join_any
              disable fork;
            end
            default : begin
              this.vif_proxy.wait_tx_accepted();
            end
          endcase
        end
        default: begin
          this.vif_proxy.wait_tx_accepted();
        end
      endcase
      #vif_proxy.hold_time;
    end //forever begin
  endtask : ready_policy_generation

  ///////////////////////////////////////////////////////////////////////////
  //Depending on the READY algorithm
  protected task ready_generation();
    this.vif_proxy.clr_ready();
    this.vif_proxy.wait_posedge_aclk();
    forever begin : READY_GENERATION
      this.ready_policy_generation();
    end //forever begin
  endtask : ready_generation

  ////////////////////////////////////////////////////////////////////////////
  //send ready and wait
  /*
   Function: send_tready
   Send ready object to the driver when HAS_TREADY is on
  */
  task send_tready(input axi4stream_ready_gen t);
    if (C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_READY] == 0) begin
      `xil_fatal(this.get_tag(), $sformatf("Attempted to send ready while component (%s) HAS_TREADY is disabled.", this.get_name()))
    end else begin
      if(this.get_is_active()) begin
        if (this.seq_item_port.put_item(t)) begin
          `xil_fatal(this.get_tag(), $sformatf("The limit of the number of outstanding has been exceeded using (seq_item_port). Trying to send:\n%s", t.convert2string()))
        end
        seq_item_port.wait_for_item_done();
      end else begin
        `xil_fatal(this.get_tag(), $sformatf("%m : Component (%s) is not active. Cannot send ready: %s", this.get_name(), t.convert2string()))
      end
    end  
  endtask

  /*
    Function: create_ready
    Returns Ready class that has been "newed" 
  */
  virtual function axi4stream_ready_gen create_ready (string name = "unnamed_ready");
    axi4stream_ready_gen    ready=new(name);
    return(ready);
  endfunction

endclass : axi4stream_slv_driver

/*
  Class: axi4stream_mst_agent
  AXI4STREAM Master Agent.
*/

class axi4stream_mst_agent `XIL_AXI4STREAM_PARAM_DECL extends xil_agent;
  axi4stream_mst_driver `XIL_AXI4STREAM_PARAM_ORDER driver;
  axi4stream_monitor `XIL_AXI4STREAM_PARAM_ORDER monitor;
  axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER vif_proxy;

  /*
    Function: new
    Constructor to create an AXI4STREAM Master Agent.
  */
  function new (input string name = "unnamed_axi4stream_mst_agent",virtual interface axi4stream_vip_if `XIL_AXI4STREAM_PARAM_ORDER vif);
    super.new(name);
    this.monitor = new($sformatf("%s_monitor",name));
    this.driver = new($sformatf("%s_driver",name));
    this.vif_proxy = new($sformatf("%s_vif",name));
    this.vif_proxy.assign_vi(vif);
    this.set_vif(this.vif_proxy);
    this.set_tag("XIL_AXI4STREAM_MST_AGENT");
  endfunction

  /*
    Function: set_verbosity
    Sets the verbosity of the Agent and all sub classes.
  */
  virtual function void set_verbosity(xil_verbosity updated);
    this.verbosity = updated;
    this.monitor.set_verbosity(this.get_verbosity);
    this.driver.set_verbosity(this.get_verbosity);
  endfunction : set_verbosity

  /*
    Function: set_agent_tag
    Sets the tag of the Agent and all sub classes.
  */
  virtual function void set_agent_tag(string updated);
    this.TAG = updated;
    this.monitor.set_tag($sformatf("%s_monitor", this.get_tag()));
    this.driver.set_tag($sformatf("%s_driver", this.get_tag()));
    this.vif_proxy.set_tag($sformatf("%s_vif", this.get_tag()));
  endfunction : set_agent_tag

  /*
    Function: set_vif
    Sets the Agent's virtual interface. This is the interface that will be monitored and/or driven.
  */
  function void set_vif( axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER vif);
      `xil_info(TAG,$sformatf("Assigning VIF to Driver"), XIL_AXI4STREAM_VERBOSITY_NONE)
      this.driver.set_vif(vif);
      `xil_info(TAG,$sformatf("Assigning VIF to Monitor"), XIL_AXI4STREAM_VERBOSITY_NONE)
      this.monitor.set_vif(vif);
  endfunction : set_vif

  /*
    Function: start_monitor
    Enables the monitor in this agent to start collecting data.
  */
  virtual task start_monitor();
    fork
      this.monitor.run_phase();
    join_none
  endtask : start_monitor

  /*
    Function: start_master
    Enables the monitor, driver in this agent to start collecting data.
    The driver will only issue transactions when the send functions are called.
  */
  virtual task start_master();
    fork
      this.start_monitor();
      this.driver.run_phase();
    join_none
    this.driver.wait_enabled();
    if(vif_proxy.m_vif.intf_is_master !=1) begin
      `xil_fatal(this.get_tag(),$sformatf("%m : Attempting to start master (%s) with non-MASTER virtual interface.", this.get_name()))
    end
  endtask : start_master

  /*
    Function: stop_master
    Disables the driver. Once disabled, no further action will occur by the driver.
  */
  virtual task stop_master();
    this.driver.stop_phase();
  endtask : stop_master

  /*
    Function: stop_monitor
    Disables the monitor in this agent from start collecting data. . Once disabled, no further action will occur by the monitor.
  */
  virtual task stop_monitor();
    this.monitor.stop_phase();
  endtask : stop_monitor

endclass : axi4stream_mst_agent

/*
  Class: axi4stream_slv_agent
  AXI4STREAM Slave agent.
*/
class axi4stream_slv_agent `XIL_AXI4STREAM_PARAM_DECL extends xil_agent;

  axi4stream_slv_driver `XIL_AXI4STREAM_PARAM_ORDER driver;
  axi4stream_monitor    `XIL_AXI4STREAM_PARAM_ORDER monitor;
  axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER vif_proxy;

   /*
    Function: new
    Constructor to create an AXI4STREAM Slave Agent.
  */
  function new (input string name = "unnamed_axi4stream_slv_agent",virtual interface axi4stream_vip_if `XIL_AXI4STREAM_PARAM_ORDER vif);
    super.new(name);
    this.monitor = new($sformatf("%s_monitor",name));
    this.driver = new($sformatf("%s_driver",name));
    this.vif_proxy = new($sformatf("%s_vif",name));
    this.vif_proxy.assign_vi(vif);
    this.set_vif(this.vif_proxy);
    this.set_tag("XIL_AXI4STREAM_SLV_AGENT");
  endfunction

  /*
   Function: set_verbosity
   Sets the verbosity of the Agent and all sub classes.
  */
  virtual function void set_verbosity(xil_verbosity updated);
    this.verbosity = updated;
    this.monitor.set_verbosity(this.get_verbosity);
    this.driver.set_verbosity(this.get_verbosity);
  endfunction : set_verbosity

  /*
    Function: set_vif
    Sets the Agent's virtual interface. This is the interface that will be monitored and/or driven.
  */
  function void set_vif( axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER vif);
    `xil_info(TAG,$sformatf("Assigning VIF to Driver"), XIL_AXI4STREAM_VERBOSITY_NONE)
    this.driver.set_vif(vif);
    `xil_info(TAG,$sformatf("Assigning VIF to Monitor"), XIL_AXI4STREAM_VERBOSITY_NONE)
    this.monitor.set_vif(vif);
  endfunction : set_vif

   /*
    Function: set_agent_tag
    Sets the tag of the Agent and all sub classes.
  */
  virtual function void set_agent_tag(string updated);
    this.TAG = updated;
    this.monitor.set_tag($sformatf("%s_monitor", this.get_tag()));
    this.driver.set_tag($sformatf("%s_driver", this.get_tag()));
    this.vif_proxy.set_tag($sformatf("%s_vif", this.get_tag()));
  endfunction : set_agent_tag

  /*
    Function: start_monitor
    Enables the monitor in this agent to start collecting data.
  */
  virtual task start_monitor();
    fork
      this.monitor.run_phase();
    join_none
  endtask : start_monitor

  /*
    Function: start_slave
    Enables the monitor,driver in this agent to start collecting data.
    The driver will only issue ready generation when the send functions are called.
  */
  virtual task start_slave();
    fork
      this.start_monitor();
      begin
        if(C_XIL_AXI4STREAM_SIGNAL_SET[XIL_AXI4STREAM_SIGSET_POS_READY] == 0) begin
          `xil_warning(this.get_tag(), $sformatf("Trying to start SLAVE component (%s) while HAS_TREADY is disabled. ", this.get_name()))
        end   
        this.driver.run_phase();      
      end
    join_none
    this.driver.wait_enabled();
    if(vif_proxy.m_vif.intf_is_slave !=1) begin
      `xil_fatal(this.get_tag(),$sformatf("%m : Attempting to start slave (%s) with non-SLAVE virtual interface.", this.get_name()))
    end
  endtask : start_slave

  /*
    Function: stop_slave
    Disables the driver. Once disabled, no further action will occur by the drivers.
  */
  virtual task stop_slave();
    this.driver.stop_phase();
  endtask : stop_slave

  /*
    Function: stop_monitor
    Disables the monitor in this agent from start collecting data. . Once disabled, no further action will occur by the monitor.
  */
  virtual task stop_monitor();
    this.monitor.stop_phase();
  endtask : stop_monitor

endclass : axi4stream_slv_agent

/*
  Class: axi4stream_passthrough_agent
  AXI4STREAM Passthrough Agent.
*/
class axi4stream_passthrough_agent `XIL_AXI4STREAM_PARAM_DECL extends xil_agent;

  axi4stream_mst_driver `XIL_AXI4STREAM_PARAM_ORDER mst_driver;
  axi4stream_slv_driver `XIL_AXI4STREAM_PARAM_ORDER slv_driver;
  axi4stream_monitor `XIL_AXI4STREAM_PARAM_ORDER monitor;
  axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER vif_proxy;

  /*
    Function: new
    Constructor to create an AXI4STREAM Passthrough Agent.
  */
  function new (input string name = "unnamed_axi4stream_passthrough_agent",virtual interface axi4stream_vip_if `XIL_AXI4STREAM_PARAM_ORDER vif);
    super.new(name);
    this.monitor = new($sformatf("%s_monitor",name));
    this.mst_driver = new($sformatf("%s_mst_driver",name));
    this.slv_driver = new($sformatf("%s_slv_driver",name));
    this.vif_proxy = new($sformatf("%s_vif",name));
    this.vif_proxy.assign_vi(vif);
    this.set_vif(this.vif_proxy);
    this.set_tag("XIL_AXI4STREAM_PASSTHROUGH_AGENT");
  endfunction

  /*
   Function: set_verbosity
   Sets the verbosity of the Agent and all sub classes.
  */
  virtual function void set_verbosity(xil_verbosity updated);
    this.verbosity = updated;
    this.monitor.set_verbosity(this.get_verbosity);
    this.mst_driver.set_verbosity(this.get_verbosity);
    this.slv_driver.set_verbosity(this.get_verbosity);
  endfunction : set_verbosity

  /*
    Function: set_vif
    Sets the Agent's virtual interface. This is the interface that will be monitored and/or driven.
  */
  function void set_vif( axi4stream_vif_proxy `XIL_AXI4STREAM_PARAM_ORDER vif);
    `xil_info(TAG,$sformatf("Set VIF in Passthrough mode"), XIL_AXI4STREAM_VERBOSITY_NONE)
    this.mst_driver.set_vif(vif);
    `xil_info(TAG,$sformatf("Assigning VIF to Passthrough VIP master write driver"), XIL_AXI4STREAM_VERBOSITY_NONE)
    this.slv_driver.set_vif(vif);
    `xil_info(TAG,$sformatf("Assigning VIF to Passthrough VIP slave write driver "), XIL_AXI4STREAM_VERBOSITY_NONE)
    this.monitor.set_vif(vif);
    `xil_info(this.get_tag(),$sformatf("Assigning VIF to Passthough VIP Monitor"), XIL_AXI4STREAM_VERBOSITY_NONE)
  endfunction : set_vif

   /*
    Function: set_agent_tag
    Sets the tag of the Agent and all sub classes.
  */
  virtual function void set_agent_tag(string updated);
    this.TAG = updated;
    this.monitor.set_tag($sformatf("%s_monitor", this.get_tag()));
    this.mst_driver.set_tag($sformatf("%s_mst_driver", this.get_tag()));
    this.slv_driver.set_tag($sformatf("%s_slv_driver", this.get_tag()));
    this.vif_proxy.set_tag($sformatf("%s_vif", this.get_tag()));
  endfunction : set_agent_tag

  /*
    Function: start_monitor
    Enables the monitor in this agent to start collecting data.
  */
  virtual task start_monitor();
    fork
      this.monitor.run_phase();
    join_none
  endtask : start_monitor

  /*
    Function: start_slave
    Enables the monitor and slave driver in this agent to start collecting data.
    The slave driver will only issue ready generation when the send functions are called.
  */
  virtual task start_slave();
    fork
      this.start_monitor();
      this.slv_driver.run_phase();
    join_none
    this.slv_driver.wait_enabled();
    if(vif_proxy.m_vif.intf_is_slave !=1) begin
      `xil_fatal(this.get_tag(),$sformatf("Attempting to assign non-SLAVE virtual interface to passthrough agent in runtime slave mode(%s). Correct operation requires the interface setting to match.Refer PG277 section about Useful Coding Guidelines and Example for how to swtich passthrough VIP into different runtime modes", this.get_name()))
    end
  endtask : start_slave

  /*
    Function: start_master
    Enables the monitor and master driver in this agent to start collecting data.
    The master driver will only issue transactions when the send functions are called.
  */
  virtual task start_master();
    fork
      this.start_monitor();
      this.mst_driver.run_phase();
    join_none
    this.mst_driver.wait_enabled();
    if(vif_proxy.m_vif.intf_is_master !=1) begin
      `xil_fatal(this.get_tag(),$sformatf("Attempting to assign non-MASTER virtual interface to passthrough agent in runtime master mode(%s). Correct operation requires the interface setting to match.Refer PG277 section about Useful Coding Guidelines and Example for how to swtich passthrough VIP into different runtime modes", this.get_name()))
    end 
  endtask : start_master

  /*
    Function: stop_master
    Disables the driver of the master. Once disabled, no further action will occur by the drivers.
  */
  virtual task stop_master();
    this.mst_driver.stop_phase();
  endtask  : stop_master

  /*
    Function: stop_slave
    Disables the driver of the slave. Once disabled, no further action will occur by the drivers.
  */
  virtual task stop_slave();
    this.slv_driver.stop_phase();
  endtask  : stop_slave

  /*
    Function: stop_monitor
    Disables the monitor in this agent from start collecting data. . Once disabled, no further action will occur by the monitor.
  */
  virtual task stop_monitor();
    this.monitor.stop_phase();
  endtask : stop_monitor

endclass : axi4stream_passthrough_agent

endpackage : axi4stream_vip_pkg
`endif

