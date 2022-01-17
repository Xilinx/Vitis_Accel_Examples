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
#include "ipc2axis_socket.h"

class sim_ipc_axis_master : public sc_core::sc_module
{
    public:
        SC_HAS_PROCESS(sim_ipc_axis_master);

        sim_ipc_axis_master(sc_core::sc_module_name name,
                xsc::common_cpp::properties& ppts);
        ~sim_ipc_axis_master();

        sc_in<bool>  m00_axis_aresetn;
        sc_in<bool>  m00_axis_aclk;

        xtlm::xtlm_axis_initiator_socket*      M00_AXIS_socket;
        xtlm::xtlm_axis_initiator_socket_util* m00_util;

    private:
        //! SystemC Method to Read incoming data from ipc...
        void ipc2axi_stream_receive();
        void send_response();
        std::string get_ipi_name(std::string s);

        xsc::ipc2axis_socket* ipc2axis_socket;
        xsc::common_cpp::report_handler m_logger;
        unsigned int stream_width_bytes;
};
