#!/usr/bin/env python3

import os
import mmap
import ctypes
import re
import pytest

# Configure required tests here
RO = True
RW = True
WO = True


def get_device_address(device):
    """
    Get the base address for the given device.
    Searches /sys/bus/platform/devices/ for a device that partially
    matches the given 'device' parameter, e.g. a00000000.device_xyz.
    The leading part is the base address in hex, which is converted
    and returned.
    Raises an exception if no matching device found.
    """
    directory = "/sys/bus/platform/devices/"
    for filename in os.listdir(directory):
        if device in filename:
            m = re.match("([0-9A-Fa-f]+).", filename)
            if m:
                return int("0x" + m.group(1), 0)
    raise Exception("No compatible device found")


def peek(offset):
    '''Use mmap to read the 32-bit value of the specified address.'''
    # regs.seek(offset)
    #val = regs.read(4)
    # return int(struct.unpack("@I", val)[0])
    # Using ctypes function here to cause 32-bit read rather than 4-bytes
    return ctypes.c_uint32.from_buffer(regs, offset).value


def poke(offset, value):
    '''Use mmap to write the 32-bit value to the specified address.'''
    # regs.seek(offset)
    #regs.write(struct.pack("@I", value))
    # Using ctypes function here to cause 32-bit write rather than 4-bytes
    ctypes.c_uint32.from_buffer(regs, offset).value = value


# Get the base-address for the device
try:
    base_addr = get_device_address("oran_radio_if")
except:
    # Skip all tests if no device
    pytestmark = pytest.mark.skip
else:
    # Set up mmap to access registers
    devmem = os.open('/dev/mem', os.O_RDWR | os.O_SYNC)
    regs = mmap.mmap(devmem,
                     0x20000,
                     mmap.MAP_SHARED,
                     mmap.PROT_READ | mmap.PROT_WRITE,
                     offset=base_addr)
    os.close(devmem)

    # Set the following variables according to device
    C_MAX_COMP_CARR_OCP = peek(0x10008) & 0xf
    ADDR_TABLE_ENTRIES = 32


def read_reg(addr, mask, offset):
    '''Read from register, given the address, mask and bit offset.'''
    return (peek(addr) & mask) >> offset


def write_reg(addr, mask, offset, value):
    '''Write value to a register, given the address, mask and bit offset.'''
    poke(addr, (peek(addr) & ~mask) | ((value << offset) & mask))


def do_rw_reg_value(addr, mask, offset, value):
    '''Perform a register read/write test with specified value.'''
    write_reg(addr, mask, offset, value)
    assert value == read_reg(addr, mask, offset)


def do_rw_reg(name, addr, mask, offset, width):
    '''Perform selected read/write tests for register.'''
    # save original value
    value = read_reg(addr, mask, offset)
    value_mask = mask >> offset
    if True:
        # zero-one r/w test
        do_rw_reg_value(addr, mask, offset, value_mask & 0x00000000)
        do_rw_reg_value(addr, mask, offset, value_mask & 0xFFFFFFFF)
    if True and width > 1:
        # checker-board r/w test
        do_rw_reg_value(addr, mask, offset, value_mask & 0x55555555)
        do_rw_reg_value(addr, mask, offset, value_mask & 0xAAAAAAAA)
    if True and width > 2:
        # walking pattern r/w test
        for i in range(0, width):
            do_rw_reg_value(addr, mask, offset, value_mask & (1 << i))
            do_rw_reg_value(addr, mask, offset, value_mask & ~(1 << i))
    # restore original value
    do_rw_reg_value(addr, mask, offset, value_mask & value)


def do_ro_reg(name, addr, mask, offset, width):
    '''Perform a register read-only test.'''
    value1 = read_reg(addr, mask, offset)
    write_reg(addr, mask, offset, ~value1)
    value2 = read_reg(addr, mask, offset)
    assert value1 == value2


