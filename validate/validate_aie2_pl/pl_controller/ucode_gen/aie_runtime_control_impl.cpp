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
#include <string>
#include <vector>
#include <array>
#include "aie_runtime_control_impl.hpp"

// declaration DevInst, it is defined in aie_control.cpp
extern XAie_DevInst DevInst;

std::vector<uint32_t> pl_ucode_gen_impl::buff;
std::vector<std::string> pl_ucode_gen_impl::ext_name;
std::vector<uint32_t> pl_ucode_gen_impl::core_en;
std::vector<uint32_t> pl_ucode_gen_impl::core_iter;
std::vector<std::array<uint32_t, 10> > pl_ucode_gen_impl::chan_info;
uint32_t pl_ucode_gen_impl::aie_start_row;
std::string pl_ucode_gen_impl::work_path;

uint32_t pl_ucode_gen_impl::relativeToAbsoluteRow(adf::tile_type tileType, uint32_t row) {
    uint32_t absoluteRow = row;
    if (tileType == adf::aie_tile)
        absoluteRow += aie_start_row;
    else if (tileType == adf::memory_tile)
        absoluteRow += 1;
    return absoluteRow;
}

void pl_ucode_gen_impl::initLockToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t val) {
    uint32_t header = ucodeHeader(LOCK_INIT, order, chan, UCODE_RT_DISABLE, UCODE_RTID_DEFAULT);
    buff.push_back(header);
    buff.push_back(addr);
    buff.push_back(val);
}

void pl_ucode_gen_impl::acqLockToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t mask, uint32_t val) {
    uint32_t header = ucodeHeader(ACQ_LOCK, order, chan, UCODE_RT_DISABLE, UCODE_RTID_DEFAULT);
    buff.push_back(header);
    buff.push_back(addr);
    buff.push_back(mask);
    buff.push_back(val);
}

void pl_ucode_gen_impl::relLockToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t mask, uint32_t val) {
    uint32_t header = ucodeHeader(REL_LOCK, order, chan, UCODE_RT_DISABLE, UCODE_RTID_DEFAULT);
    buff.push_back(header);
    buff.push_back(addr);
    buff.push_back(mask);
    buff.push_back(val);
}

void pl_ucode_gen_impl::setBdToBuff(
    bool order, uint16_t chan, bool rt, uint8_t rtid, uint32_t addr, uint32_t nm, uint32_t* val) {
    uint32_t header = ucodeHeader(SET_BD, order, chan, rt, rtid);
    buff.push_back(header);
    buff.push_back(addr);
    buff.push_back(nm);
    for (int i = 0; i < nm; i++) buff.push_back(val[i]);
}

void pl_ucode_gen_impl::writeMMToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t nm, uint32_t* val) {
    uint32_t header = ucodeHeader(WRITE_MMS, order, chan, UCODE_RT_DISABLE, UCODE_RTID_DEFAULT);
    buff.push_back(header);
    buff.push_back(addr);
    buff.push_back(nm);
    for (int i = 0; i < nm; i++) buff.push_back(val[i]);
}

void pl_ucode_gen_impl::enqBdToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t val) {
    uint32_t header = ucodeHeader(ENQ_BD, order, chan, UCODE_RT_DISABLE, UCODE_RTID_DEFAULT);
    buff.push_back(header);
    buff.push_back(addr);
    buff.push_back(val);
}

void pl_ucode_gen_impl::DmaWaitQueToBuff(
    bool order, uint16_t chan, uint32_t addr, uint32_t part_mask, uint32_t part_lsb, uint32_t mask) {
    uint32_t header = ucodeHeader(DMA_WAIT_QUE, order, chan, UCODE_RT_DISABLE, UCODE_RTID_DEFAULT);
    buff.push_back(header);
    buff.push_back(addr);
    buff.push_back(part_mask);
    buff.push_back(part_lsb);
    buff.push_back(mask);
}

void pl_ucode_gen_impl::DmaWaitToBuff(bool order, uint16_t chan, uint32_t addr, uint32_t mask, uint32_t val) {
    uint32_t header = ucodeHeader(DMA_WAIT, order, chan, UCODE_RT_DISABLE, UCODE_RTID_DEFAULT);
    buff.push_back(header);
    buff.push_back(addr);
    buff.push_back(mask);
    buff.push_back(val);
}

void pl_ucode_gen_impl::initializeLock(
    adf::tile_type tileType, uint32_t column, uint32_t row, uint32_t lockId, int32_t initVal) {
    XAie_LocType loc = XAie_TileLoc(column, relativeToAbsoluteRow(tileType, row));
    XAie_Lock Lock = XAie_LockInit(lockId, initVal);
    uint8_t TileType;
    const XAie_LockMod* LockMod;
    if (DevInst.IsReady != XAIE_COMPONENT_IS_READY) {
        printf("[ERROR] Invalid Device Instance\n");
    }

    TileType = DevInst.DevOps->GetTTypefromLoc(&DevInst, loc);
    if (TileType == XAIEGBL_TILE_TYPE_SHIMPL) {
        printf("[ERROR] Invalid Tile Type\n");
    }

    LockMod = DevInst.DevProp.DevMod[TileType].LockMod;
    if (Lock.LockId > LockMod->NumLocks) {
        printf("[ERROR] Invalid Lock Id\n");
    }

    uint64_t RegAddr =
        LockMod->LockSetValBase + LockMod->LockSetValOff * Lock.LockId + _XAie_GetTileAddr(&DevInst, loc.Row, loc.Col);

    uint32_t RegVal = XAie_SetField(Lock.LockVal, LockMod->LockInit->Lsb, LockMod->LockInit->Mask);
    initLockToBuff(UCODE_ORDER_DISABLE, UCODE_CHAN_DEFAULT, RegAddr, RegVal);
// XAie_Write32(DevInst, RegAddr, RegVal)
#ifdef _PLCTRL_DEBUG_
    std::cout << "XAie_LockSetValue "
              << "col " << (uint16_t)loc.Col << ", row " << (uint16_t)loc.Row << ", lock id " << lockId << ", value "
              << (uint16_t)initVal << std::endl;
#endif
}

