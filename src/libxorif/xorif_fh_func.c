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
 * @file xorif_fh_func.c
 * @author Steven Dickinson
 * @brief Source file for libxorif Front-Haul Interface (FHI) functions.
 * @addtogroup libxorif
 * @{
 */

#include <string.h>
#include <math.h>
#include "xorif_api.h"
#include "xorif_system.h"
#include "xorif_common.h"
#include "xorif_fh_func.h"
#include "xorif_utils.h"
#include "oran_radio_if_v2_0_ctrl.h"

// Clock default value (gets set later from register)
static double XRAN_TIMER_CLK = 2500; 

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
    {"CFG_CONFIG_MAP_TABLE_W", 0x114, 0xf, 0, 4},
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
    {"DEFM_CID_FT_MAP_MODE", 0x6900, 0x3, 0, 2},
    {"DEFM_CID_FT_RD_STREAM_PORTID", 0x6908, 0x7c0000, 18, 5},
    {"DEFM_CID_FT_RD_STREAM_TYPE", 0x6908, 0x7000, 12, 3},
    {"DEFM_CID_FT_RD_STROBE", 0x6908, 0x80000000, 31, 1},
    {"DEFM_CID_FT_RD_TABLE_ADDR", 0x6908, 0x7ff, 0, 11},
    {"DEFM_CID_FT_WR_STREAM_PORTID", 0x6904, 0x7c0000, 18, 5},
    {"DEFM_CID_FT_WR_STREAM_TYPE", 0x6904, 0x7000, 12, 3},
    {"DEFM_CID_FT_WR_STROBE", 0x6904, 0x80000000, 31, 1},
    {"DEFM_CID_FT_WR_TABLE_ADDR", 0x6904, 0x7ff, 0, 11},
    {"DEFM_CID_LTE_MASK", 0x6054, 0xff, 0, 8},
    {"DEFM_CID_LTE_VALUE", 0x6058, 0xff, 0, 8},
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
    {"ETH_SRC_ADDR_31_0", 0xa008, 0xffffffff, 0, 32},
    {"ETH_SRC_ADDR_47_32", 0xa00c, 0xffff, 0, 16},
    {"ETH_UDP_DESTINATION_PORT", 0xa050, 0xffff0000, 16, 16},
    {"ETH_UDP_SOURCE_PORT", 0xa050, 0xffff, 0, 16},
    {"ETH_VLAN_DEI", 0xa010, 0x1000, 12, 1},
    {"ETH_VLAN_ID", 0xa010, 0xfff, 0, 12},
    {"ETH_VLAN_PCP", 0xa010, 0xe000, 13, 3},
    {"FRAM_DISABLE", 0x2000, 0x1, 0, 1},
    {"FRAM_FIFO_FULL_INDICATOR", 0x2004, 0xffffffff, 0, 32},
    {"FRAM_GEN_VLAN_TAG", 0x2200, 0x10, 4, 1},
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

// Macros to access register map header file
#define NAME(a) (#a)
#define ADDR(a) (a##_ADDR)
#define MASK(a) (a##_MASK)
#define SHIFT(a) (a##_OFFSET)
#define WIDTH(a) (a##_WIDTH)

#ifdef NO_HW
uint32_t fake_reg_bank[0x10000 / 4]; // Fake register bank
#define DEVICE (fake_reg_bank)
#else
#define DEVICE (fh_device.io)
#endif

/**
 * @brief Macro handles read from specified register field.
 */
#define READ_REG(a) read_reg(DEVICE, #a, ADDR(a), MASK(a), SHIFT(a), WIDTH(a))

/**
 * @brief Macro handles read from specified register field (address + offset).
 */
#define READ_REG_OFFSET(a, o) read_reg(DEVICE, #a, ADDR(a) + (o), MASK(a), SHIFT(a), WIDTH(a))

/**
 * @brief Macro handles raw read from specified register address.
 */
#define READ_REG_RAW(a) read_reg_raw(DEVICE, #a, a)
#define READ_REG_RAW_ALT(s, a) read_reg_raw(DEVICE, s, a)

/**
 * @brief Macro handles write to specified register field.
 */
#define WRITE_REG(a, v) write_reg(DEVICE, #a, ADDR(a), MASK(a), SHIFT(a), WIDTH(a), v)

/**
 * @brief Macro handles write to specified register field (address + offset).
 */
#define WRITE_REG_OFFSET(a, o, v) write_reg(DEVICE, #a, ADDR(a) + (o), MASK(a), SHIFT(a), WIDTH(a), v)

/**
 * @brief Macro handles raw write to specified register address.
 */
#define WRITE_REG_RAW(a, v) write_reg_raw(DEVICE, #a, a, v)
#define WRITE_REG_RAW_ALT(s, a, v) write_reg_raw(DEVICE, s, a, v)

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

// Symbol duration look-up table (in microseconds)
// TODO: short, long or average?
//static const double sym_period_table[] = {71.35, 35.68, 17.84, 20.83, 8.92, 4.46};
//static const double sym_period_table[] = {71.88, 36.2, 18.36, 20.83, 9.44, 4.98};
static const double sym_period_table[] =
    {
        71.42857143,
        35.71428571,
        17.85714286,
        8.928571429,
        4.464285714
    };

// FHI alarm flags
static enum xorif_fhi_alarms fhi_alarm_status = 0;

// FHI ISR callback function
static isr_func_t fhi_callback = NULL;

// Memory allocation pointers
static void *ul_ctrl_memory = NULL;
static void *ul_ctrl_base_memory = NULL;
static void *dl_ctrl_memory = NULL;
static void *dl_data_ptrs_memory = NULL;
static void *dl_data_buff_memory = NULL;
static void *ssb_ctrl_memory = NULL;
static void *ssb_data_ptrs_memory = NULL;
static void *ssb_data_buff_memory = NULL;

// Copy of RU bits and BS bits for use in RU ports table mapping
uint16_t num_ru_bits = 0;
uint16_t num_bs_bits = 0;

// Start address of packet filter word
#define DEFM_USER_DATA_FILTER_ADDR DEFM_USER_DATA_FILTER_W0_31_0_ADDR

// Unknown "stream type" for RU port mapping (used to de-allocate the address)
#define UNKNOWN_STREAM_TYPE 0x3F

// Local function prototypes...
static uint16_t calc_sym_num(uint16_t numerology, uint16_t extended_cp, double time);
static uint16_t calc_data_buff_size(uint16_t num_rbs,
                                    enum xorif_iq_comp comp_mode,
                                    uint16_t comp_width,
                                    uint16_t num_sect,
                                    uint16_t num_frames);
static void deallocate_memory(int cc);

#if defined(ENABLE_INTERRUPTS) && !defined(NO_HW)
static int fhi_irq_handler(int id, void *data);
#endif

#ifdef NO_HW
static void init_fake_reg_bank(void);
#endif

// API functions...

int xorif_reset_fhi(uint16_t mode)
{
    TRACE("xorif_reset_fhi(%d)\n", mode);

    // Reset framer/de-framer
    WRITE_REG(FRAM_DISABLE, 1);
    WRITE_REG(DEFM_RESTART, 1);

    if (mode == 0)
    {
        // Enable framer/de-framer
        WRITE_REG(FRAM_DISABLE, 0);
        WRITE_REG(DEFM_RESTART, 0);

        // TODO should we wait for "ready" signal?

        // Initialize the memory allocation pointers
        init_memory_allocator(&ul_ctrl_memory, 0, 1024 * fhi_caps.max_ul_ctrl_1kwords);
        init_memory_allocator(&ul_ctrl_base_memory, 0, fhi_caps.max_subcarriers / RE_PER_RB);
        init_memory_allocator(&dl_ctrl_memory, 0, 1024 * fhi_caps.max_dl_ctrl_1kwords);
        init_memory_allocator(&dl_data_ptrs_memory, 0, fhi_caps.max_data_symbols);
        init_memory_allocator(&dl_data_buff_memory, 0, 1024 * fhi_caps.max_dl_data_1kwords);
        init_memory_allocator(&ssb_ctrl_memory, 0, 512 * fhi_caps.max_ssb_ctrl_512words);
        init_memory_allocator(&ssb_data_ptrs_memory, 0, fhi_caps.max_data_symbols);
        init_memory_allocator(&ssb_data_buff_memory, 0, 512 * fhi_caps.max_ssb_data_512words);

        // Reset component carrier enables
        WRITE_REG(ORAN_CC_ENABLE, 0);

        // Clear alarms and counters
        xorif_clear_fhi_alarms();
        xorif_clear_fhi_stats();
    }

    return XORIF_SUCCESS;
}

uint32_t xorif_get_fhi_hw_version(void)
{
    TRACE("xorif_get_fhi_hw_version()\n");

    uint32_t major = READ_REG(CFG_MAJOR_REVISION);
    uint32_t minor = READ_REG(CFG_MINOR_REVISION);
    uint32_t revision = READ_REG(CFG_VERSION_REVISION);

    return major << 24 | minor << 16 | revision;
}

uint32_t xorif_get_fhi_hw_internal_rev(void)
{
    TRACE("xorif_get_fhi_hw_internal_rev()\n");
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

void xorif_clear_fhi_stats(void)
{
    TRACE("xorif_clear_fhi_stats()\n");

    // Take snapshot (with reset)
    WRITE_REG_RAW(DEFM_SNAP_SHOT_ADDR, 0xFFFFFFFF);
}

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

int xorif_get_fhi_eth_stats(int port, struct xorif_fhi_eth_stats *ptr)
{
    TRACE("xorif_get_fhi_eth_stats(%d, ...)\n", port);

    if (port >= xorif_fhi_get_num_eth_ports())
    {
        PERROR("Invalid Ethernet port value\n");
        return XORIF_INVALID_ETH_PORT;
    }

    if (ptr)
    {
        // Take snapshot (no reset)
        WRITE_REG(DEFM_SNAP_SHOT, 1);

        // Read stat counters...
        ptr->total_rx_good_pkt_cnt = (uint64_t)READ_REG_OFFSET(STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_L, port * 0x100) |
                                     (uint64_t)READ_REG_OFFSET(STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_H, port * 0x100) << 32;

        ptr->total_rx_bad_pkt_cnt = (uint64_t)READ_REG_OFFSET(STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_L, port * 0x100) |
                                    (uint64_t)READ_REG_OFFSET(STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_H, port * 0x100) << 32;

        ptr->total_rx_bad_fcs_cnt = (uint64_t)READ_REG_OFFSET(STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_L, port * 0x100) |
                                    (uint64_t)READ_REG_OFFSET(STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_H, port * 0x100) << 32;

        ptr->total_rx_bit_rate = (uint64_t)READ_REG_OFFSET(STATS_ETH_STATS_TOTAL_RX_BIT_RATE, port * 0x100) * 64;

        ptr->oran_rx_bit_rate = (uint64_t)READ_REG_OFFSET(STATS_ETH_STATS_ORAN_RX_BIT_RATE, port * 0x100) * 64;

        ptr->oran_rx_total = (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_TOTAL_L, port * 0x100) |
                             (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_TOTAL_H, port * 0x100) << 32;

        ptr->oran_rx_on_time = (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_ON_TIME_L, port * 0x100) |
                               (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_ON_TIME_H, port * 0x100) << 32;

        ptr->oran_rx_early = (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_EARLY_L, port * 0x100) |
                             (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_EARLY_H, port * 0x100) << 32;

        ptr->oran_rx_late = (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_LATE_L, port * 0x100) |
                            (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_LATE_H, port * 0x100) << 32;

        ptr->oran_rx_total_c = (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_TOTAL_C_L, port * 0x100) |
                               (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_TOTAL_C_H, port * 0x100) << 32;

        ptr->oran_rx_on_time_c = (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_ON_TIME_C_L, port * 0x100) |
                                 (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_ON_TIME_C_H, port * 0x100) << 32;

        ptr->oran_rx_early_c = (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_EARLY_C_L, port * 0x100) |
                               (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_EARLY_C_H, port * 0x100) << 32;

        ptr->oran_rx_late_c = (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_LATE_C_L, port * 0x100) |
                              (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_LATE_C_H, port * 0x100) << 32;

        ptr->oran_rx_corrupt = (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_CORRUPT_L, port * 0x100) |
                               (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_CORRUPT_H, port * 0x100) << 32;

        ptr->oran_rx_error_drop = (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_ERROR_DROP_L, port * 0x100) |
                                  (uint64_t)READ_REG_OFFSET(STATS_ORAN_RX_ERROR_DROP_H, port * 0x100) << 32;

        ptr->oran_tx_total = (uint64_t)READ_REG_OFFSET(STATS_ORAN_TX_TOTAL_L, port * 0x100) |
                             (uint64_t)READ_REG_OFFSET(STATS_ORAN_TX_TOTAL_H, port * 0x100) << 32;

        ptr->oran_tx_total_c = (uint64_t)READ_REG_OFFSET(STATS_ORAN_TX_TOTAL_C_L, port * 0x100) |
                               (uint64_t)READ_REG_OFFSET(STATS_ORAN_TX_TOTAL_C_H, port * 0x100) << 32;

        ptr->offset_earliest_u_pkt = READ_REG_OFFSET(STATS_OFFSET_EARLIEST_U_PKT, port * 0x100);

        ptr->offset_earliest_c_pkt = READ_REG_OFFSET(STATS_OFFSET_EARLIEST_C_PKT, port * 0x100);

        return XORIF_SUCCESS;
    }
    return XORIF_FAILURE;
}

int xorif_set_fhi_dest_mac_addr(int port, const uint8_t address[])
{
    TRACE("xorif_set_fhi_dest_mac_addr(%d, ...)\n", port);
    if (port >= xorif_fhi_get_num_eth_ports())
    {
        PERROR("Invalid Ethernet port value\n");
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
        PERROR("Invalid Ethernet port value\n");
        return XORIF_INVALID_ETH_PORT;
    }

    uint32_t addr_hi = address[0] << 8 | address[1];
    uint32_t addr_lo = address[2] << 24 | address[3] << 16 | address[4] << 8 | address[5];
    WRITE_REG_OFFSET(ETH_SRC_ADDR_47_32, port * 0x100, addr_hi);
    WRITE_REG_OFFSET(ETH_SRC_ADDR_31_0, port * 0x100, addr_lo);
    return XORIF_SUCCESS;
}

int xorif_set_fhi_protocol(enum xorif_transport_protocol transport,
                           uint16_t vlan,
                           enum xorif_ip_mode ip_mode)
{
    TRACE("xorif_set_fhi_protocol(%d, %d, %d)\n", transport, vlan, ip_mode);

    // Set up the global protocol, VLAN and IP mode
    WRITE_REG(FRAM_PROTOCOL_DEFINITION, transport);
    WRITE_REG(FRAM_GEN_VLAN_TAG, vlan);
    WRITE_REG(FRAM_SEL_IPV_ADDRESS_TYPE, ip_mode);

    // Packet filter and mask arrays
    uint32_t filter[16];
    uint16_t mask[4];

    // Initialize defaults (no packet filter)
    for (int i = 0; i < 4; ++i)
    {
        mask[i] = 0xFFFF;
    }
    for (int i = 0; i < 16; ++i)
    {
        filter[i] = 0xFFFFFFFF;
    }

    // There are 2 "transport", 3 "ip modes" and 2 VLAN options, i.e. 12 configurations!
    // Packet filters can only be defined for some of them. For the others, the user
    // will have to configure manually.

    // Only "Raw" mode is handled currently
    if (ip_mode == IP_MODE_RAW)
    {
        switch (transport)
        {
        case PROTOCOL_ECPRI:
            if (vlan)
            {
                // ECPRI, VLAN, RAW
                mask[0] = 0xCFFF;
                mask[1] = 0xFFFC;
                filter[3] = 0xFFFF0081;
                filter[4] = 0xFFFFFEAE;
            }
            else
            {
                // ECPRI, RAW
                mask[0] = 0xCFFF;
                filter[3] = 0xFFFFFEAE;
            }
            break;

        case PROTOCOL_IEEE_1914_3:
            if (vlan)
            {
                // 1914.3, VLAN, RAW
                mask[0] = 0xCFFF;
                mask[1] = 0xFFFC;
                filter[3] = 0xFFFF0081;
                filter[4] = 0xFFFF3DFC;
            }
            else
            {
                // 1914.3, RAW
                mask[0] = 0xCFFF;
                filter[3] = 0xFFFF3DFC;
            }
            break;
        }
    }

    // Configure the same packet filter for all Ethernet ports
    for (int i = 0; i < xorif_fhi_get_num_eth_ports(); ++i)
    {
        xorif_set_fhi_packet_filter(i, filter, mask);
    }

    return XORIF_SUCCESS;
}

int xorif_set_fhi_vlan_tag(int port, uint16_t id, uint16_t dei, uint16_t pcp)
{
    TRACE("xorif_set_fhi_vlan_tag(%d, %d, %d, %d)\n", port, id, dei, pcp);

    if (port >= xorif_fhi_get_num_eth_ports())
    {
        PERROR("Invalid Ethernet port value\n");
        return XORIF_INVALID_ETH_PORT;
    }

    // VLAN tag (ID bits 0-11; DEI bit 12; PCP bits 13-15)
    WRITE_REG_OFFSET(ETH_VLAN_ID, port * 0x100, id);
    WRITE_REG_OFFSET(ETH_VLAN_DEI, port * 0x100, dei);
    WRITE_REG_OFFSET(ETH_VLAN_PCP, port * 0x100, pcp);

    return XORIF_SUCCESS;
}

int xorif_set_fhi_packet_filter(int port, const uint32_t filter[16], uint16_t mask[4])
{
    TRACE("xorif_set_fhi_packet_filter(%d, ...)\n", port);

    if (port >= xorif_fhi_get_num_eth_ports())
    {
        PERROR("Invalid Ethernet port value\n");
        return XORIF_INVALID_ETH_PORT;
    }

    // 4 packet filter words
    for (int i = 0; i < 4; ++i)
    {
        // 4 x 32-bit words per packet filter word
        for (int j = 0; j < 4; ++j)
        {
            //int a = (j+1) * 32 - 1;
            //int b = j * 32;
            uint32_t addr = DEFM_USER_DATA_FILTER_ADDR + (port * 0x100) + (i * 0x20) + (j * 4);
            //INFO("Packet filter word[%d][%d_%d]\n", i, a, b);
            WRITE_REG_RAW_ALT("DEFM_USER_DATA_FILTER_WORD_ADDR", addr, *filter++);
        }

        // 1 x 16-bit mask word per packet filter word
        uint32_t addr = DEFM_USER_DATA_FILTER_ADDR + (port * 0x100) + (i * 0x20) + (4 * 4);
        //INFO("Packet filter mask[%d]\n", i,);
        WRITE_REG_RAW_ALT("DEFM_USER_DATA_FILTER_MASK_ADDR", addr, *mask++);
    }

    return XORIF_SUCCESS;
}

int xorif_set_fhi_eaxc_id(uint16_t du_bits,
                          uint16_t bs_bits,
                          uint16_t cc_bits,
                          uint16_t ru_bits)
{
    TRACE("xorif_set_fhi_eaxc_id(%d, %d, %d, %d)\n", du_bits, bs_bits, cc_bits, ru_bits);

    if ((du_bits + bs_bits + cc_bits + ru_bits) != 16)
    {
        PERROR("Total ID bits does not equal 16\n");
        return XORIF_INVALID_EAXC_ID;
    }
    else if ((du_bits > fhi_caps.du_id_limit) || (bs_bits > fhi_caps.bs_id_limit) ||
             (cc_bits > fhi_caps.cc_id_limit) || (ru_bits > fhi_caps.ru_id_limit))
    {
        PERROR("Number of ID bits is larger than allowed\n");
        return XORIF_INVALID_EAXC_ID;
    }

    // The eAxC ID is 16 bits : (msb) [DU][BS][CC][RU] (lsb)

    // Create shifts
    uint16_t ru_shift = 0;
    uint16_t cc_shift = ru_shift + ru_bits;
    uint16_t bs_shift = cc_shift + cc_bits;
    uint16_t du_shift = bs_shift + bs_bits;

    // Create masks
    uint16_t ru_mask = (1 << ru_bits) - 1;
    uint16_t cc_mask = (1 << cc_bits) - 1;
    uint16_t bs_mask = (1 << bs_bits) - 1;
    uint16_t du_mask = (1 << du_bits) - 1;

    // Program the DU/BS/CC masks & shifts
    WRITE_REG(DEFM_CID_CC_SHIFT, cc_shift);
    WRITE_REG(DEFM_CID_CC_MASK, cc_mask);
    WRITE_REG(DEFM_CID_BS_SHIFT, bs_shift);
    WRITE_REG(DEFM_CID_BS_MASK, bs_mask);
    WRITE_REG(DEFM_CID_DU_SHIFT, du_shift);
    WRITE_REG(DEFM_CID_DU_MASK, du_mask);

    // Note, RU ID bits are handled by an additional mask register, so that
    // spatial streams can be further split into user/PRACH/SSB/...

    // Save RU bits and BS bits for use in RU ports table mapping
    num_ru_bits = ru_bits;
    num_bs_bits = bs_bits;

#if DEBUG
    if (xorif_trace >= 2)
    {
        char s[17];
        for (int i = 0; i < 16; ++i)
        {
            if (i >= du_shift)
            {
                s[15 - i] = 'D';
            }
            else if (i >= bs_shift)
            {
                s[15 - i] = 'B';
            }
            else if (i >= cc_shift)
            {
                s[15 - i] = 'C';
            }
            else if (i >= ru_shift)
            {
                s[15 - i] = 'R';
            }
        }
        s[16] = '\0';
        INFO("eAxC ID bits: %s\n", s);
        INFO("DU mask:      %s\n", binary_mask_string(du_mask << du_shift, du_mask << du_shift, 16));
        INFO("BS mask:      %s\n", binary_mask_string(bs_mask << bs_shift, bs_mask << bs_shift, 16));
        INFO("CC mask:      %s\n", binary_mask_string(cc_mask << cc_shift, cc_mask << cc_shift, 16));
        INFO("RU mask:      %s\n", binary_mask_string(ru_mask, ru_mask, 16));
    }
#endif

    return XORIF_SUCCESS;
}

int xorif_set_ru_ports(uint16_t ru_bits,
                       uint16_t ss_bits,
                       uint16_t mask,
                       uint16_t user_val,
                       uint16_t prach_val,
                       uint16_t ssb_val)
{
    TRACE("xorif_set_ru_ports(%d, %d, %d, %d, %d, %d)\n", ru_bits, ss_bits, mask, user_val, prach_val, ssb_val);

    if (ss_bits > ru_bits)
    {
        PERROR("Invalid RU port assignment\n");
        return XORIF_INVALID_EAXC_ID;
    }
    else if (ss_bits > fhi_caps.ss_id_limit)
    {
        PERROR("Invalid RU port assignment\n");
        return XORIF_INVALID_EAXC_ID;
    }

    // Create masks
    uint16_t ru_mask = (1 << ru_bits) - 1;
    uint16_t ss_mask = (1 << ss_bits) - 1;
    uint16_t xx_mask = ru_mask & mask;

    // Program the masks and values
    WRITE_REG(DEFM_CID_SS_MASK, ss_mask);
    WRITE_REG(DEFM_CID_U_MASK, xx_mask);
    WRITE_REG(DEFM_CID_U_VALUE, user_val & xx_mask);
    WRITE_REG(DEFM_CID_PRACH_MASK, xx_mask);
    WRITE_REG(DEFM_CID_PRACH_VALUE, prach_val & xx_mask);

    if (ssb_val > mask)
    {
        // Disable SSB mapping (since masking with 0x00 can never == 0xFF)
        WRITE_REG(DEFM_CID_SSB_MASK, 0x00);
        WRITE_REG(DEFM_CID_SSB_VALUE, 0xFF);
    }
    else
    {
        WRITE_REG(DEFM_CID_SSB_MASK, xx_mask);
        WRITE_REG(DEFM_CID_SSB_VALUE, ssb_val & xx_mask);
    }

    // Disable LTE mapping (sonce masking with 0x00 can never == 0xFF)
    WRITE_REG(DEFM_CID_LTE_MASK, 0x00);
    WRITE_REG(DEFM_CID_LTE_VALUE, 0xFF);

#ifdef DEBUG
    if (xorif_trace >= 2)
    {
        INFO("RU mask:     %s\n", binary_mask_string(ru_mask, ru_mask, 16));
        INFO("SS mask:     %s\n", binary_mask_string(ss_mask, ss_mask, 16));
        INFO("Mask:        %s\n", binary_mask_string(xx_mask, xx_mask, 16));
        INFO("User value:  %s\n", binary_mask_string(user_val & xx_mask, xx_mask, 16));
        INFO("PRACH value: %s\n", binary_mask_string(prach_val & xx_mask, xx_mask, 16));
        if (ssb_val <= mask)
        {
            INFO("SSB value:   %s\n", binary_mask_string(ssb_val & xx_mask, xx_mask, 16));
        }
    }
#endif

    return XORIF_SUCCESS;
}

int xorif_set_ru_ports_alt1(uint16_t ru_bits,
                            uint16_t ss_bits,
                            uint16_t mask,
                            uint16_t user_val,
                            uint16_t prach_val,
                            uint16_t ssb_val,
                            uint16_t lte_val)
{
    TRACE("xorif_set_ru_ports_alt1(%d, %d, %d, %d, %d, %d, %d)\n", ru_bits, ss_bits, mask, user_val, prach_val, ssb_val, lte_val);

    if (ss_bits > ru_bits)
    {
        PERROR("Invalid RU port assignment\n");
        return XORIF_INVALID_EAXC_ID;
    }
    else if (ss_bits > fhi_caps.ss_id_limit)
    {
        PERROR("Invalid RU port assignment\n");
        return XORIF_INVALID_EAXC_ID;
    }

    // Create masks
    uint16_t ru_mask = (1 << ru_bits) - 1;
    uint16_t ss_mask = (1 << ss_bits) - 1;
    uint16_t xx_mask = ru_mask & mask;

    // Program the masks and values
    WRITE_REG(DEFM_CID_SS_MASK, ss_mask);
    WRITE_REG(DEFM_CID_U_MASK, xx_mask);
    WRITE_REG(DEFM_CID_U_VALUE, user_val & xx_mask);
    WRITE_REG(DEFM_CID_PRACH_MASK, xx_mask);
    WRITE_REG(DEFM_CID_PRACH_VALUE, prach_val & xx_mask);

    if (ssb_val > mask)
    {
        // Disable SSB mapping (since masking with 0x00 can never == 0xFF)
        WRITE_REG(DEFM_CID_SSB_MASK, 0x00);
        WRITE_REG(DEFM_CID_SSB_VALUE, 0xFF);
    }
    else
    {
        WRITE_REG(DEFM_CID_SSB_MASK, xx_mask);
        WRITE_REG(DEFM_CID_SSB_VALUE, ssb_val & xx_mask);
    }

    if (lte_val > mask)
    {
        // Disable LTE mapping (since masking with 0x00 can never == 0xFF)
        WRITE_REG(DEFM_CID_LTE_MASK, 0x00);
        WRITE_REG(DEFM_CID_LTE_VALUE, 0xFF);
    }
    else
    {
        WRITE_REG(DEFM_CID_LTE_MASK, xx_mask);
        WRITE_REG(DEFM_CID_LTE_VALUE, lte_val & xx_mask);
    }

#ifdef DEBUG
    if (xorif_trace >= 2)
    {
        INFO("RU mask:     %s\n", binary_mask_string(ru_mask, ru_mask, 16));
        INFO("SS mask:     %s\n", binary_mask_string(ss_mask, ss_mask, 16));
        INFO("Mask:        %s\n", binary_mask_string(xx_mask, xx_mask, 16));
        INFO("User value:  %s\n", binary_mask_string(user_val & xx_mask, xx_mask, 16));
        INFO("PRACH value: %s\n", binary_mask_string(prach_val & xx_mask, xx_mask, 16));
        if (ssb_val <= mask)
        {
            INFO("SSB value:   %s\n", binary_mask_string(ssb_val & xx_mask, xx_mask, 16));
        }
        if (lte_val <= mask)
        {
            INFO("LTE value:   %s\n", binary_mask_string(lte_val & xx_mask, xx_mask, 16));
        }
    }
#endif

    return XORIF_SUCCESS;
}

int xorif_set_ru_ports_table_mode(uint16_t mode)
{
    TRACE("xorif_set_ru_ports_table_mode(%d)\n", mode);

    if (mode > 3)
    {
        PERROR("Invalid RU port mapping mode\n");
        return XORIF_INVALID_RU_PORT_MAPPING;
    }

    // Check that the table has sufficient address space for the mode
    uint16_t required = 0;
    if (mode == 0)
    {
        // mode 0: {RU} so width = ru_bits
        required = num_ru_bits;
    }
    else if (mode == 1)
    {
        // mode 1: {DIR, RU} so width = 1 + ru_bits
        required = 1 + num_ru_bits;
    }
    else if (mode == 2)
    {
        // mode 2: {BS, RU} so width = bs_bits + ru_bits
        required = num_bs_bits + num_ru_bits;
    }
    else if (mode == 3)
    {
        // mode 3: {DIR, RU, BS} so width = 1 + bs_bits + ru_bits
        required = 1 + num_bs_bits + num_ru_bits;
    }

    if ((fhi_caps.ru_ports_map_width == 0) || (required > fhi_caps.ru_ports_map_width))
    {
        PERROR("Insufficient RU port table memory for mode\n");
        return XORIF_INVALID_RU_PORT_MAPPING;
    }
    else
    {
        WRITE_REG(DEFM_CID_FT_MAP_MODE, mode);
        return XORIF_SUCCESS;
    }
}

int xorif_clear_ru_ports_table(void)
{
    TRACE("xorif_clear_ru_ports_table()\n");

    // Note, no need to call this from reset

    // Reset RU port mapping table to UNKNOWN_STREAM_TYPE (i.e. not-used)
    if (fhi_caps.ru_ports_map_width > 0)
    {
        uint16_t size = 1 << fhi_caps.ru_ports_map_width;
        for (int a = 0; a < size; ++a)
        {
            // Value: <write strobe> | <port = 0> | <type = UNKNOWN_STREAM_TYPE> | <address>
            uint32_t value = (1 << 31) | (0 << 18) | (UNKNOWN_STREAM_TYPE << 12) | (a & 0x7FF);
            WRITE_REG_RAW(DEFM_CID_FT_WR_STROBE_ADDR, value);
        }
    }

    return XORIF_SUCCESS;
}

int xorif_set_ru_ports_table(uint16_t address,
                             uint16_t port,
                             uint16_t type,
                             uint16_t number)
{
    TRACE("xorif_set_ru_ports_table(%d, %d, %d, %d)\n", address, port, type, number);

    if (fhi_caps.ru_ports_map_width > 0)
    {
        uint16_t size = 1 << fhi_caps.ru_ports_map_width;
        for (int i = 0; i < number; ++i)
        {
            // Value: <write strobe> | <port> | <type> | <address>
            uint16_t a = address + i;
            uint16_t p = port + i;
            uint32_t value = (1 << 31) | ((p & 0x1F) << 18) | ((type & 0x3F) << 12) | (a & 0x7FF);

            if (a < size)
            {
                WRITE_REG_RAW(DEFM_CID_FT_WR_STROBE_ADDR, value);
            }
            else
            {
                PERROR("Invalid RU port table address\n");
                return XORIF_INVALID_RU_PORT_MAPPING;
            }
        }
        return XORIF_SUCCESS;
    }

    PERROR("Insufficient RU port table memory\n");
    return XORIF_INVALID_RU_PORT_MAPPING;
}

int xorif_get_fhi_cc_alloc(uint16_t cc, struct xorif_cc_alloc *ptr)
{
    TRACE("xorif_get_fhi_cc_alloc(%d, ...)\n", cc);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!ptr)
    {
        PERROR("Null pointer\n");
        return XORIF_NULL_POINTER;
    }

    // Number of symbols
    ptr->ul_ctrl_sym_num = READ_REG_OFFSET(ORAN_CC_UL_CTRL_SYM_NUM_INDEX, cc * 0x70);
    ptr->dl_ctrl_sym_num = READ_REG_OFFSET(ORAN_CC_DL_CTRL_SYM_NUM_INDEX, cc * 0x70);
    ptr->dl_data_sym_num = READ_REG_OFFSET(ORAN_CC_DL_DATA_SYM_NUM_INDEX, cc * 0x70);
    ptr->ssb_ctrl_sym_num = READ_REG_OFFSET(ORAN_CC_SSB_NUM_SYM_PER_CC, cc * 0x70);
    ptr->ssb_data_sym_num = READ_REG_OFFSET(ORAN_CC_SSB_NUM_DATA_SYM_PER_CC, cc * 0x70);

    // Retrieve memory allocation
    get_alloc_block(ul_ctrl_memory, cc, &ptr->ul_ctrl_offset, &ptr->ul_ctrl_size);
    get_alloc_block(ul_ctrl_base_memory, cc, &ptr->ul_ctrl_base_offset, &ptr->ul_ctrl_base_size);
    get_alloc_block(dl_ctrl_memory, cc, &ptr->dl_ctrl_offset, &ptr->dl_ctrl_size);
    get_alloc_block(dl_data_ptrs_memory, cc, &ptr->dl_data_ptrs_offset, &ptr->dl_data_ptrs_size);
    get_alloc_block(dl_data_buff_memory, cc, &ptr->dl_data_buff_offset, &ptr->dl_data_buff_size);
    get_alloc_block(ssb_ctrl_memory, cc, &ptr->ssb_ctrl_offset, &ptr->ssb_ctrl_size);
    get_alloc_block(ssb_data_ptrs_memory, cc, &ptr->ssb_data_ptrs_offset, &ptr->ssb_data_ptrs_size);
    get_alloc_block(ssb_data_buff_memory, cc, &ptr->ssb_data_buff_offset, &ptr->ssb_data_buff_size);

    return XORIF_SUCCESS;
}

int xorif_enable_fhi_interrupts(uint32_t mask)
{
    TRACE("xorif_enable_fhi_interrupts(0x%X)\n", mask);

    // Setup interrupts (enable / disable according to mask value)
    WRITE_REG_RAW(FHI_INTR_ENABLE_ADDR, mask);

    // Finally enable the master interrupt
    WRITE_REG(CFG_MASTER_INT_ENABLE, 1);

    return XORIF_SUCCESS;
}

int xorif_register_fhi_isr(isr_func_t callback)
{
    TRACE("xorif_register_fhi_isr(...)\n");
    fhi_callback = callback;
    return XORIF_SUCCESS;
}

int xorif_set_system_constants(const struct xorif_system_constants *ptr)
{
    TRACE("xorif_set_system_constants(...)\n");

    if (!ptr)
    {
        return XORIF_NULL_POINTER;
    }

    memcpy(&fhi_sys_const, ptr, sizeof(struct xorif_system_constants));
    return XORIF_SUCCESS;
}

int xorif_set_symbol_strobe_source(uint16_t source)
{
    TRACE("xorif_set_symbol_strobe_source(%d)\n", source);
    WRITE_REG(DEFM_USE_ONE_SYMBOL_STROBE, source);
    return XORIF_SUCCESS;
}

// Non-API functions...

#if defined(ENABLE_INTERRUPTS) && !defined(NO_HW)
static int fhi_irq_handler(int id, void *data)
{
    struct xorif_device_info *device = (struct xorif_device_info *)data;

    if (device)
    {
        // Check interrupt status
        uint32_t status = READ_REG_RAW(FHI_INTR_STATUS_ADDR) & FHI_INTR_MASK;

        if (status)
        {
            INFO("fhi_irq_handler() status = 0x%X\n", status);

            // Record the alarm status
            fhi_alarm_status |= status;

            if (fhi_callback)
            {
                // Call registered call-back function
                (*fhi_callback)(status);
            }

            // Default interrupt handling...

            if (status & CFG_DEFM_INT_INFIFO_OF_MASK)
            {
                INFO("FHI IRQ: CFG_DEFM_INT_INFIFO_OF\n");
            }
            if (status & CFG_DEFM_INT_INFIFO_UF_MASK)
            {
                INFO("FHI IRQ: CFG_DEFM_INT_INFIFO_UF\n");
            }
            if (status & CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_MASK)
            {
                INFO("FHI IRQ: CFG_DEFM_INT_ETH_PIPE_C_BUF_OF\n");
            }
            if (status & CFG_DEFM_INT_ETH_PIPE_TABLE_OF_MASK)
            {
                INFO("FHI IRQ: CFG_DEFM_INT_ETH_PIPE_TABLE_OF\n");
            }
            if (status & CFG_FRAM_INT_OUTFIFO_OF_MASK)
            {
                INFO("FHI IRQ: CFG_FRAM_INT_OUTFIFO_OF\n");
            }
            if (status & CFG_FRAM_INT_OUTFIFO_UF_MASK)
            {
                INFO("FHI IRQ: CFG_FRAM_INT_OUTFIFO_UF\n");
            }
            if (status & CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_MASK)
            {
                INFO("FHI IRQ: CFG_FRAM_INT_PRACH_SECTION_OVERFLOW\n");
            }
            if (status & CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_MASK)
            {
                INFO("FHI IRQ: CFG_FRAM_INT_PRACH_SECTION_NOTFOUND\n");
            }
            if (status & CFG_AXI_TIMEOUT_STATUS_MASK)
            {
                INFO("FHI IRQ: CFG_AXI_TIMEOUT_STATUS\n");
            }

            // All interrupts are serious error conditions
            // Reset data-pipe (framer & de-framer)
            WRITE_REG(FRAM_DISABLE, 1);
            WRITE_REG(DEFM_RESTART, 1);
            WRITE_REG(FRAM_DISABLE, 0);
            WRITE_REG(DEFM_RESTART, 0);

            // Clear interrupts by writing to the "master interrupt"
            WRITE_REG(CFG_MASTER_INT_ENABLE, 0);
            WRITE_REG(CFG_MASTER_INT_ENABLE, 1);

            return METAL_IRQ_HANDLED;
        }
    }

    return METAL_IRQ_NOT_HANDLED;
}
#endif

void xorif_fhi_init_device(void)
{
#ifdef NO_HW
    // Initialize fake register bank
    init_fake_reg_bank();
#endif

    // Set-up the FHI capabilities
    memset(&fhi_caps, 0, sizeof(fhi_caps));
    fhi_caps.max_cc = READ_REG(CFG_CONFIG_XRAN_MAX_CC);
    fhi_caps.num_eth_ports = READ_REG(CFG_CONFIG_NO_OF_ETH_PORTS);
    fhi_caps.numerologies = 0x1F; // bit-map: u0 - u4
    fhi_caps.extended_cp = 0;
#if 0
    // De-compression is downlink
    fhi_caps.iq_de_comp_methods = IQ_COMP_NONE_SUPPORT | IQ_COMP_BLOCK_FP_SUPPORT | IQ_COMP_MODULATION_SUPPORT;
    fhi_caps.iq_de_comp_bfp_widths = 0x5200); // 9, 12, 14
    fhi_caps.iq_de_comp_mod_widths = 0x3E; // 1, 2, 3, 4, 5

    // Compression is Uplink
    fhi_caps.iq_comp_methods = IQ_COMP_NONE_SUPPORT | IQ_COMP_BLOCK_FP_SUPPORT;
    fhi_caps.iq_de_comp_bfp_widths = 0x5200); // 9, 12, 14
