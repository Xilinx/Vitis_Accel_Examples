# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  #Adding Page
  ipgui::add_page $IPINST -name "Page 0"


}

proc update_MODELPARAM_VALUE.C_S_AXI_CONTROL_DATA_WIDTH { MODELPARAM_VALUE.C_S_AXI_CONTROL_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "C_S_AXI_CONTROL_DATA_WIDTH". Setting updated value from the model parameter.
set_property value 32 ${MODELPARAM_VALUE.C_S_AXI_CONTROL_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_S_AXI_CONTROL_ADDR_WIDTH { MODELPARAM_VALUE.C_S_AXI_CONTROL_ADDR_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "C_S_AXI_CONTROL_ADDR_WIDTH". Setting updated value from the model parameter.
set_property value 6 ${MODELPARAM_VALUE.C_S_AXI_CONTROL_ADDR_WIDTH}
}

proc update_MODELPARAM_VALUE.C_M_AXI_GMEM_ID_WIDTH { MODELPARAM_VALUE.C_M_AXI_GMEM_ID_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "C_M_AXI_GMEM_ID_WIDTH". Setting updated value from the model parameter.
set_property value 1 ${MODELPARAM_VALUE.C_M_AXI_GMEM_ID_WIDTH}
}

proc update_MODELPARAM_VALUE.C_M_AXI_GMEM_ADDR_WIDTH { MODELPARAM_VALUE.C_M_AXI_GMEM_ADDR_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "C_M_AXI_GMEM_ADDR_WIDTH". Setting updated value from the model parameter.
set_property value 64 ${MODELPARAM_VALUE.C_M_AXI_GMEM_ADDR_WIDTH}
}

proc update_MODELPARAM_VALUE.C_M_AXI_GMEM_DATA_WIDTH { MODELPARAM_VALUE.C_M_AXI_GMEM_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "C_M_AXI_GMEM_DATA_WIDTH". Setting updated value from the model parameter.
set_property value 32 ${MODELPARAM_VALUE.C_M_AXI_GMEM_DATA_WIDTH}
}

