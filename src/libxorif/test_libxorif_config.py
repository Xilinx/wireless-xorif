#!/usr/bin/env python3

import sys
import pytest
import logging
import pytest
from collections import namedtuple
from pprint import pprint

sys.path.append('/usr/share/xorif')
import pylibxorif

lib = pylibxorif.LIBXORIF()
lib.set_log_level(logging.DEBUG)
lib.xorif_init()
const = namedtuple("xorif_const", lib.constants.keys())(*lib.constants.values())
caps = lib.xorif_get_capabilities()

# Supported compression modes
comp_list = [(const.IQ_COMP_NONE, 0),
             (const.IQ_COMP_NONE, 16),
             (const.IQ_COMP_BLOCK_FP, 9),
             (const.IQ_COMP_BLOCK_FP, 12)]

def disable_all_cc():
    """Utility to disable all component carriers."""
    caps = lib.xorif_get_capabilities()
    for i in range(0, caps['max_cc']):
        assert lib.xorif_disable_cc(i) == const.XORIF_SUCCESS

def print_config(cc):
    """Utility to print the component carrier configuration."""
    result, config = lib.xorif_get_cc_config(cc, config)
    assert result == const.XORIF_SUCCESS
    print(f"CC[{cc}]")
    pprint(config)

def print_alloc(cc):
    """Utility to print the component carrier allocation (for debug)."""
    result, alloc = lib.xorif_get_fhi_cc_alloc(cc)
    assert result == const.XORIF_SUCCESS
    print(f"CC[{cc}]")
    pprint(alloc)

def default_config():
    # Configuration
    config = {}
    config['num_rbs'] = 20
    config['num_rbs_ssb'] = 20
    config['numerology'] = 0
    config['extended_cp'] = 0
    config['numerology_ssb'] = 0
    config['extended_cp_ssb'] = 0
    config['iq_comp_meth_ul'] = const.IQ_COMP_NONE
    config['iq_comp_width_ul'] = 0
    config['iq_comp_mplane_ul'] = 1
    config['iq_comp_meth_dl'] = const.IQ_COMP_NONE
    config['iq_comp_width_dl'] = 0
    config['iq_comp_mplane_dl'] = 1
    config['iq_comp_meth_ssb'] = const.IQ_COMP_NONE
    config['iq_comp_width_ssb'] = 0
    config['iq_comp_mplane_ssb'] = 1
    config['delay_comp_cp_ul'] = 50
    config['delay_comp_cp_dl'] = 50
    config['delay_comp_up'] = 50
    config['advance_ul'] = 25
    config['advance_dl'] = 25
    config['ul_bid_forward'] = 25
    config['ul_radio_ch_dly'] = 30
    config['num_ctrl_per_sym_ul'] = 10
    config['num_ctrl_per_sym_dl'] = 10
    config['num_ctrl_per_sym_ssb'] = 5
    config['num_sect_per_sym'] = 10
    config['num_sect_per_sym_ssb'] = 5
    config['num_frames_per_sym'] = 15
    config['num_frames_per_sym_ssb'] = 3
    return config

