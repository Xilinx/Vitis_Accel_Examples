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

`timescale 1 ps / 1 ps

module des(input wire axis_aclk, input wire axis_aresetn);
 
  wire [31:0] m_axis_tdata;
  wire [0:0]  m_axis_tlast;
  wire [0:0]  m_axis_tready;
  wire [0:0]  m_axis_tvalid;

  wire [31:0] s_axis_tdata;
  wire [0:0]  s_axis_tlast;
  wire [0:0]  s_axis_tready;
  wire [0:0]  s_axis_tvalid;

  ex_sim_axi4stream_vip_mst_0 axi4stream_vip_mst
       (.aclk(axis_aclk),
        .aresetn(axis_aresetn),
        .m_axis_tdata(m_axis_tdata),
        .m_axis_tlast(m_axis_tlast),
        .m_axis_tready(m_axis_tready),
        .m_axis_tvalid(m_axis_tvalid));

  sim_ipc_axis_master_mirror gt_master
    (.s_axis_aclk(axis_aclk),
     .s_axis_aresetn(axis_aresetn),
     .s_axis_tvalid(m_axis_tvalid),
     .s_axis_tready(m_axis_tready),
     .s_axis_tdata(m_axis_tdata),
     .s_axis_tstrb({1'b1,1'b1,1'b1,1'b1}),
     .s_axis_tkeep({1'b1,1'b1,1'b1,1'b1}),
     .s_axis_tlast(m_axis_tlast),
     .s_axis_tuser(1'b1)
    );
    
  ex_sim_axi4stream_vip_slv_0 axi4stream_vip_slv
       (.aclk(axis_aclk),
        .aresetn(axis_aresetn),
        .s_axis_tdata(s_axis_tdata),
        .s_axis_tlast(s_axis_tlast),
        .s_axis_tready(s_axis_tready),
        .s_axis_tvalid(s_axis_tvalid));

  sim_ipc_axis_slave_mirror gt_slave
    (.m_axis_aclk(axis_aclk),
     .m_axis_aresetn(axis_aresetn),
     .m_axis_tdata(s_axis_tdata),
     .m_axis_tlast(s_axis_tlast),
     .m_axis_tready(s_axis_tready),
     .m_axis_tvalid(s_axis_tvalid));

endmodule
