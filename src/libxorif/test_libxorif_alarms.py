#!/usr/bin/env python3

import os
import pytest
import logging
import pytest
import pylibxorif
from collections import namedtuple
from cffi import FFI

lib = pylibxorif.LIBXORIF()
lib.set_log_level(logging.DEBUG)
lib.xorif_init()
const = namedtuple("xorif_const", lib.constants.keys())(*lib.constants.values())
caps = lib.xorif_get_capabilities()

# Open C library directly (to access hidden functions)
ffi = FFI()
ffi.cdef("int xorif_test_error_injections(uint32_t status);")
c_lib = ffi.dlopen("libxorif.so.1")

interrupts = [
    const.DEFRAMER_IN_FIFO_OF, const.DEFRAMER_IN_FIFO_UF, const.DEFRAMER_ETH_CIRC_BUFF_OF,
    const.DEFRAMER_ETH_CIRC_BUFF_PTR_OF, const.FRAMER_OUT_FIFO_OF, const.FRAMER_OUT_FIFO_UF,
    const.FRAMER_PRACH_SECTION_OF, const.FRAMER_PRACH_SECTION_NF, const.AXI_TIMEOUT,
]

METAL_IRQ_NOT_HANDLED = 0
METAL_IRQ_HANDLED = 1
test_status = 0 # global for testing status received via callback

def test_inject_errors_basic():
    assert lib.xorif_get_state() == 1

    # Clear alarms first
    lib.xorif_clear_fhi_alarms()
    assert lib.xorif_get_fhi_alarms() == 0

    # Inject interrupt with 0 status
    assert c_lib.xorif_test_error_injections(0) == METAL_IRQ_HANDLED
    assert lib.xorif_get_fhi_alarms() == 0

    bits = 0
    for i in range(1, 10):
        # Inject interrupt with specific status bits set
        for n in interrupts:
            bits |= n
            assert c_lib.xorif_test_error_injections(n) == METAL_IRQ_HANDLED
            assert lib.xorif_get_fhi_alarms() == bits

    # Clear alarms
    lib.xorif_clear_fhi_alarms()
    assert lib.xorif_get_fhi_alarms() == 0

def test_inject_errors_multiple():
    assert lib.xorif_get_state() == 1

    # Clear errors first
    lib.xorif_clear_fhi_alarms()

    # DEFRAMER_IN_FIFO_OF, DEFRAMER_ETH_CIRC_BUFF_OF
    bits = const.DEFRAMER_IN_FIFO_OF | const.DEFRAMER_ETH_CIRC_BUFF_OF
    assert c_lib.xorif_test_error_injections(bits) == METAL_IRQ_HANDLED
    assert lib.xorif_get_fhi_alarms() == bits

    # Add DEFRAMER_IN_FIFO_UF without clearing
    bits = const.DEFRAMER_IN_FIFO_OF | const.DEFRAMER_ETH_CIRC_BUFF_OF | const.DEFRAMER_IN_FIFO_UF
    assert c_lib.xorif_test_error_injections(const.DEFRAMER_IN_FIFO_UF) == METAL_IRQ_HANDLED
    assert lib.xorif_get_fhi_alarms() == bits

@ffi.callback("void (*)(uint32_t)")
def callback(status):
    # Set a global variable with the received status
    global test_status
    test_status = status
    print(f'ORIF ISR callback status = {status:08x}')

def test_inject_errors_callback():
    assert lib.xorif_get_state() == 1
    global test_status

    # Register call-back
    assert lib.xorif_register_fhi_isr(callback) == const.XORIF_SUCCESS

    # Clear errors first
    lib.xorif_clear_fhi_alarms()

    # DEFRAMER_IN_FIFO_OF, DEFRAMER_ETH_CIRC_BUFF_OF
    test_status = 0
    bits = const.DEFRAMER_IN_FIFO_OF | const.DEFRAMER_ETH_CIRC_BUFF_OF
    assert c_lib.xorif_test_error_injections(bits) == METAL_IRQ_HANDLED
    assert lib.xorif_get_fhi_alarms() == bits
    assert test_status == bits