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

#include "sim_ipc_axis_master.h"

using namespace xtlm;

sim_ipc_axis_master::sim_ipc_axis_master(sc_module_name name, 
        xsc::common_cpp::properties& ppts) : sc_module(name) ,m_logger((std::string)name)
{
    unsigned int stream_width = ppts.getLongLong("C_M00_AXIS_TDATA_WIDTH");
    stream_width_bytes = stream_width/8;

    M00_AXIS_socket = new xtlm_axis_initiator_socket("M00_AXIS", stream_width);
    m00_util = new xtlm_axis_initiator_socket_util("M00_AXIS_util", axis::TRANSACTION, stream_width);
    m00_util->stream_socket.bind(*M00_AXIS_socket);

    //! Instantiate IPC2AXIS Socket
	ipc2axis_socket = new xsc::ipc2axis_socket("ipc2axis_socket", get_ipi_name(this->name()));

    SC_METHOD(ipc2axi_stream_receive);
	sensitive << ipc2axis_socket->event();
    sensitive << m00_util->transfer_done;

    SC_METHOD(send_response);
    sensitive << m00_util->transfer_done;
	dont_initialize();    
}

void sim_ipc_axis_master::ipc2axi_stream_receive() 
{
    if (!m00_util->is_transfer_done()) {
        return;
    }

    xtlm::axis_payload* payload = ipc2axis_socket->get_payload();
    if(payload != nullptr) {

        payload->set_n_beats((payload->get_tdata_length() + stream_width_bytes - 1)/stream_width_bytes);
        std::string log; payload->get_log(log, 1);
        XSC_REPORT_INFO_VERB(m_logger, this->name(), log.c_str(), DEBUG);

        m00_util->transport(payload, SC_ZERO_TIME);
    }
}

void sim_ipc_axis_master::send_response() 
{
    ipc2axis_socket->send_response();
}

std::string sim_ipc_axis_master::get_ipi_name(std::string s)
{
#ifdef VCSSYSTEMC
    //A workaround for VCS as this->name() for VCS is giving with "_" as delimiter
    //We are unable to differentiate _ delimter from the one in instance name
    //As a workaround using a ENV variable, ideally this should be limited for 
    //2021.1 release, a clean solution is needed in upcoming releases
	std::string env_list;
    if(std::getenv("VCS_SIM_IPC_INST_LIST"))
        env_list = std::getenv("VCS_SIM_IPC_INST_LIST");

    int pos = 0;
	do
	{
	    pos = env_list.find(" ");
		if(s.find(env_list.substr(0, pos)) != std::string::npos)
			return env_list.substr(0, pos);
		env_list.erase(0, pos+1);
	}
	while(pos != std::string::npos);
    throw std::runtime_error("Unable to find sim_ipc instance name. Please set VCS_SIM_IPC_INST_LIST with list of instances");
    return "";
#else    
    s = s.substr(0, s.find_last_of("./")); // Adding "/" to support QUESTA
    s = s.substr(s.find_last_of("./") + 1);
    return s;
#endif
}

sim_ipc_axis_master::~sim_ipc_axis_master()
{
	delete m00_util;
	delete M00_AXIS_socket;
	delete ipc2axis_socket;
}
