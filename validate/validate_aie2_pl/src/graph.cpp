/*
 * Copyright 2022 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "graph.h"
#include <fstream>

#ifdef __AIESIM__
#include "../aie_runtime_control.cpp"
#endif

/* matlab reference
%change 8*64*8*52 byte to 2*64*8*52 int
ifm_length = (8/4)*64*8*52;
a = [0:ifm_length-1];
b=reshape(a,[2,64,8,52]);
c=zeros(2, 1+64+3, 8, 1+52+5);
c(:, 2:65, :, 2:53) = b;

wts = [0:78848/4-1];
wts_sv_len_32b = 4928/4;
ofm_sv_len_32b = (8/4)*8*4*7;

ofm=ones(8/4,32,16,28)*1431655765;

for cout = 0 : 3 %row and cout iteration
    for w = 0 : 3
        for h = 0 : 3
            ofm_temp = zeros(1,ofm_sv_len_32b);
            for cin = 0 : 3
                ifm_sv = c(:, (w*16+1):(w*16+20), (cin*2+1):(cin*2+2),
(h*14+1):(h*14+16));
                wts_sv = wts((cout*4*wts_sv_len_32b + cin*wts_sv_len_32b + 1) :
(cout*4*wts_sv_len_32b + cin*wts_sv_len_32b + wts_sv_len_32b));
                ifm_sv_1d = reshape(ifm_sv, [1,(8/4)*20*2*16]);
                ofm_temp = ofm_temp + ifm_sv_1d(1:ofm_sv_len_32b) +
wts_sv(1:ofm_sv_len_32b);
            end
            ofm_sv = reshape(ofm_temp, [(8/4),8,4,7]);
            ofm(:, w*8+1 : w*8+8, cout*4+1 : cout*4+4, h*7+1 : h*7+7) = ofm_sv;
        end
    end
end

ofm_ref = reshape(ofm,[1,(8/4)*32*16*28]);

fid = fopen('en2_311a.txt','wt');
fprintf(fid,'%d\n',ofm_ref);
fclose(fid);
*/

FlexmlLayer4 g;

int main() {
    const int ifm_size = 8 * 64 * 8 * 52;
    const int wts_size = 78848;
    const int ofm_size = 8 * 32 * 16 * 28;

    g.init();

    // FIXME use TinyYolo data

    uint32_t* ifmData = (uint32_t*)GMIO::malloc(ifm_size * sizeof(uint8_t));
    for (int i = 0; i < ifm_size / 4; i++) ifmData[i] = i;
    g.ifm_ddr.setAddress(ifmData);

    uint32_t* wtsData = (uint32_t*)GMIO::malloc(wts_size * sizeof(uint8_t));
    for (int i = 0; i < wts_size / 4; i++) wtsData[i] = i;
    g.wts_ddr.setAddress(wtsData);

    uint32_t* ofmData = (uint32_t*)GMIO::malloc(ofm_size * sizeof(uint8_t));
    g.ofm_ddr.setAddress(ofmData);

    g.run(1);

#ifdef __AIESIM__
    executeRuntimeControl();
#endif

    g.end();

#ifndef __X86SIM__
    system("mkdir aiesimulator_output/data");
    std::ofstream ofs;
    ofs.open("aiesimulator_output/data/output.txt", std::ofstream::out | std::ofstream::trunc);
#else
    system("mkdir -p x86simulator_output/data");
    std::ofstream ofs;
    ofs.open("x86simulator_output/data/output.txt", std::ofstream::out | std::ofstream::trunc);
#endif

    for (int i = 0; i < ofm_size / 4; i++) ofs << (uint32_t)ofmData[i] << std::endl;
    ofs.close();

    GMIO::free(ifmData);
    GMIO::free(wtsData);
    GMIO::free(ofmData);

    return 0;
}
