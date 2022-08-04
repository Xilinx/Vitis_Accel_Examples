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
#ifndef PL_CTRL_UTILS_HPP
#define PL_CTRL_UTILS_HPP

#define MEMTILEDMA_NUM_BD_WORDS 8U
#define SHIMDMA_NUM_BD_WORDS 8U
#define DMA_PAD_NUM_DIMS 3U
#define DMA_STATUS_CHNUM_OFFSET 0x4
#define DMA_STATUS_IDLE 0x0
#define DMA_WAITFORDONE_DEF_WAIT_TIME_US 1000000U
#define NOC_MODULE_DMA_BD0_1_BASE_ADDRESS_LOW_LSB 2
#define NOC_MODULE_DMA_BD0_1_BASE_ADDRESS_LOW_MASK 0xFFFFFFFC
#define NOC_MODULE_DMA_BD0_2_BASE_ADDRESS_HIGH_LSB 0
#define NOC_MODULE_DMA_BD0_2_BASE_ADDRESS_HIGH_MASK 0x0000FFFF
#define ADDR_ALIGN_SHIFT 2
#define LOCK_INIT_ENQ_BD_NM 1
#define ACQ_LOCK_NM 2
#define DMA_WAIT_QUE_NM 3

// ucode status
enum CtrlOp {
    HALT = 0,
    SET_BD = 1,
    ENQ_BD = 2,
    DMA_WAIT_QUE = 3,
    DMA_WAIT = 4,
    LOCK_INIT = 5,
    ACQ_LOCK = 6,
    REL_LOCK = 7,
    EVENT_TRIGGER = 8,
    EVENT_WAIT = 9,
    BREAK = 10,
    WRITE_MM = 11,
    WRITE_MMS = 12
};

#define OPRANDS_OF_HALT 1
#define OPRANDS_OF_SET_BD 11
#define OPRANDS_OF_ENQ_BD 3
#define OPRANDS_OF_DMA_WAIT_QUE 5
#define OPRANDS_OF_DMA_WAIT 4
#define OPRANDS_OF_LOCK_INIT 3
#define OPRANDS_OF_ACQ_LOCK 4
#define OPRANDS_OF_REL_LOCK 3
#define OPRANDS_OF_EVENT_TRIGGER 2
#define OPRANDS_OF_EVENT_WAIT 2
#define OPRANDS_OF_BREAK 1
#define OPRANDS_OF_WRITE_MM 3
// a status size in ucode buff, 259 = 256 + 3, 256 is the size of the write file in the writelayerparams function.
// If the size of the write file changes in the future, it also needs to be changed synchronously.
#define OPRANDS_OF_WRITE_MMS 259

static uint32_t UcodeOffset[] = {
    OPRANDS_OF_HALT,      OPRANDS_OF_SET_BD,   OPRANDS_OF_ENQ_BD,   OPRANDS_OF_DMA_WAIT_QUE,  OPRANDS_OF_DMA_WAIT,
    OPRANDS_OF_LOCK_INIT, OPRANDS_OF_ACQ_LOCK, OPRANDS_OF_REL_LOCK, OPRANDS_OF_EVENT_TRIGGER, OPRANDS_OF_EVENT_WAIT,
    OPRANDS_OF_BREAK,     OPRANDS_OF_WRITE_MM, OPRANDS_OF_WRITE_MMS};

enum TypeOp {
    TYPE_END = 0,
    TYPE_WRITE,
    TYPE_READ_POLL,
    TYPE_READ_PEND,
    TYPE_EVENT_TRIGGER,
    TYPE_EVENT_WAIT,
};

#endif
