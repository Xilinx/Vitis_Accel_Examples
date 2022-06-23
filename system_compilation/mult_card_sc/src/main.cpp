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

#include <fcntl.h> // O_RDWR, O_DIRECT, ...
#include <iostream>
#include <sys/stat.h> // fstat

#include "xacc.hpp"

#define COUT(X)                                          \
    {                                                    \
        std::lock_guard<std::mutex> guard(g_cout_mutex); \
        std::cout << X << '\n';                          \
    }

void run_sw(int argc, char** argv, std::map<std::string, int[26]>& cnt) {
    for (int arg = 2; arg < argc; arg++) {
        int i = arg % NCARDS;
        const char* fn = argv[arg];
        if (cnt.find(fn) != cnt.end()) {
            continue;
        }
        int fd = open(fn, O_RDONLY);
        if (fd == -1) {
            COUT("Cannot open " << fn);
            continue;
        }
        struct stat sb;
        fstat(fd, &sb);
        int sz = sb.st_size;
        char* buf = new char[sz];
        int rv = pread(fd, buf, sz, 0);
        assert(rv == sz);
        auto& c = cnt[fn];
        xacc::hls_top(buf, sz, c);
        std::cout << "File " << fn << " on sw\n";
        for (int i = 0; i < 26; i++) {
            std::cout << ' ' << (char)('A' + i) << '=' << c[i];
        }
        std::cout << '\n';
        delete buf;
    }
}

int main(int argc, char** argv) {
    int error = 0;

    if (argc < 2) {
        std::cout << "ERROR: expected aruments\n";
        std::cout << "USAGE: " << argv[0] << " <MODE> <files...>\n";
        std::cout << "  <MODE>: 0 = H2C, 1 = P2P\n";
        return 1;
    }

    std::map<std::string, int[26]> sw_cnt;
    std::cout << "\nComputing " << argc - 2 << " files on sw...\n\n";
    run_sw(argc, argv, sw_cnt);

    bool P2P = atoi(argv[1]);
    std::cout << "\nComputing " << argc - 2 << " files on " << NCARDS << " cards (" << (P2P ? "P2P" : "H2C")
              << " mode)...\n\n";

    VPP_CC cuCluster[NCARDS];
    vpp::squeue<std::string> jobQ[NCARDS];
    vpp::squeue<std::string> pendQ[NCARDS];

    for (int i = 0; i < NCARDS; i++) {
        xacc::add_card(cuCluster[i], i);
        jobQ[i].init_size(2);
        pendQ[i].init_size(2);
    }

    auto InBp = xacc::create_bufpool(vpp::input, P2P ? vpp::p2p : vpp::h2c);
    auto OutBp = xacc::create_bufpool(vpp::output);

    for (int i = 0; i < NCARDS; i++) {
        auto send_fn = [=, &jobQ, &pendQ]() -> bool {
            std::string fn = jobQ[i].get();
            if (fn.empty()) return 0;
            int mode = O_RDWR;
            if (P2P) mode |= O_DIRECT;
            int fd = open(fn.c_str(), mode);
            if (fd == -1) {
                COUT("Cannot open " << fn);
                return 1;
            }
            struct stat sb;
            fstat(fd, &sb);
            int sz = sb.st_size;
            COUT("Start compute " << fn << " [" << sz << "B] on card " << i << " ...");
            char* buf = xacc::file_buf<char>(InBp, fd, sz, 0);
            int* cnt = xacc::alloc_buf<int>(OutBp, 26);

            xacc::compute(buf, sz, cnt);

            pendQ[i].put(fn);
            return 1;
        };

        auto recv_fn = [=, &pendQ, &sw_cnt, &error]() {
            int* cnt = xacc::get_buf<int>(OutBp);
            std::string fn = pendQ[i].get();
            std::lock_guard<std::mutex> guard(g_cout_mutex);
            std::cout << "File " << fn << " on card " << i << "\n";
            for (int i = 0; i < 26; i++) {
                std::cout << ' ' << (char)('A' + i) << '=' << cnt[i];
            }
            std::cout << '\n';
            auto& c = sw_cnt[fn];
            for (int i = 0; i < 26; i++) {
                if (c[i] != cnt[i]) {
                    error++;
                    std::cout << "ERROR: mismatch @" << i << " on " << fn << "!\n";
                    break;
                }
            }
        };

        xacc::send_while(send_fn, cuCluster[i]);
        xacc::receive_all_in_order(recv_fn, cuCluster[i]);
    }

    for (int arg = 2; arg < argc; arg++) {
        int i = arg % NCARDS;
        COUT("Queueing " << argv[arg] << " on card " << i);
        jobQ[i].put(argv[arg]);
    }

    for (int i = 0; i < NCARDS; i++) {
        jobQ[i].put("");
        xacc::join(cuCluster[i]);
    }

    if (error == 0)
        std::cout << "Test Passed!" << std::endl;
    else
        std::cout << "ERROR: Test Failed" << std::endl;
    return error;
}
