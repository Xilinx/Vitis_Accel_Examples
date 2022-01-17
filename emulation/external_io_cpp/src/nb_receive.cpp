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

#include <unistd.h>
#include "xtlm_ipc.h"

void print(std::vector<char>& data) {
    for (auto& el : data) {
        std::cout << std::hex << (static_cast<int>(el) & 255) << " ";
    }
    std::cout << std::endl;
}

void receive_data() {
    //! Instantiate IPC socket with name matching in IPI diagram...
    xtlm_ipc::axis_target_socket_util<xtlm_ipc::NON_BLOCKING> socket_util("gt_slave");

    const unsigned int NUM_TRANSACTIONS = 8;
    unsigned int num_received = 0, num_outstanding = 0;
    std::vector<char> data;

    std::cout << "Receiving " << NUM_TRANSACTIONS << " data transactions..." << std::endl;
    while (num_received < NUM_TRANSACTIONS) {
        num_outstanding = socket_util.get_num_transactions();
        num_received += num_outstanding;

        if (num_outstanding != 0) {
            std::cout << "Outstanding data transactions = " << num_outstanding << std::endl;
            for (int i = 0; i < num_outstanding; i++) {
                socket_util.sample_transaction(data);
                print(data);
            }
        }
        usleep(100000);
    }
}

int main(int argc, char* argv[]) {
    receive_data();
    return 0;
}
