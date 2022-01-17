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

`include "xil_common_vip_macros.svh"
package xil_common_vip_pkg;

parameter                          XIL_VERBOSITY_NONE =0;
parameter                          XIL_VERBOSITY_FULL =400;
parameter                          XIL_STREAMBITS = 4096;

// unsigned integer: xil_verbosity
// Specifies the policy for how much debug information to produce.
// XIL_VERBOSITY_NONE      - No information will be shown.
// XIL_VERBOSITY_FULL      - All information will be shown.

typedef integer unsigned                  xil_uint;
typedef xil_uint                          xil_verbosity;
typedef logic signed [XIL_STREAMBITS-1:0] xil_bitstream_t;
typedef logic signed [63:0]               xil_integral_t;

class xil_void;
endclass: xil_void

class xil_object extends xil_void;
  protected xil_verbosity   verbosity = XIL_VERBOSITY_NONE;
  protected bit             is_active = 0;
  protected string          TAG ="xil_object";
  string                    name = "unnamed_xil_object";
  local int                 m_inst_id;
  static protected int      m_inst_count;
  xil_object                object;
  bit                       cycle_check[xil_object];
  protected bit             stop_triggered_event = 0;
  
  function new (input string name = "unnamed_xil_object");
    this.name = name;
    m_inst_id = m_inst_count++;
    this.stop_triggered_event = 0;
    this.is_active = 0;
    this.set_tag(name);
  endfunction

// function get_name
  function string get_name();
    return(this.name);
  endfunction: get_name

// function get_full_name 
  function string get_full_name();
    return get_name();
  endfunction :get_full_name

// function set_name
  function void set_name(input string n);
    this.name = n;
  endfunction: set_name

//function get_inst_id
  function int get_inst_id();
    return m_inst_id;
  endfunction : get_inst_id

//function get_type_name
  virtual function string get_type_name ();
    return "<unknown>";
  endfunction : get_type_name

  virtual function bit do_compare(xil_object rhs);
    return(1);
  endfunction: do_compare

  /*
    Function: set_verbosity
    Sets the amount of debug information will be printed.
  */
  virtual function void set_verbosity(xil_verbosity updated);
    this.verbosity = updated;
  endfunction : set_verbosity

  /*
    Function: get_verbosity
    Returns the current value of the verbosity.
  */
  virtual function xil_verbosity get_verbosity();
    return(this.verbosity);
  endfunction : get_verbosity

  /*
    Function: set_tag
    Sets the name/TAG of the object
  */
  virtual function void set_tag(input string value);
    this.TAG = value;
  endfunction : set_tag

  /*
    Function: get_tag
    Gets the name/TAG of the object
  */
  virtual function string get_tag();
    return(this.TAG); 
  endfunction : get_tag

  /*
    Function: set_is_active
    Sets the active state of the object
  */
  virtual function void set_is_active();
    this.is_active = 1;
  endfunction : set_is_active

  /*
    Function: clr_is_active
    Clears the active state of the object
  */
  virtual function void clr_is_active();
    this.is_active = 0;
  endfunction : clr_is_active

  /*
    Function: get_is_active
    Returns the is_active value of the object. A value of 1 is considered active.
  */
  virtual function bit get_is_active();
    return(this.is_active); 
  endfunction : get_is_active
 
  /*
   Function: wait_enabled
   Wait until is_active is high
  */
  task wait_enabled();
    wait(this.is_active == 1);
  endtask : wait_enabled
endclass : xil_object

typedef class xil_component;

class xil_reporter extends xil_object;
  function new (input string name = "unnamed_xil_reporter");
    super.new(name);
    this.name = name;
  endfunction

  virtual function string get_type_name();
    return this.name;
  endfunction : get_type_name

endclass : xil_reporter

class xil_component extends xil_reporter;

  protected xil_component    m_children[string];
  protected xil_component    m_children_by_handle[xil_component];
  string                     type_name;
  bit                        print_enabled =1;

  function new(input string name = "unnamed_xil_component");
    super.new(name);
    this.type_name = name;
  endfunction : new
  
  // get_first_child
  function int get_first_child( string name);
    return m_children.first(name);
  endfunction

  // get_next_child
  function int get_next_child( string name);
    return m_children.next(name);
  endfunction

  // get_child
  function xil_component get_child(string name);
    if (m_children.exists(name))
      return m_children[name];
    `xil_warning("NOCHILD",{"Component with name '",name,
         "' is not a child of component '",get_full_name(),"'"})
    return null;
  endfunction

  virtual function string get_type_name();
    return this.type_name;
  endfunction 

endclass : xil_component

