#!/usr/bin/env python3

import sys
import pytest
import logging
import pytest
from collections import namedtuple

sys.path.append('/usr/share/xorif')
import pylibxorif

lib = pylibxorif.LIBXORIF()
lib.set_log_level(logging.DEBUG)
lib.xorif_init()
const = namedtuple("xorif_const", lib.constants.keys())(*lib.constants.values())
caps = lib.xorif_get_capabilities()

# Supported compression modes
dl_comp_list = [(const.IQ_COMP_NONE, 0),
                (const.IQ_COMP_NONE, 8),
                (const.IQ_COMP_NONE, 9),
                (const.IQ_COMP_NONE, 12),
                (const.IQ_COMP_NONE, 16),
                (const.IQ_COMP_BLOCK_FP, 9),
                (const.IQ_COMP_BLOCK_FP, 12),
                (const.IQ_COMP_BLOCK_FP, 14),
                (const.IQ_COMP_MODULATION, 1),
                (const.IQ_COMP_MODULATION, 2),
                (const.IQ_COMP_MODULATION, 3),
                (const.IQ_COMP_MODULATION, 4),
                (const.IQ_COMP_MODULATION, 5)]

ul_comp_list = [(const.IQ_COMP_NONE, 0),
                (const.IQ_COMP_NONE, 8),
                (const.IQ_COMP_NONE, 9),
                (const.IQ_COMP_NONE, 12),
                (const.IQ_COMP_NONE, 16),
                (const.IQ_COMP_BLOCK_FP, 9),
                (const.IQ_COMP_BLOCK_FP, 12),
                (const.IQ_COMP_BLOCK_FP, 14)]

# Some un-supported compression modes
bad_comp_list = [(const.IQ_COMP_NONE, 17),
                 (const.IQ_COMP_BLOCK_FP, 17),
                 (const.IQ_COMP_MODULATION, 0),
                 (const.IQ_COMP_MODULATION, 6),
                 (const.IQ_COMP_BLOCK_SCALE, 12),
                 (const.IQ_COMP_U_LAW, 12)]

def test_debug_api():
    """Check debug modes."""
    lib.xorif_debug(0)
    lib.xorif_debug(2)

def test_init_api():
    """Check library finish/close API."""
    lib.xorif_finish()
    assert lib.xorif_get_state() == 0

    # Inititialize with name
    assert lib.xorif_init("oran_radio") == const.XORIF_SUCCESS
    assert lib.xorif_get_state() == 1
    lib.xorif_finish()

    # Initialize without name
    assert lib.xorif_init() == const.XORIF_SUCCESS
    assert lib.xorif_get_state() == 1

    # Initialize when already initialized
    assert lib.xorif_init() == const.XORIF_SUCCESS
    assert lib.xorif_get_state() == 1

def test_versions_api():
    """Check the get version APIs."""
    assert lib.xorif_get_state() == 1

    # S/W version
    sw_version = lib.xorif_get_sw_version()
    print(sw_version)

    # FHI versions
    fhi_version = lib.xorif_get_fhi_hw_version()
    print(fhi_version)
    fhi_revision = lib.xorif_get_fhi_hw_internal_rev()
    print(fhi_revision)

def test_capabilities_api():
    """Check the get capabilities APIs."""
    assert lib.xorif_get_state() == 1

    # General capabilities...

    print(caps)
    assert caps['max_cc'] >= 1 and caps['max_cc'] <= 8
    assert caps['num_eth_ports'] >= 1 and caps['num_eth_ports'] <= 4
    assert caps['numerologies'] != 0
    assert caps['extended_cp'] == 0
    assert caps['iq_de_comp_methods'] != 0
    assert caps['iq_comp_methods'] != 0
    assert caps['no_framer_ss'] >= 0
    assert caps['no_deframer_ss'] >= 0
    assert caps['max_subcarriers'] >= 3300
    # Note, difficult to check values, since they will change

def test_has_miscellaneous_api():
    """Check various miscellaneous APIs."""
    assert lib.xorif_has_front_haul_interface() == 1
    assert lib.xorif_reset_fhi(0) == const.XORIF_SUCCESS

def test_num_rbs_api():
    """Test the number of RBs APIs."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_cc_num_rbs(caps['max_cc'], 0) == const.XORIF_INVALID_CC

    for r in [0, 276]:
        assert lib.xorif_set_cc_num_rbs(0, r) == const.XORIF_INVALID_RBS

    for r in [1, 2, 50, 100, 200, 273, 275]:
        assert lib.xorif_set_cc_num_rbs(0, r) == const.XORIF_SUCCESS

def test_numerology_api():
    """Test the numerology APIs."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_cc_numerology(caps['max_cc'], 0, 0) == const.XORIF_INVALID_CC

    for n in range(0, 5):
        if (caps['numerologies'] & 1<<n):
            if caps['extended_cp'] == 1:
                if n == 2:
                    # Extended CP only valid for numerology 2
                    assert lib.xorif_set_cc_numerology(0, n, 0) == const.XORIF_SUCCESS
                    assert lib.xorif_set_cc_numerology(0, n, 1) == const.XORIF_SUCCESS
                else:
                    assert lib.xorif_set_cc_numerology(0, n, 0) == const.XORIF_SUCCESS
                    assert lib.xorif_set_cc_numerology(0, n, 1) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED
            else:
                assert lib.xorif_set_cc_numerology(0, n, 0) == const.XORIF_SUCCESS
                assert lib.xorif_set_cc_numerology(0, n, 1) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED
        else:
            assert lib.xorif_set_cc_numerology(0, n, 0) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED
            assert lib.xorif_set_cc_numerology(0, n, 1) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED

