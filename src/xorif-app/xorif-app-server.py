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

PROG = 'xorif-app-server'
PROG_SHORT = "XORIF-APP-SERVER"
VERSION = '1.1'

logger = logging.getLogger(PROG_SHORT)

if __name__ == "__main__":
    # Configure command line argument parser
    parser = argparse.ArgumentParser(prog=PROG,
                                     description='Python O-RAN Radio Interface server')
    parser.add_argument('-V', '--Version', action='version',
                        version=f'{PROG} {VERSION}')
    parser.add_argument('-v', '--verbose', type=int, action='store', default=1,
                        help='set verbosity level (0 = quiet, 1 = normal, 2 = high)')
    parser.add_argument('-H', '--host', type=str, default='localhost',
                        help='IP address (default is localhost)')
    parser.add_argument('-P', '--port', type=int, default=9090,
                        help='port number (default is 9090)')
    parser.add_argument('--fhi', default=True, action='store_true',
                        help='with FHI support (default is yes)')
    parser.add_argument('--no-fhi', dest='fhi', action='store_false',
                        help='without FHI support')
    parser.add_argument('--ocp', default=True, action='store_true',
                        help='with OCP support (default is yes)')
    parser.add_argument('--no-ocp', dest='ocp', action='store_false',
                        help='without OCP support')
    parser.add_argument('--oprach', default=False, action='store_true',
                        help='with OPRACH support (default is no)')
    parser.add_argument('--no-oprach', dest='oprach', action='store_false',
                        help='without OPRACH support')

    # Parse command line arguments
    # Note, parse_args() exits on -h and -V or if an error occurs
    args = parser.parse_args()

    # Configure logging
    logging.basicConfig(format="%(name)s> %(levelname)s: %(message)s")
    if args.verbose >= 2:
        level = logging.DEBUG
    elif args.verbose == 1:
        level = logging.INFO
    else:
        level = logging.WARNING
    logger.setLevel(level)

    # Expose selected Pyro objects
    exposed = {}
    if args.fhi:
        try:
            from pylibxorif import LIBXORIF
            exposed[LIBXORIF] = "XORIF"
        except ModuleNotFoundError:
            logger.warning("Can not find pylibxorif.py")
    if args.ocp:
        try:
            from pylibxocp import LIBXOCP
            exposed[LIBXOCP] = "XOCP"
        except ModuleNotFoundError:
            logger.warning("Can not find pylibxocp.py")
    if args.oprach:
        try:
            from pylibxoprach import LIBXOPRACH
            exposed[LIBXOPRACH] = "XOPRACH"
        except ModuleNotFoundError:
            logger.warning("Can not find pylibxoprach.py")
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