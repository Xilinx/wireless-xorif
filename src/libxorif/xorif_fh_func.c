/*
 * Copyright 2020 Xilinx, Inc.
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
 * @file xorif_fh_func.c
 * @author Steven Dickinson
 * @brief Source file for libxorif Front-Haul Interface (FHI) functions.
 * @addtogroup libxorif
 * @{
 */

#include <math.h>
#include "xorif_api.h"
#include "xorif_fh_func.h"
#include "xorif_common.h"
#include "xorif_utils.h"
#include "oran_radio_if_v1_0_ctrl.h"

// Constants for time advance calculation
uint32_t XRAN_TIMER_CLK = 2500;            // 2,500 ps
const uint32_t FH_DECAP_DLY = 5000000;     // 5,000,000 ps
const uint32_t UL_RADIO_CH_DLY = 30000000; // 30,000,000 ps

// The following const structure defines the register map for the Front Haul Interface
// Note, this array is sorted for more efficient access
const reg_info_t fhi_reg_map[] =
{
    {"CFG_AXI_TIMEOUT_ENABLE", 0x14, 0x80000000, 31},
    {"CFG_AXI_TIMEOUT_STATUS", 0x18, 0x80000000, 31},
    {"CFG_CONFIG_NO_OF_DEFM_ANTS", 0x20, 0xffff0000, 16},
    {"CFG_CONFIG_NO_OF_ETH_PORTS", 0x24, 0x3ff, 0},
    {"CFG_CONFIG_NO_OF_FRAM_ANTS", 0x20, 0xffff, 0},
    {"CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX", 0x44, 0xffff, 0},
    {"CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS", 0x70, 0xffff, 0},
    {"CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH", 0x6c, 0xffff, 0},
    {"CFG_CONFIG_XRAN_FRAM_AUTO_START", 0x40, 0xffff, 0},
    {"CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX", 0x3c, 0xffff, 0},
    {"CFG_CONFIG_XRAN_MAX_CC", 0x2c, 0x1f, 0},
    {"CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS", 0x4c, 0xffff, 0},
    {"CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS", 0x54, 0xffff, 0},
    {"CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS", 0x58, 0xffff, 0},
    {"CFG_CONFIG_XRAN_MAX_DL_SECT_PER_SYMBOL", 0x60, 0xffff, 0},
    {"CFG_CONFIG_XRAN_MAX_DL_SYMBOLS", 0x30, 0x1f, 0},
    {"CFG_CONFIG_XRAN_MAX_NUMEROLOGY", 0x34, 0x7, 0},
    {"CFG_CONFIG_XRAN_MAX_SCS", 0x48, 0xffff, 0},
    {"CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS", 0x50, 0xffff, 0},
    {"CFG_CONFIG_XRAN_MAX_UL_SECT_PER_SYMBOL", 0x64, 0xffff, 0},
    {"CFG_CONFIG_XRAN_MIN_NUMEROLOGY", 0x38, 0x7, 0},
    {"CFG_CONFIG_XRAN_PRACH_C_PORTS", 0x74, 0xffff, 0},
    {"CFG_CONFIG_XRAN_SUPPORT_MODE", 0x28, 0xf, 0},
    {"CFG_CONFIG_XRAN_TIMER_CLK_PS", 0x5c, 0xffff, 0},
    {"CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM", 0x68, 0xffff, 0},
    {"CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF", 0x14, 0x400, 10},
    {"CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF", 0x14, 0x800, 11},
    {"CFG_DEFM_INT_ENA_INFIFO_OF", 0x14, 0x100, 8},
    {"CFG_DEFM_INT_ENA_INFIFO_UF", 0x14, 0x200, 9},
    {"CFG_DEFM_INT_ETH_PIPE_C_BUF_OF", 0x18, 0x400, 10},
    {"CFG_DEFM_INT_ETH_PIPE_TABLE_OF", 0x18, 0x800, 11},
    {"CFG_DEFM_INT_INFIFO_OF", 0x18, 0x100, 8},
    {"CFG_DEFM_INT_INFIFO_UF", 0x18, 0x200, 9},
    {"CFG_DEFM_RESET_STATUS", 0x18, 0x2, 1},
    {"CFG_FRAM_INT_ENA_OUTFIFO_OF", 0x14, 0x1000, 12},
    {"CFG_FRAM_INT_ENA_OUTFIFO_UF", 0x14, 0x2000, 13},
    {"CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND", 0x14, 0x8000, 15},
    {"CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW", 0x14, 0x4000, 14},
    {"CFG_FRAM_INT_OUTFIFO_OF", 0x18, 0x1000, 12},
    {"CFG_FRAM_INT_OUTFIFO_UF", 0x18, 0x2000, 13},
    {"CFG_FRAM_INT_PRACH_SECTION_NOTFOUND", 0x18, 0x8000, 15},
    {"CFG_FRAM_INT_PRACH_SECTION_OVERFLOW", 0x18, 0x4000, 14},
    {"CFG_FRAM_RESET_STATUS", 0x18, 0x1, 0},
    {"CFG_INTERNAL_REVISION", 0x4, 0xffffffff, 0},
    {"CFG_INTERRUPT_STATUS_SAMPLE", 0x1c, 0x1, 0},
    {"CFG_MAJOR_REVISION", 0x0, 0xff000000, 24},
    {"CFG_MASTER_INT_ENABLE", 0x10, 0x1, 0},
    {"CFG_MINOR_REVISION", 0x0, 0xff0000, 16},
    {"CFG_TIMEOUT_VALUE", 0x8, 0xfff, 0},
    {"CFG_USER_RO_IN", 0xc, 0xff0000, 16},
    {"CFG_USER_RW_OUT", 0xc, 0xff, 0},
    {"CFG_VERSION_REVISION", 0x0, 0xff00, 8},
    {"DEFM_ERR_PACKET_FILTER", 0x6004, 0x3, 0},
    {"DEFM_READY", 0x6000, 0x2, 1},
    {"DEFM_RESTART", 0x6000, 0x1, 0},
    {"DEFM_USER_DATA_FILTER_W0_127_96", 0x610c, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W0_31_0", 0x6100, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W0_63_32", 0x6104, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W0_95_64", 0x6108, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W0_MASK", 0x6110, 0xffff, 0},
    {"DEFM_USER_DATA_FILTER_W1_127_96", 0x612c, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W1_31_0", 0x6120, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W1_63_32", 0x6124, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W1_95_64", 0x6128, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W1_MASK", 0x6130, 0xffff, 0},
    {"DEFM_USER_DATA_FILTER_W2_127_96", 0x614c, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W2_31_0", 0x6140, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W2_63_32", 0x6144, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W2_95_64", 0x6148, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W2_MASK", 0x6150, 0xffff, 0},
    {"DEFM_USER_DATA_FILTER_W3_127_96", 0x616c, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W3_31_0", 0x6160, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W3_63_32", 0x6164, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W3_95_64", 0x6168, 0xffffffff, 0},
    {"DEFM_USER_DATA_FILTER_W3_MASK", 0x6170, 0xffff, 0},
    {"DEFM_XRAN_BANDSECTOR_FIELD_WIDTH", 0x6080, 0xf00, 8},
    {"DEFM_XRAN_CC_ID_FIELD_WIDTH", 0x6080, 0xf0, 4},
    {"DEFM_XRAN_RU_PORT_ID_FIELD_WIDTH", 0x6080, 0xf, 0},
    {"ETH_DEST_ADDR_31_0", 0xa000, 0xffffffff, 0},
    {"ETH_DEST_ADDR_47_32", 0xa004, 0xffff, 0},
    {"ETH_IPV4_DESTINATION_ADD", 0xa04c, 0xffffffff, 0},
    {"ETH_IPV4_DSCP", 0xa034, 0x3f, 0},
    {"ETH_IPV4_ECN", 0xa034, 0xc0, 6},
    {"ETH_IPV4_FLAGS", 0xa03c, 0x7, 0},
    {"ETH_IPV4_FRAGMENT_OFFSET", 0xa03c, 0xfff8, 3},
    {"ETH_IPV4_ID", 0xa038, 0xffff, 0},
    {"ETH_IPV4_IHL", 0xa030, 0xf0, 4},
    {"ETH_IPV4_PROTOCOL", 0xa044, 0xff, 0},
    {"ETH_IPV4_SOURCE_ADD", 0xa048, 0xffffffff, 0},
    {"ETH_IPV4_TIME_TO_LIVE", 0xa040, 0xff, 0},
    {"ETH_IPV4_VERSION", 0xa030, 0xf, 0},
    {"ETH_IPV6_DESTINATION_ADD_127_96", 0xa0b0, 0xffffffff, 0},
    {"ETH_IPV6_DESTINATION_ADD_31_0", 0xa0a4, 0xffffffff, 0},
    {"ETH_IPV6_DESTINATION_ADD_63_32", 0xa0a8, 0xffffffff, 0},
    {"ETH_IPV6_DESTINATION_ADD_95_64", 0xa0ac, 0xffffffff, 0},
    {"ETH_IPV6_FLOW_LABEL", 0xa088, 0xfffff, 0},
    {"ETH_IPV6_HOP_LIMIT", 0xa090, 0xff, 0},
    {"ETH_IPV6_NEXT_HEADER", 0xa08c, 0xff, 0},
    {"ETH_IPV6_SOURCE_ADD_127_96", 0xa0a0, 0xffffffff, 0},
    {"ETH_IPV6_SOURCE_ADD_31_0", 0xa094, 0xffffffff, 0},
    {"ETH_IPV6_SOURCE_ADD_63_32", 0xa098, 0xffffffff, 0},
    {"ETH_IPV6_SOURCE_ADD_95_64", 0xa09c, 0xffffffff, 0},
    {"ETH_IPV6_TRAFFIC_CLASS", 0xa084, 0xff, 0},
    {"ETH_IPV6_V", 0xa080, 0xf, 0},
    {"ETH_SRC_ADDR_31_0", 0xa008, 0xffffffff, 0},
    {"ETH_SRC_ADDR_47_32", 0xa00c, 0xffff, 0},
    {"ETH_UDP_DESTINATION_PORT", 0xa050, 0xffff0000, 16},
    {"ETH_UDP_SOURCE_PORT", 0xa050, 0xffff, 0},
    {"ETH_VLAN_DEI", 0xa010, 0x1000, 12},
    {"ETH_VLAN_ID", 0xa010, 0xfff, 0},
    {"ETH_VLAN_PCP", 0xa010, 0xe000, 13},
    {"FRAM_DISABLE", 0x2000, 0x1, 0},
    {"FRAM_FIFO_FULL_INDICATOR", 0x2004, 0xffffffff, 0},
    {"FRAM_GEN_VLAN_TAG", 0x2200, 0x10, 4},
    {"FRAM_PROTOCOL_DEFINITION", 0x2200, 0xf, 0},
    {"FRAM_READY", 0x2000, 0x2, 1},
    {"FRAM_SEL_IPV_ADDRESS_TYPE", 0x2200, 0x60, 5},
    {"FRAM_XRAN_BANDSECTOR_FIELD_WIDTH", 0x2080, 0xf00, 8},
    {"FRAM_XRAN_CC_ID_FIELD_WIDTH", 0x2080, 0xf0, 4},
    {"FRAM_XRAN_RU_PORT_ID_FIELD_WIDTH", 0x2080, 0xf, 0},
    {"ORAN_CC_DL_CTRL_OFFSETS", 0xe104, 0xffff, 0},
    {"ORAN_CC_DL_CTRL_SYM_NUM_INDEX", 0xe114, 0x3f0000, 16},
    {"ORAN_CC_DL_CTRL_UNROLLED_OFFSETS", 0xe108, 0xffff, 0},
    {"ORAN_CC_DL_DATA_SYM_NUM_INDEX", 0xe114, 0x3f00, 8},
    {"ORAN_CC_DL_DATA_SYM_START_INDEX", 0xe114, 0x3f, 0},
    {"ORAN_CC_DL_DATA_UNROLL_OFFSET", 0xe500, 0xffff, 0},
    {"ORAN_CC_DL_MPLANE_UDCOMP_PARAM", 0xe11c, 0x100, 8},
    {"ORAN_CC_DL_SETUP_C_ABS_SYMBOL", 0xe130, 0xfff, 0},
    {"ORAN_CC_DL_SETUP_C_CYCLES", 0xe134, 0x1ffff, 0},
    {"ORAN_CC_DL_SETUP_D_CYCLES", 0xe138, 0x1ffff, 0},
    {"ORAN_CC_DL_UD_COMP_METH", 0xe11c, 0xf0, 4},
    {"ORAN_CC_DL_UD_IQ_WIDTH", 0xe11c, 0xf, 0},
    {"ORAN_CC_ENABLE", 0xe004, 0xff, 0},
    {"ORAN_CC_MAX_SYMBOLS", 0xe158, 0xfff, 0},
    {"ORAN_CC_MODVALS_DL", 0xe168, 0xffff, 0},
    {"ORAN_CC_MODVALS_UL", 0xe16c, 0xffff, 0},
    {"ORAN_CC_NUM_CTRL_PER_SYMBOL_DL", 0xe160, 0xffff, 0},
    {"ORAN_CC_NUM_CTRL_PER_SYMBOL_UL", 0xe164, 0xffff, 0},
    {"ORAN_CC_NUMEROLOGY", 0xe100, 0x70000, 16},
    {"ORAN_CC_NUMRBS", 0xe100, 0x1ff, 0},
    {"ORAN_CC_RELOAD", 0xe000, 0xff, 0},
    {"ORAN_CC_SYMPERSLOT", 0xe100, 0x1000000, 24},
    {"ORAN_CC_UL_BASE_OFFSET", 0xe140, 0xffff, 0},
    {"ORAN_CC_UL_CTRL_OFFSETS", 0xe10c, 0xffff, 0},
    {"ORAN_CC_UL_CTRL_SYM_NUM_INDEX", 0xe114, 0x3f000000, 24},
    {"ORAN_CC_UL_CTRL_UNROLLED_OFFSETS", 0xe110, 0xffff, 0},
    {"ORAN_CC_UL_MPLANE_UDCOMP_PARAM", 0xe118, 0x100, 8},
    {"ORAN_CC_UL_SETUP_C_ABS_SYMBOL", 0xe120, 0xfff, 0},
    {"ORAN_CC_UL_SETUP_C_CYCLES", 0xe124, 0x1ffff, 0},
    {"ORAN_CC_UL_SETUP_D_CYCLES", 0xe128, 0x1ffff, 0},
    {"ORAN_CC_UL_UD_COMP_METH", 0xe118, 0xf0, 4},
    {"ORAN_CC_UL_UD_IQ_WIDTH", 0xe118, 0xf, 0},
    {"ORAN_SETUP_CNT", 0xe600, 0xffffffff, 0},
    {"ORAN_SETUP_SF", 0xe608, 0xf, 0},
    {"ORAN_SETUP_SL", 0xe60c, 0xf, 0},
    {"ORAN_SETUP_SY", 0xe610, 0xf, 0},
    {"ORAN_STRIP_FCS", 0xe604, 0x1, 0},
    {"STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT", 0xc008, 0xffffffff, 0},
    {"STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT", 0xc004, 0xffffffff, 0},
    {"STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT", 0xc000, 0xffffffff, 0},
    {"STATS_ETH_STATS_USER_CTRL_RX_BAD_FCS_CNT", 0xc028, 0xffffffff, 0},
    {"STATS_ETH_STATS_USER_CTRL_RX_BAD_PKT_CNT", 0xc024, 0xffffffff, 0},
    {"STATS_ETH_STATS_USER_CTRL_RX_GOOD_PKT_CNT", 0xc020, 0xffffffff, 0},
    {"STATS_ETH_STATS_USER_CTRL_RX_PACKETS_CNT", 0xc01c, 0xffffffff, 0},
    {"STATS_ETH_STATS_USER_CTRL_RX_PKTS_RATE", 0xc030, 0xffffffff, 0},
    {"STATS_ETH_STATS_USER_DATA_RX_BAD_FCS_CNT", 0xc018, 0xffffffff, 0},
    {"STATS_ETH_STATS_USER_DATA_RX_BAD_PKT_CNT", 0xc014, 0xffffffff, 0},
    {"STATS_ETH_STATS_USER_DATA_RX_GOOD_PKT_CNT", 0xc010, 0xffffffff, 0},
    {"STATS_ETH_STATS_USER_DATA_RX_PACKETS_CNT", 0xc00c, 0xffffffff, 0},
    {"STATS_ETH_STATS_USER_DATA_RX_PKTS_RATE", 0xc02c, 0xffffffff, 0},
    // Last line has to be NULL, 0, 0, 0
    {NULL, 0, 0, 0},
};

// Macros to access regsiter map header file
#define NAME(a) (#a)
#define ADDR(a) (a##_ADDR)
#define MASK(a) (a##_MASK)
#define SHIFT(a) (a##_OFFSET)

/**
 * @brief Macro handles read from specified register field.
 */
#define READ_REG(a) read_reg(fh_device.io, #a, ADDR(a), MASK(a), SHIFT(a))

/**
 * @brief Macro handles read from specified register field (address + offset).
 */
#define READ_REG_OFFSET(a, o) read_reg_offset(fh_device.io, #a, ADDR(a), o, MASK(a), SHIFT(a))

/**
 * @brief Macro handles write to specified register field.
 */
#define WRITE_REG(a, v) write_reg(fh_device.io, #a, ADDR(a), MASK(a), SHIFT(a), v)

/**
 * @brief Macro handles write to specified register field (address + offset).
 */
#define WRITE_REG_OFFSET(a, o, v) write_reg_offset(fh_device.io, #a, ADDR(a), o, MASK(a), SHIFT(a), v)

// Interrupt macros
#define FHI_INTR_ENABLE_ADDR CFG_DEFM_INT_ENA_INFIFO_OF_ADDR
#define FHI_INTR_STATUS_ADDR CFG_DEFM_INT_INFIFO_OF_ADDR
#define FHI_INTR_MASK (CFG_DEFM_INT_INFIFO_OF_MASK |              \
                       CFG_DEFM_INT_INFIFO_UF_MASK |              \
                       CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_MASK |      \
                       CFG_DEFM_INT_ETH_PIPE_TABLE_OF_MASK |      \
                       CFG_FRAM_INT_OUTFIFO_OF_MASK |             \
                       CFG_FRAM_INT_OUTFIFO_UF_MASK |             \
                       CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_MASK | \
                       CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_MASK | \
                       CFG_AXI_TIMEOUT_STATUS_MASK)