def test_num_rbs_ssb_api():
    """Test the number of RBs (SSB) APIs."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_cc_num_rbs_ssb(caps['max_cc'], 0) == const.XORIF_INVALID_CC

    for r in [1, 2, 50, 100, 200, 273, 275]:
        assert lib.xorif_set_cc_num_rbs_ssb(0, r) == const.XORIF_INVALID_RBS

    for r in [0, 20]:
        assert lib.xorif_set_cc_num_rbs_ssb(0, r) == const.XORIF_SUCCESS

def test_numerology_ssb_api():
    """Test the numerology (SSB) APIs."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_cc_numerology_ssb(caps['max_cc'], 0, 0) == const.XORIF_INVALID_CC

    for n in range(0, 5):
        if (caps['numerologies'] & 1<<n):
            if caps['extended_cp'] == 1:
                if n == 2:
                    # Extended CP only valid for numerology 2
                    assert lib.xorif_set_cc_numerology_ssb(0, n, 0) == const.XORIF_SUCCESS
                    assert lib.xorif_set_cc_numerology_ssb(0, n, 1) == const.XORIF_SUCCESS
                else:
                    assert lib.xorif_set_cc_numerology_ssb(0, n, 0) == const.XORIF_SUCCESS
                    assert lib.xorif_set_cc_numerology_ssb(0, n, 1) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED
            else:
                assert lib.xorif_set_cc_numerology_ssb(0, n, 0) == const.XORIF_SUCCESS
                assert lib.xorif_set_cc_numerology_ssb(0, n, 1) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED
        else:
            assert lib.xorif_set_cc_numerology_ssb(0, n, 0) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED
            assert lib.xorif_set_cc_numerology_ssb(0, n, 1) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED

def test_iq_compression_api():
    """Test the IQ compression APIs."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_cc_dl_iq_compression(caps['max_cc'], 0, 0, 1) == const.XORIF_INVALID_CC
    assert lib.xorif_set_cc_ul_iq_compression(caps['max_cc'], 0, 0, 1) == const.XORIF_INVALID_CC

    # Supported downlink compression modes
    for m, w in dl_comp_list:
        if caps['iq_de_comp_methods'] & 1<<m:
            for mp in [0, 1]:
                assert lib.xorif_set_cc_dl_iq_compression(0, w, m, mp) == const.XORIF_SUCCESS

    # Supported uplink compression modes
    for m, w in ul_comp_list:
        if caps['iq_comp_methods'] & 1<<m:
            for mp in [0, 1]:
                assert lib.xorif_set_cc_ul_iq_compression(0, w, m, mp) == const.XORIF_SUCCESS

    # Some un-supported compression modes
    for m, w in bad_comp_list:
        for mp in [0, 1]:
            assert lib.xorif_set_cc_dl_iq_compression(0, w, m, mp) == const.XORIF_COMP_MODE_NOT_SUPPORTED
            assert lib.xorif_set_cc_ul_iq_compression(0, w, m, mp) == const.XORIF_COMP_MODE_NOT_SUPPORTED

def test_iq_compression_ssb_api():
    """Test the IQ compression (SSB) APIs."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_cc_iq_compression_ssb(caps['max_cc'], 0, 0, 1) == const.XORIF_INVALID_CC

    # Supported compression modes
    for m, w in dl_comp_list:
        if caps['iq_de_comp_methods'] & 1<<m:
            for mp in [0, 1]:
                assert lib.xorif_set_cc_iq_compression_ssb(0, w, m, mp) == const.XORIF_SUCCESS

    # Some un-supported compression modes
    for m, w in bad_comp_list:
        for mp in [0, 1]:
            assert lib.xorif_set_cc_iq_compression_ssb(0, w, m, mp) == const.XORIF_COMP_MODE_NOT_SUPPORTED

def test_iq_compression_prach_api():
    """Test the IQ compression (PRACH) APIs."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_cc_iq_compression_prach(caps['max_cc'], 0, 0, 1) == const.XORIF_INVALID_CC

    # Supported compression modes
    for m, w in ul_comp_list:
        if caps['iq_comp_methods'] & 1<<m:
            for mp in [0, 1]:
                assert lib.xorif_set_cc_iq_compression_prach(0, w, m, mp) == const.XORIF_SUCCESS

    # Some un-supported compression modes
    for m, w in bad_comp_list:
        for mp in [0, 1]:
            assert lib.xorif_set_cc_iq_compression_prach(0, w, m, mp) == const.XORIF_COMP_MODE_NOT_SUPPORTED

def test_time_advance_api():
    """Test time advance API."""
    assert lib.xorif_get_state() == 1

    # Deprecated API
    if True:
        assert lib.xorif_set_cc_time_advance(caps['max_cc'], 0, 0, 0) == const.XORIF_INVALID_CC
        assert lib.xorif_set_cc_time_advance(0, 30, 90, 90) == const.XORIF_SUCCESS

    assert lib.xorif_set_cc_ul_timing_parameters(caps['max_cc'], 0, 0, 0) == const.XORIF_INVALID_CC
    assert lib.xorif_set_cc_ul_timing_parameters(0, 100, 50, 30) == const.XORIF_SUCCESS

    assert lib.xorif_set_cc_dl_timing_parameters(caps['max_cc'], 0, 0, 0) == const.XORIF_INVALID_CC
    assert lib.xorif_set_cc_dl_timing_parameters(0, 100, 100, 50) == const.XORIF_SUCCESS

def test_sections_per_symbol_api():
    """Test sections per symbol API."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_cc_dl_sections_per_symbol(caps['max_cc'], 64, 64) == const.XORIF_INVALID_CC
    assert lib.xorif_set_cc_ul_sections_per_symbol(caps['max_cc'], 64, 64) == const.XORIF_INVALID_CC

    for s in [0, 5, 10, 20, 64, 100, 275]:
        assert lib.xorif_set_cc_dl_sections_per_symbol(0, s, s) == const.XORIF_SUCCESS
        assert lib.xorif_set_cc_ul_sections_per_symbol(0, s, s) == const.XORIF_SUCCESS

