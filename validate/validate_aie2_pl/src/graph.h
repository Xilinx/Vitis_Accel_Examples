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
#pragma once

#include "adf.h"
#include "kernels.h"

// See
// https://confluence.xilinx.com/display/XSW/2022.1+AIE-ML+EA+Release#id-2022.1AIEMLEARelease-TilingParameters
// Tiny Yolo layer 4 section

using namespace adf;

class FlexmlLayer4 : public graph {
   public:
    kernel superkernel[4][2];

    shared_buffer<uint8> ifm;
    shared_buffer<uint8> ofm;
    shared_buffer<uint8> wts;

    external_buffer<uint8> ifm_ddr;
    external_buffer<uint8> wts_ddr;
    external_buffer<uint8> ofm_ddr;

    pktmerge<2> merge[4];

    FlexmlLayer4() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 2; j++) {
                /*if (i==0 && j==0)
                {
                    superkernel[i][j] = kernel::create(dummy_conv2d_with_print); //FIXME
                use mllib super kernel
                }
                else*/
                {
                    superkernel[i][j] = kernel::create(dummy_conv2d); // FIXME use mllib super kernel
                }

                source(superkernel[i][j]) = "src/kernels.cpp"; // FIXME use mllib super kernel
                runtime<ratio>(superkernel[i][j]) = 0.9;
                repetition_count(superkernel[i][j]) = 32;
                location<kernel>(superkernel[i][j]) = tile(i, j);
            }
        }

        ifm = shared_buffer<uint8>::create({8, 64, 8, 52}, 1, 4);
        ofm = shared_buffer<uint8>::create({8, 32, 16, 28}, 4, 1);
        wts = shared_buffer<uint8>::create({78848}, 1, 2);

        // FIXME test not_equal
        location<buffer>(ifm) = address(0, 0, 0);
        location<buffer>(ofm) = address(1, 0, 262144); // 1.5 mem tile
        location<buffer>(wts) = address(3, 0, 0);

        ifm_ddr = external_buffer<uint8>::create({8, 64, 8, 52}, 0, 1);
        ofm_ddr = external_buffer<uint8>::create({8, 32, 16, 28}, 1, 0);
        wts_ddr = external_buffer<uint8>::create({78848}, 0, 1);

        for (int i = 0; i < 4; i++) {
            merge[i] = pktmerge<2>::create();
        }

        connect<stream>(ifm_ddr.out[0], ifm.in[0]);
        read_access(ifm_ddr.out[0]) =
            tiling({.buffer_dimension = {8, 64, 8, 52}, .tiling_dimension = {8, 64, 8, 52}, .offset = {0, 0, 0, 0}});
        write_access(ifm.in[0]) =
            tiling({.buffer_dimension = {8, 64, 8, 52}, .tiling_dimension = {8, 64, 8, 52}, .offset = {0, 0, 0, 0}});
        location<dma>(ifm_ddr.out[0]) = dma_channel(shim_tile, 2, 0, 0);
        location<dma>(ifm.in[0]) = dma_channel(memory_tile, 0, 0, 0);

        connect<stream>(wts_ddr.out[0], wts.in[0]);
        read_access(wts_ddr.out[0]) = tiling({.buffer_dimension = {78848}, .tiling_dimension = {78848}, .offset = {0}});
        write_access(wts.in[0]) = tiling({.buffer_dimension = {78848}, .tiling_dimension = {78848}, .offset = {0}});
        location<dma>(wts_ddr.out[0]) = dma_channel(shim_tile, 3, 0, 0);
        location<dma>(wts.in[0]) = dma_channel(memory_tile, 3, 0, 0);

        // IFM
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 2; j++) {
                // IFM sub-volume for each column broadcast along the column to 2 rows
                connect<stream, window<8 * 20 * 2 * 16 * sizeof(uint8)> >(ifm.out[i], superkernel[i][j].in[0]);
                location<buffer>(superkernel[i][j].in[0]) = {address(i, j, 0), address(i, j, 0x2000)};
                location<dma>(superkernel[i][j].in[0]) = dma_channel(aie_tile, i, j, 0); // s2mm0
            }

            read_access(ifm.out[i]) =
                tiling({.buffer_dimension = {8, 64, 8, 52},
                        .tiling_dimension = {8, 20, 2, 16},
                        .offset = {0, -1 + 16 * i, 0, -1},
                        .tile_traversal = {{.order = 2, .stride = 2, .wrap = 4}, {.order = 3, .stride = 14, .wrap = 4}},
                        .packet_port_id = -1,
                        .repetition = 2});
            location<dma>(ifm.out[i]) = dma_channel(memory_tile, 1, 0, i);
        }

        // WTS
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 4; i++) {
                // WTS sub-volume for each row broadcast along the row to 4 columns
                connect<stream, window<4928 * sizeof(uint8)> >(wts.out[j], superkernel[i][j].in[1]);
                // async(superkernel[i][j].in[1]); //wts in this case is sync
                location<buffer>(superkernel[i][j].in[1]) = {address(i, j, 0x4000), address(i, j, 0x6000)};
                location<dma>(superkernel[i][j].in[1]) = dma_channel(aie_tile, i, j, 1); // s2mm1
            }

            read_access(wts.out[j]) = {tiling({.buffer_dimension = {78848},
                                               .tiling_dimension = {4928},
                                               .offset = {j * 4928 * 4},
                                               .tile_traversal = {{.order = 0, .stride = 4928, .wrap = 4}},
                                               .packet_port_id = -1,
                                               .repetition = 4}),
                                       tiling({.buffer_dimension = {78848},
                                               .tiling_dimension = {4928},
                                               .offset = {(j + 2) * 4928 * 4},
                                               .tile_traversal = {{.order = 0, .stride = 4928, .wrap = 4}},
                                               .packet_port_id = -1,
                                               .repetition = 4})};
            location<dma>(wts.out[j]) = dma_channel(memory_tile, 3, 0, j);
        }

        // OFM
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 2; j++) {
                // merge along column
                connect<window<8 * 8 * 4 * 7 * sizeof(uint8)>, pktstream>(superkernel[i][j].out[0], merge[i].in[j]);
                async(superkernel[i][j].out[0]);
                async_repetition(superkernel[i][j].out[0]) = 4;
                location<buffer>(superkernel[i][j].out[0]) = {address(i, j, 0x8000), address(i, j, 0xa000)};
                location<dma>(superkernel[i][j].out[0]) = dma_channel(aie_tile, i, j, 0); // mm2s0
            }

            connect<pktstream>(merge[i].out[0], ofm.in[i]);

            write_access(ofm.in[i]) = {tiling({.buffer_dimension = {8, 32, 16, 28},
                                               .tiling_dimension = {8, 8, 4, 7},
                                               .offset = {0, 8 * i, 0, 0},
                                               .tile_traversal = {{.order = 3, .stride = 7, .wrap = 4}},
                                               .packet_port_id = 0}),
                                       tiling({.buffer_dimension = {8, 32, 16, 28},
                                               .tiling_dimension = {8, 8, 4, 7},
                                               .offset = {0, 8 * i, 4, 0},
                                               .tile_traversal = {{.order = 3, .stride = 7, .wrap = 4}},
                                               .packet_port_id = 1}),
                                       tiling({.buffer_dimension = {8, 32, 16, 28},
                                               .tiling_dimension = {8, 8, 4, 7},
                                               .offset = {0, 8 * i, 8, 0},
                                               .tile_traversal = {{.order = 3, .stride = 7, .wrap = 4}},
                                               .packet_port_id = 0}),
                                       tiling({.buffer_dimension = {8, 32, 16, 28},
                                               .tiling_dimension = {8, 8, 4, 7},
                                               .offset = {0, 8 * i, 12, 0},
                                               .tile_traversal = {{.order = 3, .stride = 7, .wrap = 4}},
                                               .packet_port_id = 1})};

            location<dma>(ofm.in[i]) = dma_channel(memory_tile, 1, 0, i);
        }

        connect<stream>(ofm.out[0], ofm_ddr.in[0]);
        read_access(ofm.out[0]) =
            tiling({.buffer_dimension = {8, 32, 16, 28}, .tiling_dimension = {8, 32, 16, 28}, .offset = {0, 0, 0, 0}});
        write_access(ofm_ddr.in[0]) =
            tiling({.buffer_dimension = {8, 32, 16, 28}, .tiling_dimension = {8, 32, 16, 28}, .offset = {0, 0, 0, 0}});
        location<dma>(ofm.out[0]) = dma_channel(memory_tile, 2, 0, 0);
        location<dma>(ofm_ddr.in[0]) = dma_channel(shim_tile, 2, 0, 0);
    }
};
