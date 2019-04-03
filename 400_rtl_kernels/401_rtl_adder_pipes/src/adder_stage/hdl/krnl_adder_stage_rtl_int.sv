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
// Description: This is a example of a pipe to pipe rtl core.  Stream data 
// output is equal to stream data input + constant from control register. The
// calculation is pipelined for highest performance.
///////////////////////////////////////////////////////////////////////////////

// default_nettype of none prevents implicit wire declaration.
`default_nettype none
`timescale 1 ns / 1 ps 

module krnl_adder_stage_rtl_int #( 
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
///////////////////////////////////////////////////////////////////////////////
// Local Parameters (constants)
///////////////////////////////////////////////////////////////////////////////
localparam integer LP_LENGTH_WIDTH       = 32;
localparam integer LP_FIFO_DEPTH         = 32;

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
logic areset = 1'b0;  
logic ap_start;
logic ap_start_pulse;
logic ap_start_r;
logic ap_ready;
logic ap_done;
logic ap_idle = 1'b1;
logic [LP_LENGTH_WIDTH-1:0]         ctrl_length;
logic [C_DATA_WIDTH   -1:0]         ctrl_constant;
logic final_transfer;

logic wr_fifo_prog_full;      
logic p0_tready_reg = 1'b0;      

logic                    adder_tvalid;
logic [C_DATA_WIDTH-1:0] adder_tdata;
logic                    wr_fifo_tvalid_n;
logic                    wr_fifo_tready; 
logic [C_DATA_WIDTH-1:0] wr_fifo_tdata;

///////////////////////////////////////////////////////////////////////////////
// RTL Logic 
///////////////////////////////////////////////////////////////////////////////
// Register and invert reset signal for better timing.
always @(posedge ap_clk) begin 
  areset <= ~ap_rst_n; 
end

// create pulse when ap_start transitions to 1
always @(posedge ap_clk) begin 
  begin 
    ap_start_r <= ap_start;
  end
end

assign ap_start_pulse = ap_start & ~ap_start_r;

// ap_idle is asserted when done is asserted, it is de-asserted when ap_start_pulse 
// is asserted
always @(posedge ap_clk) begin 
  if (areset) begin 
    ap_idle <= 1'b1;
  end
  else begin 
    ap_idle <= ap_done        ? 1'b1 : 
               ap_start_pulse ? 1'b0 : 
                                ap_idle;
  end
end

assign ap_ready = ap_done;

