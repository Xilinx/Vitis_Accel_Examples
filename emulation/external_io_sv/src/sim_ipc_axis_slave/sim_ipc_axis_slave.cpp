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

using namespace xtlm;

sim_ipc_axis_slave::sim_ipc_axis_slave(sc_module_name name, xsc::common_cpp::properties& ppts)
    : sc_module(name), m_logger((std::string)name) {
    unsigned int stream_width = ppts.getLongLong("C_S00_AXIS_TDATA_WIDTH");
    S00_AXIS_socket = new xtlm_axis_target_socket("S00_AXIS", stream_width);
    s00_util = new xtlm::xtlm_axis_target_socket_util("s00_util", axis::TRANSACTION, stream_width);
    S00_AXIS_socket->bind((s00_util->stream_socket));

    //! Instantiate IPC port
    axis2ipc_socket = new xsc::axis2ipc_socket("sim_ipc_axis_slave", get_ipi_name(this->name()));

    SC_METHOD(axi_stream2ipc_send);
    sensitive << s00_util->transaction_available;
    sensitive << axis2ipc_socket->event(); //! transfer complete
    dont_initialize();
}

void sim_ipc_axis_slave::axi_stream2ipc_send() {
    if (!axis2ipc_socket->is_transfer_done() || (!s00_util->is_transaction_available())) {
        return;
    }

    // Get the payload
    xtlm::axis_payload* payload = s00_util->sample_transaction();

    std::string log;
    payload->get_log(log, 1);
    XSC_REPORT_INFO_VERB(m_logger, this->name(), log.c_str(), DEBUG);

    // Send axi stream payload
    axis2ipc_socket->transport(payload);
}

std::string sim_ipc_axis_slave::get_ipi_name(std::string s) {
#ifdef VCSSYSTEMC
    // A workaround for VCS as this->name() for VCS is giving with "_" as delimiter
    // We are unable to differentiate _ delimter from the one in instance name
    // As a workaround using a ENV variable, ideally this should be limited for
    // 2021.1 release, a clean solution is needed in upcoming releases
    std::string env_list;
    if (std::getenv("VCS_SIM_IPC_INST_LIST")) env_list = std::getenv("VCS_SIM_IPC_INST_LIST");

    int pos = 0;
    do {
        pos = env_list.find(" ");
        if (s.find(env_list.substr(0, pos)) != std::string::npos) return env_list.substr(0, pos);
        env_list.erase(0, pos + 1);
    } while (pos != std::string::npos);
    throw std::runtime_error(
        "Unable to find sim_ipc instance name. Please set VCS_SIM_IPC_INST_LIST with list of instances");
    return "";
#else
    s = s.substr(0, s.find_last_of("./")); // Adding "/" to support QUESTA
    s = s.substr(s.find_last_of("./") + 1);
    return s;
#endif
}

sim_ipc_axis_slave::~sim_ipc_axis_slave() {
    delete s00_util;
    delete S00_AXIS_socket;
    delete axis2ipc_socket;
}
