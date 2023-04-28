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
import Pyro4

sys.path.append('/usr/share/xorif')

objects = {}
try:
    from pylibxorif import LIBXORIF
except:
    print("Warning: pylibxorif not loaded")
else:
    objects[LIBXORIF] = "XORIF"

try:
    from pylibxocp import LIBXOCP
except:
    print("Warning: pylibxocp not loaded")
else:
    objects[LIBXOCP] = "XOCP"

# Basic configuration
host = "localhost"
port = 9090

# Expose Pyro objects
for o in objects:
    Pyro4.expose(o)

Pyro4.Daemon.serveSimple(
    objects,
    host=host,
    port=port,
    ns=False,
    verbose=True,
)