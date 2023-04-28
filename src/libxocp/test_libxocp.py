#!/usr/bin/env python3

import sys
import logging
from collections import namedtuple
from cffi import FFI
import pytest

sys.path.append('/usr/share/xorif')
import pylibxocp

lib = pylibxocp.LIBXOCP()
lib.set_log_level(logging.DEBUG)
const = namedtuple("xocp_const", lib.constants.keys())(*lib.constants.values())

# Open C library directly (to access hidden functions)
ffi = FFI()
ffi.cdef("int xocp_test_error_injections(uint16_t instance, uint32_t status);")
c_lib = ffi.dlopen("libxocp.so.1")


def go_to_operational():
    instance = lib.xocp_start()
    assert lib.xocp_reset(instance, 0) == const.XOCP_SUCCESS
    assert lib.xocp_get_state(instance) == const.XOCP_READY
    assert lib.xocp_activate(instance) == const.XOCP_SUCCESS
    assert lib.xocp_get_state(instance) == const.XOCP_OPERATIONAL
    return instance


def test_xocp_debug():
    lib.xocp_debug(0)
    lib.xocp_debug(2)


def test_xocp_start():
    instance1 = lib.xocp_start()
    assert instance1 >= 0

    instance1 = lib.xocp_start("o_pxxch")
    assert instance1 >= 0

    instance2 = lib.xocp_start()
    assert instance1 == instance2

    instance3 = lib.xocp_start("XYZ")
    assert instance3 == const.XOCP_NO_SUCH_DEVICE


def test_xocp_finish():
    instance = lib.xocp_start()
    lib.xocp_finish(instance)


def test_xocp_get_state():
    instance = lib.xocp_start()
    assert lib.xocp_get_state(instance) == const.XOCP_RESET
    assert lib.xocp_reset(instance, 0) == const.XOCP_SUCCESS
    assert lib.xocp_get_state(instance) == const.XOCP_READY
    assert lib.xocp_activate(instance) == const.XOCP_SUCCESS
    assert lib.xocp_get_state(instance) == const.XOCP_OPERATIONAL


def test_xocp_get_sw_version():
    instance = lib.xocp_start()
    version = lib.xocp_get_sw_version()
    print(version)


def test_xocp_get_hw_version():
    instance = lib.xocp_start()
    version = lib.xocp_get_hw_version(instance)
    print(version)


def test_xocp_get_hw_internal_rev():
    instance = lib.xocp_start()
    revision = lib.xocp_get_hw_internal_rev(instance)
    print(revision)


def test_xocp_get_capabilities():
    instance = lib.xocp_start()
    result, caps = lib.xocp_get_capabilities(instance)
    assert result == const.XOCP_SUCCESS
    print(caps)


def test_xocp_get_event_status():
    instance = lib.xocp_start()
    result, status = lib.xocp_get_event_status(instance)
    assert result == const.XOCP_SUCCESS
    print(status)


def test_xocp_clear_event_status():
    instance = lib.xocp_start()
    assert lib.xocp_clear_event_status(instance) == const.XOCP_SUCCESS


def test_xocp_enable_interrupts():
    instance = lib.xocp_start()
    assert lib.xocp_enable_interrupts(instance, 0xFFFF) == const.XOCP_SUCCESS
    assert lib.xocp_enable_interrupts(instance, 0x0000) == const.XOCP_SUCCESS


def test_xocp_read_reg():
    instance = lib.xocp_start()
    result, value = lib.xocp_read_reg(instance, "CFG_MAJOR_REVISION")
    assert result == const.XOCP_SUCCESS
    print(value)

    result, value = lib.xocp_read_reg(instance, "0x0000")
    assert result == const.XOCP_SUCCESS
    print(value)

    result, value = lib.xocp_read_reg(instance, "XYZ")
    assert result == const.XOCP_REGISTER_NOT_FOUND


def test_xocp_read_reg_offset():
    instance = lib.xocp_start()
    result, value = lib.xocp_read_reg_offset(instance, "CFG_MAJOR_REVISION", 0)
    assert result == const.XOCP_SUCCESS
    print(value)

    result, value = lib.xocp_read_reg_offset(instance, "0x0000", 0)
    assert result == const.XOCP_SUCCESS
    print(value)

    result, value = lib.xocp_read_reg_offset(instance, "XYZ", 0)
    assert result == const.XOCP_REGISTER_NOT_FOUND


