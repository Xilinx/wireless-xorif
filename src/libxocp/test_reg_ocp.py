#!/usr/bin/env python3

import pytest
import os
import mmap
import ctypes
import re

# Configure required tests here
RO = True
RW = True

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
    #regs.seek(offset)
    #val = regs.read(4)
    #return int(struct.unpack("@I", val)[0])
    # Using ctypes function here to cause 32-bit read rather than 4-bytes
    return ctypes.c_uint32.from_buffer(regs, offset).value

def poke(offset, value):
    '''Use mmap to write the 32-bit value to the specified address.'''
    #regs.seek(offset)
    #regs.write(struct.pack("@I", value))
    # Using ctypes function here to cause 32-bit write rather than 4-bytes
    ctypes.c_uint32.from_buffer(regs, offset).value = value

# Get the base-address for the device
try:
    base_addr = get_device_address("xorif_chan_proc")
except:
    # Skip all tests if no device
    pytestmark = pytest.mark.skip
else:
    # Set up mmap to access registers
    devmem = os.open('/dev/mem', os.O_RDWR | os.O_SYNC)
    regs = mmap.mmap(devmem,
                    0x10000,
                    mmap.MAP_SHARED,
                    mmap.PROT_READ | mmap.PROT_WRITE,
                    offset=base_addr)
    os.close(devmem)

    # Set the following variables according to device
    MAX_NUM_CC = peek(0x8) & 0xf

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
        # checkerboard r/w test
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

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_MAJOR_REVISION():
    do_ro_reg("CFG_MAJOR_REVISION", 0x0, 0xff000000, 24, 8)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_MINOR_REVISION():
    do_ro_reg("CFG_MINOR_REVISION", 0x0, 0xff0000, 16, 8)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_VERSION_REVISION():
    do_ro_reg("CFG_VERSION_REVISION", 0x0, 0xff00, 8, 8)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_INTERNAL_REVISION():
    do_ro_reg("CFG_INTERNAL_REVISION", 0x4, 0xffffffff, 0, 32)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_MAX_NUM_CC():
    do_ro_reg("CFG_MAX_NUM_CC", 0x8, 0xf, 0, 4)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_MAX_NUM_ANTENNA():
    do_ro_reg("CFG_MAX_NUM_ANTENNA", 0x8, 0xf00, 8, 4)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_MAX_ANTENNA_INTERLEAVE():
    do_ro_reg("CFG_MAX_ANTENNA_INTERLEAVE", 0x8, 0x70000, 16, 3)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_MASTER_INT_ENABLE():
    do_rw_reg("CFG_MASTER_INT_ENABLE", 0x10, 0x1, 0, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_DL_CC_UPDATE_TRIGGERED_EN():
    do_rw_reg("CFG_DL_CC_UPDATE_TRIGGERED_EN", 0x14, 0x1, 0, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_DL_SEQUENCE_ERROR_EN():
    do_rw_reg("CFG_DL_SEQUENCE_ERROR_EN", 0x14, 0x2, 1, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_DL_SEQUENCE_TABLE_ERROR_EN():
    do_rw_reg("CFG_DL_SEQUENCE_TABLE_ERROR_EN", 0x14, 0x4, 2, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_UL_CC_UPDATE_TRIGGERED_EN():
    do_rw_reg("CFG_UL_CC_UPDATE_TRIGGERED_EN", 0x14, 0x100, 8, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_UL_SEQUENCE_ERROR_EN():
    do_rw_reg("CFG_UL_SEQUENCE_ERROR_EN", 0x14, 0x200, 9, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_UL_SEQUENCE_TABLE_ERROR_EN():
    do_rw_reg("CFG_UL_SEQUENCE_TABLE_ERROR_EN", 0x14, 0x400, 10, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_DL_CC_UPDATE_TRIGGERED():
    do_ro_reg("CFG_DL_CC_UPDATE_TRIGGERED", 0x18, 0x1, 0, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_DL_SEQUENCE_ERROR():
    do_ro_reg("CFG_DL_SEQUENCE_ERROR", 0x18, 0x2, 1, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_DL_SEQUENCE_TABLE_ERROR():
    do_ro_reg("CFG_DL_SEQUENCE_TABLE_ERROR", 0x18, 0x4, 2, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_UL_CC_UPDATE_TRIGGERED():
    do_ro_reg("CFG_UL_CC_UPDATE_TRIGGERED", 0x18, 0x100, 8, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_UL_SEQUENCE_ERROR():
    do_ro_reg("CFG_UL_SEQUENCE_ERROR", 0x18, 0x200, 9, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_UL_SEQUENCE_TABLE_ERROR():
    do_ro_reg("CFG_UL_SEQUENCE_TABLE_ERROR", 0x18, 0x400, 10, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_MONITOR_SELECT_READ():
    do_rw_reg("CFG_MONITOR_SELECT_READ", 0x104, 0x3f, 0, 6)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_MONITOR_READ_31__0():
    do_ro_reg("CFG_MONITOR_READ_31__0", 0x120, 0xffffffff, 0, 32)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_MONITOR_READ_63_32():
    do_ro_reg("CFG_MONITOR_READ_63_32", 0x124, 0xffffffff, 0, 32)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_FRAM_CHANP_DL_DISABLE():
    do_rw_reg("FRAM_CHANP_DL_DISABLE", 0x2000, 0x1, 0, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_FRAM_CHANP_DL_READY():
    do_ro_reg("FRAM_CHANP_DL_READY", 0x2000, 0x2, 1, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_FRAM_CHANP_UL_DISABLE():
    do_rw_reg("FRAM_CHANP_UL_DISABLE", 0x2000, 0x4, 2, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_FRAM_CHANP_UL_READY():
    do_ro_reg("FRAM_CHANP_UL_READY", 0x2000, 0x8, 3, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_DL_NUMBER_OF_ANTENNAS():
    do_rw_reg("CTRL_DL_NUMBER_OF_ANTENNAS", 0x4004, 0xf, 0, 4)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_DL_ANTENNA_INTERLEAVE():
    do_rw_reg("CTRL_DL_ANTENNA_INTERLEAVE", 0x4008, 0x7, 0, 3)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE():
    do_rw_reg("CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE", 0x400c, 0x1, 0, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_DL_SEQUENCE_TABLE_CC_SEL():
    do_rw_reg("CTRL_DL_SEQUENCE_TABLE_CC_SEL", 0x400c, 0xf0, 4, 4)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_DL_SEQUENCE_TABLE_ENABLE():
    do_rw_reg("CTRL_DL_SEQUENCE_TABLE_ENABLE", 0x400c, 0x40000000, 30, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_NEXT_DL_CC_NUMBER_OF_RES():
    for i in range(0, MAX_NUM_CC):
        do_rw_reg("CTRL_NEXT_DL_CC_NUMBER_OF_RES", 0x4010 + i * 0x10, 0x1fff, 0, 13)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CTRL_CURRENT_DL_CC_NUMBER_OF_RES():
    for i in range(0, MAX_NUM_CC):
        do_ro_reg("CTRL_CURRENT_DL_CC_NUMBER_OF_RES", 0x4014 + i * 0x10, 0x1fff, 0, 13)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_UL_NUMBER_OF_ANTENNAS():
    do_rw_reg("CTRL_UL_NUMBER_OF_ANTENNAS", 0x4204, 0xf, 0, 4)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_UL_ANTENNA_INTERLEAVE():
    do_rw_reg("CTRL_UL_ANTENNA_INTERLEAVE", 0x4208, 0x7, 0, 3)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE():
    do_rw_reg("CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE", 0x420c, 0x1, 0, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_UL_SEQUENCE_TABLE_CC_SEL():
    do_rw_reg("CTRL_UL_SEQUENCE_TABLE_CC_SEL", 0x420c, 0xf0, 4, 4)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_UL_SEQUENCE_TABLE_ENABLE():
    do_rw_reg("CTRL_UL_SEQUENCE_TABLE_ENABLE", 0x420c, 0x40000000, 30, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CTRL_NEXT_UL_CC_NUMBER_OF_RES():
    for i in range(0, MAX_NUM_CC):
        do_rw_reg("CTRL_NEXT_UL_CC_NUMBER_OF_RES", 0x4210 + i * 0x10, 0x1fff, 0, 13)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CTRL_CURRENT_UL_CC_NUMBER_OF_RES():
    for i in range(0, MAX_NUM_CC):
        do_ro_reg("CTRL_CURRENT_UL_CC_NUMBER_OF_RES", 0x4214 + i * 0x10, 0x1fff, 0, 13)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_NUM_RES():
    do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_NUM_RES", 0x6000, 0xfff, 0, 12)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_RE_START():
    do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_RE_START", 0x6000, 0xfff0000, 16, 12)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ID():
    do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ID", 0x6004, 0xf, 0, 4)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_NUM():
    do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_NUM", 0x6004, 0xf00, 8, 4)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ISG():
    do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ISG", 0x6004, 0xfff0000, 16, 12)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_FINAL():
    do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_FINAL", 0x6004, 0x40000000, 30, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_LAST():
    do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_LAST", 0x6004, 0x80000000, 31, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_ANT_CONFIG():
    do_rw_reg("SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_ANT_CONFIG", 0x6008, 0xffffffff, 0, 32)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_NUM_RES():
    do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_NUM_RES", 0x6400, 0xfff, 0, 12)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_RE_START():
    do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_RE_START", 0x6400, 0xfff0000, 16, 12)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_ID():
    do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_ID", 0x6404, 0xf, 0, 4)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_NUM():
    do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_NUM", 0x6404, 0xf00, 8, 4)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_FINAL():
    do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_FINAL", 0x6404, 0x40000000, 30, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_LAST():
    do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_LAST", 0x6404, 0x80000000, 31, 1)

@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_ANT_CONFIG():
    do_rw_reg("SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_ANT_CONFIG", 0x6408, 0xffffffff, 0, 32)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_NUM_RES():
    do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_NUM_RES", 0x6800, 0xfff, 0, 12)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_RE_START():
    do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_RE_START", 0x6800, 0xfff0000, 16, 12)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ID():
    do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ID", 0x6804, 0xf, 0, 4)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_NUM():
    do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_NUM", 0x6804, 0xf00, 8, 4)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ISG():
    do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ISG", 0x6804, 0xfff0000, 16, 12)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_FINAL():
    do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_FINAL", 0x6804, 0x40000000, 30, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_LAST():
    do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_LAST", 0x6804, 0x80000000, 31, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_ANT_CONFIG():
    do_ro_reg("SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_ANT_CONFIG", 0x6808, 0xffffffff, 0, 32)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_NUM_RES():
    do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_NUM_RES", 0x6c00, 0xfff, 0, 12)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_RE_START():
    do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_RE_START", 0x6c00, 0xfff0000, 16, 12)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_ID():
    do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_ID", 0x6c04, 0xf, 0, 4)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_NUM():
    do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_NUM", 0x6c04, 0xf00, 8, 4)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_FINAL():
    do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_FINAL", 0x6c04, 0x40000000, 30, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_LAST():
    do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_LAST", 0x6c04, 0x80000000, 31, 1)

@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_ANT_CONFIG():
    do_ro_reg("SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_ANT_CONFIG", 0x6c08, 0xffffffff, 0, 32)
