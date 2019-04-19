# 1 "/proj/xhdhdstaff3/rsarkari/scout/ScoutExamples/101_hello_world/src/vadd.cpp"
# 1 "/proj/xhdhdstaff3/rsarkari/scout/ScoutExamples/101_hello_world/src/vadd.cpp" 1
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 155 "<built-in>" 3
# 1 "<command line>" 1






# 1 "/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/Vivado/2019.1/common/technology/autopilot/etc/autopilot_ssdm_op.h" 1
# 157 "/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/Vivado/2019.1/common/technology/autopilot/etc/autopilot_ssdm_op.h"
extern "C" {






    void _ssdm_op_IfRead(...) __attribute__ ((nothrow));
    void _ssdm_op_IfWrite(...) __attribute__ ((nothrow));
    unsigned int __attribute__ ((bitwidth(1))) _ssdm_op_IfNbRead(...) __attribute__ ((nothrow));
    unsigned int __attribute__ ((bitwidth(1))) _ssdm_op_IfNbWrite(...) __attribute__ ((nothrow));
    unsigned int __attribute__ ((bitwidth(1))) _ssdm_op_IfCanRead(...) __attribute__ ((nothrow));
    unsigned int __attribute__ ((bitwidth(1))) _ssdm_op_IfCanWrite(...) __attribute__ ((nothrow));


    void _ssdm_StreamRead(...) __attribute__ ((nothrow));
    void _ssdm_StreamWrite(...) __attribute__ ((nothrow));
    unsigned int __attribute__ ((bitwidth(1))) _ssdm_StreamNbRead(...) __attribute__ ((nothrow));
    unsigned int __attribute__ ((bitwidth(1))) _ssdm_StreamNbWrite(...) __attribute__ ((nothrow));
    unsigned int __attribute__ ((bitwidth(1))) _ssdm_StreamCanRead(...) __attribute__ ((nothrow));
    unsigned int __attribute__ ((bitwidth(1))) _ssdm_StreamCanWrite(...) __attribute__ ((nothrow));
    unsigned _ssdm_StreamSize(...) __attribute__ ((nothrow));




    void _ssdm_op_MemShiftRead(...) __attribute__ ((nothrow));

    void _ssdm_op_Wait(...) __attribute__ ((nothrow));
    void _ssdm_op_Poll(...) __attribute__ ((nothrow));

    void _ssdm_op_Return(...) __attribute__ ((nothrow));


    void _ssdm_op_SpecSynModule(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecTopModule(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecProcessDecl(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecProcessDef(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecPort(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecConnection(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecChannel(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecSensitive(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecModuleInst(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecPortMap(...) __attribute__ ((nothrow));

    void _ssdm_op_SpecReset(...) __attribute__ ((nothrow));

    void _ssdm_op_SpecPlatform(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecClockDomain(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecPowerDomain(...) __attribute__ ((nothrow));

    int _ssdm_op_SpecRegionBegin(...) __attribute__ ((nothrow));
    int _ssdm_op_SpecRegionEnd(...) __attribute__ ((nothrow));

    void _ssdm_op_SpecLoopName(...) __attribute__ ((nothrow));

    void _ssdm_op_SpecLoopTripCount(...) __attribute__ ((nothrow));

    int _ssdm_op_SpecStateBegin(...) __attribute__ ((nothrow));
    int _ssdm_op_SpecStateEnd(...) __attribute__ ((nothrow));

    void _ssdm_op_SpecInterface(...) __attribute__ ((nothrow));

    void _ssdm_op_SpecPipeline(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecDataflowPipeline(...) __attribute__ ((nothrow));


    void _ssdm_op_SpecLatency(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecParallel(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecProtocol(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecOccurrence(...) __attribute__ ((nothrow));

    void _ssdm_op_SpecResource(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecResourceLimit(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecCHCore(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecFUCore(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecIFCore(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecIPCore(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecKeepValue(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecMemCore(...) __attribute__ ((nothrow));

    void _ssdm_op_SpecExt(...) __attribute__ ((nothrow));




    void _ssdm_SpecArrayDimSize(...) __attribute__ ((nothrow));

    void _ssdm_RegionBegin(...) __attribute__ ((nothrow));
    void _ssdm_RegionEnd(...) __attribute__ ((nothrow));

    void _ssdm_Unroll(...) __attribute__ ((nothrow));
    void _ssdm_UnrollRegion(...) __attribute__ ((nothrow));

    void _ssdm_InlineAll(...) __attribute__ ((nothrow));
    void _ssdm_InlineLoop(...) __attribute__ ((nothrow));
    void _ssdm_Inline(...) __attribute__ ((nothrow));
    void _ssdm_InlineSelf(...) __attribute__ ((nothrow));
    void _ssdm_InlineRegion(...) __attribute__ ((nothrow));

    void _ssdm_SpecArrayMap(...) __attribute__ ((nothrow));
    void _ssdm_SpecArrayPartition(...) __attribute__ ((nothrow));
    void _ssdm_SpecArrayReshape(...) __attribute__ ((nothrow));

    void _ssdm_SpecStream(...) __attribute__ ((nothrow));

    void _ssdm_op_SpecStable(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecStableContent(...) __attribute__ ((nothrow));

    void _ssdm_op_SpecPipoDepth(...) __attribute__ ((nothrow));

    void _ssdm_SpecExpr(...) __attribute__ ((nothrow));
    void _ssdm_SpecExprBalance(...) __attribute__ ((nothrow));

    void _ssdm_SpecDependence(...) __attribute__ ((nothrow));

    void _ssdm_SpecLoopMerge(...) __attribute__ ((nothrow));
    void _ssdm_SpecLoopFlatten(...) __attribute__ ((nothrow));
    void _ssdm_SpecLoopRewind(...) __attribute__ ((nothrow));

    void _ssdm_SpecFuncInstantiation(...) __attribute__ ((nothrow));
    void _ssdm_SpecFuncBuffer(...) __attribute__ ((nothrow));
    void _ssdm_SpecFuncExtract(...) __attribute__ ((nothrow));
    void _ssdm_SpecConstant(...) __attribute__ ((nothrow));

    void _ssdm_DataPack(...) __attribute__ ((nothrow));
    void _ssdm_SpecDataPack(...) __attribute__ ((nothrow));

    void _ssdm_op_SpecBitsMap(...) __attribute__ ((nothrow));
    void _ssdm_op_SpecLicense(...) __attribute__ ((nothrow));

    void __xilinx_ip_top(...) __attribute__ ((nothrow));


}
# 8 "<command line>" 2
# 1 "<built-in>" 2
# 1 "/proj/xhdhdstaff3/rsarkari/scout/ScoutExamples/101_hello_world/src/vadd.cpp" 2
# 42 "/proj/xhdhdstaff3/rsarkari/scout/ScoutExamples/101_hello_world/src/vadd.cpp"
const unsigned int c_len = 4096/1024;
const unsigned int c_size = 1024;
# 54 "/proj/xhdhdstaff3/rsarkari/scout/ScoutExamples/101_hello_world/src/vadd.cpp"
extern "C" {
void vadd(
        const unsigned int *in1,
        const unsigned int *in2,
        unsigned int *out_r,
        int size
        )
{








_ssdm_op_SpecInterface(in1, "m_axi", 0, 0, "", 0, 0, "gmem", "slave", "", 16, 16, 16, 16, "", "");
_ssdm_op_SpecInterface(in2, "m_axi", 0, 0, "", 0, 0, "gmem", "slave", "", 16, 16, 16, 16, "", "");
_ssdm_op_SpecInterface(out_r, "m_axi", 0, 0, "", 0, 0, "gmem", "slave", "", 16, 16, 16, 16, "", "");
_ssdm_op_SpecInterface(in1, "s_axilite", 0, 0, "", 0, 0, "control", "", "", 0, 0, 0, 0, "", "");
_ssdm_op_SpecInterface(in2, "s_axilite", 0, 0, "", 0, 0, "control", "", "", 0, 0, 0, 0, "", "");
_ssdm_op_SpecInterface(out_r, "s_axilite", 0, 0, "", 0, 0, "control", "", "", 0, 0, 0, 0, "", "");
_ssdm_op_SpecInterface(size, "s_axilite", 0, 0, "", 0, 0, "control", "", "", 0, 0, 0, 0, "", "");
_ssdm_op_SpecInterface(0, "s_axilite", 0, 0, "", 0, 0, "control", "", "", 0, 0, 0, 0, "", "");

 unsigned int v1_buffer[1024];
    unsigned int v2_buffer[1024];
    unsigned int vout_buffer[1024];



    for(int i = 0; i < size; i += 1024){
_ssdm_op_SpecLoopTripCount(c_len, c_len, 0, "");
 int chunk_size = 1024;

        if ((i + 1024) > size)
            chunk_size = size - i;







        read1: for (int j = 0 ; j < chunk_size ; j++){
_ssdm_op_SpecLoopTripCount(c_size, c_size, 0, "");
_ssdm_op_SpecPipeline(1, 1, 1, 0, "");
 v1_buffer[j] = in1[i + j];
        }

        read2: for (int j = 0 ; j < chunk_size ; j++){
_ssdm_op_SpecLoopTripCount(c_size, c_size, 0, "");
_ssdm_op_SpecPipeline(1, 1, 1, 0, "");
 v2_buffer[j] = in2[i + j];
        }



        vadd: for (int j = 0 ; j < chunk_size; j ++){
_ssdm_op_SpecLoopTripCount(c_size, c_size, 0, "");
_ssdm_op_SpecPipeline(1, 1, 1, 0, "");

 vout_buffer[j] = v1_buffer[j] + v2_buffer[j];
        }


        write: for (int j = 0 ; j < chunk_size ; j++){
_ssdm_op_SpecLoopTripCount(c_size, c_size, 0, "");
_ssdm_op_SpecPipeline(1, 1, 1, 0, "");
 out_r[i + j] = vout_buffer[j];
        }
    }
}
}