def test_xocp_write_reg():
    instance = lib.xocp_start()
    assert lib.xocp_write_reg(instance, "CTRL_DL_NUMBER_OF_ANTENNAS", 0) == const.XOCP_SUCCESS

    assert lib.xocp_write_reg(instance, "0x4004", 0) == const.XOCP_SUCCESS

    assert lib.xocp_write_reg(instance, "XYZ", 0) == const.XOCP_REGISTER_NOT_FOUND


def test_xocp_write_reg_offset():
    instance = lib.xocp_start()
    assert lib.xocp_write_reg_offset(instance, "CTRL_DL_NUMBER_OF_ANTENNAS", 0, 0) == const.XOCP_SUCCESS

    assert lib.xocp_write_reg_offset(instance, "0x4004", 0, 0) == const.XOCP_SUCCESS

    assert lib.xocp_write_reg_offset(instance, "XYZ", 0, 0) == const.XOCP_REGISTER_NOT_FOUND


def test_xocp_reset():
    instance = lib.xocp_start()
    assert lib.xocp_reset(instance, 1) == const.XOCP_SUCCESS
    assert lib.xocp_get_state(instance) == const.XOCP_RESET
    assert lib.xocp_reset(instance, 0) == const.XOCP_SUCCESS
    assert lib.xocp_get_state(instance) == const.XOCP_READY


def test_xocp_activate():
    instance = lib.xocp_start()
    assert lib.xocp_reset(instance, 0) == const.XOCP_SUCCESS
    assert lib.xocp_get_state(instance) == const.XOCP_READY
    assert lib.xocp_activate(instance) == const.XOCP_SUCCESS
    assert lib.xocp_get_state(instance) == const.XOCP_OPERATIONAL


def test_xocp_get_cc_cfg():
    instance = go_to_operational()
    result, cc_cfg = lib.xocp_get_cc_cfg(instance, 0)
    assert result == const.XOCP_SUCCESS
    print(cc_cfg)

    result, cc_cfg = lib.xocp_get_cc_cfg(instance, const.XOCP_MAX_NUM_CC)
    assert result == const.XOCP_INVALID_CC


def test_xocp_set_cc_cfg():
    instance = go_to_operational()
    result, cc_cfg = lib.xocp_get_cc_cfg(instance, 0)
    assert result == const.XOCP_SUCCESS
    print(cc_cfg)

    # modify config
    cc_cfg["num_rbs"] = 273
    cc_cfg["numerology"] = 1
    cc_cfg["ccid"] = 2
    cc_cfg["inter_sym_gap"] = 123
    assert lib.xocp_set_cc_cfg(instance, 0, cc_cfg) == const.XOCP_SUCCESS

    # check modifications
    result, cc_cfg = lib.xocp_get_cc_cfg(instance, 0)
    assert result == const.XOCP_SUCCESS
    print(cc_cfg)
    assert cc_cfg["num_rbs"] == 273
    assert cc_cfg["numerology"] == 1
    assert cc_cfg["ccid"] == 2
    assert cc_cfg["inter_sym_gap"] == 123


def test_xocp_get_antenna_cfg():
    instance = go_to_operational()
    result, ant_cfg = lib.xocp_get_antenna_cfg(instance)
    assert result == const.XOCP_SUCCESS
    print(ant_cfg)


def test_xocp_set_antenna_cfg():
    instance = go_to_operational()
    result, ant_cfg = lib.xocp_get_antenna_cfg(instance)
    assert result == const.XOCP_SUCCESS
    print(ant_cfg)
    result, caps = lib.xocp_get_capabilities(instance)
    assert result == const.XOCP_SUCCESS

    # modify
    ant_cfg["num_antennas"] = caps["max_num_antenna"]
    ant_cfg["interleave"] = 2
    ant_cfg["data"] = [x for x in range(caps["max_num_antenna"])]
    assert lib.xocp_set_antenna_cfg(instance, ant_cfg) == const.XOCP_SUCCESS

    # check modifications
    result, ant_cfg = lib.xocp_get_antenna_cfg(instance)
    assert result == const.XOCP_SUCCESS
    print(ant_cfg)
    assert ant_cfg["num_antennas"] == caps["max_num_antenna"]
    assert ant_cfg["interleave"] == 2
    assert ant_cfg["data"] == [x if x < caps["max_num_antenna"] else 0 for x in range(8)]


def test_xocp_get_trigger_cfg():
    instance = go_to_operational()
    result, triggers = lib.xocp_get_trigger_cfg(instance)
    assert result == const.XOCP_SUCCESS
    print(triggers)