krnl_adder_stage_rtl_counter #(
  .C_WIDTH ( LP_LENGTH_WIDTH ),
  .C_INIT  ( {LP_LENGTH_WIDTH{1'b0}} )
)
inst_transfer_cntr ( 
  .clk        ( ap_clk                 ) ,
  .clken      ( 1'b1                   ) ,
  .rst        ( areset                 ) ,
  .load       ( ap_start_pulse         ) ,
  .incr       ( 1'b0                   ) ,
  .decr       ( p1_TVALID & p1_TREADY  ) ,
  .load_value ( ctrl_length-1'b1       ) ,
  .count      (                        ) ,
  .is_zero    ( final_transfer         ) 
);

assign ap_done = p1_TVALID & p1_TREADY & final_transfer;

// AXI4-Lite slave
krnl_adder_stage_rtl_control_s_axi #(
  .C_S_AXI_ADDR_WIDTH( C_S_AXI_CONTROL_ADDR_WIDTH ),
  .C_S_AXI_DATA_WIDTH( C_S_AXI_CONTROL_DATA_WIDTH )
) 
inst_krnl_adder_stage_control_s_axi (
  .AWVALID     ( s_axi_control_AWVALID ) ,
  .AWREADY     ( s_axi_control_AWREADY ) ,
  .AWADDR      ( s_axi_control_AWADDR  ) ,
  .WVALID      ( s_axi_control_WVALID  ) ,
  .WREADY      ( s_axi_control_WREADY  ) ,
  .WDATA       ( s_axi_control_WDATA   ) ,
  .WSTRB       ( s_axi_control_WSTRB   ) ,
  .ARVALID     ( s_axi_control_ARVALID ) ,
  .ARREADY     ( s_axi_control_ARREADY ) ,
  .ARADDR      ( s_axi_control_ARADDR  ) ,
  .RVALID      ( s_axi_control_RVALID  ) ,
  .RREADY      ( s_axi_control_RREADY  ) ,
  .RDATA       ( s_axi_control_RDATA   ) ,
  .RRESP       ( s_axi_control_RRESP   ) ,
  .BVALID      ( s_axi_control_BVALID  ) ,
  .BREADY      ( s_axi_control_BREADY  ) ,
  .BRESP       ( s_axi_control_BRESP   ) ,
  .ACLK        ( ap_clk                ) ,
  .ARESET      ( areset                ) ,
  .ACLK_EN     ( 1'b1                  ) ,
  .ap_start    ( ap_start              ) ,
  .interrupt   (                       ) , // Not used
  .ap_ready    ( ap_ready              ) ,
  .ap_done     ( ap_done               ) ,
  .ap_idle     ( ap_idle               ) ,
  .inc         ( ctrl_constant         ) ,
  .size        ( ctrl_length           ) ,
  .p_xcl_gv_p0 (                       ) , // Not used
  .p_xcl_gv_p1 (                       )  // Not used
);

// Adder has 2 pipeline stages (2 cycle latency)
krnl_adder_stage_rtl_adder #(
  .C_DATA_WIDTH (C_DATA_WIDTH)
)
inst_adder (
  .aclk          ( ap_clk                ) ,
  .areset        ( areset                ) ,
  .ctrl_constant ( ctrl_constant         ) ,
  .s_tvalid      ( p0_TVALID & p0_TREADY ) ,
  .s_tdata       ( p0_TDATA              ) ,
  .m_tvalid      ( adder_tvalid          ) ,
  .m_tdata       ( adder_tdata           ) 
);

// xpm_fifo_sync: Synchronous FIFO
// Xilinx Parameterized Macro, Version 2016.4
xpm_fifo_sync # (
  .FIFO_MEMORY_TYPE          ("auto"),           //string; "auto", "block", "distributed", or "ultra";
  .ECC_MODE                  ("no_ecc"),         //string; "no_ecc" or "en_ecc";
  .FIFO_WRITE_DEPTH          (LP_FIFO_DEPTH),   //positive integer
  .WRITE_DATA_WIDTH          (C_DATA_WIDTH),               //positive integer
  .WR_DATA_COUNT_WIDTH       ($clog2(LP_FIFO_DEPTH+1)),               //positive integer, Not used
  .PROG_FULL_THRESH          (LP_FIFO_DEPTH-5),               //positive integer, 3 pipeline stages, 
  .FULL_RESET_VALUE          (1),                //positive integer; 0 or 1
  .READ_MODE                 ("fwft"),            //string; "std" or "fwft";
  .FIFO_READ_LATENCY         (1),                //positive integer;
  .READ_DATA_WIDTH           (C_DATA_WIDTH),               //positive integer
  .RD_DATA_COUNT_WIDTH       ($clog2(LP_FIFO_DEPTH+1)),               //positive integer, not used
  .PROG_EMPTY_THRESH         (10),               //positive integer, not used 
  .DOUT_RESET_VALUE          ("0"),              //string, don't care
  .WAKEUP_TIME               (0)                 //positive integer; 0 or 2;

) inst_xpm_fifo_sync (
  .sleep         ( 1'b0              ) ,
  .rst           ( areset            ) ,
  .wr_clk        ( ap_clk            ) ,
  .wr_en         ( adder_tvalid      ) ,
  .din           ( adder_tdata       ) ,
  .full          (                   ) ,
  .prog_full     ( wr_fifo_prog_full ) ,
  .wr_data_count (                   ) ,
  .overflow      (                   ) ,
  .wr_rst_busy   (                   ) ,
  .rd_en         ( wr_fifo_tready    ) ,
  .dout          ( wr_fifo_tdata     ) ,
  .empty         ( wr_fifo_tvalid_n  ) ,
  .prog_empty    (                   ) ,
  .rd_data_count (                   ) ,
  .underflow     (                   ) ,
  .rd_rst_busy   (                   ) ,
  .injectsbiterr ( 1'b0              ) ,
  .injectdbiterr ( 1'b0              ) ,
  .sbiterr       (                   ) ,
  .dbiterr       (                   ) 
);

// Don't accept data when idle when prog_full has been asserted.
always @(posedge ap_clk) begin 
  p0_tready_reg <= ~wr_fifo_prog_full & ~ap_idle;
end


assign p0_TREADY = p0_tready_reg;

assign p1_TVALID = ~wr_fifo_tvalid_n;
assign p1_TDATA  = wr_fifo_tdata; 
assign wr_fifo_tready = p1_TREADY;

endmodule : krnl_adder_stage_rtl_int

`default_nettype wire