class xil_env extends xil_component;
  
  function new(input string name="unnamed_xil_env");
    super.new(name);
  endfunction : new
endclass : xil_env

class xil_agent extends xil_component;
  
  function new(input string name="unnamed_xil_agent");
    super.new(name);
  endfunction : new
endclass : xil_agent

class xil_sequence_item extends xil_object;
  function new(input string name="unnamed_xil_sequence_item");
    super.new(name);
  endfunction : new

  function void set_id_info(xil_object p);
  endfunction : set_id_info

  virtual function string get_type_name();
    return "xil_sequence_item";
  endfunction : get_type_name

  virtual function string convert2string();
    return("");
  endfunction : convert2string

endclass: xil_sequence_item

class xil_analysis_port #(type T = xil_sequence_item) extends xil_component;
  protected logic             enabled = 0;
  protected integer unsigned  item_cnt =0;
  T                           q[$];
  event                       item_cnt_event ; 

  function new(input string name="unnamed_xil_analysis_port");
    super.new(name);
  endfunction : new

  function void set_enabled();
    `xil_info(this.get_name(), "Enabled xil_analysis_port for listening.", XIL_VERBOSITY_NONE)
    this.enabled = 1;
  endfunction : set_enabled

  function void clr_enabled();
    `xil_info(this.get_name(), "Disabled xil_analysis_port for listening. No more transactions will be returned on this port.", XIL_VERBOSITY_NONE)
    this.enabled = 0;
  endfunction : clr_enabled

  function logic get_enabled();
    return(this.enabled);
  endfunction : get_enabled

  function integer unsigned get_item_cnt();
    return(this.item_cnt);
  endfunction : get_item_cnt

  virtual task write(input T trans);
    if (this.enabled) begin
      q.push_back(trans);
      this.item_cnt++;
      ->item_cnt_event ;
    end
  endtask: write

  virtual task get(output T trans);
    if (this.enabled == 1) begin
      while (item_cnt ==0) begin
        @(item_cnt_event);
      end  
      this.item_cnt--;
      ->item_cnt_event ;
      trans = q.pop_front();
    end else begin
      `xil_fatal(this.get_name(), "Attempted to GET object from a disabled analysis_port. Enable the analysis port using set_enabled first.")
    end
  endtask : get

endclass : xil_analysis_port

class xil_sqr_if_base #(type T1=xil_object, T2=T1);
  protected string              name  = "xil_sqr_if_base";
  protected integer unsigned    item_cnt = 0;
  protected integer unsigned    item_inflight_cnt = 0;
  protected integer unsigned    item_done_cnt = 0;
  protected integer unsigned    rsp_cnt = 0;
  protected integer unsigned    rsp_inflight_cnt = 0;
  protected integer unsigned    rsp_done_cnt = 0;
  protected integer unsigned    max_item_cnt = 25;
  T1                            item_q[$];
  T2                            rsp_q[$];
  event                         item_cnt_event ; 
  event                         rsp_cnt_event ; 
  event                         done_item_cnt_event ;
  event                         done_rsp_cnt_event ;

  /*
   Function: set_name
   Change the name of the object.
  */
  function void set_name(input string in);
    this.name = in;
  endfunction : set_name

  /*
   Function: get_name
   Return the name of the object.
  */
  function string get_name();
    return(this.name);
  endfunction : get_name

  /*
   Function: set_max_item_cnt
   Change the number of concurrent connections to the same inbound/output sequence port.
  */
  function void set_max_item_cnt(input integer unsigned in);
    if (in == 0) begin
      `xil_error(this.get_name(), "Attempted to set max_item_cnt to 0 value. Only non-zero values are allowed. No Action taken.")
    end else begin
      this.max_item_cnt = in;
    end
  endfunction : set_max_item_cnt

  /*
   Function: get_max_item_cnt
   Return the number of concurrent connections to the same inbound/output sequence port.
  */
  function integer unsigned get_max_item_cnt();
    return(this.max_item_cnt);
  endfunction : get_max_item_cnt

  virtual task get_next_item(output T1 t);
    while (item_cnt == 0) begin
      @(item_cnt_event);
    end
    this.item_cnt--;
    ->item_cnt_event ;
    t = this.item_q.pop_front();
    this.item_inflight_cnt++;
  endtask

  virtual function void try_next_item(output T1 t);
    if (item_cnt == 0) begin
      t = null;
    end else begin
      this.item_cnt--;
      ->item_cnt_event ;
      t = this.item_q.pop_front();
      this.item_inflight_cnt++;
    end
  endfunction : try_next_item

  virtual task wait_for_item_done(int transaction_id = -1);
    while (this.item_done_cnt == 0) begin
      @(done_item_cnt_event);
    end
    this.item_done_cnt--;
    ->done_item_cnt_event;
  endtask : wait_for_item_done

  virtual function xil_uint item_done(input T1 t = null);
    if (this.item_inflight_cnt == 0) begin
      `xil_error(this.get_name(), "Attempted to double pop the item_done queue. Check the interaction with your analysis port.")
      return(1);
    end else begin
      this.item_done_cnt++;
      ->done_item_cnt_event ;
      this.item_inflight_cnt--;
      return(0);
    end
  endfunction

  virtual function xil_uint put_item(input T1 t = null);
    if (this.item_cnt > max_item_cnt) begin
      `xil_error(this.get_name(), "The sequence FIFO has overfilled. The item was not accepted.")
      if (t != null) begin
        `xil_error(this.get_name(), $sformatf("%s", t.convert2string()))
      end
      `xil_error(this.get_name(), $sformatf("The current sequence depth is (%d). To change the depth of inbound item port, use set_max_item_cnt to change the value.", this.get_max_item_cnt()))
      return(1);
    end else begin
      this.item_q.push_back(t);
      this.item_cnt++;
      ->item_cnt_event ;
      return(0);
    end
  endfunction

  virtual task get_next_rsp(output T2 t);
    while (rsp_cnt == 0) begin
      @(rsp_cnt_event);
    end
    this.rsp_cnt--;
    ->rsp_cnt_event ;
    t = this.rsp_q.pop_front();
    this.rsp_inflight_cnt++;
  endtask

  virtual function void try_next_rsp(output T2 t);
    if (rsp_cnt == 0) begin
      t = null;
    end else begin
      this.rsp_cnt--;
      ->rsp_cnt_event ;
      t = this.rsp_q.pop_front();
      this.rsp_inflight_cnt++;
    end
  endfunction : try_next_rsp

  virtual task wait_for_rsp_done(int transaction_id = -1);
    while (this.rsp_done_cnt == 0) begin
      @(done_rsp_cnt_event);
    end
    this.rsp_done_cnt--;
    ->done_rsp_cnt_event;
  endtask : wait_for_rsp_done

  virtual function xil_uint rsp_done(input T2 t = null);
    if (this.rsp_inflight_cnt == 0) begin
      `xil_error(this.get_name(), "Attempted to double pop the rsp_done queue. Check the interaction with your analysis port.")
      return(1);
    end else begin
      this.rsp_done_cnt++;
      ->done_rsp_cnt_event ;
      this.rsp_inflight_cnt--;
      return(0);
    end
  endfunction

  virtual function xil_uint put_rsp(input T2 t = null);
    if (this.rsp_cnt > max_item_cnt) begin
      `xil_error(this.get_name(), "The response FIFO has overfilled. The Response was not accepted.")
      if (t != null) begin
        `xil_error(this.get_name(), $sformatf("%s", t.convert2string()))
      end
      `xil_error(this.get_name(), $sformatf("The current sequence depth is (%d). To change the depth of inbound rsp_done port, use set_max_item_cnt to change the value.", this.get_max_item_cnt()))
      return(1);
    end else begin
      this.rsp_q.push_back(t);
      this.rsp_cnt++;
      ->rsp_cnt_event ;
      return(0);
    end
  endfunction