def test_frames_per_symbol_api():
    """Test frames per symbol API."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_cc_frames_per_symbol(caps['max_cc'], 15) == const.XORIF_INVALID_CC

    for s in [0, 1, 3, 5, 10, 15, 20]:
        assert lib.xorif_set_cc_frames_per_symbol(0, s) == const.XORIF_SUCCESS

def test_sections_per_symbol_ssb_api():
    """Test sections per symbol (SSB) API."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_cc_sections_per_symbol_ssb(caps['max_cc'], 64, 64) == const.XORIF_INVALID_CC

    for s in [0, 5, 10, 20]:
        assert lib.xorif_set_cc_sections_per_symbol_ssb(0, s, s) == const.XORIF_SUCCESS

def test_frames_per_symbol_ssb_api():
    """Test frames per symbol (SSB) API."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_cc_frames_per_symbol_ssb(caps['max_cc'], 15) == const.XORIF_INVALID_CC

    for s in [0, 1, 3, 5, 10]:
        assert lib.xorif_set_cc_frames_per_symbol_ssb(0, s) == const.XORIF_SUCCESS

def test_set_config_api():
    """Test set config API."""
    assert lib.xorif_get_state() == 1

    # Test configuration "set" method
    config1 = {}
    config1['num_rbs'] = 51
    config1['num_rbs_ssb'] = 20
    config1['numerology'] = 1
    config1['extended_cp'] = 0
    config1['numerology_ssb'] = 1
    config1['extended_cp_ssb'] = 0
    config1['iq_comp_meth_ul'] = const.IQ_COMP_NONE
    config1['iq_comp_width_ul'] = 16
    config1['iq_comp_meth_dl'] = const.IQ_COMP_NONE
    config1['iq_comp_width_dl'] = 16
    config1['iq_comp_meth_ssb'] = const.IQ_COMP_NONE
    config1['iq_comp_width_ssb'] = 16
    config1['iq_comp_meth_prach'] = const.IQ_COMP_NONE
    config1['iq_comp_width_prach'] = 16
    config1['delay_comp_cp_ul'] = 30
    config1['delay_comp_cp_dl'] = 30
    config1['delay_comp_up'] = 30
    config1['advance_ul'] = 90
    config1['advance_dl'] = 90
    config1['num_ctrl_per_sym_ul'] = 64
    config1['num_ctrl_per_sym_dl'] = 64
    config1['num_ctrl_per_sym_ssb'] = 32
    config1['num_sect_per_sym'] = 32
    config1['num_sect_per_sym_ssb'] = 32
    config1['num_frames_per_sym'] = 15
    config1['num_frames_per_sym_ssb'] = 5
    assert lib.xorif_set_cc_config(caps['max_cc'], config1) == const.XORIF_INVALID_CC
    assert lib.xorif_set_cc_config(0, config1) == const.XORIF_SUCCESS

    for r in [0, 276]:
        config1['num_rbs'] = r
        assert lib.xorif_set_cc_config(0, config1) == const.XORIF_INVALID_RBS
    config1['num_rbs'] = 51

    for n in range(0, 5):
        for r in [1, 2, 50, 100, 200, 273, 275]:
            config1['numerology'] = n
            config1['num_rbs'] = r
            if (caps['numerologies'] & 1<<n):
                config1['numerology'] = n
                config1['num_rbs'] = r
                assert lib.xorif_set_cc_config(0, config1) == const.XORIF_SUCCESS
            else:
                config1['numerology'] = n
                config1['num_rbs'] = r
                assert lib.xorif_set_cc_config(0, config1) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED
    config1['num_rbs'] = 51
    config1['numerology'] = 1

    for n in range(0, 5):
        if (caps['numerologies'] & 1<<n):
            config1['numerology_ssb'] = n
            config1['num_rbs_ssb'] = 19
            assert lib.xorif_set_cc_config(0, config1) == const.XORIF_INVALID_RBS
            config1['num_rbs_ssb'] = 40
            assert lib.xorif_set_cc_config(0, config1) == const.XORIF_INVALID_RBS
            config1['num_rbs_ssb'] = 20
            assert lib.xorif_set_cc_config(0, config1) == const.XORIF_SUCCESS
            config1['num_rbs_ssb'] = 0
            assert lib.xorif_set_cc_config(0, config1) == const.XORIF_SUCCESS
        else:
            config1['numerology_ssb'] = n
            config1['num_rbs_ssb'] = 20
            assert lib.xorif_set_cc_config(0, config1) == const.XORIF_NUMEROLOGY_NOT_SUPPORTED
    config1['num_rbs_ssb'] = 20
    config1['numerology_ssb'] = 1

    for m, w in ul_comp_list:
        config1['iq_comp_meth_ul'] = m
        config1['iq_comp_width_ul'] = w
        if caps['iq_comp_methods'] & 1<<m:
            assert lib.xorif_set_cc_config(0, config1) == const.XORIF_SUCCESS

    for m, w in bad_comp_list:
        config1['iq_comp_meth_ul'] = m
        config1['iq_comp_width_ul'] = w
        assert lib.xorif_set_cc_config(0, config1) == const.XORIF_COMP_MODE_NOT_SUPPORTED

    config1['iq_comp_meth_ul'] = const.IQ_COMP_NONE
    config1['iq_comp_width_ul'] = 16

    for m, w in dl_comp_list:
        config1['iq_comp_meth_dl'] = m
        config1['iq_comp_width_dl'] = w
        if caps['iq_de_comp_methods'] & 1<<m:
            assert lib.xorif_set_cc_config(0, config1) == const.XORIF_SUCCESS

    for m, w in bad_comp_list:
        config1['iq_comp_meth_dl'] = m
        config1['iq_comp_width_dl'] = w
        assert lib.xorif_set_cc_config(0, config1) == const.XORIF_COMP_MODE_NOT_SUPPORTED

    config1['iq_comp_meth_dl'] = const.IQ_COMP_NONE
    config1['iq_comp_width_dl'] = 16

    for m, w in dl_comp_list:
        config1['iq_comp_meth_ssb'] = m
        config1['iq_comp_width_ssb'] = w
        if caps['iq_de_comp_methods'] & 1<<m:
            assert lib.xorif_set_cc_config(0, config1) == const.XORIF_SUCCESS

    for m, w in bad_comp_list:
        config1['iq_comp_meth_ssb'] = m
        config1['iq_comp_width_ssb'] = w
        assert lib.xorif_set_cc_config(0, config1) == const.XORIF_COMP_MODE_NOT_SUPPORTED

    config1['iq_comp_meth_ssb'] = const.IQ_COMP_NONE
    config1['iq_comp_width_ssb'] = 16

    for m, w in ul_comp_list:
        config1['iq_comp_meth_prach'] = m
        config1['iq_comp_width_prach'] = w
        if caps['iq_comp_methods'] & 1<<m:
            assert lib.xorif_set_cc_config(0, config1) == const.XORIF_SUCCESS

    for m, w in bad_comp_list:
        config1['iq_comp_meth_prach'] = m
        config1['iq_comp_width_prach'] = w
        assert lib.xorif_set_cc_config(0, config1) == const.XORIF_COMP_MODE_NOT_SUPPORTED

    config1['iq_comp_meth_prach'] = const.IQ_COMP_NONE
    config1['iq_comp_width_prach'] = 16

def test_get_config_api():
    """Test get config API."""
    assert lib.xorif_get_state() == 1

    # Load up a valid configuration with "set" method
    config1 = {}
    config1['num_rbs'] = 51
    config1['num_rbs_ssb'] = 20
    config1['numerology'] = 1
    config1['extended_cp'] = 0
    config1['numerology_ssb'] = 1
    config1['extended_cp_ssb'] = 0
    config1['iq_comp_meth_ul'] = const.IQ_COMP_NONE
    config1['iq_comp_width_ul'] = 16
    config1['iq_comp_meth_dl'] = const.IQ_COMP_NONE
    config1['iq_comp_width_dl'] = 16
    config1['iq_comp_meth_ssb'] = const.IQ_COMP_NONE
    config1['iq_comp_width_ssb'] = 16
    config1['delay_comp_cp_ul'] = 30
    config1['delay_comp_cp_dl'] = 30
    config1['delay_comp_up'] = 30
    config1['advance_ul'] = 90
    config1['advance_dl'] = 90
    config1['num_ctrl_per_sym_ul'] = 64
    config1['num_ctrl_per_sym_dl'] = 64
    config1['num_ctrl_per_sym_ssb'] = 32
    config1['num_sect_per_sym'] = 32
    config1['num_sect_per_sym_ssb'] = 32
    config1['num_frames_per_sym'] = 15
    config1['num_frames_per_sym_ssb'] = 5
    assert lib.xorif_set_cc_config(caps['max_cc'], config1) == const.XORIF_INVALID_CC
    assert lib.xorif_set_cc_config(0, config1) == const.XORIF_SUCCESS

    # Check configuration "get" method
    result, config2 = lib.xorif_get_cc_config(caps['max_cc'])
    assert result == const.XORIF_INVALID_CC
    result, config2 = lib.xorif_get_cc_config(0)
    assert result == const.XORIF_SUCCESS
    assert config1['num_rbs'] == config2['num_rbs']
    assert config1['num_rbs_ssb'] == config2['num_rbs_ssb']
    assert config1['numerology'] == config2['numerology']
    assert config1['extended_cp'] == config2['extended_cp']
    assert config1['numerology_ssb'] == config2['numerology_ssb']
    assert config1['extended_cp_ssb'] == config2['extended_cp_ssb']
    assert config1['iq_comp_meth_ul'] == config2['iq_comp_meth_ul']
    assert config1['iq_comp_width_ul'] == config2['iq_comp_width_ul']
    assert config1['iq_comp_meth_dl'] == config2['iq_comp_meth_dl']
    assert config1['iq_comp_width_dl'] == config2['iq_comp_width_dl']
    assert config1['iq_comp_meth_ssb'] == config2['iq_comp_meth_ssb']
    assert config1['iq_comp_width_ssb'] == config2['iq_comp_width_ssb']
    assert config1['delay_comp_cp_ul'] == config2['delay_comp_cp_ul']
    assert config1['delay_comp_cp_dl'] == config2['delay_comp_cp_dl']
    assert config1['delay_comp_up'] == config2['delay_comp_up']
    assert config1['advance_ul'] == config2['advance_ul']
    assert config1['advance_dl'] == config2['advance_dl']
    assert config1['num_ctrl_per_sym_ul'] == config2['num_ctrl_per_sym_ul']
    assert config1['num_ctrl_per_sym_dl'] == config2['num_ctrl_per_sym_dl']
    assert config1['num_ctrl_per_sym_ssb'] == config2['num_ctrl_per_sym_ssb']
    assert config1['num_sect_per_sym'] == config2['num_sect_per_sym']
    assert config1['num_sect_per_sym_ssb'] == config2['num_sect_per_sym_ssb']
    assert config1['num_frames_per_sym'] == config2['num_frames_per_sym']
    assert config1['num_frames_per_sym_ssb'] == config2['num_frames_per_sym_ssb']

def test_alarm_api():
    """Test the API to get/clear alarms."""
    assert lib.xorif_get_state() == 1


    if lib.xorif_has_front_haul_interface():
        lib.xorif_clear_fhi_alarms()
        fhi_alarms = lib.xorif_get_fhi_alarms()
        print(fhi_alarms)
        assert fhi_alarms == 0

def test_stats_api():
    """Test the API to get/clear stats/counters."""
    assert lib.xorif_get_state() == 1

    if lib.xorif_has_front_haul_interface():
        lib.xorif_clear_fhi_stats()
        for p in range(0, caps['num_eth_ports']):
            result, stats = lib.xorif_get_fhi_eth_stats(p)
            assert result == const.XORIF_SUCCESS
        result, stats = lib.xorif_get_fhi_eth_stats(caps['num_eth_ports'])
        assert result == const.XORIF_INVALID_ETH_PORT

def test_set_mac_address_api():
    """Test the API to set MAC address."""
    assert lib.xorif_get_state() == 1

    addr = "01:23:45:67:89:ab"
    for p in range(0, caps['num_eth_ports']):
        assert lib.xorif_set_fhi_dest_mac_addr(p, "") == const.XORIF_FAILURE
        assert lib.xorif_set_fhi_dest_mac_addr(p, "0123456789ab") == const.XORIF_FAILURE
        assert lib.xorif_set_fhi_dest_mac_addr(p, addr) == const.XORIF_SUCCESS
        assert lib.xorif_set_fhi_src_mac_addr(p, "") == const.XORIF_FAILURE
        assert lib.xorif_set_fhi_src_mac_addr(p, "0123456789ab") == const.XORIF_FAILURE
        assert lib.xorif_set_fhi_src_mac_addr(p, addr) == const.XORIF_SUCCESS
    assert lib.xorif_set_fhi_dest_mac_addr(caps['num_eth_ports'], addr) == const.XORIF_INVALID_ETH_PORT
    assert lib.xorif_set_fhi_src_mac_addr(caps['num_eth_ports'], addr) == const.XORIF_INVALID_ETH_PORT


def test_set_modu_api():
    """Test multi O-DU features."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_modu_mode(1) == const.XORIF_SUCCESS

    addr = "01:23:45:67:89:ab"
    id = 1234
    dei = 1
    pcp = 2
    for du in range(3):
        assert lib.xorif_set_modu_dest_mac_addr(du, "", id, dei, pcp) == const.XORIF_FAILURE
        assert lib.xorif_set_modu_dest_mac_addr(du, "0123456789ab", id, dei, pcp) == const.XORIF_FAILURE
        assert lib.xorif_set_modu_dest_mac_addr(du, addr, id, dei, pcp) == const.XORIF_SUCCESS

    assert lib.xorif_set_modu_mode(0) == const.XORIF_SUCCESS

