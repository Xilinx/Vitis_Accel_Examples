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

#include <algorithm> // std::generate
#include <fstream>
#include "xtlm_ipc.h"

std::ofstream file("expected_blocking.txt", std::ios_base::app);

std::vector<char> generate_data() {
    const unsigned int MAX_BEATS = 4;
    const unsigned int DATA_WIDTH = 4; //! in Bytes

    unsigned int num_beats = std::rand() % MAX_BEATS + 1;
    unsigned int data_length = 4; //:num_beats * DATA_WIDTH;

    //! Initialize vector size...
    std::vector<char> v_data(data_length);

    //! Fill the data in vector...
    std::generate(v_data.begin(), v_data.end(), []() { return std::rand() % 256; });

    return v_data;
}

void expected_packets(const xtlm_ipc::axi_stream_packet& packet) {
    const char* data = packet.data().c_str();

    bool is_lsb = true;
    for (auto& char_elem : packet.data()) {
        if (is_lsb) {
            file << std::hex << ((static_cast<int>(char_elem) + 1) & 255) << " ";
            is_lsb = false;
        } else {
            file << std::hex << (static_cast<int>(char_elem) & 255) << " ";
        }
    }

    file << std::endl;

    file << "TLAST = " << packet.tlast() << std::endl;
    file << std::endl;
}

void print(xtlm_ipc::axi_stream_packet& packet) {
    const char* data = packet.data().c_str();

    for (auto& char_elem : packet.data()) std::cout << std::hex << (static_cast<int>(char_elem) & 255) << " ";
    std::cout << std::endl;

    std::cout << "TLAST = " << packet.tlast() << std::endl;
    std::cout << std::endl;
    expected_packets(packet);
}

xtlm_ipc::axi_stream_packet generate_packet() {
    xtlm_ipc::axi_stream_packet packet;
    std::vector<char> data = generate_data();

    //! Set packet attributes...
    packet.set_data(data.data(), data.size());
    packet.set_data_length(data.size());
    packet.set_tlast(1);
    // packet.set_tlast(std::rand()%2);
    //! Option to set tuser tkeep optional attributes...

    return packet;
}

void send_packets() {
    //! Instantiate IPC socket with name matching in IPI diagram...
    xtlm_ipc::axis_initiator_socket_util<xtlm_ipc::BLOCKING> socket_util("gt_master");

    const unsigned int NUM_TRANSACTIONS = 100;
    xtlm_ipc::axi_stream_packet packet;

    std::cout << "Sending " << NUM_TRANSACTIONS << " Packets..." << std::endl;
    for (int i = 0; i < NUM_TRANSACTIONS; i++) {
        packet = generate_packet();
        print(packet);
        socket_util.transport(packet);
    }
    file.close();
    usleep(10000);
    socket_util.end_of_simulation();
}

int main(int argc, char* argv[]) {
    send_packets();
}
