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

#pragma once

#include "xtlm.h"
#include "axis2ipc_socket.h"

class sim_ipc_axis_slave : public sc_core::sc_module
{
    public:
        SC_HAS_PROCESS(sim_ipc_axis_slave);

        sim_ipc_axis_slave(sc_core::sc_module_name name,
                xsc::common_cpp::properties& ppts);
        ~sim_ipc_axis_slave();

        sc_in<bool> s00_axis_aclk;
        sc_in<bool> s00_axis_aresetn;

        xtlm::xtlm_axis_target_socket*      S00_AXIS_socket;
        xtlm::xtlm_axis_target_socket_util* s00_util;

    private:
        //! SystemC method to send the axi stream data to external process(ipc)...
        void axi_stream2ipc_send();
        std::string get_ipi_name(std::string s);

        xsc::axis2ipc_socket* axis2ipc_socket;
        xsc::common_cpp::report_handler m_logger;
};