def test_config_cc():
    """Check various fail cases for the configure API."""
    assert lib.xorif_get_state() == 1
    caps = lib.xorif_get_capabilities()

    # Ensure any existing component carriers are disabled
    disable_all_cc()

    # Configuration
    config = default_config()

    # Set configuration
    assert lib.xorif_set_cc_config(caps['max_cc'], config) == const.XORIF_INVALID_CC
    config['num_rbs'] = 0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_INVALID_RBS
    config['num_rbs'] = 276
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_INVALID_RBS
    config['num_rbs'] = 20
    config['numerology'] = 2
    config['extended_cp'] = 1
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED
    config['extended_cp'] = 0
    config['numerology_ssb'] = 2
    config['extended_cp_ssb'] = 1
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED
    config['extended_cp_ssb'] = 0
    config['iq_comp_meth_ul'] = const.IQ_COMP_BLOCK_FP
    config['iq_comp_width_ul'] = 17
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_COMP_MODE_NOT_SUPPORTED
    config['iq_comp_meth_ul'] = const.IQ_COMP_NONE
    config['iq_comp_width_ul'] = 0
    config['iq_comp_meth_dl'] = const.IQ_COMP_BLOCK_FP
    config['iq_comp_width_dl'] = 17
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_COMP_MODE_NOT_SUPPORTED
    config['iq_comp_meth_ul'] = const.IQ_COMP_NONE
    config['iq_comp_width_ul'] = 0
    config['iq_comp_meth_dl'] = const.IQ_COMP_NONE
    config['iq_comp_width_dl'] = 0
    config['iq_comp_meth_ssb'] = const.IQ_COMP_BLOCK_FP
    config['iq_comp_width_ssb'] = 17
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_COMP_MODE_NOT_SUPPORTED

    # Configure
    assert lib.xorif_configure_cc(caps['max_cc']) == const.XORIF_INVALID_CC

    # Get alloc tests
    result, alloc = lib.xorif_get_fhi_cc_alloc(caps['max_cc'])
    assert result == const.XORIF_INVALID_CC
    result, alloc = lib.xorif_get_fhi_cc_alloc(0)
    assert result == const.XORIF_SUCCESS

def test_config_cc_1():
    """Check the component carrier configuration APIs."""
    # Note, this test does perform the "configure_cc".
    # As as such relies on sufficient memory resources in the h/w, etc.
    # Note, this test is not compatible with beamformer
    assert lib.xorif_get_state() == 1

    # Ensure any existing component carriers are disabled
    disable_all_cc()

    # Configuration
    config = default_config()

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Get the actual buffer allocation, and verify it is correct
    result, alloc = lib.xorif_get_fhi_cc_alloc(0)
    assert result == const.XORIF_SUCCESS
    assert alloc['ul_ctrl_sym_num'] == 2
    assert alloc['dl_ctrl_sym_num'] == 2
    assert alloc['dl_data_sym_num'] == 1
    assert alloc['ssb_ctrl_sym_num'] == 2
    assert alloc['ssb_data_sym_num'] == 1
    assert alloc['ul_ctrl_offset'] == 0
    assert alloc['ul_ctrl_size'] == 10 * 2
    assert alloc['ul_ctrl_base_offset'] == 0
    assert alloc['ul_ctrl_base_size'] == 20
    assert alloc['dl_ctrl_offset'] == 0
    assert alloc['dl_ctrl_size'] == 10 * 2
    assert alloc['dl_data_ptrs_offset'] == 0
    assert alloc['dl_data_buff_offset'] == 0
    assert alloc['dl_data_buff_size'] == 146 * 1
    assert alloc['ssb_ctrl_offset'] == 0
    assert alloc['ssb_ctrl_size'] == 5 * 2
    assert alloc['ssb_data_ptrs_offset'] == 0
    assert alloc['ssb_data_buff_offset'] == 0
    assert alloc['ssb_data_buff_size'] == 127 * 1

