## ----------------------------------------------------------------------------
##
## ----------------------------------------------------------------------------
source -quiet xil_vivado_build_procs.tcl

## ----------------------------------------------------------------------------
## Source a custom user file, this allows over-riding of any of the procs
## defined so far.
## ----------------------------------------------------------------------------
set USER_TCL_FILE "user_procs.tcl"
if { [file exists $USER_TCL_FILE] } {
  puts "You want to do someting cool, sourcing your stuff in $USER_TCL_FILE"  
  source -quiet $USER_TCL_FILE
}

## ----------------------------------------------------------------------------
## Process the command line arguments, show the help text
## ----------------------------------------------------------------------------
if { [info exists argc] } {
  ::roe::bin::usage
  process_tclargs $argc $argv
  ::roe::bin::usage
} else {
  ::roe::bin::usage
}
