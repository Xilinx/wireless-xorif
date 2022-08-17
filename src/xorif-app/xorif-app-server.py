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

import sys
import logging
import argparse
import Pyro4

sys.path.append('/usr/share/xorif')

# Import libraries if they exist...
try:
    from pylibxorif import LIBXORIF
except ModuleNotFoundError:
    LIBXORIF = None

try:
    from pylibxocp import LIBXOCP
except ModuleNotFoundError:
    LIBXOCP = None

try:
    from pylibxoprach import LIBXOPRACH
except ModuleNotFoundError:
    LIBXOPRACH = None

PROG = 'xorif-app-server'
PROG_SHORT = "XORIF-APP"
VERSION = '1.0'

if __name__ == "__main__":
    # Configure command line argument parser
    parser = argparse.ArgumentParser(prog=PROG,
                                     description='Experimental Pyro O-RAN Radio Interface server')
    parser.add_argument('-V', '--Version', action='version',
                        version=f'{PROG} {VERSION}')
    parser.add_argument('-H', '--host', type=str, default='localhost',
                        help='IP address (default is localhost)')
    parser.add_argument('-P', '--port', type=int, default=9090,
                        help='port number (default is 9090)')

    # Parse command line arguments
    # Note, parse_args() exits on -h and -V or if an error occurs
    args = parser.parse_args()

    # Expose Pyro objects (if they exist)
    exposed = {}
    if LIBXORIF:
        exposed[LIBXORIF] = "XORIF"
    if LIBXOCP:
        exposed[LIBXOCP] = "XOCP"
    if LIBXOPRACH:
        exposed[LIBXOPRACH] = "XOPRACH"
    for k in exposed:
        Pyro4.expose(k)

    # Start Pyro4 server with exposed objects
    try:
        # Start the Pyro server daemon
        Pyro4.Daemon.serveSimple(
                exposed,
                host=args.host,
                port=args.port,
                ns=False)
    except Exception as e:
        print(f'Error starting Pyro4 server: {e}')