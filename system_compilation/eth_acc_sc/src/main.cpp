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
#include <thread> // std::this_thread::sleep_for
#include <chrono>
#include <unistd.h>
#include "vpp_acc_core.hpp"
#include "eth.hpp"

void config_drop(int val) {
    printf("main: drop %d\n", val);

    Pkt* config = (Pkt*)ETH::alloc_buf(sizeof(Pkt), vpp::input);
    config[0].dt = val;

    auto fut = ETH::compute_async(cmd_drop, config, nullptr);
    fut.get();

    ETH::free_buf(config);
}

void config_add(int val) {
    printf("main: add %d\n", val);

    Pkt* config = (Pkt*)ETH::alloc_buf(sizeof(Pkt), vpp::input);
    config[0].dt = val;

    auto fut = ETH::compute_async(cmd_add, config, nullptr);
    fut.get();

    ETH::free_buf(config);
    std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
}

void print_sample(Pkt* sample, int sz) {
    bool first = true;
    int n = std::min(sz, 10);
    for (int i = 0; i < n; i++) {
        if (first) {
            first = false;
            printf("main: sample");
        } else {
            printf("            ");
        }
        printf(" [%10u] %3d\n", sample[i].nr, sample[i].dt);
    }
}

int config_sample(int sz) {
    printf("main: sample %d\n", sz);

    Pkt* sample = (Pkt*)ETH::alloc_buf(sz * sizeof(Pkt), vpp::output);
    Pkt* config = (Pkt*)ETH::alloc_buf(sizeof(Pkt), vpp::input);
    config[0].dt = sz;

    auto fut = ETH::compute_async(cmd_sample, config, sample);
    fut.get();

    print_sample(sample, sz);

    int pkt_nr = sample[0].nr;
    ETH::free_buf(config);
    ETH::free_buf(sample);

    return pkt_nr;
}

void config_sent() {
    Pkt* config = (Pkt*)ETH::alloc_buf(sizeof(Pkt), vpp::input);
    Pkt* snt = (Pkt*)ETH::alloc_buf(sizeof(Pkt), vpp::output);

    auto fut = ETH::compute_async(cmd_sent, config, snt);
    fut.get();

    printf("main: sent %d packets\n", snt[0].dt);
    ETH::free_buf(config);
    ETH::free_buf(snt);
}

void measure_packet_speed();
void measure_sample_speed();
void measure_overlapping_sample_speed();
void measure_sample_throughput();

int main() {
    config_sample(ringSz);
    config_add(100);
    config_sample(ringSz);
    config_drop(3);
    config_add(200);
    config_sample(ringSz);

    measure_packet_speed();
    measure_sample_speed();
    measure_overlapping_sample_speed();
    measure_sample_throughput();

    printf("\nmain: done\n\n");
}

void measure_packet_speed() {
    printf("\nmeasuring packet speed ... \n");
    int us = 1e3;
    int pkt_nr0 = config_sample(1);
    printf("main: sleep %dus\n", us);
    usleep(us);
    int pkt_nr1 = config_sample(1);

    int pkt = pkt_nr1 - pkt_nr0;
    float Mpkt_s = (float)pkt / us;
    printf("processed %d packets in %d us = %.1f Mpackets/s\n", pkt, us, Mpkt_s);
}

void measure_sample_speed() {
    int n = 10;
    printf("\nmeasuring sample speed ... \n");
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        config_sample(1);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> d = t1 - t0;
    double s = d.count();
    printf("processed %d samples in %f s = %.0f samples/s\n", n, s, n / s);
}

void config_samples(int, int);

void measure_overlapping_sample_speed() {
    int n = 10;
    printf("\nmeasuring overlapping sample speed ... \n");
    auto t0 = std::chrono::high_resolution_clock::now();
    config_samples(1, n);
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> d = t1 - t0;
    double s = d.count();
    printf("processed %d samples in %f s = %.0f samples/s\n", n, s, n / s);
}

void measure_sample_throughput() {
    int n = 10000;
    printf("\nmeasuring sample throughput ... \n");
    auto t0 = std::chrono::high_resolution_clock::now();
    config_sample(n);
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> d = t1 - t0;
    double s = d.count();
    printf("received %d samples in %f s = %.0f samples/s\n", n, s, n / s);
}

void config_samples(int sz, int cnt) {
    printf("main: %d x %d samples\n", cnt, sz);
    auto futs = new std::future<void>[ cnt ];
    Pkt** configs = new Pkt*[cnt];
    Pkt** samples = new Pkt*[cnt];
    for (int i = 0; i < cnt; i++) {
        Pkt* config = (Pkt*)ETH::alloc_buf(sizeof(Pkt), vpp::input);
        Pkt* sample = (Pkt*)ETH::alloc_buf(sz * sizeof(Pkt), vpp::output);
        config[0].dt = sz;

        futs[i] = ETH::compute_async(cmd_sample, config, sample);
        configs[i] = config;
        samples[i] = sample;
    }
#if 1
    for (int i = 0; i < cnt; i++) {
        futs[i].get(); // blocks until result is ready
        print_sample(samples[i], sz);
        ETH::free_buf(configs[i]);
        ETH::free_buf(samples[i]);
    }
#else
    futs[c - t].get(); // just block on the last sample
#endif
    delete[] futs;
    delete[] configs;
    delete[] samples;
}
