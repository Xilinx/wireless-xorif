#!/usr/bin/env python3
#
# Copyright 2020 - 2023 Advanced Micro Devices, Inc.
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
__copyright__ = "Copyright 2022, Advanced Micro Devices, Inc."

import sys
import logging
from pprint import pprint
from collections import namedtuple
import Pyro4

# Basic configuration
name = "XORIF"
host = "localhost"
port = 9090
debug = 2

# Device (fully qualified name, partial name or None)
# Note, using None will cause automatic device name search
#device = "oran_radio_if"
device = None

# Configure logging
logger = logging.getLogger(f"{name}")
logging.basicConfig(format="%(name)s> %(levelname)s: %(message)s")
if debug >= 2:
    level = logging.DEBUG
elif debug == 1:
    level = logging.INFO
else:
    level = logging.WARNING
logger.setLevel(level)

# Connect Pyro client to Pyro server
try:
    handle = Pyro4.Proxy(f"PYRO:{name}@{host}:{port}")
    handle._pyroBind()
except Pyro4.errors.CommunicationError:
    logger.error("Can not connect to Pyro server")
    sys.exit(1)
except Exception as e:
    logger.error(f"Error: {e}")
    sys.exit(1)
else:
    logger.info(handle)

# Get constants as named tuples
const = namedtuple("xorif_const", handle.constants.keys())(*handle.constants.values())

# Initialize device
handle.xorif_debug(debug)
assert handle.xorif_init(device) == const.XORIF_SUCCESS

# Check versions
print(handle.xorif_get_sw_version())
print(handle.xorif_get_fhi_hw_version())
print(handle.xorif_get_fhi_hw_internal_rev())

# Get capabilities
caps = handle.xorif_get_capabilities()
pprint(caps)

# Configuration
# Using config: 1 CC (100MHz/30kHz) "e4x8000_fr1x8x8x1x1x100x0_dsta100x100" as basis

# Configure transport and eAxC mapping
handle.xorif_reset_fhi(1)
handle.xorif_set_fhi_protocol(const.PROTOCOL_ECPRI, 0, const.IP_MODE_RAW)
handle.xorif_set_fhi_eaxc_id(4, 1, 3, 8)
handle.xorif_set_ru_ports(8, 5, 0xC0, 0x00, 0x80, 0x40)
handle.xorif_reset_fhi(0)

# Configure system constants
sys_const = {"FH_DECAP_DLY": 7}
handle.xorif_set_system_constants(sys_const)

# Component carrier configuration
_, cc_config = handle.xorif_get_cc_config(0)
cc_config["numerology"] = 1
cc_config["num_rbs"] = 273
cc_config["iq_comp_meth_ul"] = const.IQ_COMP_NONE
cc_config["iq_comp_width_ul"] = 0
cc_config["iq_comp_mplane_ul"] = 0
cc_config["iq_comp_meth_dl"] = const.IQ_COMP_NONE
cc_config["iq_comp_width_dl"] = 0
cc_config["iq_comp_mplane_dl"] = 0
cc_config["num_frames_per_sym"] = 15
cc_config["num_sect_per_sym"] = 1
cc_config["num_ctrl_per_sym_dl"] = 16
cc_config["num_ctrl_per_sym_ul"] = 16
cc_config["delay_comp_cp_dl"] = 100
cc_config["delay_comp_up"] = 100
cc_config["advance_dl"] = 100
cc_config["delay_comp_cp_ul"] = 100
cc_config["advance_ul"] = 100
cc_config["ul_radio_ch_dly"] = 30
cc_config["ul_bid_forward"] = 40
assert handle.xorif_set_cc_config(0, cc_config) == const.XORIF_SUCCESS

# Configure and enable component carrier
handle.xorif_configure_cc(0)
handle.xorif_enable_cc(0)