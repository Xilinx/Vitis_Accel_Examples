#!/usr/bin/env python

import sys
from sys import argv
import json
import glob
import os
import re
import subprocess
sys.dont_write_bytecode = True

pwd = os.path.dirname(sys.path[0])
script, folder = argv
sdaccel_folder = os.path.abspath(folder)
os.system("mkdir -p host cpp_kernels rtl_kernels ocl_kernels applications")

# Copy script begins
# Hello World
os.system("cp -rf " + sdaccel_folder +
"/getting_started/hello_world/helloworld_c/ " + pwd +
"/hello_world")

# Host
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/concurrent_kernel_execution_c/ " + pwd +
"/host/concurrent_kernel_execution")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/copy_buffer_c/ " + pwd +
"/host/copy_buffer")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/data_transfer_c/ " + pwd +
"/host/data_transfer")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/device_query_c/ " + pwd +
"/host/device_query")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/device_query_cpp/ " + pwd +
"/host/device_query_cpp")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/errors_c/ " + pwd +
"/host/errors")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/errors_cpp/ " + pwd +
"/host/errors_cpp")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/hbm_bandwidth/ " + pwd +
"/host/hbm_bandwidth")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/hbm_simple/ " + pwd +
"/host/hbm_simple")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/host_global_bandwidth_5.0_shell/ " + pwd +
"/host/host_global_bandwidth_5.0_shell")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/host_global_bandwidth/ " + pwd +
"/host/host_global_bandwidth")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/kernel_swap_c/ " + pwd +
"/host/kernel_swap")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/multiple_cus_asymmetrical/ " + pwd +
"/host/multiple_cus_asymmetrical")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/multiple_process_c/ " + pwd +
"/host/multiple_process")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/multiple_devices_c/ " + pwd +
"/host/multiple_devices")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/overlap_c/ " + pwd +
"/host/overlap")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/streaming_multi_cus/ " + pwd +
"/host/streaming_multi_cus")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/streaming_mm_mixed/ " + pwd +
"/host/streaming_mm_mixed")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/streaming_k2k_mm/ " + pwd +
"/host/streaming_k2k_mm")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/streaming_k2k/ " + pwd +
"/host/streaming_k2k")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/streaming_host_bandwidth/ " + pwd +
"/host/streaming_host_bandwidth")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/streaming_simple/ " + pwd +
"/host/streaming_simple")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/sub_devices_c/ " + pwd +
"/host/sub_devices")

# CPP Kernel
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/critical_path_c/ " + pwd +
"/cpp_kernels/critical_path")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/large_loop_c/ " + pwd +
"/cpp_kernels/large_loop")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/split_kernel_c/ " + pwd +
"/cpp_kernels/split_kernel")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/too_many_cu_c/ " + pwd +
"/cpp_kernels/too_many_cu")

os.system("cp -rf " + sdaccel_folder +
"/getting_started/cpu_to_fpga/00_cpu/ " + pwd +
"/cpp_kernels/00_cpu")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/cpu_to_fpga/01_kernel_c/ " + pwd +
"/cpp_kernels/01_kernel")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/cpu_to_fpga/02_local_mem_c/ " + pwd +
"/cpp_kernels/02_local_mem")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/cpu_to_fpga/03_burst_rw_c/ " + pwd +
"/cpp_kernels/03_burst_rw")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/cpu_to_fpga/04_partition_c/ " + pwd +
"/cpp_kernels/04_partition")

os.system("cp -rf " + sdaccel_folder +
"/getting_started/dataflow/dataflow_stream_array_c/ " + pwd +
"/cpp_kernels/dataflow_stream_array")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/dataflow/dataflow_stream_c/ " + pwd +
"/cpp_kernels/dataflow_stream")

os.system("cp -rf " + sdaccel_folder +
"/getting_started/debug/debug_profile_c/ " + pwd +
"/cpp_kernels/debug_profile")

os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/aos_vs_soa_c/ " + pwd +
"/cpp_kernels/aos_vs_soa")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/array_partition_c/ " + pwd +
"/cpp_kernels/array_partition")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/dependence_inter_c/ " + pwd +
"/cpp_kernels/dependence_inter")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/lmem_2rw_c/ " + pwd +
"/cpp_kernels/lmem_2rw")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/loop_fusion_c/ " + pwd +
"/cpp_kernels/loop_fusion")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/loop_pipeline_c/ " + pwd +
"/cpp_kernels/loop_pipeline")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/loop_reorder_c/ " + pwd +
"/cpp_kernels/loop_reorder")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/partition_cyclicblock_c/ " + pwd +
"/cpp_kernels/partition_cyclicblock")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/shift_register_c/ " + pwd +
"/cpp_kernels/shift_register")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/systolic_array_c/ " + pwd +
"/cpp_kernels/systolic_array")