def test_protocol_api():
    """Test the API to configure the Ethernet protocol, VLAN, etc."""
    assert lib.xorif_get_state() == 1

    # eCPRI, no VLAN, raw
    assert lib.xorif_set_fhi_protocol(const.PROTOCOL_ECPRI, 0, const.IP_MODE_RAW) == const.XORIF_SUCCESS
    assert lib.xorif_set_fhi_protocol_alt(const.PROTOCOL_ECPRI, 0, const.IP_MODE_RAW) == const.XORIF_SUCCESS

    # eCPRI, VLAN, raw
    assert lib.xorif_set_fhi_protocol(const.PROTOCOL_ECPRI, 1, const.IP_MODE_RAW) == const.XORIF_SUCCESS
    assert lib.xorif_set_fhi_protocol_alt(const.PROTOCOL_ECPRI, 1, const.IP_MODE_RAW) == const.XORIF_SUCCESS

    # IEEE 1914.3, no VLAN, raw
    assert lib.xorif_set_fhi_protocol(const.PROTOCOL_IEEE_1914_3, 0, const.IP_MODE_RAW) == const.XORIF_SUCCESS
    assert lib.xorif_set_fhi_protocol_alt(const.PROTOCOL_IEEE_1914_3, 0, const.IP_MODE_RAW) == const.XORIF_SUCCESS

    # IEEE 1914.3, VLAN, raw
    assert lib.xorif_set_fhi_protocol(const.PROTOCOL_IEEE_1914_3, 1, const.IP_MODE_RAW) == const.XORIF_SUCCESS
    assert lib.xorif_set_fhi_protocol_alt(const.PROTOCOL_IEEE_1914_3, 1, const.IP_MODE_RAW) == const.XORIF_SUCCESS