void pl_ucode_gen_impl::acquireLock(
    adf::tile_type tileType, uint32_t column, uint32_t row, uint32_t lockId, int32_t acqVal) {
    XAie_LocType loc = XAie_TileLoc(column, relativeToAbsoluteRow(tileType, row));
#ifdef _PLCTRL_DEBUG_
    std::cout << "To call XAie_LockAcquire "
              << "col " << (uint16_t)loc.Col << ", row " << (uint16_t)loc.Row << ", lock id " << lockId
              << ", acquire value " << (uint16_t)acqVal << std::endl;
#endif
    XAie_Lock Lock = XAie_LockInit(lockId, acqVal);
    uint8_t TileType;
    const XAie_LockMod* LockMod;
    if (DevInst.IsReady != XAIE_COMPONENT_IS_READY) {
        printf("Invalid Device Instance\n");
    }

    TileType = DevInst.DevOps->GetTTypefromLoc(&DevInst, loc);
    if (TileType == XAIEGBL_TILE_TYPE_SHIMPL) {
        printf("Invalid Tile Type\n");
    }

    LockMod = DevInst.DevProp.DevMod[TileType].LockMod;
    if (Lock.LockId > LockMod->NumLocks) {
        printf("Invalid Lock Id\n");
    }

    if ((Lock.LockVal > LockMod->LockValUpperBound) || (Lock.LockVal < LockMod->LockValLowerBound)) {
        printf("Lock value out of range\n");
    }

    uint32_t RegOff = LockMod->BaseAddr + (Lock.LockId * LockMod->LockIdOff) + (LockMod->RelAcqOff) +
                      ((Lock.LockVal & XAIEML_LOCK_VALUE_MASK) << XAIEML_LOCK_VALUE_SHIFT);
    uint64_t RegAddr = _XAie_GetTileAddr(&DevInst, loc.Row, loc.Col) + RegOff;

    acqLockToBuff(UCODE_ORDER_DISABLE, UCODE_CHAN_DEFAULT, RegAddr, XAIEML_LOCK_RESULT_MASK,
                  (XAIEML_LOCK_RESULT_SUCCESS << XAIEML_LOCK_RESULT_LSB));
// XAie_MaskPoll(DevInst, RegAddr, XAIEML_LOCK_RESULT_MASK,
// (XAIEML_LOCK_RESULT_SUCCESS << XAIEML_LOCK_RESULT_LSB),
//               TimeOut) != XAIE_O
#ifdef _PLCTRL_DEBUG_
    std::cout << "XAie_LockAcquire "
              << "col " << (uint16_t)loc.Col << ", row " << (uint16_t)loc.Row << ", lock id " << lockId
              << ", acquire value " << (uint16_t)acqVal << std::endl;
#endif
}

