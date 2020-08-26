P2P FPGA to FPGA
=============
PCIe peer-to-peer communication (P2P) is a PCIe feature which enables two PCIe devices to directly transfer data between each other without using host RAM as a temporary storage. The latest version of SDx PCIe platforms support P2P feature via PCIe Resizable BAR Capability

In this Example we highlight the data transfer between FPGA and FPGA devices. The current design support P2P between 2 similar FPGA devices. User can update the makefile if need to run P2P between different FPGA cards.

### Enable/Disable P2P
XRT xbutil is used to enable/disable P2P feature and check current configuration. P2P configuration is persistent across warm reboot. Enabling or disabling P2P requires root privilege.

Enabling P2P after cold boot is likely to fail because it resizes an existing P2P PCIe BAR to a large size and usually Linux will not reserve large IO memory for the PCIe bridges. A warm reboot is required in this scenario after which BIOS and Linux will reassign the required expanded IO memory resource for P2P BAR.
Disabling P2P takes effect immediately. Currently XRT does not check if the P2P memory is in use. Administrator needs to make sure P2P is not in use before disabling it. The result of disabling P2P while it is in use is undefined.

There are three possible values for P2P Enabled field : 

| Value | Remarks |
| ------- | ------------ |
| True     | P2P is Enabled |
| False    | P2P is Disabled |
| no iomem | P2P is enabled in device but system could not allocate IO memory, warm reboot is needed |

#### Enable P2P
Enable P2P after power up sequence.
```
# xbutil p2p --enable
ERROR: resoure busy, please try warm reboot
# xbutil query
...
PCIe            DMA chan(bidir) MIG Calibrated  P2P Enabled
GEN 3x16        2               true            no iomem
# reboot
...
# xbutil query
...
PCIe            DMA chan(bidir) MIG Calibrated  P2P Enabled
GEN 3x16        2               true            true
...
```

#### Disable P2P
```
# xbutil p2p --disable
# xbutil query
...
PCIe            DMA chan(bidir) MIG Calibrated  P2P Enabled
GEN 3x16        2               true            false
...
```

