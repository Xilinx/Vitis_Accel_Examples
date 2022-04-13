set rp_hier [get_cells -hier -filter {HD.RECONFIGURABLE == 1}]
set static_clock_nets [get_nets -of [get_pins -of [get_cells -hier -filter "PRIMITIVE_SUBGROUP == BUFFER && PARENT !~ $rp_hier*"] -filter {DIRECTION == OUT && REF_PIN_NAME == O}]]
set_property GCLK_DESKEW OFF [get_nets $static_clock_nets]