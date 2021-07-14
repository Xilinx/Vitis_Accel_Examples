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

////////////////////////////////////////////////////////////////////////////////
// Description: Basic Adder, no overflow. Unsigned. Pipelined.
////////////////////////////////////////////////////////////////////////////////

`default_nettype none

module krnl_adder_stage_rtl_adder #(
  parameter integer C_DATA_WIDTH   = 32 // Data width of both input and output data
)
(
  input wire                       aclk,
  input wire                       areset,

  input wire  [C_DATA_WIDTH-1:0]   ctrl_constant,

  input wire                       s_tvalid,
  input wire  [C_DATA_WIDTH-1:0]   s_tdata,
                                   
  output wire                      m_tvalid,
  output wire [C_DATA_WIDTH-1:0]   m_tdata

);

timeunit 1ps; 
timeprecision 1ps; 

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////
logic                    valid_in;
logic [C_DATA_WIDTH-1:0] data_in; 
logic [C_DATA_WIDTH-1:0] constant_in; 
logic                    valid_out;
logic [C_DATA_WIDTH-1:0] data_out; 

/////////////////////////////////////////////////////////////////////////////
// Adder Logic
/////////////////////////////////////////////////////////////////////////////
// Input stage
always @(posedge aclk) begin 
  data_in <= s_tdata; 
  constant_in <= ctrl_constant;
  valid_in <= s_tvalid;
end

// Output stage
always @(posedge aclk) begin 
  data_out <= data_in + constant_in;
  valid_out <= valid_in;
end

assign m_tvalid = valid_out;
assign m_tdata = data_out;

endmodule : krnl_adder_stage_rtl_adder

`default_nettype wire
