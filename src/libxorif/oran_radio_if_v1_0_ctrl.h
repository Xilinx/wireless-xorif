// SPDX-License-Identifier: GPL-2.0 
/*-----------------------------------------------------------------------------
* C Header bank BASE definitions
*------------------------------------------------------------------------------
*/
#define ORAN_RADIO_IF_V1_0_CFG_BASE_ADDR 0x0 /* 0 */
#define ORAN_RADIO_IF_V1_0_FRAM_BASE_ADDR 0x2000 /* 8192 */
#define ORAN_RADIO_IF_V1_0_DEFM_BASE_ADDR 0x6000 /* 24576 */
#define ORAN_RADIO_IF_V1_0_ETH_BASE_ADDR 0xa000 /* 40960 */
#define ORAN_RADIO_IF_V1_0_STATS_BASE_ADDR 0xc000 /* 49152 */
#define ORAN_RADIO_IF_V1_0_ORAN_BASE_ADDR 0xe000 /* 57344 */

/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v1_0_cfg 
* with prefix cfg_ @ address 0x0
*------------------------------------------------------------------------------
*/
/* Type = roInt  */
#define CFG_MAJOR_REVISION_ADDR 0x0 /* 0 */
#define CFG_MAJOR_REVISION_MASK 0xff000000 /* 4278190080 */
#define CFG_MAJOR_REVISION_OFFSET 0x18 /* 24 */
#define CFG_MAJOR_REVISION_WIDTH 0x8 /* 8 */
#define CFG_MAJOR_REVISION_DEFAULT 0x1 /* 1 */

/* Type = roInt  */
#define CFG_MINOR_REVISION_ADDR 0x0 /* 0 */
#define CFG_MINOR_REVISION_MASK 0xff0000 /* 16711680 */
#define CFG_MINOR_REVISION_OFFSET 0x10 /* 16 */
#define CFG_MINOR_REVISION_WIDTH 0x8 /* 8 */
#define CFG_MINOR_REVISION_DEFAULT 0x0 /* 0 */

/* Type = roInt  */
#define CFG_VERSION_REVISION_ADDR 0x0 /* 0 */
#define CFG_VERSION_REVISION_MASK 0xff00 /* 65280 */
#define CFG_VERSION_REVISION_OFFSET 0x8 /* 8 */
#define CFG_VERSION_REVISION_WIDTH 0x8 /* 8 */
#define CFG_VERSION_REVISION_DEFAULT 0x0 /* 0 */

/* Type = roInt  */
#define CFG_INTERNAL_REVISION_ADDR 0x4 /* 4 */
#define CFG_INTERNAL_REVISION_MASK 0xffffffff /* 4294967295 */
#define CFG_INTERNAL_REVISION_OFFSET 0x0 /* 0 */
#define CFG_INTERNAL_REVISION_WIDTH 0x20 /* 32 */
#define CFG_INTERNAL_REVISION_DEFAULT 0x12345678 /* 305419896 */

/* Type = rw  */
#define CFG_TIMEOUT_VALUE_ADDR 0x8 /* 8 */
#define CFG_TIMEOUT_VALUE_MASK 0xfff /* 4095 */
#define CFG_TIMEOUT_VALUE_OFFSET 0x0 /* 0 */
#define CFG_TIMEOUT_VALUE_WIDTH 0xc /* 12 */
#define CFG_TIMEOUT_VALUE_DEFAULT 0x80 /* 128 */

/* Type = rw  */
#define CFG_USER_RW_OUT_ADDR 0xc /* 12 */
#define CFG_USER_RW_OUT_MASK 0xff /* 255 */
#define CFG_USER_RW_OUT_OFFSET 0x0 /* 0 */
#define CFG_USER_RW_OUT_WIDTH 0x8 /* 8 */
#define CFG_USER_RW_OUT_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_USER_RO_IN_ADDR 0xc /* 12 */
#define CFG_USER_RO_IN_MASK 0xff0000 /* 16711680 */
#define CFG_USER_RO_IN_OFFSET 0x10 /* 16 */
#define CFG_USER_RO_IN_WIDTH 0x8 /* 8 */
#define CFG_USER_RO_IN_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define CFG_MASTER_INT_ENABLE_ADDR 0x10 /* 16 */
#define CFG_MASTER_INT_ENABLE_MASK 0x1 /* 1 */
#define CFG_MASTER_INT_ENABLE_OFFSET 0x0 /* 0 */
#define CFG_MASTER_INT_ENABLE_WIDTH 0x1 /* 1 */
#define CFG_MASTER_INT_ENABLE_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define CFG_DEFM_INT_ENA_INFIFO_OF_ADDR 0x14 /* 20 */
#define CFG_DEFM_INT_ENA_INFIFO_OF_MASK 0x100 /* 256 */
#define CFG_DEFM_INT_ENA_INFIFO_OF_OFFSET 0x8 /* 8 */
#define CFG_DEFM_INT_ENA_INFIFO_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ENA_INFIFO_OF_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define CFG_DEFM_INT_ENA_INFIFO_UF_ADDR 0x14 /* 20 */
#define CFG_DEFM_INT_ENA_INFIFO_UF_MASK 0x200 /* 512 */
#define CFG_DEFM_INT_ENA_INFIFO_UF_OFFSET 0x9 /* 9 */
#define CFG_DEFM_INT_ENA_INFIFO_UF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ENA_INFIFO_UF_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF_ADDR 0x14 /* 20 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF_MASK 0x400 /* 1024 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF_OFFSET 0xa /* 10 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF_ADDR 0x14 /* 20 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF_MASK 0x800 /* 2048 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF_OFFSET 0xb /* 11 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define CFG_FRAM_INT_ENA_OUTFIFO_OF_ADDR 0x14 /* 20 */
#define CFG_FRAM_INT_ENA_OUTFIFO_OF_MASK 0x1000 /* 4096 */
#define CFG_FRAM_INT_ENA_OUTFIFO_OF_OFFSET 0xc /* 12 */
#define CFG_FRAM_INT_ENA_OUTFIFO_OF_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_ENA_OUTFIFO_OF_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define CFG_FRAM_INT_ENA_OUTFIFO_UF_ADDR 0x14 /* 20 */
#define CFG_FRAM_INT_ENA_OUTFIFO_UF_MASK 0x2000 /* 8192 */
#define CFG_FRAM_INT_ENA_OUTFIFO_UF_OFFSET 0xd /* 13 */
#define CFG_FRAM_INT_ENA_OUTFIFO_UF_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_ENA_OUTFIFO_UF_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW_ADDR 0x14 /* 20 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW_MASK 0x4000 /* 16384 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW_OFFSET 0xe /* 14 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND_ADDR 0x14 /* 20 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND_MASK 0x8000 /* 32768 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND_OFFSET 0xf /* 15 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define CFG_AXI_TIMEOUT_ENABLE_ADDR 0x14 /* 20 */
#define CFG_AXI_TIMEOUT_ENABLE_MASK 0x80000000 /* 2147483648 */
#define CFG_AXI_TIMEOUT_ENABLE_OFFSET 0x1f /* 31 */
#define CFG_AXI_TIMEOUT_ENABLE_WIDTH 0x1 /* 1 */
#define CFG_AXI_TIMEOUT_ENABLE_DEFAULT 0x1 /* 1 */