def test_xocp_set_trigger_cfg():
    instance = go_to_operational()
    result, triggers = lib.xocp_get_trigger_cfg(instance)
    assert result == const.XOCP_SUCCESS
    print(triggers)

    # modify config
    triggers["dl_update"]["mode"] = 1
    triggers["dl_update"]["tuser_edge_level"] = 2
    triggers["dl_update"]["tuser_bit"] = 3
    triggers["ul_update"]["mode"] = 1
    triggers["ul_update"]["tuser_edge_level"] = 3
    triggers["ul_update"]["tuser_bit"] = 4
    assert lib.xocp_set_trigger_cfg(instance, triggers) == const.XOCP_SUCCESS

    # check modifications
    result, triggers = lib.xocp_get_trigger_cfg(instance)
    assert result == const.XOCP_SUCCESS
    print(triggers)


def test_xocp_trigger_update():
    instance = go_to_operational()
    assert lib.xocp_trigger_update(instance) == const.XOCP_SUCCESS


def test_xocp_set_schedule0():
    instance = go_to_operational()
    result, cc_cfg = lib.xocp_get_cc_cfg(instance, 0)
    assert result == const.XOCP_SUCCESS

    # schedule (DL & UL) - zero length table
    sequence = []
    assert lib.xocp_set_schedule(instance, 3, len(sequence), sequence) == const.XOCP_SUCCESS


def test_xocp_set_schedule1():
    instance = go_to_operational()
    result, cc_cfg = lib.xocp_get_cc_cfg(instance, 0)
    assert result == const.XOCP_SUCCESS
    result, caps = lib.xocp_get_capabilities(instance)
    assert result == const.XOCP_SUCCESS

    # CC[0], 273 RBs, numerology 0
    cc_cfg["num_rbs"] = 273
    cc_cfg["numerology"] = 0
    cc_cfg["ccid"] = 0
    cc_cfg["inter_sym_gap"] = 123
    assert lib.xocp_set_cc_cfg(instance, 0, cc_cfg) == const.XOCP_SUCCESS

    # set antenna config
    ant_cfg = {}
    ant_cfg["num_antennas"] = caps["max_num_antenna"]
    ant_cfg["interleave"] = 2
    ant_cfg["data"] = [x for x in range(caps["max_num_antenna"])]
    assert lib.xocp_set_antenna_cfg(instance, ant_cfg) == const.XOCP_SUCCESS

    # schedule (DL & UL)
    sequence = [0]
    assert lib.xocp_set_schedule(instance, 3, len(sequence), sequence) == const.XOCP_SUCCESS


def test_xocp_set_schedule2():
    instance = go_to_operational()
    result, cc_cfg = lib.xocp_get_cc_cfg(instance, 0)
    assert result == const.XOCP_SUCCESS
    result, caps = lib.xocp_get_capabilities(instance)
    assert result == const.XOCP_SUCCESS

    # CC[0], 273 RBs, numerology 0
    cc_cfg["num_rbs"] = 273
    cc_cfg["numerology"] = 0
    cc_cfg["ccid"] = 0
    cc_cfg["inter_sym_gap"] = 123
    assert lib.xocp_set_cc_cfg(instance, 0, cc_cfg) == const.XOCP_SUCCESS

    # set antenna config
    ant_cfg = {}
    ant_cfg["num_antennas"] = caps["max_num_antenna"]
    ant_cfg["interleave"] = 2
    ant_cfg["data"] = [x for x in range(caps["max_num_antenna"])]
    assert lib.xocp_set_antenna_cfg(instance, ant_cfg) == const.XOCP_SUCCESS

    # schedule (DL & UL)
    sequence = [0]
    assert lib.xocp_set_schedule(instance, 3, len(sequence), sequence) == const.XOCP_SUCCESS


def test_xocp_set_schedule3():
    instance = go_to_operational()
    result, cc_cfg = lib.xocp_get_cc_cfg(instance, 0)
    assert result == const.XOCP_SUCCESS
    result, caps = lib.xocp_get_capabilities(instance)
    assert result == const.XOCP_SUCCESS

    # CC[0], 273 RBs, numerology 0
    cc_cfg["num_rbs"] = 273
    cc_cfg["numerology"] = 0
    cc_cfg["ccid"] = 0
    cc_cfg["inter_sym_gap"] = 123
    assert lib.xocp_set_cc_cfg(instance, 0, cc_cfg) == const.XOCP_SUCCESS

    # set antenna config
    ant_cfg = {}
    ant_cfg["num_antennas"] = caps["max_num_antenna"]
    ant_cfg["interleave"] = 2
    ant_cfg["data"] = [x for x in range(caps["max_num_antenna"])]
    assert lib.xocp_set_antenna_cfg(instance, ant_cfg) == const.XOCP_SUCCESS

    # schedule (DL & UL)
    sequence = [0]
    assert lib.xocp_set_schedule(instance, 3, len(sequence), sequence) == const.XOCP_SUCCESS


