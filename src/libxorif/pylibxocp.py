#!/usr/bin/env python3
#
# Copyright 2020 - 2023 Advanced Micro Devices, Inc.
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

# pylint: disable=too-many-public-methods
# pylint: disable=no-member

__author__ = "Steven Dickinson"
__copyright__ = "Copyright 2022, Advanced Micro Devices, Inc."

import os
import logging
from cffi import FFI

# Use local files if available, otherwise use the standard locations
lib_path = "./libxorif.so.1" if os.path.exists("./libxorif.so.1") else "/usr/lib/libxorif.so.1"
inc_path = "./xocp_api_cffi.h" if os.path.exists("./xocp_api_cffi.h") else "/usr/share/xorif/xocp_api_cffi.h"

# Load library and definitions with CFFI
ffi = FFI()
try:
    f = open(inc_path)
    ffi.cdef(f.read())
except Exception as e:
    print(f"Failed to open/read header '{inc_path}'")
    raise e
try:
    lib = ffi.dlopen(lib_path)
except Exception as e:
    print(f"Failed to open/load library '{lib_path}'")
    raise e

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

class LIBXOCP:
    """
    Python adapter for the libxocp shared library, using CFFI.

    The adaptor class provides a more Pythonic API, for example
    converting 'pointer to struct' objects into dictionaries,
    returning tuples rather than passing pointers in order to
    write-back results, etc.
    """

    _instance = None

    # Using singleton pattern
    def __new__(cls):
        if not cls._instance:
            cls._instance = super(LIBXOCP, cls).__new__(cls)

            # Set up logger
            logging.basicConfig(format="%(name)s> %(levelname)s: %(message)s")
            cls._instance.logger = logging.getLogger("PYXOCP")
            cls._instance.logger.setLevel(logging.ERROR)

            # We're going to expose all enums and integer #define constants
            constants = {}
            for a in dir(lib):
                if isinstance(getattr(lib, a), int):
                    constants[a] = getattr(lib, a)
            cls._instance._constants = constants

        return cls._instance

    @property
    def constants(self):
        # Dictionary of enums available as read-only property
        return self._constants

    def set_log_level(self, level):
        self.logger.setLevel(level)

    # void xocp_debug(uint16_t level)
    def xocp_debug(self, level):
        self.logger.info(f'xocp_debug: {level}')
        return lib.xocp_debug(level)

    # int xocp_start(const char *dev_name)
    def xocp_start(self, dev_name=None):
        self.logger.info('xocp_start:')
        return lib.xocp_start()

    # void xocp_finish(uint16_t instance)
    def xocp_finish(self, instance):
        self.logger.info(f'xocp_finish: {instance}')
        return lib.xocp_finish(instance)

    # int xocp_get_state(uint16_t instance)
    def xocp_get_state(self, instance):
        self.logger.info(f'xocp_get_state: {instance}')
        return lib.xocp_get_state(instance)

    # uint32_t xocp_get_sw_version(void)
    def xocp_get_sw_version(self):
        self.logger.info('xocp_get_sw_version:')
        value = lib.xocp_get_sw_version()
        major = (value >> 24) & 0xFF
        minor = (value >> 16) & 0xFF
        version = (value >> 8) & 0xFF
        return (major, minor, version)

    # uint32_t xocp_get_hw_version(uint16_t instance)
    def xocp_get_hw_version(self, instance):
        self.logger.info(f'xocp_get_hw_version: {instance}')
        value = lib.xocp_get_hw_version(instance)
        major = (value >> 24) & 0xFF
        minor = (value >> 16) & 0xFF
        version = (value >> 8) & 0xFF
        return (major, minor, version)

    # uint32_t xocp_get_hw_internal_rev(uint16_t instance)
    def xocp_get_hw_internal_rev(self, instance):
        self.logger.info(f'xocp_get_hw_internal_rev: {instance}')
        return lib.xocp_get_hw_internal_rev(instance)

    # int xocp_get_capabilities(uint16_t instance, struct xocp_caps *data)
    def xocp_get_capabilities(self, instance):
        self.logger.info(f'xocp_get_capabilities: {instance}')
        data_ptr = ffi.new("struct xocp_caps *")
        result = lib.xocp_get_capabilities(instance, data_ptr)
        return (result, cdata_to_py(data_ptr[0]))

    # int xocp_get_event_status(uint16_t instance, uint32_t *status)
    def xocp_get_event_status(self, instance):
        self.logger.info(f'xocp_get_event_status: {instance}')
        data_ptr = ffi.new("uint32_t *")
        result = lib.xocp_get_event_status(instance, data_ptr)
        return (result, data_ptr[0])

    # int xocp_clear_event_status(uint16_t instance)
    def xocp_clear_event_status(self, instance):
        self.logger.info(f'xocp_clear_event_status: {instance}')
        return lib.xocp_clear_event_status(instance)

    # int xocp_enable_interrupts(uint16_t instance, uint32_t mask)
    def xocp_enable_interrupts(self, instance, mask):
        self.logger.info(f'xocp_enable_interrupts: {instance}, 0x{mask:X}')
        return lib.xocp_enable_interrupts(instance, mask)

    # int xocp_register_event_callback(uint16_t instance, xocp_isr_func_t callback)
    def xocp_register_event_callback(self, instance, callback):
        self.logger.info(f'xocp_register_event_callback: {instance}, {callback}')
        return lib.xocp_register_event_callback(instance, callback)

    # int xocp_read_reg(uint16_t instance, const char *name, uint32_t *value)
    def xocp_read_reg(self, instance, name):
        self.logger.info(f'xocp_read_reg: {instance}, {name}')
        data_ptr = ffi.new("uint32_t *")
        result = lib.xocp_read_reg(instance, bytes(name, "utf-8"), data_ptr)
        return (result, data_ptr[0])

    # int xocp_read_reg_offset(uint16_t instance, const char *name, uint16_t offset, uint32_t *value)
    def xocp_read_reg_offset(self, instance, name, offset):
        self.logger.info(f'xocp_read_reg_offset: {instance}, {name}, 0x{offset:X}')
        data_ptr = ffi.new("uint32_t *")
        result = lib.xocp_read_reg_offset(instance, bytes(name, "utf-8"), offset, data_ptr)
        return (result, data_ptr[0])

    # int xocp_write_reg(uint16_t instance, const char *name, uint32_t value)
    def xocp_write_reg(self, instance, name, value):
        self.logger.info(f'xocp_write_reg: {instance}, {name}, 0x{value:X}')
        return lib.xocp_write_reg(instance, bytes(name, "utf-8"), value)

    # int xocp_write_reg_offset(uint16_t instance, const char *name, uint16_t offset, uint32_t value)
    def xocp_write_reg_offset(self, instance, name, offset, value):
        self.logger.info(f'xocp_write_reg_offset: {instance}, {name}, 0x{offset:X}, 0x{value:X}')
        return lib.xocp_write_reg_offset(instance, bytes(name, "utf-8"), offset, value)

    # int xocp_reset(uint16_t instance, mode)
    def xocp_reset(self, instance, mode):
        self.logger.info(f'xocp_reset: {instance}, {mode}')
        return lib.xocp_reset(instance, mode)

    # int xocp_activate(uint16_t instance)
    def xocp_activate(self, instance):
        self.logger.info(f'xocp_activate: {instance}')
        return lib.xocp_activate(instance)

    # int xocp_get_cc_cfg(uint16_t instance, uint8_t cc, struct xocp_cc_data *data)
    def xocp_get_cc_cfg(self, instance, cc):
        self.logger.info(f'xocp_get_cc_cfg: {instance}, {cc}')
        data_ptr = ffi.new("struct xocp_cc_data *")
        result = lib.xocp_get_cc_cfg(instance, cc, data_ptr)
        return (result, cdata_to_py(data_ptr[0]))

    def xocp_get_cc_cfg_struct(self):
        self.logger.info(f'xocp_get_cc_cfg_struct:')
        data_ptr = ffi.new("struct xocp_cc_data *")
        return cdata_to_py(data_ptr[0])

    # int xocp_set_cc_cfg(uint16_t instance, uint8_t cc, const struct xocp_cc_data *data)
    def xocp_set_cc_cfg(self, instance, cc, data):
        self.logger.info(f'xocp_set_cc_cfg: {instance}, {cc}, {data}')
        data_ptr = ffi.new("const struct xocp_cc_data *", data)
        return lib.xocp_set_cc_cfg(instance, cc, data_ptr)

    # int xocp_get_antenna_cfg(uint16_t instance, struct xocp_antenna_data *data)
    def xocp_get_antenna_cfg(self, instance):
        self.logger.info(f'xocp_get_antenna_cfg: {instance}')
        data_ptr = ffi.new("struct xocp_antenna_data *")
        result = lib.xocp_get_antenna_cfg(instance, data_ptr)
        return (result, cdata_to_py(data_ptr[0]))

    def xocp_get_antenna_cfg_struct(self):
        self.logger.info(f'xocp_get_antenna_cfg_struct:')
        data_ptr = ffi.new("struct xocp_antenna_data *")
        return cdata_to_py(data_ptr[0])

    # int xocp_set_antenna_cfg(uint16_t instance, const struct xocp_cc_data *data)
    def xocp_set_antenna_cfg(self, instance, data):
        self.logger.info(f'xocp_set_antenna_cfg: {instance}, {data}')
        data_ptr = ffi.new("const struct xocp_antenna_data *", data)
        return lib.xocp_set_antenna_cfg(instance, data_ptr)

    # int xocp_set_schedule(uint16_t instance, uint8_t mode, uint8_t length, const uint8_t sequence[])
    def xocp_set_schedule(self, instance, mode, length, sequence):
        self.logger.info(f'xocp_set_schedule: {instance}, {mode}, {length}, {sequence}')
        data_ptr = ffi.new("const uint8_t sequence[]", sequence)
        return lib.xocp_set_schedule(instance, mode, length, data_ptr)

    # int xocp_get_trigger_cfg(uint16_t instance, struct xocp_triggers *triggers)
    def xocp_get_trigger_cfg(self, instance):
        self.logger.info(f'xocp_get_trigger_cfg: {instance}')
        data_ptr = ffi.new("struct xocp_triggers *")
        result = lib.xocp_get_trigger_cfg(instance, data_ptr)
        return (result, cdata_to_py(data_ptr[0]))

    def xocp_get_trigger_cfg_struct(self):
        self.logger.info(f'xocp_get_trigger_cfg_struct:')
        data_ptr = ffi.new("struct xocp_triggers *")
        return cdata_to_py(data_ptr[0])

    # int xocp_set_trigger_cfg(uint16_t instance, const struct xocp_triggers *triggers)
    def xocp_set_trigger_cfg(self, instance, triggers):
        self.logger.info(f'xocp_set_trigger_cfg: {instance}, {triggers}')
        data_ptr = ffi.new("const struct xocp_triggers *", triggers)
        return lib.xocp_set_trigger_cfg(instance, data_ptr)

    # int xocp_trigger_update(uint16_t instance)
    def xocp_trigger_update(self, instance):
        self.logger.info(f'xocp_trigger_update: {instance}')
        return lib.xocp_trigger_update(instance)

    # int xocp_monitor_clear(uint16_t instance)
    def xocp_monitor_clear(self, instance):
        self.logger.info(f'xocp_monitor_clear: {instance}')
        return lib.xocp_monitor_clear(instance)

    # int xocp_monitor_snapshot(uint16_t instance)
    def xocp_monitor_snapshot(self, instance):
        self.logger.info(f'xocp_monitor_snapshot: {instance}')
        return lib.xocp_monitor_snapshot(instance)

    # int xocp_monitor_read(uint16_t instance, uint8_t counter, uint64_t *value)
    def xocp_monitor_read(self, instance, counter):
        self.logger.info(f'xocp_monitor_read: {instance}, {counter}')
        data_ptr = ffi.new("uint64_t *")
        result = lib.xocp_monitor_read(instance, counter, data_ptr)
        return (result, data_ptr[0])
