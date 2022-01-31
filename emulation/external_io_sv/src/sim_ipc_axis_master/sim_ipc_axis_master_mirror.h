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

#include "sim_ipc_axis_slave.h"
#ifndef XTLM
#include "xtlm.h"
#endif
#ifndef SYSTEMC_INCLUDED
#include <systemc>
#endif

class sim_ipc_axis_master_mirror : public sc_core::sc_module {
   public:
    sim_ipc_axis_master_mirror(const sc_core::sc_module_name& nm);
    virtual ~sim_ipc_axis_master_mirror();

    // module pin-to-pin RTL interface
    sc_core::sc_in<bool> s_axis_aclk;
    sc_core::sc_in<bool> s_axis_aresetn;
    sc_core::sc_in<sc_dt::sc_bv<1> > s_axis_tvalid;
    sc_core::sc_out<sc_dt::sc_bv<1> > s_axis_tready;
    sc_core::sc_in<sc_dt::sc_bv<32> > s_axis_tdata;
    sc_core::sc_in<sc_dt::sc_bv<4> > s_axis_tstrb;
    sc_core::sc_in<sc_dt::sc_bv<4> > s_axis_tkeep;
    sc_core::sc_in<sc_dt::sc_bv<1> > s_axis_tlast;
    sc_core::sc_in<sc_dt::sc_bv<32> > s_axis_tuser;

    // module socket-to-socket AXI TLM interfaces
    xtlm::xtlm_axis_target_socket* S_AXIS_socket;

   private:
    xtlm::xaxis_pin2xtlm_t<4, 32, 1, 1, 1, 1>* mp_S_AXIS_transactor;
    xsc::common::vectorN2scalar_converter<1>* mp_s_axis_tlast_converter;
    sc_signal<bool> m_s_axis_tlast_converter_signal;
    xsc::common::scalar2vectorN_converter<1>* mp_s_axis_tready_converter;
    sc_signal<bool> m_s_axis_tready_converter_signal;
    xsc::common::vectorN2scalar_converter<1>* mp_s_axis_tvalid_converter;
    sc_signal<bool> m_s_axis_tvalid_converter_signal;

   protected:
    sim_ipc_axis_slave* mp_impl;
    virtual void before_end_of_elaboration();

   private:
    sim_ipc_axis_master_mirror(const sim_ipc_axis_master_mirror&);
    const sim_ipc_axis_master_mirror& operator=(const sim_ipc_axis_master_mirror&);
};