def test_xocp_set_schedule4():
    instance = go_to_operational()
    result, cc_cfg = lib.xocp_get_cc_cfg(instance, 0)
    assert result == const.XOCP_SUCCESS
    result, caps = lib.xocp_get_capabilities(instance)
    assert result == const.XOCP_SUCCESS

    # CC[0], 273 RBs, numerology 0
    cc_cfg["num_rbs"] = 273
    cc_cfg["numerology"] = 0
    cc_cfg["ccid"] = 0
    cc_cfg["inter_sym_gap"] = 123
    assert lib.xocp_set_cc_cfg(instance, 0, cc_cfg) == const.XOCP_SUCCESS

    # set antenna config
    ant_cfg = {}
    ant_cfg["num_antennas"] = caps["max_num_antenna"]
    ant_cfg["interleave"] = 2
    ant_cfg["data"] = [x for x in range(caps["max_num_antenna"])]
    assert lib.xocp_set_antenna_cfg(instance, ant_cfg) == const.XOCP_SUCCESS

    # schedule (DL & UL)
    sequence = [0]
    assert lib.xocp_set_schedule(instance, 3, len(sequence), sequence) == const.XOCP_SUCCESS


def test_xocp_set_schedule5():
    instance = go_to_operational()
    result, cc_cfg = lib.xocp_get_cc_cfg(instance, 0)
    assert result == const.XOCP_SUCCESS
    result, caps = lib.xocp_get_capabilities(instance)
    assert result == const.XOCP_SUCCESS

    # CC[0], 273 RBs, numerology 0
    cc_cfg["num_rbs"] = 273
    cc_cfg["numerology"] = 0
    cc_cfg["ccid"] = 0
    cc_cfg["inter_sym_gap"] = 123
    assert lib.xocp_set_cc_cfg(instance, 0, cc_cfg) == const.XOCP_SUCCESS

    # CC[1], 130 RBs, numerology 1
    cc_cfg["num_rbs"] = 130
    cc_cfg["numerology"] = 1
    cc_cfg["ccid"] = 1
    cc_cfg["inter_sym_gap"] = 123
    assert lib.xocp_set_cc_cfg(instance, 1, cc_cfg) == const.XOCP_SUCCESS

    # set antenna config
    ant_cfg = {}
    ant_cfg["num_antennas"] = caps["max_num_antenna"]
    ant_cfg["interleave"] = 2
    ant_cfg["data"] = [x for x in range(caps["max_num_antenna"])]
    assert lib.xocp_set_antenna_cfg(instance, ant_cfg) == const.XOCP_SUCCESS

    # schedule (DL & UL)
    sequence = [1, 1, 0]
    assert lib.xocp_set_schedule(instance, 3, len(sequence), sequence) == const.XOCP_SUCCESS


def test_xocp_set_schedule6():
    instance = go_to_operational()
    result, cc_cfg = lib.xocp_get_cc_cfg(instance, 0)
    assert result == const.XOCP_SUCCESS
    result, caps = lib.xocp_get_capabilities(instance)
    assert result == const.XOCP_SUCCESS

    # CC[0], 273 RBs, numerology 0
    cc_cfg["num_rbs"] = 273
    cc_cfg["numerology"] = 0
    cc_cfg["ccid"] = 0
    cc_cfg["inter_sym_gap"] = 123
    assert lib.xocp_set_cc_cfg(instance, 0, cc_cfg) == const.XOCP_SUCCESS

    # CC[1], 130 RBs, numerology 1
    cc_cfg["num_rbs"] = 130
    cc_cfg["numerology"] = 1
    cc_cfg["ccid"] = 1
    cc_cfg["inter_sym_gap"] = 999
    assert lib.xocp_set_cc_cfg(instance, 1, cc_cfg) == const.XOCP_SUCCESS

    # set antenna config
    ant_cfg = {}
    ant_cfg["num_antennas"] = caps["max_num_antenna"]
    ant_cfg["interleave"] = 2
    ant_cfg["data"] = [x for x in range(caps["max_num_antenna"])]
    assert lib.xocp_set_antenna_cfg(instance, ant_cfg) == const.XOCP_SUCCESS

    # schedule (DL & UL)
    sequence = [1, 0, 1]
    assert lib.xocp_set_schedule(instance, 3, len(sequence), sequence) == const.XOCP_SUCCESS


