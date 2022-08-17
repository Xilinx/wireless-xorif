/*
 * Copyright 2020 - 2022 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file xocp_registers.c
 * @author Steven Dickinson
 * @brief Source file for libxocp register access functions/definitions.
 * @addtogroup libxocp
 * @{
 */

#include "xocp.h"
#include "xocp_registers.h"

/************************/
/*** Data definitions ***/
/************************/

// The following const structure defines the register map
// Note, this array is sorted for more efficient access
static const reg_info_t reg_map[] =
{
    {"CFG_DL_CC_UPDATE_TRIGGERED", 0x18, 0x1, 0, 1},
    {"CFG_DL_CC_UPDATE_TRIGGERED_EN", 0x14, 0x1, 0, 1},
    {"CFG_DL_SEQUENCE_ERROR", 0x18, 0x2, 1, 1},
    {"CFG_DL_SEQUENCE_ERROR_EN", 0x14, 0x2, 1, 1},
    {"CFG_DL_SEQUENCE_TABLE_ERROR", 0x18, 0x4, 2, 1},
    {"CFG_DL_SEQUENCE_TABLE_ERROR_EN", 0x14, 0x4, 2, 1},
    {"CFG_INTERNAL_REVISION", 0x4, 0xffffffff, 0, 32},
    {"CFG_MAJOR_REVISION", 0x0, 0xff000000, 24, 8},
    {"CFG_MASTER_INT_ENABLE", 0x10, 0x1, 0, 1},
    {"CFG_MAX_ANTENNA_INTERLEAVE", 0x8, 0x70000, 16, 3},
    {"CFG_MAX_NUM_ANTENNA", 0x8, 0xf00, 8, 4},
    {"CFG_MAX_NUM_CC", 0x8, 0xf, 0, 4},
    {"CFG_MINOR_REVISION", 0x0, 0xff0000, 16, 8},
    {"CFG_MONITOR_CLEAR", 0x110, 0x1, 0, 1},
    {"CFG_MONITOR_READ_31__0", 0x120, 0xffffffff, 0, 32},
    {"CFG_MONITOR_READ_63_32", 0x124, 0xffffffff, 0, 32},
    {"CFG_MONITOR_SAMPLE", 0x10c, 0x1, 0, 1},
    {"CFG_MONITOR_SELECT_READ", 0x104, 0x3f, 0, 6},
    {"CFG_MONITOR_SNAPSHOT", 0x108, 0x1, 0, 1},
    {"CFG_UL_CC_UPDATE_TRIGGERED", 0x18, 0x100, 8, 1},
    {"CFG_UL_CC_UPDATE_TRIGGERED_EN", 0x14, 0x100, 8, 1},
    {"CFG_UL_SEQUENCE_ERROR", 0x18, 0x200, 9, 1},
    {"CFG_UL_SEQUENCE_ERROR_EN", 0x14, 0x200, 9, 1},
    {"CFG_UL_SEQUENCE_TABLE_ERROR", 0x18, 0x400, 10, 1},
    {"CFG_UL_SEQUENCE_TABLE_ERROR_EN", 0x14, 0x400, 10, 1},
    {"CFG_VERSION_REVISION", 0x0, 0xff00, 8, 8},
    {"CTRL_CURRENT_DL_CC_NUMBER_OF_RES", 0x4014, 0x1fff, 0, 13},
    {"CTRL_CURRENT_UL_CC_NUMBER_OF_RES", 0x4214, 0x1fff, 0, 13},
    {"CTRL_DL_ANTENNA_INTERLEAVE", 0x4008, 0x7, 0, 3},
    {"CTRL_DL_NUMBER_OF_ANTENNAS", 0x4004, 0xf, 0, 4},
    {"CTRL_DL_SEQUENCE_TABLE_CC_SEL", 0x400c, 0xf0, 4, 4},
    {"CTRL_DL_SEQUENCE_TABLE_ENABLE", 0x400c, 0x40000000, 30, 1},
    {"CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE", 0x400c, 0x1, 0, 1},
    {"CTRL_DL_SEQUENCE_TABLE_WR_STROBE", 0x400c, 0x80000000, 31, 1},
    {"CTRL_NEXT_DL_CC_NUMBER_OF_RES", 0x4010, 0x1fff, 0, 13},
    {"CTRL_NEXT_UL_CC_NUMBER_OF_RES", 0x4210, 0x1fff, 0, 13},
    {"CTRL_UL_ANTENNA_INTERLEAVE", 0x4208, 0x7, 0, 3},
    {"CTRL_UL_NUMBER_OF_ANTENNAS", 0x4204, 0xf, 0, 4},
    {"CTRL_UL_SEQUENCE_TABLE_CC_SEL", 0x420c, 0xf0, 4, 4},
    {"CTRL_UL_SEQUENCE_TABLE_ENABLE", 0x420c, 0x40000000, 30, 1},
    {"CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE", 0x420c, 0x1, 0, 1},
    {"CTRL_UL_SEQUENCE_TABLE_WR_STROBE", 0x420c, 0x80000000, 31, 1},
    {"FRAM_CHANP_DL_DISABLE", 0x2000, 0x1, 0, 1},
    {"FRAM_CHANP_DL_READY", 0x2000, 0x2, 1, 1},
    {"FRAM_CHANP_UL_DISABLE", 0x2000, 0x4, 2, 1},
    {"FRAM_CHANP_UL_READY", 0x2000, 0x8, 3, 1},
    {"SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_ANT_CONFIG", 0x6808, 0xffffffff, 0, 32},
    {"SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ID", 0x6804, 0xf, 0, 4},
    {"SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ISG", 0x6804, 0xfff0000, 16, 12},
    {"SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_NUM", 0x6804, 0xf00, 8, 4},
    {"SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_FINAL", 0x6804, 0x40000000, 30, 1},
    {"SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_LAST", 0x6804, 0x80000000, 31, 1},
    {"SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_NUM_RES", 0x6800, 0xfff, 0, 12},
    {"SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_RE_START", 0x6800, 0xfff0000, 16, 12},
    {"SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_ANT_CONFIG", 0x6c08, 0xffffffff, 0, 32},
    {"SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_ID", 0x6c04, 0xf, 0, 4},
    {"SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_NUM", 0x6c04, 0xf00, 8, 4},
    {"SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_FINAL", 0x6c04, 0x40000000, 30, 1},
    {"SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_LAST", 0x6c04, 0x80000000, 31, 1},
    {"SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_NUM_RES", 0x6c00, 0xfff, 0, 12},
    {"SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_RE_START", 0x6c00, 0xfff0000, 16, 12},
    {"SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_ANT_CONFIG", 0x6008, 0xffffffff, 0, 32},
    {"SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ID", 0x6004, 0xf, 0, 4},
    {"SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ISG", 0x6004, 0xfff0000, 16, 12},
    {"SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_NUM", 0x6004, 0xf00, 8, 4},
    {"SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_FINAL", 0x6004, 0x40000000, 30, 1},
    {"SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_LAST", 0x6004, 0x80000000, 31, 1},
    {"SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_NUM_RES", 0x6000, 0xfff, 0, 12},
    {"SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_RE_START", 0x6000, 0xfff0000, 16, 12},
    {"SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_ANT_CONFIG", 0x6408, 0xffffffff, 0, 32},
    {"SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_ID", 0x6404, 0xf, 0, 4},
    {"SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_NUM", 0x6404, 0xf00, 8, 4},
    {"SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_FINAL", 0x6404, 0x40000000, 30, 1},
    {"SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_LAST", 0x6404, 0x80000000, 31, 1},
    {"SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_NUM_RES", 0x6400, 0xfff, 0, 12},
    {"SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_RE_START", 0x6400, 0xfff0000, 16, 12},
    // Last line has to be NULL, 0, 0, 0
    {NULL, 0, 0, 0},
};

