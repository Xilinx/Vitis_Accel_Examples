// /*******************************************************************************
// Copyright (c) 2018, Xilinx, Inc.
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

///////////////////////////////////////////////////////////////////////////////
// Description: This is a Verilog wrapper of module "krnl_adder_stage_rtl"
///////////////////////////////////////////////////////////////////////////////

// default_nettype of none prevents implicit wire declaration.
`default_nettype none
`timescale 1 ns / 1 ps 

module krnl_adder_stage_rtl #( 
  parameter integer  C_S_AXI_CONTROL_DATA_WIDTH = 32,
  parameter integer  C_S_AXI_CONTROL_ADDR_WIDTH = 6,
  parameter integer  C_DATA_WIDTH               = 32
)
(
  // System signals
  input  wire  ap_clk,
  input  wire  ap_rst_n,
  // AXI4 master interface 
  input  wire [C_DATA_WIDTH-1:0] p0_TDATA,
  input  wire        p0_TVALID,
  output wire        p0_TREADY,
  output wire [C_DATA_WIDTH-1:0] p1_TDATA,
  output wire        p1_TVALID,
  input  wire        p1_TREADY,

  // AXI4-Lite slave interface
  input  wire                                    s_axi_control_AWVALID,
  output wire                                    s_axi_control_AWREADY,
  input  wire [C_S_AXI_CONTROL_ADDR_WIDTH-1:0]   s_axi_control_AWADDR,
  input  wire                                    s_axi_control_WVALID,
  output wire                                    s_axi_control_WREADY,
  input  wire [C_S_AXI_CONTROL_DATA_WIDTH-1:0]   s_axi_control_WDATA,
  input  wire [C_S_AXI_CONTROL_DATA_WIDTH/8-1:0] s_axi_control_WSTRB,
  input  wire                                    s_axi_control_ARVALID,
  output wire                                    s_axi_control_ARREADY,
  input  wire [C_S_AXI_CONTROL_ADDR_WIDTH-1:0]   s_axi_control_ARADDR,
  output wire                                    s_axi_control_RVALID,
  input  wire                                    s_axi_control_RREADY,
  output wire [C_S_AXI_CONTROL_DATA_WIDTH-1:0]   s_axi_control_RDATA,
  output wire [1:0]                              s_axi_control_RRESP,
  output wire                                    s_axi_control_BVALID,
  input  wire                                    s_axi_control_BREADY,
  output wire [1:0]                              s_axi_control_BRESP 
);

krnl_adder_stage_rtl_int #(
  .C_S_AXI_CONTROL_DATA_WIDTH ( C_S_AXI_CONTROL_DATA_WIDTH ),
  .C_S_AXI_CONTROL_ADDR_WIDTH ( C_S_AXI_CONTROL_ADDR_WIDTH ),
  .C_DATA_WIDTH               ( C_DATA_WIDTH )
)
inst_krnl_adder_stage_rtl_int (
  .ap_clk                 ( ap_clk ),
  .ap_rst_n               ( ap_rst_n ),
  .p0_TDATA               ( p0_TDATA ),
  .p0_TVALID              ( p0_TVALID ),
  .p0_TREADY              ( p0_TREADY ),
  .p1_TDATA               ( p1_TDATA ),
  .p1_TVALID              ( p1_TVALID ),
  .p1_TREADY              ( p1_TREADY ),
  .s_axi_control_AWVALID  ( s_axi_control_AWVALID ),
  .s_axi_control_AWREADY  ( s_axi_control_AWREADY ),
  .s_axi_control_AWADDR   ( s_axi_control_AWADDR ),
  .s_axi_control_WVALID   ( s_axi_control_WVALID ),
  .s_axi_control_WREADY   ( s_axi_control_WREADY ),
  .s_axi_control_WDATA    ( s_axi_control_WDATA ),
  .s_axi_control_WSTRB    ( s_axi_control_WSTRB ),
  .s_axi_control_ARVALID  ( s_axi_control_ARVALID ),
  .s_axi_control_ARREADY  ( s_axi_control_ARREADY ),
  .s_axi_control_ARADDR   ( s_axi_control_ARADDR ),
  .s_axi_control_RVALID   ( s_axi_control_RVALID ),
  .s_axi_control_RREADY   ( s_axi_control_RREADY ),
  .s_axi_control_RDATA    ( s_axi_control_RDATA ),
  .s_axi_control_RRESP    ( s_axi_control_RRESP ),
  .s_axi_control_BVALID   ( s_axi_control_BVALID ),
  .s_axi_control_BREADY   ( s_axi_control_BREADY ),
  .s_axi_control_BRESP    ( s_axi_control_BRESP )
);

endmodule : krnl_adder_stage_rtl

`default_nettype wire