#else
    // De-compression is downlink
    int de_comp = 0;
    de_comp |= READ_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_NOCOMP) ? IQ_COMP_NONE_SUPPORT : 0;
    de_comp |= READ_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP) ? IQ_COMP_BLOCK_FP_SUPPORT : 0;
    de_comp |= READ_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_BSC) ? IQ_COMP_BLOCK_SCALE_SUPPORT : 0;
    de_comp |= READ_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_MU) ? IQ_COMP_U_LAW_SUPPORT : 0;
    de_comp |= READ_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP) ? IQ_COMP_MODULATION_SUPPORT : 0;
    fhi_caps.iq_de_comp_methods = de_comp;
    fhi_caps.iq_de_comp_bfp_widths = READ_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_WIDTHS);
    fhi_caps.iq_de_comp_mod_widths = READ_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODC_WIDTHS);

    // Compression is Uplink
    int comp = 0;
    comp |= READ_REG(CFG_CONFIG_XRAN_COMP_IN_CORE_NOCOMP) ? IQ_COMP_NONE_SUPPORT : 0;
    comp |= READ_REG(CFG_CONFIG_XRAN_COMP_IN_CORE_BFP) ? IQ_COMP_BLOCK_FP_SUPPORT : 0;
    fhi_caps.iq_comp_methods = comp;
    fhi_caps.iq_comp_bfp_widths = READ_REG(CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_WIDTHS);