/* Type = roSig  */
#define CFG_FRAM_RESET_STATUS_ADDR 0x18 /* 24 */
#define CFG_FRAM_RESET_STATUS_MASK 0x1 /* 1 */
#define CFG_FRAM_RESET_STATUS_OFFSET 0x0 /* 0 */
#define CFG_FRAM_RESET_STATUS_WIDTH 0x1 /* 1 */
#define CFG_FRAM_RESET_STATUS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_DEFM_RESET_STATUS_ADDR 0x18 /* 24 */
#define CFG_DEFM_RESET_STATUS_MASK 0x2 /* 2 */
#define CFG_DEFM_RESET_STATUS_OFFSET 0x1 /* 1 */
#define CFG_DEFM_RESET_STATUS_WIDTH 0x1 /* 1 */
#define CFG_DEFM_RESET_STATUS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_DEFM_INT_INFIFO_OF_ADDR 0x18 /* 24 */
#define CFG_DEFM_INT_INFIFO_OF_MASK 0x100 /* 256 */
#define CFG_DEFM_INT_INFIFO_OF_OFFSET 0x8 /* 8 */
#define CFG_DEFM_INT_INFIFO_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_INFIFO_OF_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_DEFM_INT_INFIFO_UF_ADDR 0x18 /* 24 */
#define CFG_DEFM_INT_INFIFO_UF_MASK 0x200 /* 512 */
#define CFG_DEFM_INT_INFIFO_UF_OFFSET 0x9 /* 9 */
#define CFG_DEFM_INT_INFIFO_UF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_INFIFO_UF_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_ADDR 0x18 /* 24 */
#define CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_MASK 0x400 /* 1024 */
#define CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_OFFSET 0xa /* 10 */
#define CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_DEFM_INT_ETH_PIPE_TABLE_OF_ADDR 0x18 /* 24 */
#define CFG_DEFM_INT_ETH_PIPE_TABLE_OF_MASK 0x800 /* 2048 */
#define CFG_DEFM_INT_ETH_PIPE_TABLE_OF_OFFSET 0xb /* 11 */
#define CFG_DEFM_INT_ETH_PIPE_TABLE_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ETH_PIPE_TABLE_OF_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_FRAM_INT_OUTFIFO_OF_ADDR 0x18 /* 24 */
#define CFG_FRAM_INT_OUTFIFO_OF_MASK 0x1000 /* 4096 */
#define CFG_FRAM_INT_OUTFIFO_OF_OFFSET 0xc /* 12 */
#define CFG_FRAM_INT_OUTFIFO_OF_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_OUTFIFO_OF_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_FRAM_INT_OUTFIFO_UF_ADDR 0x18 /* 24 */
#define CFG_FRAM_INT_OUTFIFO_UF_MASK 0x2000 /* 8192 */
#define CFG_FRAM_INT_OUTFIFO_UF_OFFSET 0xd /* 13 */
#define CFG_FRAM_INT_OUTFIFO_UF_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_OUTFIFO_UF_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_ADDR 0x18 /* 24 */
#define CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_MASK 0x4000 /* 16384 */
#define CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_OFFSET 0xe /* 14 */
#define CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_ADDR 0x18 /* 24 */
#define CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_MASK 0x8000 /* 32768 */
#define CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_OFFSET 0xf /* 15 */
#define CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_AXI_TIMEOUT_STATUS_ADDR 0x18 /* 24 */
#define CFG_AXI_TIMEOUT_STATUS_MASK 0x80000000 /* 2147483648 */
#define CFG_AXI_TIMEOUT_STATUS_OFFSET 0x1f /* 31 */
#define CFG_AXI_TIMEOUT_STATUS_WIDTH 0x1 /* 1 */
#define CFG_AXI_TIMEOUT_STATUS_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define CFG_INTERRUPT_STATUS_SAMPLE_ADDR 0x1c /* 28 */
#define CFG_INTERRUPT_STATUS_SAMPLE_MASK 0x1 /* 1 */
#define CFG_INTERRUPT_STATUS_SAMPLE_OFFSET 0x0 /* 0 */
#define CFG_INTERRUPT_STATUS_SAMPLE_WIDTH 0x1 /* 1 */
#define CFG_INTERRUPT_STATUS_SAMPLE_DEFAULT 0x1 /* 1 */

/* Type = roSig  */
#define CFG_CONFIG_NO_OF_FRAM_ANTS_ADDR 0x20 /* 32 */
#define CFG_CONFIG_NO_OF_FRAM_ANTS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_NO_OF_FRAM_ANTS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_NO_OF_FRAM_ANTS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_NO_OF_FRAM_ANTS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_NO_OF_DEFM_ANTS_ADDR 0x20 /* 32 */
#define CFG_CONFIG_NO_OF_DEFM_ANTS_MASK 0xffff0000 /* 4294901760 */
#define CFG_CONFIG_NO_OF_DEFM_ANTS_OFFSET 0x10 /* 16 */
#define CFG_CONFIG_NO_OF_DEFM_ANTS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_NO_OF_DEFM_ANTS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_NO_OF_ETH_PORTS_ADDR 0x24 /* 36 */
#define CFG_CONFIG_NO_OF_ETH_PORTS_MASK 0x3ff /* 1023 */
#define CFG_CONFIG_NO_OF_ETH_PORTS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_NO_OF_ETH_PORTS_WIDTH 0xa /* 10 */
#define CFG_CONFIG_NO_OF_ETH_PORTS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_SUPPORT_MODE_ADDR 0x28 /* 40 */
#define CFG_CONFIG_XRAN_SUPPORT_MODE_MASK 0xf /* 15 */
#define CFG_CONFIG_XRAN_SUPPORT_MODE_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_SUPPORT_MODE_WIDTH 0x4 /* 4 */
#define CFG_CONFIG_XRAN_SUPPORT_MODE_DEFAULT 0x5 /* 5 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_MAX_CC_ADDR 0x2c /* 44 */
#define CFG_CONFIG_XRAN_MAX_CC_MASK 0x1f /* 31 */
#define CFG_CONFIG_XRAN_MAX_CC_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_CC_WIDTH 0x5 /* 5 */
#define CFG_CONFIG_XRAN_MAX_CC_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_ADDR 0x30 /* 48 */
#define CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_MASK 0x1f /* 31 */
#define CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_WIDTH 0x5 /* 5 */
#define CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_MAX_NUMEROLOGY_ADDR 0x34 /* 52 */
#define CFG_CONFIG_XRAN_MAX_NUMEROLOGY_MASK 0x7 /* 7 */
#define CFG_CONFIG_XRAN_MAX_NUMEROLOGY_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_NUMEROLOGY_WIDTH 0x3 /* 3 */
#define CFG_CONFIG_XRAN_MAX_NUMEROLOGY_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_MIN_NUMEROLOGY_ADDR 0x38 /* 56 */
#define CFG_CONFIG_XRAN_MIN_NUMEROLOGY_MASK 0x7 /* 7 */
#define CFG_CONFIG_XRAN_MIN_NUMEROLOGY_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MIN_NUMEROLOGY_WIDTH 0x3 /* 3 */
#define CFG_CONFIG_XRAN_MIN_NUMEROLOGY_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX_ADDR 0x3c /* 60 */
#define CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_FRAM_AUTO_START_ADDR 0x40 /* 64 */
#define CFG_CONFIG_XRAN_FRAM_AUTO_START_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_FRAM_AUTO_START_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_FRAM_AUTO_START_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_FRAM_AUTO_START_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX_ADDR 0x44 /* 68 */
#define CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_MAX_SCS_ADDR 0x48 /* 72 */
#define CFG_CONFIG_XRAN_MAX_SCS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_SCS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_SCS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_SCS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS_ADDR 0x4c /* 76 */
#define CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS_ADDR 0x50 /* 80 */
#define CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS_ADDR 0x54 /* 84 */
#define CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS_ADDR 0x58 /* 88 */
#define CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_TIMER_CLK_PS_ADDR 0x5c /* 92 */
#define CFG_CONFIG_XRAN_TIMER_CLK_PS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_TIMER_CLK_PS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_TIMER_CLK_PS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_TIMER_CLK_PS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_MAX_DL_SECT_PER_SYMBOL_ADDR 0x60 /* 96 */
#define CFG_CONFIG_XRAN_MAX_DL_SECT_PER_SYMBOL_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_DL_SECT_PER_SYMBOL_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_DL_SECT_PER_SYMBOL_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_DL_SECT_PER_SYMBOL_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_MAX_UL_SECT_PER_SYMBOL_ADDR 0x64 /* 100 */
#define CFG_CONFIG_XRAN_MAX_UL_SECT_PER_SYMBOL_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_UL_SECT_PER_SYMBOL_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_UL_SECT_PER_SYMBOL_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_UL_SECT_PER_SYMBOL_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM_ADDR 0x68 /* 104 */
#define CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH_ADDR 0x6c /* 108 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS_ADDR 0x70 /* 112 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define CFG_CONFIG_XRAN_PRACH_C_PORTS_ADDR 0x74 /* 116 */
#define CFG_CONFIG_XRAN_PRACH_C_PORTS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_PRACH_C_PORTS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_PRACH_C_PORTS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_PRACH_C_PORTS_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v1_0_fram 
* with prefix fram_ @ address 0x2000
*------------------------------------------------------------------------------
*/
/* Type = rwpdef  */
#define FRAM_DISABLE_ADDR 0x2000 /* 8192 */
#define FRAM_DISABLE_MASK 0x1 /* 1 */
#define FRAM_DISABLE_OFFSET 0x0 /* 0 */
#define FRAM_DISABLE_WIDTH 0x1 /* 1 */
#define FRAM_DISABLE_DEFAULT 0x1 /* 1 */