endclass : xil_sqr_if_base

class xil_seq_item_pull_port#(type REQ = xil_sequence_item, type RSP = REQ) extends xil_sqr_if_base#(REQ, RSP);

  function new(input string name="unnamed_xil_seq_item_pull_port");
    this.name = name;
  endfunction : new

endclass :xil_seq_item_pull_port


class xil_driver #(type REQ = xil_sequence_item, type RSP = REQ) extends xil_component;

  // Port: seq_item_port
  //
  // This port provides the method to sending transactions to the driver.
  xil_seq_item_pull_port #(REQ, RSP) seq_item_port;

  // Function: new
  //
  // Creates and initializes an instance of this class using the normal
  // constructor arguments for <xil_component>: ~name~ is the name of the
  // instance, and ~parent~ is the handle to the hierarchical parent, if any.

  function new (string name = "unnamed_xil_driver");
    super.new(name);
    seq_item_port    = new("seq_item_port");
  endfunction : new

  const static string type_name = "xil_driver #(REQ,RSP)";

  virtual function string get_type_name ();
    return type_name;
  endfunction
endclass :xil_driver

class xil_monitor extends xil_component;

  function new(input string name="unnamed_xil_monitor");
    super.new(name);
  endfunction : new

endclass : xil_monitor

endpackage : xil_common_vip_pkg