@pytest.mark.skipif(not(caps['iq_de_comp_methods'] & 0x2), reason = "Compression mode not supported")
def test_config_cc_1b():
    """Check the component carrier configuration APIs."""
    # Using static compression for DL and SSB
    # Note, this test does perform the "configure_cc".
    # As as such relies on sufficient memory resources in the h/w, etc.
    # Note, this test is not compatible with beamformer
    assert lib.xorif_get_state() == 1

    # Ensure any existing component carriers are disabled
    disable_all_cc()

    # Configuration
    config = default_config()
    config['iq_comp_meth_dl'] = const.IQ_COMP_BLOCK_FP
    config['iq_comp_width_dl'] = 9
    config['iq_comp_mplane_dl'] = 1
    config['iq_comp_meth_ssb'] = const.IQ_COMP_BLOCK_FP
    config['iq_comp_width_ssb'] = 9
    config['iq_comp_mplane_ssb'] = 1

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Get the actual buffer allocation, and verify it is correct
    result, alloc = lib.xorif_get_fhi_cc_alloc(0)
    assert result == const.XORIF_SUCCESS
    assert alloc['ul_ctrl_sym_num'] == 2
    assert alloc['dl_ctrl_sym_num'] == 2
    assert alloc['dl_data_sym_num'] == 1
    assert alloc['ssb_ctrl_sym_num'] == 2
    assert alloc['ssb_data_sym_num'] == 1
    assert alloc['ul_ctrl_offset'] == 0
    assert alloc['ul_ctrl_size'] == 10 * 2
    assert alloc['ul_ctrl_base_offset'] == 0
    assert alloc['ul_ctrl_base_size'] == 20
    assert alloc['dl_ctrl_offset'] == 0
    assert alloc['dl_ctrl_size'] == 10 * 2
    assert alloc['dl_data_ptrs_offset'] == 0
    assert alloc['dl_data_buff_offset'] == 0
    assert alloc['dl_data_buff_size'] == 96 * 1
    assert alloc['ssb_ctrl_offset'] == 0
    assert alloc['ssb_ctrl_size'] == 5 * 2
    assert alloc['ssb_data_ptrs_offset'] == 0
    assert alloc['ssb_data_buff_offset'] == 0
    assert alloc['ssb_data_buff_size'] == 77 * 1

@pytest.mark.skipif(not(caps['iq_de_comp_methods'] & 0x2), reason = "Compression mode not supported")
def test_config_cc_1c():
    """Check the component carrier configuration APIs."""
    # Using dynamic compression for DL and SSB
    # Note, this test does perform the "configure_cc".
    # As as such relies on sufficient memory resources in the h/w, etc.
    # Note, this test is not compatible with beamformer
    assert lib.xorif_get_state() == 1

    # Ensure any existing component carriers are disabled
    disable_all_cc()

    # Configuration
    config = default_config()
    config['iq_comp_meth_dl'] = const.IQ_COMP_BLOCK_FP
    config['iq_comp_width_dl'] = 9
    config['iq_comp_mplane_dl'] = 0
    config['iq_comp_meth_ssb'] = const.IQ_COMP_BLOCK_FP
    config['iq_comp_width_ssb'] = 9
    config['iq_comp_mplane_ssb'] = 0

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Get the actual buffer allocation, and verify it is correct
    result, alloc = lib.xorif_get_fhi_cc_alloc(0)
    assert result == const.XORIF_SUCCESS
    assert alloc['ul_ctrl_sym_num'] == 2
    assert alloc['dl_ctrl_sym_num'] == 2
    assert alloc['dl_data_sym_num'] == 1
    assert alloc['ssb_ctrl_sym_num'] == 2
    assert alloc['ssb_data_sym_num'] == 1
    assert alloc['ul_ctrl_offset'] == 0
    assert alloc['ul_ctrl_size'] == 10 * 2
    assert alloc['ul_ctrl_base_offset'] == 0
    assert alloc['ul_ctrl_base_size'] == 20
    assert alloc['dl_ctrl_offset'] == 0
    assert alloc['dl_ctrl_size'] == 10 * 2
    assert alloc['dl_data_ptrs_offset'] == 0
    assert alloc['dl_data_buff_offset'] == 0
    assert alloc['dl_data_buff_size'] == 99 * 1
    assert alloc['ssb_ctrl_offset'] == 0
    assert alloc['ssb_ctrl_size'] == 5 * 2
    assert alloc['ssb_data_ptrs_offset'] == 0
    assert alloc['ssb_data_buff_offset'] == 0
    assert alloc['ssb_data_buff_size'] == 78 * 1

