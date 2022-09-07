set num_si [get_property CONFIG.NUM_SI [get_bd_cells /ConfigNoc]]
for {set i 1} {$i < $num_si} { incr i } {
set CONNECTION ""
append CONNECTION "M0${i}_AXI { read_bw {128} write_bw {128} read_avg_burst {4} write_avg_burst {4}} "
set_property -dict [list CONFIG.CONNECTIONS $CONNECTION] [get_bd_intf_pins /ConfigNoc/S0${i}_AXI]
}
