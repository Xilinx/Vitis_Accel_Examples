#TA_BUILD=CustTA/2022.2_0813_1
TA_BUILD=2022.2_daily_latest
#used passed all flow, but not exist in the xsj now, only find in xco

source /proj/xbuilds/${TA_BUILD}/installs/lin64/Vitis/2022.2/settings64.sh
#source /proj/xbuilds/${TA_BUILD}/xbb/xrt/packages/setenv.sh
source /proj/picasso/2022.2_stable_latest/xbb/xrt/packages/setenv.sh

export XILINXD_LICENSE_FILE=2100@aiengine-eng
#export PLATFORM_REPO_PATHS=/proj/xbuilds/${TA_BUILD}/internal_platforms
#echo "PLATFORM_REPO_PATHS: $PLATFORM_REPO_PATHS"


export DEVICE=/proj/xbuilds/${TA_BUILD}/internal_platforms/xilinx_v70_gen5x8_qdma_2_202220_1/xilinx_v70_gen5x8_qdma_2_202220_1.xpfm
echo "DEVICE: $DEVICE"

export LD_LIBRARY_PATH=/proj/xbuilds/${TA_BUILD}/installs/lin64/Vitis/2022.2/aietools/tps/lnx64/gcc/lib64:${XILINX_XRT}/lib:/tools/batonroot/rodin/devkits/lnx64/gcc-8.3.0/lib64:${LD_LIBRARY_PATH}
export TEMP=/proj/xcohdstaff/yunleiz/nobkup/tmp
export LC_ALL="C"