int pl_ucode_gen_impl::setDmaBd(XAie_DmaDesc* dmaDesc, XAie_LocType loc, uint8_t bdNum, int64_t rtId) {
    // mem tile
    if (loc.Row != 0) {
        if (rtId < 0) {
            printf("Only shim tile support configuration of runtime buffer\n");
            return -1;
        }
        XAie_AieMlDimDesc* dDesc = dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc;
        XAie_PadDesc* pDesc = dmaDesc->PadDesc;

        for (uint8_t dim = 0U; dim < DMA_PAD_NUM_DIMS; dim++) {
            if (dDesc[dim].Wrap == 0U) {
                if (pDesc[dim].After != 0U) {
                    printf("Padding after for dimension %u must be 0 when wrap is 1\n", dim);
                    return -1;
                }

                for (int padDim = dim + 1; padDim < DMA_PAD_NUM_DIMS; padDim++) {
                    if ((pDesc[padDim].After != 0U) || (pDesc[padDim].Before != 0U)) {
                        printf(
                            "After and Before pading for dimension %u must be 0 when "
                            "wrap for dimension %u is 0\n",
                            padDim, dim);
                        return -1;
                    }
                }
            }
        }

        const XAie_DmaMod* dmaMod = DevInst.DevProp.DevMod[dmaDesc->TileType].DmaMod;
        const XAie_DmaBdProp* bdProp = dmaMod->BdProp;

        uint64_t bdBaseAddr = dmaMod->BaseAddr + bdNum * dmaMod->IdxOffset;

        uint32_t bdWord[MEMTILEDMA_NUM_BD_WORDS];
        /* Setup bdWord with the right values from DmaDesc */
        bdWord[0U] = XAie_SetField(dmaDesc->PktDesc.PktEn, bdProp->Pkt->EnPkt.Lsb, bdProp->Pkt->EnPkt.Mask) |
                     XAie_SetField(dmaDesc->PktDesc.PktType, bdProp->Pkt->PktType.Lsb, bdProp->Pkt->PktType.Mask) |
                     XAie_SetField(dmaDesc->PktDesc.PktId, bdProp->Pkt->PktId.Lsb, bdProp->Pkt->PktId.Mask) |
                     XAie_SetField(dmaDesc->BdEnDesc.OutofOrderBdId, bdProp->BdEn->OutofOrderBdId.Lsb,
                                   bdProp->BdEn->OutofOrderBdId.Mask) |
                     XAie_SetField(dmaDesc->AddrDesc.Length, bdProp->Buffer->TileDmaBuff.BufferLen.Lsb,
                                   bdProp->Buffer->TileDmaBuff.BufferLen.Mask);

        bdWord[1U] =
            XAie_SetField(dmaDesc->PadDesc[0U].Before, bdProp->Pad->D0_PadBefore.Lsb, bdProp->Pad->D0_PadBefore.Mask) |
            XAie_SetField(dmaDesc->BdEnDesc.NxtBd, bdProp->BdEn->NxtBd.Lsb, bdProp->BdEn->NxtBd.Mask) |
            XAie_SetField(dmaDesc->BdEnDesc.UseNxtBd, bdProp->BdEn->UseNxtBd.Lsb, bdProp->BdEn->UseNxtBd.Mask) |
            XAie_SetField(dmaDesc->AddrDesc.Address, bdProp->Buffer->TileDmaBuff.BaseAddr.Lsb,
                          bdProp->Buffer->TileDmaBuff.BaseAddr.Mask);

        bdWord[2U] =
            XAie_SetField(dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[0U].Wrap,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[0U].Wrap.Lsb,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[0U].Wrap.Mask) |
            XAie_SetField((dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[0U].StepSize - 1U),
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[0U].StepSize.Lsb,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[0U].StepSize.Mask) |
            XAie_SetField(dmaDesc->TlastSuppress, bdProp->BdEn->TlastSuppress.Lsb, bdProp->BdEn->TlastSuppress.Mask);

        bdWord[3U] =
            XAie_SetField(dmaDesc->PadDesc[1U].Before, bdProp->Pad->D1_PadBefore.Lsb, bdProp->Pad->D1_PadBefore.Mask) |
            XAie_SetField(dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[1U].Wrap,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[1U].Wrap.Lsb,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[1U].Wrap.Mask) |
            XAie_SetField((dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[1U].StepSize - 1U),
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[1U].StepSize.Lsb,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[1U].StepSize.Mask);

        bdWord[4U] =
            XAie_SetField(dmaDesc->PadDesc[2U].Before, bdProp->Pad->D2_PadBefore.Lsb, bdProp->Pad->D2_PadBefore.Mask) |
            XAie_SetField(dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[2U].Wrap,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[2U].Wrap.Lsb,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[2U].Wrap.Mask) |
            XAie_SetField((dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[2U].StepSize - 1U),
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[2U].StepSize.Lsb,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[2U].StepSize.Mask) |
            XAie_SetField(dmaDesc->EnCompression, bdProp->Compression->EnCompression.Lsb,
                          bdProp->Compression->EnCompression.Mask);

        bdWord[5U] =
            XAie_SetField(dmaDesc->PadDesc[2U].After, bdProp->Pad->D2_PadAfter.Lsb, bdProp->Pad->D2_PadAfter.Mask) |
            XAie_SetField(dmaDesc->PadDesc[1U].After, bdProp->Pad->D1_PadAfter.Lsb, bdProp->Pad->D1_PadAfter.Mask) |
            XAie_SetField(dmaDesc->PadDesc[0U].After, bdProp->Pad->D0_PadAfter.Lsb, bdProp->Pad->D0_PadAfter.Mask) |
            XAie_SetField((dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[3U].StepSize - 1U),
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[3U].StepSize.Lsb,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[3U].StepSize.Mask);

        bdWord[6U] = XAie_SetField(dmaDesc->MultiDimDesc.AieMlMultiDimDesc.IterCurr,
                                   bdProp->AddrMode->AieMlMultiDimAddr.IterCurr.Lsb,
                                   bdProp->AddrMode->AieMlMultiDimAddr.IterCurr.Mask) |
                     XAie_SetField((dmaDesc->MultiDimDesc.AieMlMultiDimDesc.IterDesc.Wrap - 1U),
                                   bdProp->AddrMode->AieMlMultiDimAddr.Iter.Wrap.Lsb,
                                   bdProp->AddrMode->AieMlMultiDimAddr.Iter.Wrap.Mask) |
                     XAie_SetField((dmaDesc->MultiDimDesc.AieMlMultiDimDesc.IterDesc.StepSize - 1U),
                                   bdProp->AddrMode->AieMlMultiDimAddr.Iter.StepSize.Lsb,
                                   bdProp->AddrMode->AieMlMultiDimAddr.Iter.StepSize.Mask);

        bdWord[7U] = XAie_SetField(dmaDesc->BdEnDesc.ValidBd, bdProp->BdEn->ValidBd.Lsb, bdProp->BdEn->ValidBd.Mask) |
                     XAie_SetField(dmaDesc->LockDesc.LockRelVal, bdProp->Lock->AieMlDmaLock.LckRelVal.Lsb,
                                   bdProp->Lock->AieMlDmaLock.LckRelVal.Mask) |
                     XAie_SetField(dmaDesc->LockDesc.LockRelId, bdProp->Lock->AieMlDmaLock.LckRelId.Lsb,
                                   bdProp->Lock->AieMlDmaLock.LckRelId.Mask) |
                     XAie_SetField(dmaDesc->LockDesc.LockAcqId, bdProp->Lock->AieMlDmaLock.LckAcqId.Lsb,
                                   bdProp->Lock->AieMlDmaLock.LckAcqId.Mask) |
                     XAie_SetField(dmaDesc->LockDesc.LockAcqVal, bdProp->Lock->AieMlDmaLock.LckAcqVal.Lsb,
                                   bdProp->Lock->AieMlDmaLock.LckAcqVal.Mask) |
                     XAie_SetField(dmaDesc->LockDesc.LockAcqEn, bdProp->Lock->AieMlDmaLock.LckAcqEn.Lsb,
                                   bdProp->Lock->AieMlDmaLock.LckAcqEn.Mask);

        uint64_t addr = bdBaseAddr + _XAie_GetTileAddr(&DevInst, loc.Row, loc.Col);

#ifdef _PLCTRL_DEBUG_
        printf("setDmaBd: addr %d\n", addr);
#endif
        setBdToBuff(UCODE_ORDER_DISABLE, UCODE_CHAN_DEFAULT, UCODE_RT_DISABLE, UCODE_RTID_DEFAULT, addr,
                    MEMTILEDMA_NUM_BD_WORDS, bdWord);
        for (uint8_t i = 0; i < MEMTILEDMA_NUM_BD_WORDS; i++) {
#ifdef _PLCTRL_DEBUG_
            printf("setDmaBd: addr %u, value: %u\n", addr, bdWord[i]);
#endif
        }
#ifdef _PLCTRL_DEBUG_
        std::cout << "buff.size(): " << buff.size() << std::endl;
#endif
        return 0;
    } else {
        // shim tile
        if (rtId >= 0) {
            printf("Only shim tile support configuration of runtime buffer\n");
            return -1;
        }
        const XAie_DmaMod* dmaMod = DevInst.DevProp.DevMod[dmaDesc->TileType].DmaMod;
        const XAie_DmaBdProp* bdProp = dmaMod->BdProp;

        uint64_t bdBaseAddr = dmaMod->BaseAddr + bdNum * dmaMod->IdxOffset;

        uint32_t bdWord[SHIMDMA_NUM_BD_WORDS];
        /* Setup bdWord with the right values from dmaDesc */
        bdWord[0U] = XAie_SetField(dmaDesc->AddrDesc.Length, bdProp->Buffer->ShimDmaBuff.BufferLen.Lsb,
                                   bdProp->Buffer->ShimDmaBuff.BufferLen.Mask);

        bdWord[1U] = 0;
        bdWord[2U] = XAie_SetField(dmaDesc->PktDesc.PktEn, bdProp->Pkt->EnPkt.Lsb, bdProp->Pkt->EnPkt.Mask) |
                     XAie_SetField(dmaDesc->PktDesc.PktType, bdProp->Pkt->PktType.Lsb, bdProp->Pkt->PktType.Mask) |
                     XAie_SetField(dmaDesc->PktDesc.PktId, bdProp->Pkt->PktId.Lsb, bdProp->Pkt->PktId.Mask) |
                     XAie_SetField(dmaDesc->BdEnDesc.OutofOrderBdId, bdProp->BdEn->OutofOrderBdId.Lsb,
                                   bdProp->BdEn->OutofOrderBdId.Mask);
        bdWord[3U] = XAie_SetField(dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[0U].Wrap,
                                   bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[0U].Wrap.Lsb,
                                   bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[0U].Wrap.Mask) |
                     XAie_SetField((dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[0U].StepSize - 1U),
                                   bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[0U].StepSize.Lsb,
                                   bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[0U].StepSize.Mask) |
                     XAie_SetField(dmaDesc->AxiDesc.SecureAccess, bdProp->SysProp->SecureAccess.Lsb,
                                   bdProp->SysProp->SecureAccess.Mask);
        bdWord[4U] =
            XAie_SetField(dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[1U].Wrap,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[1U].Wrap.Lsb,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[1U].Wrap.Mask) |
            XAie_SetField((dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[1U].StepSize - 1U),
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[1U].StepSize.Lsb,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[1U].StepSize.Mask) |
            XAie_SetField(dmaDesc->AxiDesc.BurstLen, bdProp->SysProp->BurstLen.Lsb, bdProp->SysProp->BurstLen.Mask);

        bdWord[5U] =
            XAie_SetField(dmaDesc->AxiDesc.SMID, bdProp->SysProp->SMID.Lsb, bdProp->SysProp->SMID.Mask) |
            XAie_SetField(dmaDesc->AxiDesc.AxQos, bdProp->SysProp->AxQos.Lsb, bdProp->SysProp->AxQos.Mask) |
            XAie_SetField(dmaDesc->AxiDesc.AxCache, bdProp->SysProp->AxCache.Lsb, bdProp->SysProp->AxCache.Mask) |
            XAie_SetField((dmaDesc->MultiDimDesc.AieMlMultiDimDesc.DimDesc[2U].StepSize - 1U),
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[2U].StepSize.Lsb,
                          bdProp->AddrMode->AieMlMultiDimAddr.DmaDimProp[2U].StepSize.Mask);

        bdWord[6U] = XAie_SetField(dmaDesc->MultiDimDesc.AieMlMultiDimDesc.IterCurr,
                                   bdProp->AddrMode->AieMlMultiDimAddr.IterCurr.Lsb,
                                   bdProp->AddrMode->AieMlMultiDimAddr.IterCurr.Mask) |
                     XAie_SetField((dmaDesc->MultiDimDesc.AieMlMultiDimDesc.IterDesc.Wrap - 1U),
                                   bdProp->AddrMode->AieMlMultiDimAddr.Iter.Wrap.Lsb,
                                   bdProp->AddrMode->AieMlMultiDimAddr.Iter.Wrap.Mask) |
                     XAie_SetField((dmaDesc->MultiDimDesc.AieMlMultiDimDesc.IterDesc.StepSize - 1U),
                                   bdProp->AddrMode->AieMlMultiDimAddr.Iter.StepSize.Lsb,
                                   bdProp->AddrMode->AieMlMultiDimAddr.Iter.StepSize.Mask);

        bdWord[7U] =
            XAie_SetField(dmaDesc->BdEnDesc.ValidBd, bdProp->BdEn->ValidBd.Lsb, bdProp->BdEn->ValidBd.Mask) |
            XAie_SetField(dmaDesc->LockDesc.LockRelVal, bdProp->Lock->AieMlDmaLock.LckRelVal.Lsb,
                          bdProp->Lock->AieMlDmaLock.LckRelVal.Mask) |
            XAie_SetField(dmaDesc->LockDesc.LockRelId, bdProp->Lock->AieMlDmaLock.LckRelId.Lsb,
                          bdProp->Lock->AieMlDmaLock.LckRelId.Mask) |
            XAie_SetField(dmaDesc->LockDesc.LockAcqId, bdProp->Lock->AieMlDmaLock.LckAcqId.Lsb,
                          bdProp->Lock->AieMlDmaLock.LckAcqId.Mask) |
            XAie_SetField(dmaDesc->LockDesc.LockAcqVal, bdProp->Lock->AieMlDmaLock.LckAcqVal.Lsb,
                          bdProp->Lock->AieMlDmaLock.LckAcqVal.Mask) |
            XAie_SetField(dmaDesc->LockDesc.LockAcqEn, bdProp->Lock->AieMlDmaLock.LckAcqEn.Lsb,
                          bdProp->Lock->AieMlDmaLock.LckAcqEn.Mask) |
            XAie_SetField(dmaDesc->BdEnDesc.UseNxtBd, bdProp->BdEn->UseNxtBd.Lsb, bdProp->BdEn->UseNxtBd.Mask) |
            XAie_SetField(dmaDesc->BdEnDesc.NxtBd, bdProp->BdEn->NxtBd.Lsb, bdProp->BdEn->NxtBd.Mask) |
            XAie_SetField(dmaDesc->TlastSuppress, bdProp->BdEn->TlastSuppress.Lsb, bdProp->BdEn->TlastSuppress.Mask);

        uint32_t addr = bdBaseAddr + _XAie_GetTileAddr(&DevInst, loc.Row, loc.Col);

#ifdef _PLCTRL_DEBUG_
        printf("setDmaBd: addr %d\n", addr);
#endif

        uint32_t bd_addr_id = std::abs(rtId / 4 + 1);
        setBdToBuff(UCODE_ORDER_DISABLE, UCODE_CHAN_DEFAULT, UCODE_RT_ENABLE, bd_addr_id, addr, MEMTILEDMA_NUM_BD_WORDS,
                    bdWord);
        for (uint8_t i = 0; i < MEMTILEDMA_NUM_BD_WORDS; i++) {
#ifdef _PLCTRL_DEBUG_
            printf("setDmaBd: addr %u, value: %u\n", addr, bdWord[i]);
#endif
        }

        return 0;
    }
}