def test_vlan_tag_api():
    """Test the API to configure the Ethernet protocol, VLAN, etc."""
    assert lib.xorif_get_state() == 1

    for p in range(0, caps['num_eth_ports']):
        for id in [0, 100, 200, 1000, 4095]:
            assert lib.xorif_set_fhi_vlan_tag(p, id, 1, 5) == const.XORIF_SUCCESS

    assert lib.xorif_set_fhi_vlan_tag(caps['num_eth_ports'], 2000, 1, 5) == const.XORIF_INVALID_ETH_PORT

def test_ethernet_packet_filter_api():
    """Test the API to configure the Ethernet packet filter."""
    assert lib.xorif_get_state() == 1

    filter = [x + 0x12345678 for x in range(16)]
    mask = [x + 0xABCD for x in range(4)]
    assert lib.xorif_set_fhi_packet_filter(caps['num_eth_ports'], filter, mask) == const.XORIF_INVALID_ETH_PORT
    assert lib.xorif_set_fhi_packet_filter(0, [0, 1, 2], mask) == const.XORIF_FAILURE
    assert lib.xorif_set_fhi_packet_filter(0, filter, [0, 1, 2]) == const.XORIF_FAILURE
    assert lib.xorif_set_fhi_packet_filter(0, filter, mask) == const.XORIF_SUCCESS

