// /*******************************************************************************
// Copyright (c) 2019, Xilinx, Inc.
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// 
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
// 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *******************************************************************************/

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
