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

#include "sim_ipc_axis_slave_mirror.h"

#if defined(XILINX_SIMULATOR) || defined(XM_SYSTEMC) || defined(RIVIERA)
sim_ipc_axis_slave_mirror::sim_ipc_axis_slave_mirror(const sc_core::sc_module_name& nm)
    : sc_core::sc_module(nm),
      mp_impl(NULL),
      m_axis_aclk("m_axis_aclk"),
      m_axis_aresetn("m_axis_aresetn"),
      m_axis_tvalid("m_axis_tvalid"),
      m_axis_tready("m_axis_tready"),
      m_axis_tdata("m_axis_tdata"),
      m_axis_tstrb("m_axis_tstrb"),
      m_axis_tkeep("m_axis_tkeep"),
      m_axis_tlast("m_axis_tlast"),
      m_axis_tuser("m_axis_tuser") {
    // initialize module
    xsc::common_cpp::properties model_param_props;
    model_param_props.addLong("C_M00_AXIS_TDATA_WIDTH", "32");
    mp_impl = new sim_ipc_axis_master("inst", model_param_props);

    // initialize pins
    mp_impl->m00_axis_aclk(m_axis_aclk);
    mp_impl->m00_axis_aresetn(m_axis_aresetn);

    // initialize AXI sockets
    M_AXIS_socket = mp_impl->M00_AXIS_socket;

    // initialize transactors
    mp_M_AXIS_transactor = NULL;
    mp_m_axis_tlast_converter = NULL;
    mp_m_axis_tready_converter = NULL;
    mp_m_axis_tvalid_converter = NULL;
}

void sim_ipc_axis_slave_mirror::before_end_of_elaboration() {
    // configure 'M_AXIS' transactor

    // 'M_AXIS' transactor parameters
    xsc::common_cpp::properties M_AXIS_transactor_param_props;
    M_AXIS_transactor_param_props.addLong("TDATA_NUM_BYTES", "4");
    M_AXIS_transactor_param_props.addLong("TDEST_WIDTH", "0");
    M_AXIS_transactor_param_props.addLong("TID_WIDTH", "0");
    M_AXIS_transactor_param_props.addLong("TUSER_WIDTH", "32");
    M_AXIS_transactor_param_props.addLong("HAS_TREADY", "1");
    M_AXIS_transactor_param_props.addLong("HAS_TSTRB", "1");
    M_AXIS_transactor_param_props.addLong("HAS_TKEEP", "1");
    M_AXIS_transactor_param_props.addLong("HAS_TLAST", "1");
    M_AXIS_transactor_param_props.addLong("FREQ_HZ", "150000000");
    M_AXIS_transactor_param_props.addLong("HAS_RESET", "1");
    M_AXIS_transactor_param_props.addFloat("PHASE", "0.000");
    M_AXIS_transactor_param_props.addString("CLK_DOMAIN", "xilinx_zcu102_base_clk_wiz_0_0_clk_out1");
    M_AXIS_transactor_param_props.addString("LAYERED_METADATA", "undef");
    M_AXIS_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    M_AXIS_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_M_AXIS_transactor =
        new xtlm::xaxis_xtlm2pin_t<4, 32, 1, 1, 1, 1>("M_AXIS_transactor", M_AXIS_transactor_param_props);

    // M_AXIS' transactor ports
    mp_M_AXIS_transactor->CLK(m_axis_aclk);
    mp_M_AXIS_transactor->RST(m_axis_aresetn);

    mp_M_AXIS_transactor->TDATA(m_axis_tdata);
    mp_M_AXIS_transactor->TUSER(m_axis_tuser);

    mp_m_axis_tlast_converter = new xsc::common::scalar2vectorN_converter<1>("m_axis_tlast_converter");
    mp_m_axis_tlast_converter->scalar_in(m_m_axis_tlast_converter_signal);
    mp_m_axis_tlast_converter->vector_out(m_axis_tlast);
    mp_M_AXIS_transactor->TLAST(m_m_axis_tlast_converter_signal);

    mp_m_axis_tready_converter = new xsc::common::vectorN2scalar_converter<1>("m_axis_tready_converter");
    mp_m_axis_tready_converter->vector_in(m_axis_tready);
    mp_m_axis_tready_converter->scalar_out(m_m_axis_tready_converter_signal);
    mp_M_AXIS_transactor->TREADY(m_m_axis_tready_converter_signal);

    mp_M_AXIS_transactor->TSTRB(m_axis_tstrb);
    mp_M_AXIS_transactor->TKEEP(m_axis_tkeep);

    mp_m_axis_tvalid_converter = new xsc::common::scalar2vectorN_converter<1>("m_axis_tvalid_converter");
    mp_m_axis_tvalid_converter->scalar_in(m_m_axis_tvalid_converter_signal);
    mp_m_axis_tvalid_converter->vector_out(m_axis_tvalid);
    mp_M_AXIS_transactor->TVALID(m_m_axis_tvalid_converter_signal);

    // M_AXIS' transactor sockets
    mp_impl->M00_AXIS_socket->bind(*(mp_M_AXIS_transactor->socket));
}
#endif

