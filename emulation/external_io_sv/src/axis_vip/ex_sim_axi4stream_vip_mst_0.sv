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


// IP VLNV: xilinx.com:ip:axi4stream_vip:1.1
// IP Revision: 9

`timescale 1ns/1ps

(* DowngradeIPIdentifiedWarnings = "yes" *)
module ex_sim_axi4stream_vip_mst_0 (
  aclk,
  aresetn,
  m_axis_tvalid,
  m_axis_tready,
  m_axis_tdata,
  m_axis_tlast
);

(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME CLOCK, ASSOCIATED_BUSIF S_AXIS:M_AXIS, ASSOCIATED_RESET ARESETN, FREQ_HZ 300000000, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN design_sim_clk_wiz_0_0_clk_out1, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 CLOCK CLK" *)
input wire aclk;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME RESET, POLARITY ACTIVE_LOW, INSERT_VIP 0, TYPE INTERCONNECT" *)
(* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 RESET RST" *)
input wire aresetn;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS TVALID" *)
output wire [0 : 0] m_axis_tvalid;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS TREADY" *)
input wire [0 : 0] m_axis_tready;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS TDATA" *)
output wire [31 : 0] m_axis_tdata;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME M_AXIS, TDATA_NUM_BYTES 4, TDEST_WIDTH 0, TID_WIDTH 0, TUSER_WIDTH 0, HAS_TREADY 1, HAS_TSTRB 0, HAS_TKEEP 0, HAS_TLAST 1, FREQ_HZ 300000000, PHASE 0.0, CLK_DOMAIN design_sim_clk_wiz_0_0_clk_out1, LAYERED_METADATA undef, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS TLAST" *)
output wire [0 : 0] m_axis_tlast;

  axi4stream_vip_v1_1_9_top #(
    .C_AXI4STREAM_SIGNAL_SET(32'B00000000000000000000000000010011),
    .C_AXI4STREAM_INTERFACE_MODE(0),
    .C_AXI4STREAM_DATA_WIDTH(32),
    .C_AXI4STREAM_USER_BITS_PER_BYTE(0),
    .C_AXI4STREAM_ID_WIDTH(0),
    .C_AXI4STREAM_DEST_WIDTH(0),
    .C_AXI4STREAM_USER_WIDTH(0),
    .C_AXI4STREAM_HAS_ARESETN(1)
  ) inst (
    .aclk(aclk),
    .aresetn(aresetn),
    .aclken(1'B1),
    .s_axis_tvalid(1'B0),
    .s_axis_tready(),
    .s_axis_tdata(32'B0),
    .s_axis_tstrb(4'B0),
    .s_axis_tkeep(4'B0),
    .s_axis_tlast(1'B0),
    .s_axis_tid(1'B0),
    .s_axis_tdest(1'B0),
    .s_axis_tuser(1'B0),
    .m_axis_tvalid(m_axis_tvalid),
    .m_axis_tready(m_axis_tready),
    .m_axis_tdata(m_axis_tdata),
    .m_axis_tstrb(),
    .m_axis_tkeep(),
    .m_axis_tlast(m_axis_tlast),
    .m_axis_tid(),
    .m_axis_tdest(),
    .m_axis_tuser()
  );
endmodule