/* Type = roSig  */
#define FRAM_READY_ADDR 0x2000 /* 8192 */
#define FRAM_READY_MASK 0x2 /* 2 */
#define FRAM_READY_OFFSET 0x1 /* 1 */
#define FRAM_READY_WIDTH 0x1 /* 1 */
#define FRAM_READY_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define FRAM_FIFO_FULL_INDICATOR_ADDR 0x2004 /* 8196 */
#define FRAM_FIFO_FULL_INDICATOR_MASK 0xffffffff /* 4294967295 */
#define FRAM_FIFO_FULL_INDICATOR_OFFSET 0x0 /* 0 */
#define FRAM_FIFO_FULL_INDICATOR_WIDTH 0x20 /* 32 */
#define FRAM_FIFO_FULL_INDICATOR_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define FRAM_XRAN_RU_PORT_ID_FIELD_WIDTH_ADDR 0x2080 /* 8320 */
#define FRAM_XRAN_RU_PORT_ID_FIELD_WIDTH_MASK 0xf /* 15 */
#define FRAM_XRAN_RU_PORT_ID_FIELD_WIDTH_OFFSET 0x0 /* 0 */
#define FRAM_XRAN_RU_PORT_ID_FIELD_WIDTH_WIDTH 0x4 /* 4 */
#define FRAM_XRAN_RU_PORT_ID_FIELD_WIDTH_DEFAULT 0x4 /* 4 */

/* Type = rw  */
#define FRAM_XRAN_CC_ID_FIELD_WIDTH_ADDR 0x2080 /* 8320 */
#define FRAM_XRAN_CC_ID_FIELD_WIDTH_MASK 0xf0 /* 240 */
#define FRAM_XRAN_CC_ID_FIELD_WIDTH_OFFSET 0x4 /* 4 */
#define FRAM_XRAN_CC_ID_FIELD_WIDTH_WIDTH 0x4 /* 4 */
#define FRAM_XRAN_CC_ID_FIELD_WIDTH_DEFAULT 0x4 /* 4 */

/* Type = rw  */
#define FRAM_XRAN_BANDSECTOR_FIELD_WIDTH_ADDR 0x2080 /* 8320 */
#define FRAM_XRAN_BANDSECTOR_FIELD_WIDTH_MASK 0xf00 /* 3840 */
#define FRAM_XRAN_BANDSECTOR_FIELD_WIDTH_OFFSET 0x8 /* 8 */
#define FRAM_XRAN_BANDSECTOR_FIELD_WIDTH_WIDTH 0x4 /* 4 */
#define FRAM_XRAN_BANDSECTOR_FIELD_WIDTH_DEFAULT 0x4 /* 4 */

/* Type = rw  */
#define FRAM_PROTOCOL_DEFINITION_ADDR 0x2200 /* 8704 */
#define FRAM_PROTOCOL_DEFINITION_MASK 0xf /* 15 */
#define FRAM_PROTOCOL_DEFINITION_OFFSET 0x0 /* 0 */
#define FRAM_PROTOCOL_DEFINITION_WIDTH 0x4 /* 4 */
#define FRAM_PROTOCOL_DEFINITION_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define FRAM_GEN_VLAN_TAG_ADDR 0x2200 /* 8704 */
#define FRAM_GEN_VLAN_TAG_MASK 0x10 /* 16 */
#define FRAM_GEN_VLAN_TAG_OFFSET 0x4 /* 4 */
#define FRAM_GEN_VLAN_TAG_WIDTH 0x1 /* 1 */
#define FRAM_GEN_VLAN_TAG_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define FRAM_SEL_IPV_ADDRESS_TYPE_ADDR 0x2200 /* 8704 */
#define FRAM_SEL_IPV_ADDRESS_TYPE_MASK 0x60 /* 96 */
#define FRAM_SEL_IPV_ADDRESS_TYPE_OFFSET 0x5 /* 5 */
#define FRAM_SEL_IPV_ADDRESS_TYPE_WIDTH 0x2 /* 2 */
#define FRAM_SEL_IPV_ADDRESS_TYPE_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v1_0_defm 
* with prefix defm_ @ address 0x6000
*------------------------------------------------------------------------------
*/
/* Type = rw  */
#define DEFM_RESTART_ADDR 0x6000 /* 24576 */
#define DEFM_RESTART_MASK 0x1 /* 1 */
#define DEFM_RESTART_OFFSET 0x0 /* 0 */
#define DEFM_RESTART_WIDTH 0x1 /* 1 */
#define DEFM_RESTART_DEFAULT 0x0 /* 0 */

/* Type = roSig  */
#define DEFM_READY_ADDR 0x6000 /* 24576 */
#define DEFM_READY_MASK 0x2 /* 2 */
#define DEFM_READY_OFFSET 0x1 /* 1 */
#define DEFM_READY_WIDTH 0x1 /* 1 */
#define DEFM_READY_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define DEFM_ERR_PACKET_FILTER_ADDR 0x6004 /* 24580 */
#define DEFM_ERR_PACKET_FILTER_MASK 0x3 /* 3 */
#define DEFM_ERR_PACKET_FILTER_OFFSET 0x0 /* 0 */
#define DEFM_ERR_PACKET_FILTER_WIDTH 0x2 /* 2 */
#define DEFM_ERR_PACKET_FILTER_DEFAULT 0x0 /* 0 */

/* Type = roCst  */
#define DEFM_XRAN_RU_PORT_ID_FIELD_WIDTH_ADDR 0x6080 /* 24704 */
#define DEFM_XRAN_RU_PORT_ID_FIELD_WIDTH_MASK 0xf /* 15 */
#define DEFM_XRAN_RU_PORT_ID_FIELD_WIDTH_OFFSET 0x0 /* 0 */
#define DEFM_XRAN_RU_PORT_ID_FIELD_WIDTH_WIDTH 0x4 /* 4 */
#define DEFM_XRAN_RU_PORT_ID_FIELD_WIDTH_DEFAULT 0x4 /* 4 */