#endif
    fhi_caps.no_framer_ss = READ_REG(CFG_CONFIG_NO_OF_FRAM_ANTS);
    fhi_caps.no_deframer_ss = READ_REG(CFG_CONFIG_NO_OF_DEFM_ANTS);
    fhi_caps.max_framer_ethernet_pkt = READ_REG(CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX);
    fhi_caps.max_deframer_ethernet_pkt = READ_REG(CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX);
    fhi_caps.max_subcarriers = READ_REG(CFG_CONFIG_XRAN_MAX_SCS);
    fhi_caps.max_data_symbols = READ_REG(CFG_CONFIG_XRAN_MAX_DL_SYMBOLS);
    fhi_caps.max_ctrl_symbols = READ_REG(CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS);
    fhi_caps.max_ul_ctrl_1kwords = READ_REG(CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS);
    fhi_caps.max_dl_ctrl_1kwords = READ_REG(CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS);
    fhi_caps.max_dl_data_1kwords = READ_REG(CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS);
    fhi_caps.max_ssb_ctrl_512words = 1; // TODO add register read when available
    fhi_caps.max_ssb_data_512words = 2; // TODO add register read when available
    fhi_caps.timer_clk_ps = READ_REG(CFG_CONFIG_XRAN_TIMER_CLK_PS);
    fhi_caps.num_unsolicited_ports = READ_REG(CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM);
    fhi_caps.num_prach_ports = READ_REG(CFG_CONFIG_XRAN_PRACH_C_PORTS);
    fhi_caps.du_id_limit = READ_REG(CFG_CONFIG_LIMIT_DU_W);
    fhi_caps.bs_id_limit = READ_REG(CFG_CONFIG_LIMIT_BS_W);
    fhi_caps.cc_id_limit = READ_REG(CFG_CONFIG_LIMIT_CC_W);
    fhi_caps.ru_id_limit = READ_REG(CFG_CONFIG_LIMIT_RU_I_W);
    fhi_caps.ss_id_limit = READ_REG(CFG_CONFIG_LIMIT_RU_O_W);
    fhi_caps.ru_ports_map_width = READ_REG(CFG_CONFIG_MAP_TABLE_W);

    // Set up any useful defaults, etc.
    XRAN_TIMER_CLK = fhi_caps.timer_clk_ps; //READ_REG(CFG_CONFIG_XRAN_TIMER_CLK_PS);

    // Additional properties extracted from device node
