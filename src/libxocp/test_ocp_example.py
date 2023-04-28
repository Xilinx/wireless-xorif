#!/usr/bin/env python3

import subprocess
import time

def test_example1():
    # Start server
    proc = subprocess.Popen("./server_example.py")
    time.sleep(5)

    try:
        # Run the "example1" script
        test = subprocess.Popen("./ocp_example1.py")
        test.wait()
    finally:
        # Kill server
        proc.kill()

    assert test.returncode == 0