/* Type = roCst  */
#define DEFM_XRAN_CC_ID_FIELD_WIDTH_ADDR 0x6080 /* 24704 */
#define DEFM_XRAN_CC_ID_FIELD_WIDTH_MASK 0xf0 /* 240 */
#define DEFM_XRAN_CC_ID_FIELD_WIDTH_OFFSET 0x4 /* 4 */
#define DEFM_XRAN_CC_ID_FIELD_WIDTH_WIDTH 0x4 /* 4 */
#define DEFM_XRAN_CC_ID_FIELD_WIDTH_DEFAULT 0x4 /* 4 */

/* Type = roCst  */
#define DEFM_XRAN_BANDSECTOR_FIELD_WIDTH_ADDR 0x6080 /* 24704 */
#define DEFM_XRAN_BANDSECTOR_FIELD_WIDTH_MASK 0xf00 /* 3840 */
#define DEFM_XRAN_BANDSECTOR_FIELD_WIDTH_OFFSET 0x8 /* 8 */
#define DEFM_XRAN_BANDSECTOR_FIELD_WIDTH_WIDTH 0x4 /* 4 */
#define DEFM_XRAN_BANDSECTOR_FIELD_WIDTH_DEFAULT 0x4 /* 4 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W0_31_0_ADDR 0x6100 /* 24832 */
#define DEFM_USER_DATA_FILTER_W0_31_0_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W0_31_0_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W0_31_0_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W0_31_0_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W0_63_32_ADDR 0x6104 /* 24836 */
#define DEFM_USER_DATA_FILTER_W0_63_32_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W0_63_32_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W0_63_32_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W0_63_32_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W0_95_64_ADDR 0x6108 /* 24840 */
#define DEFM_USER_DATA_FILTER_W0_95_64_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W0_95_64_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W0_95_64_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W0_95_64_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W0_127_96_ADDR 0x610c /* 24844 */
#define DEFM_USER_DATA_FILTER_W0_127_96_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W0_127_96_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W0_127_96_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W0_127_96_DEFAULT 0xfffffeae /* 4294966958 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W0_MASK_ADDR 0x6110 /* 24848 */
#define DEFM_USER_DATA_FILTER_W0_MASK_MASK 0xffff /* 65535 */
#define DEFM_USER_DATA_FILTER_W0_MASK_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W0_MASK_WIDTH 0x10 /* 16 */
#define DEFM_USER_DATA_FILTER_W0_MASK_DEFAULT 0xcfff /* 53247 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W1_31_0_ADDR 0x6120 /* 24864 */
#define DEFM_USER_DATA_FILTER_W1_31_0_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W1_31_0_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W1_31_0_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W1_31_0_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W1_63_32_ADDR 0x6124 /* 24868 */
#define DEFM_USER_DATA_FILTER_W1_63_32_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W1_63_32_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W1_63_32_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W1_63_32_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W1_95_64_ADDR 0x6128 /* 24872 */
#define DEFM_USER_DATA_FILTER_W1_95_64_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W1_95_64_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W1_95_64_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W1_95_64_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W1_127_96_ADDR 0x612c /* 24876 */
#define DEFM_USER_DATA_FILTER_W1_127_96_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W1_127_96_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W1_127_96_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W1_127_96_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W1_MASK_ADDR 0x6130 /* 24880 */
#define DEFM_USER_DATA_FILTER_W1_MASK_MASK 0xffff /* 65535 */
#define DEFM_USER_DATA_FILTER_W1_MASK_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W1_MASK_WIDTH 0x10 /* 16 */
#define DEFM_USER_DATA_FILTER_W1_MASK_DEFAULT 0xffff /* 65535 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W2_31_0_ADDR 0x6140 /* 24896 */
#define DEFM_USER_DATA_FILTER_W2_31_0_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W2_31_0_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W2_31_0_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W2_31_0_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W2_63_32_ADDR 0x6144 /* 24900 */
#define DEFM_USER_DATA_FILTER_W2_63_32_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W2_63_32_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W2_63_32_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W2_63_32_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W2_95_64_ADDR 0x6148 /* 24904 */
#define DEFM_USER_DATA_FILTER_W2_95_64_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W2_95_64_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W2_95_64_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W2_95_64_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W2_127_96_ADDR 0x614c /* 24908 */
#define DEFM_USER_DATA_FILTER_W2_127_96_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W2_127_96_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W2_127_96_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W2_127_96_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W2_MASK_ADDR 0x6150 /* 24912 */
#define DEFM_USER_DATA_FILTER_W2_MASK_MASK 0xffff /* 65535 */
#define DEFM_USER_DATA_FILTER_W2_MASK_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W2_MASK_WIDTH 0x10 /* 16 */
#define DEFM_USER_DATA_FILTER_W2_MASK_DEFAULT 0xffff /* 65535 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W3_31_0_ADDR 0x6160 /* 24928 */
#define DEFM_USER_DATA_FILTER_W3_31_0_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W3_31_0_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W3_31_0_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W3_31_0_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W3_63_32_ADDR 0x6164 /* 24932 */
#define DEFM_USER_DATA_FILTER_W3_63_32_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W3_63_32_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W3_63_32_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W3_63_32_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W3_95_64_ADDR 0x6168 /* 24936 */
#define DEFM_USER_DATA_FILTER_W3_95_64_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W3_95_64_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W3_95_64_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W3_95_64_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W3_127_96_ADDR 0x616c /* 24940 */
#define DEFM_USER_DATA_FILTER_W3_127_96_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W3_127_96_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W3_127_96_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W3_127_96_DEFAULT 0xffffffff /* 4294967295 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define DEFM_USER_DATA_FILTER_W3_MASK_ADDR 0x6170 /* 24944 */
#define DEFM_USER_DATA_FILTER_W3_MASK_MASK 0xffff /* 65535 */
#define DEFM_USER_DATA_FILTER_W3_MASK_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W3_MASK_WIDTH 0x10 /* 16 */
#define DEFM_USER_DATA_FILTER_W3_MASK_DEFAULT 0xffff /* 65535 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v1_0_eth 
* with prefix eth_ @ address 0xa000
*------------------------------------------------------------------------------
*/
/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_DEST_ADDR_31_0_ADDR 0xa000 /* 40960 */
#define ETH_DEST_ADDR_31_0_MASK 0xffffffff /* 4294967295 */
#define ETH_DEST_ADDR_31_0_OFFSET 0x0 /* 0 */
#define ETH_DEST_ADDR_31_0_WIDTH 0x20 /* 32 */
#define ETH_DEST_ADDR_31_0_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_DEST_ADDR_47_32_ADDR 0xa004 /* 40964 */
#define ETH_DEST_ADDR_47_32_MASK 0xffff /* 65535 */
#define ETH_DEST_ADDR_47_32_OFFSET 0x0 /* 0 */
#define ETH_DEST_ADDR_47_32_WIDTH 0x10 /* 16 */
#define ETH_DEST_ADDR_47_32_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_SRC_ADDR_31_0_ADDR 0xa008 /* 40968 */
#define ETH_SRC_ADDR_31_0_MASK 0xffffffff /* 4294967295 */
#define ETH_SRC_ADDR_31_0_OFFSET 0x0 /* 0 */
#define ETH_SRC_ADDR_31_0_WIDTH 0x20 /* 32 */
#define ETH_SRC_ADDR_31_0_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_SRC_ADDR_47_32_ADDR 0xa00c /* 40972 */
#define ETH_SRC_ADDR_47_32_MASK 0xffff /* 65535 */
#define ETH_SRC_ADDR_47_32_OFFSET 0x0 /* 0 */
#define ETH_SRC_ADDR_47_32_WIDTH 0x10 /* 16 */
#define ETH_SRC_ADDR_47_32_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_VLAN_ID_ADDR 0xa010 /* 40976 */
#define ETH_VLAN_ID_MASK 0xfff /* 4095 */
#define ETH_VLAN_ID_OFFSET 0x0 /* 0 */
#define ETH_VLAN_ID_WIDTH 0xc /* 12 */
#define ETH_VLAN_ID_DEFAULT 0x1 /* 1 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_VLAN_DEI_ADDR 0xa010 /* 40976 */
#define ETH_VLAN_DEI_MASK 0x1000 /* 4096 */
#define ETH_VLAN_DEI_OFFSET 0xc /* 12 */
#define ETH_VLAN_DEI_WIDTH 0x1 /* 1 */
#define ETH_VLAN_DEI_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_VLAN_PCP_ADDR 0xa010 /* 40976 */
#define ETH_VLAN_PCP_MASK 0xe000 /* 57344 */
#define ETH_VLAN_PCP_OFFSET 0xd /* 13 */
#define ETH_VLAN_PCP_WIDTH 0x3 /* 3 */
#define ETH_VLAN_PCP_DEFAULT 0x7 /* 7 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV4_VERSION_ADDR 0xa030 /* 41008 */
#define ETH_IPV4_VERSION_MASK 0xf /* 15 */
#define ETH_IPV4_VERSION_OFFSET 0x0 /* 0 */
#define ETH_IPV4_VERSION_WIDTH 0x4 /* 4 */
#define ETH_IPV4_VERSION_DEFAULT 0x4 /* 4 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV4_IHL_ADDR 0xa030 /* 41008 */
#define ETH_IPV4_IHL_MASK 0xf0 /* 240 */
#define ETH_IPV4_IHL_OFFSET 0x4 /* 4 */
#define ETH_IPV4_IHL_WIDTH 0x4 /* 4 */
#define ETH_IPV4_IHL_DEFAULT 0x5 /* 5 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV4_DSCP_ADDR 0xa034 /* 41012 */
#define ETH_IPV4_DSCP_MASK 0x3f /* 63 */
#define ETH_IPV4_DSCP_OFFSET 0x0 /* 0 */
#define ETH_IPV4_DSCP_WIDTH 0x6 /* 6 */
#define ETH_IPV4_DSCP_DEFAULT 0x2e /* 46 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV4_ECN_ADDR 0xa034 /* 41012 */
#define ETH_IPV4_ECN_MASK 0xc0 /* 192 */
#define ETH_IPV4_ECN_OFFSET 0x6 /* 6 */
#define ETH_IPV4_ECN_WIDTH 0x2 /* 2 */
#define ETH_IPV4_ECN_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV4_ID_ADDR 0xa038 /* 41016 */
#define ETH_IPV4_ID_MASK 0xffff /* 65535 */
#define ETH_IPV4_ID_OFFSET 0x0 /* 0 */
#define ETH_IPV4_ID_WIDTH 0x10 /* 16 */
#define ETH_IPV4_ID_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV4_FLAGS_ADDR 0xa03c /* 41020 */
#define ETH_IPV4_FLAGS_MASK 0x7 /* 7 */
#define ETH_IPV4_FLAGS_OFFSET 0x0 /* 0 */
#define ETH_IPV4_FLAGS_WIDTH 0x3 /* 3 */
#define ETH_IPV4_FLAGS_DEFAULT 0x2 /* 2 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV4_FRAGMENT_OFFSET_ADDR 0xa03c /* 41020 */
#define ETH_IPV4_FRAGMENT_OFFSET_MASK 0xfff8 /* 65528 */
#define ETH_IPV4_FRAGMENT_OFFSET_OFFSET 0x3 /* 3 */
#define ETH_IPV4_FRAGMENT_OFFSET_WIDTH 0xd /* 13 */
#define ETH_IPV4_FRAGMENT_OFFSET_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV4_TIME_TO_LIVE_ADDR 0xa040 /* 41024 */
#define ETH_IPV4_TIME_TO_LIVE_MASK 0xff /* 255 */
#define ETH_IPV4_TIME_TO_LIVE_OFFSET 0x0 /* 0 */
#define ETH_IPV4_TIME_TO_LIVE_WIDTH 0x8 /* 8 */
#define ETH_IPV4_TIME_TO_LIVE_DEFAULT 0x40 /* 64 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV4_PROTOCOL_ADDR 0xa044 /* 41028 */
#define ETH_IPV4_PROTOCOL_MASK 0xff /* 255 */
#define ETH_IPV4_PROTOCOL_OFFSET 0x0 /* 0 */
#define ETH_IPV4_PROTOCOL_WIDTH 0x8 /* 8 */
#define ETH_IPV4_PROTOCOL_DEFAULT 0x11 /* 17 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV4_SOURCE_ADD_ADDR 0xa048 /* 41032 */
#define ETH_IPV4_SOURCE_ADD_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV4_SOURCE_ADD_OFFSET 0x0 /* 0 */
#define ETH_IPV4_SOURCE_ADD_WIDTH 0x20 /* 32 */
#define ETH_IPV4_SOURCE_ADD_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV4_DESTINATION_ADD_ADDR 0xa04c /* 41036 */
#define ETH_IPV4_DESTINATION_ADD_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV4_DESTINATION_ADD_OFFSET 0x0 /* 0 */
#define ETH_IPV4_DESTINATION_ADD_WIDTH 0x20 /* 32 */
#define ETH_IPV4_DESTINATION_ADD_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_UDP_SOURCE_PORT_ADDR 0xa050 /* 41040 */
#define ETH_UDP_SOURCE_PORT_MASK 0xffff /* 65535 */
#define ETH_UDP_SOURCE_PORT_OFFSET 0x0 /* 0 */
#define ETH_UDP_SOURCE_PORT_WIDTH 0x10 /* 16 */
#define ETH_UDP_SOURCE_PORT_DEFAULT 0x8000 /* 32768 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_UDP_DESTINATION_PORT_ADDR 0xa050 /* 41040 */
#define ETH_UDP_DESTINATION_PORT_MASK 0xffff0000 /* 4294901760 */
#define ETH_UDP_DESTINATION_PORT_OFFSET 0x10 /* 16 */
#define ETH_UDP_DESTINATION_PORT_WIDTH 0x10 /* 16 */
#define ETH_UDP_DESTINATION_PORT_DEFAULT 0xc000 /* 49152 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_V_ADDR 0xa080 /* 41088 */
#define ETH_IPV6_V_MASK 0xf /* 15 */
#define ETH_IPV6_V_OFFSET 0x0 /* 0 */
#define ETH_IPV6_V_WIDTH 0x4 /* 4 */
#define ETH_IPV6_V_DEFAULT 0x6 /* 6 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_TRAFFIC_CLASS_ADDR 0xa084 /* 41092 */
#define ETH_IPV6_TRAFFIC_CLASS_MASK 0xff /* 255 */
#define ETH_IPV6_TRAFFIC_CLASS_OFFSET 0x0 /* 0 */
#define ETH_IPV6_TRAFFIC_CLASS_WIDTH 0x8 /* 8 */
#define ETH_IPV6_TRAFFIC_CLASS_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_FLOW_LABEL_ADDR 0xa088 /* 41096 */
#define ETH_IPV6_FLOW_LABEL_MASK 0xfffff /* 1048575 */
#define ETH_IPV6_FLOW_LABEL_OFFSET 0x0 /* 0 */
#define ETH_IPV6_FLOW_LABEL_WIDTH 0x14 /* 20 */
#define ETH_IPV6_FLOW_LABEL_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_NEXT_HEADER_ADDR 0xa08c /* 41100 */
#define ETH_IPV6_NEXT_HEADER_MASK 0xff /* 255 */
#define ETH_IPV6_NEXT_HEADER_OFFSET 0x0 /* 0 */
#define ETH_IPV6_NEXT_HEADER_WIDTH 0x8 /* 8 */
#define ETH_IPV6_NEXT_HEADER_DEFAULT 0x11 /* 17 */