def test_eaxc_id_api():
    """Test the API to set the eaxc id."""
    assert lib.xorif_get_state() == 1

    # Sum of fields is bigger than 16
    assert lib.xorif_set_fhi_eaxc_id(5, 4, 4, 4) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_fhi_eaxc_id(4, 5, 4, 4) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_fhi_eaxc_id(4, 4, 5, 4) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_fhi_eaxc_id(4, 4, 4, 5) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_fhi_eaxc_id(3, 4, 4, 4) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_fhi_eaxc_id(4, 3, 4, 4) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_fhi_eaxc_id(4, 4, 3, 4) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_fhi_eaxc_id(4, 4, 4, 3) == const.XORIF_INVALID_EAXC_ID

    ## Sum of fields is 16, but some fields are bigger than allowed
    max = caps['du_id_limit'] + caps['bs_id_limit'] + caps['cc_id_limit'] + caps['ru_id_limit']
    x = max - 16
    assert lib.xorif_set_fhi_eaxc_id(caps['du_id_limit'], caps['bs_id_limit'], caps['cc_id_limit'], caps['ru_id_limit'] - x) == const.XORIF_SUCCESS
    assert lib.xorif_set_fhi_eaxc_id(caps['du_id_limit'] + 1, caps['bs_id_limit'], caps['cc_id_limit'], caps['ru_id_limit'] - x - 1) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_fhi_eaxc_id(caps['du_id_limit'], caps['bs_id_limit'] + 1, caps['cc_id_limit'], caps['ru_id_limit'] - x - 1) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_fhi_eaxc_id(caps['du_id_limit'], caps['bs_id_limit'], caps['cc_id_limit'] + 1, caps['ru_id_limit'] - x - 1) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_fhi_eaxc_id(caps['du_id_limit'], caps['bs_id_limit'] - x, caps['cc_id_limit'], caps['ru_id_limit'] + 1) == const.XORIF_INVALID_EAXC_ID

    # Finally, some known working configurations
    assert lib.xorif_set_fhi_eaxc_id(4, 2, 2, 8) == const.XORIF_SUCCESS
    assert lib.xorif_set_fhi_eaxc_id(4, 1, 3, 8) == const.XORIF_SUCCESS

def test_set_ru_ports_api():
    """Test the API to set RU port ids."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_fhi_eaxc_id(4, 1, 3, 8) == const.XORIF_SUCCESS
    assert lib.xorif_set_ru_ports(2, 4, 0xC0, 0, 0, 0) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_ru_ports(8, caps['ss_id_limit'] + 1, 0xC0, 0, 0, 0) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_ru_ports(8, caps['ss_id_limit'], 0xC0, 0x00, 0x80, 0x40) == const.XORIF_SUCCESS
    assert lib.xorif_set_ru_ports(8, caps['ss_id_limit'], 0xC0, 0x00, 0xFF, 0xFF) == const.XORIF_SUCCESS

def test_set_ru_ports_alt_api():
    """Test the API to set RU port ids (alternative API)."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_fhi_eaxc_id(4, 1, 3, 8) == const.XORIF_SUCCESS
    assert lib.xorif_set_ru_ports_alt1(2, 4, 0xE0, 0, 0, 0, 0) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_ru_ports_alt1(8, caps['ss_id_limit'] + 1, 0xE0, 0, 0, 0, 0) == const.XORIF_INVALID_EAXC_ID
    assert lib.xorif_set_ru_ports_alt1(8, caps['ss_id_limit'], 0xE0, 0x00, 0x80, 0x40, 0x20) == const.XORIF_SUCCESS
    assert lib.xorif_set_ru_ports_alt1(8, caps['ss_id_limit'], 0xE0, 0x00, 0xFF, 0xFF, 0xFF) == const.XORIF_SUCCESS

def test_set_ru_ports_table_api():
    """Test the API to set RU port table mapping."""
    assert lib.xorif_get_state() == 1

    width = caps['ru_ports_map_width']

    if width == 0:
        # 0 addresses
         # Set RU bits = 8, BS bits = 1
        assert lib.xorif_set_fhi_eaxc_id(4, 1, 3, 8) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(0) == const.XORIF_INVALID_RU_PORT_MAPPING
        assert lib.xorif_set_ru_ports_table_mode(1) == const.XORIF_INVALID_RU_PORT_MAPPING
        assert lib.xorif_set_ru_ports_table_mode(2) == const.XORIF_INVALID_RU_PORT_MAPPING
        assert lib.xorif_set_ru_ports_table_mode(3) == const.XORIF_INVALID_RU_PORT_MAPPING

        # Write some mappings
        assert lib.xorif_set_ru_ports_table(0, 0, 1, 0) == const.XORIF_INVALID_RU_PORT_MAPPING
        assert lib.xorif_set_ru_ports_table(0, 0, 1, 1) == const.XORIF_INVALID_RU_PORT_MAPPING

    if width == 8:
        # 256 addresses
        # Set RU bits = 8, BS bits = 1
        assert lib.xorif_set_fhi_eaxc_id(4, 1, 3, 8) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(0) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(1) == const.XORIF_INVALID_RU_PORT_MAPPING
        assert lib.xorif_set_ru_ports_table_mode(2) == const.XORIF_INVALID_RU_PORT_MAPPING
        assert lib.xorif_set_ru_ports_table_mode(3) == const.XORIF_INVALID_RU_PORT_MAPPING

        # Set RU bits = 6, BS bits = 3
        assert lib.xorif_set_fhi_eaxc_id(4, 3, 3, 6) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(0) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(1) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(2) == const.XORIF_INVALID_RU_PORT_MAPPING
        assert lib.xorif_set_ru_ports_table_mode(3) == const.XORIF_INVALID_RU_PORT_MAPPING

        # Write some mappings
        assert lib.xorif_set_ru_ports_table(0, 0, 1, 1) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table(0, 0, 1, 16) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table(0, 0, 1, 256) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table(0, 0, 1, 257) == const.XORIF_INVALID_RU_PORT_MAPPING

    if width == 11:
        # 2048 addresses
        # Set RU bits = 8, BS bits = 1
        assert lib.xorif_set_fhi_eaxc_id(4, 1, 3, 8) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(0) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(1) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(2) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(3) == const.XORIF_SUCCESS

        # Set RU bits = 6, BS bits = 3
        assert lib.xorif_set_fhi_eaxc_id(4, 3, 3, 6) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(0) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(1) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(2) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(3) == const.XORIF_SUCCESS

        # Set RU bits = 8, BS bits = 3
        assert lib.xorif_set_fhi_eaxc_id(2, 3, 3, 8) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(0) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(1) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(2) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table_mode(3) == const.XORIF_INVALID_RU_PORT_MAPPING

        # Write some mappings
        assert lib.xorif_set_ru_ports_table(0, 0, 1, 1) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table(0, 0, 1, 16) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table(0, 0, 1, 2048) == const.XORIF_SUCCESS
        assert lib.xorif_set_ru_ports_table(0, 0, 1, 2049) == const.XORIF_INVALID_RU_PORT_MAPPING

    assert lib.xorif_set_ru_ports_table_mode(4) == const.XORIF_INVALID_RU_PORT_MAPPING
    assert lib.xorif_clear_ru_ports_table() == const.XORIF_SUCCESS

