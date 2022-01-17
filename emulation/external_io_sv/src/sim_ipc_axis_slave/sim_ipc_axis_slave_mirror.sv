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
`timescale 1ns/1ps

`ifdef XILINX_SIMULATOR

`ifndef XILINX_SIMULATOR_BITASBOOL
`define XILINX_SIMULATOR_BITASBOOL
typedef bit bit_as_bool;
`endif

(* SC_MODULE_EXPORT *)
module sim_ipc_axis_slave_mirror (
  input  bit_as_bool  m_axis_aclk,
  input  bit_as_bool  m_axis_aresetn,
  output bit [0 : 0]  m_axis_tvalid,
  input  bit [0 : 0]  m_axis_tready,
  output bit [32-1 : 0] m_axis_tdata,
  output bit [4-1 : 0]  m_axis_tstrb,
  output bit [4-1 : 0]  m_axis_tkeep,
  output bit [0 : 0]  m_axis_tlast,
  output bit [32-1 : 0] m_axis_tuser
);
endmodule
`endif

`ifdef XCELIUM
(* XMSC_MODULE_EXPORT *)
module sim_ipc_axis_slave_mirror (m_axis_aclk, m_axis_aresetn, m_axis_tvalid, m_axis_tready, m_axis_tdata , m_axis_tstrb, m_axis_tkeep, m_axis_tlast, m_axis_tuser)
(* integer foreign = "SystemC";
*);
  input  bit  m_axis_aclk;
  input  bit  m_axis_aresetn;
  output bit [0 : 0]  m_axis_tvalid;
  input  bit [0 : 0]  m_axis_tready;
  output bit [32-1 : 0] m_axis_tdata;
  output bit [4-1 : 0]  m_axis_tstrb;
  output bit [4-1 : 0]  m_axis_tkeep;
  output bit [0 : 0]  m_axis_tlast;
  output bit [32-1 : 0] m_axis_tuser;
endmodule
`endif

`ifdef RIVIERA
(* SC_MODULE_EXPORT *)
module sim_ipc_axis_slave_mirror (m_axis_aclk, m_axis_aresetn, m_axis_tvalid, m_axis_tready, m_axis_tdata , m_axis_tstrb, m_axis_tkeep, m_axis_tlast, m_axis_tuser)
  input  bit  m_axis_aclk;
  input  bit  m_axis_aresetn;
  output bit [0 : 0]  m_axis_tvalid;
  input  bit [0 : 0]  m_axis_tready;
  output bit [32-1 : 0] m_axis_tdata;
  output bit [4-1 : 0]  m_axis_tstrb;
  output bit [4-1 : 0]  m_axis_tkeep;
  output bit [0 : 0]  m_axis_tlast;
  output bit [32-1 : 0] m_axis_tuser;
endmodule
`endif