/* Type = rw  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_HOP_LIMIT_ADDR 0xa090 /* 41104 */
#define ETH_IPV6_HOP_LIMIT_MASK 0xff /* 255 */
#define ETH_IPV6_HOP_LIMIT_OFFSET 0x0 /* 0 */
#define ETH_IPV6_HOP_LIMIT_WIDTH 0x8 /* 8 */
#define ETH_IPV6_HOP_LIMIT_DEFAULT 0x40 /* 64 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_SOURCE_ADD_31_0_ADDR 0xa094 /* 41108 */
#define ETH_IPV6_SOURCE_ADD_31_0_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_SOURCE_ADD_31_0_OFFSET 0x0 /* 0 */
#define ETH_IPV6_SOURCE_ADD_31_0_WIDTH 0x20 /* 32 */
#define ETH_IPV6_SOURCE_ADD_31_0_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_SOURCE_ADD_63_32_ADDR 0xa098 /* 41112 */
#define ETH_IPV6_SOURCE_ADD_63_32_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_SOURCE_ADD_63_32_OFFSET 0x0 /* 0 */
#define ETH_IPV6_SOURCE_ADD_63_32_WIDTH 0x20 /* 32 */
#define ETH_IPV6_SOURCE_ADD_63_32_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_SOURCE_ADD_95_64_ADDR 0xa09c /* 41116 */
#define ETH_IPV6_SOURCE_ADD_95_64_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_SOURCE_ADD_95_64_OFFSET 0x0 /* 0 */
#define ETH_IPV6_SOURCE_ADD_95_64_WIDTH 0x20 /* 32 */
#define ETH_IPV6_SOURCE_ADD_95_64_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_SOURCE_ADD_127_96_ADDR 0xa0a0 /* 41120 */
#define ETH_IPV6_SOURCE_ADD_127_96_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_SOURCE_ADD_127_96_OFFSET 0x0 /* 0 */
#define ETH_IPV6_SOURCE_ADD_127_96_WIDTH 0x20 /* 32 */
#define ETH_IPV6_SOURCE_ADD_127_96_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_DESTINATION_ADD_31_0_ADDR 0xa0a4 /* 41124 */
#define ETH_IPV6_DESTINATION_ADD_31_0_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_DESTINATION_ADD_31_0_OFFSET 0x0 /* 0 */
#define ETH_IPV6_DESTINATION_ADD_31_0_WIDTH 0x20 /* 32 */
#define ETH_IPV6_DESTINATION_ADD_31_0_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_DESTINATION_ADD_63_32_ADDR 0xa0a8 /* 41128 */
#define ETH_IPV6_DESTINATION_ADD_63_32_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_DESTINATION_ADD_63_32_OFFSET 0x0 /* 0 */
#define ETH_IPV6_DESTINATION_ADD_63_32_WIDTH 0x20 /* 32 */
#define ETH_IPV6_DESTINATION_ADD_63_32_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_DESTINATION_ADD_95_64_ADDR 0xa0ac /* 41132 */
#define ETH_IPV6_DESTINATION_ADD_95_64_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_DESTINATION_ADD_95_64_OFFSET 0x0 /* 0 */
#define ETH_IPV6_DESTINATION_ADD_95_64_WIDTH 0x20 /* 32 */
#define ETH_IPV6_DESTINATION_ADD_95_64_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
#define ETH_IPV6_DESTINATION_ADD_127_96_ADDR 0xa0b0 /* 41136 */
#define ETH_IPV6_DESTINATION_ADD_127_96_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_DESTINATION_ADD_127_96_OFFSET 0x0 /* 0 */
#define ETH_IPV6_DESTINATION_ADD_127_96_WIDTH 0x20 /* 32 */
#define ETH_IPV6_DESTINATION_ADD_127_96_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v1_0_stats 
* with prefix stats_ @ address 0xc000
*------------------------------------------------------------------------------
*/
/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_ADDR 0xc000 /* 49152 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_ADDR 0xc004 /* 49156 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_ADDR 0xc008 /* 49160 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_USER_DATA_RX_PACKETS_CNT_ADDR 0xc00c /* 49164 */
#define STATS_ETH_STATS_USER_DATA_RX_PACKETS_CNT_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_USER_DATA_RX_PACKETS_CNT_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_USER_DATA_RX_PACKETS_CNT_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_USER_DATA_RX_PACKETS_CNT_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_USER_DATA_RX_GOOD_PKT_CNT_ADDR 0xc010 /* 49168 */
#define STATS_ETH_STATS_USER_DATA_RX_GOOD_PKT_CNT_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_USER_DATA_RX_GOOD_PKT_CNT_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_USER_DATA_RX_GOOD_PKT_CNT_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_USER_DATA_RX_GOOD_PKT_CNT_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_USER_DATA_RX_BAD_PKT_CNT_ADDR 0xc014 /* 49172 */
#define STATS_ETH_STATS_USER_DATA_RX_BAD_PKT_CNT_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_USER_DATA_RX_BAD_PKT_CNT_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_USER_DATA_RX_BAD_PKT_CNT_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_USER_DATA_RX_BAD_PKT_CNT_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_USER_DATA_RX_BAD_FCS_CNT_ADDR 0xc018 /* 49176 */
#define STATS_ETH_STATS_USER_DATA_RX_BAD_FCS_CNT_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_USER_DATA_RX_BAD_FCS_CNT_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_USER_DATA_RX_BAD_FCS_CNT_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_USER_DATA_RX_BAD_FCS_CNT_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_USER_CTRL_RX_PACKETS_CNT_ADDR 0xc01c /* 49180 */
#define STATS_ETH_STATS_USER_CTRL_RX_PACKETS_CNT_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_USER_CTRL_RX_PACKETS_CNT_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_USER_CTRL_RX_PACKETS_CNT_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_USER_CTRL_RX_PACKETS_CNT_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_USER_CTRL_RX_GOOD_PKT_CNT_ADDR 0xc020 /* 49184 */
#define STATS_ETH_STATS_USER_CTRL_RX_GOOD_PKT_CNT_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_USER_CTRL_RX_GOOD_PKT_CNT_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_USER_CTRL_RX_GOOD_PKT_CNT_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_USER_CTRL_RX_GOOD_PKT_CNT_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_USER_CTRL_RX_BAD_PKT_CNT_ADDR 0xc024 /* 49188 */
#define STATS_ETH_STATS_USER_CTRL_RX_BAD_PKT_CNT_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_USER_CTRL_RX_BAD_PKT_CNT_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_USER_CTRL_RX_BAD_PKT_CNT_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_USER_CTRL_RX_BAD_PKT_CNT_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_USER_CTRL_RX_BAD_FCS_CNT_ADDR 0xc028 /* 49192 */
#define STATS_ETH_STATS_USER_CTRL_RX_BAD_FCS_CNT_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_USER_CTRL_RX_BAD_FCS_CNT_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_USER_CTRL_RX_BAD_FCS_CNT_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_USER_CTRL_RX_BAD_FCS_CNT_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_USER_DATA_RX_PKTS_RATE_ADDR 0xc02c /* 49196 */
#define STATS_ETH_STATS_USER_DATA_RX_PKTS_RATE_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_USER_DATA_RX_PKTS_RATE_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_USER_DATA_RX_PKTS_RATE_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_USER_DATA_RX_PKTS_RATE_DEFAULT 0x0 /* 0 */