void pl_ucode_gen_impl::configureBufferDescriptor(adf::tile_type tileType,
                                                  uint32_t col,
                                                  uint32_t row,
                                                  uint32_t bdId,
                                                  const adf::dma_buffer_descriptor& bdParam,
                                                  uint32_t bytes,
                                                  uint32_t burst_length) {
    int driverStatus = XAIE_OK; // 0
    XAie_DmaDesc dmaInst;
    XAie_LocType tileLoc = XAie_TileLoc(col, relativeToAbsoluteRow(tileType, row));

    driverStatus |= XAie_DmaDescInit(&DevInst, &dmaInst, tileLoc);

    // address, length, stepsize, wrap,
    std::vector<XAie_DmaDimDesc> dimDescs;
    for (int j = 0; j < bdParam.stepsize.size(); j++) {
        XAie_DmaDimDesc dimDesc;
        dimDesc.AieMlDimDesc = {bdParam.stepsize[j], ((j < bdParam.wrap.size()) ? bdParam.wrap[j] : 0)};
        dimDescs.push_back(dimDesc);
#ifdef _PLCTRL_DEBUG_
        std::cout << "D" << j << " stepsize " << bdParam.stepsize[j] << ", wrap "
                  << ((j < bdParam.wrap.size()) ? bdParam.wrap[j] : 0) << std::endl;
#endif
    }
    XAie_DmaTensor dims = {(uint8_t)dimDescs.size(), dimDescs.data()};
    driverStatus |= XAie_DmaSetMultiDimAddr(&dmaInst, &dims, bdParam.address * bytes, bdParam.length * bytes);
#ifdef _PLCTRL_DEBUG_
    std::cout << "address " << std::hex << bdParam.address * bytes << std::dec << ", length " << bdParam.length * bytes
              << std::endl;
#endif

    // zero padding
    for (int j = 0; j < bdParam.padding.size(); j++) {
        driverStatus |= XAie_DmaSetZeroPadding(&dmaInst, j, DMA_ZERO_PADDING_BEFORE, bdParam.padding[j].first);
        driverStatus |= XAie_DmaSetZeroPadding(&dmaInst, j, DMA_ZERO_PADDING_AFTER, bdParam.padding[j].second);
#ifdef _PLCTRL_DEBUG_
        std::cout << "D" << j << " zero before " << bdParam.padding[j].first << ", zero after "
                  << bdParam.padding[j].second << std::endl;
        ;
#endif
    }

    // packet id
    if (bdParam.enable_packet) {
        driverStatus |= XAie_DmaSetPkt(&dmaInst, {.PktId = bdParam.packet_id, .PktType = 0});
        driverStatus |= XAie_DmaSetOutofOrderBdId(&dmaInst, bdParam.out_of_order_bd_id);
    }
#ifdef _PLCTRL_DEBUG_
    std::cout << "enable packet " << bdParam.enable_packet << ", packet id " << (uint16_t)bdParam.packet_id
              << ", out_of_order_bd_id " << (uint16_t)bdParam.out_of_order_bd_id << std::endl;
#endif

    // tlast suppress
    if (bdParam.tlast_suppress) driverStatus |= XAie_DmaTlastDisable(&dmaInst);
#ifdef _PLCTRL_DEBUG_
    std::cout << "tlast suppress " << bdParam.tlast_suppress << std::endl;
#endif

    // iteration
    if (bdParam.iteration_stepsize > 0 || bdParam.iteration_wrap > 0 || bdParam.iteration_current > 0)
        driverStatus |= XAie_DmaSetBdIteration(&dmaInst, bdParam.iteration_stepsize, bdParam.iteration_wrap,
                                               bdParam.iteration_current);
#ifdef _PLCTRL_DEBUG_
    std::cout << "iteration stepsize " << bdParam.iteration_stepsize << ", iteration wrap " << bdParam.iteration_wrap
              << ", iteration current " << (uint16_t)bdParam.iteration_current << std::endl;
#endif

    // enable compression
    if (bdParam.enable_compression) driverStatus |= XAie_DmaEnableCompression(&dmaInst);
#ifdef _PLCTRL_DEBUG_
    std::cout << "enable compression " << bdParam.enable_compression << std::endl;
#endif

    // lock
    if (bdParam.lock_acq_enable)
        driverStatus |= XAie_DmaSetLock(&dmaInst, XAie_LockInit(bdParam.lock_acq_id, bdParam.lock_acq_value),
                                        XAie_LockInit(bdParam.lock_rel_id, bdParam.lock_rel_value));
#ifdef _PLCTRL_DEBUG_
    std::cout << "lock_acq_enable " << bdParam.lock_acq_enable << ", lock_acq_id " << (uint16_t)bdParam.lock_acq_id
              << ", lock_acq_value " << (int)bdParam.lock_acq_value << ", lock_rel_id " << (uint16_t)bdParam.lock_rel_id
              << ", lock_rel_value " << (int)bdParam.lock_rel_value << std::endl;
#endif

    // burst length
    if (tileLoc.Row == 0) // shim tile
    {
        driverStatus |=
            XAie_DmaSetAxi(&dmaInst, 0 /*Smid*/, burst_length /*BurstLen*/, 0 /*Qos*/, 0 /*Cache*/, 0 /*Secure*/);
#ifdef _PLCTRL_DEBUG_
        std::cout << "burst length " << (uint16_t)burst_length << std::endl;
#endif
    }

    // next bd
    if (bdParam.use_next_bd) {
        driverStatus |= XAie_DmaSetNextBd(&dmaInst, bdParam.next_bd, XAIE_ENABLE);
#ifdef _PLCTRL_DEBUG_
        std::cout << "next bd " << (uint16_t)bdParam.next_bd << std::endl;
#endif
    }

    // valid bd
    driverStatus |= XAie_DmaEnableBd(&dmaInst);

    // write bd
    driverStatus |= setDmaBd(&dmaInst, tileLoc, bdId, bdParam.address * 4);
// driverStatus |= XAie_DmaWriteBd(&DevInst, &dmaInst, tileLoc, bdIds[i]);
#ifdef _PLCTRL_DEBUG_
    std::cout << "setDmaBd " << (uint16_t)bdId << std::endl << std::endl;
#endif
}

