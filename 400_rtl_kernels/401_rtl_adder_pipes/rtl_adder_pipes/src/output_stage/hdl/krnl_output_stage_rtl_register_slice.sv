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

//-----------------------------------------------------------------------------
// AXI protocol compliant register pipeline.  Isolates slave side from master 
// side with flops.
//-----------------------------------------------------------------------------
`default_nettype none

module krnl_output_stage_rtl_register_slice  #(
  parameter integer C_TDATA_WIDTH  = 32
)
( 
  input  wire                     aclk,
  input  wire                     areset,
  
  input  wire                     s_tvalid,
  output wire                     s_tready,
  input  wire [C_TDATA_WIDTH-1:0] s_tdata,

  output wire                     m_tvalid,
  input  wire                     m_tready,
  output wire [C_TDATA_WIDTH-1:0] m_tdata

);

timeunit 1ps;
timeprecision 1ps;

typedef enum logic [1:0] {RESET = 2'b00, IDLE = 2'b01, ACTIVE = 2'b11, STALL = 2'b10} e_state;
(* fsm_encoding = "none" *) e_state state = RESET;
logic [C_TDATA_WIDTH-1:0] primary_data;
logic [C_TDATA_WIDTH-1:0] secondary_data;


// Output values are encoded into state definition
assign m_tvalid = state[1];
assign s_tready = state[0];

assign m_tdata = primary_data;

always @(posedge aclk) begin 
  primary_data <= (state == IDLE) || (state == ACTIVE && m_tready) ? s_tdata : 
                  (state == STALL & m_tready) ? secondary_data : primary_data;
end

always @(posedge aclk) begin 
  secondary_data <= (state != STALL) ? s_tdata : secondary_data;
end

always @(posedge aclk) begin 
  if (areset) begin 
    state <= RESET; 
  end
  else begin 
    case (state)
      IDLE: 
        state <= s_tvalid ? ACTIVE : IDLE;
      ACTIVE: 
        state <= s_tvalid & ~m_tready ? STALL : 
                 ~s_tvalid & m_tready ? IDLE : ACTIVE;
      STALL:
        state <= m_tready ? ACTIVE : STALL; 
      default: 
        state <= IDLE;
    endcase
  end
end

endmodule : krnl_output_stage_rtl_register_slice
`default_nettype wire