/* Type = roSig  repeats using C_ETH_W @ multiples of 0x100 */
#define STATS_ETH_STATS_USER_CTRL_RX_PKTS_RATE_ADDR 0xc030 /* 49200 */
#define STATS_ETH_STATS_USER_CTRL_RX_PKTS_RATE_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_USER_CTRL_RX_PKTS_RATE_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_USER_CTRL_RX_PKTS_RATE_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_USER_CTRL_RX_PKTS_RATE_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v1_0_oran 
* with prefix oran_ @ address 0xe000
*------------------------------------------------------------------------------
*/
/* Type = wPlsH  */
#define ORAN_CC_RELOAD_ADDR 0xe000 /* 57344 */
#define ORAN_CC_RELOAD_MASK 0xff /* 255 */
#define ORAN_CC_RELOAD_OFFSET 0x0 /* 0 */
#define ORAN_CC_RELOAD_WIDTH 0x8 /* 8 */
#define ORAN_CC_RELOAD_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define ORAN_CC_ENABLE_ADDR 0xe004 /* 57348 */
#define ORAN_CC_ENABLE_MASK 0xff /* 255 */
#define ORAN_CC_ENABLE_OFFSET 0x0 /* 0 */
#define ORAN_CC_ENABLE_WIDTH 0x8 /* 8 */
#define ORAN_CC_ENABLE_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_SYMPERSLOT_ADDR 0xe100 /* 57600 */
#define ORAN_CC_SYMPERSLOT_MASK 0x1000000 /* 16777216 */
#define ORAN_CC_SYMPERSLOT_OFFSET 0x18 /* 24 */
#define ORAN_CC_SYMPERSLOT_WIDTH 0x1 /* 1 */
#define ORAN_CC_SYMPERSLOT_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_NUMEROLOGY_ADDR 0xe100 /* 57600 */
#define ORAN_CC_NUMEROLOGY_MASK 0x70000 /* 458752 */
#define ORAN_CC_NUMEROLOGY_OFFSET 0x10 /* 16 */
#define ORAN_CC_NUMEROLOGY_WIDTH 0x3 /* 3 */
#define ORAN_CC_NUMEROLOGY_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_NUMRBS_ADDR 0xe100 /* 57600 */
#define ORAN_CC_NUMRBS_MASK 0x1ff /* 511 */
#define ORAN_CC_NUMRBS_OFFSET 0x0 /* 0 */
#define ORAN_CC_NUMRBS_WIDTH 0x9 /* 9 */
#define ORAN_CC_NUMRBS_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_DL_CTRL_OFFSETS_ADDR 0xe104 /* 57604 */
#define ORAN_CC_DL_CTRL_OFFSETS_MASK 0xffff /* 65535 */
#define ORAN_CC_DL_CTRL_OFFSETS_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_CTRL_OFFSETS_WIDTH 0x10 /* 16 */
#define ORAN_CC_DL_CTRL_OFFSETS_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_ADDR 0xe108 /* 57608 */
#define ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_MASK 0xffff /* 65535 */
#define ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_WIDTH 0x10 /* 16 */
#define ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_UL_CTRL_OFFSETS_ADDR 0xe10c /* 57612 */
#define ORAN_CC_UL_CTRL_OFFSETS_MASK 0xffff /* 65535 */
#define ORAN_CC_UL_CTRL_OFFSETS_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_CTRL_OFFSETS_WIDTH 0x10 /* 16 */
#define ORAN_CC_UL_CTRL_OFFSETS_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_ADDR 0xe110 /* 57616 */
#define ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_MASK 0xffff /* 65535 */
#define ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_WIDTH 0x10 /* 16 */
#define ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_DL_DATA_SYM_START_INDEX_ADDR 0xe114 /* 57620 */
#define ORAN_CC_DL_DATA_SYM_START_INDEX_MASK 0x3f /* 63 */
#define ORAN_CC_DL_DATA_SYM_START_INDEX_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_DATA_SYM_START_INDEX_WIDTH 0x6 /* 6 */
#define ORAN_CC_DL_DATA_SYM_START_INDEX_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_DL_DATA_SYM_NUM_INDEX_ADDR 0xe114 /* 57620 */
#define ORAN_CC_DL_DATA_SYM_NUM_INDEX_MASK 0x3f00 /* 16128 */
#define ORAN_CC_DL_DATA_SYM_NUM_INDEX_OFFSET 0x8 /* 8 */
#define ORAN_CC_DL_DATA_SYM_NUM_INDEX_WIDTH 0x6 /* 6 */
#define ORAN_CC_DL_DATA_SYM_NUM_INDEX_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_DL_CTRL_SYM_NUM_INDEX_ADDR 0xe114 /* 57620 */
#define ORAN_CC_DL_CTRL_SYM_NUM_INDEX_MASK 0x3f0000 /* 4128768 */
#define ORAN_CC_DL_CTRL_SYM_NUM_INDEX_OFFSET 0x10 /* 16 */
#define ORAN_CC_DL_CTRL_SYM_NUM_INDEX_WIDTH 0x6 /* 6 */
#define ORAN_CC_DL_CTRL_SYM_NUM_INDEX_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_UL_CTRL_SYM_NUM_INDEX_ADDR 0xe114 /* 57620 */
#define ORAN_CC_UL_CTRL_SYM_NUM_INDEX_MASK 0x3f000000 /* 1056964608 */
#define ORAN_CC_UL_CTRL_SYM_NUM_INDEX_OFFSET 0x18 /* 24 */
#define ORAN_CC_UL_CTRL_SYM_NUM_INDEX_WIDTH 0x6 /* 6 */
#define ORAN_CC_UL_CTRL_SYM_NUM_INDEX_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_UL_UD_IQ_WIDTH_ADDR 0xe118 /* 57624 */
#define ORAN_CC_UL_UD_IQ_WIDTH_MASK 0xf /* 15 */
#define ORAN_CC_UL_UD_IQ_WIDTH_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_UD_IQ_WIDTH_WIDTH 0x4 /* 4 */
#define ORAN_CC_UL_UD_IQ_WIDTH_DEFAULT 0x9 /* 9 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_UL_UD_COMP_METH_ADDR 0xe118 /* 57624 */
#define ORAN_CC_UL_UD_COMP_METH_MASK 0xf0 /* 240 */
#define ORAN_CC_UL_UD_COMP_METH_OFFSET 0x4 /* 4 */
#define ORAN_CC_UL_UD_COMP_METH_WIDTH 0x4 /* 4 */
#define ORAN_CC_UL_UD_COMP_METH_DEFAULT 0x1 /* 1 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_UL_MPLANE_UDCOMP_PARAM_ADDR 0xe118 /* 57624 */
#define ORAN_CC_UL_MPLANE_UDCOMP_PARAM_MASK 0x100 /* 256 */
#define ORAN_CC_UL_MPLANE_UDCOMP_PARAM_OFFSET 0x8 /* 8 */
#define ORAN_CC_UL_MPLANE_UDCOMP_PARAM_WIDTH 0x1 /* 1 */
#define ORAN_CC_UL_MPLANE_UDCOMP_PARAM_DEFAULT 0x1 /* 1 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_DL_UD_IQ_WIDTH_ADDR 0xe11c /* 57628 */
#define ORAN_CC_DL_UD_IQ_WIDTH_MASK 0xf /* 15 */
#define ORAN_CC_DL_UD_IQ_WIDTH_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_UD_IQ_WIDTH_WIDTH 0x4 /* 4 */
#define ORAN_CC_DL_UD_IQ_WIDTH_DEFAULT 0x9 /* 9 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_DL_UD_COMP_METH_ADDR 0xe11c /* 57628 */
#define ORAN_CC_DL_UD_COMP_METH_MASK 0xf0 /* 240 */
#define ORAN_CC_DL_UD_COMP_METH_OFFSET 0x4 /* 4 */
#define ORAN_CC_DL_UD_COMP_METH_WIDTH 0x4 /* 4 */
#define ORAN_CC_DL_UD_COMP_METH_DEFAULT 0x1 /* 1 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_DL_MPLANE_UDCOMP_PARAM_ADDR 0xe11c /* 57628 */
#define ORAN_CC_DL_MPLANE_UDCOMP_PARAM_MASK 0x100 /* 256 */
#define ORAN_CC_DL_MPLANE_UDCOMP_PARAM_OFFSET 0x8 /* 8 */
#define ORAN_CC_DL_MPLANE_UDCOMP_PARAM_WIDTH 0x1 /* 1 */
#define ORAN_CC_DL_MPLANE_UDCOMP_PARAM_DEFAULT 0x1 /* 1 */