// FHI alarm flags
enum xorif_fhi_alarms fhi_alarm_status;

// Local function prototypes...
static int get_left_cc(uint16_t cc);
static int get_right_cc(uint16_t cc);
static uint16_t calc_sym_num(uint16_t numerology, uint16_t extended_cp, uint32_t time);
static uint16_t calc_dl_data_buff_size(uint16_t num_rbs, enum xorif_iq_comp comp_mode, uint16_t comp_width);
static int fhi_irq_handler(int id, void *data);

// API functions...

void xorif_reset_fhi(void)
{
    TRACE("xorif_reset_fhi()\n");

    // Deactivate / disable
    WRITE_REG(FRAM_DISABLE, 1);
    WRITE_REG(DEFM_RESTART, 1);
    WRITE_REG(ORAN_CC_ENABLE, 0);

    // Clear alarms
    fhi_alarm_status = 0;

    // Reactivate / enable
    WRITE_REG(DEFM_RESTART, 0);
    WRITE_REG(FRAM_DISABLE, 0);

    return;
}

uint32_t xorif_get_fhi_hw_version(void)
{
    TRACE("xorif_get_fhi_hw_version()\n");

    uint32_t major = READ_REG(CFG_MAJOR_REVISION);
    uint32_t minor = READ_REG(CFG_MINOR_REVISION);
    uint32_t revision = READ_REG(CFG_VERSION_REVISION);

    return major << 24 | minor << 16 | revision;
}

