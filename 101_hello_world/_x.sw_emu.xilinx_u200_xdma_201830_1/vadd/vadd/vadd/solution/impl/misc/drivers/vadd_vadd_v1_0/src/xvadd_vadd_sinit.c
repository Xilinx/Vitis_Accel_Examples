// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xvadd_vadd.h"

extern XVadd_vadd_Config XVadd_vadd_ConfigTable[];

XVadd_vadd_Config *XVadd_vadd_LookupConfig(u16 DeviceId) {
	XVadd_vadd_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XVADD_VADD_NUM_INSTANCES; Index++) {
		if (XVadd_vadd_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XVadd_vadd_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XVadd_vadd_Initialize(XVadd_vadd *InstancePtr, u16 DeviceId) {
	XVadd_vadd_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XVadd_vadd_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XVadd_vadd_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

