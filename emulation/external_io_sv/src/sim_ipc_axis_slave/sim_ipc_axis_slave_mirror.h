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

#ifndef XTLM
#include "xtlm.h"
#endif
#ifndef SYSTEMC_INCLUDED
#include <systemc>
#endif

#include "sim_ipc_axis_master.h"
#include <map>
#include <string>

class sim_ipc_axis_slave_mirror : public sc_core::sc_module {
   public:
    sim_ipc_axis_slave_mirror(const sc_core::sc_module_name& nm);
    virtual ~sim_ipc_axis_slave_mirror();

    // module pin-to-pin RTL interface
    sc_core::sc_in<bool> m_axis_aclk;
    sc_core::sc_in<bool> m_axis_aresetn;
    sc_core::sc_in<sc_dt::sc_bv<1> > m_axis_tready;
    sc_core::sc_out<sc_dt::sc_bv<1> > m_axis_tvalid;
    sc_core::sc_out<sc_dt::sc_bv<32> > m_axis_tdata;
    sc_core::sc_out<sc_dt::sc_bv<4> > m_axis_tstrb;
    sc_core::sc_out<sc_dt::sc_bv<4> > m_axis_tkeep;
    sc_core::sc_out<sc_dt::sc_bv<1> > m_axis_tlast;
    sc_core::sc_out<sc_dt::sc_bv<32> > m_axis_tuser;

    // module socket-to-socket AXIS TLM interfaces
    xtlm::xtlm_axis_initiator_socket* M_AXIS_socket;

   private:
    xtlm::xaxis_xtlm2pin_t<4, 32, 1, 1, 1, 1>* mp_M_AXIS_transactor;
    xsc::common::scalar2vectorN_converter<1>* mp_m_axis_tlast_converter;
    sc_signal<bool> m_m_axis_tlast_converter_signal;
    xsc::common::vectorN2scalar_converter<1>* mp_m_axis_tready_converter;
    sc_signal<bool> m_m_axis_tready_converter_signal;
    xsc::common::scalar2vectorN_converter<1>* mp_m_axis_tvalid_converter;
    sc_signal<bool> m_m_axis_tvalid_converter_signal;

   protected:
    virtual void before_end_of_elaboration();
    sim_ipc_axis_master* mp_impl;

   private:
    sim_ipc_axis_slave_mirror(const sim_ipc_axis_slave_mirror&);
    const sim_ipc_axis_slave_mirror& operator=(const sim_ipc_axis_slave_mirror&);
};
