#!/usr/bin/env python3
#
# Copyright 2020 - 2022 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

__author__ = "Steven Dickinson"
__copyright__ = "Copyright 2022, Xilinx"

import sys
import logging
import argparse
import Pyro4
import time
from pprint import pprint

PROG = 'xorif-app-client'
PROG_SHORT = "XORIF-APP"
VERSION = '1.0'

SUCCESS = 0
FAIL = 1
EXIT = 2

logger = logging.getLogger(PROG_SHORT)
handles = {}
commands = []

"""
TODO
add reverse-look-up of error codes
add BF
add OCP
add OPRACH
add better output
"""

def match(s1, s2):
    """Perform case-insensitive string match."""
    return s1.upper() == s2.upper()

def integer(s):
    """
    Convert string representation of a number (either dex or hex) to an integer.
    """
    try:
        # Try and convert (auto: decimal, hex, oct, binary)
        return int(s, 0)
    except Exception as e:
        raise e

def help_cmd(args):
    # help
    if len(args) == 1:
        print("Available commands...")
        for cmd_name, _, help_text in commands:
            if help_text:
                if help_text[0] != "?":
                    print(f"\t{cmd_name:20}: {help_text}")
        return SUCCESS

    # help <topic>
    elif len(args) == 2:
        count = 0
        for cmd_name, _, help_text in commands:
            if match(args[1], cmd_name) or args[1] == "*":
                count += 1
                if help_text:
                    if help_text[0] != "?":
                        print(f"{cmd_name}: {help_text}")
                    else:
                        print(f"\tusage: {help_text[1:]}")
        if count > 0:
            return SUCCESS

def version_cmd(args):
    # version
    if len(args) == 1:
        print(f"Version {VERSION}")
        return SUCCESS

def magic_cmd(args):
    # magic
    print("Today's magic number is 42")
    return SUCCESS

def terminate_cmd(args):
    # terminate
    if len(args) == 1:
        result = True
        for name, handle in handles.items():
            if name == "FHI":
                result &= (handle.xorif_finish() == SUCCESS)
            elif name == "OCP":
                result &= (handle.xocp_finish(0) == SUCCESS)
            elif name == "OPRACH":
                result &= (handle.xoprach_finish(0) == SUCCESS)
        return EXIT

def exit_cmd(args):
    # quit
    if len(args) == 1:
        return EXIT

def wait_cmd(args):
    # wait <seconds>
    if len(args) == 2:
        time.sleep(integer(args[1]))
        return SUCCESS

def run_cmd(args):
    # run <file>
    if len(args) == 2:
        return do_file(args[1])

def debug_cmd(args):
    # debug <level>
    if len(args) == 2:
        level = integer(args[1])
        if level == 0:
            logging_level = logging.ERROR
        elif level == 1:
            logging_level = logging. INFO
        elif level >= 2:
            logging_level = logging.DEBUG
        for name, handle in handles.items():
            if name == "FHI":
                handle.xorif_debug(level)
                handle.set_log_level(logging_level)
            elif name == "OCP":
                handle.xocp_debug(level)
                handle.set_log_level(logging_level)
            elif name == "OPRACH":
                handle.xoprach_debug(level)
                handle.set_log_level(logging_level)
        return SUCCESS

    # debug (fhi | bf | ocp | ...) <level>
    elif len(args) == 3:
        name = args[1].upper()
        level = integer(args[2])
        if level == 0:
            logging_level = logging.ERROR
        elif level == 1:
            logging_level = logging. INFO
        elif level >= 2:
            logging_level = logging.DEBUG
        if name in handles:
            handle = handles[name]
            if name == "FHI":
                handle.xorif_debug(level)
                handle.set_log_level(logging_level)
            elif name == "OCP":
                handle.xocp_debug(level)
                handle.set_log_level(logging_level)
            elif name == "OPRACH":
                handle.xoprach_debug(level)
                handle.set_log_level(logging_level)
            return SUCCESS

def devices_cmd(args):
    # devices
    if len(args) == 1:
        pprint(handles)
        return SUCCESS

def init_cmd(args):
    # init
    if len(args) == 1:
        result = True
        for name, handle in handles.items():
            if name == "FHI":
                result &= (handle.xorif_init() == SUCCESS)
            elif name == "OCP":
                instance = handle.xocp_start()
                result &= (instance >= 0)
            elif name == "OPRACH":
                instance = handle.xoprach_start()
                result &= (instance >= 0)
        return SUCCESS if result else FAIL

    # init (fhi | bf | ocp | ...) [<device>]
    elif len(args) == 2 or len(args) == 3:
        name = args[1].upper()
        if len(args) == 3:
            device = args[2]
        else:
            device = None
        if name in handles:
            handle = handles[name]
            if name == "FHI":
               return handle.xorif_init(device)
            elif name == "OCP":
                instance = handle.xocp_start(device)
                return SUCCESS if (instance >= 0) else FAIL
            elif name == "OPRACH":
                instance = handle.xoprach_start(device)
                return SUCCESS if (instance >= 0) else FAIL

def finish_cmd(args):
    # finish
    if len(args) == 1:
        for name, handle in handles.items():
            if name == "FHI":
               handle.xorif_finish()
            elif name == "OCP":
               handle.xocp_finish(0)
            elif name == "OPRACH":
               handle.xoprach_finish(0)
        return SUCCESS

    # finish (fhi | bf | ocp | ...)
    elif len(args) == 2:
        name = args[1].upper()
        if name in handles:
            handle = handles[name]
            if name == "FHI":
                handle.xorif_finish()
            elif name == "OCP":
                handle.xocp_finish(0)
            elif name == "OPRACH":
               handle.xoprach_finish(0)
            return SUCCESS

def has_cmd(args):
    # has (fhi | bf | ocp | ...)
    if len(args) == 2:
        print(f"{args[1].upper() in handles}")
        return SUCCESS

def reset_cmd(args):
    # reset
    if  len(args) == 1:
        result = True
        for name, handle in handles.items():
            if name == "FHI":
                result &= (handle.xorif_reset_fhi(0) == SUCCESS)
            elif name == "OCP":
                result &= (handle.xocp_reset(0, 0) == SUCCESS)
            elif name == "OPRACH":
                result &= (handle.xoprach_reset(0) == SUCCESS)
        return SUCCESS if result else FAIL

    # reset (fhi | bf | ocp | ...) [<mode>]
    elif len(args) == 2 or len(args) == 3:
        name = args[1].upper()
        mode = integer(args[2]) if len(args) == 3 else 0
        if name in handles:
            handle = handles[name]
            if name == "FHI":
                return handle.xorif_reset_fhi(mode)
            elif name == "OCP":
                return handle.xocp_reset(0, mode)
            elif name == "OPRACH":
                return handle.xoprach_reset(0)