def do_wo_reg(name, addr, mask, offset, width):
    '''Perform selected write-only tests for register.'''
    value_mask = mask >> offset
    if True:
        # zero-one r/w test
        write_reg(addr, mask, offset, value_mask & 0x00000000)
        write_reg(addr, mask, offset, value_mask & 0xFFFFFFFF)
    if True and width > 1:
        # checker-board r/w test
        write_reg(addr, mask, offset, value_mask & 0x55555555)
        write_reg(addr, mask, offset, value_mask & 0xAAAAAAAA)
    if True and width > 2:
        # walking pattern r/w test
        for i in range(0, width):
            write_reg(addr, mask, offset, value_mask & (1 << i))
            write_reg(addr, mask, offset, value_mask & ~(1 << i))


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CFG_MAX_NUM_CC():
    do_ro_reg("OPXXCH_CFG_MAX_NUM_CC", 0x10008, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CFG_MAX_NUM_ANTENNA():
    do_ro_reg("OPXXCH_CFG_MAX_NUM_ANTENNA", 0x10008, 0xf00, 8, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CFG_MAX_ANTENNA_INTERLEAVE():
    do_ro_reg("OPXXCH_CFG_MAX_ANTENNA_INTERLEAVE", 0x10008, 0x70000, 16, 3)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CFG_CHAN_PROC_MASTER_INT_ENABLE():
    do_rw_reg("OPXXCH_CFG_CHAN_PROC_MASTER_INT_ENABLE", 0x10010, 0x1, 0, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_EN():
    do_rw_reg("OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_EN", 0x10014, 0x1, 0, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CFG_DL_SEQUENCE_ERROR_EN():
    do_rw_reg("OPXXCH_CFG_DL_SEQUENCE_ERROR_EN", 0x10014, 0x2, 1, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_EN():
    do_rw_reg("OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_EN", 0x10014, 0x4, 2, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_EN():
    do_rw_reg("OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_EN", 0x10014, 0x100, 8, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CFG_UL_SEQUENCE_ERROR_EN():
    do_rw_reg("OPXXCH_CFG_UL_SEQUENCE_ERROR_EN", 0x10014, 0x200, 9, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_EN():
    do_rw_reg("OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_EN", 0x10014, 0x400, 10, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED():
    do_ro_reg("OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED", 0x10018, 0x1, 0, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CFG_DL_SEQUENCE_ERROR():
    do_ro_reg("OPXXCH_CFG_DL_SEQUENCE_ERROR", 0x10018, 0x2, 1, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR():
    do_ro_reg("OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR", 0x10018, 0x4, 2, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED():
    do_ro_reg("OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED", 0x10018, 0x100, 8, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CFG_UL_SEQUENCE_ERROR():
    do_ro_reg("OPXXCH_CFG_UL_SEQUENCE_ERROR", 0x10018, 0x200, 9, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR():
    do_ro_reg("OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR", 0x10018, 0x400, 10, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CFG_CHANP_MONITOR_SELECT_READ():
    do_rw_reg("OPXXCH_CFG_CHANP_MONITOR_SELECT_READ", 0x10104, 0x3f, 0, 6)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_OPXXCH_CFG_CHANP_MONITOR_SNAPSHOT():
    do_wo_reg("OPXXCH_CFG_CHANP_MONITOR_SNAPSHOT", 0x10108, 0x1, 0, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_OPXXCH_CFG_CHANP_MONITOR_SAMPLE():
    do_wo_reg("OPXXCH_CFG_CHANP_MONITOR_SAMPLE", 0x1010c, 0x1, 0, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_OPXXCH_CFG_CHANP_MONITOR_CLEAR():
    do_wo_reg("OPXXCH_CFG_CHANP_MONITOR_CLEAR", 0x10110, 0x1, 0, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CFG_CHANP_MONITOR_READ_31__0():
    do_ro_reg("OPXXCH_CFG_CHANP_MONITOR_READ_31__0", 0x10120, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CFG_CHANP_MONITOR_READ_63_32():
    do_ro_reg("OPXXCH_CFG_CHANP_MONITOR_READ_63_32", 0x10124, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_FRAM_CHANP_DL_DISABLE():
    do_rw_reg("OPXXCH_FRAM_CHANP_DL_DISABLE", 0x12000, 0x1, 0, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_FRAM_CHANP_DL_READY():
    do_ro_reg("OPXXCH_FRAM_CHANP_DL_READY", 0x12000, 0x2, 1, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_FRAM_CHANP_UL_DISABLE():
    do_rw_reg("OPXXCH_FRAM_CHANP_UL_DISABLE", 0x12000, 0x4, 2, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_FRAM_CHANP_UL_READY():
    do_ro_reg("OPXXCH_FRAM_CHANP_UL_READY", 0x12000, 0x8, 3, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_DL_NUMBER_OF_ANTENNAS():
    do_rw_reg("OPXXCH_CTRL_DL_NUMBER_OF_ANTENNAS", 0x14004, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_DL_ANTENNA_INTERLEAVE():
    do_rw_reg("OPXXCH_CTRL_DL_ANTENNA_INTERLEAVE", 0x14008, 0x7, 0, 3)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE():
    do_rw_reg("OPXXCH_CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE", 0x1400c, 0x1, 0, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_DL_SEQUENCE_TABLE_CC_SEL():
    do_rw_reg("OPXXCH_CTRL_DL_SEQUENCE_TABLE_CC_SEL", 0x1400c, 0xf0, 4, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_DL_SEQUENCE_TABLE_ENABLE():
    do_rw_reg("OPXXCH_CTRL_DL_SEQUENCE_TABLE_ENABLE", 0x1400c, 0x40000000, 30, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_OPXXCH_CTRL_DL_SEQUENCE_TABLE_WR_STROBE():
    do_wo_reg("OPXXCH_CTRL_DL_SEQUENCE_TABLE_WR_STROBE", 0x1400c, 0x80000000, 31, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_NEXT_DL_CC_NUMBER_OF_RES():
    for i in range(0, C_MAX_COMP_CARR_OCP):
        do_rw_reg("OPXXCH_CTRL_NEXT_DL_CC_NUMBER_OF_RES", 0x14010 + i * 0x10, 0x1fff, 0, 13)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CTRL_CURRENT_DL_CC_NUMBER_OF_RES():
    for i in range(0, C_MAX_COMP_CARR_OCP):
        do_ro_reg("OPXXCH_CTRL_CURRENT_DL_CC_NUMBER_OF_RES", 0x14014 + i * 0x10, 0x1fff, 0, 13)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_UL_NUMBER_OF_ANTENNAS():
    do_rw_reg("OPXXCH_CTRL_UL_NUMBER_OF_ANTENNAS", 0x14204, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_UL_ANTENNA_INTERLEAVE():
    do_rw_reg("OPXXCH_CTRL_UL_ANTENNA_INTERLEAVE", 0x14208, 0x7, 0, 3)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE():
    do_rw_reg("OPXXCH_CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE", 0x1420c, 0x1, 0, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_UL_SEQUENCE_TABLE_CC_SEL():
    do_rw_reg("OPXXCH_CTRL_UL_SEQUENCE_TABLE_CC_SEL", 0x1420c, 0xf0, 4, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_UL_SEQUENCE_TABLE_ENABLE():
    do_rw_reg("OPXXCH_CTRL_UL_SEQUENCE_TABLE_ENABLE", 0x1420c, 0x40000000, 30, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_OPXXCH_CTRL_UL_SEQUENCE_TABLE_WR_STROBE():
    do_wo_reg("OPXXCH_CTRL_UL_SEQUENCE_TABLE_WR_STROBE", 0x1420c, 0x80000000, 31, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_OPXXCH_CTRL_NEXT_UL_CC_NUMBER_OF_RES():
    for i in range(0, C_MAX_COMP_CARR_OCP):
        do_rw_reg("OPXXCH_CTRL_NEXT_UL_CC_NUMBER_OF_RES", 0x14210 + i * 0x10, 0x1fff, 0, 13)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_OPXXCH_CTRL_CURRENT_UL_CC_NUMBER_OF_RES():
    for i in range(0, C_MAX_COMP_CARR_OCP):
        do_ro_reg("OPXXCH_CTRL_CURRENT_UL_CC_NUMBER_OF_RES", 0x14214 + i * 0x10, 0x1fff, 0, 13)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_NUM_RES():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_NUM_RES", 0x16000 + i * 0x10, 0xfff, 0, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_RE_START():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_RE_START", 0x16000 + i * 0x10, 0xfff0000, 16, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ID():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ID", 0x16004 + i * 0x10, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_NUM():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_NUM", 0x16004 + i * 0x10, 0xf00, 8, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ISG():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ISG", 0x16004 + i * 0x10, 0xfff0000, 16, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_FINAL():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_FINAL", 0x16004 + i * 0x10, 0x40000000, 30, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_LAST():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_LAST", 0x16004 + i * 0x10, 0x80000000, 31, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_ANT_CONFIG():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_ANT_CONFIG", 0x16008 + i * 0x10, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_NUM_RES():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_NUM_RES", 0x16400 + i * 0x10, 0xfff, 0, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_RE_START():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_RE_START", 0x16400 + i * 0x10, 0xfff0000, 16, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_ID():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_ID", 0x16404 + i * 0x10, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_NUM():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_NUM", 0x16404 + i * 0x10, 0xf00, 8, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_FINAL():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_FINAL", 0x16404 + i * 0x10, 0x40000000, 30, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_LAST():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_LAST", 0x16404 + i * 0x10, 0x80000000, 31, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_ANT_CONFIG():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_ANT_CONFIG", 0x16408 + i * 0x10, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_NUM_RES():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_NUM_RES", 0x16800 + i * 0x10, 0xfff, 0, 12)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_RE_START():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_RE_START", 0x16800 + i * 0x10, 0xfff0000, 16, 12)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ID():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ID", 0x16804 + i * 0x10, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_NUM():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_NUM", 0x16804 + i * 0x10, 0xf00, 8, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ISG():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ISG", 0x16804 + i * 0x10, 0xfff0000, 16, 12)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_FINAL():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_FINAL", 0x16804 + i * 0x10, 0x40000000, 30, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_LAST():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_LAST", 0x16804 + i * 0x10, 0x80000000, 31, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_ANT_CONFIG():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_ANT_CONFIG", 0x16808 + i * 0x10, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_NUM_RES():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_NUM_RES", 0x16c00 + i * 0x10, 0xfff, 0, 12)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_RE_START():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_RE_START", 0x16c00 + i * 0x10, 0xfff0000, 16, 12)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_ID():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_ID", 0x16c04 + i * 0x10, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_NUM():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_NUM", 0x16c04 + i * 0x10, 0xf00, 8, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_FINAL():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_FINAL", 0x16c04 + i * 0x10, 0x40000000, 30, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_LAST():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_LAST", 0x16c04 + i * 0x10, 0x80000000, 31, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_ANT_CONFIG():
    for i in range(0, ADDR_TABLE_ENTRIES):
        do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_ANT_CONFIG", 0x16c08 + i * 0x10, 0xffffffff, 0, 32)
