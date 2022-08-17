/*
 * Copyright 2020 - 2021 Xilinx, Inc.
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
 * @file xorif_registers.c
 * @author Steven Dickinson
 * @brief Source file for libxorif register access functions.
 * @addtogroup libxorif
 * @{
 */

#include "xorif_common.h"
#include "xorif_fh_func.h"
#include "xorif_utils.h"
#include "xorif_registers.h"

// The following const structure defines the register map for the Front Haul Interface
// Note, this array is sorted for more efficient access
static const reg_info_t fhi_reg_map[] =
{
    {"CFG_AXI_TIMEOUT_ENABLE", 0x14, 0x80000000, 31, 1},
    {"CFG_AXI_TIMEOUT_STATUS", 0x18, 0x80000000, 31, 1},
    {"CFG_CONFIG_LIMIT_BS_W", 0x10c, 0xf, 0, 4},
    {"CFG_CONFIG_LIMIT_CC_W", 0x108, 0xf, 0, 4},
    {"CFG_CONFIG_LIMIT_DU_W", 0x110, 0xf, 0, 4},
    {"CFG_CONFIG_LIMIT_RU_I_W", 0x100, 0xf, 0, 4},
    {"CFG_CONFIG_LIMIT_RU_O_W", 0x104, 0xf, 0, 4},
    {"CFG_CONFIG_MAP_STREAM_W", 0x114, 0xf00, 8, 4},
    {"CFG_CONFIG_MAP_TABLE_W", 0x114, 0xf, 0, 4},
    {"CFG_CONFIG_MAP_TYPE_W", 0x114, 0xf0000, 16, 4},
    {"CFG_CONFIG_NO_OF_DEFM_ANTS", 0x20, 0xffff0000, 16, 16},
    {"CFG_CONFIG_NO_OF_ETH_PORTS", 0x24, 0x3ff, 0, 10},
    {"CFG_CONFIG_NO_OF_FRAM_ANTS", 0x20, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_COMP_IN_CORE_BFP", 0x88, 0x20000, 17, 1},
    {"CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_SELRE", 0x88, 0x200000, 21, 1},
    {"CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_WIDTHS", 0x8c, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_COMP_IN_CORE_ENABLED", 0x28, 0x20, 5, 1},
    {"CFG_CONFIG_XRAN_COMP_IN_CORE_NOCOMP", 0x88, 0x10000, 16, 1},
    {"CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP", 0x80, 0x2, 1, 1},
    {"CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_SELRE", 0x80, 0x20, 5, 1},
    {"CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_WIDTHS", 0x84, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_DECOMP_IN_CORE_BSC", 0x80, 0x4, 2, 1},
    {"CFG_CONFIG_XRAN_DECOMP_IN_CORE_ENABLED", 0x28, 0x10, 4, 1},
    {"CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP", 0x80, 0x10, 4, 1},
    {"CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_SELRE", 0x80, 0x40, 6, 1},
    {"CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODC_WIDTHS", 0x84, 0x3f0000, 16, 6},
    {"CFG_CONFIG_XRAN_DECOMP_IN_CORE_MU", 0x80, 0x8, 3, 1},
    {"CFG_CONFIG_XRAN_DECOMP_IN_CORE_NOCOMP", 0x80, 0x1, 0, 1},
    {"CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX", 0x44, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS", 0x70, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH", 0x6c, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_FRAM_AUTO_START", 0x40, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX", 0x3c, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_MAX_CC", 0x2c, 0x1f, 0, 5},
    {"CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS", 0x4c, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS", 0x54, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS", 0x58, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_MAX_DL_SYMBOLS", 0x30, 0x1f, 0, 5},
    {"CFG_CONFIG_XRAN_MAX_NUMEROLOGY", 0x34, 0x7, 0, 3},
    {"CFG_CONFIG_XRAN_MAX_SCS", 0x48, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS", 0x50, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_MIN_NUMEROLOGY", 0x38, 0x7, 0, 3},
    {"CFG_CONFIG_XRAN_PRACH_C_PORTS", 0x74, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_PRECODING_EXT3_PORT", 0x28, 0x40, 6, 1},
    {"CFG_CONFIG_XRAN_SUPPORT_MODE", 0x28, 0xf, 0, 4},
    {"CFG_CONFIG_XRAN_TIMER_CLK_PS", 0x5c, 0xffff, 0, 16},
    {"CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM", 0x68, 0xffff, 0, 16},
    {"CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF", 0x14, 0x400, 10, 1},
    {"CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF", 0x14, 0x800, 11, 1},
    {"CFG_DEFM_INT_ENA_INFIFO_OF", 0x14, 0x100, 8, 1},
    {"CFG_DEFM_INT_ENA_INFIFO_UF", 0x14, 0x200, 9, 1},
    {"CFG_DEFM_INT_ETH_PIPE_C_BUF_OF", 0x18, 0x400, 10, 1},
    {"CFG_DEFM_INT_ETH_PIPE_TABLE_OF", 0x18, 0x800, 11, 1},
    {"CFG_DEFM_INT_INFIFO_OF", 0x18, 0x100, 8, 1},
    {"CFG_DEFM_INT_INFIFO_UF", 0x18, 0x200, 9, 1},
    {"CFG_DEFM_RESET_STATUS", 0x18, 0x2, 1, 1},
    {"CFG_FRAM_INT_ENA_OUTFIFO_OF", 0x14, 0x1000, 12, 1},
    {"CFG_FRAM_INT_ENA_OUTFIFO_UF", 0x14, 0x2000, 13, 1},
    {"CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND", 0x14, 0x8000, 15, 1},
    {"CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW", 0x14, 0x4000, 14, 1},
    {"CFG_FRAM_INT_OUTFIFO_OF", 0x18, 0x1000, 12, 1},
    {"CFG_FRAM_INT_OUTFIFO_UF", 0x18, 0x2000, 13, 1},
    {"CFG_FRAM_INT_PRACH_SECTION_NOTFOUND", 0x18, 0x8000, 15, 1},
    {"CFG_FRAM_INT_PRACH_SECTION_OVERFLOW", 0x18, 0x4000, 14, 1},
    {"CFG_FRAM_RESET_STATUS", 0x18, 0x1, 0, 1},
    {"CFG_INTERNAL_REVISION", 0x4, 0xffffffff, 0, 32},
    {"CFG_INTERRUPT_STATUS_SAMPLE", 0x1c, 0x1, 0, 1},
    {"CFG_MAJOR_REVISION", 0x0, 0xff000000, 24, 8},
    {"CFG_MASTER_INT_ENABLE", 0x10, 0x1, 0, 1},
    {"CFG_MINOR_REVISION", 0x0, 0xff0000, 16, 8},
    {"CFG_MONITOR_CLEAR", 0x810, 0x1, 0, 1},
    {"CFG_MONITOR_READ_31__0", 0x820, 0xffffffff, 0, 32},
    {"CFG_MONITOR_READ_63_32", 0x824, 0xffffffff, 0, 32},
    {"CFG_MONITOR_SAMPLE", 0x80c, 0x1, 0, 1},
    {"CFG_MONITOR_SELECT_CNT", 0x800, 0x3f, 0, 6},
    {"CFG_MONITOR_SELECT_READ", 0x804, 0x3f, 0, 6},
    {"CFG_MONITOR_SNAPSHOT", 0x808, 0x1, 0, 1},
    {"CFG_TIMEOUT_VALUE", 0x8, 0xfff, 0, 12},
    {"CFG_USER_RO_IN", 0xc, 0xff0000, 16, 8},
    {"CFG_USER_RW_OUT", 0xc, 0xff, 0, 8},
    {"CFG_VERSION_REVISION", 0x0, 0xff00, 8, 8},
    {"DEFM_CID_BS_MASK", 0x602c, 0x3f, 0, 6},
    {"DEFM_CID_BS_SHIFT", 0x6028, 0xf, 0, 4},
    {"DEFM_CID_CC_MASK", 0x6024, 0x3f, 0, 6},
    {"DEFM_CID_CC_SHIFT", 0x6020, 0xf, 0, 4},
    {"DEFM_CID_DU_MASK", 0x6034, 0x3f, 0, 6},
    {"DEFM_CID_DU_SHIFT", 0x6030, 0xf, 0, 4},
    {"DEFM_CID_LTE_MASK", 0x6054, 0xff, 0, 8},
    {"DEFM_CID_LTE_VALUE", 0x6058, 0xff, 0, 8},
    {"DEFM_CID_MAP_MODE", 0x6900, 0x3, 0, 2},
    {"DEFM_CID_MAP_RD_STREAM_PORTID", 0x6908, 0x7c0000, 18, 5},
    {"DEFM_CID_MAP_RD_STREAM_TYPE", 0x6908, 0x7000, 12, 3},
    {"DEFM_CID_MAP_RD_STROBE", 0x6908, 0x80000000, 31, 1},
    {"DEFM_CID_MAP_RD_TABLE_ADDR", 0x6908, 0x7ff, 0, 11},
    {"DEFM_CID_MAP_WR_STREAM_PORTID", 0x6904, 0x7c0000, 18, 5},
    {"DEFM_CID_MAP_WR_STREAM_TYPE", 0x6904, 0x7000, 12, 3},
    {"DEFM_CID_MAP_WR_STROBE", 0x6904, 0x80000000, 31, 1},
    {"DEFM_CID_MAP_WR_TABLE_ADDR", 0x6904, 0x7ff, 0, 11},
    {"DEFM_CID_PRACH_MASK", 0x6044, 0xff, 0, 8},
    {"DEFM_CID_PRACH_VALUE", 0x6048, 0xff, 0, 8},
    {"DEFM_CID_SSB_MASK", 0x604c, 0xff, 0, 8},
    {"DEFM_CID_SSB_VALUE", 0x6050, 0xff, 0, 8},
    {"DEFM_CID_SS_MASK", 0x6038, 0xff, 0, 8},
    {"DEFM_CID_U_MASK", 0x603c, 0xff, 0, 8},
    {"DEFM_CID_U_VALUE", 0x6040, 0xff, 0, 8},
    {"DEFM_CTRL_SS_RESET_E0_E", 0x6010, 0x10, 4, 1},
    {"DEFM_CTRL_SS_RESET_E0_RCOR", 0x6010, 0x40, 6, 1},
    {"DEFM_CTRL_SS_RESET_E0_RWIN", 0x6010, 0x20, 5, 1},
    {"DEFM_CTRL_SS_RESET_E0_T", 0x6010, 0x80, 7, 1},
    {"DEFM_CTRL_SS_RESET_E1_E", 0x6010, 0x100, 8, 1},
    {"DEFM_CTRL_SS_RESET_E1_RCOR", 0x6010, 0x400, 10, 1},
    {"DEFM_CTRL_SS_RESET_E1_RWIN", 0x6010, 0x200, 9, 1},
    {"DEFM_CTRL_SS_RESET_E1_T", 0x6010, 0x800, 11, 1},
    {"DEFM_CTRL_SS_RESET_E2_E", 0x6010, 0x1000, 12, 1},
    {"DEFM_CTRL_SS_RESET_E2_RCOR", 0x6010, 0x4000, 14, 1},
    {"DEFM_CTRL_SS_RESET_E2_RWIN", 0x6010, 0x2000, 13, 1},
    {"DEFM_CTRL_SS_RESET_E2_T", 0x6010, 0x8000, 15, 1},
    {"DEFM_CTRL_SS_RESET_E3_E", 0x6010, 0x10000, 16, 1},
    {"DEFM_CTRL_SS_RESET_E3_RCOR", 0x6010, 0x40000, 18, 1},
    {"DEFM_CTRL_SS_RESET_E3_RWIN", 0x6010, 0x20000, 17, 1},
    {"DEFM_CTRL_SS_RESET_E3_T", 0x6010, 0x80000, 19, 1},
    {"DEFM_DEBUG", 0x600c, 0xf, 0, 4},
    {"DEFM_DECOMP_SS_ADDRESS", 0x6910, 0xff, 0, 8},
    {"DEFM_DECOMP_SS_MODE_ENABLE", 0x6910, 0x10000, 16, 1},
    {"DEFM_DECOMP_SS_UDCOMP_TYPE", 0x6910, 0xf000, 12, 4},
    {"DEFM_DECOMP_SS_UDCOMP_WIDTH", 0x6910, 0xf00, 8, 4},
    {"DEFM_DECOMP_SS_WR", 0x6910, 0x80000000, 31, 1},
    {"DEFM_ERR_PACKET_FILTER", 0x6004, 0x3, 0, 2},
    {"DEFM_READY", 0x6000, 0x2, 1, 1},
    {"DEFM_RESTART", 0x6000, 0x1, 0, 1},
    {"DEFM_SNAP_SHOT", 0x6010, 0x1, 0, 1},
    {"DEFM_USER_DATA_FILTER_W0_127_96", 0x610c, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W0_31_0", 0x6100, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W0_63_32", 0x6104, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W0_95_64", 0x6108, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W0_MASK", 0x6110, 0xffff, 0, 16},
    {"DEFM_USER_DATA_FILTER_W1_127_96", 0x612c, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W1_31_0", 0x6120, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W1_63_32", 0x6124, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W1_95_64", 0x6128, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W1_MASK", 0x6130, 0xffff, 0, 16},
    {"DEFM_USER_DATA_FILTER_W2_127_96", 0x614c, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W2_31_0", 0x6140, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W2_63_32", 0x6144, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W2_95_64", 0x6148, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W2_MASK", 0x6150, 0xffff, 0, 16},
    {"DEFM_USER_DATA_FILTER_W3_127_96", 0x616c, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W3_31_0", 0x6160, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W3_63_32", 0x6164, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W3_95_64", 0x6168, 0xffffffff, 0, 32},
    {"DEFM_USER_DATA_FILTER_W3_MASK", 0x6170, 0xffff, 0, 16},
    {"DEFM_USE_ONE_SYMBOL_STROBE", 0x6008, 0x1, 0, 1},
    {"ETH_DEST_ADDR_31_0", 0xa000, 0xffffffff, 0, 32},
    {"ETH_DEST_ADDR_47_32", 0xa004, 0xffff, 0, 16},
    {"ETH_DU_TABLE_RD_DEST_ADDR_31_0", 0xa0f0, 0xffffffff, 0, 32},
    {"ETH_DU_TABLE_RD_DEST_ADDR_47_32", 0xa0f4, 0xffff, 0, 16},
    {"ETH_DU_TABLE_RD_STROBE", 0xa0fc, 0x80000000, 31, 1},
    {"ETH_DU_TABLE_RD_TABLE_ADDR", 0xa0fc, 0xf, 0, 4},
    {"ETH_DU_TABLE_RD_VLAN_DEI", 0xa0f8, 0x1000, 12, 1},
    {"ETH_DU_TABLE_RD_VLAN_ID", 0xa0f8, 0xfff, 0, 12},
    {"ETH_DU_TABLE_RD_VLAN_PCP", 0xa0f8, 0xe000, 13, 3},
    {"ETH_DU_TABLE_WR_DEST_ADDR_31_0", 0xa0e0, 0xffffffff, 0, 32},
    {"ETH_DU_TABLE_WR_DEST_ADDR_47_32", 0xa0e4, 0xffff, 0, 16},
    {"ETH_DU_TABLE_WR_STROBE", 0xa0ec, 0x80000000, 31, 1},
    {"ETH_DU_TABLE_WR_TABLE_ADDR", 0xa0ec, 0xf, 0, 4},
    {"ETH_DU_TABLE_WR_VLAN_DEI", 0xa0e8, 0x1000, 12, 1},
    {"ETH_DU_TABLE_WR_VLAN_ID", 0xa0e8, 0xfff, 0, 12},
    {"ETH_DU_TABLE_WR_VLAN_PCP", 0xa0e8, 0xe000, 13, 3},
    {"ETH_IPV4_DESTINATION_ADD", 0xa04c, 0xffffffff, 0, 32},
    {"ETH_IPV4_DSCP", 0xa034, 0x3f, 0, 6},
    {"ETH_IPV4_ECN", 0xa034, 0xc0, 6, 2},
    {"ETH_IPV4_FLAGS", 0xa03c, 0x7, 0, 3},
    {"ETH_IPV4_FRAGMENT_OFFSET", 0xa03c, 0xfff8, 3, 13},
    {"ETH_IPV4_ID", 0xa038, 0xffff, 0, 16},
    {"ETH_IPV4_IHL", 0xa030, 0xf0, 4, 4},
    {"ETH_IPV4_PROTOCOL", 0xa044, 0xff, 0, 8},
    {"ETH_IPV4_SOURCE_ADD", 0xa048, 0xffffffff, 0, 32},
    {"ETH_IPV4_TIME_TO_LIVE", 0xa040, 0xff, 0, 8},
    {"ETH_IPV4_VERSION", 0xa030, 0xf, 0, 4},
    {"ETH_IPV6_DESTINATION_ADD_127_96", 0xa0b0, 0xffffffff, 0, 32},
    {"ETH_IPV6_DESTINATION_ADD_31_0", 0xa0a4, 0xffffffff, 0, 32},
    {"ETH_IPV6_DESTINATION_ADD_63_32", 0xa0a8, 0xffffffff, 0, 32},
    {"ETH_IPV6_DESTINATION_ADD_95_64", 0xa0ac, 0xffffffff, 0, 32},
    {"ETH_IPV6_FLOW_LABEL", 0xa088, 0xfffff, 0, 20},
    {"ETH_IPV6_HOP_LIMIT", 0xa090, 0xff, 0, 8},
    {"ETH_IPV6_NEXT_HEADER", 0xa08c, 0xff, 0, 8},
    {"ETH_IPV6_SOURCE_ADD_127_96", 0xa0a0, 0xffffffff, 0, 32},
    {"ETH_IPV6_SOURCE_ADD_31_0", 0xa094, 0xffffffff, 0, 32},
    {"ETH_IPV6_SOURCE_ADD_63_32", 0xa098, 0xffffffff, 0, 32},
    {"ETH_IPV6_SOURCE_ADD_95_64", 0xa09c, 0xffffffff, 0, 32},
    {"ETH_IPV6_TRAFFIC_CLASS", 0xa084, 0xff, 0, 8},
    {"ETH_IPV6_V", 0xa080, 0xf, 0, 4},
    {"ETH_MULTI_ODU_SUPPORT", 0xa0d0, 0x1, 0, 1},
    {"ETH_SRC_ADDR_31_0", 0xa008, 0xffffffff, 0, 32},
    {"ETH_SRC_ADDR_47_32", 0xa00c, 0xffff, 0, 16},
    {"ETH_UDP_DESTINATION_PORT", 0xa050, 0xffff0000, 16, 16},
    {"ETH_UDP_SOURCE_PORT", 0xa050, 0xffff, 0, 16},
    {"ETH_VLAN_DEI", 0xa010, 0x1000, 12, 1},
    {"ETH_VLAN_ID", 0xa010, 0xfff, 0, 12},
    {"ETH_VLAN_PCP", 0xa010, 0xe000, 13, 3},
    {"FRAM_DISABLE", 0x2000, 0x1, 0, 1},
    {"FRAM_ENABLE_PER_SYM_RESET", 0x2004, 0x1, 0, 1},
    {"FRAM_GEN_VLAN_TAG", 0x2200, 0x10, 4, 1},
    {"FRAM_MTU_SIZE", 0x2008, 0x3fff, 0, 14},
    {"FRAM_PROTOCOL_DEFINITION", 0x2200, 0xf, 0, 4},
    {"FRAM_READY", 0x2000, 0x2, 1, 1},
    {"FRAM_SEL_IPV_ADDRESS_TYPE", 0x2200, 0x60, 5, 2},
    {"FRAM_XRAN_BANDSECTOR_FIELD_WIDTH", 0x2080, 0xf00, 8, 4},
    {"FRAM_XRAN_CC_ID_FIELD_WIDTH", 0x2080, 0xf0, 4, 4},
    {"FRAM_XRAN_RU_PORT_ID_FIELD_WIDTH", 0x2080, 0xf, 0, 4},
    {"ORAN_CC_DL_CTRL_OFFSETS", 0xe104, 0xffff, 0, 16},
    {"ORAN_CC_DL_CTRL_SYM_NUM_INDEX", 0xe114, 0x3f0000, 16, 6},
    {"ORAN_CC_DL_CTRL_UNROLLED_OFFSETS", 0xe108, 0xffff, 0, 16},
    {"ORAN_CC_DL_DATA_SYM_NUM_INDEX", 0xe114, 0x3f00, 8, 6},
    {"ORAN_CC_DL_DATA_SYM_START_INDEX", 0xe114, 0x3f, 0, 6},
    {"ORAN_CC_DL_DATA_UNROLL_OFFSET", 0xe500, 0xffff, 0, 16},
    {"ORAN_CC_DL_MPLANE_UDCOMP_HDR_SEL", 0xe11c, 0x100, 8, 1},
    {"ORAN_CC_DL_SETUP_C_ABS_SYMBOL", 0xe130, 0xfff, 0, 12},
    {"ORAN_CC_DL_SETUP_C_CYCLES", 0xe134, 0x1ffff, 0, 17},
    {"ORAN_CC_DL_SETUP_D_CYCLES", 0xe138, 0x1ffff, 0, 17},
    {"ORAN_CC_DL_UD_COMP_METH", 0xe11c, 0xf0, 4, 4},
    {"ORAN_CC_DL_UD_IQ_WIDTH", 0xe11c, 0xf, 0, 4},
    {"ORAN_CC_ENABLE", 0xe004, 0xff, 0, 8},
    {"ORAN_CC_MAX_SYMBOLS", 0xe158, 0xfff, 0, 12},
    {"ORAN_CC_MODVALS_DL", 0xe168, 0xffff, 0, 16},
    {"ORAN_CC_MODVALS_UL", 0xe16c, 0xffff, 0, 16},
    {"ORAN_CC_NUMEROLOGY", 0xe100, 0x70000, 16, 3},
    {"ORAN_CC_NUMRBS", 0xe100, 0x1ff, 0, 9},
    {"ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC", 0xe960, 0xffff, 0, 16},
    {"ORAN_CC_NUM_CTRL_PER_SYMBOL_DL", 0xe160, 0xffff, 0, 16},
    {"ORAN_CC_NUM_CTRL_PER_SYMBOL_UL", 0xe164, 0xffff, 0, 16},
    {"ORAN_CC_PRACH_MPLANE_UDCOMP_HDR_SEL", 0xe920, 0x100, 8, 1},
    {"ORAN_CC_PRACH_UD_COMP_METH", 0xe920, 0xf0, 4, 4},
    {"ORAN_CC_PRACH_UD_IQ_WIDTH", 0xe920, 0xf, 0, 4},
    {"ORAN_CC_RELOAD", 0xe000, 0xff, 0, 8},
    {"ORAN_CC_SSB_CTRL_OFFSETS", 0xe904, 0xffff, 0, 16},
    {"ORAN_CC_SSB_DATA_SYM_START_INDEX", 0xe914, 0x3f, 0, 6},
    {"ORAN_CC_SSB_DATA_UNROLL_OFFSET", 0xed00, 0xffff, 0, 16},
    {"ORAN_CC_SSB_MPLANE_UDCOMP_HDR_SEL", 0xe91c, 0x100, 8, 1},
    {"ORAN_CC_SSB_NUMEROLOGY", 0xe900, 0x70000, 16, 3},
    {"ORAN_CC_SSB_NUMRBS", 0xe900, 0x1ff, 0, 9},
    {"ORAN_CC_SSB_NUM_DATA_SYM_PER_CC", 0xe914, 0x3f00, 8, 6},
    {"ORAN_CC_SSB_NUM_SYM_PER_CC", 0xe914, 0x3f0000, 16, 6},
    {"ORAN_CC_SSB_SECTS_X_SYMBOLS", 0xe968, 0xffff, 0, 16},
    {"ORAN_CC_SSB_SETUP_C_ABS_SYMBOL", 0xe930, 0xfff, 0, 12},
    {"ORAN_CC_SSB_SETUP_C_CYCLES", 0xe934, 0x1ffff, 0, 17},
    {"ORAN_CC_SSB_SETUP_D_CYCLES", 0xe938, 0x1ffff, 0, 17},
    {"ORAN_CC_SSB_SYMPERSLOT", 0xe900, 0x1000000, 24, 1},
    {"ORAN_CC_SSB_UD_COMP_METH", 0xe91c, 0xf0, 4, 4},
    {"ORAN_CC_SSB_UD_IQ_WIDTH", 0xe91c, 0xf, 0, 4},
    {"ORAN_CC_SYMPERSLOT", 0xe100, 0x1000000, 24, 1},
    {"ORAN_CC_UL_BASE_OFFSET", 0xe140, 0xffff, 0, 16},
    {"ORAN_CC_UL_BIDF_C_ABS_SYMBOL", 0xe144, 0xfff, 0, 12},
    {"ORAN_CC_UL_BIDF_C_CYCLES", 0xe148, 0x1ffff, 0, 17},
    {"ORAN_CC_UL_CTRL_OFFSETS", 0xe10c, 0xffff, 0, 16},
    {"ORAN_CC_UL_CTRL_SYM_NUM_INDEX", 0xe114, 0x3f000000, 24, 6},
    {"ORAN_CC_UL_CTRL_UNROLLED_OFFSETS", 0xe110, 0xffff, 0, 16},
    {"ORAN_CC_UL_MPLANE_UDCOMP_HDR_SEL", 0xe118, 0x100, 8, 1},
    {"ORAN_CC_UL_SETUP_C_ABS_SYMBOL", 0xe120, 0xfff, 0, 12},
    {"ORAN_CC_UL_SETUP_C_CYCLES", 0xe124, 0x1ffff, 0, 17},
    {"ORAN_CC_UL_SETUP_D_CYCLES", 0xe128, 0x1ffff, 0, 17},
    {"ORAN_CC_UL_UD_COMP_METH", 0xe118, 0xf0, 4, 4},
    {"ORAN_CC_UL_UD_IQ_WIDTH", 0xe118, 0xf, 0, 4},
    {"ORAN_SETUP_CNT", 0xe600, 0xffffffff, 0, 32},
    {"ORAN_SETUP_SF", 0xe608, 0xf, 0, 4},
    {"ORAN_SETUP_SL", 0xe60c, 0xf, 0, 4},
    {"ORAN_SETUP_SY", 0xe610, 0xf, 0, 4},
    {"ORAN_SETUP_SY_COUNTER", 0xe604, 0x2, 1, 1},
    {"ORAN_STRIP_FCS", 0xe604, 0x1, 0, 1},
    {"STATS_ETH_STATS_ORAN_RX_BIT_RATE", 0xc01c, 0xffffffff, 0, 32},
    {"STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_H", 0xc014, 0xffffffff, 0, 32},
    {"STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_L", 0xc010, 0xffffffff, 0, 32},
    {"STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_H", 0xc00c, 0xffffffff, 0, 32},
    {"STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_L", 0xc008, 0xffffffff, 0, 32},
    {"STATS_ETH_STATS_TOTAL_RX_BIT_RATE", 0xc018, 0xffffffff, 0, 32},
    {"STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_H", 0xc004, 0xffffffff, 0, 32},
    {"STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_L", 0xc000, 0xffffffff, 0, 32},
    {"STATS_OFFSET_EARLIEST_C_PKT", 0xc084, 0xfff, 0, 12},
    {"STATS_OFFSET_EARLIEST_U_PKT", 0xc080, 0xfff, 0, 12},
    {"STATS_ORAN_RX_CORRUPT_H", 0xc064, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_CORRUPT_L", 0xc060, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_EARLY_C_H", 0xc054, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_EARLY_C_L", 0xc050, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_EARLY_H", 0xc034, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_EARLY_L", 0xc030, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_ERROR_DROP_H", 0xc06c, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_ERROR_DROP_L", 0xc068, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_LATE_C_H", 0xc05c, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_LATE_C_L", 0xc058, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_LATE_H", 0xc03c, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_LATE_L", 0xc038, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_ON_TIME_C_H", 0xc04c, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_ON_TIME_C_L", 0xc048, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_ON_TIME_H", 0xc02c, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_ON_TIME_L", 0xc028, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_TOTAL_C_H", 0xc044, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_TOTAL_C_L", 0xc040, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_TOTAL_H", 0xc024, 0xffffffff, 0, 32},
    {"STATS_ORAN_RX_TOTAL_L", 0xc020, 0xffffffff, 0, 32},
    {"STATS_ORAN_TX_TOTAL_C_H", 0xc07c, 0xffffffff, 0, 32},
    {"STATS_ORAN_TX_TOTAL_C_L", 0xc078, 0xffffffff, 0, 32},
    {"STATS_ORAN_TX_TOTAL_H", 0xc074, 0xffffffff, 0, 32},
    {"STATS_ORAN_TX_TOTAL_L", 0xc070, 0xffffffff, 0, 32},
    // Last line has to be NULL, 0, 0, 0
    {NULL, 0, 0, 0},
};

// Number of registers in register map
// Note, the -1 is to skip the NULL terminating entry
static const int fhi_reg_num = (sizeof(fhi_reg_map) / sizeof(reg_info_t)) - 1;

int xorif_read_fhi_reg(const char *name, uint32_t *val)
{
    TRACE("xorif_read_fhi_reg(%s)\n", name);

    const reg_info_t *ptr = find_register(&fhi_reg_map[0], fhi_reg_num, name);
    if (!ptr)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return XORIF_REGISTER_NOT_FOUND;
    }
    else
    {
        *val = read_reg(DEVICE, name, ptr->addr, ptr->mask, ptr->shift, ptr->width);
        return XORIF_SUCCESS;
    }
}

int xorif_read_fhi_reg_offset(const char *name, uint16_t offset, uint32_t *val)
{
    TRACE("xorif_read_fhi_reg_offset(%s, 0x%X)\n", name, offset);

    const reg_info_t *ptr = find_register(&fhi_reg_map[0], fhi_reg_num, name);
    if (!ptr)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return XORIF_REGISTER_NOT_FOUND;
    }
    else
    {
        *val = read_reg(DEVICE, name, ptr->addr + offset, ptr->mask, ptr->shift, ptr->width);
        return XORIF_SUCCESS;
    }
}

int xorif_write_fhi_reg(const char *name, uint32_t value)
{
    TRACE("xorif_write_fhi_reg(%s, 0x%X)\n", name, value);

    const reg_info_t *ptr = find_register(&fhi_reg_map[0], fhi_reg_num, name);
    if (!ptr)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return XORIF_REGISTER_NOT_FOUND;
    }
    else
    {
        write_reg(DEVICE, name, ptr->addr, ptr->mask, ptr->shift, ptr->width, value);
        return XORIF_SUCCESS;
    }
}

int xorif_write_fhi_reg_offset(const char *name, uint16_t offset, uint32_t value)
{
    TRACE("xorif_write_fhi_reg(%s, 0x%X, 0x%X)\n", name, offset, value);

    const reg_info_t *ptr = find_register(&fhi_reg_map[0], fhi_reg_num, name);
    if (!ptr)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return XORIF_REGISTER_NOT_FOUND;
    }
    else
    {
        write_reg(DEVICE, name, ptr->addr + offset, ptr->mask, ptr->shift, ptr->width, value);
        return XORIF_SUCCESS;
    }
}

/** @} */