#ifndef NO_HW
    // TODO these might be replaced by registers in future release
    uint32_t temp;
    if (get_device_property_u32(fh_device.dev_name, "xlnx,xran-max-ssb-ctrl-512words", &temp))
    {
        fhi_caps.max_ssb_ctrl_512words = temp;
    }
    if (get_device_property_u32(fh_device.dev_name, "xlnx,xran-max-ssb-data-512words", &temp))
    {
        fhi_caps.max_ssb_data_512words = temp;
    }
#endif

    // Reset everything
    xorif_reset_fhi(0);

#if defined(ENABLE_INTERRUPTS) && !defined(NO_HW)
    // Register and enable ISR
    int num = fh_device.dev->irq_num;
    if (num > 0)
    {
        int irq = (intptr_t)fh_device.dev->irq_info;
        if (irq != -1)
        {
            metal_irq_register(irq, fhi_irq_handler, &fh_device);
            metal_irq_enable(irq);
            INFO("FHI IRQ registered (%d)\n", irq);

            // Setup interrupts *** all disabled by default ***
            // Use xorif_enable_fhi_interrupts() to enable
            WRITE_REG_RAW(FHI_INTR_ENABLE_ADDR, 0);

            // Finally enable the master interrupt
            WRITE_REG(CFG_MASTER_INT_ENABLE, 1);
        }
    }