uint32_t xorif_get_fhi_hw_revision(void)
{
    TRACE("xorif_get_fhi_hw_revision()\n");
    return READ_REG(CFG_INTERNAL_REVISION);
}

uint32_t xorif_get_fhi_alarms(void)
{
    TRACE("xorif_get_fhi_alarms()\n");
    return fhi_alarm_status;
}

void xorif_clear_fhi_alarms(void)
{
    TRACE("xorif_clear_fhi_alarms()\n");
    fhi_alarm_status = 0;
}

int xorif_read_fhi_reg(const char *name, uint32_t *val)
{
    TRACE("xorif_read_fhi_reg(%s)\n", name);

    const reg_info_t *ptr = find_register(&fhi_reg_map[0], name);
    if (!ptr)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return REGISTER_NOT_FOUND;
    }
    else
    {
        *val = read_reg(fh_device.io, name, ptr->addr, ptr->mask, ptr->shift);
        return XORIF_SUCCESS;
    }
}

int xorif_read_fhi_reg_offset(const char *name, uint16_t offset, uint32_t *val)
{
    TRACE("xorif_read_fhi_reg_offset(%s, 0x%x)\n", name, offset);

    const reg_info_t *ptr = find_register(&fhi_reg_map[0], name);
    if (!ptr)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return REGISTER_NOT_FOUND;
    }
    else
    {
        *val = read_reg_offset(fh_device.io, name, ptr->addr, offset, ptr->mask, ptr->shift);
        return XORIF_SUCCESS;
    }
}