def get_cmd(args):
    if len(args) >= 2:
        # get sw_version
        if match(args[1], "sw_version"):
            if len(args) == 2:
                for name, handle in handles.items():
                    if name == "FHI":
                        print(f"{name}: {handle.xorif_get_sw_version()}")
                    elif name == "BF":
                        print(f"{name}: {handle.xobf_get_sw_version()}")
                    elif name == "OCP":
                        print(f"{name}: {handle.xocp_get_sw_version()}")
                    elif name == "OPRACH":
                        print(f"{name}: {handle.xoprach_get_sw_version()}")
                return SUCCESS

        # get fhi_sw_version
        if match(args[1], "fhi_sw_version"):
            if len(args) == 2 and "FHI" in handles:
                handle = handles["FHI"]
                print(f"{handle.xorif_get_sw_version()}")
                return SUCCESS

        # get fhi_hw_version
        if match(args[1], "fhi_hw_version"):
            if len(args) == 2 and "FHI" in handles:
                handle = handles["FHI"]
                print(f"{handle.xorif_get_fhi_hw_version()}")
                return SUCCESS

        # get fhi_hw_internal_rev
        if match(args[1], "fhi_hw_internal_rev"):
            if len(args) == 2 and "FHI" in handles:
                handle = handles["FHI"]
                print(f"{handle.xorif_get_fhi_hw_internal_rev()}")
                return SUCCESS

        # get (fhi_capabilities | fhi_caps)
        if (match(args[1], "fhi_capabilities") or match(args[1], "fhi_caps")):
            if len(args) == 2 and "FHI" in handles:
                handle = handles["FHI"]
                caps = handle.xorif_get_capabilities()
                pprint(caps)
                return SUCCESS

        # get fhi_alarms
        if match(args[1], "fhi_alarms"):
            if len(args) == 2 and "FHI" in handles:
                handle = handles["FHI"]
                print(f"{handle.xorif_get_fhi_alarms()}")
                return SUCCESS

        # get fhi_state
        if match(args[1], "fhi_state"):
            if len(args) == 2 and "FHI" in handles:
                handle = handles["FHI"]
                print(f"{handle.xorif_get_state()}")
                return SUCCESS

        # get fhi_enabled
        if match(args[1], "fhi_enabled"):
            if len(args) == 2 and "FHI" in handles:
                handle = handles["FHI"]
                print(f"0x{handle.xorif_get_enabled_cc_mask():02x}")
                return SUCCESS

        # get fhi_cc_config <cc>
        if match(args[1], "fhi_cc_config"):
            if len(args) == 3 and "FHI" in handles:
                handle = handles["FHI"]
                result, config = handle.xorif_get_cc_config(integer(args[2]))
                if result == SUCCESS:
                    pprint(config)
                return result

        # get fhi_cc_alloc <cc>
        if match(args[1], "fhi_cc_alloc"):
            if len(args) == 3 and "FHI" in handles:
                handle = handles["FHI"]
                result, config = handle.xorif_get_fhi_cc_alloc(integer(args[2]))
                if result == SUCCESS:
                    pprint(config)
                return result

        # get fhi_stats <port>
        if match(args[1], "fhi_stats"):
            if len(args) == 3 and "FHI" in handles:
                handle = handles["FHI"]
                result, config = handle.xorif_get_fhi_eth_stats(integer(args[2]))
                if result == SUCCESS:
                    pprint(config)
                return result

        # get ocp_sw_version
        if match(args[1], "ocp_sw_version"):
            if len(args) == 2 and "OCP" in handles:
                handle = handles["OCP"]
                print(f"{handle.xocp_get_sw_version()}")
                return SUCCESS

        # get ocp_hw_version
        if match(args[1], "ocp_hw_version"):
            if len(args) == 2 and "OCP" in handles:
                handle = handles["OCP"]
                print(f"{handle.xocp_get_hw_version(0)}")
                return SUCCESS

        # get ocp_hw_internal_rev
        if match(args[1], "ocp_hw_internal_rev"):
            if len(args) == 2 and "OCP" in handles:
                handle = handles["OCP"]
                print(f"{handle.xocp_get_hw_internal_rev(0)}")
                return SUCCESS

        # get (ocp_capabilities | ocp_caps)
        if match(args[1], "ocp_capabilities") or match(args[1], "ocp_caps"):
            if len(args) == 2 and "OCP" in handles:
                handle = handles["OCP"]
                result, caps = handle.xocp_get_capabilities(0)
                if result == SUCCESS:
                    pprint(caps)
                return result

        # get ocp_state
        if match(args[1], "ocp_state"):
            if len(args) == 2 and "OCP" in handles:
                handle = handles["OCP"]
                print(f"{handle.xocp_get_state(0)}")
                return SUCCESS

        # get ocp_events
        if match(args[1], "ocp_events"):
            if len(args) == 2 and "OCP" in handles:
                handle = handles["OCP"]
                result, status = handle.xocp_get_event_status(0)
                if result == SUCCESS:
                    print(status)
                return result

        # get ocp_cc_cfg <cc>
        if match(args[1], "ocp_cc_cfg"):
            if len(args) == 3 and "OCP" in handles:
                handle = handles["OCP"]
                result, cfg = handle.xocp_get_cc_cfg(0, integer(args[2]))
                if result == SUCCESS:
                    pprint(cfg)
                return result

        # get ocp_antenna_cfg
        if match(args[1], "ocp_antenna_cfg"):
            if len(args) == 2 and "OCP" in handles:
                handle = handles["OCP"]
                result, cfg = handle.xocp_get_antenna_cfg(0)
                if result == SUCCESS:
                    pprint(cfg)
                return result

        # get ocp_trigger_cfg
        if match(args[1], "ocp_trigger_cfg"):
            if len(args) == 2 and "OCP" in handles:
                handle = handles["OCP"]
                result, cfg = handle.xocp_get_trigger_cfg(0)
                if result == SUCCESS:
                    pprint(cfg)
                return result

        # get oprach_sw_version
        if match(args[1], "oprach_sw_version"):
            if len(args) == 2 and "OPRACH" in handles:
                handle = handles["OPRACH"]
                print(f"{handle.xoprach_get_sw_version()}")
                return SUCCESS

        # get oprach_hw_version
        if match(args[1], "oprach_hw_version"):
            if len(args) == 2 and "OPRACH" in handles:
                handle = handles["OPRACH"]
                print(f"{handle.xoprach_get_hw_version(0)}")
                return SUCCESS

        # get (oprach_capabilities | oprach_caps)
        if match(args[1], "oprach_capabilities") or match(args[1], "oprach_caps"):
            if len(args) == 2 and "OPRACH" in handles:
                handle = handles["OPRACH"]
                result, caps = handle.xoprach_get_capabilities(0)
                if result == SUCCESS:
                    pprint(caps)
                return result

        # get oprach_state
        if match(args[1], "oprach_state"):
            if len(args) == 2 and "OPRACH" in handles:
                handle = handles["OPRACH"]
                print(f"{handle.xoprach_get_state(0)}")
                return SUCCESS

        # get oprach_events
        if match(args[1], "oprach_events"):
            if len(args) == 2 and "OPRACH" in handles:
                handle = handles["OPRACH"]
                result, status = handle.xoprach_get_event_status(0)
                if result == SUCCESS:
                    print(status)
                return result

        # get oprach_cc_cfg
        if match(args[1], "oprach_cc_cfg"):
            if len(args) == 2 and "OPRACH" in handles:
                handle = handles["OPRACH"]
                result, cfg = handle.xoprach_get_cc_cfg(0)
                if result == SUCCESS:
                    pprint(cfg)
                return result

        # get oprach_rcid_cfg
        if match(args[1], "oprach_rcid_cfg"):
            if len(args) == 2 and "OPRACH" in handles:
                handle = handles["OPRACH"]
                result, cfg = handle.xoprach_get_rcid_cfg(0)
                if result == SUCCESS:
                    pprint(cfg)
                return result

        # get oprach_trigger_cfg
        if match(args[1], "oprach_trigger_cfg"):
            if len(args) == 2 and "OPRACH" in handles:
                handle = handles["OPRACH"]
                result, cfg = handle.xoprach_get_trigger_cfg(0)
                if result == SUCCESS:
                    pprint(cfg)
                return result

