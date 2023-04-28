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

# Configure optional parts of tests
cfg_wide_filt = True
cfg_ena_ipv6 = True
cfg_ena_ipv4 = True
cfg_ena_ssb = True


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
                     0x10000,
                     mmap.MAP_SHARED,
                     mmap.PROT_READ | mmap.PROT_WRITE,
                     offset=base_addr)
    os.close(devmem)

    # Set the following variables according to device
    C_ETH_W = peek(0x24) & 0x3ff
    C_MAX_COMP_CARR = peek(0x2c) & 0x1f
    C_MAX_DL_SYMS = peek(0x30) & 0x1f

    # Set the following variables according to device
    opt = peek(0x78)
    cfg_wide_filt = opt & 0x10
    cfg_ena_ipv6 = opt & 0x20
    cfg_ena_ipv4 = opt & 0x40
    cfg_ena_ssb = opt & 0x80


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


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_TIMEOUT_VALUE():
    do_rw_reg("CFG_TIMEOUT_VALUE", 0x8, 0xfff, 0, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_USER_RW_OUT():
    do_rw_reg("CFG_USER_RW_OUT", 0xc, 0xff, 0, 8)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_USER_RO_IN():
    do_ro_reg("CFG_USER_RO_IN", 0xc, 0xff0000, 16, 8)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_MASTER_INT_ENABLE():
    do_rw_reg("CFG_MASTER_INT_ENABLE", 0x10, 0x1, 0, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_DEFM_INT_ENA_INFIFO_OF():
    do_rw_reg("CFG_DEFM_INT_ENA_INFIFO_OF", 0x14, 0x100, 8, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_DEFM_INT_ENA_INFIFO_UF():
    do_rw_reg("CFG_DEFM_INT_ENA_INFIFO_UF", 0x14, 0x200, 9, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF():
    do_rw_reg("CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF", 0x14, 0x400, 10, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF():
    do_rw_reg("CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF", 0x14, 0x800, 11, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_FRAM_INT_ENA_OUTFIFO_OF():
    do_rw_reg("CFG_FRAM_INT_ENA_OUTFIFO_OF", 0x14, 0x1000, 12, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_FRAM_INT_ENA_OUTFIFO_UF():
    do_rw_reg("CFG_FRAM_INT_ENA_OUTFIFO_UF", 0x14, 0x2000, 13, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW():
    do_rw_reg("CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW", 0x14, 0x4000, 14, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND():
    do_rw_reg("CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND", 0x14, 0x8000, 15, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_AXI_TIMEOUT_ENABLE():
    do_rw_reg("CFG_AXI_TIMEOUT_ENABLE", 0x14, 0x80000000, 31, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_FRAM_RESET_STATUS():
    do_ro_reg("CFG_FRAM_RESET_STATUS", 0x18, 0x1, 0, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_DEFM_RESET_STATUS():
    do_ro_reg("CFG_DEFM_RESET_STATUS", 0x18, 0x2, 1, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_DEFM_INT_INFIFO_OF():
    do_ro_reg("CFG_DEFM_INT_INFIFO_OF", 0x18, 0x100, 8, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_DEFM_INT_INFIFO_UF():
    do_ro_reg("CFG_DEFM_INT_INFIFO_UF", 0x18, 0x200, 9, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_DEFM_INT_ETH_PIPE_C_BUF_OF():
    do_ro_reg("CFG_DEFM_INT_ETH_PIPE_C_BUF_OF", 0x18, 0x400, 10, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_DEFM_INT_ETH_PIPE_TABLE_OF():
    do_ro_reg("CFG_DEFM_INT_ETH_PIPE_TABLE_OF", 0x18, 0x800, 11, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_FRAM_INT_OUTFIFO_OF():
    do_ro_reg("CFG_FRAM_INT_OUTFIFO_OF", 0x18, 0x1000, 12, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_FRAM_INT_OUTFIFO_UF():
    do_ro_reg("CFG_FRAM_INT_OUTFIFO_UF", 0x18, 0x2000, 13, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_FRAM_INT_PRACH_SECTION_OVERFLOW():
    do_ro_reg("CFG_FRAM_INT_PRACH_SECTION_OVERFLOW", 0x18, 0x4000, 14, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_FRAM_INT_PRACH_SECTION_NOTFOUND():
    do_ro_reg("CFG_FRAM_INT_PRACH_SECTION_NOTFOUND", 0x18, 0x8000, 15, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_AXI_TIMEOUT_STATUS():
    do_ro_reg("CFG_AXI_TIMEOUT_STATUS", 0x18, 0x80000000, 31, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_INTERRUPT_STATUS_SAMPLE():
    do_rw_reg("CFG_INTERRUPT_STATUS_SAMPLE", 0x1c, 0x1, 0, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_NO_OF_FRAM_ANTS():
    do_ro_reg("CFG_CONFIG_NO_OF_FRAM_ANTS", 0x20, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_NO_OF_DEFM_ANTS():
    do_ro_reg("CFG_CONFIG_NO_OF_DEFM_ANTS", 0x20, 0xffff0000, 16, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_NO_OF_ETH_PORTS():
    do_ro_reg("CFG_CONFIG_NO_OF_ETH_PORTS", 0x24, 0x3ff, 0, 10)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_SUPPORT_MODE():
    do_ro_reg("CFG_CONFIG_XRAN_SUPPORT_MODE", 0x28, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_DECOMP_IN_CORE_ENABLED():
    do_ro_reg("CFG_CONFIG_XRAN_DECOMP_IN_CORE_ENABLED", 0x28, 0x10, 4, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_COMP_IN_CORE_ENABLED():
    do_ro_reg("CFG_CONFIG_XRAN_COMP_IN_CORE_ENABLED", 0x28, 0x20, 5, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_PRECODING_EXT3_PORT():
    do_ro_reg("CFG_CONFIG_XRAN_PRECODING_EXT3_PORT", 0x28, 0x40, 6, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_OCP_IN_CORE():
    do_ro_reg("CFG_CONFIG_XRAN_OCP_IN_CORE", 0x28, 0x80, 7, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_MAX_CC():
    do_ro_reg("CFG_CONFIG_XRAN_MAX_CC", 0x2c, 0x3f, 0, 6)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_MAX_DL_SYMBOLS():
    do_ro_reg("CFG_CONFIG_XRAN_MAX_DL_SYMBOLS", 0x30, 0x7f, 0, 7)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_MAX_NUMEROLOGY():
    do_ro_reg("CFG_CONFIG_XRAN_MAX_NUMEROLOGY", 0x34, 0x7, 0, 3)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_MIN_NUMEROLOGY():
    do_ro_reg("CFG_CONFIG_XRAN_MIN_NUMEROLOGY", 0x38, 0x7, 0, 3)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX():
    do_ro_reg("CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX", 0x3c, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_FRAM_AUTO_START():
    do_ro_reg("CFG_CONFIG_XRAN_FRAM_AUTO_START", 0x40, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX():
    do_ro_reg("CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX", 0x44, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_MAX_SCS():
    do_ro_reg("CFG_CONFIG_XRAN_MAX_SCS", 0x48, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS():
    do_ro_reg("CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS", 0x4c, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS():
    do_ro_reg("CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS", 0x50, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS():
    do_ro_reg("CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS", 0x54, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS():
    do_ro_reg("CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS", 0x58, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_TIMER_CLK_PS():
    do_ro_reg("CFG_CONFIG_XRAN_TIMER_CLK_PS", 0x5c, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM():
    do_ro_reg("CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM", 0x68, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH():
    do_ro_reg("CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH", 0x6c, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS():
    do_ro_reg("CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS", 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_PRACH_C_PORTS():
    do_ro_reg("CFG_CONFIG_XRAN_PRACH_C_PORTS", 0x74, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_OPTIMISATIONS():
    do_ro_reg("CFG_CONFIG_XRAN_OPTIMISATIONS", 0x78, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_DECOMP_IN_CORE_NOCOMP():
    do_ro_reg("CFG_CONFIG_XRAN_DECOMP_IN_CORE_NOCOMP", 0x80, 0x1, 0, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP():
    do_ro_reg("CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP", 0x80, 0x2, 1, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_DECOMP_IN_CORE_BSC():
    do_ro_reg("CFG_CONFIG_XRAN_DECOMP_IN_CORE_BSC", 0x80, 0x4, 2, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_DECOMP_IN_CORE_MU():
    do_ro_reg("CFG_CONFIG_XRAN_DECOMP_IN_CORE_MU", 0x80, 0x8, 3, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP():
    do_ro_reg("CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP", 0x80, 0x10, 4, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_SELRE():
    do_ro_reg("CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_SELRE", 0x80, 0x20, 5, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_SELRE():
    do_ro_reg("CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_SELRE", 0x80, 0x40, 6, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_WIDTHS():
    do_ro_reg("CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_WIDTHS", 0x84, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODC_WIDTHS():
    do_ro_reg("CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODC_WIDTHS", 0x84, 0x3f0000, 16, 6)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_COMP_IN_CORE_NOCOMP():
    do_ro_reg("CFG_CONFIG_XRAN_COMP_IN_CORE_NOCOMP", 0x88, 0x10000, 16, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_COMP_IN_CORE_BFP():
    do_ro_reg("CFG_CONFIG_XRAN_COMP_IN_CORE_BFP", 0x88, 0x20000, 17, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_SELRE():
    do_ro_reg("CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_SELRE", 0x88, 0x200000, 21, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_WIDTHS():
    do_ro_reg("CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_WIDTHS", 0x8c, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_LIMIT_RU_I_W():
    do_ro_reg("CFG_CONFIG_LIMIT_RU_I_W", 0x100, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_LIMIT_RU_O_W():
    do_ro_reg("CFG_CONFIG_LIMIT_RU_O_W", 0x104, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_LIMIT_CC_W():
    do_ro_reg("CFG_CONFIG_LIMIT_CC_W", 0x108, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_LIMIT_BS_W():
    do_ro_reg("CFG_CONFIG_LIMIT_BS_W", 0x10c, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_LIMIT_DU_W():
    do_ro_reg("CFG_CONFIG_LIMIT_DU_W", 0x110, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_MAP_TABLE_W():
    do_ro_reg("CFG_CONFIG_MAP_TABLE_W", 0x114, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_MAP_STREAM_W():
    do_ro_reg("CFG_CONFIG_MAP_STREAM_W", 0x114, 0xf00, 8, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_CONFIG_MAP_TYPE_W():
    do_ro_reg("CFG_CONFIG_MAP_TYPE_W", 0x114, 0xf0000, 16, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_MONITOR_SELECT_CNT():
    do_rw_reg("CFG_MONITOR_SELECT_CNT", 0x800, 0x3f, 0, 6)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_CFG_MONITOR_SELECT_READ():
    do_rw_reg("CFG_MONITOR_SELECT_READ", 0x804, 0x3f, 0, 6)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_CFG_MONITOR_SNAPSHOT():
    do_wo_reg("CFG_MONITOR_SNAPSHOT", 0x808, 0x1, 0, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_CFG_MONITOR_SAMPLE():
    do_wo_reg("CFG_MONITOR_SAMPLE", 0x80c, 0x1, 0, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_CFG_MONITOR_CLEAR():
    do_wo_reg("CFG_MONITOR_CLEAR", 0x810, 0x1, 0, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_MONITOR_READ_31__0():
    do_ro_reg("CFG_MONITOR_READ_31__0", 0x820, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_CFG_MONITOR_READ_63_32():
    do_ro_reg("CFG_MONITOR_READ_63_32", 0x824, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_FRAM_DISABLE():
    do_rw_reg("FRAM_DISABLE", 0x2000, 0x1, 0, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_FRAM_READY():
    do_ro_reg("FRAM_READY", 0x2000, 0x2, 1, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_FRAM_ENABLE_PER_SYM_RESET():
    do_rw_reg("FRAM_ENABLE_PER_SYM_RESET", 0x2004, 0x1, 0, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_FRAM_MTU_SIZE():
    do_rw_reg("FRAM_MTU_SIZE", 0x2008, 0x3fff, 0, 14)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_FRAM_CID_SEQTABLE_MODE():
    do_rw_reg("FRAM_CID_SEQTABLE_MODE", 0x200c, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_FRAM_PROTOCOL_DEFINITION():
    do_rw_reg("FRAM_PROTOCOL_DEFINITION", 0x2200, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_FRAM_GEN_VLAN_TAG():
    do_rw_reg("FRAM_GEN_VLAN_TAG", 0x2200, 0x10, 4, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_FRAM_SEL_IPV_ADDRESS_TYPE():
    do_rw_reg("FRAM_SEL_IPV_ADDRESS_TYPE", 0x2200, 0x60, 5, 2)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_RESTART():
    do_rw_reg("DEFM_RESTART", 0x6000, 0x1, 0, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_DEFM_READY():
    do_ro_reg("DEFM_READY", 0x6000, 0x2, 1, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_ERR_PACKET_FILTER():
    do_rw_reg("DEFM_ERR_PACKET_FILTER", 0x6004, 0x3, 0, 2)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USE_ONE_SYMBOL_STROBE():
    do_rw_reg("DEFM_USE_ONE_SYMBOL_STROBE", 0x6008, 0x1, 0, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_DEBUG():
    do_rw_reg("DEFM_DEBUG", 0x600c, 0xf, 0, 4)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_SNAP_SHOT():
    do_wo_reg("DEFM_SNAP_SHOT", 0x6010, 0x1, 0, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E0_E():
    do_wo_reg("DEFM_CTRL_SS_RESET_E0_E", 0x6010, 0x10, 4, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E0_RWIN():
    do_wo_reg("DEFM_CTRL_SS_RESET_E0_RWIN", 0x6010, 0x20, 5, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E0_RCOR():
    do_wo_reg("DEFM_CTRL_SS_RESET_E0_RCOR", 0x6010, 0x40, 6, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E0_T():
    do_wo_reg("DEFM_CTRL_SS_RESET_E0_T", 0x6010, 0x80, 7, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E1_E():
    do_wo_reg("DEFM_CTRL_SS_RESET_E1_E", 0x6010, 0x100, 8, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E1_RWIN():
    do_wo_reg("DEFM_CTRL_SS_RESET_E1_RWIN", 0x6010, 0x200, 9, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E1_RCOR():
    do_wo_reg("DEFM_CTRL_SS_RESET_E1_RCOR", 0x6010, 0x400, 10, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E1_T():
    do_wo_reg("DEFM_CTRL_SS_RESET_E1_T", 0x6010, 0x800, 11, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E2_E():
    do_wo_reg("DEFM_CTRL_SS_RESET_E2_E", 0x6010, 0x1000, 12, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E2_RWIN():
    do_wo_reg("DEFM_CTRL_SS_RESET_E2_RWIN", 0x6010, 0x2000, 13, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E2_RCOR():
    do_wo_reg("DEFM_CTRL_SS_RESET_E2_RCOR", 0x6010, 0x4000, 14, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E2_T():
    do_wo_reg("DEFM_CTRL_SS_RESET_E2_T", 0x6010, 0x8000, 15, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E3_E():
    do_wo_reg("DEFM_CTRL_SS_RESET_E3_E", 0x6010, 0x10000, 16, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E3_RWIN():
    do_wo_reg("DEFM_CTRL_SS_RESET_E3_RWIN", 0x6010, 0x20000, 17, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E3_RCOR():
    do_wo_reg("DEFM_CTRL_SS_RESET_E3_RCOR", 0x6010, 0x40000, 18, 1)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CTRL_SS_RESET_E3_T():
    do_wo_reg("DEFM_CTRL_SS_RESET_E3_T", 0x6010, 0x80000, 19, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CTRL_ENA_CRUPT_EAXE_CNT():
    do_rw_reg("DEFM_CTRL_ENA_CRUPT_EAXE_CNT", 0x6014, 0x1, 0, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_CC_SHIFT():
    do_rw_reg("DEFM_CID_CC_SHIFT", 0x6020, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_CC_MASK():
    do_rw_reg("DEFM_CID_CC_MASK", 0x6024, 0x3f, 0, 6)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_BS_SHIFT():
    do_rw_reg("DEFM_CID_BS_SHIFT", 0x6028, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_BS_MASK():
    do_rw_reg("DEFM_CID_BS_MASK", 0x602c, 0x3f, 0, 6)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_DU_SHIFT():
    do_rw_reg("DEFM_CID_DU_SHIFT", 0x6030, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_DU_MASK():
    do_rw_reg("DEFM_CID_DU_MASK", 0x6034, 0x3f, 0, 6)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_SS_MASK():
    do_rw_reg("DEFM_CID_SS_MASK", 0x6038, 0xff, 0, 8)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_U_MASK():
    do_rw_reg("DEFM_CID_U_MASK", 0x603c, 0xff, 0, 8)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_U_VALUE():
    do_rw_reg("DEFM_CID_U_VALUE", 0x6040, 0xff, 0, 8)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_PRACH_MASK():
    do_rw_reg("DEFM_CID_PRACH_MASK", 0x6044, 0xff, 0, 8)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_PRACH_VALUE():
    do_rw_reg("DEFM_CID_PRACH_VALUE", 0x6048, 0xff, 0, 8)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_SSB_MASK():
    do_rw_reg("DEFM_CID_SSB_MASK", 0x604c, 0xff, 0, 8)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_SSB_VALUE():
    do_rw_reg("DEFM_CID_SSB_VALUE", 0x6050, 0xff, 0, 8)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_LTE_MASK():
    do_rw_reg("DEFM_CID_LTE_MASK", 0x6054, 0xff, 0, 8)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_LTE_VALUE():
    do_rw_reg("DEFM_CID_LTE_VALUE", 0x6058, 0xff, 0, 8)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W0_31_0():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W0_31_0", 0x6100 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W0_63_32():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W0_63_32", 0x6104 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W0_95_64():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W0_95_64", 0x6108 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W0_127_96():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W0_127_96", 0x610c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W0_MASK():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W0_MASK", 0x6110 + i * 0x100, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W1_31_0():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W1_31_0", 0x6120 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W1_63_32():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W1_63_32", 0x6124 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W1_95_64():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W1_95_64", 0x6128 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W1_127_96():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W1_127_96", 0x612c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W1_MASK():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W1_MASK", 0x6130 + i * 0x100, 0xffff, 0, 16)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W2_31_0():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W2_31_0", 0x6140 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W2_63_32():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W2_63_32", 0x6144 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W2_95_64():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W2_95_64", 0x6148 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W2_127_96():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W2_127_96", 0x614c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W2_MASK():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W2_MASK", 0x6150 + i * 0x100, 0xffff, 0, 16)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W3_31_0():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W3_31_0", 0x6160 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W3_63_32():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W3_63_32", 0x6164 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W3_95_64():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W3_95_64", 0x6168 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W3_127_96():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W3_127_96", 0x616c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_wide_filt, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_USER_DATA_FILTER_W3_MASK():
    for i in range(0, C_ETH_W):
        do_rw_reg("DEFM_USER_DATA_FILTER_W3_MASK", 0x6170 + i * 0x100, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_MAP_MODE():
    do_rw_reg("DEFM_CID_MAP_MODE", 0x6900, 0x3, 0, 2)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_MAP_SUBMODE():
    do_rw_reg("DEFM_CID_MAP_SUBMODE", 0x6900, 0xf00, 8, 4)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CID_MAP_WR_STROBE():
    do_wo_reg("DEFM_CID_MAP_WR_STROBE", 0x6904, 0x80000000, 31, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_MAP_WR_STREAM_CCID():
    do_rw_reg("DEFM_CID_MAP_WR_STREAM_CCID", 0x6904, 0x7000000, 24, 3)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_MAP_WR_STREAM_PORTID():
    do_rw_reg("DEFM_CID_MAP_WR_STREAM_PORTID", 0x6904, 0x7c0000, 18, 5)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_MAP_WR_STREAM_TYPE():
    do_rw_reg("DEFM_CID_MAP_WR_STREAM_TYPE", 0x6904, 0x7000, 12, 3)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_MAP_WR_TABLE_ADDR():
    do_rw_reg("DEFM_CID_MAP_WR_TABLE_ADDR", 0x6904, 0x7ff, 0, 11)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_CID_MAP_RD_STROBE():
    do_wo_reg("DEFM_CID_MAP_RD_STROBE", 0x6908, 0x80000000, 31, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_DEFM_CID_MAP_RD_STREAM_CCID():
    do_ro_reg("DEFM_CID_MAP_RD_STREAM_CCID", 0x6908, 0x7000000, 24, 3)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_DEFM_CID_MAP_RD_STREAM_PORTID():
    do_ro_reg("DEFM_CID_MAP_RD_STREAM_PORTID", 0x6908, 0x7c0000, 18, 5)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_DEFM_CID_MAP_RD_STREAM_TYPE():
    do_ro_reg("DEFM_CID_MAP_RD_STREAM_TYPE", 0x6908, 0x7000, 12, 3)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_CID_MAP_RD_TABLE_ADDR():
    do_rw_reg("DEFM_CID_MAP_RD_TABLE_ADDR", 0x6908, 0x7ff, 0, 11)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_DEFM_DECOMP_SS_WR():
    do_wo_reg("DEFM_DECOMP_SS_WR", 0x6910, 0x80000000, 31, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_DECOMP_SS_MODE_ENABLE():
    do_rw_reg("DEFM_DECOMP_SS_MODE_ENABLE", 0x6910, 0x10000, 16, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_DECOMP_SS_UDCOMP_TYPE():
    do_rw_reg("DEFM_DECOMP_SS_UDCOMP_TYPE", 0x6910, 0xf000, 12, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_DECOMP_SS_UDCOMP_WIDTH():
    do_rw_reg("DEFM_DECOMP_SS_UDCOMP_WIDTH", 0x6910, 0xf00, 8, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_DEFM_DECOMP_SS_ADDRESS():
    do_rw_reg("DEFM_DECOMP_SS_ADDRESS", 0x6910, 0xff, 0, 8)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_SETUP_SF():
    do_rw_reg("ORAN_SETUP_SF", 0x8008, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_SETUP_SL():
    do_rw_reg("ORAN_SETUP_SL", 0x800c, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_SETUP_SY():
    do_rw_reg("ORAN_SETUP_SY", 0x8010, 0xf, 0, 4)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_SYMPERSLOT():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_SYMPERSLOT", 0x8100 + i * 0x70, 0x1000000, 24, 1)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_NUMEROLOGY():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_NUMEROLOGY", 0x8100 + i * 0x70, 0x70000, 16, 3)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_NUMRBS():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_NUMRBS", 0x8100 + i * 0x70, 0x1ff, 0, 9)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_CTRL_OFFSETS():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_CTRL_OFFSETS", 0x8104 + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_DATA_SYM_START_INDEX():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_DATA_SYM_START_INDEX", 0x8114 + i * 0x70, 0x3f, 0, 6)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_NUM_DATA_SYM_PER_CC():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_NUM_DATA_SYM_PER_CC", 0x8114 + i * 0x70, 0x3f00, 8, 6)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_NUM_SYM_PER_CC():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_NUM_SYM_PER_CC", 0x8114 + i * 0x70, 0x3f0000, 16, 6)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_UD_IQ_WIDTH():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_UD_IQ_WIDTH", 0x811c + i * 0x70, 0xf, 0, 4)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_UD_COMP_METH():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_UD_COMP_METH", 0x811c + i * 0x70, 0xf0, 4, 4)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_MPLANE_UDCOMP_HDR_SEL():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_MPLANE_UDCOMP_HDR_SEL", 0x811c + i * 0x70, 0x100, 8, 1)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_PRACH_UD_IQ_WIDTH():
    do_rw_reg("ORAN_CC_PRACH_UD_IQ_WIDTH", 0x8120, 0xf, 0, 4)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_PRACH_UD_COMP_METH():
    do_rw_reg("ORAN_CC_PRACH_UD_COMP_METH", 0x8120, 0xf0, 4, 4)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_PRACH_MPLANE_UDCOMP_HDR_SEL():
    do_rw_reg("ORAN_CC_PRACH_MPLANE_UDCOMP_HDR_SEL", 0x8120, 0x100, 8, 1)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_SETUP_C_ABS_SYMBOL():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_SETUP_C_ABS_SYMBOL", 0x8130 + i * 0x70, 0xfff, 0, 12)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_SETUP_C_CYCLES():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_SETUP_C_CYCLES", 0x8134 + i * 0x70, 0x1ffff, 0, 17)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_SETUP_D_CYCLES():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_SETUP_D_CYCLES", 0x8138 + i * 0x70, 0x1ffff, 0, 17)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC", 0x8160 + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_SECTS_X_SYMBOLS():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SSB_SECTS_X_SYMBOLS", 0x8168 + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not cfg_ena_ssb, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SSB_DATA_UNROLL_OFFSET():
    for i in range(0, C_MAX_DL_SYMS):
        do_rw_reg("ORAN_CC_SSB_DATA_UNROLL_OFFSET", 0x8800 + i * 0x4, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_DEST_ADDR_31_0():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_DEST_ADDR_31_0", 0xa000 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_DEST_ADDR_47_32():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_DEST_ADDR_47_32", 0xa004 + i * 0x100, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_SRC_ADDR_31_0():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_SRC_ADDR_31_0", 0xa008 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_SRC_ADDR_47_32():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_SRC_ADDR_47_32", 0xa00c + i * 0x100, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_VLAN_ID():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_VLAN_ID", 0xa010 + i * 0x100, 0xfff, 0, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_VLAN_DEI():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_VLAN_DEI", 0xa010 + i * 0x100, 0x1000, 12, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_VLAN_PCP():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_VLAN_PCP", 0xa010 + i * 0x100, 0xe000, 13, 3)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV4_VERSION():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV4_VERSION", 0xa030 + i * 0x100, 0xf, 0, 4)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV4_IHL():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV4_IHL", 0xa030 + i * 0x100, 0xf0, 4, 4)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV4_DSCP():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV4_DSCP", 0xa034 + i * 0x100, 0x3f, 0, 6)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV4_ECN():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV4_ECN", 0xa034 + i * 0x100, 0xc0, 6, 2)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV4_ID():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV4_ID", 0xa038 + i * 0x100, 0xffff, 0, 16)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV4_FLAGS():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV4_FLAGS", 0xa03c + i * 0x100, 0x7, 0, 3)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV4_FRAGMENT_OFFSET():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV4_FRAGMENT_OFFSET", 0xa03c + i * 0x100, 0xfff8, 3, 13)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV4_TIME_TO_LIVE():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV4_TIME_TO_LIVE", 0xa040 + i * 0x100, 0xff, 0, 8)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV4_PROTOCOL():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV4_PROTOCOL", 0xa044 + i * 0x100, 0xff, 0, 8)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV4_SOURCE_ADD():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV4_SOURCE_ADD", 0xa048 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV4_DESTINATION_ADD():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV4_DESTINATION_ADD", 0xa04c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_UDP_SOURCE_PORT():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_UDP_SOURCE_PORT", 0xa050 + i * 0x100, 0xffff, 0, 16)


@pytest.mark.skipif(not cfg_ena_ipv4, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_UDP_DESTINATION_PORT():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_UDP_DESTINATION_PORT", 0xa050 + i * 0x100, 0xffff0000, 16, 16)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_V():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_V", 0xa080 + i * 0x100, 0xf, 0, 4)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_TRAFFIC_CLASS():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_TRAFFIC_CLASS", 0xa084 + i * 0x100, 0xff, 0, 8)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_FLOW_LABEL():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_FLOW_LABEL", 0xa088 + i * 0x100, 0xfffff, 0, 20)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_NEXT_HEADER():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_NEXT_HEADER", 0xa08c + i * 0x100, 0xff, 0, 8)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_HOP_LIMIT():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_HOP_LIMIT", 0xa090 + i * 0x100, 0xff, 0, 8)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_SOURCE_ADD_31_0():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_SOURCE_ADD_31_0", 0xa094 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_SOURCE_ADD_63_32():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_SOURCE_ADD_63_32", 0xa098 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_SOURCE_ADD_95_64():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_SOURCE_ADD_95_64", 0xa09c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_SOURCE_ADD_127_96():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_SOURCE_ADD_127_96", 0xa0a0 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_DESTINATION_ADD_31_0():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_DESTINATION_ADD_31_0", 0xa0a4 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_DESTINATION_ADD_63_32():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_DESTINATION_ADD_63_32", 0xa0a8 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_DESTINATION_ADD_95_64():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_DESTINATION_ADD_95_64", 0xa0ac + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not cfg_ena_ipv6, reason="register not enabled")
@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_IPV6_DESTINATION_ADD_127_96():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_IPV6_DESTINATION_ADD_127_96", 0xa0b0 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_MULTI_ODU_SUPPORT():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_MULTI_ODU_SUPPORT", 0xa0d0 + i * 0x100, 0x1, 0, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_DU_TABLE_WR_DEST_ADDR_31_0():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_DU_TABLE_WR_DEST_ADDR_31_0", 0xa0e0 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_DU_TABLE_WR_DEST_ADDR_47_32():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_DU_TABLE_WR_DEST_ADDR_47_32", 0xa0e4 + i * 0x100, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_DU_TABLE_WR_VLAN_ID():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_DU_TABLE_WR_VLAN_ID", 0xa0e8 + i * 0x100, 0xfff, 0, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_DU_TABLE_WR_VLAN_DEI():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_DU_TABLE_WR_VLAN_DEI", 0xa0e8 + i * 0x100, 0x1000, 12, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_DU_TABLE_WR_VLAN_PCP():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_DU_TABLE_WR_VLAN_PCP", 0xa0e8 + i * 0x100, 0xe000, 13, 3)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_ETH_DU_TABLE_WR_STROBE():
    for i in range(0, C_ETH_W):
        do_wo_reg("ETH_DU_TABLE_WR_STROBE", 0xa0ec + i * 0x100, 0x80000000, 31, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_DU_TABLE_WR_TABLE_ADDR():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_DU_TABLE_WR_TABLE_ADDR", 0xa0ec + i * 0x100, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_ETH_DU_TABLE_RD_DEST_ADDR_31_0():
    for i in range(0, C_ETH_W):
        do_ro_reg("ETH_DU_TABLE_RD_DEST_ADDR_31_0", 0xa0f0 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_ETH_DU_TABLE_RD_DEST_ADDR_47_32():
    for i in range(0, C_ETH_W):
        do_ro_reg("ETH_DU_TABLE_RD_DEST_ADDR_47_32", 0xa0f4 + i * 0x100, 0xffff, 0, 16)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_ETH_DU_TABLE_RD_VLAN_ID():
    for i in range(0, C_ETH_W):
        do_ro_reg("ETH_DU_TABLE_RD_VLAN_ID", 0xa0f8 + i * 0x100, 0xfff, 0, 12)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_ETH_DU_TABLE_RD_VLAN_DEI():
    for i in range(0, C_ETH_W):
        do_ro_reg("ETH_DU_TABLE_RD_VLAN_DEI", 0xa0f8 + i * 0x100, 0x1000, 12, 1)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_ETH_DU_TABLE_RD_VLAN_PCP():
    for i in range(0, C_ETH_W):
        do_ro_reg("ETH_DU_TABLE_RD_VLAN_PCP", 0xa0f8 + i * 0x100, 0xe000, 13, 3)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_ETH_DU_TABLE_RD_STROBE():
    for i in range(0, C_ETH_W):
        do_wo_reg("ETH_DU_TABLE_RD_STROBE", 0xa0fc + i * 0x100, 0x80000000, 31, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ETH_DU_TABLE_RD_TABLE_ADDR():
    for i in range(0, C_ETH_W):
        do_rw_reg("ETH_DU_TABLE_RD_TABLE_ADDR", 0xa0fc + i * 0x100, 0xf, 0, 4)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_L", 0xc000 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_H", 0xc004 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_L", 0xc008 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_H", 0xc00c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_L", 0xc010 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_H", 0xc014 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ETH_STATS_TOTAL_RX_BIT_RATE():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ETH_STATS_TOTAL_RX_BIT_RATE", 0xc018 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ETH_STATS_ORAN_RX_BIT_RATE():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ETH_STATS_ORAN_RX_BIT_RATE", 0xc01c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_TOTAL_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_TOTAL_L", 0xc020 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_TOTAL_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_TOTAL_H", 0xc024 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_ON_TIME_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_ON_TIME_L", 0xc028 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_ON_TIME_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_ON_TIME_H", 0xc02c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_EARLY_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_EARLY_L", 0xc030 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_EARLY_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_EARLY_H", 0xc034 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_LATE_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_LATE_L", 0xc038 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_LATE_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_LATE_H", 0xc03c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_TOTAL_C_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_TOTAL_C_L", 0xc040 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_TOTAL_C_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_TOTAL_C_H", 0xc044 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_ON_TIME_C_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_ON_TIME_C_L", 0xc048 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_ON_TIME_C_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_ON_TIME_C_H", 0xc04c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_EARLY_C_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_EARLY_C_L", 0xc050 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_EARLY_C_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_EARLY_C_H", 0xc054 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_LATE_C_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_LATE_C_L", 0xc058 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_LATE_C_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_LATE_C_H", 0xc05c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_CORRUPT_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_CORRUPT_L", 0xc060 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_CORRUPT_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_CORRUPT_H", 0xc064 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_ERROR_DROP_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_ERROR_DROP_L", 0xc068 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_RX_ERROR_DROP_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_RX_ERROR_DROP_H", 0xc06c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_TX_TOTAL_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_TX_TOTAL_L", 0xc070 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_TX_TOTAL_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_TX_TOTAL_H", 0xc074 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_TX_TOTAL_C_L():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_TX_TOTAL_C_L", 0xc078 + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_ORAN_TX_TOTAL_C_H():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_ORAN_TX_TOTAL_C_H", 0xc07c + i * 0x100, 0xffffffff, 0, 32)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_OFFSET_EARLIEST_U_PKT():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_OFFSET_EARLIEST_U_PKT", 0xc080 + i * 0x100, 0xfff, 0, 12)


@pytest.mark.skipif(not RO, reason="r/o test deselected")
def test_ro_reg_STATS_OFFSET_EARLIEST_C_PKT():
    for i in range(0, C_ETH_W):
        do_ro_reg("STATS_OFFSET_EARLIEST_C_PKT", 0xc084 + i * 0x100, 0xfff, 0, 12)


@pytest.mark.skipif(not WO, reason="w/o test deselected")
def test_wo_reg_ORAN_CC_RELOAD():
    do_wo_reg("ORAN_CC_RELOAD", 0xe000, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_ENABLE():
    do_rw_reg("ORAN_CC_ENABLE", 0xe004, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_SYMPERSLOT():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_SYMPERSLOT", 0xe100 + i * 0x70, 0x1000000, 24, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_NUMEROLOGY():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_NUMEROLOGY", 0xe100 + i * 0x70, 0x70000, 16, 3)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_NUMRBS():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_NUMRBS", 0xe100 + i * 0x70, 0x1ff, 0, 9)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_CTRL_OFFSETS():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_DL_CTRL_OFFSETS", 0xe104 + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_CTRL_UNROLLED_OFFSETS():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_DL_CTRL_UNROLLED_OFFSETS", 0xe108 + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_CTRL_OFFSETS():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_CTRL_OFFSETS", 0xe10c + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_CTRL_UNROLLED_OFFSETS():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_CTRL_UNROLLED_OFFSETS", 0xe110 + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_DATA_SYM_START_INDEX():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_DL_DATA_SYM_START_INDEX", 0xe114 + i * 0x70, 0x3f, 0, 6)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_DATA_SYM_NUM_INDEX():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_DL_DATA_SYM_NUM_INDEX", 0xe114 + i * 0x70, 0x3f00, 8, 6)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_CTRL_SYM_NUM_INDEX():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_DL_CTRL_SYM_NUM_INDEX", 0xe114 + i * 0x70, 0x3f0000, 16, 6)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_CTRL_SYM_NUM_INDEX():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_CTRL_SYM_NUM_INDEX", 0xe114 + i * 0x70, 0x3f000000, 24, 6)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_UD_IQ_WIDTH():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_UD_IQ_WIDTH", 0xe118 + i * 0x70, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_UD_COMP_METH():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_UD_COMP_METH", 0xe118 + i * 0x70, 0xf0, 4, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_MPLANE_UDCOMP_HDR_SEL():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_MPLANE_UDCOMP_HDR_SEL", 0xe118 + i * 0x70, 0x100, 8, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_UD_IQ_WIDTH():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_DL_UD_IQ_WIDTH", 0xe11c + i * 0x70, 0xf, 0, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_UD_COMP_METH():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_DL_UD_COMP_METH", 0xe11c + i * 0x70, 0xf0, 4, 4)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_MPLANE_UDCOMP_HDR_SEL():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_DL_MPLANE_UDCOMP_HDR_SEL", 0xe11c + i * 0x70, 0x100, 8, 1)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_SETUP_C_ABS_SYMBOL():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_SETUP_C_ABS_SYMBOL", 0xe120 + i * 0x70, 0xfff, 0, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_SETUP_C_CYCLES():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_SETUP_C_CYCLES", 0xe124 + i * 0x70, 0x1ffff, 0, 17)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_SETUP_D_CYCLES():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_SETUP_D_CYCLES", 0xe128 + i * 0x70, 0x1ffff, 0, 17)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_SETUP_C_ABS_SYMBOL():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_DL_SETUP_C_ABS_SYMBOL", 0xe130 + i * 0x70, 0xfff, 0, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_SETUP_C_CYCLES():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_DL_SETUP_C_CYCLES", 0xe134 + i * 0x70, 0x1ffff, 0, 17)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_SETUP_D_CYCLES():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_DL_SETUP_D_CYCLES", 0xe138 + i * 0x70, 0x1ffff, 0, 17)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_BASE_OFFSET():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_BASE_OFFSET", 0xe140 + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_BIDF_C_ABS_SYMBOL():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_BIDF_C_ABS_SYMBOL", 0xe144 + i * 0x70, 0xfff, 0, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_UL_BIDF_C_CYCLES():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_UL_BIDF_C_CYCLES", 0xe148 + i * 0x70, 0x1ffff, 0, 17)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_MAX_SYMBOLS():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_MAX_SYMBOLS", 0xe158 + i * 0x70, 0xfff, 0, 12)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_NUM_CTRL_PER_SYMBOL_DL():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_NUM_CTRL_PER_SYMBOL_DL", 0xe160 + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_NUM_CTRL_PER_SYMBOL_UL():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_NUM_CTRL_PER_SYMBOL_UL", 0xe164 + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_MODVALS_DL():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_MODVALS_DL", 0xe168 + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_MODVALS_UL():
    for i in range(0, C_MAX_COMP_CARR):
        do_rw_reg("ORAN_CC_MODVALS_UL", 0xe16c + i * 0x70, 0xffff, 0, 16)


@pytest.mark.skipif(not RW, reason="r/w test deselected")
def test_rw_reg_ORAN_CC_DL_DATA_UNROLL_OFFSET():
    for i in range(0, C_MAX_DL_SYMS):
        do_rw_reg("ORAN_CC_DL_DATA_UNROLL_OFFSET", 0xe800 + i * 0x4, 0xffff, 0, 16)
