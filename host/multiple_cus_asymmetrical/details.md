Vector Addition ~ Asymmetrical Multiple Compute Units (C)
=========================================================

This example demonstrates how multiple compute units can do parallel processing where these compute units are asymmetrical i.e.  connected to different memory banks(and potentially can perform unique tasks).

Kernel input and output ports can be connected to different memory banks for improved bandwidth. `--sp` option is used to specify such connections in `xocc` link stage.

`--sp vadd_1.in1:DDR[0] --sp vadd_1.in2:DDR[0] --sp vadd_1.out_r:DDR[0] --sp vadd_2.in1:DDR[1] --sp vadd_2.in2:DDR[1] --sp vadd_2.out_r:DDR[1]`

SDAccel allows for different kernels to be associated with different CUs by name in xocc linking stage using `--nk` option.

` --nk vadd:4:vadd_1.vadd_2.vadd_3.vadd_4`
