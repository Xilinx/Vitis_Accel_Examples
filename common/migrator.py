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
os.system("mkdir -p 100_hello_world 200_host 300_cpp_kernels 400_rtl_kernels 500_ocl_kernels 600_applications")

# Copy script begins
# Hello World
os.system("cp -rf " + sdaccel_folder +
"/getting_started/hello_world/helloworld_c/ " + pwd +
"/101_hello_world")
# Host
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/concurrent_kernel_execution_c/ " + pwd +
"/200_host/207_concurrent_kernel_execution")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/copy_buffer_c/ " + pwd +
"/200_host/210_copy_buffer")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/data_transfer_c/ " + pwd +
"/200_host/205_data_transfer")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/device_query_c/ " + pwd +
"/200_host/201_device_query")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/device_query_cpp/ " + pwd +
"/200_host/202_device_query_cpp")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/errors_c/ " + pwd +
"/200_host/203_errors")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/errors_cpp/ " + pwd +
"/200_host/204_errors_cpp")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/host_global_bandwidth/ " + pwd +
"/200_host/206_host_global_bandwidth")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/kernel_swap_c/ " + pwd +
"/200_host/208_kernel_swap")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/multiple_process_c/ " + pwd +
"/200_host/211_multiple_process")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/overlap_c/ " + pwd +
"/200_host/209_overlap")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/stream_access_c/ " + pwd +
"/200_host/213_stream_access")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/host/sub_devices_c/ " + pwd +
"/200_host/212_sub_devices")
# CPP Kernel
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/critical_path_c/ " + pwd +
"/300_cpp_kernels/301_critical_path")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/large_loop_c/ " + pwd +
"/300_cpp_kernels/302_large_loop")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/split_kernel_c/ " + pwd +
"/300_cpp_kernels/303_split_kernel")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/too_many_cu_c/ " + pwd +
"/300_cpp_kernels/304_too_many_cu")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/dataflow/dataflow_stream_array_c/ " + pwd +
"/300_cpp_kernels/305_dataflow_stream_array")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/dataflow/dataflow_stream_c/ " + pwd +
"/300_cpp_kernels/306_dataflow_stream")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/aos_vs_soa_c/ " + pwd +
"/300_cpp_kernels/307_aos_vs_soa")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/array_partition_c/ " + pwd +
"/300_cpp_kernels/308_array_partition")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/dependence_inter_c/ " + pwd +
"/300_cpp_kernels/309_dependence_inter")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/lmem_2rw_c/ " + pwd +
"/300_cpp_kernels/310_lmem_2rw")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/loop_fusion_c/ " + pwd +
"/300_cpp_kernels/311_loop_fusion")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/loop_pipeline_c/ " + pwd +
"/300_cpp_kernels/312_loop_pipeline")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/loop_reorder_c/ " + pwd +
"/300_cpp_kernels/313_loop_reorder")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/partition_cyclicblock_c/ " + pwd +
"/300_cpp_kernels/314_partition_cyclicblock")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/shift_register_c/ " + pwd +
"/300_cpp_kernels/315_shift_register")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/systolic_array_c/ " + pwd +
"/300_cpp_kernels/316_systolic_array")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/burst_rw_c/ " + pwd +
"/300_cpp_kernels/317_burst_rw")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/custom_datatype_c/ " + pwd +
"/300_cpp_kernels/318_custom_datatype")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/full_array_2d_c/ " + pwd +
"/300_cpp_kernels/319_full_array_2d")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/gmem_2banks_c/ " + pwd +
"/300_cpp_kernels/320_gmem_2banks")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/kernel_global_bandwidth/ " + pwd +
"/300_cpp_kernels/321_kernel_global_bandwidth")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/memcoalesce_hang_c/ " + pwd +
"/300_cpp_kernels/322_memcoalesce_hang")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/plram_access_c/ " + pwd +
"/300_cpp_kernels/323_plram_access")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/row_array_2d_c/ " + pwd +
"/300_cpp_kernels/324_row_array_2d")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/slr_assign/ " + pwd +
"/300_cpp_kernels/325_slr_assign")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/wide_mem_rw_c/ " + pwd +
"/300_cpp_kernels/326_wide_mem_rw")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/window_array_2d_c/ " + pwd +
"/300_cpp_kernels/327_window_array_2d")
# RTL Kernels
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_adder_pipes/ " + pwd +
"/400_rtl_kernels/401_rtl_adder_pipes")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_vadd/ " + pwd +
"/400_rtl_kernels/402_rtl_vadd")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_vadd_2clks/ " + pwd +
"/400_rtl_kernels/403_rtl_vadd_2clks")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_vadd_2kernels/ " + pwd +
"/400_rtl_kernels/404_rtl_vadd_2kernels")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_vadd_hw_debug/ " + pwd +
"/400_rtl_kernels/405_rtl_vadd_hw_debug")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/rtl_kernel/rtl_vadd_mixed_c_vadd/ " + pwd +
"/400_rtl_kernels/406_rtl_vadd_mixed_c_vadd")
# OCL Kernels
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/large_loop_ocl/ " + pwd +
"/500_ocl_kernels/501_cl_large_loop")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/split_kernel_ocl/ " + pwd +
"/500_ocl_kernels/502_cl_split_kernel")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/clk_freq/too_many_cu_ocl/ " + pwd +
"/500_ocl_kernels/503_cl_too_many_cu")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/dataflow/dataflow_func_ocl/ " + pwd +
"/500_ocl_kernels/504_cl_dataflow_func")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/dataflow/dataflow_pipes_ocl/ " + pwd +
"/500_ocl_kernels/505_cl_dataflow_pipes")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/dataflow/dataflow_subfunc_ocl/ " + pwd +
"/500_ocl_kernels/506_cl_dataflow_subfunc")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/debug/debug_printf_ocl/ " + pwd +
"/500_ocl_kernels/507_cl_debug_printf")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/debug/debug_profile_ocl/ " + pwd +
"/500_ocl_kernels/508_cl_debug_profile")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/hello_world/helloworld_ocl/ " + pwd +
"/500_ocl_kernels/509_cl_helloworld")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/array_partition_ocl/ " + pwd +
"/500_ocl_kernels/510_cl_array_partition")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/lmem_2rw_ocl/ " + pwd +
"/500_ocl_kernels/511_cl_lmem_2rw")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/loop_fusion_ocl/ " + pwd +
"/500_ocl_kernels/512_cl_loop_fusion")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/loop_reorder_ocl/ " + pwd +
"/500_ocl_kernels/513_cl_loop_reorder")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/partition_cyclicblock_ocl/ " + pwd +
"/500_ocl_kernels/514_cl_partition_cyclicblock")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/shift_register_ocl/ " + pwd +
"/500_ocl_kernels/515_cl_shift_register")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/sum_scan_ocl/ " + pwd +
"/500_ocl_kernels/516_cl_sum_scan")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/systolic_array_ocl/ " + pwd +
"/500_ocl_kernels/517_cl_systolic_array")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_opt/vectorization_memorycoalescing_ocl/ " + pwd +
"/500_ocl_kernels/518_cl_vectorization_memorycoalescing")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/burst_rw_ocl/ " + pwd +
"/500_ocl_kernels/519_cl_burst_rw")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/full_array_2d_ocl/ " + pwd +
"/500_ocl_kernels/520_cl_full_array_2d")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/gmem_2banks_ocl/ " + pwd +
"/500_ocl_kernels/521_cl_gmem_2banks")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/row_array_2d_ocl/ " + pwd +
"/500_ocl_kernels/522_cl_row_array_2d")
os.system("cp -rf " + sdaccel_folder +
"/getting_started/kernel_to_gmem/wide_mem_rw_ocl/ " + pwd +
"/500_ocl_kernels/523_cl_wide_mem_rw")
# Applications
os.system("cp -rf " + sdaccel_folder +
"/acceleration/kmeans/ " + pwd +
"/600_applications/601_kmeans")
os.system("cp -rf " + sdaccel_folder +
"/vision/median_filter/ " + pwd +
"/600_applications/602_median_filter")
os.system("cp -rf " + sdaccel_folder +
"/vision/watermarking/ " + pwd +
"/600_applications/603_watermarking")

#os.system(pwd + "/common/utility/makefile_gen/update_makegen_all.sh")
#os.system(pwd + "/common/utility/readme_gen/update_all_readme.sh")
#os.system(pwd + "/common/utility/check_readme.sh")
#os.system(pwd + "/common/utility/check_makefile.sh")

# Since the new Makefile structute has changed we need to copy some files like
# config.mk in rtl_kernel examples from Scout present repo.
