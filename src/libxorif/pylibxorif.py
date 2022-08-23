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

__author__ = "Steven Dickinson"
__copyright__ = "Copyright 2022, Xilinx"

import os
import logging
from cffi import FFI
import Pyro4

# Use local files if available, otherwise use the standard locations
lib_path = "./libxorif.so" if os.path.exists("./libxorif.so") else "/usr/lib/libxorif.so"
inc_path = "./xorif_api.txt" if os.path.exists("./xorif_api.txt") else "/usr/include/xorif/xorif_api.txt"

# Load library and definitions with CFFI
ffi = FFI()
try:
    f = open(inc_path)
    ffi.cdef(f.read())
except Exception as e:
    print(f"Failed to open/read header '{inc_path}'")
    print(e)
try:
    lib = ffi.dlopen(lib_path)
except Exception as e:
    print(f"Failed to open/read library '{lib_path}'")
    print(e)

# Some utilities to convert cdata types to regular Python types...

def struct_to_py(CdataPtr, TypeFields):
    for Fld, FldType in TypeFields:
        if FldType.type.kind == 'primitive':
            yield (Fld, getattr(CdataPtr, Fld))
        else:
            yield (Fld, cdata_to_py(getattr(CdataPtr, Fld)))

def array_to_py(CdataPtr):
    Type = ffi.typeof(CdataPtr)
    if Type.item.kind == 'primitive':
        if Type.item.cname == 'wchar_t' or Type.item.cname == 'char':
            return ffi.string(CdataPtr)
        else:
            return [CdataPtr[m] for m in range(Type.length)]
    else:
        return [cdata_to_py(CdataPtr[m]) for m in range(Type.length)]

def cdata_to_py(CdataPtr):
    Type = ffi.typeof(CdataPtr)
    if Type.kind == 'struct':
        return dict(struct_to_py(CdataPtr, Type.fields))
    elif Type.kind == 'primitive':
        return int(CdataPtr)
    elif Type.kind == 'array':
        return array_to_py(CdataPtr)

def cptr_to_pylist(CdataPtr, len):
    List = []
    for i in range(len):
        List.append(CdataPtr[i])
    return List

def cdata_string_to_py(CdataPtr):
    return ffi.string(CdataPtr)

