#!/usr/bin/tclsh
if { $argc != 3 } {
    puts "Usage: sdt.tcl <xsa> <board> <outdir>"
    exit 2
}
set xsa [lindex $argv 0]
set board [lindex $argv 1]
set outdir [lindex $argv 2]
exec rm -rf $outdir
sdtgen set_dt_param -xsa $xsa -board_dts $board -dir $outdir
sdtgen generate_sdt