/* Type = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_UL_SETUP_C_ABS_SYMBOL_ADDR 0xe120 /* 57632 */
#define ORAN_CC_UL_SETUP_C_ABS_SYMBOL_MASK 0xfff /* 4095 */
#define ORAN_CC_UL_SETUP_C_ABS_SYMBOL_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_SETUP_C_ABS_SYMBOL_WIDTH 0xc /* 12 */
#define ORAN_CC_UL_SETUP_C_ABS_SYMBOL_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_UL_SETUP_C_CYCLES_ADDR 0xe124 /* 57636 */
#define ORAN_CC_UL_SETUP_C_CYCLES_MASK 0x1ffff /* 131071 */
#define ORAN_CC_UL_SETUP_C_CYCLES_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_SETUP_C_CYCLES_WIDTH 0x11 /* 17 */
#define ORAN_CC_UL_SETUP_C_CYCLES_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_UL_SETUP_D_CYCLES_ADDR 0xe128 /* 57640 */
#define ORAN_CC_UL_SETUP_D_CYCLES_MASK 0x1ffff /* 131071 */
#define ORAN_CC_UL_SETUP_D_CYCLES_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_SETUP_D_CYCLES_WIDTH 0x11 /* 17 */
#define ORAN_CC_UL_SETUP_D_CYCLES_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_DL_SETUP_C_ABS_SYMBOL_ADDR 0xe130 /* 57648 */
#define ORAN_CC_DL_SETUP_C_ABS_SYMBOL_MASK 0xfff /* 4095 */
#define ORAN_CC_DL_SETUP_C_ABS_SYMBOL_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_SETUP_C_ABS_SYMBOL_WIDTH 0xc /* 12 */
#define ORAN_CC_DL_SETUP_C_ABS_SYMBOL_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_DL_SETUP_C_CYCLES_ADDR 0xe134 /* 57652 */
#define ORAN_CC_DL_SETUP_C_CYCLES_MASK 0x1ffff /* 131071 */
#define ORAN_CC_DL_SETUP_C_CYCLES_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_SETUP_C_CYCLES_WIDTH 0x11 /* 17 */
#define ORAN_CC_DL_SETUP_C_CYCLES_DEFAULT 0x0 /* 0 */