def test_xocp_monitor_clear():
    instance = go_to_operational()
    assert lib.xocp_monitor_clear(instance) == const.XOCP_SUCCESS


def test_xocp_monitor_read():
    instance = go_to_operational()
    assert lib.xocp_monitor_clear(instance) == const.XOCP_SUCCESS
    assert lib.xocp_monitor_snapshot(instance) == const.XOCP_SUCCESS
    result, value = lib.xocp_monitor_read(instance, 0)
    assert result == const.XOCP_SUCCESS
    print(value)

    # Writing to registers, but this only works with "fake reg-map"
    if "NO_HW" in lib.constants:
        assert lib.xocp_write_reg(instance, "CFG_MONITOR_READ_31__0", 0x89ABCDEF) == const.XOCP_SUCCESS
        assert lib.xocp_write_reg(instance, "CFG_MONITOR_READ_63_32", 0x01234567) == const.XOCP_SUCCESS
        assert lib.xocp_monitor_snapshot(instance) == const.XOCP_SUCCESS
        result, value = lib.xocp_monitor_read(instance, 0)
        assert result == const.XOCP_SUCCESS
        assert value == 0x0123456789ABCDEF


METAL_IRQ_NOT_HANDLED = 0
METAL_IRQ_HANDLED = 1
test_status = 0  # global for testing status received via callback
interrupts = [
    const.XOCP_DL_CC_UPDATE_TRIGGERED, const.XOCP_DL_SEQUENCE_ERROR,
    const.XOCP_DL_SEQUENCE_TABLE_ERROR, const.XOCP_UL_CC_UPDATE_TRIGGERED,
    const.XOCP_UL_SEQUENCE_ERROR, const.XOCP_UL_SEQUENCE_TABLE_ERROR,
]


@pytest.mark.skipif("EXTRA_DEBUG" not in lib.constants, reason="No test code to inject errors")
def test_inject_errors_basic():
    instance = go_to_operational()

    # Clear errors
    assert lib.xocp_clear_event_status(instance) == const.XOCP_SUCCESS
    result, status = lib.xocp_get_event_status(instance)
    assert result == const.XOCP_SUCCESS

    # Inject interrupt with 0 status
    assert c_lib.xocp_test_error_injections(instance, 0) == METAL_IRQ_HANDLED
    result, status = lib.xocp_get_event_status(instance)
    assert result == const.XOCP_SUCCESS
    #assert status == 0

    bits = 0
    for i in range(1, 10):
        # Inject interrupt with specific status bits set
        for n in interrupts:
            bits |= n
            assert c_lib.xocp_test_error_injections(instance, n) == METAL_IRQ_HANDLED
            result, status = lib.xocp_get_event_status(instance)
            assert result == const.XOCP_SUCCESS
            #assert status == bits

    # Clear errors
    lib.xocp_clear_event_status(instance)
    result, status = lib.xocp_get_event_status(instance)
    assert result == const.XOCP_SUCCESS
    #assert status == 0


@ffi.callback("void (*)(uint16_t, uint32_t)")
def callback(instance, status):
    # Set a global variable with the received status
    global test_status
    test_status = status
    print(f'OCP ISR callback status = {instance}, {status:08x}')


@pytest.mark.skipif("EXTRA_DEBUG" not in lib.constants, reason="No test code to inject errors")
def test_inject_errors_callback():
    instance = go_to_operational()
    global test_status

    # Register call-back
    assert lib.xocp_register_event_callback(instance, callback) == const.XOCP_SUCCESS

    # Clear errors first
    assert lib.xocp_clear_event_status(instance) == const.XOCP_SUCCESS

    # XOCP_DL_CC_UPDATE_TRIGGERED, XOCP_UL_SEQUENCE_TABLE_ERROR
    test_status = 0
    bits = const.XOCP_DL_CC_UPDATE_TRIGGERED | const.XOCP_UL_SEQUENCE_TABLE_ERROR
    assert c_lib.xocp_test_error_injections(instance, bits) == METAL_IRQ_HANDLED
    result, status = lib.xocp_get_event_status(instance)
    assert result == const.XOCP_SUCCESS
    assert status == bits
    assert test_status == bits
