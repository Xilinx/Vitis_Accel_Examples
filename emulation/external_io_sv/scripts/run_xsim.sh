xvlog -sv -work axis_vip --include "./src/axis_vip/include" --include "." \
./src/axis_vip/axi4stream_vip_axi4streampc.sv \
./src/axis_vip/xil_common_vip_pkg.sv \
./src/axis_vip/axi4stream_vip_pkg.sv \
./src/axis_vip/axi4stream_vip_if.sv \

xvlog -sv -work xil_defaultlib --relax -L axis_vip --include "./infra"  --include "." --include "./src/axis_vip/include" \
./src/axis_vip/axi4stream_vip_v1_1_vl_rfs.sv \
./src/axis_vip/ex_sim_axi4stream_vip_mst_0_pkg.sv \
./src/axis_vip/ex_sim_axi4stream_vip_mst_0.sv \
./src/axis_vip/ex_sim_axi4stream_vip_slv_0_pkg.sv \
./src/axis_vip/ex_sim_axi4stream_vip_slv_0.sv 

#XILINX_VIVADO is the path of the vivado installation

#path for the systemC shared libraries provided with vivado, to be used for inter process communication
XSIM_LIB_PATH="$XILINX_VIVADO/data/xsim"
XSIM_EXT_LIB_PATH="$XILINX_VIVADO/data/simmodels/xsim/2022.1/lnx64/6.2.0/ext"
XSIM_BOOST_LIB_PATH="$XILINX_VIVADO/tps/boost_1_64_0"

simipc_project_dir="./src"

xsc -compile --gcc_compile_options "-DBOOST_SYSTEM_NO_DEPRECATED" --gcc_compile_options "-I$XSIM_LIB_PATH -I$XSIM_LIB_PATH/ip/xtlm_ipc_v1_0/include -I$XSIM_EXT_LIB_PATH/protobuf/include -I$XSIM_LIB_PATH/ip/xtlm/include -I$XSIM_LIB_PATH/ip/common_cpp_v1_0/include -I$simipc_project_dir/sim_ipc_axis_master -I$simipc_project_dir/sim_ipc_axis_slave" -work xil_defaultlib \
$simipc_project_dir/sim_ipc_axis_slave/sim_ipc_axis_slave.cpp \
$simipc_project_dir/sim_ipc_axis_master/sim_ipc_axis_master.cpp \
$simipc_project_dir/sim_ipc_axis_master/sim_ipc_axis_master_mirror.cpp \
$simipc_project_dir/sim_ipc_axis_slave/sim_ipc_axis_slave_mirror.cpp 

xvlog -sv -work xil_defaultlib --relax --include "." --include "$simipc_project_dir" \
$simipc_project_dir/sim_ipc_axis_master/sim_ipc_axis_master_mirror.sv \
$simipc_project_dir/sim_ipc_axis_slave/sim_ipc_axis_slave_mirror.sv

xvlog -sv -work xil_defaultlib --relax -L axis_vip --include "./infra"  --include "." --include "./src/axis_vip/include" \
./src/des.v \
./src/top.sv

#elaboration
xsc --shared -gcc_link_options "-L$XSIM_LIB_PATH/ip/xtlm_ipc_v1_0 -lxtlm_ipc_v1_0" -gcc_link_options "-L$XSIM_EXT_LIB_PATH/protobuf -lprotobuf" -gcc_link_options "-L$XSIM_LIB_PATH/ip/xtlm -lxtlm" -gcc_link_options -gcc_link_options "-L$XSIM_LIB_PATH/ip/common_cpp_v1_0 -lcommon_cpp_v1_0"  -lib xil_defaultlib  -o libdpi.so -v 

xelab --relax --debug all -L axis_vip -L xil_defaultlib \
-sc_root "." -sc_lib libdpi.so \
-sv_root "$XSIM_EXT_LIB_PATH/protobuf" -sc_lib libprotobuf.so --include "$XSIM_EXT_LIB_PATH/protobuf/include" \
-sv_root "$XSIM_LIB_PATH/ip/xtlm_ipc_v1_0" -sc_lib libxtlm_ipc_v1_0.so --include "$XSIM_LIB_PATH/ip/xtlm_ipc_v1_0/include" \
-sv_root "$XSIM_LIB_PATH/ip/xtlm" -sc_lib libxtlm.so --include "$XSIM_LIB_PATH/ip/xtlm/include" \
-sv_root "$XSIM_LIB_PATH/ip/common_cpp_v1_0" -sc_lib libcommon_cpp_v1_0.so --include "$XSIM_LIB_PATH/ip/common_cpp_v1_0/include" \
--include "$simipc_project_dir/sim_ipc_axis_master" \
--include "$simipc_project_dir/sim_ipc_axis_slave" \
--snapshot top_behav xil_defaultlib.top

export LD_LIBRARY_PATH=$PWD:$XILINX_VIVADO/lib/lnx64.o/Default:$XILINX_VIVADO/lib/lnx64.o:$XSIM_EXT_LIB_PATH/protobuf:$XSIM_LIB_PATH/ip/xtlm_ipc_v1_0:$XSIM_LIB_PATH/ip/xtlm:$XSIM_LIB_PATH/ip/common_cpp_v1_0:$LD_LIBRARY_PATH

xsim top_behav --runall