int xorif_write_fhi_reg(const char *name, uint32_t value)
{
    TRACE("xorif_write_fhi_reg(%s, 0x%x)\n", name, value);

    const reg_info_t *ptr = find_register(&fhi_reg_map[0], name);
    if (!ptr)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return REGISTER_NOT_FOUND;
    }
    else
    {
        write_reg(fh_device.io, name, ptr->addr, ptr->mask, ptr->shift, value);
        return XORIF_SUCCESS;
    }
}

int xorif_write_fhi_reg_offset(const char *name, uint16_t offset, uint32_t value)
{
    TRACE("xorif_write_fhi_reg(%s, 0x%x, 0x%x)\n", name, offset, value);

    const reg_info_t *ptr = find_register(&fhi_reg_map[0], name);
    if (!ptr)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return REGISTER_NOT_FOUND;
    }
    else
    {
        write_reg_offset(fh_device.io, name, ptr->addr, offset, ptr->mask, ptr->shift, value);
        return XORIF_SUCCESS;
    }
}

int xorif_get_fhi_stats(struct xorif_fhi_stats *ptr)
{
    TRACE("xorif_get_fhi_stats(...)\n");

    if (ptr)
    {
        ptr->total_rx_good_pkt_cnt = READ_REG(STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT);
        ptr->total_rx_bad_pkt_cnt = READ_REG(STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT);
        ptr->total_rx_bad_fcs_cnt = READ_REG(STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT);
        ptr->user_data_rx_packets_cnt = READ_REG(STATS_ETH_STATS_USER_DATA_RX_PACKETS_CNT);
        ptr->user_data_rx_good_pkt_cnt = READ_REG(STATS_ETH_STATS_USER_DATA_RX_GOOD_PKT_CNT);
        ptr->user_data_rx_bad_pkt_cnt = READ_REG(STATS_ETH_STATS_USER_DATA_RX_BAD_PKT_CNT);
        ptr->user_data_rx_bad_fcs_cnt = READ_REG(STATS_ETH_STATS_USER_DATA_RX_BAD_FCS_CNT);
        ptr->user_ctrl_rx_packets_cnt = READ_REG(STATS_ETH_STATS_USER_CTRL_RX_PACKETS_CNT);
        ptr->user_ctrl_rx_good_pkt_cnt = READ_REG(STATS_ETH_STATS_USER_CTRL_RX_GOOD_PKT_CNT);
        ptr->user_ctrl_rx_bad_pkt_cnt = READ_REG(STATS_ETH_STATS_USER_CTRL_RX_BAD_PKT_CNT);
        ptr->user_ctrl_rx_bad_fcs_cnt = READ_REG(STATS_ETH_STATS_USER_CTRL_RX_BAD_FCS_CNT);
        ptr->user_data_rx_pkts_rate = READ_REG(STATS_ETH_STATS_USER_DATA_RX_PKTS_RATE);
        ptr->user_ctrl_rx_pkts_rate = READ_REG(STATS_ETH_STATS_USER_CTRL_RX_PKTS_RATE);
        return XORIF_SUCCESS;
    }
    return XORIF_FAILURE;
}

void xorif_fhi_get_cc_alloc(uint16_t cc, struct xorif_cc_alloc *ptr)
{
    TRACE("xorif_fhi_get_cc_alloc(%d, ...)\n", cc);

    if (ptr)
    {
        // Get enabled status
        ptr->enabled = (READ_REG(ORAN_CC_ENABLE) & (1 << cc)) != 0;

        // Get numerology
        ptr->numerology = READ_REG_OFFSET(ORAN_CC_NUMEROLOGY, cc * 0x70);

        // Get the number of RBs
        ptr->num_rbs = READ_REG_OFFSET(ORAN_CC_NUMRBS, cc * 0x70);

        // Uplink control
        ptr->ul_ctrl_sym_num = READ_REG_OFFSET(ORAN_CC_UL_CTRL_SYM_NUM_INDEX, cc * 0x70);
        ptr->ul_ctrl_offset = READ_REG_OFFSET(ORAN_CC_UL_CTRL_OFFSETS, cc * 0x70);
        ptr->ul_ctrl_unroll_offset = READ_REG_OFFSET(ORAN_CC_UL_CTRL_UNROLLED_OFFSETS, cc * 0x70);
        ptr->ul_ctrl_base_offset = READ_REG_OFFSET(ORAN_CC_UL_BASE_OFFSET, cc * 0x70);

        // Downlink control
        ptr->dl_ctrl_sym_num = READ_REG_OFFSET(ORAN_CC_DL_CTRL_SYM_NUM_INDEX, cc * 0x70);
        ptr->dl_ctrl_offset = READ_REG_OFFSET(ORAN_CC_DL_CTRL_OFFSETS, cc * 0x70);
        ptr->dl_ctrl_unroll_offset = READ_REG_OFFSET(ORAN_CC_DL_CTRL_UNROLLED_OFFSETS, cc * 0x70);

        // Downlink data
        ptr->dl_data_sym_num = READ_REG_OFFSET(ORAN_CC_DL_DATA_SYM_NUM_INDEX, cc * 0x70);
        ptr->dl_data_sym_start = READ_REG_OFFSET(ORAN_CC_DL_DATA_SYM_START_INDEX, cc * 0x70);

        // Downlink data buffer size
        uint16_t comp_mode = READ_REG_OFFSET(ORAN_CC_DL_UD_COMP_METH, cc * 0x70);
        uint16_t comp_width = READ_REG_OFFSET(ORAN_CC_DL_UD_IQ_WIDTH, cc * 0x70);
        ptr->dl_data_buff_size = calc_dl_data_buff_size(ptr->num_rbs, comp_mode, comp_width);

        // Downlink data buffer start
        ptr->dl_data_buff_start = READ_REG_OFFSET(ORAN_CC_DL_DATA_UNROLL_OFFSET, ptr->dl_data_sym_start * 0x4);
    }
}

const struct xorif_fhi_caps *xorif_get_fhi_capabilities(void)
{
    TRACE("xorif_get_fhi_capabilities()\n");
    return &fhi_caps;
}

int xorif_set_fhi_dest_mac_addr(int port, const uint8_t address[])
{
    TRACE("xorif_set_fhi_dest_mac_addr(%d, ...)\n", port);
    if (port >= xorif_fhi_get_num_eth_ports())
    {
        TRACE("Invalid ethernet port value\n");
        return XORIF_INVALID_ETH_PORT;
    }

    uint32_t addr_hi = address[0] << 8 | address[1];
    uint32_t addr_lo = address[2] << 24 | address[3] << 16 | address[4] << 8 | address[5];
    WRITE_REG_OFFSET(ETH_DEST_ADDR_47_32, port * 0x100, addr_hi);
    WRITE_REG_OFFSET(ETH_DEST_ADDR_31_0, port * 0x100, addr_lo);
    return XORIF_SUCCESS;
}

int xorif_set_fhi_src_mac_addr(int port, const uint8_t address[])
{
    TRACE("xorif_set_fhi_src_mac_addr(%d, ...)\n", port);
    if (port >= xorif_fhi_get_num_eth_ports())
    {
        TRACE("Invalid ethernet port value\n");
        return XORIF_INVALID_ETH_PORT;
    }

    uint32_t addr_hi = address[0] << 8 | address[1];
    uint32_t addr_lo = address[2] << 24 | address[3] << 16 | address[4] << 8 | address[5];
    WRITE_REG_OFFSET(ETH_SRC_ADDR_47_32, port * 0x100, addr_hi);
    WRITE_REG_OFFSET(ETH_SRC_ADDR_31_0, port * 0x100, addr_lo);
    return XORIF_SUCCESS;
}