@pytest.mark.skipif(caps['max_cc'] < 2, reason = "Insufficient component carrier supported")
def test_config_cc_2():
    """Check the component carrier configuration APIs."""
    # Note, this test does perform the "configure_cc".
    # As as such relies on sufficient memory resources in the h/w, etc.
    # Note, this test is not compatible with beamformer
    assert lib.xorif_get_state() == 1

    # Ensure any existing component carriers are disabled
    disable_all_cc()

    # Configuration
    config = default_config()

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Configure CC#1
    assert lib.xorif_set_cc_config(1, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(1) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(1) == const.XORIF_SUCCESS
    print_alloc(1)

    # Get the actual buffer allocation, and verify it is correct
    result, alloc = lib.xorif_get_fhi_cc_alloc(1)
    assert result == const.XORIF_SUCCESS
    assert alloc['ul_ctrl_sym_num'] == 2
    assert alloc['dl_ctrl_sym_num'] == 2
    assert alloc['dl_data_sym_num'] == 1
    assert alloc['ssb_ctrl_sym_num'] == 2
    assert alloc['ssb_data_sym_num'] == 1
    assert alloc['ul_ctrl_offset'] == 20
    assert alloc['ul_ctrl_size'] == 10 * 2
    assert alloc['ul_ctrl_base_offset'] == 20
    assert alloc['ul_ctrl_base_size'] == 20
    assert alloc['dl_ctrl_offset'] == 20
    assert alloc['dl_ctrl_size'] == 10 * 2
    assert alloc['dl_data_ptrs_offset'] == 1
    assert alloc['dl_data_buff_offset'] == 146
    assert alloc['dl_data_buff_size'] == 146 * 1
    assert alloc['ssb_ctrl_offset'] == 10
    assert alloc['ssb_ctrl_size'] == 5 * 2
    assert alloc['ssb_data_ptrs_offset'] == 1
    assert alloc['ssb_data_buff_offset'] == 127
    assert alloc['ssb_data_buff_size'] == 127 * 1

@pytest.mark.skipif(caps['max_cc'] < 3, reason = "Insufficient component carrier supported")
def test_config_cc_3():
    """Check the component carrier configuration APIs."""
    # Note, this test does perform the "configure_cc".
    # As as such relies on sufficient memory resources in the h/w, etc.
    # Note, this test is not compatible with beamformer
    assert lib.xorif_get_state() == 1

    # Ensure any existing component carriers are disabled
    disable_all_cc()

    # Configuration
    config = default_config()

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Configure CC#1
    assert lib.xorif_set_cc_config(1, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(1) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(1) == const.XORIF_SUCCESS
    print_alloc(1)

    # Configure CC#2
    assert lib.xorif_set_cc_config(2, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(2) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(2) == const.XORIF_SUCCESS
    print_alloc(2)

    # Get the actual buffer allocation, and verify it is correct
    result, alloc = lib.xorif_get_fhi_cc_alloc(2)
    assert result == const.XORIF_SUCCESS
    assert alloc['ul_ctrl_sym_num'] == 2
    assert alloc['dl_ctrl_sym_num'] == 2
    assert alloc['dl_data_sym_num'] == 1
    assert alloc['ssb_ctrl_sym_num'] == 2
    assert alloc['ssb_data_sym_num'] == 1
    assert alloc['ul_ctrl_offset'] == 40
    assert alloc['ul_ctrl_size'] == 10 * 2
    assert alloc['ul_ctrl_base_offset'] == 40
    assert alloc['ul_ctrl_base_size'] == 20
    assert alloc['dl_ctrl_offset'] == 40
    assert alloc['dl_ctrl_size'] == 10 * 2
    assert alloc['dl_data_ptrs_offset'] == 2
    assert alloc['dl_data_buff_offset'] == 146 + 146
    assert alloc['dl_data_buff_size'] == 146 * 1
    assert alloc['ssb_ctrl_offset'] == 20
    assert alloc['ssb_ctrl_size'] == 5 * 2
    assert alloc['ssb_data_ptrs_offset'] == 2
    assert alloc['ssb_data_buff_offset'] == 127 + 127
    assert alloc['ssb_data_buff_size'] == 127 * 1

@pytest.mark.skipif(caps['max_cc'] < 4, reason = "Insufficient component carrier supported")
@pytest.mark.skipif(caps['max_data_symbols'] < 8, reason = "Insufficient symbol space")
def test_config_cc_4():
    """Check the component carrier configuration APIs."""
    # Note, this test does perform the "configure_cc".
    # As as such relies on sufficient memory resources in the h/w, etc.
    # Note, this test is not compatible with beamformer
    assert lib.xorif_get_state() == 1

    # Ensure any existing component carriers are disabled
    disable_all_cc()

    # Configuration
    config = default_config()

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Configure CC#1
    assert lib.xorif_set_cc_config(1, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(1) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(1) == const.XORIF_SUCCESS
    print_alloc(1)

    # Configure CC#2
    assert lib.xorif_set_cc_config(2, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(2) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(2) == const.XORIF_SUCCESS
    print_alloc(2)

    # Configure CC#3
    assert lib.xorif_set_cc_config(3, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(3) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(3) == const.XORIF_SUCCESS
    print_alloc(3)

    # Get the actual buffer allocation, and verify it is correct
    result, alloc = lib.xorif_get_fhi_cc_alloc(3)
    assert result == const.XORIF_SUCCESS
    assert alloc['ul_ctrl_sym_num'] == 2
    assert alloc['dl_ctrl_sym_num'] == 2
    assert alloc['dl_data_sym_num'] == 1
    assert alloc['ssb_ctrl_sym_num'] == 2
    assert alloc['ssb_data_sym_num'] == 1
    assert alloc['ul_ctrl_offset'] == 60
    assert alloc['ul_ctrl_size'] == 10 * 2
    assert alloc['ul_ctrl_base_offset'] == 60
    assert alloc['ul_ctrl_base_size'] == 20
    assert alloc['dl_ctrl_offset'] == 60
    assert alloc['dl_ctrl_size'] == 10 * 2
    assert alloc['dl_data_ptrs_offset'] == 3
    assert alloc['dl_data_buff_offset'] == 146 + 146 + 146
    assert alloc['dl_data_buff_size'] == 146 * 1
    assert alloc['ssb_ctrl_offset'] == 30
    assert alloc['ssb_ctrl_size'] == 5 * 2
    assert alloc['ssb_data_ptrs_offset'] == 3
    assert alloc['ssb_data_buff_offset'] == 127 + 127 + 127
    assert alloc['ssb_data_buff_size'] == 127 * 1

@pytest.mark.skipif(caps['max_cc'] < 2, reason = "Insufficient component carrier supported")
def test_reconfig_cc_2():
    """Check the component carrier configuration APIs."""
    # Note, this test does perform the "configure_cc".
    # As as such relies on sufficient memory resources in the h/w, etc.
    # Note, this test is not compatible with beamformer
    assert lib.xorif_get_state() == 1

    # Ensure any existing component carriers are disabled
    disable_all_cc()

    # Configuration
    config = default_config()

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Configure CC#1
    assert lib.xorif_set_cc_config(1, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(1) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(1) == const.XORIF_SUCCESS
    print_alloc(1)

    # Now reconfigure CC#0 to have more RBs + UL sections
    config['num_rbs'] = 30
    config['num_ctrl_per_sym_ul'] = 15
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Get the actual buffer allocation, and verify it is correct
    result, alloc = lib.xorif_get_fhi_cc_alloc(0)
    assert result == const.XORIF_SUCCESS
    assert alloc['ul_ctrl_sym_num'] == 2
    assert alloc['dl_ctrl_sym_num'] == 2
    assert alloc['dl_data_sym_num'] == 1
    assert alloc['ssb_ctrl_sym_num'] == 2
    assert alloc['ssb_data_sym_num'] == 1
    assert alloc['ul_ctrl_offset'] == 40
    assert alloc['ul_ctrl_size'] == 15 * 2
    assert alloc['ul_ctrl_base_offset'] == 40
    assert alloc['ul_ctrl_base_size'] == 30
    assert alloc['dl_ctrl_offset'] == 0
    assert alloc['dl_ctrl_size'] == 10 * 2
    assert alloc['dl_data_ptrs_offset'] == 0
    assert alloc['dl_data_buff_offset'] == 146 + 146
    assert alloc['dl_data_buff_size'] == 206 * 1
    assert alloc['ssb_ctrl_offset'] == 0
    assert alloc['ssb_ctrl_size'] == 5 * 2
    assert alloc['ssb_data_ptrs_offset'] == 0
    assert alloc['ssb_data_buff_offset'] == 0
    assert alloc['ssb_data_buff_size'] == 127 * 1

@pytest.mark.skipif(caps['max_cc'] < 3, reason = "Insufficient component carrier supported")
def test_reverse_config_cc_3():
    """Check the component carrier configuration APIs."""
    # Note, this test does perform the "configure_cc".
    # As as such relies on sufficient memory resources in the h/w, etc.
    # Note, this test is not compatible with beamformer
    assert lib.xorif_get_state() == 1

    # Ensure any existing component carriers are disabled
    disable_all_cc()

    # Configuration
    config = default_config()

    # Configure CC#2
    assert lib.xorif_set_cc_config(2, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(2) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(2) == const.XORIF_SUCCESS
    print_alloc(2)

    # Configure CC#1
    assert lib.xorif_set_cc_config(1, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(1) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(1) == const.XORIF_SUCCESS
    print_alloc(1)

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Get the actual buffer allocation, and verify it is correct
    result, alloc = lib.xorif_get_fhi_cc_alloc(0)
    assert result == const.XORIF_SUCCESS
    assert alloc['ul_ctrl_sym_num'] == 2
    assert alloc['dl_ctrl_sym_num'] == 2
    assert alloc['dl_data_sym_num'] == 1
    assert alloc['ssb_ctrl_sym_num'] == 2
    assert alloc['ssb_data_sym_num'] == 1
    assert alloc['ul_ctrl_offset'] == 40
    assert alloc['ul_ctrl_size'] == 10 * 2
    assert alloc['ul_ctrl_base_offset'] == 40
    assert alloc['ul_ctrl_base_size'] == 20
    assert alloc['dl_ctrl_offset'] == 40
    assert alloc['dl_ctrl_size'] == 10 * 2
    assert alloc['dl_data_ptrs_offset'] == 2
    assert alloc['dl_data_buff_offset'] == 146 + 146
    assert alloc['dl_data_buff_size'] == 146 * 1
    assert alloc['ssb_ctrl_offset'] == 20
    assert alloc['ssb_ctrl_size'] == 5 * 2
    assert alloc['ssb_data_ptrs_offset'] == 2
    assert alloc['ssb_data_buff_offset'] == 127 + 127
    assert alloc['ssb_data_buff_size'] == 127 * 1

@pytest.mark.skipif(caps['max_cc'] < 3, reason = "Insufficient component carrier supported")
def test_reorder_config_cc_3():
    """Check the component carrier configuration APIs."""
    # Note, this test does perform the "configure_cc".
    # As as such relies on sufficient memory resources in the h/w, etc.
    # Note, this test is not compatible with beamformer
    assert lib.xorif_get_state() == 1

    # Ensure any existing component carriers are disabled
    disable_all_cc()

    # Configuration
    config = default_config()

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Configure CC#1
    assert lib.xorif_set_cc_config(1, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(1) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(1) == const.XORIF_SUCCESS
    print_alloc(1)

    # Configure CC#2
    assert lib.xorif_set_cc_config(2, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(2) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(2) == const.XORIF_SUCCESS
    print_alloc(2)

    # Disable CC#0 & CC#2
    assert lib.xorif_disable_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_disable_cc(2) == const.XORIF_SUCCESS

    # Configure CC#2
    assert lib.xorif_set_cc_config(2, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(2) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(2) == const.XORIF_SUCCESS
    print_alloc(2)

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Get the actual buffer allocation, and verify it is correct
    result, alloc = lib.xorif_get_fhi_cc_alloc(0)
    assert result == const.XORIF_SUCCESS
    assert alloc['ul_ctrl_sym_num'] == 2
    assert alloc['dl_ctrl_sym_num'] == 2
    assert alloc['dl_data_sym_num'] == 1
    assert alloc['ssb_ctrl_sym_num'] == 2
    assert alloc['ssb_data_sym_num'] == 1
    assert alloc['ul_ctrl_offset'] == 40
    assert alloc['ul_ctrl_size'] == 10 * 2
    assert alloc['ul_ctrl_base_offset'] == 40
    assert alloc['ul_ctrl_base_size'] == 20
    assert alloc['dl_ctrl_offset'] == 40
    assert alloc['dl_ctrl_size'] == 10 * 2
    assert alloc['dl_data_ptrs_offset'] == 2
    assert alloc['dl_data_buff_offset'] == 146 + 146
    assert alloc['dl_data_buff_size'] == 146 * 1
    assert alloc['ssb_ctrl_offset'] == 20
    assert alloc['ssb_ctrl_size'] == 5 * 2
    assert alloc['ssb_data_ptrs_offset'] == 2
    assert alloc['ssb_data_buff_offset'] == 127 + 127
    assert alloc['ssb_data_buff_size'] == 127 * 1

@pytest.mark.skipif(caps['max_cc'] < 4, reason = "Insufficient component carrier supported")
@pytest.mark.skipif(caps['max_data_symbols'] < 8, reason = "Insufficient symbol space")
def test_reorder_config_cc_4():
    """Check the component carrier configuration APIs."""
    # Note, this test does perform the "configure_cc".
    # As as such relies on sufficient memory resources in the h/w, etc.
    # Note, this test is not compatible with beamformer
    assert lib.xorif_get_state() == 1

    # Ensure any existing component carriers are disabled
    disable_all_cc()

    # Configuration
    config = default_config()

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Configure CC#1
    assert lib.xorif_set_cc_config(1, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(1) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(1) == const.XORIF_SUCCESS
    print_alloc(1)

    # Configure CC#2
    assert lib.xorif_set_cc_config(2, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(2) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(2) == const.XORIF_SUCCESS
    print_alloc(2)

    # Configure CC#3
    assert lib.xorif_set_cc_config(3, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(3) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(3) == const.XORIF_SUCCESS
    print_alloc(3)

    # Disable CC#0 & CC#3
    assert lib.xorif_disable_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_disable_cc(3) == const.XORIF_SUCCESS

    # Configure CC#3
    assert lib.xorif_set_cc_config(3, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(3) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(3) == const.XORIF_SUCCESS
    print_alloc(3)

    # Configure CC#0
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    print_alloc(0)

    # Get the actual buffer allocation, and verify it is correct
    result, alloc = lib.xorif_get_fhi_cc_alloc(0)
    assert result == const.XORIF_SUCCESS
    assert alloc['ul_ctrl_sym_num'] == 2
    assert alloc['dl_ctrl_sym_num'] == 2
    assert alloc['dl_data_sym_num'] == 1
    assert alloc['ssb_ctrl_sym_num'] == 2
    assert alloc['ssb_data_sym_num'] == 1
    assert alloc['ul_ctrl_offset'] == 60
    assert alloc['ul_ctrl_size'] == 10 * 2
    assert alloc['ul_ctrl_base_offset'] == 60
    assert alloc['ul_ctrl_base_size'] == 20
    assert alloc['dl_ctrl_offset'] == 60
    assert alloc['dl_ctrl_size'] == 10 * 2
    assert alloc['dl_data_ptrs_offset'] == 3
    assert alloc['dl_data_buff_offset'] == 146 + 146 + 146
    assert alloc['dl_data_buff_size'] == 146 * 1
    assert alloc['ssb_ctrl_offset'] == 30
    assert alloc['ssb_ctrl_size'] == 5 * 2
    assert alloc['ssb_data_ptrs_offset'] == 3
    assert alloc['ssb_data_buff_offset'] == 127 + 127 + 127
    assert alloc['ssb_data_buff_size'] == 127 * 1