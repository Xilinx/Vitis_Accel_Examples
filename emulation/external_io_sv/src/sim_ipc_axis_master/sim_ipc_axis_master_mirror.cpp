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
#include "sim_ipc_axis_master_mirror.h"

#if defined XILINX_SIMULATOR || defined XM_SYSTEMC || defined RIVIERA
sim_ipc_axis_master_mirror::sim_ipc_axis_master_mirror(const sc_core::sc_module_name& nm)
    : sc_core::sc_module(nm),
      mp_impl(NULL),
      s_axis_aclk("s_axis_aclk"),
      s_axis_aresetn("s_axis_aresetn"),
      s_axis_tvalid("s_axis_tvalid"),
      s_axis_tready("s_axis_tready"),
      s_axis_tdata("s_axis_tdata"),
      s_axis_tstrb("s_axis_tstrb"),
      s_axis_tkeep("s_axis_tkeep"),
      s_axis_tlast("s_axis_tlast"),
      s_axis_tuser("s_axis_tuser") {
    // initialize module
    xsc::common_cpp::properties model_param_props;
    model_param_props.addLong("C_S00_AXIS_TDATA_WIDTH", "32");
    mp_impl = new sim_ipc_axis_slave("inst", model_param_props);

    // initialize pins
    mp_impl->s00_axis_aclk(s_axis_aclk);
    mp_impl->s00_axis_aresetn(s_axis_aresetn);

    // initialize transactors
    mp_S_AXIS_transactor = NULL;
    mp_s_axis_tlast_converter = NULL;
    mp_s_axis_tready_converter = NULL;
    mp_s_axis_tvalid_converter = NULL;

    // initialize AXI sockets
    S_AXIS_socket = mp_impl->S00_AXIS_socket;
}

void sim_ipc_axis_master_mirror::before_end_of_elaboration() {
    // configure 'S_AXIS' transactor

    // 'S_AXIS' transactor parameters
    xsc::common_cpp::properties S_AXIS_transactor_param_props;
    S_AXIS_transactor_param_props.addLong("TDATA_NUM_BYTES", "4");
    S_AXIS_transactor_param_props.addLong("TDEST_WIDTH", "0");
    S_AXIS_transactor_param_props.addLong("TID_WIDTH", "0");
    S_AXIS_transactor_param_props.addLong("TUSER_WIDTH", "32");
    S_AXIS_transactor_param_props.addLong("HAS_TREADY", "1");
    S_AXIS_transactor_param_props.addLong("HAS_TSTRB", "1");
    S_AXIS_transactor_param_props.addLong("HAS_TKEEP", "1");
    S_AXIS_transactor_param_props.addLong("HAS_TLAST", "1");
    S_AXIS_transactor_param_props.addLong("FREQ_HZ", "150000000");
    S_AXIS_transactor_param_props.addLong("HAS_RESET", "1");
    S_AXIS_transactor_param_props.addFloat("PHASE", "0.000");
    S_AXIS_transactor_param_props.addString("CLK_DOMAIN", "xilinx_zcu102_base_clk_wiz_0_0_clk_out1");
    S_AXIS_transactor_param_props.addString("LAYERED_METADATA", "undef");
    S_AXIS_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    S_AXIS_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_S_AXIS_transactor =
        new xtlm::xaxis_pin2xtlm_t<4, 32, 1, 1, 1, 1>("S_AXIS_transactor", S_AXIS_transactor_param_props);

    // S_AXIS' transactor ports
    mp_S_AXIS_transactor->CLK(s_axis_aclk);
    mp_S_AXIS_transactor->RST(s_axis_aresetn);

    mp_S_AXIS_transactor->TDATA(s_axis_tdata);
    mp_S_AXIS_transactor->TUSER(s_axis_tuser);

    mp_s_axis_tlast_converter = new xsc::common::vectorN2scalar_converter<1>("s_axis_tlast_converter");
    mp_s_axis_tlast_converter->vector_in(s_axis_tlast);
    mp_s_axis_tlast_converter->scalar_out(m_s_axis_tlast_converter_signal);
    mp_S_AXIS_transactor->TLAST(m_s_axis_tlast_converter_signal);

    mp_s_axis_tready_converter = new xsc::common::scalar2vectorN_converter<1>("s_axis_tready_converter");
    mp_s_axis_tready_converter->scalar_in(m_s_axis_tready_converter_signal);
    mp_s_axis_tready_converter->vector_out(s_axis_tready);
    mp_S_AXIS_transactor->TREADY(m_s_axis_tready_converter_signal);

    mp_S_AXIS_transactor->TSTRB(s_axis_tstrb);
    mp_S_AXIS_transactor->TKEEP(s_axis_tkeep);

    mp_s_axis_tvalid_converter = new xsc::common::vectorN2scalar_converter<1>("s_axis_tvalid_converter");
    mp_s_axis_tvalid_converter->vector_in(s_axis_tvalid);
    mp_s_axis_tvalid_converter->scalar_out(m_s_axis_tvalid_converter_signal);
    mp_S_AXIS_transactor->TVALID(m_s_axis_tvalid_converter_signal);

    // S_AXIS' transactor sockets
    mp_impl->S00_AXIS_socket->bind(*(mp_S_AXIS_transactor->socket));
}
#endif