os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/burst_rw_c/ " + pwd +
"/cpp_kernels/burst_rw")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/custom_datatype_c/ " + pwd +
"/cpp_kernels/custom_datatype")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/full_array_2d_c/ " + pwd +
"/cpp_kernels/full_array_2d")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/gmem_2banks_c_5.0_shell/ " + pwd +
"/cpp_kernels/gmem_2banks_5.0_shell")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/gmem_2banks_c/ " + pwd +
"/cpp_kernels/gmem_2banks")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/kernel_global_bandwidth_5.0_shell/ " + pwd +
"/cpp_kernels/kernel_global_bandwidth_5.0_shell")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/kernel_global_bandwidth/ " + pwd +
"/cpp_kernels/kernel_global_bandwidth")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/memcoalesce_hang_c/ " + pwd +
"/cpp_kernels/memcoalesce_hang")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/plram_access_c/ " + pwd +
"/cpp_kernels/plram_access")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/row_array_2d_c/ " + pwd +
"/cpp_kernels/row_array_2d")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/slr_assign/ " + pwd +
"/cpp_kernels/slr_assign")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/wide_mem_rw_c/ " + pwd +
"/cpp_kernels/wide_mem_rw")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/window_array_2d_c/ " + pwd +
"/cpp_kernels/window_array_2d")

# RTL Kernels
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_adder_streams/ " + pwd +
"/rtl_kernels/rtl_adder_streams")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_streaming_free_running/ " + pwd +
"/rtl_kernels/rtl_streaming_free_running")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_streaming_k2k/ " + pwd +
"/rtl_kernels/rtl_streaming_k2k")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_vadd/ " + pwd +
"/rtl_kernels/rtl_vadd")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_vadd_2clks/ " + pwd +
"/rtl_kernels/rtl_vadd_2clks")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_vadd_2kernels/ " + pwd +
"/rtl_kernels/rtl_vadd_2kernels")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_vadd_hw_debug/ " + pwd +
"/rtl_kernels/rtl_vadd_hw_debug")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_vadd_mixed_c_vadd/ " + pwd +
"/rtl_kernels/rtl_vadd_mixed_c_vadd")

# OCL Kernels
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/large_loop_ocl/ " + pwd +
"/ocl_kernels/cl_large_loop")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/split_kernel_ocl/ " + pwd +
"/ocl_kernels/cl_split_kernel")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/too_many_cu_ocl/ " + pwd +
"/ocl_kernels/cl_too_many_cu")

os.system("cp -rf " + sdaccel_folder +
"/getting_started/dataflow/dataflow_func_ocl/ " + pwd +
"/ocl_kernels/cl_dataflow_func")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/dataflow/dataflow_pipes_ocl/ " + pwd +
"/ocl_kernels/cl_dataflow_pipes")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/dataflow/dataflow_subfunc_ocl/ " + pwd +
"/ocl_kernels/cl_dataflow_subfunc")

os.system("cp -rf " + sdaccel_folder +
"/getting_started/debug/debug_printf_ocl/ " + pwd +
"/ocl_kernels/cl_debug_printf")

os.system("cp -rf " + sdaccel_folder +
"/getting_started/hello_world/helloworld_ocl/ " + pwd +
"/ocl_kernels/cl_helloworld")

os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/array_partition_ocl/ " + pwd +
"/ocl_kernels/cl_array_partition")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/lmem_2rw_ocl/ " + pwd +
"/ocl_kernels/cl_lmem_2rw")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/loop_fusion_ocl/ " + pwd +
"/ocl_kernels/cl_loop_fusion")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/loop_reorder_ocl/ " + pwd +
"/ocl_kernels/cl_loop_reorder")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/partition_cyclicblock_ocl/ " + pwd +
"/ocl_kernels/cl_partition_cyclicblock")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/shift_register_ocl/ " + pwd +
"/ocl_kernels/cl_shift_register")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/sum_scan_ocl/ " + pwd +
"/ocl_kernels/cl_sum_scan")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/systolic_array_ocl/ " + pwd +
"/ocl_kernels/cl_systolic_array")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/vectorization_memorycoalescing_ocl/ " + pwd +
"/ocl_kernels/cl_vectorization_memorycoalescing")

os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/burst_rw_ocl/ " + pwd +
"/ocl_kernels/cl_burst_rw")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/full_array_2d_ocl/ " + pwd +
"/ocl_kernels/cl_full_array_2d")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/gmem_2banks_ocl_5.0_shell/ " + pwd +
"/ocl_kernels/cl_gmem_2banks_5.0_shell")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/gmem_2banks_ocl/ " + pwd +
"/ocl_kernels/cl_gmem_2banks")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/row_array_2d_ocl/ " + pwd +
"/ocl_kernels/cl_row_array_2d")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/wide_mem_rw_ocl/ " + pwd +
"/ocl_kernels/cl_wide_mem_rw")

# Applications
os.system("cp -rf " + sdaccel_folder +
"/acceleration/kmeans/ " + pwd +
"/applications/kmeans")
os.system("cp -rf " + sdaccel_folder +
"/vision/median_filter/ " + pwd +
"/applications/median_filter")
os.system("cp -rf " + sdaccel_folder +
"/vision/watermarking/ " + pwd +
"/applications/watermarking")


# Common includes and utilities
os.system("cp -rf " + sdaccel_folder +
"/libs/ " + pwd +
"/common/includes")
os.system("cp -rf " + sdaccel_folder +
"/utility/ " + pwd +
"/common/utility")

#os.system(pwd + "/common/utility/makefile_gen/update_makegen_all.sh")
#os.system(pwd + "/common/utility/readme_gen/update_all_readme.sh")
#os.system(pwd + "/common/utility/check_readme.sh")
#os.system(pwd + "/common/utility/check_makefile.sh")

# Since the new Makefile structute has changed we need to copy some files like
# config.mk in rtl_kernel examples from Scout present repo.
