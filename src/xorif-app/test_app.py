#!/usr/bin/env python3

import logging
import subprocess
import os
import time

logger = logging.getLogger()

# Select server/client based on "app-type"
server = ["./xorif-app", "./xorif-app-server.py --fhi --ocp --oprach"]
client = ["./xorif-app ", "coverage run -a ./xorif-app-client.py "]

def do_command(command, mode=0):
    '''
    Run command in a subshell.
    mode = 0: error if result != 0
    mode = 1: ignore any errors
    mode = 2: error if result == 0
    '''
    logger.debug(command)
    result = os.system(command)
    logger.debug(result)
    if mode == 0:
        assert result == 0
    elif mode == 2:
        assert result != 0

def test_app_command_line(app_type, bf, ocp, oprach):
    # Start server
    proc = subprocess.Popen(server[app_type].split())
    time.sleep(5)
    try:
        do_command(client[app_type] + "-f simple.txt")
        do_command(client[app_type] + "-f xorif_app_cmds.txt")
        do_command(client[app_type] + "-h -b -v -B -F -p 5001 -n 127.0.0.1")
        do_command(client[app_type] + "-c help")
        do_command(client[app_type] + "-V")
        do_command(client[app_type] + "-I < xorif_app_interactive.txt")
        do_command(client[app_type] + "-f bad_config.txt", mode=2)
        do_command(client[app_type] + "-f XXX", mode=2)
        do_command(client[app_type] + "-X", mode=2)
        do_command(client[app_type] + "-p", mode=2)
        do_command(client[app_type] + "-n", mode=2)
        do_command(client[app_type] + "XXX", mode=2)
        do_command(client[app_type] + "-c \"XXX\"", mode=2)
        do_command(client[app_type] + "-c \"help XXX\"", mode=2)
        do_command(client[app_type] + "-c \"version XXX\"", mode=2)
        do_command(client[app_type] + "-c \"magic XXX\"", mode=2)
        do_command(client[app_type] + "-c \"terminate XXX\"", mode=2)
        do_command(client[app_type] + "-c \"exit XXX\"", mode=2)
        do_command(client[app_type] + "-c \"quit XXX\"", mode=2)
        do_command(client[app_type] + "-c \"wait XXX\"", mode=2)
        do_command(client[app_type] + "-c \"run XXX\"", mode=2)
        do_command(client[app_type] + "-c \"run XXX XXX\"", mode=2)
        do_command(client[app_type] + "-c \"peek XXX\"", mode=2)
        do_command(client[app_type] + "-c \"poke XXX\"", mode=2)
        do_command(client[app_type] + "-c \"debug XXX\"", mode=2)
        do_command(client[app_type] + "-c \"init XXX\"", mode=2)
        do_command(client[app_type] + "-c \"finish XXX\"", mode=2)
        do_command(client[app_type] + "-c \"reset XXX\"", mode=2)
        do_command(client[app_type] + "-c \"get\"", mode=2)
        do_command(client[app_type] + "-c \"get XXX\"", mode=2)
        do_command(client[app_type] + "-c \"get fhi_cc_config XXX\"", mode=2)
        do_command(client[app_type] + "-c \"get fhi_cc_alloc XXX\"", mode=2)
        do_command(client[app_type] + "-c \"get fhi_stats XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set\"", mode=2)
        do_command(client[app_type] + "-c \"set XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set num_rbs XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set numerology XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set num_rbs_ssb XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set numerology_ssb XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set time_advance XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ul_timing_params XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set dl_timing_params XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ul_bid_forward XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ul_bid_forward_fine XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ul_radio_ch_dly XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set dl_iq_compression XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set dl_iq_comp XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ul_iq_compression XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ul_iq_comp XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ssb_iq_compression XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ssb_iq_comp XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set prach_iq_compression XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set prach_iq_comp XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set dl_sections_per_sym XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ul_sections_per_sym XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ssb_sections_per_sym XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set frames_per_sym XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set frames_per_sym_ssb XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set dest_mac_addr XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set src_mac_addr XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set protocol XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set protocol_alt XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set vlan XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set eAxC_id XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ru_ports XX\"", mode=2)
        do_command(client[app_type] + "-c \"set ru_ports_table_mode XXX\"", mode=2)
        #do_command(client[app_type] + "-c \"set ru_ports_table_mode1 XXX\"", mode=2)
        #do_command(client[app_type] + "-c \"set ru_ports_table_mode2 XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ru_ports_table XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set ru_ports_table_alt XXX\"", mode=2)
        do_command(client[app_type] + "-c \"set system_constants XXX\"", mode=2)
        do_command(client[app_type] + "-c \"configure XXX\"", mode=2)
        do_command(client[app_type] + "-c \"enable XXX\"", mode=2)
        do_command(client[app_type] + "-c \"disable XXX\"", mode=2)
        do_command(client[app_type] + "-c \"clear XXX\"", mode=2)
        do_command(client[app_type] + "-c \"configure XXX\"", mode=2)
        do_command(client[app_type] + "-c \"read_reg fhi XXX\"", mode=2)
        do_command(client[app_type] + "-c \"read_reg_offset fhi XXX\"", mode=2)
        do_command(client[app_type] + "-c \"write_reg fhi XXX\"", mode=2)
        do_command(client[app_type] + "-c \"write_reg_offset fhi XXX\"", mode=2)

        if bf:
            do_command(client[app_type] + "-f xorif_app_cmds_bf.txt")
            do_command(client[app_type] + "-c \"get bf_cc_config XXX\"", mode=2)
            do_command(client[app_type] + "-c \"get bf_cc_alloc XXX\"", mode=2)
            do_command(client[app_type] + "-c \"set bw_compression XXX\"", mode=2)
            do_command(client[app_type] + "-c \"set bw_comp XXX\"", mode=2)
            do_command(client[app_type] + "-c \"set bf_fft_shift_mode XXX\"", mode=2)
            do_command(client[app_type] + "-c \"set bf_scheduler_mode XXX\"", mode=2)
            do_command(client[app_type] + "-c \"schedule_bf XXX\"", mode=2)
            do_command(client[app_type] + "-c \"load XXX\"", mode=2)
            do_command(client[app_type] + "-c \"load beam_weights XXX\"", mode=2)
            do_command(client[app_type] + "-c \"read_reg bf XXX\"", mode=2)
            do_command(client[app_type] + "-c \"read_reg_offset bf XXX\"", mode=2)
            do_command(client[app_type] + "-c \"write_reg bf XXX\"", mode=2)
            do_command(client[app_type] + "-c \"write_reg_offset bf XXX\"", mode=2)

        if app_type == 1 and ocp:
            do_command(client[app_type] + "-f xorif_app_cmds_ocp.txt")
            do_command(client[app_type] + "-c \"echo 1 2 3\"", mode=2)
            do_command(client[app_type] + "-c \"set ocp_schedule 3 4\"", mode=2)
            do_command(client[app_type] + "-c \"read_reg ocp XXX\"", mode=2)
            do_command(client[app_type] + "-c \"read_reg_offset ocp XXX\"", mode=2)
            do_command(client[app_type] + "-c \"write_reg ocp XXX\"", mode=2)
            do_command(client[app_type] + "-c \"write_reg_offset ocp XXX\"", mode=2)

        if app_type == 1 and oprach:
            do_command(client[app_type] + "-f xorif_app_cmds_oprach.txt")
            do_command(client[app_type] + "-c \"echo 1 2 3\"", mode=2)
            do_command(client[app_type] + "-c \"read_reg oprach XXX\"", mode=2)
            do_command(client[app_type] + "-c \"read_reg_offset oprach XXX\"", mode=2)
            do_command(client[app_type] + "-c \"write_reg oprach XXX\"", mode=2)
            do_command(client[app_type] + "-c \"write_reg_offset oprach XXX\"", mode=2)

        # Using terminate at the end ensures any coverage info is recorded
        do_command(client[app_type] + "-c terminate")
    finally:
        # Kill server
        proc.kill()

