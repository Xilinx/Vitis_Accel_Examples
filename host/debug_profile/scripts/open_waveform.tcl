if {$argc == 0 } {
    error "Insufficient TCL argument. Pass .wdb file as an tcl argument";
}
load_features simulator
puts $argv
open_wave_database $argv
