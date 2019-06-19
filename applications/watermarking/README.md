Watermarking
======================

This is an optimized implementation of a watermarking application to add watermarking to images targeting execution on an SDAccel supported FPGA acceleration card.

### PERFORMANCE
Board|Image Size|Frames / Second
-----|-----|-----
xilinx:adm-pcie-ku3:2ddr-xpr|1024 x 768|2,658
xilinx:xil-accel-rd-ku115:4ddr-xpr|1024 x 768|3526
## SUPPORTED PLATFORMS
Platform | Board             | Software Version
---------|-------------------|-----------------
xilinx_u200_qdma|Xilinx Alveo U200|SDx 2019.1
xilinx_u280_xdma|Xilinx Alveo U280|SDx 2019.1
xilinx_u250_qdma|Xilinx Alveo U250|SDx 2019.1
xilinx_u200_xdma|Xilinx Alveo U200|SDx 2019.1
xilinx_u250_xdma|Xilinx Alveo U250|SDx 2019.1
xilinx_u280-es1_xdma|Xilinx Alveo U280|SDx 2019.1


##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
data/golden.bmp
data/inputImage.bmp
data/inputImage128x128.bmp
src/krnl_watermarking.cl
src/watermarking.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./watermark <apply_watermark XCLBIN> ./data/inputImage.bmp ./data/golden.bmp
```