// Non-API functions...

static int fhi_irq_handler(int id, void *data)
{
    struct xorif_device_info *device = (struct xorif_device_info *)data;

    if (device)
    {
        // Check interrupt status
        uint32_t val = metal_io_read32(fh_device.io, FHI_INTR_STATUS_ADDR) & FHI_INTR_MASK;
        TRACE("fhi_irq_handler mask = 0x%x\n", val);

        if (val)
        {
            // Record the alarm status
            fhi_alarm_status |= val;

            if (val & CFG_DEFM_INT_INFIFO_OF_MASK)
            {
                TRACE("FHI IRQ: CFG_DEFM_INT_INFIFO_OF\n");
            }
            if (val & CFG_DEFM_INT_INFIFO_UF_MASK)
            {
                TRACE("FHI IRQ: CFG_DEFM_INT_INFIFO_UF\n");
            }
            if (val & CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_MASK)
            {
                TRACE("FHI IRQ: CFG_DEFM_INT_ETH_PIPE_C_BUF_OF\n");
            }
            if (val & CFG_DEFM_INT_ETH_PIPE_TABLE_OF_MASK)
            {
                TRACE("FHI IRQ: CFG_DEFM_INT_ETH_PIPE_TABLE_OF\n");
            }
            if (val & CFG_FRAM_INT_OUTFIFO_OF_MASK)
            {
                TRACE("FHI IRQ: CFG_FRAM_INT_OUTFIFO_OF\n");
            }
            if (val & CFG_FRAM_INT_OUTFIFO_UF_MASK)
            {
                TRACE("FHI IRQ: CFG_FRAM_INT_OUTFIFO_UF\n");
            }
            if (val & CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_MASK)
            {
                TRACE("FHI IRQ: CFG_FRAM_INT_PRACH_SECTION_OVERFLOW\n");
            }
            if (val & CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_MASK)
            {
                TRACE("FHI IRQ: CFG_FRAM_INT_PRACH_SECTION_NOTFOUND\n");
            }
            if (val & CFG_AXI_TIMEOUT_STATUS_MASK)
            {
                TRACE("FHI IRQ: CFG_AXI_TIMEOUT_STATUS\n");
            }

            // Clear interrupts by writing to the "master interrupt"
            // TODO write just 1, or 0 then 1? This doesn't seem to work currently!!!
            WRITE_REG(CFG_MASTER_INT_ENABLE, 0);
            //WRITE_REG(CFG_MASTER_INT_ENABLE, 1);

            // TODO handle call-backs if required

            return METAL_IRQ_HANDLED;
        }
    }

    return METAL_IRQ_NOT_HANDLED;
}

void xorif_fhi_init_device(void)
{
    // Clear alarm status
    fhi_alarm_status = 0;

    // Set-up the FHI capabilities
    memset(&fhi_caps, 0, sizeof(fhi_caps));
    fhi_caps.no_framer_antenna = READ_REG(CFG_CONFIG_NO_OF_FRAM_ANTS);
    fhi_caps.no_deframer_antennas = READ_REG(CFG_CONFIG_NO_OF_DEFM_ANTS);
    fhi_caps.num_eth_ports = READ_REG(CFG_CONFIG_NO_OF_ETH_PORTS);
    fhi_caps.max_cc = READ_REG(CFG_CONFIG_XRAN_MAX_CC);
    fhi_caps.max_dl_symbols = READ_REG(CFG_CONFIG_XRAN_MAX_DL_SYMBOLS);
    fhi_caps.max_framer_ethernet_pkt = READ_REG(CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX);
    fhi_caps.max_deframer_ethernet_pkt = READ_REG(CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX);
    fhi_caps.max_subcarriers = READ_REG(CFG_CONFIG_XRAN_MAX_SCS);
    fhi_caps.max_ctrl_symbols = READ_REG(CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS);
    fhi_caps.max_ul_ctrl_1kwords = READ_REG(CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS);
    fhi_caps.max_dl_ctrl_1kwords = READ_REG(CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS);
    fhi_caps.max_dl_data_1kwords = READ_REG(CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS);
    fhi_caps.timer_clk_ps = READ_REG(CFG_CONFIG_XRAN_TIMER_CLK_PS);
    fhi_caps.num_unsolicited_ports = READ_REG(CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM);
    fhi_caps.num_prach_ports = READ_REG(CFG_CONFIG_XRAN_PRACH_C_PORTS);

    // Set up any useful defaults, etc.
    XRAN_TIMER_CLK = READ_REG(CFG_CONFIG_XRAN_TIMER_CLK_PS);

    // Register and enable ISR
    int num = fh_device.dev->irq_num;
    if (num > 0)
    {
        int irq = (intptr_t)fh_device.dev->irq_info;
        if (irq != -1)
        {
            metal_irq_register(irq, fhi_irq_handler, &fh_device);
            metal_irq_enable(irq);
            TRACE("FHI IRQ registered (%d)\n", irq);

            // Setup interrupts (doing it in one go rather than separate WRITE_REG's)
            uint32_t val = metal_io_read32(fh_device.io, FHI_INTR_ENABLE_ADDR);
            val |= FHI_INTR_MASK;
            metal_io_write32(fh_device.io, FHI_INTR_ENABLE_ADDR, val);

            // Finally enable the master interrupt
            //WRITE_REG(CFG_MASTER_INT_ENABLE, 1); // HACK for now, since seems broken
        }
    }
}

int xorif_fhi_get_num_ul_spatial_streams(void)
{
    // Note, number of U/L spatial streams = number of "framer antennas"
    return fhi_caps.no_framer_antenna; //READ_REG(CFG_CONFIG_NO_OF_FRAM_ANTS);
}

int xorif_fhi_get_num_dl_spatial_streams(void)
{
    // Note, number of D/L spatial streams = number of "de-framer antennas"
    return fhi_caps.no_deframer_antennas; //READ_REG(CFG_CONFIG_NO_OF_DEFM_ANTS);
}

int xorif_fhi_get_max_cc(void)
{
    return fhi_caps.max_cc; //READ_REG(CFG_CONFIG_XRAN_MAX_CC);
}

int xorif_fhi_get_num_eth_ports(void)
{
    return fhi_caps.num_eth_ports; //READ_REG(CFG_CONFIG_NO_OF_ETH_PORTS);
}

int xorif_fhi_get_min_numerology(void)
{
    return 0; //READ_REG(CFG_CONFIG_XRAN_MIN_NUMEROLOGY);
}

int xorif_fhi_get_max_numerology(void)
{
    return 4; //READ_REG(CFG_CONFIG_XRAN_MAX_NUMEROLOGY);
}

int xorif_fhi_get_max_dl_symbols(void)
{
    return fhi_caps.max_dl_symbols; //READ_REG(CFG_CONFIG_XRAN_MAX_DL_SYMBOLS);
}