void pl_ucode_gen_impl::enqueueTask(adf::tile_type tileType,
                                    uint32_t col,
                                    uint32_t row,
                                    adf::dma_channel_dir Dir,
                                    uint32_t ChNum,
                                    uint32_t StartBd,
                                    uint32_t repeatCount,
                                    bool enableTaskCompleteToken) {
    XAie_LocType Loc = XAie_TileLoc(col, relativeToAbsoluteRow(tileType, row));
#ifdef _PLCTRL_DEBUG_
    std::cout << "XAie_DmaChannelSetStartQueue "
              << "col " << (uint16_t)Loc.Col << ", row " << (uint16_t)Loc.Row << ", channel " << (uint16_t)ChNum
              << ", dir " << (uint16_t)Dir << ", startBD " << (uint16_t)StartBd << ", repeat count " << repeatCount
              << ", enable task complete token " << enableTaskCompleteToken << std::endl;
#endif
    uint8_t TileType = DevInst.DevOps->GetTTypefromLoc(&DevInst, Loc);
    const XAie_DmaMod* DmaMod = DevInst.DevProp.DevMod[TileType].DmaMod;
    uint32_t Val = 0;
    uint64_t Addr;

    uint64_t tileAddr = (Loc.Row << DevInst.DevProp.RowShift) | (Loc.Col << DevInst.DevProp.ColShift);
    Addr = tileAddr + DmaMod->StartQueueBase + ChNum * DmaMod->ChIdxOffset +
           Dir * DmaMod->ChIdxOffset * DmaMod->NumChannels;
    Val = XAie_SetField(StartBd, DmaMod->ChProp->StartBd.Lsb, DmaMod->ChProp->StartBd.Mask) |
          XAie_SetField((repeatCount - 1), DmaMod->ChProp->RptCount.Lsb, DmaMod->ChProp->RptCount.Mask) |
          XAie_SetField(enableTaskCompleteToken, DmaMod->ChProp->EnToken.Lsb, DmaMod->ChProp->EnToken.Mask);

#ifdef _PLCTRL_DEBUG_
    printf("enqueueTask: addr %d, value: %d\n", Addr, Val);
#endif
    enqBdToBuff(UCODE_ORDER_ENABLE, UCODE_CHAN_DEFAULT, Addr, Val);
}

void pl_ucode_gen_impl::waitDMAChannelTaskQueue(
    adf::tile_type tileType, uint32_t column, uint32_t row, adf::dma_channel_dir dir, uint32_t channel) {
    XAie_LocType loc = XAie_TileLoc(column, relativeToAbsoluteRow(tileType, row));
    int driverStatus = XAIE_OK;
#ifdef _PLCTRL_DEBUG_
    std::cout << "To call XAie_DmaGetPendingBdCount "
              << "col " << (uint16_t)loc.Col << ", row " << (uint16_t)loc.Row << ", channel " << (uint16_t)channel
              << ", dir " << dir << std::endl;
#endif

    uint8_t numPendingBDs = 4;
    uint8_t TileType;
    const XAie_DmaMod* DmaMod;
    if (DevInst.IsReady != XAIE_COMPONENT_IS_READY) {
        printf("Invalid Device Instance\n");
    }

    if (dir >= DMA_MAX) {
        printf("Invalid DMA direction\n");
    }

    TileType = DevInst.DevOps->GetTTypefromLoc(&DevInst, loc);
    if (TileType == XAIEGBL_TILE_TYPE_SHIMPL) {
        printf("Invalid Tile Type\n");
    }

    DmaMod = DevInst.DevProp.DevMod[TileType].DmaMod;
    if (channel > DmaMod->NumChannels) {
        printf("Invalid Channel number\n");
    }

    uint64_t addr = _XAie_GetTileAddr(&DevInst, loc.Row, loc.Col) + DmaMod->ChStatusBase +
                    channel * DMA_STATUS_CHNUM_OFFSET + dir * DmaMod->ChStatusOffset;

    uint32_t Mask = DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.Status.Mask |
                    DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.StalledLockAcq.Mask |
                    DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.StalledLockRel.Mask |
                    DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.StalledStreamStarve.Mask |
                    DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.StalledTCT.Mask;
    DmaWaitQueToBuff(UCODE_ORDER_DISABLE, UCODE_CHAN_DEFAULT, addr,
                     DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.TaskQSize.Mask,
                     DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.TaskQSize.Lsb, Mask);
}