def test_fhi_register_read_api():
    """Check the FHI register read API."""
    assert lib.xorif_get_state() == 1

    # Try a few random reads of known register names, and numeric offsets
    reg_list1 = ['CFG_MAJOR_REVISION',
                 'CFG_MINOR_REVISION',
                 'CFG_VERSION_REVISION',
                 'CFG_CONFIG_NO_OF_ETH_PORTS',
                 'CFG_CONFIG_XRAN_MAX_NUMEROLOGY',
                 '0x0000']
    for reg in reg_list1:
        result, value = lib.xorif_read_fhi_reg(reg)
        assert result == const.XORIF_SUCCESS

    # Try a few random reads of made-up /incorrect register names
    reg_list2 = ['THIS_IS_NOT_A_REG',
                 'cfg_major_revision',
                 'CFG_MAJOR_REVISIONX',
                 'CFG_MAJOR_REVISIO']
    for reg in reg_list2:
        result, value = lib.xorif_read_fhi_reg(reg)
        assert result == const.XORIF_REGISTER_NOT_FOUND

def test_fhi_register_read_offset_api():
    """Check the FHI register read offset API."""
    assert lib.xorif_get_state() == 1

    # Read a register with various offsets
    reg = 'DEFM_USER_DATA_FILTER_W0_31_0'
    for i in range(0, caps['num_eth_ports']):
        o = i * 0x100
        result, value = lib.xorif_read_fhi_reg_offset(reg, o)
        assert result == const.XORIF_SUCCESS

    # Try a few random reads of made-up /incorrect register names
    reg_list2 = ['THIS_IS_NOT_A_REG',
                 'cfg_major_revision',
                 'CFG_MAJOR_REVISIONX',
                 'CFG_MAJOR_REVISIO']
    for reg in reg_list2:
        result, value = lib.xorif_read_fhi_reg_offset(reg, 0)
        assert result == const.XORIF_REGISTER_NOT_FOUND

def test_fhi_register_write_api():
    """Check the FHI register write API."""
    assert lib.xorif_get_state() == 1

    reg = 'DEFM_USER_DATA_FILTER_W0_31_0'

    # Save original value
    result, orig = lib.xorif_read_fhi_reg(reg)
    assert result == const.XORIF_SUCCESS

    # Try some basic write patterns
    for n in [0x00000000, 0xFFFFFFFF, 0x12345678, 0x87654321, 0xAAAAAAAA, 0x55555555]:
        # Write value
        assert lib.xorif_write_fhi_reg(reg, n) == const.XORIF_SUCCESS
        # Read it back
        result, value = lib.xorif_read_fhi_reg(reg)
        assert result == const.XORIF_SUCCESS
        assert value == n

    # Restore original value
    assert lib.xorif_write_fhi_reg(reg, orig) == const.XORIF_SUCCESS

    # Try a few random writes of made-up /incorrect register names
    reg_list = ['THIS_IS_NOT_A_REG',
                'cfg_major_revision',
                'CFG_MAJOR_REVISIONX',
                'CFG_MAJOR_REVISIO']
    for reg in reg_list:
        assert lib.xorif_write_fhi_reg(reg, 0) == const.XORIF_REGISTER_NOT_FOUND

def test_fhi_register_write_offset_api():
    """Check the FHI register write offset API."""
    assert lib.xorif_get_state() == 1

    reg = 'DEFM_USER_DATA_FILTER_W0_31_0'

    # Write a register with various offsets
    for i in range(0, caps['num_eth_ports']):
        o = i * 0x100

        # Save original value
        result, orig = lib.xorif_read_fhi_reg_offset(reg, o)
        assert result == const.XORIF_SUCCESS

        # Try some basic write patterns
        for n in [0x00000000, 0xFFFFFFFF, 0x12345678, 0x87654321, 0xAAAAAAAA, 0x55555555]:
            # Write value
            assert lib.xorif_write_fhi_reg_offset(reg, o, n) == const.XORIF_SUCCESS
            # Read it back
            result, value = lib.xorif_read_fhi_reg_offset(reg, o)
            assert result == const.XORIF_SUCCESS
            assert value == n

        # Restore original value
        assert lib.xorif_write_fhi_reg_offset(reg, o, orig) == const.XORIF_SUCCESS

    # Try a few random writes of made-up /incorrect register names
    reg_list = ['THIS_IS_NOT_A_REG',
                'cfg_major_revision',
                'CFG_MAJOR_REVISIONX',
                'CFG_MAJOR_REVISIO']
    for reg in reg_list:
        assert lib.xorif_write_fhi_reg_offset(reg, 0, 0) == const.XORIF_REGISTER_NOT_FOUND

def test_ul_bid_forward_api():
    """Check uplink beam-id forward API."""
    assert lib.xorif_get_state() == 1


    assert lib.xorif_set_ul_bid_forward(caps['max_cc'], 0) == const.XORIF_INVALID_CC
    assert lib.xorif_set_ul_bid_forward(0, 90) == const.XORIF_SUCCESS

def test_ul_radio_ch_dly_api():
    """Check uplink radio chnnel delay API."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_ul_radio_ch_dly(caps['max_cc'], 0) == const.XORIF_INVALID_CC
    assert lib.xorif_set_ul_radio_ch_dly(0, 30) == const.XORIF_SUCCESS

def test_timing_constants_api():
    """Check timing constants API."""
    assert lib.xorif_get_state() == 1

    sys_const = {}
    sys_const['FH_DECAP_DLY'] = 0
    lib.xorif_set_system_constants(sys_const) == const.XORIF_SUCCESS

    sys_const['FH_DECAP_DLY'] = 5
    lib.xorif_set_system_constants(sys_const) == const.XORIF_SUCCESS

def test_enable_interrupts_api():
    """Check the enable/disable interrupts API."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_enable_fhi_interrupts(0xFFFFFFFF) == const.XORIF_SUCCESS
    assert lib.xorif_enable_fhi_interrupts(0x0) == const.XORIF_SUCCESS