int xorif_fhi_get_num_sect_per_sym_ul(void)
{
    return -1; //READ_REG(CFG_CONFIG_XRAN_MAX_UL_SECT_PER_SYMBOL);
}

int xorif_fhi_get_num_sect_per_sym_dl(void)
{
    return -1; //READ_REG(CFG_CONFIG_XRAN_MAX_DL_SECT_PER_SYMBOL);
}

int xorif_fhi_cc_reload(uint16_t cc)
{
    WRITE_REG(ORAN_CC_RELOAD, 1 << cc);
    return XORIF_SUCCESS;
}

int xorif_fhi_cc_enable(uint16_t cc)
{
    uint32_t val = READ_REG(ORAN_CC_ENABLE);
    val |= (1 << cc);
    WRITE_REG(ORAN_CC_ENABLE, val);
    return XORIF_SUCCESS;
}

int xorif_fhi_cc_disable(uint16_t cc)
{
    uint32_t val = READ_REG(ORAN_CC_ENABLE);
    val &= ~(1 << cc);
    WRITE_REG(ORAN_CC_ENABLE, val);
    return XORIF_SUCCESS;
}

uint16_t xorif_fhi_get_enabled_mask(void)
{
    return READ_REG(ORAN_CC_ENABLE);
}

void xorif_fhi_set_enabled_mask(uint16_t mask)
{
    WRITE_REG(ORAN_CC_ENABLE, mask);
}