class LIBXORIF:
    """
    Python adapter for the libxorif shared library, using CFFI.

    The adaptor class provides a more Pythonic API, for example
    converting 'pointer to struct' objects into dictionaries,
    returning tuples rather than passing pointers in order to
    write-back results, etc.
    """

    _instance = None

    # Using singleton pattern
    def __new__(cls):
        if not cls._instance:
            cls._instance = super(LIBXORIF, cls).__new__(cls)

            # Set up logger
            logging.basicConfig(format="%(name)s> %(levelname)s: %(message)s")
            cls._instance.logger = logging.getLogger("PYXORIF")
            cls._instance.logger.setLevel(logging.ERROR)
            
            # We're going to expose all enums and integer #define constants
            constants = {}
            for a in dir(lib):
                if type(getattr(lib, a)) == int:
                    constants[a] = getattr(lib, a)
            cls._instance._constants = constants

        return cls._instance

    @property
    def constants(self):
        # Dictionary of enums available as read-only property
        return self._constants

    def set_log_level(self, level):
        self.logger.setLevel(level)

    # int xorif_get_state(void)
    def xorif_get_state(self):
        self.logger.info(f'xorif_get_state:')
        return lib.xorif_get_state()

    # void xorif_debug(int level)
    def xorif_debug(self, level):
        self.logger.info(f'xorif_debug: {level}')
        return lib.xorif_debug(level)

    # int xorif_init(const char *device_name)
    def xorif_init(self, device_name=None):
        self.logger.info(f'xorif_init: {device_name}')
        if not device_name:
            device_name = ffi.NULL
        else:
            device_name = bytes(device_name, "ascii")
        return lib.xorif_init(device_name)

    # void xorif_finish(void)
    def xorif_finish(self):
        self.logger.info(f'xorif_finish:')
        return lib.xorif_finish()

    # uint32_t xorif_get_sw_version(void)
    def xorif_get_sw_version(self):
        self.logger.info(f'xorif_get_sw_version:')
        value = lib.xorif_get_sw_version()
        major = (value >> 24) & 0xFF
        minor = (value >> 16) & 0xFF
        version = value & 0xFFFF
        return (major, minor, version)

    # uint32_t xorif_get_fhi_hw_version(void)
    def xorif_get_fhi_hw_version(self):
        self.logger.info(f'xorif_get_fhi_hw_version:')
        value = lib.xorif_get_fhi_hw_version()
        major = (value >> 24) & 0xFF
        minor = (value >> 16) & 0xFF
        version = value & 0xFFFF
        return (major, minor, version)

    # uint32_t xorif_get_fhi_hw_internal_rev(void)
    def xorif_get_fhi_hw_internal_rev(self):
        self.logger.info(f'xorif_get_fhi_hw_internal_rev:')
        value = lib.xorif_get_fhi_hw_internal_rev()
        return f'{value:08x}'

    # const struct xorif_caps *xorif_get_capabilities(void)
    def xorif_get_capabilities(self):
        self.logger.info(f'xorif_get_capabilities:')
        caps_ptr = lib.xorif_get_capabilities()
        return cdata_to_py(caps_ptr[0])

    # int xorif_has_front_haul_interface(void)
    def xorif_has_front_haul_interface(self):
        self.logger.info(f'xorif_get_capabilities:')
        return lib.xorif_has_front_haul_interface()

    # int xorif_configure_cc(uint16_t cc)
    def xorif_configure_cc(self, cc):
        self.logger.info(f'xorif_get_capabilities: {cc}')
        return lib.xorif_configure_cc(cc)

    # int xorif_disable_cc(uint16_t cc)
    def xorif_enable_cc(self, cc):
        self.logger.info(f'xorif_enable_cc: {cc}')
        return lib.xorif_enable_cc(cc)

    # int xorif_disable_cc(uint16_t cc)
    def xorif_disable_cc(self, cc):
        self.logger.info(f'xorif_disable_cc: {cc}')
        return lib.xorif_disable_cc(cc)

    # uint8_t xorif_get_enabled_cc_mask(void)
    def xorif_get_enabled_cc_mask(self):
        self.logger.info(f'xorif_get_enabled_cc_mask:')
        return lib.xorif_get_enabled_cc_mask()

    # xorif_set_cc_config(uint16_t cc, const struct xorif_cc_config *ptr)
    def xorif_set_cc_config(self, cc, config):
        self.logger.info(f'xorif_set_cc_config: {cc}, {config}')
        config_ptr = ffi.new("const struct xorif_cc_config *", config)
        return lib.xorif_set_cc_config(cc, config_ptr)

    # int xorif_get_cc_config(uint16_t cc, struct xorif_cc_config *ptr)
    def xorif_get_cc_config(self, cc):
        self.logger.info(f'xorif_get_cc_config: {cc}')
        config_ptr = ffi.new("struct xorif_cc_config *")
        result = lib.xorif_get_cc_config(cc, config_ptr)
        return (result, cdata_to_py(config_ptr[0]))

    # int xorif_set_cc_num_rbs(uint16_t cc, uint16_t num_rbs)
    def xorif_set_cc_num_rbs(self, cc, num_rbs):
        self.logger.info(f'xorif_set_cc_num_rbs: {cc}, {num_rbs}')
        return lib.xorif_set_cc_num_rbs(cc, num_rbs)

    # int xorif_set_cc_numerology(uint16_t cc, uint16_t numerology, uint16_t extended_cp)
    def xorif_set_cc_numerology(self, cc, numerology, extended_cp=False):
        self.logger.info(f'xorif_set_cc_numerology: {cc}, {numerology}, {extended_cp}')
        return lib.xorif_set_cc_numerology(cc, numerology, extended_cp)

    # int xorif_set_cc_num_rbs_ssb(uint16_t cc, uint16_t num_rbs)
    def xorif_set_cc_num_rbs_ssb(self, cc, num_rbs):
        self.logger.info(f'xorif_set_cc_num_rbs_ssb: {cc}, {num_rbs}')
        return lib.xorif_set_cc_num_rbs_ssb(cc, num_rbs)

    # int xorif_set_cc_numerology_ssb(uint16_t cc, uint16_t numerology, uint16_t extended_cp)
    def xorif_set_cc_numerology_ssb(self, cc, numerology, extended_cp=False):
        self.logger.info(f'xorif_set_cc_numerology_ssb: {cc}, {numerology}, {extended_cp}')
        return lib.xorif_set_cc_numerology_ssb(cc, numerology, extended_cp)

    # int xorif_set_cc_time_advance(uint16_t cc, double deskew, double advance_ul, double advance_dl)
    def xorif_set_cc_time_advance(self, cc, deskew, advance_ul, advance_dl):
        self.logger.info(f'xorif_set_cc_time_advance: {cc}, {deskew}, {advance_ul}, {advance_dl}')
        return lib.xorif_set_cc_time_advance(cc, deskew, advance_ul, advance_dl)

    # int xorif_set_cc_ul_timing_parameters(uint16_t cc, double delay_comp_cp, double advance, double radio_ch_delay)
    def xorif_set_cc_ul_timing_parameters(self, cc, delay_comp_cp, advance, radio_ch_delay):
        self.logger.info(f'xorif_set_cc_ul_timing_parameters: {cc}, {delay_comp_cp}, {advance}, {radio_ch_delay}')
        return lib.xorif_set_cc_ul_timing_parameters(cc, delay_comp_cp, advance, radio_ch_delay)

    # int xorif_set_cc_dl_timing_parameters(uint16_t cc, double delay_comp_cp, double delay_comp_up, double advance)
    def xorif_set_cc_dl_timing_parameters(self, cc, delay_comp_cp, delay_comp_up, advance):
        self.logger.info(f'xorif_set_cc_dl_timing_parameters: {cc}, {delay_comp_cp}, {delay_comp_up}, {advance}')
        return lib.xorif_set_cc_dl_timing_parameters(cc, delay_comp_cp, delay_comp_up, advance)

    # int xorif_set_ul_bid_forward(uint16_t cc, double time)
    def xorif_set_ul_bid_forward(self, cc, time):
        self.logger.info(f'xorif_set_ul_bid_forward: {cc}, {time}')
        return lib.xorif_set_ul_bid_forward(cc, time)

    # int xorif_set_ul_radio_ch_dly(uint16_t cc, double delay)
    def xorif_set_ul_radio_ch_dly(self, cc, delay):
        self.logger.info(f'xorif_set_ul_radio_ch_dly: {cc}, {delay}')
        return lib.xorif_set_ul_radio_ch_dly(cc, delay)

    # int xorif_set_cc_dl_iq_compression(uint16_t cc, uint16_t bit_width, enum xorif_iq_comp comp_method, uint16_t mplane)
    def xorif_set_cc_dl_iq_compression(self, cc, bit_width, comp_method, mplane):
        self.logger.info(f'xorif_set_cc_dl_iq_compression: {cc}, {bit_width}, {comp_method}, {mplane}')
        return lib.xorif_set_cc_dl_iq_compression(cc, bit_width, comp_method, mplane)

    # int xorif_set_cc_dl_iq_compression_per_ss(uint16_t ss, uint16_t bit_width, enum xorif_iq_comp comp_method, uint16_t enable, uint16_t number)
    def xorif_set_cc_dl_iq_compression_per_ss(self, ss, bit_width, comp_method, enable, number):
        self.logger.info(f'xorif_set_cc_dl_iq_compression_per_ss: {ss}, {bit_width}, {comp_method}, {enable}, {number}')
        return lib.xorif_set_cc_dl_iq_compression_per_ss(ss, bit_width, comp_method, enable, number)

    # int xorif_set_cc_ul_iq_compression(uint16_t cc, uint16_t bit_width, enum xorif_iq_comp comp_method, uint16_t mplane)
    def xorif_set_cc_ul_iq_compression(self, cc, bit_width, comp_method, mplane):
        self.logger.info(f'xorif_set_cc_ul_iq_compression: {cc}, {bit_width}, {comp_method}, {mplane}')
        return lib.xorif_set_cc_ul_iq_compression(cc, bit_width, comp_method, mplane)

    # int xorif_set_cc_iq_compression_ssb(uint16_t cc, uint16_t bit_width, enum xorif_iq_comp comp_method, uint16_t mplane)
    def xorif_set_cc_iq_compression_ssb(self, cc, bit_width, comp_method, mplane):
        self.logger.info(f'xorif_set_cc_iq_compression_ssb: {cc}, {bit_width}, {comp_method}, {mplane}')
        return lib.xorif_set_cc_iq_compression_ssb(cc, bit_width, comp_method, mplane)

    # int xorif_set_cc_iq_compression_prach(uint16_t cc, uint16_t bit_width, enum xorif_iq_comp comp_method, uint16_t mplane)
    def xorif_set_cc_iq_compression_prach(self, cc, bit_width, comp_method, mplane):
        self.logger.info(f'xorif_set_cc_iq_compression_prach: {cc}, {bit_width}, {comp_method}, {mplane}')
        return lib.xorif_set_cc_iq_compression_prach(cc, bit_width, comp_method, mplane)

    # int xorif_set_cc_dl_sections_per_symbol(uint16_t cc, uint16_t num_sect, uint16_t num_ctrl)
    def xorif_set_cc_dl_sections_per_symbol(self, cc, num_sect, num_ctrl):
        self.logger.info(f'xorif_set_cc_dl_sections_per_symbol: {cc}, {num_sect}, {num_ctrl}')
        return lib.xorif_set_cc_dl_sections_per_symbol(cc, num_sect, num_ctrl)

    # int xorif_set_cc_ul_sections_per_symbol(uint16_t cc, uint16_t num_sect, uint16_t num_ctrl)
    def xorif_set_cc_ul_sections_per_symbol(self, cc, num_sect, num_ctrl):
        self.logger.info(f'xorif_set_cc_ul_sections_per_symbol: {cc}, {num_sect}, {num_ctrl}')
        return lib.xorif_set_cc_ul_sections_per_symbol(cc, num_sect, num_ctrl)

    # int xorif_set_cc_frames_per_symbol(uint16_t cc, uint16_t num_frames)
    def xorif_set_cc_frames_per_symbol(self, cc, num_frames):
        self.logger.info(f'xorif_set_cc_frames_per_symbol: {cc}, {num_frames}')
        return lib.xorif_set_cc_frames_per_symbol(cc, num_frames)

    # int xorif_set_cc_sections_per_symbol_ssb(uint16_t cc, uint16_t num_sect, uint16_t num_ctrl)
    def xorif_set_cc_sections_per_symbol_ssb(self, cc, num_sect, num_ctrl):
        self.logger.info(f'xorif_set_cc_sections_per_symbol_ssb: {cc}, {num_sect}, {num_ctrl}')
        return lib.xorif_set_cc_sections_per_symbol_ssb(cc, num_sect, num_ctrl)

    # int xorif_set_cc_frames_per_symbol_ssb(uint16_t cc, uint16_t num_frames)
    def xorif_set_cc_frames_per_symbol_ssb(self, cc, num_frames):
        self.logger.info(f'xorif_set_cc_frames_per_symbol_ssb: {cc}, {num_frames}')
        return lib.xorif_set_cc_frames_per_symbol_ssb(cc, num_frames)

    # int xorif_reset_fhi(uint16_t mode)
    def xorif_reset_fhi(self, mode):
        self.logger.info(f'xorif_reset_fhi: {mode}')
        return lib.xorif_reset_fhi(mode)

    # uint32_t xorif_get_fhi_alarms(void)
    def xorif_get_fhi_alarms(self):
        self.logger.info(f'xorif_get_fhi_alarms:')
        return lib.xorif_get_fhi_alarms()

    # void xorif_clear_fhi_alarms(void)
    def xorif_clear_fhi_alarms(self):
        self.logger.info(f'xorif_clear_fhi_alarms:')
        return lib.xorif_clear_fhi_alarms()

    # void xorif_clear_fhi_stats(void)
    def xorif_clear_fhi_stats(self):
        self.logger.info(f'xorif_clear_fhi_stats:')
        return lib.xorif_clear_fhi_stats()

    # int xorif_get_fhi_cc_alloc(uint16_t cc, struct xorif_cc_alloc *ptr)
    def xorif_get_fhi_cc_alloc(self, cc):
        self.logger.info(f'xorif_get_fhi_cc_alloc: {cc}')
        alloc_ptr = ffi.new("struct xorif_cc_alloc *")
        result = lib.xorif_get_fhi_cc_alloc(cc, alloc_ptr)
        return (result, cdata_to_py(alloc_ptr[0]))

    # int xorif_read_fhi_reg(const char *name, uint32_t *val)
    def xorif_read_fhi_reg(self, name):
        self.logger.info(f'xorif_read_fhi_reg: {name}')
        data_ptr = ffi.new("uint32_t *")
        result = lib.xorif_read_fhi_reg(bytes(name, 'utf-8'), data_ptr)
        return (result, data_ptr[0])

    # int xorif_read_fhi_reg_offset(const char *name, uint16_t offset, uint32_t *val)
    def xorif_read_fhi_reg_offset(self, name, offset):
        self.logger.info(f'xorif_read_fhi_reg_offset: {name}, {offset}')
        data_ptr = ffi.new("uint32_t *")
        result = lib.xorif_read_fhi_reg_offset(bytes(name, 'utf-8'), offset, data_ptr)
        return (result, data_ptr[0])

    # int xorif_write_fhi_reg(const char *name, uint32_t value)
    def xorif_write_fhi_reg(self, name, value):
        self.logger.info(f'xorif_write_fhi_reg: {name}, {value}')
        result = lib.xorif_write_fhi_reg(bytes(name, 'utf-8'), value)
        return result

    # int xorif_write_fhi_reg_offset(const char *name, uint16_t offset, uint32_t value)
    def xorif_write_fhi_reg_offset(self, name, offset, value):
        self.logger.info(f'xorif_write_fhi_reg_offset: {name}, {offset}, {value}')
        result = lib.xorif_write_fhi_reg_offset(bytes(name, 'utf-8'), offset, value)
        return result

    # int xorif_get_fhi_eth_stats(int port, struct xorif_fhi_eth_stats *ptr)
    def xorif_get_fhi_eth_stats(self, port):
        self.logger.info(f'xorif_get_fhi_eth_stats: {port}')
        stats_ptr = ffi.new("struct xorif_fhi_eth_stats *")
        result = lib.xorif_get_fhi_eth_stats(port, stats_ptr)
        return (result, cdata_to_py(stats_ptr[0]))

    # int xorif_set_fhi_dest_mac_addr(int port, const uint8_t address[])
    def xorif_set_fhi_dest_mac_addr(self, port, address):
        self.logger.info(f'xorif_set_fhi_dest_mac_addr: {port}, {address}')
        array = [int(x, 16) for x in address.split(':')]
        if len(array) == 6:
            return lib.xorif_set_fhi_dest_mac_addr(port, array)
        else:
            self.logger.error(f'xorif_set_fhi_dest_mac_addr: invalid MAC address format')
            return lib.XORIF_FAILURE

    # int xorif_set_fhi_src_mac_addr(int port, const uint8_t address[])
    def xorif_set_fhi_src_mac_addr(self, port, address):
        self.logger.info(f'xorif_set_fhi_src_mac_addr: {port}, {address}')
        array = [int(x, 16) for x in address.split(':')]
        if len(array) == 6:
            return lib.xorif_set_fhi_src_mac_addr(port, array)
        else:
            self.logger.error(f'xorif_set_fhi_src_mac_addr: invalid MAC address format')
            return lib.XORIF_FAILURE

    # int xorif_set_modu_mode(uint16_t enable)
    def xorif_set_modu_mode(self, enable):
        self.logger.info(f'xorif_set_modu_mode: {enable}')
        return lib.xorif_set_modu_mode(enable)

    # int xorif_set_modu_dest_mac_addr(uint16_t du, const uint8_t address[], uint16_t id, uint16_t dei, int16_t pcp)
    def xorif_set_modu_dest_mac_addr(self, du, address, id=0, dei=0, pcp=0):
        self.logger.info(f'xorif_set_modu_dest_mac_addr: {du}, {address}, {id}, {dei}, {pcp}')
        array = [int(x, 16) for x in address.split(':')]
        if len(array) == 6:
            return lib.xorif_set_modu_dest_mac_addr(du, array, id, dei, pcp)
        else:
            self.logger.error(f'xorif_set_modu_dest_mac_addr: invalid MAC address format')
            return lib.XORIF_FAILURE

    # int xorif_set_mtu_size(uint16_t size)
    def xorif_set_mtu_size(self, size):
        self.logger.info(f'xorif_set_mtu_size: {size}')
        return lib.xorif_set_mtu_size(size)

    # int xorif_set_fhi_protocol(enum xorif_transport_protocol transport, uint16_t vlan, enum xorif_ip_mode ip_mode)
    def xorif_set_fhi_protocol(self, transport, vlan, ip_mode):
        self.logger.info(f'xorif_set_fhi_protocol: {transport}, {vlan}, {ip_mode}')
        return lib.xorif_set_fhi_protocol(transport, vlan, ip_mode)

    # int xorif_set_fhi_protocol_alt(enum xorif_transport_protocol transport, uint16_t vlan, enum xorif_ip_mode ip_mode)
    def xorif_set_fhi_protocol_alt(self, transport, vlan, ip_mode):
        self.logger.info(f'xorif_set_fhi_protocol_alt: {transport}, {vlan}, {ip_mode}')
        return lib.xorif_set_fhi_protocol(transport, vlan, ip_mode)

    # int xorif_set_fhi_vlan_tag(int port, uint16_t id, uint16_t dei, uint16_t pcp)
    def xorif_set_fhi_vlan_tag(self, port, id, dei, pcp):
        self.logger.info(f'xorif_set_fhi_vlan_tag: {port}, {id}, {dei}, {pcp}')
        return lib.xorif_set_fhi_vlan_tag(port, id, dei, pcp)

    # int xorif_set_fhi_packet_filter(int port, const uint32_t filter[16], uint16_t mask[4])
    def xorif_set_fhi_packet_filter(self, port, filter, mask):
        self.logger.info(f'xorif_set_fhi_packet_filter: {port}, {filter}, {mask}')
        if len(filter) == 16 and len(mask) == 4:
            return lib.xorif_set_fhi_packet_filter(port, filter, mask)
        else:
            self.logger.error(f'xorif_set_fhi_packet_filter: invalid filter / mask size')
            return lib.XORIF_FAILURE

    # int xorif_set_fhi_eaxc_id(uint16_t du_bits, uint16_t bs_bits, uint16_t cc_bits, uint16_t ru_bits)
    def xorif_set_fhi_eaxc_id(self, du_bits, bs_bits, cc_bits, ru_bits):
        self.logger.info(f'xorif_set_fhi_eaxc_id: {du_bits}, {bs_bits}, {cc_bits}, {ru_bits}')
        return lib.xorif_set_fhi_eaxc_id(du_bits, bs_bits, cc_bits, ru_bits)

    # int xorif_set_ru_ports(uint16_t ru_bits, uint16_t ss_bits, uint16_t mask, uint16_t user_val, uint16_t prach_val, uint16_t ssb_val)
    def xorif_set_ru_ports(self, ru_bits, ss_bits, mask, user_val, prach_val, ssb_val):
        self.logger.info(f'xorif_set_ru_ports: {ru_bits}, {ss_bits}, 0x{mask:02x}, 0x{user_val:02x}, 0x{prach_val:02x}, 0x{ssb_val:02x}')
        return lib.xorif_set_ru_ports(ru_bits, ss_bits, mask, user_val, prach_val, ssb_val)

    # int xorif_set_ru_ports_alt1(uint16_t ru_bits, uint16_t ss_bits, uint16_t mask, uint16_t user_val, uint16_t prach_val, uint16_t ssb_val, uint16_t lte_val)
    def xorif_set_ru_ports_alt1(self, ru_bits, ss_bits, mask, user_val, prach_val, ssb_val, lte_val):
        self.logger.info(f'xorif_set_ru_ports_alt1: {ru_bits}, {ss_bits}, 0x{mask:02x}, 0x{user_val:02x}, 0x{prach_val:02x}, 0x{ssb_val:02x}, 0x{lte_val:02x}')
        return lib.xorif_set_ru_ports_alt1(ru_bits, ss_bits, mask, user_val, prach_val, ssb_val, lte_val)

    # int xorif_set_ru_ports_table_mode(uint16_t mode)
    def xorif_set_ru_ports_table_mode(self, mode):
        self.logger.info(f'xorif_set_ru_ports_table_mode: {mode}')
        return lib.xorif_set_ru_ports_table_mode(mode)

    # int xorif_clear_ru_ports_table(void)
    def xorif_clear_ru_ports_table(self):
        self.logger.info(f'xorif_clear_ru_ports_table:')
        return lib.xorif_clear_ru_ports_table()

    # int xorif_set_ru_ports_table(uint16_t address, uint16_t port, uint16_t type, uint16_t number)
    def xorif_set_ru_ports_table(self, address, port, type, number):
        self.logger.info(f'xorif_set_ru_ports_table: {address}, {port}, {type}, {number}')
        return lib.xorif_set_ru_ports_table(address, port, type, number)

    # int xorif_enable_fhi_interrupts(uint32_t mask)
    def xorif_enable_fhi_interrupts(self, mask):
        self.logger.info(f'xorif_enable_fhi_interrupts: 0x{mask:08x}')
        return lib.xorif_enable_fhi_interrupts(mask)

    # int xorif_set_system_constants(const struct xorif_system_constants *ptr)
    def xorif_set_system_constants(self, constants):
        self.logger.info(f'xorif_set_system_constants: {constants}')
        constants_ptr = ffi.new("const struct xorif_system_constants *", constants)
        return lib.xorif_set_system_constants(constants_ptr)

    # int xorif_set_symbol_strobe_source(uint16_t source)
    def xorif_set_symbol_strobe_source(self, source):
        self.logger.info(f't: {source}')
        return lib.xorif_set_symbol_strobe_source(source)

    # int xorif_register_fhi_isr(isr_func_t callback)
    def xorif_register_fhi_isr(self, callback):
        self.logger.info(f'xorif_register_fhi_isr: {callback}')
        self.logger.error(f'Call-back not supported in this version')
        return lib.XORIF_FAILURE

    #int xorif_monitor_clear()
    def xorif_monitor_clear(self):
        self.logger.info(f'xorif_monitor_clear:')
        return lib.xorif_monitor_clear()

    # int xorif_monitor_select(uint8_t stream)
    def xorif_monitor_select(self, stream):
        self.logger.info(f'xorif_monitor_select: {stream}')
        return lib.xorif_monitor_select(stream)

    # int xorif_monitor_snapshot(void)
    def xorif_monitor_snapshot(self):
        self.logger.info(f'xorif_monitor_clear:')
        return lib.xorif_monitor_clear()

    # int xorif_monitor_read(uint8_t counter, uint64_t *value)
    def xorif_monitor_read(self, counter):
        self.logger.info(f'xorif_monitor_read: {counter}')
        data_ptr = ffi.new("uint64_t *")
        result = lib.xorif_monitor_read(counter, data_ptr)
        return (result, data_ptr[0])
