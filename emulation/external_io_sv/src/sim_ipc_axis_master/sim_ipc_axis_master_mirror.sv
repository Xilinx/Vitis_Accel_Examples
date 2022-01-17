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
module sim_ipc_axis_master_mirror (
  input  bit_as_bool  s_axis_aclk,
  input  bit_as_bool  s_axis_aresetn,
  input  bit [0 : 0]  s_axis_tvalid,
  output bit [0 : 0]  s_axis_tready,
  input  bit [32-1 : 0] s_axis_tdata,
  input  bit [4-1 : 0]  s_axis_tstrb,
  input  bit [4-1 : 0]  s_axis_tkeep,
  input  bit [0 : 0]  s_axis_tlast,
  input  bit [32-1 : 0] s_axis_tuser
);
endmodule
`endif

`ifdef XCELIUM
(* XMSC_MODULE_EXPORT *)
module sim_ipc_axis_master_mirror (s_axis_aclk, s_axis_aresetn, s_axis_tvalid, s_axis_tready, s_axis_tdata, s_axis_tstrb, s_axis_tkeep, s_axis_tlast, s_axis_tuser)
(* integer foreign = "SystemC";
*);
  input  bit  s_axis_aclk;
  input  bit  s_axis_aresetn;
  input  bit [0 : 0]  s_axis_tvalid;
  output bit [0 : 0]  s_axis_tready;
  input  bit [32-1 : 0] s_axis_tdata;
  input  bit [4-1 : 0]  s_axis_tstrb;
  input  bit [4-1 : 0]  s_axis_tkeep;
  input  bit [0 : 0]  s_axis_tlast;
  input  bit [32-1 : 0] s_axis_tuser;
endmodule
`endif

`ifdef RIVIERA
(* SC_MODULE_EXPORT *)
module sim_ipc_axis_master_mirror (s_axis_aclk, s_axis_aresetn, s_axis_tvalid, s_axis_tready, s_axis_tdata, s_axis_tstrb, s_axis_tkeep, s_axis_tlast, s_axis_tuser)
  input  bit  s_axis_aclk;
  input  bit  s_axis_aresetn;
  input  bit [0 : 0]  s_axis_tvalid;
  output bit [0 : 0]  s_axis_tready;
  input  bit [32-1 : 0] s_axis_tdata;
  input  bit [4-1 : 0]  s_axis_tstrb;
  input  bit [4-1 : 0]  s_axis_tkeep;
  input  bit [0 : 0]  s_axis_tlast;
  input  bit [32-1 : 0] s_axis_tuser;
endmodule
`endif
