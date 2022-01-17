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

std::ofstream file("expected_non_blocking.txt", std::ios_base::app);

std::vector<char> generate_data() {
    const unsigned int MAX_BEATS = 4;
    const unsigned int DATA_WIDTH = 4; //! in Bytes

    unsigned int num_beats = std::rand() % MAX_BEATS + 1;
    unsigned int data_length = num_beats * DATA_WIDTH;

    //! Initialize vector size...
    std::vector<char> v_data(data_length);

    //! Fill the data in vector...
    std::generate(v_data.begin(), v_data.end(), []() { return std::rand() % 256; });

    return v_data;
}

void expected_data(const std::vector<char>& data) {
    bool is_lsb = true;
    int data_count = 0;
    for (auto& el : data) {
        if (data_count % 4 == 0) {
            is_lsb = true;
        }
        if (is_lsb) {
            int el_data = static_cast<int>(el) + 1;
            file << std::hex << (el_data & 255) << " ";
            if (el_data == 0) {
                is_lsb = true;
            } else {
                is_lsb = false;
            }
        } else {
            file << std::hex << (static_cast<int>(el) & 255) << " ";
            is_lsb = false;
        }
        data_count++;
    }

    file << std::endl;
}

void print(std::vector<char>& data) {
    for (auto& el : data) {
        std::cout << std::hex << (static_cast<int>(el) & 255) << " ";
    }
    std::cout << std::endl;
    expected_data(data);
}

void send_data() {
    //! Instantiate IPC socket with name matching in IPI diagram...
    xtlm_ipc::axis_initiator_socket_util<xtlm_ipc::NON_BLOCKING> socket_util("gt_master");

    const unsigned int NUM_TRANSACTIONS = 8;
    std::vector<char> data;

    std::cout << "Sending " << NUM_TRANSACTIONS << " data transactions..." << std::endl;
    for (int i = 0; i < NUM_TRANSACTIONS / 2; i++) {
        data = generate_data();
        print(data);
        socket_util.transport(data.data(), data.size());
    }

    std::cout << "Adding Barrier to complete all outstanding transactions..." << std::endl;
    socket_util.barrier_wait();
    for (int i = NUM_TRANSACTIONS / 2; i < NUM_TRANSACTIONS; i++) {
        data = generate_data();
        print(data);
        socket_util.transport(data.data(), data.size());
    }
    file.close();
    sleep(2);
    socket_util.end_of_simulation();
}

int main(int argc, char* argv[]) {
    send_data();
}