#if defined VCSSYSTEMC || defined MTI_SYSTEMC
sim_ipc_axis_slave_mirror::sim_ipc_axis_slave_mirror(const sc_core::sc_module_name& nm)
    : sc_core::sc_module(nm),
      mp_impl(NULL),
      m_axis_aclk("m_axis_aclk"),
      m_axis_aresetn("m_axis_aresetn"),
      m_axis_tvalid("m_axis_tvalid"),
      m_axis_tready("m_axis_tready"),
      m_axis_tdata("m_axis_tdata"),
      m_axis_tstrb("m_axis_tstrb"),
      m_axis_tkeep("m_axis_tkeep"),
      m_axis_tlast("m_axis_tlast"),
      m_axis_tuser("m_axis_tuser") {
    // initialize module
    xsc::common_cpp::properties model_param_props;
    model_param_props.addLong("C_M00_AXIS_TDATA_WIDTH", "32");
    mp_impl = new sim_ipc_axis_master("inst", model_param_props);

    // initialize pins
    mp_impl->m00_axis_aclk(m_axis_aclk);
    mp_impl->m00_axis_aresetn(m_axis_aresetn);

    // initialize AXI sockets
    M_AXIS_socket = mp_impl->M00_AXIS_socket;

    // initialize transactors
    mp_M_AXIS_transactor = NULL;
    mp_m_axis_tlast_converter = NULL;
    mp_m_axis_tready_converter = NULL;
    mp_m_axis_tvalid_converter = NULL;

    // 'M_AXIS' transactor parameters
    xsc::common_cpp::properties M_AXIS_transactor_param_props;
    M_AXIS_transactor_param_props.addLong("TDATA_NUM_BYTES", "4");
    M_AXIS_transactor_param_props.addLong("TDEST_WIDTH", "0");
    M_AXIS_transactor_param_props.addLong("TID_WIDTH", "0");
    M_AXIS_transactor_param_props.addLong("TUSER_WIDTH", "32");
    M_AXIS_transactor_param_props.addLong("HAS_TREADY", "1");
    M_AXIS_transactor_param_props.addLong("HAS_TSTRB", "1");
    M_AXIS_transactor_param_props.addLong("HAS_TKEEP", "1");
    M_AXIS_transactor_param_props.addLong("HAS_TLAST", "1");
    M_AXIS_transactor_param_props.addLong("FREQ_HZ", "150000000");
    M_AXIS_transactor_param_props.addLong("HAS_RESET", "1");
    M_AXIS_transactor_param_props.addFloat("PHASE", "0.000");
    M_AXIS_transactor_param_props.addString("CLK_DOMAIN", "xilinx_zcu102_base_clk_wiz_0_0_clk_out1");
    M_AXIS_transactor_param_props.addString("LAYERED_METADATA", "undef");
    M_AXIS_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    M_AXIS_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_M_AXIS_transactor =
        new xtlm::xaxis_xtlm2pin_t<4, 32, 1, 1, 1, 1>("M_AXIS_transactor", M_AXIS_transactor_param_props);

    // M_AXIS' transactor ports
    mp_M_AXIS_transactor->CLK(m_axis_aclk);
    mp_M_AXIS_transactor->RST(m_axis_aresetn);

    mp_M_AXIS_transactor->TDATA(m_axis_tdata);
    mp_M_AXIS_transactor->TUSER(m_axis_tuser);

    mp_m_axis_tlast_converter = new xsc::common::scalar2vectorN_converter<1>("m_axis_tlast_converter");
    mp_m_axis_tlast_converter->scalar_in(m_m_axis_tlast_converter_signal);
    mp_m_axis_tlast_converter->vector_out(m_axis_tlast);
    mp_M_AXIS_transactor->TLAST(m_m_axis_tlast_converter_signal);

    mp_m_axis_tready_converter = new xsc::common::vectorN2scalar_converter<1>("m_axis_tready_converter");
    mp_m_axis_tready_converter->vector_in(m_axis_tready);
    mp_m_axis_tready_converter->scalar_out(m_m_axis_tready_converter_signal);
    mp_M_AXIS_transactor->TREADY(m_m_axis_tready_converter_signal);

    mp_M_AXIS_transactor->TSTRB(m_axis_tstrb);
    mp_M_AXIS_transactor->TKEEP(m_axis_tkeep);

    mp_m_axis_tvalid_converter = new xsc::common::scalar2vectorN_converter<1>("m_axis_tvalid_converter");
    mp_m_axis_tvalid_converter->scalar_in(m_m_axis_tvalid_converter_signal);
    mp_m_axis_tvalid_converter->vector_out(m_axis_tvalid);
    mp_M_AXIS_transactor->TVALID(m_m_axis_tvalid_converter_signal);
}

void sim_ipc_axis_slave_mirror::before_end_of_elaboration() {
    // M_AXIS' transactor sockets
    mp_impl->M00_AXIS_socket->bind(*(mp_M_AXIS_transactor->socket));
}
#endif

sim_ipc_axis_slave_mirror::~sim_ipc_axis_slave_mirror() {
    delete mp_M_AXIS_transactor;
    delete mp_m_axis_tlast_converter;
    delete mp_m_axis_tready_converter;
    delete mp_m_axis_tvalid_converter;
    delete mp_impl;
}

#ifdef MTI_SYSTEMC
SC_MODULE_EXPORT(sim_ipc_axis_slave_mirror);
#endif

#ifdef XM_SYSTEMC
XMSC_MODULE_EXPORT(sim_ipc_axis_slave_mirror);
#endif

#ifdef RIVIERA
SC_MODULE_EXPORT(sim_ipc_axis_slave_mirror);
#endif