/****************************/
/*** Function definitions ***/
/****************************/

uint32_t read_reg(void *io,
                  const char *name,
                  uint32_t addr,
                  uint32_t mask,
                  uint8_t shift,
                  uint8_t width)
{
    ASSERT_NV(io, 0);

    uint32_t x;
#ifdef NO_HW
    // Read from fake register
    x = ((uint32_t *)io)[addr / 4];
#else
    // Read with libmetal
    x = metal_io_read32((struct metal_io_region *)io, addr);
#endif
    x = (x & mask) >> shift;

    TRACE("READ_REG: %s (0x%04X)[%d:%d] => 0x%X (%u)\n", name, addr, shift + width - 1, shift, x, x);
    return x;
}

void write_reg(void *io,
               const char *name,
               uint32_t addr,
               uint32_t mask,
               uint8_t shift,
               uint8_t width,
               uint32_t value)
{
    ASSERT_V(io);

    uint32_t x;
#ifdef NO_HW
    // Read from fake register
    x = ((uint32_t *)io)[addr / 4];
#else
    // Read with libmetal
    x = metal_io_read32((struct metal_io_region *)io, addr);
#endif
    // Modify register field
    x = (x & ~mask) | ((value << shift) & mask);
#ifdef NO_HW
    // Write to fake register
    ((uint32_t *)io)[addr / 4] = x;
#else
    // Write with libmetal
    metal_io_write32((struct metal_io_region *)io, addr, x);
#endif

    TRACE("WRITE_REG: %s (0x%04X)[%d:%d] <= 0x%X (%u)\n", name, addr, shift + width - 1, shift, value, value);
}

/**
 * @brief Comparator function for bsearch algorithm.
 * @param[in] key Key to look for
 * @param[in] data Data element that we're comparing against
 * @returns
 *      - <0 if key is before data
 *      - =0 if key is equal to data
 *      - >0 if key is after data
 */
static int reg_comparator(const void *key, const void *data)
{
    return strcmp((const char *)key, ((const reg_info_t *)data)->name);
}

const reg_info_t *find_register(const char *name)
{
    static reg_info_t dummy_reg = { "OFFSET", 0, 0xffffffff, 0, 32 };
    uint32_t u;

    void *result = bsearch((const void *)name,
                           (const void *)reg_map,
                           (sizeof(reg_map) / sizeof(reg_info_t)) - 1,
                           sizeof(reg_info_t),
                           reg_comparator);

    if (result)
    {
        return (const reg_info_t *)result;
    }
    else if ((sscanf(name, "0x%X", &u) == 1) || (sscanf(name, "%u", &u) == 1))
    {
        // Interpret the "name" as an address, with 32-bit width
        // Set the address in the "dummy_reg" entry
        dummy_reg.addr = u;
        return (const reg_info_t *)&dummy_reg;
    }
    else
    {
        return NULL;
    }
}

/** @} */