/* Type = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_DL_SETUP_D_CYCLES_ADDR 0xe138 /* 57656 */
#define ORAN_CC_DL_SETUP_D_CYCLES_MASK 0x1ffff /* 131071 */
#define ORAN_CC_DL_SETUP_D_CYCLES_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_SETUP_D_CYCLES_WIDTH 0x11 /* 17 */
#define ORAN_CC_DL_SETUP_D_CYCLES_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_UL_BASE_OFFSET_ADDR 0xe140 /* 57664 */
#define ORAN_CC_UL_BASE_OFFSET_MASK 0xffff /* 65535 */
#define ORAN_CC_UL_BASE_OFFSET_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_BASE_OFFSET_WIDTH 0x10 /* 16 */
#define ORAN_CC_UL_BASE_OFFSET_DEFAULT 0x0 /* 0 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_MAX_SYMBOLS_ADDR 0xe158 /* 57688 */
#define ORAN_CC_MAX_SYMBOLS_MASK 0xfff /* 4095 */
#define ORAN_CC_MAX_SYMBOLS_OFFSET 0x0 /* 0 */
#define ORAN_CC_MAX_SYMBOLS_WIDTH 0xc /* 12 */
#define ORAN_CC_MAX_SYMBOLS_DEFAULT 0x8c /* 140 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_DL_ADDR 0xe160 /* 57696 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_DL_MASK 0xffff /* 65535 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_DL_OFFSET 0x0 /* 0 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_DL_WIDTH 0x10 /* 16 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_DL_DEFAULT 0x10 /* 16 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_UL_ADDR 0xe164 /* 57700 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_UL_MASK 0xffff /* 65535 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_UL_OFFSET 0x0 /* 0 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_UL_WIDTH 0x10 /* 16 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_UL_DEFAULT 0x10 /* 16 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_MODVALS_DL_ADDR 0xe168 /* 57704 */
#define ORAN_CC_MODVALS_DL_MASK 0xffff /* 65535 */
#define ORAN_CC_MODVALS_DL_OFFSET 0x0 /* 0 */
#define ORAN_CC_MODVALS_DL_WIDTH 0x10 /* 16 */
#define ORAN_CC_MODVALS_DL_DEFAULT 0x40 /* 64 */

/* Type = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
#define ORAN_CC_MODVALS_UL_ADDR 0xe16c /* 57708 */
#define ORAN_CC_MODVALS_UL_MASK 0xffff /* 65535 */
#define ORAN_CC_MODVALS_UL_OFFSET 0x0 /* 0 */
#define ORAN_CC_MODVALS_UL_WIDTH 0x10 /* 16 */
#define ORAN_CC_MODVALS_UL_DEFAULT 0x40 /* 64 */

/* Type = rw  repeats using C_MAX_DL_SYMS @ multiples of 0x04 */
#define ORAN_CC_DL_DATA_UNROLL_OFFSET_ADDR 0xe500 /* 58624 */
#define ORAN_CC_DL_DATA_UNROLL_OFFSET_MASK 0xffff /* 65535 */
#define ORAN_CC_DL_DATA_UNROLL_OFFSET_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_DATA_UNROLL_OFFSET_WIDTH 0x10 /* 16 */
#define ORAN_CC_DL_DATA_UNROLL_OFFSET_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define ORAN_SETUP_CNT_ADDR 0xe600 /* 58880 */
#define ORAN_SETUP_CNT_MASK 0xffffffff /* 4294967295 */
#define ORAN_SETUP_CNT_OFFSET 0x0 /* 0 */
#define ORAN_SETUP_CNT_WIDTH 0x20 /* 32 */
#define ORAN_SETUP_CNT_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define ORAN_STRIP_FCS_ADDR 0xe604 /* 58884 */
#define ORAN_STRIP_FCS_MASK 0x1 /* 1 */
#define ORAN_STRIP_FCS_OFFSET 0x0 /* 0 */
#define ORAN_STRIP_FCS_WIDTH 0x1 /* 1 */
#define ORAN_STRIP_FCS_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define ORAN_SETUP_SF_ADDR 0xe608 /* 58888 */
#define ORAN_SETUP_SF_MASK 0xf /* 15 */
#define ORAN_SETUP_SF_OFFSET 0x0 /* 0 */
#define ORAN_SETUP_SF_WIDTH 0x4 /* 4 */
#define ORAN_SETUP_SF_DEFAULT 0x9 /* 9 */

/* Type = rw  */
#define ORAN_SETUP_SL_ADDR 0xe60c /* 58892 */
#define ORAN_SETUP_SL_MASK 0xf /* 15 */
#define ORAN_SETUP_SL_OFFSET 0x0 /* 0 */
#define ORAN_SETUP_SL_WIDTH 0x4 /* 4 */
#define ORAN_SETUP_SL_DEFAULT 0x0 /* 0 */

/* Type = rw  */
#define ORAN_SETUP_SY_ADDR 0xe610 /* 58896 */
#define ORAN_SETUP_SY_MASK 0xf /* 15 */
#define ORAN_SETUP_SY_OFFSET 0x0 /* 0 */
#define ORAN_SETUP_SY_WIDTH 0x4 /* 4 */
#define ORAN_SETUP_SY_DEFAULT 0x7 /* 7 */