#if defined VCSSYSTEMC || defined MTI_SYSTEMC
sim_ipc_axis_master_mirror::sim_ipc_axis_master_mirror(const sc_core::sc_module_name& nm)
    : sc_core::sc_module(nm),
      mp_impl(NULL),
      s_axis_aclk("s_axis_aclk"),
      s_axis_aresetn("s_axis_aresetn"),
      s_axis_tvalid("s_axis_tvalid"),
      s_axis_tready("s_axis_tready"),
      s_axis_tdata("s_axis_tdata"),
      s_axis_tstrb("s_axis_tstrb"),
      s_axis_tkeep("s_axis_tkeep"),
      s_axis_tlast("s_axis_tlast"),
      s_axis_tuser("s_axis_tuser") {
    // initialize module
    xsc::common_cpp::properties model_param_props;
    model_param_props.addLong("C_S00_AXIS_TDATA_WIDTH", "32");
    mp_impl = new sim_ipc_axis_slave("inst", model_param_props);

    // initialize pins
    mp_impl->s00_axis_aclk(s_axis_aclk);
    mp_impl->s00_axis_aresetn(s_axis_aresetn);

    // initialize transactors
    mp_S_AXIS_transactor = NULL;
    mp_s_axis_tlast_converter = NULL;
    mp_s_axis_tready_converter = NULL;
    mp_s_axis_tvalid_converter = NULL;

    // initialize AXI sockets
    S_AXIS_socket = mp_impl->S00_AXIS_socket;

    // configure 'S_AXIS' transactor

    // 'S_AXIS' transactor parameters
    xsc::common_cpp::properties S_AXIS_transactor_param_props;
    S_AXIS_transactor_param_props.addLong("TDATA_NUM_BYTES", "4");
    S_AXIS_transactor_param_props.addLong("TDEST_WIDTH", "0");
    S_AXIS_transactor_param_props.addLong("TID_WIDTH", "0");
    S_AXIS_transactor_param_props.addLong("TUSER_WIDTH", "32");
    S_AXIS_transactor_param_props.addLong("HAS_TREADY", "1");
    S_AXIS_transactor_param_props.addLong("HAS_TSTRB", "1");
    S_AXIS_transactor_param_props.addLong("HAS_TKEEP", "1");
    S_AXIS_transactor_param_props.addLong("HAS_TLAST", "1");
    S_AXIS_transactor_param_props.addLong("FREQ_HZ", "150000000");
    S_AXIS_transactor_param_props.addLong("HAS_RESET", "1");
    S_AXIS_transactor_param_props.addFloat("PHASE", "0.000");
    S_AXIS_transactor_param_props.addString("CLK_DOMAIN", "xilinx_zcu102_base_clk_wiz_0_0_clk_out1");
    S_AXIS_transactor_param_props.addString("LAYERED_METADATA", "undef");
    S_AXIS_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    S_AXIS_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_S_AXIS_transactor =
        new xtlm::xaxis_pin2xtlm_t<4, 32, 1, 1, 1, 1>("S_AXIS_transactor", S_AXIS_transactor_param_props);

    // S_AXIS' transactor ports
    mp_S_AXIS_transactor->CLK(s_axis_aclk);
    mp_S_AXIS_transactor->RST(s_axis_aresetn);

    mp_S_AXIS_transactor->TDATA(s_axis_tdata);
    mp_S_AXIS_transactor->TUSER(s_axis_tuser);

    mp_s_axis_tlast_converter = new xsc::common::vectorN2scalar_converter<1>("s_axis_tlast_converter");
    mp_s_axis_tlast_converter->vector_in(s_axis_tlast);
    mp_s_axis_tlast_converter->scalar_out(m_s_axis_tlast_converter_signal);
    mp_S_AXIS_transactor->TLAST(m_s_axis_tlast_converter_signal);

    mp_s_axis_tready_converter = new xsc::common::scalar2vectorN_converter<1>("s_axis_tready_converter");
    mp_s_axis_tready_converter->scalar_in(m_s_axis_tready_converter_signal);
    mp_s_axis_tready_converter->vector_out(s_axis_tready);
    mp_S_AXIS_transactor->TREADY(m_s_axis_tready_converter_signal);

    mp_S_AXIS_transactor->TSTRB(s_axis_tstrb);
    mp_S_AXIS_transactor->TKEEP(s_axis_tkeep);

    mp_s_axis_tvalid_converter = new xsc::common::vectorN2scalar_converter<1>("s_axis_tvalid_converter");
    mp_s_axis_tvalid_converter->vector_in(s_axis_tvalid);
    mp_s_axis_tvalid_converter->scalar_out(m_s_axis_tvalid_converter_signal);
    mp_S_AXIS_transactor->TVALID(m_s_axis_tvalid_converter_signal);
}

void sim_ipc_axis_master_mirror::before_end_of_elaboration() {
    // S_AXIS' transactor sockets
    mp_impl->S00_AXIS_socket->bind(*(mp_S_AXIS_transactor->socket));
}

#endif

sim_ipc_axis_master_mirror::~sim_ipc_axis_master_mirror() {
    delete mp_S_AXIS_transactor;
    delete mp_s_axis_tlast_converter;
    delete mp_s_axis_tready_converter;
    delete mp_s_axis_tvalid_converter;
    delete mp_impl;
}

#ifdef MTI_SYSTEMC
SC_MODULE_EXPORT(sim_ipc_axis_master_mirror);
#endif

#ifdef XM_SYSTEMC
XMSC_MODULE_EXPORT(sim_ipc_axis_master_mirror);
#endif

#ifdef RIVIERA
SC_MODULE_EXPORT(sim_ipc_axis_master_mirror);
#endif