def test_app_example_configs(app_type, bf, ocp, oprach):
    # Start server
    proc = subprocess.Popen(server[app_type].split())
    time.sleep(5)
    try:
        do_command(client[app_type] + "-f pg370_example1.txt")
        if not bf:
            do_command(client[app_type] + "-f pg370_example2.txt")
        # Using terminate at the end ensures any coverage info is recorded
        do_command(client[app_type] + "-c terminate")
    finally:
        # Kill server
        proc.kill()

def test_app_golden_configs(app_type, bf, ocp, oprach):
    # Start server
    proc = subprocess.Popen(server[app_type].split())
    time.sleep(5)
    try:
        do_command(client[app_type] + "-f config1.txt")
        assert os.system("diff libxorif.log libxorif-golden1.txt") == 0

        if bf:
            do_command(client[app_type] + "-f config2.txt")
            assert os.system("diff libxobf.log libxobf-golden2a.txt") == 0
            do_command(client[app_type] + "-f config3.txt")
            assert os.system("diff libxobf.log libxobf-golden3a.txt") == 0

        if app_type == 1 and ocp:
            do_command(client[app_type] + "-f config4.txt")

        # Using terminate at the end ensures any coverage info is recorded
        do_command(client[app_type] + "-c terminate")

    finally:
        # Kill server
        proc.kill()

if __name__ == "__main__":
    test_app_command_line(app_type=1, bf=False, ocp=True, oprach=False)
    #test_app_example_configs(app_type=1, bf=False, ocp=False, oprach=False)
    #test_app_golden_configs(app_type=1, bf=False, ocp=False, oprach=False)