void pl_ucode_gen_impl::waitDMAChannelDone(
    adf::tile_type tileType, uint32_t column, uint32_t row, adf::dma_channel_dir dir, uint32_t channel) {
    XAie_LocType loc = XAie_TileLoc(column, relativeToAbsoluteRow(tileType, row));
#ifdef _PLCTRL_DEBUG_
    std::cout << "To call XAie_DmaWaitForDone "
              << "col " << (uint16_t)loc.Col << ", row " << (uint16_t)loc.Row << ", channel " << (uint16_t)channel
              << ", dir " << dir << std::endl;
#endif
    uint8_t TileType = DevInst.DevOps->GetTTypefromLoc(&DevInst, loc);
    const XAie_DmaMod* DmaMod = DevInst.DevProp.DevMod[TileType].DmaMod;
    uint64_t Addr;
    uint32_t Mask, Value;
    uint64_t tileAddr = (loc.Row << DevInst.DevProp.RowShift) | (loc.Col << DevInst.DevProp.ColShift);
    Addr = tileAddr + DmaMod->ChStatusBase + channel * 0x4 + dir * DmaMod->ChStatusOffset;
    Mask = DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.Status.Mask |
           DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.TaskQSize.Mask |
           DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.StalledLockAcq.Mask |
           DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.StalledLockRel.Mask |
           DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.StalledStreamStarve.Mask |
           DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.StalledTCT.Mask;
    Value = 0x0 << DmaMod->ChProp->DmaChStatus->AieMlDmaChStatus.Status.Lsb;

    DmaWaitToBuff(UCODE_ORDER_DISABLE, UCODE_CHAN_DEFAULT, Addr, Mask, Value);
#ifdef _PLCTRL_DEBUG_
    std::cout << "waitDMAChannelDone WAIT_ON_DMA=" << WAIT_ON_DMA << ", Addr=" << Addr << ", Mask=" << Mask
              << ", Value=" << Value << std::endl;
#endif
}

void pl_ucode_gen_impl::releaseLock(
    adf::tile_type tileType, uint32_t column, uint32_t row, uint32_t lockId, int32_t relVal) {
    XAie_LocType loc = XAie_TileLoc(column, relativeToAbsoluteRow(tileType, row));
    XAie_Lock Lock = XAie_LockInit(lockId, relVal);
    uint8_t TileType;
    const XAie_LockMod* LockMod;
    if (DevInst.IsReady != XAIE_COMPONENT_IS_READY) {
        printf("Invalid Device Instance\n");
    }

    TileType = DevInst.DevOps->GetTTypefromLoc(&DevInst, loc);
    if (TileType == XAIEGBL_TILE_TYPE_SHIMPL) {
        printf("Invalid Tile Type\n");
    }

    LockMod = DevInst.DevProp.DevMod[TileType].LockMod;
    if (Lock.LockId > LockMod->NumLocks) {
        printf("Invalid Lock Id\n");
    }

    if ((Lock.LockVal > LockMod->LockValUpperBound) || (Lock.LockVal < LockMod->LockValLowerBound)) {
        printf("Lock value out of range\n");
    }

    uint32_t RegOff = LockMod->BaseAddr + (Lock.LockId * LockMod->LockIdOff) +
                      ((Lock.LockVal & XAIEML_LOCK_VALUE_MASK) << XAIEML_LOCK_VALUE_SHIFT);
    uint64_t RegAddr = _XAie_GetTileAddr(&DevInst, loc.Row, loc.Col) + RegOff;

    relLockToBuff(UCODE_ORDER_DISABLE, UCODE_CHAN_DEFAULT, RegAddr, XAIEML_LOCK_RESULT_MASK,
                  (XAIEML_LOCK_RESULT_SUCCESS << XAIEML_LOCK_RESULT_LSB));
// XAie_MaskPoll(DevInst, RegAddr, XAIEML_LOCK_RESULT_MASK,
// (XAIEML_LOCK_RESULT_SUCCESS << XAIEML_LOCK_RESULT_LSB),
//               TimeOut) != XAIE_O
#ifdef _PLCTRL_DEBUG_
    std::cout << "XAie_LockRelease "
              << "col " << (uint16_t)loc.Col << ", row " << (uint16_t)loc.Row << ", lock id " << lockId
              << ", release value " << (uint16_t)acqVal << std::endl;
#endif
}

uint64_t pl_ucode_gen_impl::getExternalBufferAddress(std::string& externalBufferName) {
    // std::cout << "getExternalBufferAddress: name=" << externalBufferName << std::endl;
    uint64_t index;
    bool flag = false;
    for (int id = 0; id < ext_name.size(); id++) {
        if (ext_name[id] == externalBufferName) {
            index = (uint64_t)(-1) - (uint64_t)(id * 4);
            flag = true;
            break;
        }
    }
    if (!flag) {
        int id = ext_name.size();
        ext_name.push_back(externalBufferName);
        index = (uint64_t)(-1) - (uint64_t)(id * 4);
    }
    // returning a negative number means that the id is returned instead of the actual address
    return index;
}

