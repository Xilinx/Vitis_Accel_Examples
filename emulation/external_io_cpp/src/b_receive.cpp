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

#include "xtlm_ipc.h"

void print(xtlm_ipc::axi_stream_packet& packet) {
    const char* data = packet.data().c_str();

    for (auto& char_elem : packet.data()) std::cout << std::hex << (static_cast<int>(char_elem) & 255) << " ";
    std::cout << std::endl;

    std::cout << "TLAST = " << packet.tlast() << std::endl;
    std::cout << std::endl;
}

void receive_packets() {
    //! Instantiate IPC socket with name matching in IPI diagram...
    xtlm_ipc::axis_target_socket_util<xtlm_ipc::BLOCKING> socket_util("gt_slave");

    const unsigned int NUM_TRANSACTIONS = 100;
    unsigned int num_received = 0;
    xtlm_ipc::axi_stream_packet packet;

    std::cout << "Receiving " << NUM_TRANSACTIONS << " packets..." << std::endl;
    while (num_received < NUM_TRANSACTIONS) {
        socket_util.sample_transaction(packet);
        print(packet);
        num_received += 1;
    }
}

int main(int argc, char* argv[]) {
    receive_packets();
    return 0;
}
