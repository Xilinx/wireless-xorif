## Tool Setup commands
## 
module switch xilinx/ta/2023.2_daily_latest
source /proj/petalinux/2023.2/petalinux-v2023.2_daily_latest/tool/petalinux-v2023.2-final/settings.csh

## Examples of build commands. The configurations are driven by the YAML file, however, you
## do also have to add a target in the makefile.
## 
runVpBuild zcu111 e1x9600x25_ss8x4_cc4x6600_dcc16x4x4x16x16_cmw00i_aptw262144       Exd 2023_2 ""
runVpBuild zcu111 e1x9600x25_ss8x4_cc4x6600_dcc16x4x4x16x16_cmw00i_aptw262144       Exs 2023_2 "Loop"
runVpBuild zcu111 e1x9600x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144              Exd 2023_2 ""
runVpBuild zcu111 e1x9600x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144              Exs 2023_2 "Loop"
runVpBuild zcu111 e1x9000x10_ss8x8_cc4x6600_dcc12x4x4x16x8_ocpi                     Exd 2023_2 ""
runVpBuild zcu670 e1x9600x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144              Exs 2023_2 "Loop"
runVpBuild zcu111 e1x9000x10_ss8x8_cc4x6600_dcc12x4x4x16x8_ocpi                     Exs 2023_2 "Loop"
runVpBuild zcu111 e4x8000x25_ss4x4_cc4x6600_dcc16x4x4x12x8_swtest                   Exd 2023_2 ""
runVpBuild zcu111 e4x8000x10_ss4x4_cc4x6600_dcc16x4x4x12x8_swtest                   Exd 2023_2 ""

runVpBuild zcu111 e1x9600x10_ss8x8_cc4x6600_dcc12x4x4x16x8_cmw00i_opt               Exd 2023_2 ""
runVpBuild zcu111 e1x9600x10_ss8x8_cc4x6600_dcc12x4x4x16x8_ocpi_opt                 Exd 2023_2 ""
runVpBuild zcu111 e1x9600x10_ss8x8_cc4x6600_dcc12x4x4x16x8_cmw00i_ocpi_opt          Exd 2023_2 ""

## These are the common SLV test beds, but in reality ay config can be.
runVpBuild zcu111 e1x9600x10_ss8x4_cc4x6600_dcc16x4x4x16x16_cmw00i_aptw262144       Exd 2023_2 ""
runVpBuild zcu111 e1x9600x10_ss8x4_cc4x6600_dcc16x4x4x16x16_cmw00i_aptw262144map11  Exd 2023_2 ""
