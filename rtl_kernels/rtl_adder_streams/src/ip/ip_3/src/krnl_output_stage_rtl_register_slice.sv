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