void pl_ucode_gen_impl::blockWrite(
    adf::tile_type tileType, uint8_t column, uint8_t row, uint32_t address, const std::string& file, uint32_t size) {
    XAie_LocType Loc = XAie_TileLoc(column, relativeToAbsoluteRow(tileType, row));
    // int driverStatus = XAie_DataMemBlockWrite(config_manager::s_pDevInst, tileLoc, address, pData, size);
    if (DevInst.IsReady != XAIE_COMPONENT_IS_READY) {
        std::cout << "Invalid device instance or source pointer\n";
    }
    uint8_t TileType = DevInst.DevOps->GetTTypefromLoc(&DevInst, Loc);
    if ((TileType != XAIEGBL_TILE_TYPE_AIETILE) && (TileType != XAIEGBL_TILE_TYPE_MEMTILE)) {
        std::cout << "Invalid tile type\n";
    }
    const XAie_MemMod* MemMod = DevInst.DevProp.DevMod[TileType].MemMod;
    /* Check for any size overflow */
    if ((uint64_t)address + size > MemMod->Size) {
        std::cout << "Size of source block overflows tile data memory\n";
    }
    uint8_t FirstWriteOffset = address & XAIE_MEM_WORD_ALIGN_MASK;
    uint64_t DmAddrRoundDown =
        MemMod->MemAddr + XAIE_MEM_WORD_ROUND_DOWN(address) + _XAie_GetTileAddr(&DevInst, Loc.Row, Loc.Col);
    uint64_t DmAddrRoundUp =
        MemMod->MemAddr + XAIE_MEM_WORD_ROUND_UP(address) + _XAie_GetTileAddr(&DevInst, Loc.Row, Loc.Col);
    uint32_t BytePtr = 0;
    /* Unaligned start bytes */
    if (FirstWriteOffset) {
        std::cout << "ERROR TODO\n";
        exit(-1);
    }
    // std::cout << "FirstWriteOffset=" << (int)FirstWriteOffset << ", DmAddrRoundDown=" << DmAddrRoundDown
    //          << ", DmAddrRoundUp=" << DmAddrRoundUp << std::endl;
    /* Aligned bytes */
    // XAie_BlockWrite32(DevInst, DmAddrRoundUp, (const u32*)(CharSrc + BytePtr), (RemBytes /
    // XAIE_MEM_WORD_ALIGN_SIZE));
    std::ifstream fin(file, std::ios::binary);
    if (!fin) {
        std::cout << "[ERROR] blockWrite: open file fail!\n";
        exit(-1);
    }
    uint32_t RemBytes = size;
    uint32_t valBuf[RemBytes / XAIE_MEM_WORD_ALIGN_SIZE];
    fin.read((char*)valBuf, RemBytes);
    fin.close();
    writeMMToBuff(UCODE_ORDER_DISABLE, UCODE_CHAN_DEFAULT, DmAddrRoundUp, RemBytes / XAIE_MEM_WORD_ALIGN_SIZE, valBuf);
    /* Remaining unaligned bytes */
    if (RemBytes % XAIE_MEM_WORD_ALIGN_SIZE) {
        std::cout << "ERROR TODO\n";
        exit(-1);
    }

    std::cout << "XAie_DataMemBlockWrite "
              << "col " << (uint16_t)Loc.Col << ", row " << (uint16_t)Loc.Row << ", address 0x" << std::hex << address
              << std::dec << ", size " << size << std::endl;
}

void pl_ucode_gen_impl::writeLayerParams(const std::vector<adf::LayerParamsLocation> LayerParamsLocs) {
    for (int i = 0; i < LayerParamsLocs.size(); i++) {
        std::string file;
        if (LayerParamsLocs[i].path[0] == '/') // absolute path
            file = LayerParamsLocs[i].path;
        else // relative path
            file = work_path + LayerParamsLocs[i].path;
        std::ifstream fin(file, std::ios::binary);
        if (!fin) {
            std::cout << "[ERROR] writeLayerParams[" << i << "]: open " << file << " fail!\n";
            exit(-1);
        }
        uint32_t d = fin.tellg();
        fin.seekg(0, std::ios::end);
        uint32_t u = fin.tellg();
        fin.close();
        uint32_t sizeInBytes = u - d;
        // std::cout << "sizeInBytes=" << sizeInBytes << std::endl;
        adf::acquireLock(adf::aie_tile, LayerParamsLocs[i].column, LayerParamsLocs[i].row, LayerParamsLocs[i].acqLockId,
                         LayerParamsLocs[i].acqVal);
        blockWrite(adf::aie_tile, LayerParamsLocs[i].column, LayerParamsLocs[i].row, LayerParamsLocs[i].address, file,
                   sizeInBytes);
        adf::releaseLock(adf::aie_tile, LayerParamsLocs[i].column, LayerParamsLocs[i].row, LayerParamsLocs[i].relLockId,
                         LayerParamsLocs[i].relVal);
    }
}

void pl_ucode_gen_impl::parseControlConfig(std::string& file) {
    std::cout << "json file=" << file << std::endl;
    std::ifstream input_file(file);
    if (!input_file.is_open()) {
        std::cout << "Could not open the file: " << file << std::endl;
    }
    std::string json_str = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    rapidjson::Document doc;
    doc.Parse(json_str.c_str());
    rapidjson::Value& val = doc["aie_metadata"]["graphs"]["graph0"];
    rapidjson::Value& aie_row_start = doc["aie_metadata"]["driver_config"]["aie_tile_row_start"];
    aie_start_row = aie_row_start.GetInt();
    // aie core enable
    rapidjson::Value& core_cols = val["core_columns"];
    rapidjson::Value& core_rows = val["core_rows"];
    for (int i = 0; i < core_cols.Size(); i++) {
        uint32_t addr = 0;
        uint32_t cols = core_cols[i].GetInt();
        addr += (cols << 25); // column offset
        uint32_t rows = core_rows[i].GetInt() + aie_start_row;
        addr += (rows << 20); // row offset
        addr += 0x32000;      // register core control
        core_en.push_back(addr);
    }

    // aie core iteration
    rapidjson::Value& iter_cols = val["iteration_memory_columns"];
    rapidjson::Value& iter_rows = val["iteration_memory_rows"];
    rapidjson::Value& iter_addr = val["iteration_memory_addresses"];
    for (int i = 0; i < iter_cols.Size(); i++) {
        uint32_t addr = 0;
        uint32_t cols = iter_cols[i].GetInt();
        addr += (cols << 25);
        uint32_t rows = iter_rows[i].GetInt() + aie_start_row;
        addr += (rows << 20);
        addr += iter_addr[i].GetInt();
        core_iter.push_back(addr);
    }
}

int pl_ucode_gen_impl::write2Output(std::string& file) {
    buff.push_back(0); // end flag
    std::cout << "write2Output: buff_sz=" << buff.size() << ", map_sz=" << ext_name.size() << std::endl;
    std::ofstream fout(file, std::ios::binary);
    if (!fout) {
        std::cout << "[ERROR] write2Output: open file failure!\n";
        exit(-1);
    }
    // write ucode number and runtime buffer ID to ucode binary
    int u_sz = buff.size() * sizeof(uint32_t);
    int rt_sz = (ext_name.size() + 1) * sizeof(uint64_t);
    fout.write(reinterpret_cast<const char*>(&u_sz), sizeof(uint32_t));
    fout.write(reinterpret_cast<const char*>(&rt_sz), sizeof(uint32_t));
    for (int i = 0; i < buff.size(); i++) {
        fout.write((char*)&buff[i], sizeof(uint32_t));
    }
    int map_sz = ext_name.size();
    fout.write(reinterpret_cast<const char*>(&map_sz), sizeof(uint32_t));
    for (int i = 0; i < map_sz; i++) {
        int str_sz = ext_name[i].size();
        fout.write(reinterpret_cast<const char*>(&str_sz), sizeof(uint32_t));
        fout.write(ext_name[i].data(), str_sz);
    }
    // write core enable and iteration
    int core_iter_sz = core_iter.size();
    fout.write(reinterpret_cast<const char*>(&core_iter_sz), sizeof(uint32_t));
    for (int i = 0; i < core_iter_sz; i++) {
        fout.write((char*)&core_iter[i], sizeof(uint32_t));
    }
    int core_en_sz = core_en.size();
    fout.write(reinterpret_cast<const char*>(&core_en_sz), sizeof(uint32_t));
    for (int i = 0; i < core_en_sz; i++) {
        fout.write((char*)&core_en[i], sizeof(uint32_t));
    }
    fout.close();
    return 0;
}

