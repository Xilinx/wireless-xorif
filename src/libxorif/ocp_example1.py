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
name = "XOCP"
host = "localhost"
port = 9090
debug = 2

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
const = namedtuple("xocp_const", handle.constants.keys())(*handle.constants.values())

# Initialize device
handle.xocp_debug(debug)
instance = handle.xocp_start()
assert instance >= 0
print(instance)

# Check versions
print(handle.xocp_get_sw_version())
print(handle.xocp_get_hw_version(instance))
print(handle.xocp_get_hw_internal_rev(instance))

# Get capabilities
_, caps = handle.xocp_get_capabilities(instance)
pprint(caps)

# Go to operational state
handle.xocp_reset(instance, 0)
handle.xocp_activate(instance)
assert handle.xocp_get_state(instance) == const.XOCP_OPERATIONAL

# Carrier configuration
# Using config: 1 CC (100MHz/30kHz) "e4x8000_fr1x8x8x1x1x100x0_dsta100x100" as basis
_, cc_config = handle.xocp_get_cc_cfg(instance, 0)
cc_config["enable"] = 1
cc_config["numerology"] = 1
cc_config["num_rbs"] = 273
cc_config["ccid"] = 0
cc_config["inter_sym_gap"] = 0 # Check this value
assert handle.xocp_set_cc_cfg(instance, 0, cc_config) == const.XOCP_SUCCESS

# Antenna configuration
_, ant_config = handle.xocp_get_antenna_cfg(instance)
ant_config["num_antennas"] = caps["max_num_antenna"]
ant_config["interleave"] = 2
ant_config["data"] = [x for x in range(caps["max_num_antenna"])]
assert handle.xocp_set_antenna_cfg(instance, ant_config) == const.XOCP_SUCCESS

# Schedule
sequence = [0] # Only 1 CC
handle.xocp_set_schedule(instance, 3, len(sequence), sequence) # 3 means UL + DL

# Trigger update
handle.xocp_trigger_update(instance)