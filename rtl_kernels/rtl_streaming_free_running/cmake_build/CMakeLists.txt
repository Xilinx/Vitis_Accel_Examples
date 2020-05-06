set(EXECNAME "vadd_stream")
set(XBIN_DIR "myadder1.xclbin")

if (WIN32)
  set(OpenCL_INCLUDE_DIR ${OCL_ROOT}/include)
  find_library(OpenCL_LIBRARY
    NAMES OpenCL
    HINTS "${OCL_ROOT}/lib")

  include_directories(${OpenCL_INCLUDE_DIR})
else()
  find_package(OpenCL)
endif(WIN32)

include_directories(${XILINX_XRT}/include ${XILINX_XRT}/ext/include ../src ../../../common/includes/xcl2)

add_executable(${EXECNAME} ../../../common/includes/xcl2/xcl2.cpp ../src/host.cpp)

target_link_libraries(${EXECNAME} PRIVATE ${OpenCL_LIBRARY})

install(TARGETS ${EXECNAME}
  RUNTIME DESTINATION ${INSTALL_DIR}/${EXECNAME})