uint32_t pl_ucode_gen_impl::getChan(uint32_t id) {
    uint32_t chan = 0;
    std::array tmp = chan_info[id];
    if (tmp[3] & 0x2) chan = 1;
    if ((tmp[0] == DMA_WAIT) || (tmp[0] == ACQ_LOCK)) chan = 0;
    // std::cout << "col=" << tmp[3] << ", chan=" << chan << std::endl;
    return chan;
}

void pl_ucode_gen_impl::addSyncPoint() {
    std::cout << "addSyncPoint: chan_info.size()=" << chan_info.size() << ", buff size=" << buff.size() << std::endl;
    int pc = 0, cnt = 0;
    uint16_t event_id = 64; // 0 ~ 63 reserved as other status information
    while (pc < buff.size()) {
        uint32_t opcode = buff[pc];
        uint32_t state = opcode & 0x3F;
        opcode &= 0xFFFFF87F; // clear old chan
        uint32_t chan = getChan(cnt++);
        uint32_t new_opcode = opcode | (chan << 7);
        buff[pc] = new_opcode;
        // std::cout << std::hex << "opcode1=" << opcode << ", opcode2=" << new_opcode << ", state=" << state
        //          << ", cnt=" << cnt << ", pc=" << pc << std::endl;
        pc += UcodeOffset[state];
        // add sync event
        if ((state == DMA_WAIT) || (state == ACQ_LOCK)) {
            // insert event trigger
            uint32_t header =
                ucodeHeader(EVENT_TRIGGER, UCODE_ORDER_DISABLE, chan, UCODE_RT_DISABLE, UCODE_RTID_DEFAULT);
            uint32_t ch_flag = 0;
            for (int ch = 0; ch < CH_NUM; ch++) {
                if (ch != chan) ch_flag += (1 << ch);
            }
            uint32_t data = (ch_flag << 16) + event_id;
            buff.insert(buff.begin() + pc++, header);
            buff.insert(buff.begin() + pc++, data);
            // insert event wait
            for (int ch = 0; ch < CH_NUM; ch++) {
                if (ch != chan) {
                    // std::cout << "ch=" << ch << ", chan=" << chan << std::endl;
                    header = ucodeHeader(EVENT_WAIT, UCODE_ORDER_DISABLE, ch, UCODE_RT_DISABLE, UCODE_RTID_DEFAULT);
                    buff.insert(buff.begin() + pc++, header);
                    data = event_id;
                    buff.insert(buff.begin() + pc++, data);
                }
            }
            event_id++;
        }
    }
}

namespace adf {

// implement fucnitons from adf/adf_api/AIERuntimeControl.h on x86
void configureBufferDescriptor(
    adf::tile_type tileType, uint32_t column, uint32_t row, uint32_t bdId, const adf::dma_buffer_descriptor& bd) {
    static int cnt = 0;
    std::array<uint32_t, 10> tmp = {SET_BD, cnt, tileType, column, row, bdId};
    pl_ucode_gen_impl::chan_info.push_back(tmp);
    cnt++;
    pl_ucode_gen_impl::configureBufferDescriptor(tileType, column, row, bdId, bd, 4, 4);
}

void enqueueTask(tile_type tileType,
                 uint32_t column,
                 uint32_t row,
                 adf::dma_channel_dir Dir,
                 uint32_t ChNum,
                 uint32_t StartBd,
                 uint32_t repeatCount,
                 bool enableTaskCompleteToken) {
    static int cnt = 0;
    std::array<uint32_t, 10> tmp = {ENQ_BD, cnt,   tileType, column,      row,
                                    Dir,    ChNum, StartBd,  repeatCount, enableTaskCompleteToken};
    pl_ucode_gen_impl::chan_info.push_back(tmp);
    cnt++;
    pl_ucode_gen_impl::enqueueTask(tileType, column, row, Dir, ChNum, StartBd, repeatCount, enableTaskCompleteToken);
}

void waitDMAChannelTaskQueue(
    tile_type tileType, uint32_t column, uint32_t row, adf::dma_channel_dir dir, uint32_t channel) {
    static int cnt = 0;
    std::array<uint32_t, 10> tmp = {DMA_WAIT_QUE, cnt, tileType, column, row, dir, channel};
    pl_ucode_gen_impl::chan_info.push_back(tmp);
    cnt++;
    pl_ucode_gen_impl::waitDMAChannelTaskQueue(tileType, column, row, dir, channel);
}

void waitDMAChannelDone(tile_type tileType, uint32_t column, uint32_t row, adf::dma_channel_dir dir, uint32_t channel) {
    static int cnt = 0;
    std::array<uint32_t, 10> tmp = {DMA_WAIT, cnt, tileType, column, row, dir, channel};
    pl_ucode_gen_impl::chan_info.push_back(tmp);
    cnt++;
    pl_ucode_gen_impl::waitDMAChannelDone(tileType, column, row, dir, channel);
}

void initializeLock(tile_type tileType, uint32_t column, uint32_t row, uint32_t lockId, int32_t initVal) {
    static int cnt = 0;
    std::array<uint32_t, 10> tmp = {LOCK_INIT, cnt, tileType, column, row, lockId, initVal};
    pl_ucode_gen_impl::chan_info.push_back(tmp);
    cnt++;
    pl_ucode_gen_impl::initializeLock(tileType, column, row, lockId, initVal);
}

void acquireLock(tile_type tileType, uint32_t column, uint32_t row, uint32_t lockId, int32_t acqVal) {
    static int cnt = 0;
    std::array<uint32_t, 10> tmp = {ACQ_LOCK, cnt, tileType, column, row, lockId, acqVal};
    pl_ucode_gen_impl::chan_info.push_back(tmp);
    cnt++;
    pl_ucode_gen_impl::acquireLock(tileType, column, row, lockId, acqVal);
}

void releaseLock(tile_type tileType, uint32_t column, uint32_t row, uint32_t lockId, int32_t relVal) {
    static int cnt = 0;
    std::array<uint32_t, 10> tmp = {REL_LOCK, cnt, tileType, column, row, lockId, relVal};
    pl_ucode_gen_impl::chan_info.push_back(tmp);
    cnt++;
    pl_ucode_gen_impl::releaseLock(tileType, column, row, lockId, relVal);
}

uint64_t getExternalBufferAddress(std::string externalBufferName) {
    return pl_ucode_gen_impl::getExternalBufferAddress(externalBufferName);
}

void writeLayerParams(const std::vector<LayerParamsLocation> LayerParamsLocs) {
    pl_ucode_gen_impl::writeLayerParams(LayerParamsLocs);
}

void waitLayerParams() {}

} // adf