#endif
}

int xorif_fhi_get_max_cc(void)
{
    return fhi_caps.max_cc; //READ_REG(CFG_CONFIG_XRAN_MAX_CC);
}

int xorif_fhi_get_num_eth_ports(void)
{
    return fhi_caps.num_eth_ports; //READ_REG(CFG_CONFIG_NO_OF_ETH_PORTS);
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
    // Disable the component carrier
    uint32_t val = READ_REG(ORAN_CC_ENABLE);
    val &= ~(1 << cc);
    WRITE_REG(ORAN_CC_ENABLE, val);

    // Deallocate any memory associated with this component carrier
    deallocate_memory(cc);

    return XORIF_SUCCESS;
}

uint8_t xorif_fhi_get_enabled_mask(void)
{
    return (uint8_t)READ_REG(ORAN_CC_ENABLE);
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

int xorif_fhi_init_cc_symbol_pointers_ssb(
    uint16_t cc,
    uint16_t ssb_data_sym_num,
    uint16_t ssb_data_sym_start_index,
    uint16_t ssb_ctrl_sym_num)
{
    WRITE_REG_OFFSET(ORAN_CC_SSB_NUM_DATA_SYM_PER_CC, cc * 0x70, ssb_data_sym_num);
    WRITE_REG_OFFSET(ORAN_CC_SSB_DATA_SYM_START_INDEX, cc * 0x70, ssb_data_sym_start_index);
    WRITE_REG_OFFSET(ORAN_CC_SSB_NUM_SYM_PER_CC, cc * 0x70, ssb_ctrl_sym_num);
    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_dl_data_offsets(uint16_t cc,
                                      uint16_t dl_data_sym_num,
                                      uint16_t dl_data_sym_start_index,
                                      uint16_t symbol_buffer_start,
                                      uint16_t symbol_buffer_size)
{
    for (int i = 0; i < dl_data_sym_num; ++i)
    {
        WRITE_REG_OFFSET(ORAN_CC_DL_DATA_UNROLL_OFFSET, dl_data_sym_start_index * 0x4, symbol_buffer_start);
        dl_data_sym_start_index += 1;
        symbol_buffer_start += symbol_buffer_size;
    }

    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_dl_data_offsets_ssb(uint16_t cc,
                                          uint16_t ssb_data_sym_num,
                                          uint16_t ssb_data_sym_start_index,
                                          uint16_t symbol_buffer_start,
                                          uint16_t symbol_buffer_size)
{
    for (int i = 0; i < ssb_data_sym_num; ++i)
    {
        WRITE_REG_OFFSET(ORAN_CC_SSB_DATA_UNROLL_OFFSET, ssb_data_sym_start_index * 0x4, symbol_buffer_start);
        ssb_data_sym_start_index += 1;
        symbol_buffer_start += symbol_buffer_size;
    }

    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_rbs(uint16_t cc,
                          uint16_t num_rbs,
                          uint16_t numerology,
                          uint16_t sym_per_slot)
{
    WRITE_REG_OFFSET(ORAN_CC_NUMRBS, cc * 0x70, num_rbs);
    WRITE_REG_OFFSET(ORAN_CC_NUMEROLOGY, cc * 0x70, numerology);
    WRITE_REG_OFFSET(ORAN_CC_SYMPERSLOT, cc * 0x70, sym_per_slot);
    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_rbs_ssb(uint16_t cc,
                              uint16_t num_rbs,
                              uint16_t numerology,
                              uint16_t sym_per_slot)
{
    WRITE_REG_OFFSET(ORAN_CC_SSB_NUMRBS, cc * 0x70, num_rbs);
    WRITE_REG_OFFSET(ORAN_CC_SSB_NUMEROLOGY, cc * 0x70, numerology);
    WRITE_REG_OFFSET(ORAN_CC_SSB_SYMPERSLOT, cc * 0x70, sym_per_slot);
    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_ul_section_mem(uint16_t cc,
                                     uint16_t num_ctrl_per_sym_ul,
                                     uint16_t ul_ctrl_offset,
                                     uint16_t ul_ctrl_base_offset)
{
    WRITE_REG_OFFSET(ORAN_CC_NUM_CTRL_PER_SYMBOL_UL, cc * 0x70, num_ctrl_per_sym_ul);
    WRITE_REG_OFFSET(ORAN_CC_UL_CTRL_OFFSETS, cc * 0x70, ul_ctrl_offset);
    //WRITE_REG_OFFSET(ORAN_CC_UL_CTRL_UNROLLED_OFFSETS, cc * 0x70, ul_ctrl_unrolled_offset); // Not used
    WRITE_REG_OFFSET(ORAN_CC_UL_BASE_OFFSET, cc * 0x70, ul_ctrl_base_offset);
    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_dl_section_mem(uint16_t cc,
                                     uint16_t num_ctrl_per_sym_dl,
                                     uint16_t dl_ctrl_offset)
{
    WRITE_REG_OFFSET(ORAN_CC_NUM_CTRL_PER_SYMBOL_DL, cc * 0x70, num_ctrl_per_sym_dl);
    WRITE_REG_OFFSET(ORAN_CC_DL_CTRL_OFFSETS, cc * 0x70, dl_ctrl_offset);
    //WRITE_REG_OFFSET(ORAN_CC_DL_CTRL_UNROLLED_OFFSETS, cc * 0x70, dl_ctrl_unrolled_offset); // Not used
    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_section_mem_ssb(uint16_t cc,
                                      uint16_t num_ctrl_per_sym_ssb,
                                      uint16_t ssb_ctrl_offset)
{
    WRITE_REG_OFFSET(ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC, cc * 0x70, num_ctrl_per_sym_ssb);
    WRITE_REG_OFFSET(ORAN_CC_SSB_CTRL_OFFSETS, cc * 0x70, ssb_ctrl_offset);
    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_ctrl_constants(uint16_t cc,
                                     uint16_t dl_ctrl_sym_num,
                                     uint16_t num_ctrl_per_sym_dl,
                                     uint16_t ul_ctrl_sym_num,
                                     uint16_t num_ctrl_per_sym_ul)
{
    //WRITE_REG_OFFSET(ORAN_CC_MAX_SYMBOLS, cc * 0x70, 10 * (1 << numerology) * (sym_per_slot ? 12 : 14)); // Not used
    WRITE_REG_OFFSET(ORAN_CC_MODVALS_DL, cc * 0x70, dl_ctrl_sym_num * num_ctrl_per_sym_dl);
    WRITE_REG_OFFSET(ORAN_CC_MODVALS_UL, cc * 0x70, ul_ctrl_sym_num * num_ctrl_per_sym_ul);
    return XORIF_SUCCESS;
}

int xorif_fhi_init_cc_ctrl_constants_ssb(uint16_t cc,
                                         uint16_t ssb_ctrl_sym_num,
                                         uint16_t num_ctrl_per_sym_ssb)
{
    WRITE_REG_OFFSET(ORAN_CC_SSB_SECTS_X_SYMBOLS, cc * 0x70, ssb_ctrl_sym_num * num_ctrl_per_sym_ssb);
    return XORIF_SUCCESS;
}

int xorif_fhi_set_cc_dl_iq_compression(uint16_t cc,
                                       uint16_t bit_width,
                                       enum xorif_iq_comp comp_meth,
                                       uint16_t mplane)
{
    WRITE_REG_OFFSET(ORAN_CC_DL_UD_IQ_WIDTH, cc * 0x70, (bit_width & 0xF));
    WRITE_REG_OFFSET(ORAN_CC_DL_UD_COMP_METH, cc * 0x70, comp_meth);
    WRITE_REG_OFFSET(ORAN_CC_DL_MPLANE_UDCOMP_HDR_SEL, cc * 0x70, (mplane & 0x1));
    return XORIF_SUCCESS;
}

int xorif_fhi_set_cc_ul_iq_compression(uint16_t cc,
                                       uint16_t bit_width,
                                       enum xorif_iq_comp comp_meth,
                                       uint16_t mplane)
{
    WRITE_REG_OFFSET(ORAN_CC_UL_UD_IQ_WIDTH, cc * 0x70, (bit_width & 0xF));
    WRITE_REG_OFFSET(ORAN_CC_UL_UD_COMP_METH, cc * 0x70, comp_meth);
    WRITE_REG_OFFSET(ORAN_CC_UL_MPLANE_UDCOMP_HDR_SEL, cc * 0x70, (mplane & 0x1));
    return XORIF_SUCCESS;
}

int xorif_fhi_set_cc_iq_compression_ssb(uint16_t cc,
                                        uint16_t bit_width,
                                        enum xorif_iq_comp comp_meth,
                                        uint16_t mplane)
{
    WRITE_REG_OFFSET(ORAN_CC_SSB_UD_IQ_WIDTH, cc * 0x70, (bit_width & 0xF));
    WRITE_REG_OFFSET(ORAN_CC_SSB_UD_COMP_METH, cc * 0x70, comp_meth);
    WRITE_REG_OFFSET(ORAN_CC_SSB_MPLANE_UDCOMP_HDR_SEL, cc * 0x70, (mplane & 0x1));
    return XORIF_SUCCESS;
}

int xorif_fhi_configure_time_advance_offsets(uint16_t cc,
                                             uint16_t numerology,
                                             uint16_t sym_per_slot,
                                             double advance_ul,
                                             double advance_dl,
                                             double ul_bid_forward)
{
    // Symbol period in picoseconds
    double sym_period = sym_period_table[numerology] * 1e6;

    // Convert parameters from microseconds to picoseconds
    advance_dl = advance_dl * 1e6;
    advance_ul = advance_ul * 1e6;
    ul_bid_forward = ul_bid_forward * 1e6;
    double fh_decap_dly = fhi_sys_const.FH_DECAP_DLY * 1e6;
    double ul_radio_ch_dly = cc_config[cc].ul_radio_ch_dly * 1e6;

    // Compute offsets from 10 ms strobe
    double dl_offset = advance_dl + fh_decap_dly;
    double ul_offset = advance_ul + ul_radio_ch_dly;
    double ul_bidf_offset;

    // Constrain UL BIDF time
    // TODO might need to offset +/- a few cycles
    if (ul_bid_forward > ul_offset)
    {
        // UL BID FWD time can't be earlier than the UL offset
        INFO("UL BID FWD time can't be earlier than the UL offset!\n");
        ul_bidf_offset = ul_offset;
    }
    else if (ul_bid_forward < sym_period)
    {
        // UL BIDF can't be later than 1 symbol period
        INFO("UL BID FWD can't be later than 1 symbol period!\n");
        ul_bidf_offset = sym_period;
    }
    else
    {
        ul_bidf_offset = ul_bid_forward;
    }

    // Downlink settings
    WRITE_REG_OFFSET(ORAN_CC_DL_SETUP_D_CYCLES, cc * 0x70, (sym_period - fh_decap_dly) / XRAN_TIMER_CLK);
    WRITE_REG_OFFSET(ORAN_CC_DL_SETUP_C_ABS_SYMBOL, cc * 0x70, ceil(dl_offset / sym_period));
    WRITE_REG_OFFSET(ORAN_CC_DL_SETUP_C_CYCLES, cc * 0x70, (sym_period - fmod(dl_offset, sym_period)) / XRAN_TIMER_CLK);

    // Uplink settings
    WRITE_REG_OFFSET(ORAN_CC_UL_SETUP_C_ABS_SYMBOL, cc * 0x70, ceil(ul_offset / sym_period));
    WRITE_REG_OFFSET(ORAN_CC_UL_SETUP_C_CYCLES, cc * 0x70, (sym_period - fmod(ul_offset, sym_period)) / XRAN_TIMER_CLK);

    // Uplink BIDF settings
    WRITE_REG_OFFSET(ORAN_CC_UL_BIDF_C_ABS_SYMBOL, cc * 0x70, ceil(ul_bidf_offset /  sym_period));
    WRITE_REG_OFFSET(ORAN_CC_UL_BIDF_C_CYCLES, cc * 0x70, (sym_period - fmod(ul_bidf_offset, sym_period)) / XRAN_TIMER_CLK);

    return XORIF_SUCCESS;
}

int xorif_fhi_configure_time_advance_offsets_ssb(uint16_t cc,
                                                 uint16_t numerology,
                                                 uint16_t sym_per_slot,
                                                 double advance_dl)
{
    // Symbol period in picoseconds
    double sym_period = sym_period_table[numerology] * 1e6;

    // Convert parameters from microseconds to picoseconds
    advance_dl = advance_dl * 1e6;
    double fh_decap_dly = fhi_sys_const.FH_DECAP_DLY * 1e6;

    // Compute offsets from 10 ms strobe
    double dl_offset = advance_dl + fh_decap_dly;

    // Downlink settings
    WRITE_REG_OFFSET(ORAN_CC_SSB_SETUP_D_CYCLES, cc * 0x70, (sym_period - fh_decap_dly) / XRAN_TIMER_CLK);
    WRITE_REG_OFFSET(ORAN_CC_SSB_SETUP_C_ABS_SYMBOL, cc * 0x70, ceil(dl_offset / sym_period));
    WRITE_REG_OFFSET(ORAN_CC_SSB_SETUP_C_CYCLES, cc * 0x70, (sym_period - fmod(dl_offset, sym_period)) / XRAN_TIMER_CLK);

    return XORIF_SUCCESS;
}

int xorif_fhi_configure_cc(uint16_t cc)
{
    // Set up pointer to configuration data
    const struct xorif_cc_config *ptr = &cc_config[cc];

    // Calculate required number of symbols
    uint16_t ul_ctrl_sym_num = calc_sym_num(ptr->numerology, ptr->extended_cp, ptr->delay_comp_cp_ul + ptr->advance_ul + ptr->ul_radio_ch_dly);
    uint16_t dl_ctrl_sym_num = calc_sym_num(ptr->numerology, ptr->extended_cp, ptr->delay_comp_cp_dl + ptr->advance_dl + fhi_sys_const.FH_DECAP_DLY);
    uint16_t dl_data_sym_num = calc_sym_num(ptr->numerology, ptr->extended_cp, ptr->delay_comp_up + fhi_sys_const.FH_DECAP_DLY);
    uint16_t ssb_ctrl_sym_num = calc_sym_num(ptr->numerology_ssb, ptr->extended_cp_ssb, ptr->delay_comp_cp_dl + ptr->advance_dl + fhi_sys_const.FH_DECAP_DLY);
    uint16_t ssb_data_sym_num = calc_sym_num(ptr->numerology_ssb, ptr->extended_cp_ssb, ptr->delay_comp_up + fhi_sys_const.FH_DECAP_DLY);

    // Check number ctrl symbols
    if ((ul_ctrl_sym_num > fhi_caps.max_ctrl_symbols) ||
        (dl_ctrl_sym_num > fhi_caps.max_ctrl_symbols) ||
        (ssb_ctrl_sym_num > fhi_caps.max_ctrl_symbols))
    {
        PERROR("Configuration exceeds max control symbols\n");
        return XORIF_MAX_CTRL_SYM_EXCEEDED;
    }

    // Check number data symbols
    if ((dl_data_sym_num > fhi_caps.max_data_symbols) ||
        (ssb_data_sym_num > fhi_caps.max_data_symbols))
    {
        PERROR("Configuration exceeds max data symbols\n");
        return XORIF_MAX_DATA_SYM_EXCEEDED;
    }

    // Calculate downlink data buffer size (per symbol)
    uint16_t dl_data_buff_size = calc_data_buff_size(ptr->num_rbs,
                                                     ptr->iq_comp_meth_dl,
                                                     ptr->iq_comp_width_dl,
                                                     ptr->num_sect_per_sym,
                                                     ptr->num_frames_per_sym);

    // Calculate SSB data buffer size (per symbol)
    uint16_t ssb_data_buff_size = calc_data_buff_size(ptr->num_rbs_ssb,
                                                      ptr->iq_comp_meth_ssb,
                                                      ptr->iq_comp_width_ssb,
                                                      ptr->num_sect_per_sym_ssb,
                                                      ptr->num_frames_per_sym_ssb);

    // Deallocate any memory associated with this component carrier
    deallocate_memory(cc);

    // Get new memory allocations
    int ul_ctrl_offset = alloc_block(ul_ctrl_memory, (ul_ctrl_sym_num * ptr->num_ctrl_per_sym_ul), cc);
    int ul_ctrl_base_offset = alloc_block(ul_ctrl_base_memory, ptr->num_rbs, cc);
    int dl_ctrl_offset = alloc_block(dl_ctrl_memory, (dl_ctrl_sym_num * ptr->num_ctrl_per_sym_dl), cc);
    int dl_data_ptrs_offset = alloc_block(dl_data_ptrs_memory, dl_data_sym_num, cc);
    int dl_data_buff_offset = alloc_block(dl_data_buff_memory, (dl_data_sym_num * dl_data_buff_size), cc);
    int ssb_ctrl_offset = alloc_block(ssb_ctrl_memory, (ssb_ctrl_sym_num * ptr->num_ctrl_per_sym_ssb), cc);
    int ssb_data_ptrs_offset = alloc_block(ssb_data_ptrs_memory, ssb_data_sym_num, cc);
    int ssb_data_buff_offset = alloc_block(ssb_data_buff_memory, (ssb_data_sym_num * ssb_data_buff_size), cc);

    // Check for memory allocation errors...
    int error = 0;
    if (ul_ctrl_offset == -1)
    {
        PERROR("Configuration exceeds available buffer space (uplink ctrl section memory)\n");
        error = 1;
    }

    if (ul_ctrl_base_offset == -1)
    {
        PERROR("Configuration exceeds available subcarriers (uplink ctrl base)\n");
        error = 1;
    }

    if (dl_ctrl_offset == -1)
    {
        PERROR("Configuration exceeds available buffer space (downlink ctrl section memory)\n");
        error = 1;
    }

    if (ssb_ctrl_offset  == -1)
    {
        PERROR("Configuration exceeds available buffer space (SSB ctrl section memory)\n");
        error = 1;
    }

    if (dl_data_ptrs_offset == -1)
    {
        PERROR("Configuration exceeds allocated buffer space (downlink data pointers)\n");
        error = 1;
    }

    if (dl_data_buff_offset == -1)
    {
        PERROR("Configuration exceeds allocated buffer space (downlink data buffer)\n");
        error = 1;
    }

    if (ssb_data_ptrs_offset == -1)
    {
        PERROR("Configuration exceeds allocated buffer space (SSB data pointers)\n");
        error = 1;
    }

    if (ssb_data_buff_offset == -1)
    {
        PERROR("Configuration exceeds allocated buffer space (SSB data buffer)\n");
        error = 1;
    }

    if (error)
    {
        // Deallocate any memory associated with this component carrier
        deallocate_memory(cc);

        return XORIF_BUFFER_SPACE_EXCEEDED;
    }

    // Everything fits!
    INFO("Configuration valid\n");

    // Program the h/w
    xorif_fhi_init_cc_rbs(cc, ptr->num_rbs, ptr->numerology, ptr->extended_cp);

    xorif_fhi_init_cc_rbs_ssb(cc, ptr->num_rbs_ssb, ptr->numerology_ssb, ptr->extended_cp_ssb);

    xorif_fhi_init_cc_symbol_pointers(cc, dl_data_sym_num, dl_data_ptrs_offset, dl_ctrl_sym_num, ul_ctrl_sym_num);

    xorif_fhi_init_cc_symbol_pointers_ssb(cc, ssb_data_sym_num, ssb_data_ptrs_offset, ssb_ctrl_sym_num);

    xorif_fhi_set_cc_dl_iq_compression(cc, ptr->iq_comp_width_dl, ptr->iq_comp_meth_dl, ptr->iq_comp_mplane_dl);

    xorif_fhi_set_cc_ul_iq_compression(cc, ptr->iq_comp_width_ul, ptr->iq_comp_meth_ul, ptr->iq_comp_mplane_ul);

    xorif_fhi_set_cc_iq_compression_ssb(cc, ptr->iq_comp_width_ssb, ptr->iq_comp_meth_ssb, ptr->iq_comp_mplane_ssb);

    xorif_fhi_init_cc_dl_section_mem(cc, ptr->num_ctrl_per_sym_dl, dl_ctrl_offset);

    xorif_fhi_init_cc_ul_section_mem(cc, ptr->num_ctrl_per_sym_ul, ul_ctrl_offset, ul_ctrl_base_offset);

    xorif_fhi_init_cc_section_mem_ssb(cc, ptr->num_ctrl_per_sym_ssb, ssb_ctrl_offset);

    xorif_fhi_init_cc_dl_data_offsets(cc, dl_data_sym_num, dl_data_ptrs_offset, dl_data_buff_offset, dl_data_buff_size);

    xorif_fhi_init_cc_dl_data_offsets_ssb(cc, ssb_data_sym_num, ssb_data_ptrs_offset, ssb_data_buff_offset, ssb_data_buff_size);

    xorif_fhi_init_cc_ctrl_constants(cc, dl_ctrl_sym_num, ptr->num_ctrl_per_sym_dl, ul_ctrl_sym_num, ptr->num_ctrl_per_sym_ul);

    xorif_fhi_init_cc_ctrl_constants_ssb(cc, ssb_ctrl_sym_num, ptr->num_ctrl_per_sym_ssb);

    xorif_fhi_configure_time_advance_offsets(cc, ptr->numerology, ptr->extended_cp, ptr->advance_ul, ptr->advance_dl, ptr->ul_bid_forward);

    xorif_fhi_configure_time_advance_offsets_ssb(cc, ptr->numerology_ssb, ptr->extended_cp_ssb, ptr->advance_dl);

    // Perform "reload" on the component carrier
    xorif_fhi_cc_reload(cc);

#ifdef AUTO_ENABLE
    // Enable component carrier
    xorif_fhi_cc_enable(cc);
#endif

    return XORIF_SUCCESS;
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
static uint16_t calc_sym_num(uint16_t numerology, uint16_t extended_cp, double time)
{
    // Compute number of symbols per second based on numerology
    // Note, 10 sub-frames per frame, 100 frames per second
    int num = 100 * 10 * (1 << numerology) * (extended_cp ? 12 : 14);

    // Symbol period in microseconds = 1e6 / num
    double sym_period = 1e6 / num;

    // Number of symbols required rounded-up and converted to integer
    return (uint16_t)(ceil(time / sym_period));
}

/**
 * @brief Calculate data buffer size based on number of RBs and compression scheme.
 * @param[in] num_rbs Number of RBs
 * @param[in] comp_mode IQ compression mode
 * @param[in] comp_width compression width
 * @param[in] num_sect Number of sections
 * @param[in] num_frames Number of Ethernet frames
 * @returns
 *      - Downlink data buffer size (in 8-byte words)
 */
static uint16_t calc_data_buff_size(uint16_t num_rbs,
                                    enum xorif_iq_comp comp_mode,
                                    uint16_t comp_width,
                                    uint16_t num_sect,
                                    uint16_t num_frames)
{
    // For calculation purposes, a width of 0 means 16 bits
    comp_width = (comp_width == 0) ? 16 : comp_width;

    // Calculate size of RB in bytes (depends on compression mode)
    uint16_t size;
    switch (comp_mode)
    {
    case IQ_COMP_BLOCK_FP:
        // (n bits I + n bits Q) per RE and round-up, plus 1 byte for exponent
        size = CEIL_DIV((comp_width * 2 * RE_PER_RB), 8) + 1;
        break;

    case IQ_COMP_MODULATION:
        // (n bits I + n bits Q) per RE and round-up
        size = CEIL_DIV((comp_width * 2 * RE_PER_RB), 8);
        break;

    case IQ_COMP_NONE:
    default:
        // (16 bits I + 16 bits Q) per RE and round-up
        size = CEIL_DIV((16 * 2 * RE_PER_RB), 8);
        break;
    }

    // Multiply by number of RBs
    size *= num_rbs;

    // Add 4 bytes per section header
    // TODO this could be 6 or 8 for dynamic compression cases
    size += (4 * num_sect);

    // Add 11 bytes per Ethernet frame (4 + 7 for worse-case rounding to 8-byte word)
    size += (11 * num_frames);

    // Divide by 8 (since 8 byte words) and round-up
    size = CEIL_DIV(size, 8);

    return size;
}

/**
 * @brief Deallocate memory assigned to specific component carrier.
 * @param[in] cc Component carrier
 */
static void deallocate_memory(int cc)
{
    // Deallocate memory associated with this component carrier
    dealloc_block(ul_ctrl_memory, cc);
    dealloc_block(ul_ctrl_base_memory, cc);
    dealloc_block(dl_ctrl_memory, cc);
    dealloc_block(dl_data_ptrs_memory, cc);
    dealloc_block(dl_data_buff_memory, cc);
    dealloc_block(ssb_ctrl_memory, cc);
    dealloc_block(ssb_data_ptrs_memory, cc);
    dealloc_block(ssb_data_buff_memory, cc);
}

#ifdef NO_HW
/**
 * @brief Initialize fake register bank.
 * @note For test only
 */
static void init_fake_reg_bank(void)
{
    INFO("Initializing fake register bank\n");

    // Clear memory
    memset(fake_reg_bank, 0, sizeof(fake_reg_bank));

    // Disable debug tracing of following WRITE_REG's, since they're all fake
    int temp = xorif_trace;
    xorif_trace = 0;

    // Set configuration registers to suitable values
    WRITE_REG(CFG_CONFIG_NO_OF_FRAM_ANTS, 8);
    WRITE_REG(CFG_CONFIG_NO_OF_DEFM_ANTS, 16);
    WRITE_REG(CFG_CONFIG_NO_OF_ETH_PORTS, 4);
    WRITE_REG(CFG_CONFIG_XRAN_MAX_CC, 8);
    WRITE_REG(CFG_CONFIG_XRAN_MAX_DL_SYMBOLS, 16);
    WRITE_REG(CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX, 8000);
    WRITE_REG(CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX, 8000);
    WRITE_REG(CFG_CONFIG_XRAN_MAX_SCS, 6600);
    WRITE_REG(CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS, 16);
    WRITE_REG(CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS, 4);
    WRITE_REG(CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS, 4);
    WRITE_REG(CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS, 16);
    WRITE_REG(CFG_CONFIG_XRAN_TIMER_CLK_PS, 5000); // 2500, 2560, 5000
    WRITE_REG(CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM, 1);
    WRITE_REG(CFG_CONFIG_XRAN_PRACH_C_PORTS, 1);
    WRITE_REG(CFG_CONFIG_LIMIT_DU_W, 4);
    WRITE_REG(CFG_CONFIG_LIMIT_BS_W, 6);
    WRITE_REG(CFG_CONFIG_LIMIT_CC_W, 3);
    WRITE_REG(CFG_CONFIG_LIMIT_RU_I_W, 8);
    WRITE_REG(CFG_CONFIG_LIMIT_RU_O_W, 5);
    WRITE_REG(CFG_CONFIG_MAP_TABLE_W, 8); // Use 0, 8, 11 ?
    WRITE_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_NOCOMP, 1);
    WRITE_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP, 1);
    WRITE_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP, 1);
    WRITE_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_WIDTHS, 0x5200); // 9, 12, 14
    WRITE_REG(CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODC_WIDTHS, 0x3E); // 1, 2, 3, 4, 5
    WRITE_REG(CFG_CONFIG_XRAN_COMP_IN_CORE_NOCOMP, 1);
    WRITE_REG(CFG_CONFIG_XRAN_COMP_IN_CORE_BFP, 1);
    WRITE_REG(CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_WIDTHS, 0x5200); // 9, 12, 14

    // Restore debug tracing level
    xorif_trace = temp;
}
#endif

/** @} */