// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xvadd_vadd.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XVadd_vadd_CfgInitialize(XVadd_vadd *InstancePtr, XVadd_vadd_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XVadd_vadd_Start(XVadd_vadd *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_AP_CTRL) & 0x80;
    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XVadd_vadd_IsDone(XVadd_vadd *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XVadd_vadd_IsIdle(XVadd_vadd *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XVadd_vadd_IsReady(XVadd_vadd *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XVadd_vadd_EnableAutoRestart(XVadd_vadd *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XVadd_vadd_DisableAutoRestart(XVadd_vadd *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_AP_CTRL, 0);
}

void XVadd_vadd_Set_in1(XVadd_vadd *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IN1_DATA, (u32)(Data));
    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IN1_DATA + 4, (u32)(Data >> 32));
}

u64 XVadd_vadd_Get_in1(XVadd_vadd *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IN1_DATA);
    Data += (u64)XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IN1_DATA + 4) << 32;
    return Data;
}

void XVadd_vadd_Set_in2(XVadd_vadd *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IN2_DATA, (u32)(Data));
    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IN2_DATA + 4, (u32)(Data >> 32));
}

u64 XVadd_vadd_Get_in2(XVadd_vadd *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IN2_DATA);
    Data += (u64)XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IN2_DATA + 4) << 32;
    return Data;
}

void XVadd_vadd_Set_out_r(XVadd_vadd *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_OUT_R_DATA, (u32)(Data));
    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_OUT_R_DATA + 4, (u32)(Data >> 32));
}

u64 XVadd_vadd_Get_out_r(XVadd_vadd *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_OUT_R_DATA);
    Data += (u64)XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_OUT_R_DATA + 4) << 32;
    return Data;
}

void XVadd_vadd_Set_size(XVadd_vadd *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_SIZE_DATA, Data);
}

u32 XVadd_vadd_Get_size(XVadd_vadd *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_SIZE_DATA);
    return Data;
}

void XVadd_vadd_InterruptGlobalEnable(XVadd_vadd *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_GIE, 1);
}

void XVadd_vadd_InterruptGlobalDisable(XVadd_vadd *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_GIE, 0);
}

void XVadd_vadd_InterruptEnable(XVadd_vadd *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IER);
    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IER, Register | Mask);
}

void XVadd_vadd_InterruptDisable(XVadd_vadd *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IER);
    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IER, Register & (~Mask));
}

void XVadd_vadd_InterruptClear(XVadd_vadd *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVadd_vadd_WriteReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_ISR, Mask);
}

u32 XVadd_vadd_InterruptGetEnabled(XVadd_vadd *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_IER);
}

u32 XVadd_vadd_InterruptGetStatus(XVadd_vadd *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XVadd_vadd_ReadReg(InstancePtr->Control_BaseAddress, XVADD_VADD_CONTROL_ADDR_ISR);
}