int xorif_fhi_init_cc_symbol_pointers(
    uint16_t cc,
    uint16_t dl_data_sym_num,
    uint16_t dl_data_sym_start_index,
    uint16_t dl_ctrl_sym_num,
    uint16_t ul_ctrl_sym_num)
{
    WRITE_REG_OFFSET(ORAN_CC_DL_DATA_SYM_NUM_INDEX, cc * 0x70, dl_data_sym_num);
    WRITE_REG_OFFSET(ORAN_CC_DL_DATA_SYM_START_INDEX, cc * 0x70, dl_data_sym_start_index);
    WRITE_REG_OFFSET(ORAN_CC_DL_CTRL_SYM_NUM_INDEX, cc * 0x70, dl_ctrl_sym_num);
    WRITE_REG_OFFSET(ORAN_CC_UL_CTRL_SYM_NUM_INDEX, cc * 0x70, ul_ctrl_sym_num);
    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_dl_data_offsets(uint16_t cc,
                                      uint16_t dl_data_sym_num,
                                      uint16_t dl_data_sym_start_index,
                                      uint16_t symbol_buffer_size,
                                      uint16_t dl_data_buff_start)
{
    for (int i = 0; i < dl_data_sym_num; ++i)
    {
        WRITE_REG_OFFSET(ORAN_CC_DL_DATA_UNROLL_OFFSET, dl_data_sym_start_index * 0x4, dl_data_buff_start);
        dl_data_sym_start_index += 1;
        dl_data_buff_start += symbol_buffer_size;
    }

    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_rbs(uint16_t cc,
                          uint16_t numerology,
                          uint16_t num_rbs,
                          uint16_t sym_per_slot)
{
    WRITE_REG_OFFSET(ORAN_CC_NUMEROLOGY, cc * 0x70, numerology);
    WRITE_REG_OFFSET(ORAN_CC_NUMRBS, cc * 0x70, num_rbs);
    WRITE_REG_OFFSET(ORAN_CC_SYMPERSLOT, cc * 0x70, sym_per_slot);
    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_ul_section_mem(uint16_t cc,
                                     uint16_t ul_ctrl_offset,
                                     uint16_t ul_ctrl_unrolled_offset,
                                     uint16_t ul_ctrl_base_offset)
{
    WRITE_REG_OFFSET(ORAN_CC_UL_CTRL_OFFSETS, cc * 0x70, ul_ctrl_offset);
    WRITE_REG_OFFSET(ORAN_CC_UL_CTRL_UNROLLED_OFFSETS, cc * 0x70, ul_ctrl_unrolled_offset);
    WRITE_REG_OFFSET(ORAN_CC_UL_BASE_OFFSET, cc * 0x70, ul_ctrl_base_offset);
    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_dl_section_mem(uint16_t cc,
                                     uint16_t dl_ctrl_offset,
                                     uint16_t dl_ctrl_unrolled_offset)
{
    WRITE_REG_OFFSET(ORAN_CC_DL_CTRL_OFFSETS, cc * 0x70, dl_ctrl_offset);
    WRITE_REG_OFFSET(ORAN_CC_DL_CTRL_UNROLLED_OFFSETS, cc * 0x70, dl_ctrl_unrolled_offset);
    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_ctrl_constants(uint16_t cc,
                                     uint16_t numerology,
                                     uint16_t num_ctrl_per_sym_dl,
                                     uint16_t num_ctrl_per_sym_ul,
                                     uint16_t dl_ctrl_sym_num,
                                     uint16_t ul_ctrl_sym_num,
                                     uint16_t sym_per_slot)
{
    WRITE_REG_OFFSET(ORAN_CC_MAX_SYMBOLS, cc * 0x70, 10 * (1 << numerology) * (sym_per_slot ? 12 : 14));
    WRITE_REG_OFFSET(ORAN_CC_MODVALS_DL, cc * 0x70, dl_ctrl_sym_num * num_ctrl_per_sym_dl);
    WRITE_REG_OFFSET(ORAN_CC_MODVALS_UL, cc * 0x70, ul_ctrl_sym_num * num_ctrl_per_sym_ul);
    return XORIF_SUCCESS;
}

int xorif_fhi_set_cc_dl_iq_compression(uint16_t cc,
                                       uint16_t bit_width,
                                       enum xorif_iq_comp comp_meth)
{
    WRITE_REG_OFFSET(ORAN_CC_DL_UD_IQ_WIDTH, cc * 0x70, bit_width);
    WRITE_REG_OFFSET(ORAN_CC_DL_UD_COMP_METH, cc * 0x70, comp_meth);
    return XORIF_SUCCESS;
}

int xorif_fhi_set_cc_ul_iq_compression(uint16_t cc,
                                       uint16_t bit_width,
                                       enum xorif_iq_comp comp_meth)
{
    WRITE_REG_OFFSET(ORAN_CC_UL_UD_IQ_WIDTH, cc * 0x70, bit_width);
    WRITE_REG_OFFSET(ORAN_CC_UL_UD_COMP_METH, cc * 0x70, comp_meth);
    return XORIF_SUCCESS;
}

int xorif_fhi_configure_time_advance_offsets(uint16_t cc,
                                             uint16_t numerology,
                                             uint16_t sym_per_slot,
                                             uint32_t advance_ul,
                                             uint32_t advance_dl)
{
    // Compute number of symbols per second based on numerology
    // Note, 10 sub-frames per frame, 100 frames per second
    int num = 100 * 10 * slots_per_subframe[numerology] * (sym_per_slot ? 12 : 14);

    // Symbol period in picoseconds = 1e12 / num
    double sym_period = 1e12 / num;

    // Adjust symbol period to be in clock cycles
    // Note, rounding-down / truncating here
    uint32_t sym_period_clk = sym_period / XRAN_TIMER_CLK;

    // Convert from ps to us
    uint32_t T2A_MIN_CP_UL = advance_ul * 1e6;
    uint32_t TCP_ADV_DL = advance_dl * 1e6;

    // Intermediate values used in the calculation
    uint32_t DL_CTRL_RXWIN_ADV_CP = (TCP_ADV_DL + FH_DECAP_DLY) / XRAN_TIMER_CLK;
    uint32_t UL_CTRL_RXWIN_ADV_CP = (T2A_MIN_CP_UL + UL_RADIO_CH_DLY) / XRAN_TIMER_CLK;

    // Downlink settings
    WRITE_REG_OFFSET(ORAN_CC_DL_SETUP_D_CYCLES, cc * 0x70, sym_period_clk - (FH_DECAP_DLY / XRAN_TIMER_CLK));
    WRITE_REG_OFFSET(ORAN_CC_DL_SETUP_C_CYCLES, cc * 0x70, sym_period_clk - (DL_CTRL_RXWIN_ADV_CP % sym_period_clk));
    WRITE_REG_OFFSET(ORAN_CC_DL_SETUP_C_ABS_SYMBOL, cc * 0x70, (DL_CTRL_RXWIN_ADV_CP / sym_period_clk) + 1);

    // Uplink settings
    WRITE_REG_OFFSET(ORAN_CC_UL_SETUP_D_CYCLES, cc * 0x70, sym_period_clk);
    WRITE_REG_OFFSET(ORAN_CC_UL_SETUP_C_CYCLES, cc * 0x70, sym_period_clk - (UL_CTRL_RXWIN_ADV_CP % sym_period_clk));
    WRITE_REG_OFFSET(ORAN_CC_UL_SETUP_C_ABS_SYMBOL, cc * 0x70, (UL_CTRL_RXWIN_ADV_CP / sym_period_clk) + 2);

    return XORIF_SUCCESS;
}

int xorif_fhi_configure_cc(uint16_t cc)
{
    // Set up pointer to configuration data
    const struct xorif_cc_config *ptr = &cc_config[cc];

    // Using temporary structure to store data before finalizing the allocation
    struct xorif_cc_alloc temp;

    // Calculate required number of symbols
    temp.ul_ctrl_sym_num = calc_sym_num(ptr->numerology, ptr->extended_cp, ptr->deskew + ptr->advance_ul);
    temp.dl_ctrl_sym_num = calc_sym_num(ptr->numerology, ptr->extended_cp, ptr->deskew + ptr->advance_dl);
    temp.dl_data_sym_num = calc_sym_num(ptr->numerology, ptr->extended_cp, ptr->deskew) + 1;

    // Check number ctrl symbols
    uint16_t max_ctrl_sym = READ_REG(CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS);
    if ((temp.ul_ctrl_sym_num > max_ctrl_sym) || (temp.dl_ctrl_sym_num > max_ctrl_sym))
    {
        TRACE("Configuration exceeds max control symbols\n");
        return XORIF_MAX_CTRL_SYM_EXCEEDED;
    }

    // Check number data symbols
    uint16_t max_data_sym = READ_REG(CFG_CONFIG_XRAN_MAX_DL_SYMBOLS);
    if (temp.dl_data_sym_num > max_data_sym)
    {
        TRACE("Configuration exceeds max many data symbols\n");
        return XORIF_MAX_DATA_SYM_EXCEEDED;
    }

    // Set number of RBs
    temp.num_rbs = ptr->num_rbs;

    // Calculate downlink data buffer size
    temp.dl_data_buff_size = calc_dl_data_buff_size(temp.num_rbs, ptr->iq_comp_meth_dl, ptr->iq_comp_width_dl);

    // Get the "left" active component carrier
    int left_cc = get_left_cc(cc);
    if (left_cc == -1)
    {
        // Nothing active to the left, so set start values to 0
        temp.ul_ctrl_offset = 0;
        temp.ul_ctrl_unroll_offset = 0;
        temp.ul_ctrl_base_offset = 0;
        temp.dl_ctrl_offset = 0;
        temp.dl_ctrl_unroll_offset = 0;
        temp.dl_data_sym_start = 0;
        temp.dl_data_buff_start = 0;
    }
    else
    {
        // Use the component carrier to the left to set start values
        struct xorif_cc_alloc left;
        xorif_fhi_get_cc_alloc(left_cc, &left);

        temp.ul_ctrl_offset = left.ul_ctrl_offset + (left.num_rbs * left.ul_ctrl_sym_num);
        temp.ul_ctrl_unroll_offset = left.ul_ctrl_unroll_offset + (left.num_rbs);
        temp.ul_ctrl_base_offset = left.ul_ctrl_base_offset + (left.num_rbs);
        temp.dl_ctrl_offset = left.dl_ctrl_offset + (left.num_rbs * left.dl_ctrl_sym_num);
        temp.dl_ctrl_unroll_offset = left.dl_ctrl_unroll_offset + (left.num_rbs);
        temp.dl_data_sym_start = left.dl_data_sym_start + (left.dl_data_sym_num);
        temp.dl_data_buff_start = left.dl_data_buff_start + (left.dl_data_sym_num * left.dl_data_buff_size);
    }

    // Need to check the allocation fits within the "limit" values, which is based on what
    // is to the "right" of the component carrier (or max values if nothing to right)
    uint16_t ul_ctrl_offset_lim;
    uint16_t ul_ctrl_unroll_offset_lim;
    uint16_t ul_ctrl_base_offset_lim;
    uint16_t dl_ctrl_offset_lim;
    uint16_t dl_ctrl_unroll_offset_lim;
    uint16_t dl_data_sym_start_lim;
    uint16_t dl_data_buff_start_lim;

    // Get the "right" active component carrier
    int right_cc = get_right_cc(cc);
    if (right_cc == MAX_NUM_CC)
    {
        // Nothing active to the right, so set limit values to max
        uint16_t max_rbs = READ_REG(CFG_CONFIG_XRAN_MAX_SCS) / 12;
        ul_ctrl_offset_lim = 1024 * READ_REG(CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS);
        ul_ctrl_unroll_offset_lim = max_rbs;
        ul_ctrl_base_offset_lim = max_rbs;
        dl_ctrl_offset_lim = 1024 * READ_REG(CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS);
        dl_ctrl_unroll_offset_lim = max_rbs;
        dl_data_sym_start_lim = READ_REG(CFG_CONFIG_XRAN_MAX_DL_SYMBOLS);
        dl_data_buff_start_lim = 1024 * READ_REG(CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS);
    }
    else
    {
        // Use the component carrier to the right to set limit values
        struct xorif_cc_alloc right;
        xorif_fhi_get_cc_alloc(right_cc, &right);

        ul_ctrl_offset_lim = right.ul_ctrl_offset;
        ul_ctrl_unroll_offset_lim = right.ul_ctrl_unroll_offset;
        ul_ctrl_base_offset_lim = right.ul_ctrl_base_offset;
        dl_ctrl_offset_lim = right.dl_ctrl_offset;
        dl_ctrl_unroll_offset_lim = right.dl_ctrl_unroll_offset;
        dl_data_sym_start_lim = right.dl_data_sym_start;
        dl_data_buff_start_lim = right.dl_data_buff_start;
    }

    // Check fit
    if (((temp.ul_ctrl_offset + (temp.num_rbs * temp.ul_ctrl_sym_num)) > ul_ctrl_offset_lim) ||
        ((temp.dl_ctrl_offset + (temp.num_rbs * temp.dl_ctrl_sym_num)) > dl_ctrl_offset_lim))
    {
        TRACE("Configuration exceeds available buffer space (1)\n");
        return XORIF_AVAIL_BUFFER_SPACE_EXCEEDED;
    }

    if (((temp.ul_ctrl_unroll_offset + (temp.num_rbs)) > ul_ctrl_unroll_offset_lim) ||
        ((temp.dl_ctrl_unroll_offset + (temp.num_rbs)) > dl_ctrl_unroll_offset_lim))
    {
        TRACE("Configuration exceeds available subcarriers (2)\n");
        return XORIF_AVAIL_BUFFER_SPACE_EXCEEDED;
    }

    if ((temp.ul_ctrl_base_offset + (temp.num_rbs)) > ul_ctrl_base_offset_lim)
    {
        TRACE("Configuration exceeds available subcarriers (3)\n");
        return XORIF_AVAIL_BUFFER_SPACE_EXCEEDED;
    }

    if ((temp.dl_data_sym_start + (temp.dl_data_sym_num)) > dl_data_sym_start_lim)
    {
        TRACE("Configuration exceeds allocated buffer space (4)\n");
        return XORIF_AVAIL_BUFFER_SPACE_EXCEEDED;
    }

    if ((temp.dl_data_buff_start + (temp.dl_data_sym_num * temp.dl_data_buff_size)) > dl_data_buff_start_lim)
    {
        TRACE("Configuration exceeds allocated buffer space (5)\n");
        return XORIF_AVAIL_BUFFER_SPACE_EXCEEDED;
    }

    // Everything fits!
    TRACE("Configuration valid\n");

    // Program the h/w
    xorif_fhi_init_cc_rbs(cc, ptr->numerology, ptr->num_rbs, ptr->extended_cp);
    xorif_fhi_init_cc_symbol_pointers(cc, temp.dl_data_sym_num, temp.dl_data_sym_start,
                                      temp.dl_ctrl_sym_num, temp.ul_ctrl_sym_num);
    xorif_fhi_set_cc_dl_iq_compression(cc, ptr->iq_comp_width_dl, ptr->iq_comp_meth_dl);
    xorif_fhi_set_cc_ul_iq_compression(cc, ptr->iq_comp_width_ul, ptr->iq_comp_meth_ul);
    xorif_fhi_init_cc_dl_section_mem(cc, temp.dl_ctrl_offset, temp.dl_ctrl_unroll_offset);
    xorif_fhi_init_cc_ul_section_mem(cc, temp.ul_ctrl_offset, temp.ul_ctrl_unroll_offset,
                                     temp.ul_ctrl_base_offset);
    xorif_fhi_init_cc_dl_data_offsets(cc, temp.dl_data_sym_num, temp.dl_data_sym_start,
                                      temp.dl_data_buff_size, temp.dl_data_buff_start);
    xorif_fhi_init_cc_ctrl_constants(cc, ptr->numerology, ptr->num_ctrl_per_sym_dl,
                                     ptr->num_ctrl_per_sym_ul, temp.dl_data_sym_num,
                                     temp.ul_ctrl_sym_num, ptr->extended_cp);
    xorif_fhi_configure_time_advance_offsets(cc, ptr->numerology, ptr->extended_cp,
                                             ptr->advance_ul, ptr->advance_dl);

    // Perform "reload" on the component carrier
    xorif_fhi_cc_reload(cc);

    // Enable component carrier
    xorif_fhi_cc_enable(cc);

    return XORIF_SUCCESS;
}

/**
 * @brief Scans component carriers for the 1st "active" one to the "left" of the
 * specified component carrier.
 * @param [in] cc Component carrier
 * @returns
 *      - Component carrier index (or -1 if none)
 */
static int get_left_cc(uint16_t cc)
{
    // Scan left for 1st instance that's active (i.e. enabled bit = 1)
    // Note, by "left" here we mean, for example: CC[0], CC[1], CC[2]...
    // then "CC[1]" is left of "CC[2]", etc.
    // Returns -1 if nothing to the left was found

    // Using the component carrier enabled bit-map
    uint16_t mask = xorif_fhi_get_enabled_mask();

    for (int i = cc - 1; i >= 0; --i)
    {
        if (mask & (1 << i))
        {
            return i;
        }
    }

    return -1;
}

/**
 * @brief Scans component carriers for the 1st "active" one to the "right" of the
 * specified component carrier.
 * @param [in] cc Component carrier
 * @returns
 *      - Component carrier index (or MAX_NUM_CC if none)
 */
static int get_right_cc(uint16_t cc)
{
    // Scan right for 1st instance that's active (i.e. enabled bit = 1)
    // Note, by "right" here we mean, for example: CC[0], CC[1], CC[2]...
    // then "CC[1]" is right of "CC[0]", etc.
    // Returns MAX_NUM_CC if nothing to the left was found

    // Using the component carrier enabled bit-map
    uint16_t mask = xorif_fhi_get_enabled_mask();

    for (int i = cc + 1; i < MAX_NUM_CC; ++i)
    {
        if (mask & (1 << i))
        {
            return i;
        }
    }

    return MAX_NUM_CC;
}

/**
 * @brief Utility function to compute the number of symbols, based on numerology, deskew
 * and time advance.
 * @param[in] numerology Numerology
 * @param[in] extended_cp Extended CP supported (0 = no, 1 = yes) (for numerology 2)
 * @param[in] time Required time in microseconds (e.g. deskew + control advance)
 * @returns
 *      - Number of symbols required
 */
static uint16_t calc_sym_num(uint16_t numerology, uint16_t extended_cp, uint32_t time)
{
    // Compute number of symbols per second based on numerology
    // Note, 10 sub-frames per frame, 100 frames per second
    int num = 100 * 10 * slots_per_subframe[numerology] * (extended_cp ? 12 : 14);

    // Symbol period in microseconds = 1e6 * 1 / num
    double sym_period = 1e6 * 1 / num;

    // Number of symbols required is ceil(time / sym_period) converted to integer
    return (uint16_t)(ceil(time / sym_period));
}

/**
 * @brief Calculate dowblink data buffer size based on number of RBs and compression scheme.
 * @param[in] num_rbs Number of RBs
 * @param[in] comp_mode IQ compression mode
 * @param[in] comp_width compression width
 * @returns
 *      - Downlink data buffer size (in bytes)
 */
static uint16_t calc_dl_data_buff_size(uint16_t num_rbs, enum xorif_iq_comp comp_mode, uint16_t comp_width)
{
    // Calculate size of RB in bytes
    uint16_t size;
    switch (comp_mode)
    {
    case IQ_COMP_BLOCK_FP:
        // (n bits I + n bits Q) per RE + 1 byte for exponent
        size = (comp_width * 2 * RE_PER_RB + 7) / 8 + 1;
        break;

    case IQ_COMP_NONE:
    default:
        // (16 bits I + 16 bits Q) per RE
        size = (16 * 2 * RE_PER_RB + 7) / 8;
        break;
    }

    // Add 4 byte section header
    size += 4;

    // Multiply by number of RBs
    size *= num_rbs;

    // Divide by 8 (since 8 bytes per location)
    size = (size + 7) / 8;

    return size;
}

/** @} */