def test_enable_disable_api():
    """Check the enable/disable APIs."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_enable_cc(caps['max_cc']) == const.XORIF_INVALID_CC
    assert lib.xorif_enable_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_disable_cc(caps['max_cc']) == const.XORIF_INVALID_CC
    assert lib.xorif_disable_cc(0) == const.XORIF_SUCCESS
    assert lib.xorif_get_enabled_cc_mask() == 0

def test_symbol_strobe_api():
    """Check symbol strobe source API."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_symbol_strobe_source(1) == const.XORIF_SUCCESS
    result, value = lib.xorif_read_fhi_reg("DEFM_USE_ONE_SYMBOL_STROBE")
    assert result == const.XORIF_SUCCESS
    assert value == 1

    assert lib.xorif_set_symbol_strobe_source(0) == const.XORIF_SUCCESS
    result, value = lib.xorif_read_fhi_reg("DEFM_USE_ONE_SYMBOL_STROBE")
    assert result == const.XORIF_SUCCESS
    assert value == 0

def test_per_ss_decompression_api():
    """Test the per-SS decompression API."""
    assert lib.xorif_get_state() == 1

    # Try some invalid values
    assert lib.xorif_set_cc_dl_iq_compression_per_ss(caps['no_deframer_ss'], 0, 0, 1, 1) == const.XORIF_INVALID_SS
    assert lib.xorif_set_cc_dl_iq_compression_per_ss(0, 0, 0, 1, caps['no_deframer_ss'] + 1) == const.XORIF_INVALID_SS
    for m, w in bad_comp_list:
        assert lib.xorif_set_cc_dl_iq_compression_per_ss(0, w, m, 1, 1) == const.XORIF_COMP_MODE_NOT_SUPPORTED

    # Try some valid values
    for m, w in dl_comp_list:
        if caps['iq_de_comp_methods'] & 1<<m:
            assert lib.xorif_set_cc_dl_iq_compression_per_ss(0, w, m, 1, 0) == const.XORIF_SUCCESS
            assert lib.xorif_set_cc_dl_iq_compression_per_ss(0, w, m, 1, caps['no_deframer_ss']) == const.XORIF_SUCCESS

    # Finally leave disabled
    assert lib.xorif_set_cc_dl_iq_compression_per_ss(0, 0, 0, 0, caps['no_deframer_ss']) == const.XORIF_SUCCESS

def test_configure_api():
    """Test configure API."""
    assert lib.xorif_get_state() == 1

    # Test configuration "set" method
    config = {}
    config['num_rbs'] = 51
    config['num_rbs_ssb'] = 20
    config['numerology'] = 1
    config['extended_cp'] = 0
    config['numerology_ssb'] = 1
    config['extended_cp_ssb'] = 0
    config['iq_comp_meth_ul'] = const.IQ_COMP_NONE
    config['iq_comp_width_ul'] = 16
    config['iq_comp_meth_dl'] = const.IQ_COMP_NONE
    config['iq_comp_width_dl'] = 16
    config['iq_comp_meth_ssb'] = const.IQ_COMP_NONE
    config['iq_comp_width_ssb'] = 16
    config['iq_comp_meth_prach'] = const.IQ_COMP_NONE
    config['iq_comp_width_prach'] = 16
    config['delay_comp_cp_ul'] = 30
    config['delay_comp_cp_dl'] = 30
    config['delay_comp_up'] = 30
    config['advance_ul'] = 90
    config['advance_dl'] = 90
    config['num_ctrl_per_sym_ul'] = 64
    config['num_ctrl_per_sym_dl'] = 64
    config['num_ctrl_per_sym_ssb'] = 32
    config['num_sect_per_sym'] = 32
    config['num_sect_per_sym_ssb'] = 32
    config['num_frames_per_sym'] = 15
    config['num_frames_per_sym_ssb'] = 5
    assert lib.xorif_set_cc_config(0, config) == const.XORIF_SUCCESS

    # Configure
    assert lib.xorif_configure_cc(caps['max_cc']) == const.XORIF_INVALID_CC
    assert lib.xorif_configure_cc(0) == const.XORIF_SUCCESS

    # Get allocation
    result, alloc = lib.xorif_get_fhi_cc_alloc(caps['max_cc'])
    assert result == const.XORIF_INVALID_CC
    result, alloc = lib.xorif_get_fhi_cc_alloc(0)
    assert result == const.XORIF_SUCCESS

def test_mtu_size_api():
    """Test the setting of MTU size API."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_set_mtu_size(0) == const.XORIF_INVALID_CONFIG
    assert lib.xorif_set_mtu_size(caps['max_framer_ethernet_pkt'] + 1) == const.XORIF_INVALID_CONFIG
    assert lib.xorif_set_mtu_size(caps['max_framer_ethernet_pkt']) == const.XORIF_SUCCESS
    assert lib.xorif_set_mtu_size(8000) == const.XORIF_SUCCESS

def test_monitor_api():
    """Test the monitor API."""
    assert lib.xorif_get_state() == 1

    assert lib.xorif_monitor_clear() == const.XORIF_SUCCESS
    assert lib.xorif_monitor_select(0) == const.XORIF_SUCCESS
    assert lib.xorif_monitor_snapshot() == const.XORIF_SUCCESS
    result, value = lib.xorif_monitor_read(0)
    assert result == const.XORIF_SUCCESS
    print(value)
    result, value = lib.xorif_monitor_read(1)
    assert result == const.XORIF_SUCCESS
    print(value)