def set_cmd(args):
    if len(args) >= 2:
        # set num_rbs <cc> <number_of_rbs>
        if match(args[1], "num_rbs"):
            if len(args) == 4:
                cc = integer(args[2])
                num_rbs = integer(args[3])
                result = True
                for name, handle in handles.items():
                    if name == "FHI":
                        result &= (handle.xorif_set_cc_num_rbs(cc, num_rbs) == SUCCESS)
                return SUCCESS if result else FAIL

        # set numerology <cc> <numerology = 0..4> <extended_cp = 0|1>
        if match(args[1], "numerology"):
            if len(args) == 5:
                cc = integer(args[2])
                numerology = integer(args[3])
                ext_cp = integer(args[4])
                result = True
                for name, handle in handles.items():
                    if name == "FHI":
                        result &= (handle.xorif_set_cc_numerology(cc, numerology, ext_cp) == SUCCESS)
                return SUCCESS if result else FAIL

        # set numerology_ssb <cc> <numerology = 0..4> <extended_cp = 0|1>
        if match(args[1], "numerology_ssb"):
            if len(args) == 5:
                cc = integer(args[2])
                numerology = integer(args[3])
                ext_cp = integer(args[4])
                result = True
                for name, handle in handles.items():
                    if name == "FHI":
                        result &= (handle.xorif_set_cc_numerology_ssb(cc, numerology, ext_cp) == SUCCESS)
                return SUCCESS if result else FAIL

        # set time_advance <cc> <deskew> <advance_uplink> <advance_downlink> # deprecated
        if match(args[1], "time_advance"):
            if len(args) == 6 and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                deskew = float(args[3])
                adv_ul = float(args[4])
                adv_dl = float(args[5])
                return handle.xorif_set_cc_time_advance(cc, deskew, adv_ul, adv_dl)

        # set ul_timing_params <cc> <delay_comp> <advance> <radio_ch_delay>
        if match(args[1], "ul_timing_params"):
            if len(args) == 6 and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                delay_comp = float(args[3])
                advance = float(args[4])
                radio_ch_delay = float(args[5])
                return handle.xorif_set_cc_ul_timing_parameters(cc, delay_comp, advance, radio_ch_delay)

        # set dl_timing_params <cc> <delay_comp_cp> <delay_comp_up> <advance>
        if match(args[1], "dl_timing_params"):
            if len(args) == 6 and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                delay_comp_cp = float(args[3])
                delay_comp_up = float(args[4])
                advance = float(args[5])
                return handle.xorif_set_cc_dl_timing_parameters(cc, delay_comp_cp, delay_comp_up, advance)

        # set ul_bid_forward <cc> <time>
        if match(args[1], "ul_bid_forward"):
            if len(args) == 4 and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                time = float(args[3])
                return handle.xorif_set_ul_bid_forward(cc,time)

        # set ul_bid_forward_fine <cc> <symbols> <cycles>
        if match(args[1], "ul_bid_forward_fine"):
            if len(args) == 5:
                handle = handles["FHI"]
                cc = integer(args[2])
                symbols = integer(args[3])
                cycles = integer(args[4])
                # No API call for this, instead use register writes
                handle.xorif_write_fhi_reg_offset("ORAN_CC_UL_BIDF_C_ABS_SYMBOL", 0x70 * cc, symbols)
                handle.xorif_write_fhi_reg_offset("ORAN_CC_UL_BIDF_C_CYCLES", 0x70 * cc, cycles)
                return SUCCESS

        # set ul_radio_ch_dly <cc> <delay> # deprecated
        if match(args[1], "ul_radio_ch_dly"):
            if len(args) == 4 and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                delay = float(args[3])
                return handle.xorif_set_ul_radio_ch_dly(cc, delay)

        # set (dl_iq_compression | dl_iq_comp) <cc> <width> <method = 0..4> [<mplane = 0|1>]
        if match(args[1], "dl_iq_compression") or match(args[1], "dl_iq_comp"):
            if (len(args) == 5 or len(args) == 6) and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                width = integer(args[3])
                method = integer(args[4])
                mplane = integer(args[5]) if len(args) == 6 else 1
                return handle.xorif_set_cc_dl_iq_compression(cc, width, method, mplane)

        # set (ul_iq_compression | ul_iq_comp) <cc> <width> <method = 0..4> [<mplane = 0|1>]
        if match(args[1], "ul_iq_compression") or match(args[1], "ul_iq_comp"):
            if (len(args) == 5 or len(args) == 6) and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                width = integer(args[3])
                method = integer(args[4])
                mplane = integer(args[5]) if len(args) == 6 else 1
                return handle.xorif_set_cc_ul_iq_compression(cc, width, method, mplane)

        # set (ssb_iq_compression | ssb_iq_comp) <cc> <width> <method = 0..4> [<mplane = 0|1>]
        if match(args[1], "ssb_iq_compression") or match(args[1], "ssb_iq_comp"):
            if (len(args) == 5 or len(args) == 6) and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                width = integer(args[3])
                method = integer(args[4])
                mplane = integer(args[5]) if len(args) == 6 else 1
                return handle.xorif_set_cc_iq_compression_ssb(cc, width, method, mplane)

        # set (prach_iq_compression | prach_iq_comp) <cc> <width> <method = 0..4> [<mplane = 0|1>]
        if match(args[1], "prach_iq_compression") or match(args[1], "prach_iq_comp"):
            if (len(args) == 5 or len(args) == 6) and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                width = integer(args[3])
                method = integer(args[4])
                mplane = integer(args[5]) if len(args) == 6 else 1
                return handle.xorif_set_cc_iq_compression_prach(cc, width, method, mplane)

        # set (per_ss_decompression | per_ss_decomp) <ss> <width> <method = 0..4> <enable = 0|1> [<number>]
        if match(args[1], "per_ss_decompression") or match(args[1], "per_ss_decomp"):
            if (len(args) == 6 or len(args) == 7) and "FHI" in handles:
                handle = handles["FHI"]
                ss = integer(args[2])
                width = integer(args[3])
                method = integer(args[4])
                enable = integer(args[5])
                number = integer(args[6]) if len(args) == 7 else 1
                return handle.xorif_set_cc_dl_iq_compression_per_ss(ss, width, method, enable, number)

        # set dl_sections_per_sym <cc> <number_of_sections> [<number_of_ctrl_words>]
        if match(args[1], "dl_sections_per_sym"):
            if (len(args) == 4 or len(args) == 5) and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                num_sections = integer(args[3])
                num_ctrl = integer(args[4]) if len(args) == 5 else num_sections
                return handle.xorif_set_cc_dl_sections_per_symbol(cc, num_sections, num_ctrl)

        # set ul_sections_per_sym <cc> <number_of_sections> [<number_of_ctrl_words>]
        if match(args[1], "ul_sections_per_sym"):
            if (len(args) == 4 or len(args) == 5) and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                num_sections = integer(args[3])
                num_ctrl = integer(args[4]) if len(args) == 5 else num_sections
                return handle.xorif_set_cc_ul_sections_per_symbol(cc, num_sections, num_ctrl)

        # set ssb_sections_per_sym <cc> <number_of_sections> [<number_of_ctrl_words>]
        if match(args[1], "ssb_sections_per_sym"):
            if (len(args) == 4 or len(args) == 5) and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                num_sections = integer(args[3])
                num_ctrl = integer(args[4]) if len(args) == 5 else num_sections
                return handle.xorif_set_cc_sections_per_symbol_ssb(cc, num_sections, num_ctrl)

        # set frames_per_sym <cc> <number_of_frames>
        if match(args[1], "frames_per_sym"):
            if len(args) == 4 and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                num_frames = integer(args[3])
                return handle.xorif_set_cc_frames_per_symbol(cc, num_frames)

        # set frames_per_sym_ssb <cc> <number_of_frames>
        if match(args[1], "frames_per_sym_ssb"):
            if len(args) == 4 and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                num_frames = integer(args[3])
                return handle.xorif_set_cc_frames_per_symbol_ssb(cc, num_frames)

        # set dest_mac_addr <port> <address = XX:XX:XX:XX:XX:XX>
        if match(args[1], "dest_mac_addr"):
            if len(args) == 4 and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                return handle.xorif_set_fhi_dest_mac_addr(cc, args[3])

        # set src_mac_addr <port> <address = XX:XX:XX:XX:XX:XX>
        if match(args[1], "src_mac_addr"):
            if len(args) == 4 and "FHI" in handles:
                handle = handles["FHI"]
                cc = integer(args[2])
                return handle.xorif_set_fhi_src_mac_addr(cc, args[3])

        # set vlan <port> <id> <dei> <pcp>
        if match(args[1], "vlan"):
            if len(args) == 6 and "FHI" in handles:
                handle = handles["FHI"]
                port = integer(args[2])
                id = integer(args[3])
                dei = integer(args[4])
                pcp = integer(args[5])
                return handle.xorif_set_fhi_vlan_tag(port, id, dei, pcp)

        # set modu_mode <0 = disabled | 1 = enabled>
        if match(args[1], "modu_mode"):
            if len(args) == 3 and "FHI" in handles:
                handle = handles["FHI"]
                enabled = integer(args[2])
                return handle.xorif_set_modu_mode(enabled)

        # set modu_dest_mac_addr <du> <address = XX:XX:XX:XX:XX:XX> [<id> <dei> <pcp>]
        if match(args[1], "modu_dest_mac_addr"):
            if (len(args) == 4 or len(args) == 7) and "FHI" in handles:
                handle = handles["FHI"]
                du = integer(args[2])
                if len(args) == 7:
                    id = integer(args[4])
                    dei = integer(args[5])
                    pcp = integer(args[6])
                else:
                    id, dei, pcp = 0, 0, 0
                return handle.xorif_set_modu_dest_mac_addr(du, args[3], id, dei, pcp)

        # set mtu_size <size>
        if match(args[1], "mtu_size"):
            if (len(args) == 3) and "FHI" in handles:
                handle = handles["FHI"]
                size = integer(args[2])
                return handle.xorif_set_mtu_size(size)

        # set protocol <ECPRI | 1914.3> <VLAN = 0|1> <RAW | IPv4 | IPv6> # sets packet filter
        # set protocol_alt <ECPRI | 1914.3> <VLAN = 0|1> <RAW | IPv4 | IPv6> # doesn't set packet filter
        if match(args[1], "protocol") or match(args[1], "protocol_alt"):
            if len(args) == 5 and "FHI" in handles:
                handle = handles["FHI"]
                if match(args[2], "ECPRI"):
                    transport = handle.constants["PROTOCOL_ECPRI"]
                elif match(args[2], "1914.3"):
                    transport = handle.constants["PROTOCOL_IEEE_1914_3"]
                else:
                    return
                vlan = integer(args[3])
                if match(args[4], "RAW"):
                    mode = handle.constants["IP_MODE_RAW"]
                elif match(args[4], "IPv4"):
                    mode = handle.constants["IP_MODE_IPV4"]
                elif match(args[4], "IPv6"):
                    mode = handle.constants["IP_MODE_IPV6"]
                else:
                    return
                if  match(args[1], "protocol"):
                    return handle.xorif_set_fhi_protocol(transport, vlan, mode)
                else:
                    return handle.xorif_set_fhi_protocol_alt(transport, vlan, mode)

        # set packet_filter <port> <filter = (16 x 32b)> <mask = (4 x 16b)>
        if match(args[1], "packet_filter"):
            if len(args) == 23 and "FHI" in handles:
                handle = handles["FHI"]
                port = integer(args[2])
                filter = []
                for i in range(16):
                    filter.append(integer(args[3 + i]))
                mask = []
                for i in range(4):
                    mask.append(integer(args[19 + i]))
                return handle.xorif_set_fhi_packet_filter(port, filter, mask)

        # set eaxc_id <DU_bits> <BS_bits> <CC_bits> <RU_bits>
        if match(args[1], "eaxc_id"):
            if len(args) == 6 and "FHI" in handles:
                handle = handles["FHI"]
                du_bits = integer(args[2])
                bs_bits = integer(args[3])
                cc_bits = integer(args[4])
                ru_bits = integer(args[5])
                return handle.xorif_set_fhi_eaxc_id(du_bits, bs_bits, cc_bits, ru_bits)

        # set ru_ports <RU_bits> <SS_bits> <mask> <user_value> <prach_value> <ssb_value> [<lte_value>]
        if match(args[1], "ru_ports"):
            if (len(args) == 8) or (len(args) == 9) and "FHI" in handles:
                handle = handles["FHI"]
                ru_bits = integer(args[2])
                ss_bits = integer(args[3])
                mask = integer(args[4])
                user_val = integer(args[5])
                prach_val = integer(args[6])
                ssb_val = integer(args[7])
                lte_val = integer(args[8]) if len(args) == 9 else None
                if len(args) == 8:
                    return handle.xorif_set_ru_ports(ru_bits, ss_bits, mask, user_val, prach_val, ssb_val)
                else:
                    return handle.xorif_set_ru_ports_alt1(ru_bits, ss_bits, mask, user_val, prach_val, ssb_val, lte_val)

        # set ru_ports_table_mode <mode>
        if match(args[1], "ru_ports_table_mode"):
            if len(args) == 3 and "FHI" in handles:
                handle = handles["FHI"]
                mode = integer(args[2])
                return handle.xorif_set_ru_ports_table_mode(mode)

        # set ru_ports_table <address> <port> <type> [<number>]
        if match(args[1], "ru_ports_table"):
            if (len(args) == 5 or len(args) == 6) and "FHI" in handles:
                handle = handles["FHI"]
                address = integer(args[2])
                port = integer(args[3])
                type = integer(args[4])
                number = integer(args[5]) if len(args) == 6 else 1
                return handle.xorif_set_ru_ports_table(address, port, type, number)

        # set system_constants <fh_decap_dly> {...}
        if match(args[1], "system_constants"):
            if len(args) == 3 and "FHI" in handles:
                handle = handles["FHI"]
                delay = float(args[2])
                sys_const = {"FH_DECAP_DLY": delay}
                return handle.xorif_set_system_constants(sys_const)

        # set ocp_cc_cfg <cc> <enable> <num_rbs> <numerology> <ccid> [<inter_sym_gap>]
        if match(args[1], "ocp_cc_cfg"):
            if (len(args) == 7 or len(args) == 8) and "OCP" in handles:
                handle = handles["OCP"]
                cc = integer(args[2])
                cfg = {}
                cfg["enable"] = integer(args[3])
                cfg["num_rbs"] = integer(args[4])
                cfg["numerology"] = integer(args[5])
                cfg["ccid"] = integer(args[6])
                if len(args) == 8:
                    cfg["inter_sym_gap"] = integer(args[7])
                else:
                    cfg["inter_sym_gap"] = 0
                return handle.xocp_set_cc_cfg(0, cc, cfg)

        # set ocp_antenna_cfg <num_antennas> <interleave> <data = (8 x 4b)>
        if match(args[1], "ocp_antenna_cfg"):
            if len(args) == 12 and "OCP" in handles:
                handle = handles["OCP"]
                cfg = {}
                cfg["num_antennas"] = integer(args[2])
                cfg["interleave"] = integer(args[3])
                data = []
                for i in range(8):
                    data.append(integer(args[4 + i]))
                cfg["data"] = data
                return handle.xocp_set_antenna_cfg(0, cfg)

        # set ocp_trigger_cfg <enable> <mode> <edge_level> <bit> <enable> <mode> <edge_level> <bit>
        if match(args[1], "ocp_trigger_cfg"):
            if len(args) == 10 and "OCP" in handles:
                handle = handles["OCP"]
                cfg1 = {}
                cfg1["enable"] = integer(args[2])
                cfg1["mode"] = integer(args[3])
                cfg1["tuser_edge_level"] = integer(args[4])
                cfg1["tuser_bit"] = integer(args[5])
                cfg2 = {}
                cfg2["enable"] = integer(args[6])
                cfg2["mode"] = integer(args[7])
                cfg2["tuser_edge_level"] = integer(args[8])
                cfg2["tuser_bit"] = integer(args[9])
                return handle.xocp_set_trigger_cfg(0, {"dl_update": cfg1, "ul_update": cfg2})

        # set ocp_schedule <mode> <length> {sequence}
        if match(args[1], "ocp_schedule"):
            if len(args) >= 4 and "OCP" in handles:
                handle = handles["OCP"]
                mode = integer(args[2])
                length = integer(args[3])
                if len(args) != 4 + length:
                    return FAIL
                sequence = []
                for i in range(length):
                    sequence.append(integer(args[4 + i]))
                return handle.xocp_set_schedule(0, mode, length, sequence)

        # set oprach_cc_cfg {<enable> <sample_rate>}
        if match(args[1], "oprach_cc_cfg"):
            if len(args) >= 2 and "OPRACH" in handles:
                handle = handles["OPRACH"]
                cfg = [{"enable": 0, "sample_rate": 0} for _ in range(16)]
                for i in range((len(args) - 2) // 2):
                    cfg[i]["enable"] = integer(args[2 + i * 2])
                    cfg[i]["sample_rate"] = integer(args[3 + i * 2])
                return handle.xoprach_set_cc_cfg(0, cfg)

        # set oprach_rcid_cfg {<ss> <cc>}
        if match(args[1], "oprach_rcid_cfg"):
            if len(args) >= 2 and "OPRACH" in handles:
                handle = handles["OPRACH"]
                cfg = [{"ss": 0, "cc": 0} for _ in range(16)]
                for i in range((len(args) - 2) // 2):
                    cfg[i]["ss"] = integer(args[2 + i * 2])
                    cfg[i]["cc"] = integer(args[3 + i * 2])
                return handle.xoprach_set_rcid_cfg(0, cfg)

        # set oprach_trigger_cfg <enable> <mode> <edge_level> <bit> <enable> <mode> <edge_level> <bit>
        if match(args[1], "oprach_trigger_cfg"):
            if len(args) == 10 and "OPRACH" in handles:
                handle = handles["OPRACH"]
                cfg1 = {}
                cfg1["enable"] = integer(args[2])
                cfg1["mode"] = integer(args[3])
                cfg1["tuser_edge_level"] = integer(args[4])
                cfg1["tuser_bit"] = integer(args[5])
                cfg2 = {}
                cfg2["enable"] = integer(args[6])
                cfg2["mode"] = integer(args[7])
                cfg2["tuser_edge_level"] = integer(args[8])
                cfg2["tuser_bit"] = integer(args[9])
                return handle.xoprach_set_trigger_cfg(0, {"activate": cfg1, "update": cfg2})

def configure_cmd(args):
    # configure <cc>
    if len(args) == 2:
        cc = integer(args[1])
        result = True
        for name, handle in handles.items():
            if name == "FHI":
                result &= (handle.xorif_configure_cc(cc) == SUCCESS)
        return SUCCESS if result else FAIL

    # configure (fhi | bf) <cc>
    elif len(args) == 3:
        name = args[1].upper()
        cc = integer(args[2])
        if name in handles:
            handle = handles[name]
            if name == "FHI":
                return handle.xorif_configure_cc(cc)

def enable_cmd(args):
    # enable <cc>
    if len(args) == 2:
        cc = integer(args[1])
        result = True
        for name, handle in handles.items():
            if name == "FHI":
                result &= (handle.xorif_enable_cc(cc) == SUCCESS)
        return SUCCESS if result else FAIL

    # enable (fhi | bf | ...) <cc>
    elif len(args) == 3:
        name = args[1].upper()
        cc = integer(args[2])
        if name in handles:
            handle = handles[name]
            if name == "FHI":
                return handle.xorif_enable_cc(cc)

def disable_cmd(args):
    # disable <cc>
    if len(args) == 2:
        cc = integer(args[1])
        result = True
        for name, handle in handles.items():
            if name == "FHI":
                result &= (handle.xorif_disable_cc(cc) == SUCCESS)
        return SUCCESS if result else FAIL

    # disable (fhi | bf | ...) <cc>
    elif len(args) == 3:
        name = args[1].upper()
        cc = integer(args[2])
        if name in handles:
            handle = handles[name]
            if name == "FHI":
                return handle.xorif_disable_cc(cc)

def clear_cmd(args):
    if len(args) == 2:
        # clear (fhi_alarms | fhi_stats | ru_ports_table)
        if "FHI" in handles:
            handle = handles["FHI"]
            if match(args[1], "fhi_alarms"):
                handle.xorif_clear_fhi_alarms()
                return SUCCESS
            elif match(args[1], "fhi_stats"):
                handle.xorif_clear_fhi_stats()
                return SUCCESS
            elif match(args[1], "ru_ports_table"):
                handle.xorif_clear_ru_ports_table()
                return SUCCESS

        # clear ocp_events
        if "OCP" in handles:
            handle = handles["OCP"]
            if match(args[1], "ocp_events"):
                return handle.xocp_clear_event_status(0)

        # clear oprach_events
        if "OPRACH" in handles:
            handle = handles["OPRACH"]
            if match(args[1], "oprach_events"):
                return handle.xoprach_clear_event_status(0)

def read_reg_cmd(args):
    # read_reg (fhi | bf | ...) <name>
    if len(args) == 3:
        name = args[1].upper()
        reg = args[2]
        result = FAIL
        if name in handles:
            handle = handles[name]
            if name == "FHI":
                result, value = handle.xorif_read_fhi_reg(reg)
            elif name == "BF":
                result, value = handle.xobf_read_bf_reg(reg)
            elif name == "OCP":
                result, value = handle.xocp_read_reg(0, reg)
            elif name == "OPRACH":
                result, value = handle.xoprach_read_reg(0, reg)
        if result == SUCCESS:
            print(f"0x{value:x}")
        return result

def read_reg_offset_cmd(args):
    # read_reg_offset (fhi | bf | ocp | ...) <name> <offset>
    if len(args) == 4:
        name = args[1].upper()
        reg = args[2]
        offset = integer(args[3])
        result = FAIL
        if name in handles:
            handle = handles[name]
            if name == "FHI":
                result, value = handle.xorif_read_fhi_reg_offset(reg, offset)
            elif name == "BF":
                result, value = handle.xobf_read_bf_reg_offset(reg, offset)
            elif name == "OCP":
                result, value = handle.xocp_read_reg(0, reg, offset)
            elif name == "OPRACH":
                result, value = handle.xoprach_read_reg(0, reg, offset)
        if result == SUCCESS:
            print(f"0x{value:x}")
        return result

def write_reg_cmd(args):
    # write_reg (fhi | bf | ocp | ...) <name> <value>
    if len(args) == 4:
        name = args[1].upper()
        reg = args[2]
        value = integer(args[3])
        if name in handles:
            handle = handles[name]
            if name == "FHI":
                return handle.xorif_write_fhi_reg(reg, value)
            elif name == "BF":
                return handle.xobf_write_bf_reg(reg, value)
            elif name == "OCP":
                return handle.xocp_write_reg(0, reg, value)
            elif name == "OPRACH":
                return handle.xoprach_write_reg(0, reg, value)

def write_reg_offset_cmd(args):
    # write_reg_offset (fhi | bf | ocp | ...) <name> <offset> <value>
    if len(args) == 5:
        name = args[1].upper()
        reg = args[2]
        offset = integer(args[3])
        value = integer(args[4])
        if name in handles:
            handle = handles[name]
            if name == "FHI":
                return handle.xorif_write_fhi_reg_offset(reg, offset, value)
            elif name == "BF":
                return handle.xobf_write_bf_reg_offset(reg, offset, value)
            elif name == "OCP":
                return handle.xocp_write_reg_offset(0, reg, offset, value)
            elif name == "OPRACH":
                return handle.xoprach_write_reg_offset(0, reg, offset, value)

def dump_cmd(args):
    logging.info("'dump' is deprecated in the Python version of the application")
    return SUCCESS

def monitor_cmd(args):
    # monitor (fhi | ocp | ...) clear
    # monitor (fhi | ocp | ...) snapshot
    if len(args) == 3:
        name = args[1].upper()
        if name in handles:
            handle = handles[name]
            if match(args[2], "clear"):
                if name == "FHI":
                    return handle.xorif_monitor_clear()
                elif name == "OCP":
                    return handle.xocp_monitor_clear(0)
            elif match(args[2], "snapshot"):
                if name == "FHI":
                    return handle.xorif_monitor_snapshot()
                elif name == "OCP":
                    return handle.xocp_monitor_snapshot(0)

    # monitor (fhi | ocp | ...) select <stream>
    # monitor (fhi | ocp | ...) read <counter>
    elif len(args) == 4:
        name = args[1].upper()
        if name in handles:
            handle = handles[name]
            if match(args[2], "select"):
                if name == "FHI":
                    return handle.xorif_monitor_select(integer(args[3]))
            elif match(args[2], "read"):
                if name == "FHI":
                    result, value = handle.xorif_monitor_read(integer(args[3]))
                    if result == SUCCESS:
                        print(value)
                    return result
                elif name == "OCP":
                    result, value = handle.xocp_monitor_read(0, integer(args[3]))
                    if result == SUCCESS:
                        print(value)
                    return result

def activate_cmd(args):
    # activate (ocp | oprach | ...)
    if len(args) == 2:
        name = args[1].upper()
        if name in handles:
            handle = handles[name]
            if name == "OCP":
                result = handle.xocp_reset(0, 0)
                if result == SUCCESS:
                    result = handle.xocp_activate(0)
                return result
            elif name == "OPRACH":
                result = handle.xoprach_configure(0)
                if result == SUCCESS:
                    result = handle.xoprach_initialize(0)
                if result == SUCCESS:
                    result = handle.xoprach_activate(0)
                return result

def trigger_cmd(args):
    # trigger (ocp | oprach | ...)
    if len(args) == 2:
        name = args[1].upper()
        if name in handles:
            handle = handles[name]
            if name == "OCP":
                return handle.xocp_trigger_update(0)
            elif name == "OPRACH":
                return handle.xoprach_trigger_update(0)

def deactivate_cmd(args):
    # deactivate oprach
    if len(args) == 2:
        name = args[1].upper()
        if name in handles:
            handle = handles[name]
            if name == "OPRACH":
                return handle.xoprach_deactivate(0)

def parse(command):
    """
    Parse command (space-separated tokens).
    Returns SUCCESS, EXIT or FAIL.
    The commands themselves can return SUCCESS, EXIT, FAIL or None.
    If the command returns None then the parser will try other
    matching commands until one of them returns SUCCESS, EXIT or FAIL. 
    """

    # Remove any text after comment symbol
    if "#" in command:
        command = command[:command.find("#")]

    # Tokenize
    args = command.strip().split()
    command = ' '.join(args)

    # Process
    if args:
        found = False
        for cmd_name, cmd_func, _ in commands:
            if match(args[0], cmd_name):
                found = True
                if cmd_func:
                    try:
                        result = cmd_func(args)
                        if result == SUCCESS:
                            logger.info(f"Success: '{command}'")
                            return SUCCESS
                        elif result == EXIT:
                            logger.info("Exiting...")
                            return EXIT
                        elif result == None:
                            pass
                        else:
                            logger.error(f"Error executing command: '{command}' ({result})")
                            return FAIL
                    except Exception as e:
                        logger.error(f"Error: {e}")
                        return FAIL
        if not found:
            logger.error(f"Unknown command: '{command}'")
            #help_cmd(["help"])
        else:
            logger.error(f"Malformed / invalid command: '{command}'")
            #help_cmd(["help", args[0]])
        return FAIL
    else:
        return SUCCESS

def do_command(command):
    """
    Command-line mode.
    Process commands from the command line arguments.
    """
    return parse(command)

def do_interactive(_):
    """
    Interactive mode.
    Process user input at the command prompt.
    Note, Python 'input()' does not allow cursor / edits :-(
    """
    try:
        while True:
            if parse(input('> ')) == EXIT:
                return EXIT
    except KeyboardInterrupt:
        logger.error("Exiting...")
        return EXIT
    except Exception as e:
        logger.error(f"Error: {e}")
        return FAIL

def do_file(file_name):
    """
    File mode.
    Run commands from a file.
    Empty lines or lines beginning with '#' are ignored (comments).
    """
    try:
        with open(file_name, "r") as f:
            for num, line in enumerate(f):
                result = parse(line)
                if result == FAIL:
                    logger.error(f"Error reading file line {num + 1}")
                    return FAIL
                elif result == EXIT:
                    return EXIT
        return SUCCESS
    except FileNotFoundError:
        logger.error(f"File not found: {file_name}")
        return FAIL
    except Exception as e:
        logger.error(f"Error: {e}")
        return FAIL

# Commands ...
cmds = []
cmds.append(("?", help_cmd, None))
cmds.append(("help", help_cmd, "Provide command help and syntax"))
cmds.append(("help", None, "?help [<topic>]"))
cmds.append(("activate", activate_cmd, "Activate HW, transitioning to operational state"))
cmds.append(("activate", None, "?activate (ocp | oprach | ...)"))
cmds.append(("clear", clear_cmd, "Clear various status, alarms, etc."))
cmds.append(("clear", None, "?clear (fhi_alarms | fhi_stats | ru_ports_table)"))
cmds.append(("clear", None, "?clear ocp_events"))
cmds.append(("clear", None, "?clear oprach_events"))
cmds.append(("configure", configure_cmd, "Program component carrier configuration"))
cmds.append(("configure", None, "?configure (fhi | bf) <cc>"))
cmds.append(("configure", None, "?configure <cc>"))
cmds.append(("deactivate", deactivate_cmd, "Deactivate HW, transitioning to initialized state"))
cmds.append(("deactivate", None, "?deactivate oprach"))
cmds.append(("debug", debug_cmd, "Set debugging level"))
cmds.append(("debug", None, "?debug (fhi | bf | ocp | ...) <level = 0..2>"))
cmds.append(("debug", None, "?debug <level = 0..2>"))
cmds.append(("devices", devices_cmd, "List devices accessible as Pyro proxies"))
cmds.append(("devices", None, "?devices"))
cmds.append(("disable", disable_cmd, "Disable component carrier"))
cmds.append(("disable", None, "?disable (fhi | bf) <cc>"))
cmds.append(("disable", None, "?disable <cc>"))
cmds.append(("dump", dump_cmd, "Dump debug information"))
cmds.append(("dump", None, "?dump (fhi | bf | ocp | ...)"))
cmds.append(("enable", enable_cmd, "Enable component carrier"))
cmds.append(("enable", None, "?enable (fhi | bf) <cc>"))
cmds.append(("enable", None, "?enable <cc>"))
cmds.append(("exit", exit_cmd, "Exit the client script / interactive session"))
cmds.append(("exit", None, "?exit"))
cmds.append(("finish", finish_cmd, "Close-down device driver libraries"))
cmds.append(("finish", None, "?finish (fhi | bf | ocp | ...)"))
cmds.append(("finish", None, "?finish"))
cmds.append(("get", get_cmd, "Get various configuration and status data from a device"))
cmds.append(("get", None, "?get (fhi_alarms | fhi_state | fhi_enabled)"))
cmds.append(("get", None, "?get (fhi_capabilities | fhi_caps)"))
cmds.append(("get", None, "?get (fhi_sw_version | fhi_hw_version | fhi_hw_internal_rev)"))
cmds.append(("get", None, "?get (ocp_capabilities | ocp_caps)"))
cmds.append(("get", None, "?get (ocp_events | ocp_state)"))
cmds.append(("get", None, "?get (ocp_sw_version | ocp_hw_version | ocp_hw_internal_rev)"))
cmds.append(("get", None, "?get (oprach_capabilities | oprach_caps)"))
cmds.append(("get", None, "?get (oprach_events | oprach_state)"))
cmds.append(("get", None, "?get (oprach_sw_version | oprach_hw_version)"))
cmds.append(("get", None, "?get fhi_cc_alloc <cc>"))
cmds.append(("get", None, "?get fhi_cc_config <cc>"))
cmds.append(("get", None, "?get fhi_stats <port>"))
cmds.append(("get", None, "?get ocp_antenna_cfg"))
cmds.append(("get", None, "?get ocp_cc_cfg <cc>"))
cmds.append(("get", None, "?get ocp_trigger_cfg"))
cmds.append(("get", None, "?get oprach_cc_cfg"))
cmds.append(("get", None, "?get oprach_rcid_cfg"))
cmds.append(("get", None, "?get oprach_trigger_cfg"))
cmds.append(("has", has_cmd, "Check for the presence of a device"))
cmds.append(("has", None, "?has (fhi | bf | ocp | ...)"))
cmds.append(("init", init_cmd, "Start-up device driver libraries"))
cmds.append(("init", None, "?init (fhi | bf | ocp | ...) [<device>]"))
cmds.append(("init", None, "?init"))
cmds.append(("magic", magic_cmd, None))
cmds.append(("monitor", monitor_cmd, "Configure / use monitor block"))
cmds.append(("monitor", None, "?monitor (fhi | ocp | ...) clear"))
cmds.append(("monitor", None, "?monitor (fhi | ocp | ...) read <counter>"))
cmds.append(("monitor", None, "?monitor (fhi | ocp | ...) select <stream>"))
cmds.append(("monitor", None, "?monitor (fhi | ocp | ...) snapshot"))
cmds.append(("quit", exit_cmd, None))
cmds.append(("read_reg", read_reg_cmd, "Read device registers"))
cmds.append(("read_reg", None, "?read_reg (fhi | bf | ocp | ...) <name>"))
cmds.append(("read_reg_offset", read_reg_offset_cmd, "Read device registers (with offsets)"))
cmds.append(("read_reg_offset", None, "?read_reg_offset (fhi | bf | ocp | ...) <name> <offset>"))
cmds.append(("reset", reset_cmd, "Reset devices"))
cmds.append(("reset", None, "?reset"))
cmds.append(("reset", None, "?reset (fhi | bf | ocp | ...) [<mode = 0|1>]"))
cmds.append(("run", run_cmd, "Run a command file"))
cmds.append(("run", None, "?run <file>"))
cmds.append(("set", set_cmd, "Set various configuration data for device"))
cmds.append(("set", None, "?set (dl_iq_compression | dl_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>"))
cmds.append(("set", None, "?set (per_ss_decompression | per_ss_decomp) <ss> <width> <method = 0..4> <enable = 0|1> [<number>]"))
cmds.append(("set", None, "?set (prach_iq_compression | prach_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>"))
cmds.append(("set", None, "?set (ssb_iq_compression | ssb_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>"))
cmds.append(("set", None, "?set (ul_iq_compression | ul_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>"))
cmds.append(("set", None, "?set dest_mac_addr <port> <address = XX:XX:XX:XX:XX:XX>"))
cmds.append(("set", None, "?set dl_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>"))
cmds.append(("set", None, "?set dl_timing_params <cc> <delay_comp_cp> <delay_comp_up> <advance>"))
cmds.append(("set", None, "?set eaxc_id <DU_bits> <BS_bits> <CC_bits> <RU_bits>"))
cmds.append(("set", None, "?set frames_per_sym <cc> <number_of_frames>"))
cmds.append(("set", None, "?set frames_per_sym_ssb <cc> <number_of_frames>"))
cmds.append(("set", None, "?set modu_dest_mac_addr <du> <address = XX:XX:XX:XX:XX:XX> [<id> <dei> <pcp>]"))
cmds.append(("set", None, "?set modu_mode <0 = disabled | 1 = enabled>"))
cmds.append(("set", None, "?set num_rbs <cc> <number_of_rbs>"))
cmds.append(("set", None, "?set numerology <cc> <numerology = 0..4> <extended_cp = 0|1>"))
cmds.append(("set", None, "?set numerology_ssb <cc> <numerology = 0..4> <extended_cp = 0|1>"))
cmds.append(("set", None, "?set ocp_antenna_cfg <num_antennas> <interleave> <data = (8 x 4b)>"))
cmds.append(("set", None, "?set ocp_cc_cfg <cc> <enable> <num_rbs> <numerology> <ccid> [<inter_sym_gap>]"))
cmds.append(("set", None, "?set ocp_schedule <mode> <length> {sequence}"))
cmds.append(("set", None, "?set ocp_trigger_cfg <enable> <mode> <edge_level> <bit> <enable> <mode> <edge_level> <bit>"))
cmds.append(("set", None, "?set oprach_cc_cfg {<enable> <sample_rate>}"))
cmds.append(("set", None, "?set oprach_rcid_cfg {<cc> <ss>}"))
cmds.append(("set", None, "?set oprach_trigger_cfg <enable> <mode> <edge_level> <bit> <enable> <mode> <edge_level> <bit>"))
cmds.append(("set", None, "?set packet_filter <port> <filter = (16 x 32b)> <mask = (4 x 16b)>"))
cmds.append(("set", None, "?set protocol <ECPRI | 1914.3> <VLAN = 0|1> <RAW | IPv4 | IPv6> # sets packet filter"))
cmds.append(("set", None, "?set protocol_alt <ECPRI | 1914.3> <VLAN = 0|1> <RAW | IPv4 | IPv6> # doesn't set packet filter"))
cmds.append(("set", None, "?set ru_ports <RU_bits> <SS_bits> <mask> <user_value> <prach_value> <ssb_value> [<lte_value>]"))
cmds.append(("set", None, "?set ru_ports_table <address> <port> <type> [<number>]"))
cmds.append(("set", None, "?set ru_ports_table_mode <mode>"))
cmds.append(("set", None, "?set src_mac_addr <port> <address = XX:XX:XX:XX:XX:XX>"))
cmds.append(("set", None, "?set ssb_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>"))
cmds.append(("set", None, "?set system_constants <fh_decap_dly> {...}"))
cmds.append(("set", None, "?set time_advance <cc> <deskew> <advance_uplink> <advance_downlink> # deprecated"))
cmds.append(("set", None, "?set ul_bid_forward <cc> <time>"))
cmds.append(("set", None, "?set ul_bid_forward_fine <cc> <symbols> <cycles>"))
cmds.append(("set", None, "?set ul_radio_ch_dly <cc> <delay> # deprecated"))
cmds.append(("set", None, "?set ul_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>"))
cmds.append(("set", None, "?set ul_timing_params <cc> <delay_comp> <advance> <radio_ch_delay>"))
cmds.append(("set", None, "?set vlan <port> <id> <dei> <pcp>"))
cmds.append(("terminate", terminate_cmd, "Terminate server")),
cmds.append(("terminate", None, "?terminate"))
cmds.append(("trigger", trigger_cmd, "Trigger HW update, e.g. causing update from 'current' to 'next'"))
cmds.append(("trigger", None, "?trigger (ocp | oprach | ...)"))
cmds.append(("version", version_cmd, "Display version of the application"))
cmds.append(("version", None, "?version"))
cmds.append(("wait", wait_cmd, "Wait for a short time"))
cmds.append(("wait", None, "?wait <seconds>"))
cmds.append(("write_reg", write_reg_cmd, "Write device registers"))
cmds.append(("write_reg", None, "?write_reg (fhi | bf | ocp | ...) <name> <value>"))
cmds.append(("write_reg_offset", write_reg_offset_cmd, "Write device registers (with offsets)"))
cmds.append(("write_reg_offset", None, "?write_reg_offset (fhi | bf | ocp | ...) <name> <offset> <value>"))

if __name__ == "__main__":
    # Configure command line argument parser
    parser = argparse.ArgumentParser(prog=PROG,
                                     description='Experimental Pyro O-RAN Radio Interface server')
    parser.add_argument('-V', '--Version', action='version',
                        version=f'{PROG} {VERSION}')
    parser.add_argument('-v', '--verbose', action='count', default=0,
                        help='set verbosity level, e.g. -v, -vv, -vvv')
    parser.add_argument('-H', '--host', type=str, default='localhost',
                        help='IP address (default is localhost)')
    parser.add_argument('-P', '--port', type=int, default=9090,
                        help='port number (default is 9090)')
    parser.add_argument('-I', '--interactive', action='store_true',
                        help='process commands in interactive mode')
    parser.add_argument('-f', '--file', type=str,
                        help='process commands from file')
    parser.add_argument('-c', '--command', metavar='CMD', type=str,
                        help='process command directly')

    # Parse command line arguments
    # Note, parse_args() exits on -h and -V or if an error occurs
    args = parser.parse_args()

    # Configure logging
    logging.basicConfig(format="%(name)s> %(levelname)s: %(message)s")
    if args.verbose >= 3:
        level = logging.DEBUG
    elif args.verbose == 2:
        level = logging.INFO
    elif args.verbose == 1:
        level = logging.INFO
    else:
        level = logging.ERROR
    logger.setLevel(level)

    # Mapping of 'nicknames' to formal class names
    targets = {
        "FHI": "XORIF",
        "BF": "XOBF",
        "OCP": "XOCP",
        "OPRACH": "XOPRACH",
    }

    # Get handles to possible target objects
    for nickname, name in targets.items():
        # See if the object is reachable on the Pyro server
        try:
            handle = Pyro4.Proxy(f"PYRO:{name}@{args.host}:{args.port}")
            handle._pyroBind()
        except Pyro4.errors.CommunicationError:
            pass
        except Exception as e:
             logger.error(f"Error: {e}")
        else:
            # If no error was raised, then we have a valid/active object
            handles[nickname] = handle

    # Populate and sort the list of available command
    commands += cmds
    commands.sort(key=lambda x: x[0])

    # Commands can be processed from multiple sources
    # We'll process them in the order: command line, file, interactive
    actions = []
    if args.command:
        actions.append((do_command, args.command))
    if args.file:
        actions.append((do_file, args.file))
    if args.interactive:
        actions.append((do_interactive, None))

    # Process the queued actions
    for func, args in actions:
        result = func(args)
        if result == FAIL:
            sys.exit(1)
        elif result == EXIT:
            sys.exit(0)
    sys.exit(0)