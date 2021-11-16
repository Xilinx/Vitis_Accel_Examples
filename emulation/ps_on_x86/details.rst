Embedded SW-Emulation using x86 PS
==================================

This example demonstrates how SW Emulation for Embedded platforms can run the PS App as an x86 process instead of ARM process. SW Emulation is an abstract model and does not use any of the petalinux drivers like ZOCL, interrupt controller or DTB. Hence, the overhead of creating sd_card.img, booting petalinux on full QEMU machine is too heavy weight and should be avoided for SW Emulation.


