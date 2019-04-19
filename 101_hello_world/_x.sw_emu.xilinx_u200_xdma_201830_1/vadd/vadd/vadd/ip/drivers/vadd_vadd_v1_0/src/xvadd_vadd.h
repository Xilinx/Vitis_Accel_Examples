// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XVADD_VADD_H
#define XVADD_VADD_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xvadd_vadd_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
typedef struct {
    u16 DeviceId;
    u32 Control_BaseAddress;
} XVadd_vadd_Config;
#endif

typedef struct {
    u32 Control_BaseAddress;
    u32 IsReady;
} XVadd_vadd;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XVadd_vadd_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XVadd_vadd_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XVadd_vadd_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XVadd_vadd_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XVadd_vadd_Initialize(XVadd_vadd *InstancePtr, u16 DeviceId);
XVadd_vadd_Config* XVadd_vadd_LookupConfig(u16 DeviceId);
int XVadd_vadd_CfgInitialize(XVadd_vadd *InstancePtr, XVadd_vadd_Config *ConfigPtr);
#else
int XVadd_vadd_Initialize(XVadd_vadd *InstancePtr, const char* InstanceName);
int XVadd_vadd_Release(XVadd_vadd *InstancePtr);
#endif

void XVadd_vadd_Start(XVadd_vadd *InstancePtr);
u32 XVadd_vadd_IsDone(XVadd_vadd *InstancePtr);
u32 XVadd_vadd_IsIdle(XVadd_vadd *InstancePtr);
u32 XVadd_vadd_IsReady(XVadd_vadd *InstancePtr);
void XVadd_vadd_EnableAutoRestart(XVadd_vadd *InstancePtr);
void XVadd_vadd_DisableAutoRestart(XVadd_vadd *InstancePtr);

void XVadd_vadd_Set_in1(XVadd_vadd *InstancePtr, u64 Data);
u64 XVadd_vadd_Get_in1(XVadd_vadd *InstancePtr);
void XVadd_vadd_Set_in2(XVadd_vadd *InstancePtr, u64 Data);
u64 XVadd_vadd_Get_in2(XVadd_vadd *InstancePtr);
void XVadd_vadd_Set_out_r(XVadd_vadd *InstancePtr, u64 Data);
u64 XVadd_vadd_Get_out_r(XVadd_vadd *InstancePtr);
void XVadd_vadd_Set_size(XVadd_vadd *InstancePtr, u32 Data);
u32 XVadd_vadd_Get_size(XVadd_vadd *InstancePtr);

void XVadd_vadd_InterruptGlobalEnable(XVadd_vadd *InstancePtr);
void XVadd_vadd_InterruptGlobalDisable(XVadd_vadd *InstancePtr);
void XVadd_vadd_InterruptEnable(XVadd_vadd *InstancePtr, u32 Mask);
void XVadd_vadd_InterruptDisable(XVadd_vadd *InstancePtr, u32 Mask);
void XVadd_vadd_InterruptClear(XVadd_vadd *InstancePtr, u32 Mask);
u32 XVadd_vadd_InterruptGetEnabled(XVadd_vadd *InstancePtr);
u32 XVadd_vadd_InterruptGetStatus(XVadd_vadd *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
