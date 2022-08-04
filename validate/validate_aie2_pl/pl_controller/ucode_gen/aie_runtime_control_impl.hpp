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
#ifndef AIE_RUNTIME_CONTROL_IMPL_HPP
#define AIE_RUNTIME_CONTROL_IMPL_HPP

#include <stdint.h>
#include <fstream>
#include <sstream>

#include "utils.hpp"
extern "C" {
#include <xaiengine.h>
}
#include "adf/adf_api/AIEControlConfig.h"

#include "adf/adf_api/AIERuntimeControl.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

#define MEMTILEDMA_NUM_BD_WORDS 8U
#define SHIMDMA_NUM_BD_WORDS 8U
#define DMA_PAD_NUM_DIMS 3U
#define DMA_STATUS_CHNUM_OFFSET 0x4
#define DMA_STATUS_IDLE 0x0
#define DMA_WAITFORDONE_DEF_WAIT_TIME_US 1000000U

#define XAIEML_LOCK_VALUE_MASK 0x7F
#define XAIEML_LOCK_VALUE_SHIFT 0x2
#define XAIEML_LOCK_RESULT_LSB 0x0
#define XAIEML_LOCK_RESULT_MASK 0x1
#define XAIEML_LOCK_RESULT_SUCCESS 1U
#define XAIE_DMA_32BIT_TXFER_LEN 2U

#define UCODE_ORDER_DISABLE 0
#define UCODE_ORDER_ENABLE 1
#define UCODE_CHAN_DEFAULT 0
#define UCODE_RT_DISABLE 0
#define UCODE_RT_ENABLE 1
#define UCODE_RTID_DEFAULT 0

inline uint32_t ucodeHeader(uint32_t op, uint32_t order, uint32_t chan, uint32_t rt, uint32_t rtid) {
    uint32_t re = (op & 0x3F) + ((order & 0x1) << 6) + ((chan & 0xF) << 7) + ((rt & 0x1) << 11) + ((rtid & 0xFF) << 12);
    return re;
}

class pl_ucode_gen_impl {
   private:
    static uint32_t relativeToAbsoluteRow(adf::tile_type tileType, uint32_t row);
    static int setDmaBd(XAie_DmaDesc* dmaDesc, XAie_LocType loc, uint8_t bdNum, int64_t rtId);
    static void initLockToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t val);
    static void acqLockToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t mask, uint32_t val);
    static void relLockToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t mask, uint32_t val);
    static void setBdToBuff(
        bool order, uint16_t chan, bool rt, uint8_t rtid, uint32_t addr, uint32_t nm, uint32_t* val);
    static void writeMMToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t nm, uint32_t* val);
    static void enqBdToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t val);
    static void DmaWaitQueToBuff(
        bool order, uint16_t chan, uint32_t addr, uint32_t part_mask, uint32_t part_lsb, uint32_t mask);
    static void DmaWaitToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t mask, uint32_t val);
    static uint32_t getChan(uint32_t id);

   public:
    static std::vector<uint32_t> buff;
    static std::vector<std::string> ext_name;
    static std::vector<uint32_t> core_en;
    static std::vector<uint32_t> core_iter;
    static std::string work_path;
    static uint32_t aie_start_row;
    // channel select information
    static std::vector<std::array<uint32_t, 10> > chan_info;

    static void configureBufferDescriptor(adf::tile_type tileType,
                                          uint32_t column,
                                          uint32_t row,
                                          uint32_t bdId,
                                          const adf::dma_buffer_descriptor& bd,
                                          uint32_t bytes,
                                          uint32_t burst_length);
    static void enqueueTask(adf::tile_type tileType,
                            uint32_t col,
                            uint32_t row,
                            adf::dma_channel_dir Dir,
                            uint32_t ChNum,
                            uint32_t StartBd,
                            uint32_t repeatCount,
                            bool enableTaskCompleteToken);
    static void waitDMAChannelTaskQueue(
        adf::tile_type tileType, uint32_t column, uint32_t row, adf::dma_channel_dir dir, uint32_t channel);
    static void waitDMAChannelDone(
        adf::tile_type tileType, uint32_t column, uint32_t row, adf::dma_channel_dir dir, uint32_t channel);
    static void initializeLock(
        adf::tile_type tileType, uint32_t column, uint32_t row, uint32_t lockId, int32_t initVal);
    static void acquireLock(adf::tile_type tileType, uint32_t column, uint32_t row, uint32_t lockId, int32_t acqVal);
    static void releaseLock(adf::tile_type tileType, uint32_t column, uint32_t row, uint32_t lockId, int32_t relVal);
    static uint64_t getExternalBufferAddress(std::string& externalBufferName);
    static void writeLayerParams(const std::vector<adf::LayerParamsLocation> LayerParamsLocs);
    static void blockWrite(
        adf::tile_type tileType, uint8_t column, uint8_t row, uint32_t address, const std::string& file, uint32_t size);
    static int write2Output(std::string& file);
    static void parseControlConfig(std::string& file);
    static void addSyncPoint();
};

#endif // __AIE_RUNTIME_CONTROL_IMPL_HPP_
