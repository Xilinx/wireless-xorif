// SPDX-License-Identifier: GPL-2.0 
/*-----------------------------------------------------------------------------
* C Header bank BASE definitions
*------------------------------------------------------------------------------
*/
#define ORAN_RADIO_IF_V3_0_CFG_BASE_ADDR 0x0 /* 0 */
#define ORAN_RADIO_IF_V3_0_FRAM_BASE_ADDR 0x2000 /* 8192 */
#define ORAN_RADIO_IF_V3_0_DEFM_BASE_ADDR 0x6000 /* 24576 */
#define ORAN_RADIO_IF_V3_0_ORANSHR_BASE_ADDR 0x8000 /* 32768 */
#define ORAN_RADIO_IF_V3_0_ETH_BASE_ADDR 0xa000 /* 40960 */
#define ORAN_RADIO_IF_V3_0_STATS_BASE_ADDR 0xc000 /* 49152 */
#define ORAN_RADIO_IF_V3_0_ORAN_BASE_ADDR 0xe000 /* 57344 */
#define ORAN_RADIO_IF_V3_0_CHAN_PROC_CFG_BASE_ADDR 0x10000 /* 65536 */
#define ORAN_RADIO_IF_V3_0_CHAN_PROC_FRAM_BASE_ADDR 0x12000 /* 73728 */
#define ORAN_RADIO_IF_V3_0_CHAN_PROC_SETUP_BASE_ADDR 0x14000 /* 81920 */
#define ORAN_RADIO_IF_V3_0_CHAN_PROC_SEQUENCE_TABLE_BASE_ADDR 0x16000 /* 90112 */

/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v3_0_cfg 
* with prefix cfg_ @ address 0x0
*------------------------------------------------------------------------------
*/
/* Type   = roInt  */
/* sigena =  */
/* Doc    : Major revision ID */
#define CFG_MAJOR_REVISION_ADDR 0x0 /* 0 */
#define CFG_MAJOR_REVISION_MASK 0xff000000 /* 4278190080 */
#define CFG_MAJOR_REVISION_OFFSET 0x18 /* 24 */
#define CFG_MAJOR_REVISION_WIDTH 0x8 /* 8 */
#define CFG_MAJOR_REVISION_DEFAULT 0x3 /* 3 */

/* Type   = roInt  */
/* sigena =  */
/* Doc    : Minor revision ID */
#define CFG_MINOR_REVISION_ADDR 0x0 /* 0 */
#define CFG_MINOR_REVISION_MASK 0xff0000 /* 16711680 */
#define CFG_MINOR_REVISION_OFFSET 0x10 /* 16 */
#define CFG_MINOR_REVISION_WIDTH 0x8 /* 8 */
#define CFG_MINOR_REVISION_DEFAULT 0x0 /* 0 */

/* Type   = roInt  */
/* sigena =  */
/* Doc    : Version revision */
#define CFG_VERSION_REVISION_ADDR 0x0 /* 0 */
#define CFG_VERSION_REVISION_MASK 0xff00 /* 65280 */
#define CFG_VERSION_REVISION_OFFSET 0x8 /* 8 */
#define CFG_VERSION_REVISION_WIDTH 0x8 /* 8 */
#define CFG_VERSION_REVISION_DEFAULT 0x0 /* 0 */

/* Type   = roInt  */
/* sigena =  */
/* Doc    : Holds date of last regmap generation */
#define CFG_INTERNAL_REVISION_ADDR 0x4 /* 4 */
#define CFG_INTERNAL_REVISION_MASK 0xffffffff /* 4294967295 */
#define CFG_INTERNAL_REVISION_OFFSET 0x0 /* 0 */
#define CFG_INTERNAL_REVISION_WIDTH 0x20 /* 32 */
#define CFG_INTERNAL_REVISION_DEFAULT 0x15f92c8 /* 23040712 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Timeout for AXI4-Lite interface access */
#define CFG_TIMEOUT_VALUE_ADDR 0x8 /* 8 */
#define CFG_TIMEOUT_VALUE_MASK 0xfff /* 4095 */
#define CFG_TIMEOUT_VALUE_OFFSET 0x0 /* 0 */
#define CFG_TIMEOUT_VALUE_WIDTH 0xc /* 12 */
#define CFG_TIMEOUT_VALUE_DEFAULT 0x80 /* 128 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : User RW GPIO for custom user control */
#define CFG_USER_RW_OUT_ADDR 0xc /* 12 */
#define CFG_USER_RW_OUT_MASK 0xff /* 255 */
#define CFG_USER_RW_OUT_OFFSET 0x0 /* 0 */
#define CFG_USER_RW_OUT_WIDTH 0x8 /* 8 */
#define CFG_USER_RW_OUT_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : User RO GPIO for custom user status */
#define CFG_USER_RO_IN_ADDR 0xc /* 12 */
#define CFG_USER_RO_IN_MASK 0xff0000 /* 16711680 */
#define CFG_USER_RO_IN_OFFSET 0x10 /* 16 */
#define CFG_USER_RO_IN_WIDTH 0x8 /* 8 */
#define CFG_USER_RO_IN_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Master Interrupt Enable, set high to enable interrupts and set low to clear all interrupts. Interrupts signify that most likely there is an error in the core configuration that should be corrected. In normal operation interrupts are not expected, even with ORAN traffic outside the reception window. */
#define CFG_MASTER_INT_ENABLE_ADDR 0x10 /* 16 */
#define CFG_MASTER_INT_ENABLE_MASK 0x1 /* 1 */
#define CFG_MASTER_INT_ENABLE_OFFSET 0x0 /* 0 */
#define CFG_MASTER_INT_ENABLE_WIDTH 0x1 /* 1 */
#define CFG_MASTER_INT_ENABLE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable interrupt for defm_int_infifo_of */
#define CFG_DEFM_INT_ENA_INFIFO_OF_ADDR 0x14 /* 20 */
#define CFG_DEFM_INT_ENA_INFIFO_OF_MASK 0x100 /* 256 */
#define CFG_DEFM_INT_ENA_INFIFO_OF_OFFSET 0x8 /* 8 */
#define CFG_DEFM_INT_ENA_INFIFO_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ENA_INFIFO_OF_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable interrupt for defm_int_infifo_uf */
#define CFG_DEFM_INT_ENA_INFIFO_UF_ADDR 0x14 /* 20 */
#define CFG_DEFM_INT_ENA_INFIFO_UF_MASK 0x200 /* 512 */
#define CFG_DEFM_INT_ENA_INFIFO_UF_OFFSET 0x9 /* 9 */
#define CFG_DEFM_INT_ENA_INFIFO_UF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ENA_INFIFO_UF_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable interrupt for defm_int_eth_pipe_c_buf_of */
#define CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF_ADDR 0x14 /* 20 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF_MASK 0x400 /* 1024 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF_OFFSET 0xa /* 10 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_C_BUF_OF_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable interrupt for defm_int_eth_pipe_table_of */
#define CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF_ADDR 0x14 /* 20 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF_MASK 0x800 /* 2048 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF_OFFSET 0xb /* 11 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ENA_ETH_PIPE_TABLE_OF_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable interrupt for fram_int_outfifo_of */
#define CFG_FRAM_INT_ENA_OUTFIFO_OF_ADDR 0x14 /* 20 */
#define CFG_FRAM_INT_ENA_OUTFIFO_OF_MASK 0x1000 /* 4096 */
#define CFG_FRAM_INT_ENA_OUTFIFO_OF_OFFSET 0xc /* 12 */
#define CFG_FRAM_INT_ENA_OUTFIFO_OF_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_ENA_OUTFIFO_OF_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable interrupt for fram_int_outfifo_uf */
#define CFG_FRAM_INT_ENA_OUTFIFO_UF_ADDR 0x14 /* 20 */
#define CFG_FRAM_INT_ENA_OUTFIFO_UF_MASK 0x2000 /* 8192 */
#define CFG_FRAM_INT_ENA_OUTFIFO_UF_OFFSET 0xd /* 13 */
#define CFG_FRAM_INT_ENA_OUTFIFO_UF_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_ENA_OUTFIFO_UF_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable interrupt for fram_int_prach_section_overflow */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW_ADDR 0x14 /* 20 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW_MASK 0x4000 /* 16384 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW_OFFSET 0xe /* 14 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_OVERFLOW_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable interrupt for fram_int_prach_section_notfound */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND_ADDR 0x14 /* 20 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND_MASK 0x8000 /* 32768 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND_OFFSET 0xf /* 15 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_ENA_PRACH_SECTION_NOTFOUND_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : AXI4-Lite interface Interrupt Enable */
#define CFG_AXI_TIMEOUT_ENABLE_ADDR 0x14 /* 20 */
#define CFG_AXI_TIMEOUT_ENABLE_MASK 0x80000000 /* 2147483648 */
#define CFG_AXI_TIMEOUT_ENABLE_OFFSET 0x1f /* 31 */
#define CFG_AXI_TIMEOUT_ENABLE_WIDTH 0x1 /* 1 */
#define CFG_AXI_TIMEOUT_ENABLE_DEFAULT 0x1 /* 1 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Reports the level of the FRAM reset. Not used for interrupt, just status reporting. */
#define CFG_FRAM_RESET_STATUS_ADDR 0x18 /* 24 */
#define CFG_FRAM_RESET_STATUS_MASK 0x1 /* 1 */
#define CFG_FRAM_RESET_STATUS_OFFSET 0x0 /* 0 */
#define CFG_FRAM_RESET_STATUS_WIDTH 0x1 /* 1 */
#define CFG_FRAM_RESET_STATUS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Reports the level of the DEFM reset. Not used for interrupt, just status reporting. */
#define CFG_DEFM_RESET_STATUS_ADDR 0x18 /* 24 */
#define CFG_DEFM_RESET_STATUS_MASK 0x2 /* 2 */
#define CFG_DEFM_RESET_STATUS_OFFSET 0x1 /* 1 */
#define CFG_DEFM_RESET_STATUS_WIDTH 0x1 /* 1 */
#define CFG_DEFM_RESET_STATUS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Reports an input Ethernet Fifo Overflow on at least one of the Ethernet Input FIFOs on Deframer Ethernet pipes. This should not trigger during normal core management. Even when pushing back the traffic (i.e. keeping mX_message_tready =0) the core should safely drop the incoming packets. Then the actual occurrence of the interrupt indicates an internal un-expected state. To clear the root of the fault it is necessary to apply defm_reset and clear the sticky flag using the master interrupt enable. */
#define CFG_DEFM_INT_INFIFO_OF_ADDR 0x18 /* 24 */
#define CFG_DEFM_INT_INFIFO_OF_MASK 0x100 /* 256 */
#define CFG_DEFM_INT_INFIFO_OF_OFFSET 0x8 /* 8 */
#define CFG_DEFM_INT_INFIFO_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_INFIFO_OF_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Reports an input Ethernet Fifo Underflow on at least one of the Ethernet Input FIFOs on the Deframer Ethernet pipes. Should never happen during normal core management. As previous, to clear the root of the fault it is necessary to apply defm_reset and clear the sticky flag using the master interrupt enable. */
#define CFG_DEFM_INT_INFIFO_UF_ADDR 0x18 /* 24 */
#define CFG_DEFM_INT_INFIFO_UF_MASK 0x200 /* 512 */
#define CFG_DEFM_INT_INFIFO_UF_OFFSET 0x9 /* 9 */
#define CFG_DEFM_INT_INFIFO_UF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_INFIFO_UF_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Reports an Overflow in the circular buffer, that is the memory available at the end of each Deframer Ethernet pipe to store packets for a given spatial stream while such a resource buffer is kept busy by another Ethernet pipe. A prolonged pushback on deframer output interfaces can trigger such an overflow situation. Packets arriving during circular buffer overflow are safely dropped and the circular buffer is expected to recover automatically once the cause of the overflow is removed. However, if the condition persists this means that the core reached a wrong internal state and to clear the root of the fault it is necessary to apply defm_reset and clear the sticky flag using the master interrupt enable. */
#define CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_ADDR 0x18 /* 24 */
#define CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_MASK 0x400 /* 1024 */
#define CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_OFFSET 0xa /* 10 */
#define CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Reports an Overflow in the table which indexes the circular buffer. It is relatively easy to trigger such a condition by reducing the depth of the CIRC_META_TABLE to 8 and then sending a maximum size DL U-Plane packets followed by a series of 8 or more short C-Plane packets. Such a condition is not blocking but causes packets to be dropped and should be solved automatically. The core should recover automatically, but if the condition persists this means that the core reached a wrong internal state and to clear the root of the fault it is necessary to apply defm_reset and clear the sticky flag using the master interrupt enable. */
#define CFG_DEFM_INT_ETH_PIPE_TABLE_OF_ADDR 0x18 /* 24 */
#define CFG_DEFM_INT_ETH_PIPE_TABLE_OF_MASK 0x800 /* 2048 */
#define CFG_DEFM_INT_ETH_PIPE_TABLE_OF_OFFSET 0xb /* 11 */
#define CFG_DEFM_INT_ETH_PIPE_TABLE_OF_WIDTH 0x1 /* 1 */
#define CFG_DEFM_INT_ETH_PIPE_TABLE_OF_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Reports an Overflow on at least one of the Ethernet Output FIFOs on Framer Ethernet pipes This should never happen during normal core management: the stream of UL packets can be safely pushed back. To clear the root of the fault it is necessary to apply fram_reset and clear the sticky flag using the master interrupt enable. */
#define CFG_FRAM_INT_OUTFIFO_OF_ADDR 0x18 /* 24 */
#define CFG_FRAM_INT_OUTFIFO_OF_MASK 0x1000 /* 4096 */
#define CFG_FRAM_INT_OUTFIFO_OF_OFFSET 0xc /* 12 */
#define CFG_FRAM_INT_OUTFIFO_OF_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_OUTFIFO_OF_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Reports an Underflow on at least one of the Ethernet Output FIFOs on Framer Ethernet pipes This should never happen during normal core management. The actual occurrence of the interrupt indicates an internal un-expected state, such as to produce runt or malformed packets. To clear the root of the fault it is necessary to apply fram_reset and clear the sticky flag using the master interrupt enable. */
#define CFG_FRAM_INT_OUTFIFO_UF_ADDR 0x18 /* 24 */
#define CFG_FRAM_INT_OUTFIFO_UF_MASK 0x2000 /* 8192 */
#define CFG_FRAM_INT_OUTFIFO_UF_OFFSET 0xd /* 13 */
#define CFG_FRAM_INT_OUTFIFO_UF_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_OUTFIFO_UF_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Reports a Ethernet pipe prach section Overflow. The cause is generally no response for the PRACH C plane request, or more PRACH sections have been requested than the core has been configured for using Xran_Prach_Sections. Automatically recovered once the fault condition removed. If the issue persists to clear the root of the fault it is necessary to apply fram_reset and clear the sticky flag using the master interrupt enable. */
#define CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_ADDR 0x18 /* 24 */
#define CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_MASK 0x4000 /* 16384 */
#define CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_OFFSET 0xe /* 14 */
#define CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Reports a section was not found in the PRACH UL table, indicating that sX_fram_prach_tuser did not match the fields CCID[23:20], Ru_Port_ID[19:12] and Section ID[11:0]. Automatically recovered once the fault condition removed. If the issue persists to clear the root of the fault it is necessary to apply fram_reset and clear the sticky flag using the master interrupt enable. */
#define CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_ADDR 0x18 /* 24 */
#define CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_MASK 0x8000 /* 32768 */
#define CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_OFFSET 0xf /* 15 */
#define CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_WIDTH 0x1 /* 1 */
#define CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : AXI4-Lite interface timeout has occurred */
#define CFG_AXI_TIMEOUT_STATUS_ADDR 0x18 /* 24 */
#define CFG_AXI_TIMEOUT_STATUS_MASK 0x80000000 /* 2147483648 */
#define CFG_AXI_TIMEOUT_STATUS_OFFSET 0x1f /* 31 */
#define CFG_AXI_TIMEOUT_STATUS_WIDTH 0x1 /* 1 */
#define CFG_AXI_TIMEOUT_STATUS_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : NODOC Flag used to sample status register in alternate clock domains. Routed but unused */
#define CFG_INTERRUPT_STATUS_SAMPLE_ADDR 0x1c /* 28 */
#define CFG_INTERRUPT_STATUS_SAMPLE_MASK 0x1 /* 1 */
#define CFG_INTERRUPT_STATUS_SAMPLE_OFFSET 0x0 /* 0 */
#define CFG_INTERRUPT_STATUS_SAMPLE_WIDTH 0x1 /* 1 */
#define CFG_INTERRUPT_STATUS_SAMPLE_DEFAULT 0x1 /* 1 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Constant reporting the max number of antennas supported by the Framer. */
#define CFG_CONFIG_NO_OF_FRAM_ANTS_ADDR 0x20 /* 32 */
#define CFG_CONFIG_NO_OF_FRAM_ANTS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_NO_OF_FRAM_ANTS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_NO_OF_FRAM_ANTS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_NO_OF_FRAM_ANTS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Constant reporting the max number of antennas supported by the De-framer. */
#define CFG_CONFIG_NO_OF_DEFM_ANTS_ADDR 0x20 /* 32 */
#define CFG_CONFIG_NO_OF_DEFM_ANTS_MASK 0xffff0000 /* 4294901760 */
#define CFG_CONFIG_NO_OF_DEFM_ANTS_OFFSET 0x10 /* 16 */
#define CFG_CONFIG_NO_OF_DEFM_ANTS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_NO_OF_DEFM_ANTS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Number of Ethernet data ports */
#define CFG_CONFIG_NO_OF_ETH_PORTS_ADDR 0x24 /* 36 */
#define CFG_CONFIG_NO_OF_ETH_PORTS_MASK 0x3ff /* 1023 */
#define CFG_CONFIG_NO_OF_ETH_PORTS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_NO_OF_ETH_PORTS_WIDTH 0xa /* 10 */
#define CFG_CONFIG_NO_OF_ETH_PORTS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : ORAN_Mode, always reports as 5. */
#define CFG_CONFIG_XRAN_SUPPORT_MODE_ADDR 0x28 /* 40 */
#define CFG_CONFIG_XRAN_SUPPORT_MODE_MASK 0xf /* 15 */
#define CFG_CONFIG_XRAN_SUPPORT_MODE_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_SUPPORT_MODE_WIDTH 0x4 /* 4 */
#define CFG_CONFIG_XRAN_SUPPORT_MODE_DEFAULT 0x5 /* 5 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Decompression logic is available in the core for BFP 9/12/14, Modulation 1/2/4 (Bit Unpack). */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_ENABLED_ADDR 0x28 /* 40 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_ENABLED_MASK 0x10 /* 16 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_ENABLED_OFFSET 0x4 /* 4 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_ENABLED_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_ENABLED_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Compression is available in the core for BFP 9/12/14. */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_ENABLED_ADDR 0x28 /* 40 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_ENABLED_MASK 0x20 /* 32 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_ENABLED_OFFSET 0x5 /* 5 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_ENABLED_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_ENABLED_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Precoding Extension 3 support is available in the core. */
#define CFG_CONFIG_XRAN_PRECODING_EXT3_PORT_ADDR 0x28 /* 40 */
#define CFG_CONFIG_XRAN_PRECODING_EXT3_PORT_MASK 0x40 /* 64 */
#define CFG_CONFIG_XRAN_PRECODING_EXT3_PORT_OFFSET 0x6 /* 6 */
#define CFG_CONFIG_XRAN_PRECODING_EXT3_PORT_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_PRECODING_EXT3_PORT_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Double Buffer PUxCH, PDxCH processing included in core. */
#define CFG_CONFIG_XRAN_OCP_IN_CORE_ADDR 0x28 /* 40 */
#define CFG_CONFIG_XRAN_OCP_IN_CORE_MASK 0x80 /* 128 */
#define CFG_CONFIG_XRAN_OCP_IN_CORE_OFFSET 0x7 /* 7 */
#define CFG_CONFIG_XRAN_OCP_IN_CORE_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_OCP_IN_CORE_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max number of supported Component carriers (CC). */
#define CFG_CONFIG_XRAN_MAX_CC_ADDR 0x2c /* 44 */
#define CFG_CONFIG_XRAN_MAX_CC_MASK 0x3f /* 63 */
#define CFG_CONFIG_XRAN_MAX_CC_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_CC_WIDTH 0x6 /* 6 */
#define CFG_CONFIG_XRAN_MAX_CC_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max number of symbols supported in DL UPlane for all Component Carriers. */
#define CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_ADDR 0x30 /* 48 */
#define CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_MASK 0x7f /* 127 */
#define CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_WIDTH 0x7 /* 7 */
#define CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : NODOC XRAN max supported Numerology. */
#define CFG_CONFIG_XRAN_MAX_NUMEROLOGY_ADDR 0x34 /* 52 */
#define CFG_CONFIG_XRAN_MAX_NUMEROLOGY_MASK 0x7 /* 7 */
#define CFG_CONFIG_XRAN_MAX_NUMEROLOGY_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_NUMEROLOGY_WIDTH 0x3 /* 3 */
#define CFG_CONFIG_XRAN_MAX_NUMEROLOGY_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : NODOC XRAN min supported Numerology. */
#define CFG_CONFIG_XRAN_MIN_NUMEROLOGY_ADDR 0x38 /* 56 */
#define CFG_CONFIG_XRAN_MIN_NUMEROLOGY_MASK 0x7 /* 7 */
#define CFG_CONFIG_XRAN_MIN_NUMEROLOGY_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MIN_NUMEROLOGY_WIDTH 0x3 /* 3 */
#define CFG_CONFIG_XRAN_MIN_NUMEROLOGY_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max size of the framer payload in bytes. */
#define CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX_ADDR 0x3c /* 60 */
#define CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : NODOC, unused remove in .2 */
#define CFG_CONFIG_XRAN_FRAM_AUTO_START_ADDR 0x40 /* 64 */
#define CFG_CONFIG_XRAN_FRAM_AUTO_START_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_FRAM_AUTO_START_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_FRAM_AUTO_START_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_FRAM_AUTO_START_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max size of the deframer payload in bytes. */
#define CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX_ADDR 0x44 /* 68 */
#define CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max number of subcarriers supported over all Component Carriers. */
#define CFG_CONFIG_XRAN_MAX_SCS_ADDR 0x48 /* 72 */
#define CFG_CONFIG_XRAN_MAX_SCS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_SCS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_SCS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_SCS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max number of ctrl symbols per Component Carrier. */
#define CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS_ADDR 0x4c /* 76 */
#define CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max ul ctrl 1kwords. Per Spatial Stream. Shared over all Component Carriers. */
#define CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS_ADDR 0x50 /* 80 */
#define CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_UL_CTRL_1KWORDS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max dl ctrl 1kwords. Per Spatial Stream. Shared over all Component Carriers. */
#define CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS_ADDR 0x54 /* 84 */
#define CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_DL_CTRL_1KWORDS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max dl data 1kwords. Per Spatial Stream. Shared over all Component Carriers. */
#define CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS_ADDR 0x58 /* 88 */
#define CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_MAX_DL_DATA_1KWORDS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Timer clk period for internal_bus_clk in ps. */
#define CFG_CONFIG_XRAN_TIMER_CLK_PS_ADDR 0x5c /* 92 */
#define CFG_CONFIG_XRAN_TIMER_CLK_PS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_TIMER_CLK_PS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_TIMER_CLK_PS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_TIMER_CLK_PS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Number of unsolicited ports on the framer. */
#define CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM_ADDR 0x68 /* 104 */
#define CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_UNSOL_PORTS_FRAM_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Depth of the Ethernet Spatial Stream Buffer in 8 Byte words */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH_ADDR 0x6c /* 108 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_WORD_DEPTH_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Number of Packet pointers in the Ethernet Spatial Stream Buffer. */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS_ADDR 0x70 /* 112 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_ETH_SS_BUF_PKT_PTRS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Number of PRACH ports on the framer. */
#define CFG_CONFIG_XRAN_PRACH_C_PORTS_ADDR 0x74 /* 116 */
#define CFG_CONFIG_XRAN_PRACH_C_PORTS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_PRACH_C_PORTS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_PRACH_C_PORTS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_PRACH_C_PORTS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Optimisation bus. 24d0        , cfg_wide_filt, cfg_ena_ipv4 , cfg_ena_ipv6 , cfg_ena_ssb  , cfg_ena_ext11, cfg_ena_ext45, cfg_ena_ext3 , cfg_ena_modc  */
#define CFG_CONFIG_XRAN_OPTIMISATIONS_ADDR 0x78 /* 120 */
#define CFG_CONFIG_XRAN_OPTIMISATIONS_MASK 0xffffffff /* 4294967295 */
#define CFG_CONFIG_XRAN_OPTIMISATIONS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_OPTIMISATIONS_WIDTH 0x20 /* 32 */
#define CFG_CONFIG_XRAN_OPTIMISATIONS_DEFAULT 0x0 /* 0 */

/* Type   = roInt  */
/* sigena =  */
/* Doc    : No Compression support in decompressor. */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_NOCOMP_ADDR 0x80 /* 128 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_NOCOMP_MASK 0x1 /* 1 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_NOCOMP_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_NOCOMP_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_NOCOMP_DEFAULT 0x1 /* 1 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Block Floating Point support in decompressor. */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_ADDR 0x80 /* 128 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_MASK 0x2 /* 2 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_OFFSET 0x1 /* 1 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Block Scaling support in decompressor. */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BSC_ADDR 0x80 /* 128 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BSC_MASK 0x4 /* 4 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BSC_OFFSET 0x2 /* 2 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BSC_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BSC_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Mu-law support in decompressor. */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MU_ADDR 0x80 /* 128 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MU_MASK 0x8 /* 8 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MU_OFFSET 0x3 /* 3 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MU_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MU_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Modulation compression in core. */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_ADDR 0x80 /* 128 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_MASK 0x10 /* 16 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_OFFSET 0x4 /* 4 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Block Floating Point, selective RE send, support in decompressor. */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_SELRE_ADDR 0x80 /* 128 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_SELRE_MASK 0x20 /* 32 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_SELRE_OFFSET 0x5 /* 5 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_SELRE_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_SELRE_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Modulation compression, selective RE send, in core. */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_SELRE_ADDR 0x80 /* 128 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_SELRE_MASK 0x40 /* 64 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_SELRE_OFFSET 0x6 /* 6 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_SELRE_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODCOMP_SELRE_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Block Floating Point in core bit width support, in decompressor. E.g. Bit 9 = BFP9. */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_WIDTHS_ADDR 0x84 /* 132 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_WIDTHS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_WIDTHS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_WIDTHS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_BFP_WIDTHS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Modulation compression in core bit width support, in decompressor. E.g. Bit 1 = ModComp 1Bit. */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODC_WIDTHS_ADDR 0x84 /* 132 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODC_WIDTHS_MASK 0x3f0000 /* 4128768 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODC_WIDTHS_OFFSET 0x10 /* 16 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODC_WIDTHS_WIDTH 0x6 /* 6 */
#define CFG_CONFIG_XRAN_DECOMP_IN_CORE_MODC_WIDTHS_DEFAULT 0x0 /* 0 */

/* Type   = roInt  */
/* sigena =  */
/* Doc    : No Compression support in compressor. */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_NOCOMP_ADDR 0x88 /* 136 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_NOCOMP_MASK 0x10000 /* 65536 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_NOCOMP_OFFSET 0x10 /* 16 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_NOCOMP_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_NOCOMP_DEFAULT 0x1 /* 1 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Block Floating Point support in compressor. */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_ADDR 0x88 /* 136 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_MASK 0x20000 /* 131072 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_OFFSET 0x11 /* 17 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Block Floating Point, selective RE send, support in compressor. */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_SELRE_ADDR 0x88 /* 136 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_SELRE_MASK 0x200000 /* 2097152 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_SELRE_OFFSET 0x15 /* 21 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_SELRE_WIDTH 0x1 /* 1 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_SELRE_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Block Floating Point in core bit width support, in compressor. */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_WIDTHS_ADDR 0x8c /* 140 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_WIDTHS_MASK 0xffff /* 65535 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_WIDTHS_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_WIDTHS_WIDTH 0x10 /* 16 */
#define CFG_CONFIG_XRAN_COMP_IN_CORE_BFP_WIDTHS_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max width of the RU port ID field in bits. */
#define CFG_CONFIG_LIMIT_RU_I_W_ADDR 0x100 /* 256 */
#define CFG_CONFIG_LIMIT_RU_I_W_MASK 0xf /* 15 */
#define CFG_CONFIG_LIMIT_RU_I_W_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_LIMIT_RU_I_W_WIDTH 0x4 /* 4 */
#define CFG_CONFIG_LIMIT_RU_I_W_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max Width of the internal SS port selection bus in bits. */
#define CFG_CONFIG_LIMIT_RU_O_W_ADDR 0x104 /* 260 */
#define CFG_CONFIG_LIMIT_RU_O_W_MASK 0xf /* 15 */
#define CFG_CONFIG_LIMIT_RU_O_W_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_LIMIT_RU_O_W_WIDTH 0x4 /* 4 */
#define CFG_CONFIG_LIMIT_RU_O_W_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max Width of the internal Component Carrier selection bus in bits. */
#define CFG_CONFIG_LIMIT_CC_W_ADDR 0x108 /* 264 */
#define CFG_CONFIG_LIMIT_CC_W_MASK 0xf /* 15 */
#define CFG_CONFIG_LIMIT_CC_W_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_LIMIT_CC_W_WIDTH 0x4 /* 4 */
#define CFG_CONFIG_LIMIT_CC_W_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max Width of the internal Band Sector selection bus in bits. */
#define CFG_CONFIG_LIMIT_BS_W_ADDR 0x10c /* 268 */
#define CFG_CONFIG_LIMIT_BS_W_MASK 0xf /* 15 */
#define CFG_CONFIG_LIMIT_BS_W_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_LIMIT_BS_W_WIDTH 0x4 /* 4 */
#define CFG_CONFIG_LIMIT_BS_W_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Max Width of the internal DU port selection bus in bits. */
#define CFG_CONFIG_LIMIT_DU_W_ADDR 0x110 /* 272 */
#define CFG_CONFIG_LIMIT_DU_W_MASK 0xf /* 15 */
#define CFG_CONFIG_LIMIT_DU_W_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_LIMIT_DU_W_WIDTH 0x4 /* 4 */
#define CFG_CONFIG_LIMIT_DU_W_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Address width in bits of the MapTable. 0 = disabled. For example, 8 gives 256 map entries. */
#define CFG_CONFIG_MAP_TABLE_W_ADDR 0x114 /* 276 */
#define CFG_CONFIG_MAP_TABLE_W_MASK 0xf /* 15 */
#define CFG_CONFIG_MAP_TABLE_W_OFFSET 0x0 /* 0 */
#define CFG_CONFIG_MAP_TABLE_W_WIDTH 0x4 /* 4 */
#define CFG_CONFIG_MAP_TABLE_W_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Width in bits of the Stream entry used in the MapTable. */
#define CFG_CONFIG_MAP_STREAM_W_ADDR 0x114 /* 276 */
#define CFG_CONFIG_MAP_STREAM_W_MASK 0xf00 /* 3840 */
#define CFG_CONFIG_MAP_STREAM_W_OFFSET 0x8 /* 8 */
#define CFG_CONFIG_MAP_STREAM_W_WIDTH 0x4 /* 4 */
#define CFG_CONFIG_MAP_STREAM_W_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Width in bits of the Type entry used in the MapTable. */
#define CFG_CONFIG_MAP_TYPE_W_ADDR 0x114 /* 276 */
#define CFG_CONFIG_MAP_TYPE_W_MASK 0xf0000 /* 983040 */
#define CFG_CONFIG_MAP_TYPE_W_OFFSET 0x10 /* 16 */
#define CFG_CONFIG_MAP_TYPE_W_WIDTH 0x4 /* 4 */
#define CFG_CONFIG_MAP_TYPE_W_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Select the stream to count transactions on. Currently only 0-19, PDxCH Streams. */
#define CFG_MONITOR_SELECT_CNT_ADDR 0x800 /* 2048 */
#define CFG_MONITOR_SELECT_CNT_MASK 0x3f /* 63 */
#define CFG_MONITOR_SELECT_CNT_OFFSET 0x0 /* 0 */
#define CFG_MONITOR_SELECT_CNT_WIDTH 0x6 /* 6 */
#define CFG_MONITOR_SELECT_CNT_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Select the counter to read. 0=Sections, 1=Bytes. */
#define CFG_MONITOR_SELECT_READ_ADDR 0x804 /* 2052 */
#define CFG_MONITOR_SELECT_READ_MASK 0x3f /* 63 */
#define CFG_MONITOR_SELECT_READ_OFFSET 0x0 /* 0 */
#define CFG_MONITOR_SELECT_READ_WIDTH 0x6 /* 6 */
#define CFG_MONITOR_SELECT_READ_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Sample the counter values in their clock domain. Step one in reading the selected register. */
#define CFG_MONITOR_SNAPSHOT_ADDR 0x808 /* 2056 */
#define CFG_MONITOR_SNAPSHOT_MASK 0x1 /* 1 */
#define CFG_MONITOR_SNAPSHOT_OFFSET 0x0 /* 0 */
#define CFG_MONITOR_SNAPSHOT_WIDTH 0x1 /* 1 */
#define CFG_MONITOR_SNAPSHOT_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Sample the counter values in the AXI4-Lite clock domain. Step two in reading the selected register. */
#define CFG_MONITOR_SAMPLE_ADDR 0x80c /* 2060 */
#define CFG_MONITOR_SAMPLE_MASK 0x1 /* 1 */
#define CFG_MONITOR_SAMPLE_OFFSET 0x0 /* 0 */
#define CFG_MONITOR_SAMPLE_WIDTH 0x1 /* 1 */
#define CFG_MONITOR_SAMPLE_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Reset all the counters in the block. */
#define CFG_MONITOR_CLEAR_ADDR 0x810 /* 2064 */
#define CFG_MONITOR_CLEAR_MASK 0x1 /* 1 */
#define CFG_MONITOR_CLEAR_OFFSET 0x0 /* 0 */
#define CFG_MONITOR_CLEAR_WIDTH 0x1 /* 1 */
#define CFG_MONITOR_CLEAR_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Read value 31-0. Once the two sampling stages are complete, these registers can be read. */
#define CFG_MONITOR_READ_31__0_ADDR 0x820 /* 2080 */
#define CFG_MONITOR_READ_31__0_MASK 0xffffffff /* 4294967295 */
#define CFG_MONITOR_READ_31__0_OFFSET 0x0 /* 0 */
#define CFG_MONITOR_READ_31__0_WIDTH 0x20 /* 32 */
#define CFG_MONITOR_READ_31__0_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Read value 63-32. */
#define CFG_MONITOR_READ_63_32_ADDR 0x824 /* 2084 */
#define CFG_MONITOR_READ_63_32_MASK 0xffffffff /* 4294967295 */
#define CFG_MONITOR_READ_63_32_OFFSET 0x0 /* 0 */
#define CFG_MONITOR_READ_63_32_WIDTH 0x20 /* 32 */
#define CFG_MONITOR_READ_63_32_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v3_0_fram 
* with prefix fram_ @ address 0x2000
*------------------------------------------------------------------------------
*/
/* Type   = rwpdef  */
/* sigena =  */
/* Doc    : Framer Disable, if 1 flushes buffers and resets SN generation. 0 enables normal function. This is internally routed into the reset line driven by fram_reset. This means the external signal can be tied off and just driven from this register. */
#define FRAM_DISABLE_ADDR 0x2000 /* 8192 */
#define FRAM_DISABLE_MASK 0x1 /* 1 */
#define FRAM_DISABLE_OFFSET 0x0 /* 0 */
#define FRAM_DISABLE_WIDTH 0x1 /* 1 */
#define FRAM_DISABLE_DEFAULT 0x1 /* 1 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Framer Ready, if 1 the Framer is running and generating Ethernet packets. */
#define FRAM_READY_ADDR 0x2000 /* 8192 */
#define FRAM_READY_MASK 0x2 /* 2 */
#define FRAM_READY_OFFSET 0x1 /* 1 */
#define FRAM_READY_WIDTH 0x1 /* 1 */
#define FRAM_READY_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  */
/* sigena =  */
/* Doc    : This enables a per symbol reset in the framer path. This can be used when the upstream logic does not respond to UL requests. Enabling this can limit the bandwidth on UL. It is recommended that the upstream logic responds to all requests from the oDU. */
#define FRAM_ENABLE_PER_SYM_RESET_ADDR 0x2004 /* 8196 */
#define FRAM_ENABLE_PER_SYM_RESET_MASK 0x1 /* 1 */
#define FRAM_ENABLE_PER_SYM_RESET_OFFSET 0x0 /* 0 */
#define FRAM_ENABLE_PER_SYM_RESET_WIDTH 0x1 /* 1 */
#define FRAM_ENABLE_PER_SYM_RESET_DEFAULT 0x1 /* 1 */

/* Type   = rwpdef  */
/* sigena =  */
/* Doc    : This register allows the UL MTU side to be reduced. This defaults to the value set in the GUI. This is also available in the register config_xran_fram_eth_pkt_max. This can be used to force the framer to generate smaller packets, however, its use is not recommended as it results in non optimal traffic with extra framing overhead. The size output may not be the max MTU requested. It may be smaller depending on the the number of sections. This cannot be set greater than config_xran_fram_eth_pkt_max. The min value supported is 256. */
#define FRAM_MTU_SIZE_ADDR 0x2008 /* 8200 */
#define FRAM_MTU_SIZE_MASK 0x3fff /* 16383 */
#define FRAM_MTU_SIZE_OFFSET 0x0 /* 0 */
#define FRAM_MTU_SIZE_WIDTH 0xe /* 14 */
#define FRAM_MTU_SIZE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : This register allows the selection of RtPcid bits to be used in the UL FLow ID. This table is 11 bits wide, giving 2048 individual FlowId streams. The RtPcid field is 16 bits. This register allows a preset bit selection from these 16 bits to generate the 11-bit address. This register is new from IP version 3_0, however the default value of 0 retains the same mapping as previous releases. */
#define FRAM_CID_SEQTABLE_MODE_ADDR 0x200c /* 8204 */
#define FRAM_CID_SEQTABLE_MODE_MASK 0xf /* 15 */
#define FRAM_CID_SEQTABLE_MODE_OFFSET 0x0 /* 0 */
#define FRAM_CID_SEQTABLE_MODE_WIDTH 0x4 /* 4 */
#define FRAM_CID_SEQTABLE_MODE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : 0=eCPRI, 1=IEEE 1914.3, 2-15=Reserved */
#define FRAM_PROTOCOL_DEFINITION_ADDR 0x2200 /* 8704 */
#define FRAM_PROTOCOL_DEFINITION_MASK 0xf /* 15 */
#define FRAM_PROTOCOL_DEFINITION_OFFSET 0x0 /* 0 */
#define FRAM_PROTOCOL_DEFINITION_WIDTH 0x4 /* 4 */
#define FRAM_PROTOCOL_DEFINITION_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : If 1 then VLAN TAG added to packet. Note if set the deframer expects packets to be VLAN tagged. */
#define FRAM_GEN_VLAN_TAG_ADDR 0x2200 /* 8704 */
#define FRAM_GEN_VLAN_TAG_MASK 0x10 /* 16 */
#define FRAM_GEN_VLAN_TAG_OFFSET 0x4 /* 4 */
#define FRAM_GEN_VLAN_TAG_WIDTH 0x1 /* 1 */
#define FRAM_GEN_VLAN_TAG_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : 0 = Raw, 1 = IPv4 Header, 3 = IPv6 */
#define FRAM_SEL_IPV_ADDRESS_TYPE_ADDR 0x2200 /* 8704 */
#define FRAM_SEL_IPV_ADDRESS_TYPE_MASK 0x60 /* 96 */
#define FRAM_SEL_IPV_ADDRESS_TYPE_OFFSET 0x5 /* 5 */
#define FRAM_SEL_IPV_ADDRESS_TYPE_WIDTH 0x2 /* 2 */
#define FRAM_SEL_IPV_ADDRESS_TYPE_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v3_0_defm 
* with prefix defm_ @ address 0x6000
*------------------------------------------------------------------------------
*/
/* Type   = rw  */
/* sigena =  */
/* Doc    : DeFramer Restart. If 1 empties all buffers and resets Write and Read pointers. 0 enables normal function. This is internally routed into the reset line driven by defm_reset. This means the external signal can be tied off and just driven from this register. */
#define DEFM_RESTART_ADDR 0x6000 /* 24576 */
#define DEFM_RESTART_MASK 0x1 /* 1 */
#define DEFM_RESTART_OFFSET 0x0 /* 0 */
#define DEFM_RESTART_WIDTH 0x1 /* 1 */
#define DEFM_RESTART_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : If 1 the DeFramer is running, and no buffer is either empty or full */
#define DEFM_READY_ADDR 0x6000 /* 24576 */
#define DEFM_READY_MASK 0x2 /* 2 */
#define DEFM_READY_OFFSET 0x1 /* 1 */
#define DEFM_READY_WIDTH 0x1 /* 1 */
#define DEFM_READY_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : If 11 then IP discards all packets marked as bad by MAC rx_axis_tuser=1 else if 01 then discard only those packets holding rx_axis_tuser = 1 and stat_rx_packet_bad_fcs = 0 */
#define DEFM_ERR_PACKET_FILTER_ADDR 0x6004 /* 24580 */
#define DEFM_ERR_PACKET_FILTER_MASK 0x3 /* 3 */
#define DEFM_ERR_PACKET_FILTER_OFFSET 0x0 /* 0 */
#define DEFM_ERR_PACKET_FILTER_WIDTH 0x2 /* 2 */
#define DEFM_ERR_PACKET_FILTER_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Use external symbol strobe in DEFM and FRAM paths. If 1 Uses the external Strobe. 0 Uses internally generated strobe. Must be enabled via GUI */
#define DEFM_USE_ONE_SYMBOL_STROBE_ADDR 0x6008 /* 24584 */
#define DEFM_USE_ONE_SYMBOL_STROBE_MASK 0x1 /* 1 */
#define DEFM_USE_ONE_SYMBOL_STROBE_OFFSET 0x0 /* 0 */
#define DEFM_USE_ONE_SYMBOL_STROBE_WIDTH 0x1 /* 1 */
#define DEFM_USE_ONE_SYMBOL_STROBE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : NODOC - Internal debug register. Use for trouble shooting */
#define DEFM_DEBUG_ADDR 0x600c /* 24588 */
#define DEFM_DEBUG_MASK 0xf /* 15 */
#define DEFM_DEBUG_OFFSET 0x0 /* 0 */
#define DEFM_DEBUG_WIDTH 0x4 /* 4 */
#define DEFM_DEBUG_DEFAULT 0x1 /* 1 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Take a snapshot of the stats counters. Optionally apply a ss_reset field to reset required counters for a specific Ethernet Lane. */
#define DEFM_SNAP_SHOT_ADDR 0x6010 /* 24592 */
#define DEFM_SNAP_SHOT_MASK 0x1 /* 1 */
#define DEFM_SNAP_SHOT_OFFSET 0x0 /* 0 */
#define DEFM_SNAP_SHOT_WIDTH 0x1 /* 1 */
#define DEFM_SNAP_SHOT_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 0, reset ethernet stats         */
#define DEFM_CTRL_SS_RESET_E0_E_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E0_E_MASK 0x10 /* 16 */
#define DEFM_CTRL_SS_RESET_E0_E_OFFSET 0x4 /* 4 */
#define DEFM_CTRL_SS_RESET_E0_E_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E0_E_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 0, reset rx_windows monitors    */
#define DEFM_CTRL_SS_RESET_E0_RWIN_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E0_RWIN_MASK 0x20 /* 32 */
#define DEFM_CTRL_SS_RESET_E0_RWIN_OFFSET 0x5 /* 5 */
#define DEFM_CTRL_SS_RESET_E0_RWIN_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E0_RWIN_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 0, reset rx_corrupted counters  */
#define DEFM_CTRL_SS_RESET_E0_RCOR_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E0_RCOR_MASK 0x40 /* 64 */
#define DEFM_CTRL_SS_RESET_E0_RCOR_OFFSET 0x6 /* 6 */
#define DEFM_CTRL_SS_RESET_E0_RCOR_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E0_RCOR_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 0, reset tx_counters            */
#define DEFM_CTRL_SS_RESET_E0_T_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E0_T_MASK 0x80 /* 128 */
#define DEFM_CTRL_SS_RESET_E0_T_OFFSET 0x7 /* 7 */
#define DEFM_CTRL_SS_RESET_E0_T_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E0_T_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 1, reset ethernet stats         */
#define DEFM_CTRL_SS_RESET_E1_E_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E1_E_MASK 0x100 /* 256 */
#define DEFM_CTRL_SS_RESET_E1_E_OFFSET 0x8 /* 8 */
#define DEFM_CTRL_SS_RESET_E1_E_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E1_E_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 1, reset rx_windows monitors    */
#define DEFM_CTRL_SS_RESET_E1_RWIN_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E1_RWIN_MASK 0x200 /* 512 */
#define DEFM_CTRL_SS_RESET_E1_RWIN_OFFSET 0x9 /* 9 */
#define DEFM_CTRL_SS_RESET_E1_RWIN_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E1_RWIN_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 1, reset rx_corrupted counters  */
#define DEFM_CTRL_SS_RESET_E1_RCOR_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E1_RCOR_MASK 0x400 /* 1024 */
#define DEFM_CTRL_SS_RESET_E1_RCOR_OFFSET 0xa /* 10 */
#define DEFM_CTRL_SS_RESET_E1_RCOR_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E1_RCOR_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 1, reset tx_counters            */
#define DEFM_CTRL_SS_RESET_E1_T_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E1_T_MASK 0x800 /* 2048 */
#define DEFM_CTRL_SS_RESET_E1_T_OFFSET 0xb /* 11 */
#define DEFM_CTRL_SS_RESET_E1_T_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E1_T_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 2, reset ethernet stats         */
#define DEFM_CTRL_SS_RESET_E2_E_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E2_E_MASK 0x1000 /* 4096 */
#define DEFM_CTRL_SS_RESET_E2_E_OFFSET 0xc /* 12 */
#define DEFM_CTRL_SS_RESET_E2_E_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E2_E_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 2, reset rx_windows monitors    */
#define DEFM_CTRL_SS_RESET_E2_RWIN_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E2_RWIN_MASK 0x2000 /* 8192 */
#define DEFM_CTRL_SS_RESET_E2_RWIN_OFFSET 0xd /* 13 */
#define DEFM_CTRL_SS_RESET_E2_RWIN_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E2_RWIN_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 2, reset rx_corrupted counters  */
#define DEFM_CTRL_SS_RESET_E2_RCOR_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E2_RCOR_MASK 0x4000 /* 16384 */
#define DEFM_CTRL_SS_RESET_E2_RCOR_OFFSET 0xe /* 14 */
#define DEFM_CTRL_SS_RESET_E2_RCOR_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E2_RCOR_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 2, reset tx_counters            */
#define DEFM_CTRL_SS_RESET_E2_T_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E2_T_MASK 0x8000 /* 32768 */
#define DEFM_CTRL_SS_RESET_E2_T_OFFSET 0xf /* 15 */
#define DEFM_CTRL_SS_RESET_E2_T_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E2_T_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 3, reset ethernet stats         */
#define DEFM_CTRL_SS_RESET_E3_E_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E3_E_MASK 0x10000 /* 65536 */
#define DEFM_CTRL_SS_RESET_E3_E_OFFSET 0x10 /* 16 */
#define DEFM_CTRL_SS_RESET_E3_E_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E3_E_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 3, reset rx_windows monitors    */
#define DEFM_CTRL_SS_RESET_E3_RWIN_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E3_RWIN_MASK 0x20000 /* 131072 */
#define DEFM_CTRL_SS_RESET_E3_RWIN_OFFSET 0x11 /* 17 */
#define DEFM_CTRL_SS_RESET_E3_RWIN_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E3_RWIN_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 3, reset rx_corrupted counters  */
#define DEFM_CTRL_SS_RESET_E3_RCOR_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E3_RCOR_MASK 0x40000 /* 262144 */
#define DEFM_CTRL_SS_RESET_E3_RCOR_OFFSET 0x12 /* 18 */
#define DEFM_CTRL_SS_RESET_E3_RCOR_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E3_RCOR_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Eth Lane 3, reset tx_counters            */
#define DEFM_CTRL_SS_RESET_E3_T_ADDR 0x6010 /* 24592 */
#define DEFM_CTRL_SS_RESET_E3_T_MASK 0x80000 /* 524288 */
#define DEFM_CTRL_SS_RESET_E3_T_OFFSET 0x13 /* 19 */
#define DEFM_CTRL_SS_RESET_E3_T_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_SS_RESET_E3_T_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : When 1,the ORAN Corrupt count will increment on eAxC that are not processed by the IP. These may used internally for SRS. Real eAxC errors will not be counted when low as the IP has no method to distinguish these, unless mapping mode is used. */
#define DEFM_CTRL_ENA_CRUPT_EAXE_CNT_ADDR 0x6014 /* 24596 */
#define DEFM_CTRL_ENA_CRUPT_EAXE_CNT_MASK 0x1 /* 1 */
#define DEFM_CTRL_ENA_CRUPT_EAXE_CNT_OFFSET 0x0 /* 0 */
#define DEFM_CTRL_ENA_CRUPT_EAXE_CNT_WIDTH 0x1 /* 1 */
#define DEFM_CTRL_ENA_CRUPT_EAXE_CNT_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Component Carrier right bit shift to apply. 8 implies the CCID starts at bit 8, 4 bit 4 etc. */
#define DEFM_CID_CC_SHIFT_ADDR 0x6020 /* 24608 */
#define DEFM_CID_CC_SHIFT_MASK 0xf /* 15 */
#define DEFM_CID_CC_SHIFT_OFFSET 0x0 /* 0 */
#define DEFM_CID_CC_SHIFT_WIDTH 0x4 /* 4 */
#define DEFM_CID_CC_SHIFT_DEFAULT 0x8 /* 8 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Component Carrier Mask bits to apply. 0x7 implies 3 bits in the CCID field. */
#define DEFM_CID_CC_MASK_ADDR 0x6024 /* 24612 */
#define DEFM_CID_CC_MASK_MASK 0x3f /* 63 */
#define DEFM_CID_CC_MASK_OFFSET 0x0 /* 0 */
#define DEFM_CID_CC_MASK_WIDTH 0x6 /* 6 */
#define DEFM_CID_CC_MASK_DEFAULT 0x7 /* 7 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Band Sector right bit shift to apply. 11 implies the BSID starts at bit 11, 8 bit 8 etc. */
#define DEFM_CID_BS_SHIFT_ADDR 0x6028 /* 24616 */
#define DEFM_CID_BS_SHIFT_MASK 0xf /* 15 */
#define DEFM_CID_BS_SHIFT_OFFSET 0x0 /* 0 */
#define DEFM_CID_BS_SHIFT_WIDTH 0x4 /* 4 */
#define DEFM_CID_BS_SHIFT_DEFAULT 0xb /* 11 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Band Sector Mask bits to apply */
#define DEFM_CID_BS_MASK_ADDR 0x602c /* 24620 */
#define DEFM_CID_BS_MASK_MASK 0x3f /* 63 */
#define DEFM_CID_BS_MASK_OFFSET 0x0 /* 0 */
#define DEFM_CID_BS_MASK_WIDTH 0x6 /* 6 */
#define DEFM_CID_BS_MASK_DEFAULT 0x1 /* 1 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : DU right bit shift to apply. 12 implies the DUID starts at bit 12, 10 bit 10 etc */
#define DEFM_CID_DU_SHIFT_ADDR 0x6030 /* 24624 */
#define DEFM_CID_DU_SHIFT_MASK 0xf /* 15 */
#define DEFM_CID_DU_SHIFT_OFFSET 0x0 /* 0 */
#define DEFM_CID_DU_SHIFT_WIDTH 0x4 /* 4 */
#define DEFM_CID_DU_SHIFT_DEFAULT 0xc /* 12 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : DU Mask bits to apply */
#define DEFM_CID_DU_MASK_ADDR 0x6034 /* 24628 */
#define DEFM_CID_DU_MASK_MASK 0x3f /* 63 */
#define DEFM_CID_DU_MASK_OFFSET 0x0 /* 0 */
#define DEFM_CID_DU_MASK_WIDTH 0x6 /* 6 */
#define DEFM_CID_DU_MASK_DEFAULT 0xf /* 15 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : RU_Port_ID Mask bits to apply */
#define DEFM_CID_SS_MASK_ADDR 0x6038 /* 24632 */
#define DEFM_CID_SS_MASK_MASK 0xff /* 255 */
#define DEFM_CID_SS_MASK_OFFSET 0x0 /* 0 */
#define DEFM_CID_SS_MASK_WIDTH 0x8 /* 8 */
#define DEFM_CID_SS_MASK_DEFAULT 0x1f /* 31 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Mask to apply for user data selection */
#define DEFM_CID_U_MASK_ADDR 0x603c /* 24636 */
#define DEFM_CID_U_MASK_MASK 0xff /* 255 */
#define DEFM_CID_U_MASK_OFFSET 0x0 /* 0 */
#define DEFM_CID_U_MASK_WIDTH 0x8 /* 8 */
#define DEFM_CID_U_MASK_DEFAULT 0xc0 /* 192 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Value to test against MASKed field to determine User/Prach/SSB/LTE */
#define DEFM_CID_U_VALUE_ADDR 0x6040 /* 24640 */
#define DEFM_CID_U_VALUE_MASK 0xff /* 255 */
#define DEFM_CID_U_VALUE_OFFSET 0x0 /* 0 */
#define DEFM_CID_U_VALUE_WIDTH 0x8 /* 8 */
#define DEFM_CID_U_VALUE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Mask to apply for Prach selection */
#define DEFM_CID_PRACH_MASK_ADDR 0x6044 /* 24644 */
#define DEFM_CID_PRACH_MASK_MASK 0xff /* 255 */
#define DEFM_CID_PRACH_MASK_OFFSET 0x0 /* 0 */
#define DEFM_CID_PRACH_MASK_WIDTH 0x8 /* 8 */
#define DEFM_CID_PRACH_MASK_DEFAULT 0xc0 /* 192 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Value to test against MASKed field to determine User/Prach/SSB/LTE */
#define DEFM_CID_PRACH_VALUE_ADDR 0x6048 /* 24648 */
#define DEFM_CID_PRACH_VALUE_MASK 0xff /* 255 */
#define DEFM_CID_PRACH_VALUE_OFFSET 0x0 /* 0 */
#define DEFM_CID_PRACH_VALUE_WIDTH 0x8 /* 8 */
#define DEFM_CID_PRACH_VALUE_DEFAULT 0x80 /* 128 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Mask to apply for SSB selection. Set to 0x0 to disable SSB streams in non-table mode. */
#define DEFM_CID_SSB_MASK_ADDR 0x604c /* 24652 */
#define DEFM_CID_SSB_MASK_MASK 0xff /* 255 */
#define DEFM_CID_SSB_MASK_OFFSET 0x0 /* 0 */
#define DEFM_CID_SSB_MASK_WIDTH 0x8 /* 8 */
#define DEFM_CID_SSB_MASK_DEFAULT 0xc0 /* 192 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Value to test against MASKed field to determine User/Prach/SSB/LTE */
#define DEFM_CID_SSB_VALUE_ADDR 0x6050 /* 24656 */
#define DEFM_CID_SSB_VALUE_MASK 0xff /* 255 */
#define DEFM_CID_SSB_VALUE_OFFSET 0x0 /* 0 */
#define DEFM_CID_SSB_VALUE_WIDTH 0x8 /* 8 */
#define DEFM_CID_SSB_VALUE_DEFAULT 0x40 /* 64 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Mask to apply for LTE selection. Set to 0x0 to disable LTE streams in non-table mode. */
#define DEFM_CID_LTE_MASK_ADDR 0x6054 /* 24660 */
#define DEFM_CID_LTE_MASK_MASK 0xff /* 255 */
#define DEFM_CID_LTE_MASK_OFFSET 0x0 /* 0 */
#define DEFM_CID_LTE_MASK_WIDTH 0x8 /* 8 */
#define DEFM_CID_LTE_MASK_DEFAULT 0xc0 /* 192 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Value to test against MASKed field to determine User/Prach/SSB/LTE */
#define DEFM_CID_LTE_VALUE_ADDR 0x6058 /* 24664 */
#define DEFM_CID_LTE_VALUE_MASK 0xff /* 255 */
#define DEFM_CID_LTE_VALUE_OFFSET 0x0 /* 0 */
#define DEFM_CID_LTE_VALUE_WIDTH 0x8 /* 8 */
#define DEFM_CID_LTE_VALUE_DEFAULT 0xc0 /* 192 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Packet filter for message Word 0 at ETHX port */
#define DEFM_USER_DATA_FILTER_W0_31_0_ADDR 0x6100 /* 24832 */
#define DEFM_USER_DATA_FILTER_W0_31_0_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W0_31_0_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W0_31_0_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W0_31_0_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Packet filter for message Word 0 at ETHX port */
#define DEFM_USER_DATA_FILTER_W0_63_32_ADDR 0x6104 /* 24836 */
#define DEFM_USER_DATA_FILTER_W0_63_32_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W0_63_32_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W0_63_32_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W0_63_32_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Packet filter for message Word 0 at ETHX port */
#define DEFM_USER_DATA_FILTER_W0_95_64_ADDR 0x6108 /* 24840 */
#define DEFM_USER_DATA_FILTER_W0_95_64_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W0_95_64_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W0_95_64_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W0_95_64_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Packet filter for message Word 0 at ETHX port */
#define DEFM_USER_DATA_FILTER_W0_127_96_ADDR 0x610c /* 24844 */
#define DEFM_USER_DATA_FILTER_W0_127_96_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W0_127_96_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W0_127_96_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W0_127_96_DEFAULT 0xfffffeae /* 4294966958 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : If (bit_X == 1) then 1st word byte_X must not be checked */
#define DEFM_USER_DATA_FILTER_W0_MASK_ADDR 0x6110 /* 24848 */
#define DEFM_USER_DATA_FILTER_W0_MASK_MASK 0xffff /* 65535 */
#define DEFM_USER_DATA_FILTER_W0_MASK_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W0_MASK_WIDTH 0x10 /* 16 */
#define DEFM_USER_DATA_FILTER_W0_MASK_DEFAULT 0xcfff /* 53247 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Packet filter for message Word 1 at ETHX port */
#define DEFM_USER_DATA_FILTER_W1_31_0_ADDR 0x6120 /* 24864 */
#define DEFM_USER_DATA_FILTER_W1_31_0_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W1_31_0_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W1_31_0_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W1_31_0_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : Packet filter for message Word 1 at ETHX port */
#define DEFM_USER_DATA_FILTER_W1_63_32_ADDR 0x6124 /* 24868 */
#define DEFM_USER_DATA_FILTER_W1_63_32_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W1_63_32_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W1_63_32_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W1_63_32_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : Packet filter for message Word 1 at ETHX port */
#define DEFM_USER_DATA_FILTER_W1_95_64_ADDR 0x6128 /* 24872 */
#define DEFM_USER_DATA_FILTER_W1_95_64_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W1_95_64_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W1_95_64_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W1_95_64_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : Packet filter for message Word 1 at ETHX port */
#define DEFM_USER_DATA_FILTER_W1_127_96_ADDR 0x612c /* 24876 */
#define DEFM_USER_DATA_FILTER_W1_127_96_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W1_127_96_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W1_127_96_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W1_127_96_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : If (bit_X == 1) then 2nd word byte_X must not be checked */
#define DEFM_USER_DATA_FILTER_W1_MASK_ADDR 0x6130 /* 24880 */
#define DEFM_USER_DATA_FILTER_W1_MASK_MASK 0xffff /* 65535 */
#define DEFM_USER_DATA_FILTER_W1_MASK_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W1_MASK_WIDTH 0x10 /* 16 */
#define DEFM_USER_DATA_FILTER_W1_MASK_DEFAULT 0xffff /* 65535 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : Packet filter for message Word 2 at ETHX port */
#define DEFM_USER_DATA_FILTER_W2_31_0_ADDR 0x6140 /* 24896 */
#define DEFM_USER_DATA_FILTER_W2_31_0_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W2_31_0_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W2_31_0_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W2_31_0_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : Packet filter for message Word 2 at ETHX port */
#define DEFM_USER_DATA_FILTER_W2_63_32_ADDR 0x6144 /* 24900 */
#define DEFM_USER_DATA_FILTER_W2_63_32_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W2_63_32_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W2_63_32_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W2_63_32_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : Packet filter for message Word 2 at ETHX port */
#define DEFM_USER_DATA_FILTER_W2_95_64_ADDR 0x6148 /* 24904 */
#define DEFM_USER_DATA_FILTER_W2_95_64_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W2_95_64_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W2_95_64_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W2_95_64_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : Packet filter for message Word 2 at ETHX port */
#define DEFM_USER_DATA_FILTER_W2_127_96_ADDR 0x614c /* 24908 */
#define DEFM_USER_DATA_FILTER_W2_127_96_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W2_127_96_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W2_127_96_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W2_127_96_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : If (bit_X == 1) then 3rd word byte_X must not be checked */
#define DEFM_USER_DATA_FILTER_W2_MASK_ADDR 0x6150 /* 24912 */
#define DEFM_USER_DATA_FILTER_W2_MASK_MASK 0xffff /* 65535 */
#define DEFM_USER_DATA_FILTER_W2_MASK_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W2_MASK_WIDTH 0x10 /* 16 */
#define DEFM_USER_DATA_FILTER_W2_MASK_DEFAULT 0xffff /* 65535 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : Packet filter for message Word 3 at ETHX port */
#define DEFM_USER_DATA_FILTER_W3_31_0_ADDR 0x6160 /* 24928 */
#define DEFM_USER_DATA_FILTER_W3_31_0_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W3_31_0_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W3_31_0_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W3_31_0_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : Packet filter for message Word 3 at ETHX port */
#define DEFM_USER_DATA_FILTER_W3_63_32_ADDR 0x6164 /* 24932 */
#define DEFM_USER_DATA_FILTER_W3_63_32_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W3_63_32_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W3_63_32_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W3_63_32_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : Packet filter for message Word 3 at ETHX port */
#define DEFM_USER_DATA_FILTER_W3_95_64_ADDR 0x6168 /* 24936 */
#define DEFM_USER_DATA_FILTER_W3_95_64_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W3_95_64_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W3_95_64_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W3_95_64_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : Packet filter for message Word 3 at ETHX port */
#define DEFM_USER_DATA_FILTER_W3_127_96_ADDR 0x616c /* 24940 */
#define DEFM_USER_DATA_FILTER_W3_127_96_MASK 0xffffffff /* 4294967295 */
#define DEFM_USER_DATA_FILTER_W3_127_96_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W3_127_96_WIDTH 0x20 /* 32 */
#define DEFM_USER_DATA_FILTER_W3_127_96_DEFAULT 0xffffffff /* 4294967295 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_wide_filt */
/* Doc    : If (bit_X == 1) then 4th word byte_X must not be checked */
#define DEFM_USER_DATA_FILTER_W3_MASK_ADDR 0x6170 /* 24944 */
#define DEFM_USER_DATA_FILTER_W3_MASK_MASK 0xffff /* 65535 */
#define DEFM_USER_DATA_FILTER_W3_MASK_OFFSET 0x0 /* 0 */
#define DEFM_USER_DATA_FILTER_W3_MASK_WIDTH 0x10 /* 16 */
#define DEFM_USER_DATA_FILTER_W3_MASK_DEFAULT 0xffff /* 65535 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Set the full table mapping mode. The mode determines what bits from the RtPcid are used for the address. 1={Direction,RU_Port_ID} (Default), 2={Direction,Band Sector,CC ID,RU_Port_ID}, 3={Direction,Band Sector,RU_Port_ID}  */
#define DEFM_CID_MAP_MODE_ADDR 0x6900 /* 26880 */
#define DEFM_CID_MAP_MODE_MASK 0x3 /* 3 */
#define DEFM_CID_MAP_MODE_OFFSET 0x0 /* 0 */
#define DEFM_CID_MAP_MODE_WIDTH 0x2 /* 2 */
#define DEFM_CID_MAP_MODE_DEFAULT 0x1 /* 1 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : SubMode Select. Controls bits selected to form the address to the Map Table. */
#define DEFM_CID_MAP_SUBMODE_ADDR 0x6900 /* 26880 */
#define DEFM_CID_MAP_SUBMODE_MASK 0xf00 /* 3840 */
#define DEFM_CID_MAP_SUBMODE_OFFSET 0x8 /* 8 */
#define DEFM_CID_MAP_SUBMODE_WIDTH 0x4 /* 4 */
#define DEFM_CID_MAP_SUBMODE_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Write the stream information to the table when high. Auto cleared */
#define DEFM_CID_MAP_WR_STROBE_ADDR 0x6904 /* 26884 */
#define DEFM_CID_MAP_WR_STROBE_MASK 0x80000000 /* 2147483648 */
#define DEFM_CID_MAP_WR_STROBE_OFFSET 0x1f /* 31 */
#define DEFM_CID_MAP_WR_STROBE_WIDTH 0x1 /* 1 */
#define DEFM_CID_MAP_WR_STROBE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Set the internal stream CC ID. */
#define DEFM_CID_MAP_WR_STREAM_CCID_ADDR 0x6904 /* 26884 */
#define DEFM_CID_MAP_WR_STREAM_CCID_MASK 0x7000000 /* 117440512 */
#define DEFM_CID_MAP_WR_STREAM_CCID_OFFSET 0x18 /* 24 */
#define DEFM_CID_MAP_WR_STREAM_CCID_WIDTH 0x3 /* 3 */
#define DEFM_CID_MAP_WR_STREAM_CCID_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Set the internal stream Port ID. All IP streams are indexed from 0 to the MaxNumOfStreamsForType */
#define DEFM_CID_MAP_WR_STREAM_PORTID_ADDR 0x6904 /* 26884 */
#define DEFM_CID_MAP_WR_STREAM_PORTID_MASK 0x7c0000 /* 8126464 */
#define DEFM_CID_MAP_WR_STREAM_PORTID_OFFSET 0x12 /* 18 */
#define DEFM_CID_MAP_WR_STREAM_PORTID_WIDTH 0x5 /* 5 */
#define DEFM_CID_MAP_WR_STREAM_PORTID_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Set the internal stream Type. The IP interpreted types are shown below. Other types can be used to identify other streams not processed by the IP. IP UNKNOWN = All ones UserDefined = 5 to (Max-1), IP LTE = 4, IP PRACH = 3, IP SSB = 2, IP PUXCH = 1, IP PDXCH = 0 */
#define DEFM_CID_MAP_WR_STREAM_TYPE_ADDR 0x6904 /* 26884 */
#define DEFM_CID_MAP_WR_STREAM_TYPE_MASK 0x7000 /* 28672 */
#define DEFM_CID_MAP_WR_STREAM_TYPE_OFFSET 0xc /* 12 */
#define DEFM_CID_MAP_WR_STREAM_TYPE_WIDTH 0x3 /* 3 */
#define DEFM_CID_MAP_WR_STREAM_TYPE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Table address to use. This must be in the bit range defined in the GUI. */
#define DEFM_CID_MAP_WR_TABLE_ADDR_ADDR 0x6904 /* 26884 */
#define DEFM_CID_MAP_WR_TABLE_ADDR_MASK 0x7ff /* 2047 */
#define DEFM_CID_MAP_WR_TABLE_ADDR_OFFSET 0x0 /* 0 */
#define DEFM_CID_MAP_WR_TABLE_ADDR_WIDTH 0xb /* 11 */
#define DEFM_CID_MAP_WR_TABLE_ADDR_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Read the stream information to the table when high. Auto cleared */
#define DEFM_CID_MAP_RD_STROBE_ADDR 0x6908 /* 26888 */
#define DEFM_CID_MAP_RD_STROBE_MASK 0x80000000 /* 2147483648 */
#define DEFM_CID_MAP_RD_STROBE_OFFSET 0x1f /* 31 */
#define DEFM_CID_MAP_RD_STROBE_WIDTH 0x1 /* 1 */
#define DEFM_CID_MAP_RD_STROBE_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Read the internal stream CC ID. */
#define DEFM_CID_MAP_RD_STREAM_CCID_ADDR 0x6908 /* 26888 */
#define DEFM_CID_MAP_RD_STREAM_CCID_MASK 0x7000000 /* 117440512 */
#define DEFM_CID_MAP_RD_STREAM_CCID_OFFSET 0x18 /* 24 */
#define DEFM_CID_MAP_RD_STREAM_CCID_WIDTH 0x3 /* 3 */
#define DEFM_CID_MAP_RD_STREAM_CCID_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Holds the stream portId from the last read table address */
#define DEFM_CID_MAP_RD_STREAM_PORTID_ADDR 0x6908 /* 26888 */
#define DEFM_CID_MAP_RD_STREAM_PORTID_MASK 0x7c0000 /* 8126464 */
#define DEFM_CID_MAP_RD_STREAM_PORTID_OFFSET 0x12 /* 18 */
#define DEFM_CID_MAP_RD_STREAM_PORTID_WIDTH 0x5 /* 5 */
#define DEFM_CID_MAP_RD_STREAM_PORTID_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Holds the stream type from the last read table address */
#define DEFM_CID_MAP_RD_STREAM_TYPE_ADDR 0x6908 /* 26888 */
#define DEFM_CID_MAP_RD_STREAM_TYPE_MASK 0x7000 /* 28672 */
#define DEFM_CID_MAP_RD_STREAM_TYPE_OFFSET 0xc /* 12 */
#define DEFM_CID_MAP_RD_STREAM_TYPE_WIDTH 0x3 /* 3 */
#define DEFM_CID_MAP_RD_STREAM_TYPE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Table address to use */
#define DEFM_CID_MAP_RD_TABLE_ADDR_ADDR 0x6908 /* 26888 */
#define DEFM_CID_MAP_RD_TABLE_ADDR_MASK 0x7ff /* 2047 */
#define DEFM_CID_MAP_RD_TABLE_ADDR_OFFSET 0x0 /* 0 */
#define DEFM_CID_MAP_RD_TABLE_ADDR_WIDTH 0xb /* 11 */
#define DEFM_CID_MAP_RD_TABLE_ADDR_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Write the PUxCH Static Spatial Stream Decompression (PSSSD) settings high. Auto cleared */
#define DEFM_DECOMP_SS_WR_ADDR 0x6910 /* 26896 */
#define DEFM_DECOMP_SS_WR_MASK 0x80000000 /* 2147483648 */
#define DEFM_DECOMP_SS_WR_OFFSET 0x1f /* 31 */
#define DEFM_DECOMP_SS_WR_WIDTH 0x1 /* 1 */
#define DEFM_DECOMP_SS_WR_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable the PSSSD mode, if Static decompression enabled for PDxCH */
#define DEFM_DECOMP_SS_MODE_ENABLE_ADDR 0x6910 /* 26896 */
#define DEFM_DECOMP_SS_MODE_ENABLE_MASK 0x10000 /* 65536 */
#define DEFM_DECOMP_SS_MODE_ENABLE_OFFSET 0x10 /* 16 */
#define DEFM_DECOMP_SS_MODE_ENABLE_WIDTH 0x1 /* 1 */
#define DEFM_DECOMP_SS_MODE_ENABLE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Set the PSSSD decompression Type. 0=NoCompression, 1=BFP, 4=ModComp */
#define DEFM_DECOMP_SS_UDCOMP_TYPE_ADDR 0x6910 /* 26896 */
#define DEFM_DECOMP_SS_UDCOMP_TYPE_MASK 0xf000 /* 61440 */
#define DEFM_DECOMP_SS_UDCOMP_TYPE_OFFSET 0xc /* 12 */
#define DEFM_DECOMP_SS_UDCOMP_TYPE_WIDTH 0x4 /* 4 */
#define DEFM_DECOMP_SS_UDCOMP_TYPE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Set the width of the PSSSD decompression */
#define DEFM_DECOMP_SS_UDCOMP_WIDTH_ADDR 0x6910 /* 26896 */
#define DEFM_DECOMP_SS_UDCOMP_WIDTH_MASK 0xf00 /* 3840 */
#define DEFM_DECOMP_SS_UDCOMP_WIDTH_OFFSET 0x8 /* 8 */
#define DEFM_DECOMP_SS_UDCOMP_WIDTH_WIDTH 0x4 /* 4 */
#define DEFM_DECOMP_SS_UDCOMP_WIDTH_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Set the Spatial Stream address. This should be in the valid range of PDxCH Spatial Streams */
#define DEFM_DECOMP_SS_ADDRESS_ADDR 0x6910 /* 26896 */
#define DEFM_DECOMP_SS_ADDRESS_MASK 0xff /* 255 */
#define DEFM_DECOMP_SS_ADDRESS_OFFSET 0x0 /* 0 */
#define DEFM_DECOMP_SS_ADDRESS_WIDTH 0x8 /* 8 */
#define DEFM_DECOMP_SS_ADDRESS_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v3_0_oranshr 
* with prefix oran_ @ address 0x8000
*------------------------------------------------------------------------------
*/
/* Type   = rw  */
/* sigena =  */
/* Doc    : Simulation only, start subframe for timer */
#define ORAN_SETUP_SF_ADDR 0x8008 /* 32776 */
#define ORAN_SETUP_SF_MASK 0xf /* 15 */
#define ORAN_SETUP_SF_OFFSET 0x0 /* 0 */
#define ORAN_SETUP_SF_WIDTH 0x4 /* 4 */
#define ORAN_SETUP_SF_DEFAULT 0x9 /* 9 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Simulation only, start slot for timer */
#define ORAN_SETUP_SL_ADDR 0x800c /* 32780 */
#define ORAN_SETUP_SL_MASK 0xf /* 15 */
#define ORAN_SETUP_SL_OFFSET 0x0 /* 0 */
#define ORAN_SETUP_SL_WIDTH 0x4 /* 4 */
#define ORAN_SETUP_SL_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Simulation only, start symbol for timer */
#define ORAN_SETUP_SY_ADDR 0x8010 /* 32784 */
#define ORAN_SETUP_SY_MASK 0xf /* 15 */
#define ORAN_SETUP_SY_OFFSET 0x0 /* 0 */
#define ORAN_SETUP_SY_WIDTH 0x4 /* 4 */
#define ORAN_SETUP_SY_DEFAULT 0x7 /* 7 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : Set the number of symbols per slot for this SSB CC - 0 = 14, 1 = 12 */
#define ORAN_CC_SSB_SYMPERSLOT_ADDR 0x8100 /* 33024 */
#define ORAN_CC_SSB_SYMPERSLOT_MASK 0x1000000 /* 16777216 */
#define ORAN_CC_SSB_SYMPERSLOT_OFFSET 0x18 /* 24 */
#define ORAN_CC_SSB_SYMPERSLOT_WIDTH 0x1 /* 1 */
#define ORAN_CC_SSB_SYMPERSLOT_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : Set the numerology for this SSB Component Carrier */
#define ORAN_CC_SSB_NUMEROLOGY_ADDR 0x8100 /* 33024 */
#define ORAN_CC_SSB_NUMEROLOGY_MASK 0x70000 /* 458752 */
#define ORAN_CC_SSB_NUMEROLOGY_OFFSET 0x10 /* 16 */
#define ORAN_CC_SSB_NUMEROLOGY_WIDTH 0x3 /* 3 */
#define ORAN_CC_SSB_NUMEROLOGY_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : Number of PRBs for this SSB Component Carrier (default is 16) */
#define ORAN_CC_SSB_NUMRBS_ADDR 0x8100 /* 33024 */
#define ORAN_CC_SSB_NUMRBS_MASK 0x1ff /* 511 */
#define ORAN_CC_SSB_NUMRBS_OFFSET 0x0 /* 0 */
#define ORAN_CC_SSB_NUMRBS_WIDTH 0x9 /* 9 */
#define ORAN_CC_SSB_NUMRBS_DEFAULT 0x10 /* 16 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : Base offset for DL Type 1/3 Section messages. E.g. CC[0]=0, CC[1]=CC[0]+(SectionsPerSymbol[1]*Symbols[1]), etc. */
#define ORAN_CC_SSB_CTRL_OFFSETS_ADDR 0x8104 /* 33028 */
#define ORAN_CC_SSB_CTRL_OFFSETS_MASK 0xffff /* 65535 */
#define ORAN_CC_SSB_CTRL_OFFSETS_OFFSET 0x0 /* 0 */
#define ORAN_CC_SSB_CTRL_OFFSETS_WIDTH 0x10 /* 16 */
#define ORAN_CC_SSB_CTRL_OFFSETS_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : Starting index for the shared Uplane data pointers concerning SSB data for this CC. E.g. cc_ssb_data_sym_start_index[0]=0, cc_ssb_data_sym_start_index[1] = cc_ssb_data_sym_start_index[0]+ssb_num_data_sym_per_cc[0] */
#define ORAN_CC_SSB_DATA_SYM_START_INDEX_ADDR 0x8114 /* 33044 */
#define ORAN_CC_SSB_DATA_SYM_START_INDEX_MASK 0x3f /* 63 */
#define ORAN_CC_SSB_DATA_SYM_START_INDEX_OFFSET 0x0 /* 0 */
#define ORAN_CC_SSB_DATA_SYM_START_INDEX_WIDTH 0x6 /* 6 */
#define ORAN_CC_SSB_DATA_SYM_START_INDEX_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : Number of shared Uplane data pointers for this CC. Note the totals for all CC must less than or equal to the number set in the "Xran_Max_Dl_Data_Pointers" GUI field */
#define ORAN_CC_SSB_NUM_DATA_SYM_PER_CC_ADDR 0x8114 /* 33044 */
#define ORAN_CC_SSB_NUM_DATA_SYM_PER_CC_MASK 0x3f00 /* 16128 */
#define ORAN_CC_SSB_NUM_DATA_SYM_PER_CC_OFFSET 0x8 /* 8 */
#define ORAN_CC_SSB_NUM_DATA_SYM_PER_CC_WIDTH 0x6 /* 6 */
#define ORAN_CC_SSB_NUM_DATA_SYM_PER_CC_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : Number of symbols concerning SSB ctrl for this CC */
#define ORAN_CC_SSB_NUM_SYM_PER_CC_ADDR 0x8114 /* 33044 */
#define ORAN_CC_SSB_NUM_SYM_PER_CC_MASK 0x3f0000 /* 4128768 */
#define ORAN_CC_SSB_NUM_SYM_PER_CC_OFFSET 0x10 /* 16 */
#define ORAN_CC_SSB_NUM_SYM_PER_CC_WIDTH 0x6 /* 6 */
#define ORAN_CC_SSB_NUM_SYM_PER_CC_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : Bit width of the chosen DL IQ representation (uncompressed or mantissa) */
#define ORAN_CC_SSB_UD_IQ_WIDTH_ADDR 0x811c /* 33052 */
#define ORAN_CC_SSB_UD_IQ_WIDTH_MASK 0xf /* 15 */
#define ORAN_CC_SSB_UD_IQ_WIDTH_OFFSET 0x0 /* 0 */
#define ORAN_CC_SSB_UD_IQ_WIDTH_WIDTH 0x4 /* 4 */
#define ORAN_CC_SSB_UD_IQ_WIDTH_DEFAULT 0x9 /* 9 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : DL Compression Method: 0=no_compression, 1=block_floating_point, 4=modulation */
#define ORAN_CC_SSB_UD_COMP_METH_ADDR 0x811c /* 33052 */
#define ORAN_CC_SSB_UD_COMP_METH_MASK 0xf0 /* 240 */
#define ORAN_CC_SSB_UD_COMP_METH_OFFSET 0x4 /* 4 */
#define ORAN_CC_SSB_UD_COMP_METH_WIDTH 0x4 /* 4 */
#define ORAN_CC_SSB_UD_COMP_METH_DEFAULT 0x1 /* 1 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : 1=udCompHdr is Static and defined by M-Plane, 0=see Section Header */
#define ORAN_CC_SSB_MPLANE_UDCOMP_HDR_SEL_ADDR 0x811c /* 33052 */
#define ORAN_CC_SSB_MPLANE_UDCOMP_HDR_SEL_MASK 0x100 /* 256 */
#define ORAN_CC_SSB_MPLANE_UDCOMP_HDR_SEL_OFFSET 0x8 /* 8 */
#define ORAN_CC_SSB_MPLANE_UDCOMP_HDR_SEL_WIDTH 0x1 /* 1 */
#define ORAN_CC_SSB_MPLANE_UDCOMP_HDR_SEL_DEFAULT 0x1 /* 1 */

/* Type   = rw  */
/* sigena = cfg_ena_ssb */
/* Doc    : Bit width of the chosen DL IQ representation (uncompressed or mantissa) */
#define ORAN_CC_PRACH_UD_IQ_WIDTH_ADDR 0x8120 /* 33056 */
#define ORAN_CC_PRACH_UD_IQ_WIDTH_MASK 0xf /* 15 */
#define ORAN_CC_PRACH_UD_IQ_WIDTH_OFFSET 0x0 /* 0 */
#define ORAN_CC_PRACH_UD_IQ_WIDTH_WIDTH 0x4 /* 4 */
#define ORAN_CC_PRACH_UD_IQ_WIDTH_DEFAULT 0x9 /* 9 */

/* Type   = rw  */
/* sigena = cfg_ena_ssb */
/* Doc    : DL Compression Method: 0=no_compression, 1=block_floating_point, 4=modulation */
#define ORAN_CC_PRACH_UD_COMP_METH_ADDR 0x8120 /* 33056 */
#define ORAN_CC_PRACH_UD_COMP_METH_MASK 0xf0 /* 240 */
#define ORAN_CC_PRACH_UD_COMP_METH_OFFSET 0x4 /* 4 */
#define ORAN_CC_PRACH_UD_COMP_METH_WIDTH 0x4 /* 4 */
#define ORAN_CC_PRACH_UD_COMP_METH_DEFAULT 0x1 /* 1 */

/* Type   = rw  */
/* sigena = cfg_ena_ssb */
/* Doc    : 1=udcomp_param defined by M-Plane is valid (Static Compression), 0=see Section Header (Dynamic Compression, Default) */
#define ORAN_CC_PRACH_MPLANE_UDCOMP_HDR_SEL_ADDR 0x8120 /* 33056 */
#define ORAN_CC_PRACH_MPLANE_UDCOMP_HDR_SEL_MASK 0x100 /* 256 */
#define ORAN_CC_PRACH_MPLANE_UDCOMP_HDR_SEL_OFFSET 0x8 /* 8 */
#define ORAN_CC_PRACH_MPLANE_UDCOMP_HDR_SEL_WIDTH 0x1 /* 1 */
#define ORAN_CC_PRACH_MPLANE_UDCOMP_HDR_SEL_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : DL, as UL */
#define ORAN_CC_SSB_SETUP_C_ABS_SYMBOL_ADDR 0x8130 /* 33072 */
#define ORAN_CC_SSB_SETUP_C_ABS_SYMBOL_MASK 0xfff /* 4095 */
#define ORAN_CC_SSB_SETUP_C_ABS_SYMBOL_OFFSET 0x0 /* 0 */
#define ORAN_CC_SSB_SETUP_C_ABS_SYMBOL_WIDTH 0xc /* 12 */
#define ORAN_CC_SSB_SETUP_C_ABS_SYMBOL_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : DL, as UL */
#define ORAN_CC_SSB_SETUP_C_CYCLES_ADDR 0x8134 /* 33076 */
#define ORAN_CC_SSB_SETUP_C_CYCLES_MASK 0x1ffff /* 131071 */
#define ORAN_CC_SSB_SETUP_C_CYCLES_OFFSET 0x0 /* 0 */
#define ORAN_CC_SSB_SETUP_C_CYCLES_WIDTH 0x11 /* 17 */
#define ORAN_CC_SSB_SETUP_C_CYCLES_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : DL, as UL */
#define ORAN_CC_SSB_SETUP_D_CYCLES_ADDR 0x8138 /* 33080 */
#define ORAN_CC_SSB_SETUP_D_CYCLES_MASK 0x1ffff /* 131071 */
#define ORAN_CC_SSB_SETUP_D_CYCLES_OFFSET 0x0 /* 0 */
#define ORAN_CC_SSB_SETUP_D_CYCLES_WIDTH 0x11 /* 17 */
#define ORAN_CC_SSB_SETUP_D_CYCLES_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : SSB maximum number of CTRL sections per each symbol within each component carrier */
#define ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC_ADDR 0x8160 /* 33120 */
#define ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC_MASK 0xffff /* 65535 */
#define ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC_OFFSET 0x0 /* 0 */
#define ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC_WIDTH 0x10 /* 16 */
#define ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC_DEFAULT 0x10 /* 16 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena = cfg_ena_ssb */
/* Doc    : SSB maximum number of CTRL sections for all symbols in a component carrier, ssb_num_sym_per_cc * numssbctrlsect_x_sym_x_cc */
#define ORAN_CC_SSB_SECTS_X_SYMBOLS_ADDR 0x8168 /* 33128 */
#define ORAN_CC_SSB_SECTS_X_SYMBOLS_MASK 0xffff /* 65535 */
#define ORAN_CC_SSB_SECTS_X_SYMBOLS_OFFSET 0x0 /* 0 */
#define ORAN_CC_SSB_SECTS_X_SYMBOLS_WIDTH 0x10 /* 16 */
#define ORAN_CC_SSB_SECTS_X_SYMBOLS_DEFAULT 0x40 /* 64 */

/* Type   = rw  repeats using C_MAX_DL_SYMS @ multiples of 0x04 */
/* sigena = cfg_ena_ssb */
/* Doc    : This register is repeated config_xran_max_dl_symbols times, with address offset 0x4. It holds the base address for the indexed data symbol in the DL data buffer. These pointers are shared between component carriers. */
#define ORAN_CC_SSB_DATA_UNROLL_OFFSET_ADDR 0x8800 /* 34816 */
#define ORAN_CC_SSB_DATA_UNROLL_OFFSET_MASK 0xffff /* 65535 */
#define ORAN_CC_SSB_DATA_UNROLL_OFFSET_OFFSET 0x0 /* 0 */
#define ORAN_CC_SSB_DATA_UNROLL_OFFSET_WIDTH 0x10 /* 16 */
#define ORAN_CC_SSB_DATA_UNROLL_OFFSET_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v3_0_eth 
* with prefix eth_ @ address 0xa000
*------------------------------------------------------------------------------
*/
/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Destination_MAC[31:0] */
#define ETH_DEST_ADDR_31_0_ADDR 0xa000 /* 40960 */
#define ETH_DEST_ADDR_31_0_MASK 0xffffffff /* 4294967295 */
#define ETH_DEST_ADDR_31_0_OFFSET 0x0 /* 0 */
#define ETH_DEST_ADDR_31_0_WIDTH 0x20 /* 32 */
#define ETH_DEST_ADDR_31_0_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Destination_MAC[47:32] */
#define ETH_DEST_ADDR_47_32_ADDR 0xa004 /* 40964 */
#define ETH_DEST_ADDR_47_32_MASK 0xffff /* 65535 */
#define ETH_DEST_ADDR_47_32_OFFSET 0x0 /* 0 */
#define ETH_DEST_ADDR_47_32_WIDTH 0x10 /* 16 */
#define ETH_DEST_ADDR_47_32_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Source_MAC[31:0] */
#define ETH_SRC_ADDR_31_0_ADDR 0xa008 /* 40968 */
#define ETH_SRC_ADDR_31_0_MASK 0xffffffff /* 4294967295 */
#define ETH_SRC_ADDR_31_0_OFFSET 0x0 /* 0 */
#define ETH_SRC_ADDR_31_0_WIDTH 0x20 /* 32 */
#define ETH_SRC_ADDR_31_0_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Source_MAC[47:32] */
#define ETH_SRC_ADDR_47_32_ADDR 0xa00c /* 40972 */
#define ETH_SRC_ADDR_47_32_MASK 0xffff /* 65535 */
#define ETH_SRC_ADDR_47_32_OFFSET 0x0 /* 0 */
#define ETH_SRC_ADDR_47_32_WIDTH 0x10 /* 16 */
#define ETH_SRC_ADDR_47_32_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : VLAN TAG. Set the ID */
#define ETH_VLAN_ID_ADDR 0xa010 /* 40976 */
#define ETH_VLAN_ID_MASK 0xfff /* 4095 */
#define ETH_VLAN_ID_OFFSET 0x0 /* 0 */
#define ETH_VLAN_ID_WIDTH 0xc /* 12 */
#define ETH_VLAN_ID_DEFAULT 0x1 /* 1 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : VLAN TAG. Set the DEI */
#define ETH_VLAN_DEI_ADDR 0xa010 /* 40976 */
#define ETH_VLAN_DEI_MASK 0x1000 /* 4096 */
#define ETH_VLAN_DEI_OFFSET 0xc /* 12 */
#define ETH_VLAN_DEI_WIDTH 0x1 /* 1 */
#define ETH_VLAN_DEI_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : VLAN TAG. Set the PCP */
#define ETH_VLAN_PCP_ADDR 0xa010 /* 40976 */
#define ETH_VLAN_PCP_MASK 0xe000 /* 57344 */
#define ETH_VLAN_PCP_OFFSET 0xd /* 13 */
#define ETH_VLAN_PCP_WIDTH 0x3 /* 3 */
#define ETH_VLAN_PCP_DEFAULT 0x7 /* 7 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : Ethernet Port X IPv4 version */
#define ETH_IPV4_VERSION_ADDR 0xa030 /* 41008 */
#define ETH_IPV4_VERSION_MASK 0xf /* 15 */
#define ETH_IPV4_VERSION_OFFSET 0x0 /* 0 */
#define ETH_IPV4_VERSION_WIDTH 0x4 /* 4 */
#define ETH_IPV4_VERSION_DEFAULT 0x4 /* 4 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : Ethernet Port X IPv4 ihl */
#define ETH_IPV4_IHL_ADDR 0xa030 /* 41008 */
#define ETH_IPV4_IHL_MASK 0xf0 /* 240 */
#define ETH_IPV4_IHL_OFFSET 0x4 /* 4 */
#define ETH_IPV4_IHL_WIDTH 0x4 /* 4 */
#define ETH_IPV4_IHL_DEFAULT 0x5 /* 5 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : Ethernet Port X IPv4 dscp */
#define ETH_IPV4_DSCP_ADDR 0xa034 /* 41012 */
#define ETH_IPV4_DSCP_MASK 0x3f /* 63 */
#define ETH_IPV4_DSCP_OFFSET 0x0 /* 0 */
#define ETH_IPV4_DSCP_WIDTH 0x6 /* 6 */
#define ETH_IPV4_DSCP_DEFAULT 0x2e /* 46 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : Ethernet Port X IPv4 ecn */
#define ETH_IPV4_ECN_ADDR 0xa034 /* 41012 */
#define ETH_IPV4_ECN_MASK 0xc0 /* 192 */
#define ETH_IPV4_ECN_OFFSET 0x6 /* 6 */
#define ETH_IPV4_ECN_WIDTH 0x2 /* 2 */
#define ETH_IPV4_ECN_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : Ethernet Port X IPv4 id */
#define ETH_IPV4_ID_ADDR 0xa038 /* 41016 */
#define ETH_IPV4_ID_MASK 0xffff /* 65535 */
#define ETH_IPV4_ID_OFFSET 0x0 /* 0 */
#define ETH_IPV4_ID_WIDTH 0x10 /* 16 */
#define ETH_IPV4_ID_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : Ethernet Port X IPv4 flags */
#define ETH_IPV4_FLAGS_ADDR 0xa03c /* 41020 */
#define ETH_IPV4_FLAGS_MASK 0x7 /* 7 */
#define ETH_IPV4_FLAGS_OFFSET 0x0 /* 0 */
#define ETH_IPV4_FLAGS_WIDTH 0x3 /* 3 */
#define ETH_IPV4_FLAGS_DEFAULT 0x2 /* 2 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : Ethernet Port X IPv4 fragment_offset */
#define ETH_IPV4_FRAGMENT_OFFSET_ADDR 0xa03c /* 41020 */
#define ETH_IPV4_FRAGMENT_OFFSET_MASK 0xfff8 /* 65528 */
#define ETH_IPV4_FRAGMENT_OFFSET_OFFSET 0x3 /* 3 */
#define ETH_IPV4_FRAGMENT_OFFSET_WIDTH 0xd /* 13 */
#define ETH_IPV4_FRAGMENT_OFFSET_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : Ethernet Port X IPv4 Time to Live */
#define ETH_IPV4_TIME_TO_LIVE_ADDR 0xa040 /* 41024 */
#define ETH_IPV4_TIME_TO_LIVE_MASK 0xff /* 255 */
#define ETH_IPV4_TIME_TO_LIVE_OFFSET 0x0 /* 0 */
#define ETH_IPV4_TIME_TO_LIVE_WIDTH 0x8 /* 8 */
#define ETH_IPV4_TIME_TO_LIVE_DEFAULT 0x40 /* 64 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : Ethernet Port X IPv4 protocol (udp) */
#define ETH_IPV4_PROTOCOL_ADDR 0xa044 /* 41028 */
#define ETH_IPV4_PROTOCOL_MASK 0xff /* 255 */
#define ETH_IPV4_PROTOCOL_OFFSET 0x0 /* 0 */
#define ETH_IPV4_PROTOCOL_WIDTH 0x8 /* 8 */
#define ETH_IPV4_PROTOCOL_DEFAULT 0x11 /* 17 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : Ethernet Port X IPv4 Source Add */
#define ETH_IPV4_SOURCE_ADD_ADDR 0xa048 /* 41032 */
#define ETH_IPV4_SOURCE_ADD_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV4_SOURCE_ADD_OFFSET 0x0 /* 0 */
#define ETH_IPV4_SOURCE_ADD_WIDTH 0x20 /* 32 */
#define ETH_IPV4_SOURCE_ADD_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : Ethernet Port X IPv4 Destination Add */
#define ETH_IPV4_DESTINATION_ADD_ADDR 0xa04c /* 41036 */
#define ETH_IPV4_DESTINATION_ADD_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV4_DESTINATION_ADD_OFFSET 0x0 /* 0 */
#define ETH_IPV4_DESTINATION_ADD_WIDTH 0x20 /* 32 */
#define ETH_IPV4_DESTINATION_ADD_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : UDP configuration */
#define ETH_UDP_SOURCE_PORT_ADDR 0xa050 /* 41040 */
#define ETH_UDP_SOURCE_PORT_MASK 0xffff /* 65535 */
#define ETH_UDP_SOURCE_PORT_OFFSET 0x0 /* 0 */
#define ETH_UDP_SOURCE_PORT_WIDTH 0x10 /* 16 */
#define ETH_UDP_SOURCE_PORT_DEFAULT 0x8000 /* 32768 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv4 */
/* Doc    : UDP configuration */
#define ETH_UDP_DESTINATION_PORT_ADDR 0xa050 /* 41040 */
#define ETH_UDP_DESTINATION_PORT_MASK 0xffff0000 /* 4294901760 */
#define ETH_UDP_DESTINATION_PORT_OFFSET 0x10 /* 16 */
#define ETH_UDP_DESTINATION_PORT_WIDTH 0x10 /* 16 */
#define ETH_UDP_DESTINATION_PORT_DEFAULT 0xc000 /* 49152 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 version */
#define ETH_IPV6_V_ADDR 0xa080 /* 41088 */
#define ETH_IPV6_V_MASK 0xf /* 15 */
#define ETH_IPV6_V_OFFSET 0x0 /* 0 */
#define ETH_IPV6_V_WIDTH 0x4 /* 4 */
#define ETH_IPV6_V_DEFAULT 0x6 /* 6 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Traffic Class */
#define ETH_IPV6_TRAFFIC_CLASS_ADDR 0xa084 /* 41092 */
#define ETH_IPV6_TRAFFIC_CLASS_MASK 0xff /* 255 */
#define ETH_IPV6_TRAFFIC_CLASS_OFFSET 0x0 /* 0 */
#define ETH_IPV6_TRAFFIC_CLASS_WIDTH 0x8 /* 8 */
#define ETH_IPV6_TRAFFIC_CLASS_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Flow Label */
#define ETH_IPV6_FLOW_LABEL_ADDR 0xa088 /* 41096 */
#define ETH_IPV6_FLOW_LABEL_MASK 0xfffff /* 1048575 */
#define ETH_IPV6_FLOW_LABEL_OFFSET 0x0 /* 0 */
#define ETH_IPV6_FLOW_LABEL_WIDTH 0x14 /* 20 */
#define ETH_IPV6_FLOW_LABEL_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Next Header (udp protocol) */
#define ETH_IPV6_NEXT_HEADER_ADDR 0xa08c /* 41100 */
#define ETH_IPV6_NEXT_HEADER_MASK 0xff /* 255 */
#define ETH_IPV6_NEXT_HEADER_OFFSET 0x0 /* 0 */
#define ETH_IPV6_NEXT_HEADER_WIDTH 0x8 /* 8 */
#define ETH_IPV6_NEXT_HEADER_DEFAULT 0x11 /* 17 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Hop Limit */
#define ETH_IPV6_HOP_LIMIT_ADDR 0xa090 /* 41104 */
#define ETH_IPV6_HOP_LIMIT_MASK 0xff /* 255 */
#define ETH_IPV6_HOP_LIMIT_OFFSET 0x0 /* 0 */
#define ETH_IPV6_HOP_LIMIT_WIDTH 0x8 /* 8 */
#define ETH_IPV6_HOP_LIMIT_DEFAULT 0x40 /* 64 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Source Add[31:0] */
#define ETH_IPV6_SOURCE_ADD_31_0_ADDR 0xa094 /* 41108 */
#define ETH_IPV6_SOURCE_ADD_31_0_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_SOURCE_ADD_31_0_OFFSET 0x0 /* 0 */
#define ETH_IPV6_SOURCE_ADD_31_0_WIDTH 0x20 /* 32 */
#define ETH_IPV6_SOURCE_ADD_31_0_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Source Add[63:32] */
#define ETH_IPV6_SOURCE_ADD_63_32_ADDR 0xa098 /* 41112 */
#define ETH_IPV6_SOURCE_ADD_63_32_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_SOURCE_ADD_63_32_OFFSET 0x0 /* 0 */
#define ETH_IPV6_SOURCE_ADD_63_32_WIDTH 0x20 /* 32 */
#define ETH_IPV6_SOURCE_ADD_63_32_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Source Add[95:64] */
#define ETH_IPV6_SOURCE_ADD_95_64_ADDR 0xa09c /* 41116 */
#define ETH_IPV6_SOURCE_ADD_95_64_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_SOURCE_ADD_95_64_OFFSET 0x0 /* 0 */
#define ETH_IPV6_SOURCE_ADD_95_64_WIDTH 0x20 /* 32 */
#define ETH_IPV6_SOURCE_ADD_95_64_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Source Add[127:96] */
#define ETH_IPV6_SOURCE_ADD_127_96_ADDR 0xa0a0 /* 41120 */
#define ETH_IPV6_SOURCE_ADD_127_96_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_SOURCE_ADD_127_96_OFFSET 0x0 /* 0 */
#define ETH_IPV6_SOURCE_ADD_127_96_WIDTH 0x20 /* 32 */
#define ETH_IPV6_SOURCE_ADD_127_96_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Destination Add[31:0] */
#define ETH_IPV6_DESTINATION_ADD_31_0_ADDR 0xa0a4 /* 41124 */
#define ETH_IPV6_DESTINATION_ADD_31_0_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_DESTINATION_ADD_31_0_OFFSET 0x0 /* 0 */
#define ETH_IPV6_DESTINATION_ADD_31_0_WIDTH 0x20 /* 32 */
#define ETH_IPV6_DESTINATION_ADD_31_0_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Destination Add[63:32] */
#define ETH_IPV6_DESTINATION_ADD_63_32_ADDR 0xa0a8 /* 41128 */
#define ETH_IPV6_DESTINATION_ADD_63_32_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_DESTINATION_ADD_63_32_OFFSET 0x0 /* 0 */
#define ETH_IPV6_DESTINATION_ADD_63_32_WIDTH 0x20 /* 32 */
#define ETH_IPV6_DESTINATION_ADD_63_32_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Destination Add[95:64] */
#define ETH_IPV6_DESTINATION_ADD_95_64_ADDR 0xa0ac /* 41132 */
#define ETH_IPV6_DESTINATION_ADD_95_64_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_DESTINATION_ADD_95_64_OFFSET 0x0 /* 0 */
#define ETH_IPV6_DESTINATION_ADD_95_64_WIDTH 0x20 /* 32 */
#define ETH_IPV6_DESTINATION_ADD_95_64_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena = cfg_ena_ipv6 */
/* Doc    : Ethernet Port X IPv6 Destination Add[127:96] */
#define ETH_IPV6_DESTINATION_ADD_127_96_ADDR 0xa0b0 /* 41136 */
#define ETH_IPV6_DESTINATION_ADD_127_96_MASK 0xffffffff /* 4294967295 */
#define ETH_IPV6_DESTINATION_ADD_127_96_OFFSET 0x0 /* 0 */
#define ETH_IPV6_DESTINATION_ADD_127_96_WIDTH 0x20 /* 32 */
#define ETH_IPV6_DESTINATION_ADD_127_96_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Enable Multi-oDU support via the DU address table. This must have been enabled in the GUI, otherwise setting this bit has no affect. */
#define ETH_MULTI_ODU_SUPPORT_ADDR 0xa0d0 /* 41168 */
#define ETH_MULTI_ODU_SUPPORT_MASK 0x1 /* 1 */
#define ETH_MULTI_ODU_SUPPORT_OFFSET 0x0 /* 0 */
#define ETH_MULTI_ODU_SUPPORT_WIDTH 0x1 /* 1 */
#define ETH_MULTI_ODU_SUPPORT_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Destination_MAC[31:0] */
#define ETH_DU_TABLE_WR_DEST_ADDR_31_0_ADDR 0xa0e0 /* 41184 */
#define ETH_DU_TABLE_WR_DEST_ADDR_31_0_MASK 0xffffffff /* 4294967295 */
#define ETH_DU_TABLE_WR_DEST_ADDR_31_0_OFFSET 0x0 /* 0 */
#define ETH_DU_TABLE_WR_DEST_ADDR_31_0_WIDTH 0x20 /* 32 */
#define ETH_DU_TABLE_WR_DEST_ADDR_31_0_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Destination_MAC[47:32] */
#define ETH_DU_TABLE_WR_DEST_ADDR_47_32_ADDR 0xa0e4 /* 41188 */
#define ETH_DU_TABLE_WR_DEST_ADDR_47_32_MASK 0xffff /* 65535 */
#define ETH_DU_TABLE_WR_DEST_ADDR_47_32_OFFSET 0x0 /* 0 */
#define ETH_DU_TABLE_WR_DEST_ADDR_47_32_WIDTH 0x10 /* 16 */
#define ETH_DU_TABLE_WR_DEST_ADDR_47_32_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : VLAN TAG. Set the ID */
#define ETH_DU_TABLE_WR_VLAN_ID_ADDR 0xa0e8 /* 41192 */
#define ETH_DU_TABLE_WR_VLAN_ID_MASK 0xfff /* 4095 */
#define ETH_DU_TABLE_WR_VLAN_ID_OFFSET 0x0 /* 0 */
#define ETH_DU_TABLE_WR_VLAN_ID_WIDTH 0xc /* 12 */
#define ETH_DU_TABLE_WR_VLAN_ID_DEFAULT 0x1 /* 1 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : VLAN TAG. Set the DEI */
#define ETH_DU_TABLE_WR_VLAN_DEI_ADDR 0xa0e8 /* 41192 */
#define ETH_DU_TABLE_WR_VLAN_DEI_MASK 0x1000 /* 4096 */
#define ETH_DU_TABLE_WR_VLAN_DEI_OFFSET 0xc /* 12 */
#define ETH_DU_TABLE_WR_VLAN_DEI_WIDTH 0x1 /* 1 */
#define ETH_DU_TABLE_WR_VLAN_DEI_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : VLAN TAG. Set the PCP */
#define ETH_DU_TABLE_WR_VLAN_PCP_ADDR 0xa0e8 /* 41192 */
#define ETH_DU_TABLE_WR_VLAN_PCP_MASK 0xe000 /* 57344 */
#define ETH_DU_TABLE_WR_VLAN_PCP_OFFSET 0xd /* 13 */
#define ETH_DU_TABLE_WR_VLAN_PCP_WIDTH 0x3 /* 3 */
#define ETH_DU_TABLE_WR_VLAN_PCP_DEFAULT 0x7 /* 7 */

/* Type   = wPlsH  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Write the address/VLAN information to the table when high. Auto cleared. */
#define ETH_DU_TABLE_WR_STROBE_ADDR 0xa0ec /* 41196 */
#define ETH_DU_TABLE_WR_STROBE_MASK 0x80000000 /* 2147483648 */
#define ETH_DU_TABLE_WR_STROBE_OFFSET 0x1f /* 31 */
#define ETH_DU_TABLE_WR_STROBE_WIDTH 0x1 /* 1 */
#define ETH_DU_TABLE_WR_STROBE_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Table address to use */
#define ETH_DU_TABLE_WR_TABLE_ADDR_ADDR 0xa0ec /* 41196 */
#define ETH_DU_TABLE_WR_TABLE_ADDR_MASK 0xf /* 15 */
#define ETH_DU_TABLE_WR_TABLE_ADDR_OFFSET 0x0 /* 0 */
#define ETH_DU_TABLE_WR_TABLE_ADDR_WIDTH 0x4 /* 4 */
#define ETH_DU_TABLE_WR_TABLE_ADDR_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Destination_MAC[31:0] */
#define ETH_DU_TABLE_RD_DEST_ADDR_31_0_ADDR 0xa0f0 /* 41200 */
#define ETH_DU_TABLE_RD_DEST_ADDR_31_0_MASK 0xffffffff /* 4294967295 */
#define ETH_DU_TABLE_RD_DEST_ADDR_31_0_OFFSET 0x0 /* 0 */
#define ETH_DU_TABLE_RD_DEST_ADDR_31_0_WIDTH 0x20 /* 32 */
#define ETH_DU_TABLE_RD_DEST_ADDR_31_0_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Destination_MAC[47:32] */
#define ETH_DU_TABLE_RD_DEST_ADDR_47_32_ADDR 0xa0f4 /* 41204 */
#define ETH_DU_TABLE_RD_DEST_ADDR_47_32_MASK 0xffff /* 65535 */
#define ETH_DU_TABLE_RD_DEST_ADDR_47_32_OFFSET 0x0 /* 0 */
#define ETH_DU_TABLE_RD_DEST_ADDR_47_32_WIDTH 0x10 /* 16 */
#define ETH_DU_TABLE_RD_DEST_ADDR_47_32_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : VLAN TAG. Set the ID */
#define ETH_DU_TABLE_RD_VLAN_ID_ADDR 0xa0f8 /* 41208 */
#define ETH_DU_TABLE_RD_VLAN_ID_MASK 0xfff /* 4095 */
#define ETH_DU_TABLE_RD_VLAN_ID_OFFSET 0x0 /* 0 */
#define ETH_DU_TABLE_RD_VLAN_ID_WIDTH 0xc /* 12 */
#define ETH_DU_TABLE_RD_VLAN_ID_DEFAULT 0x1 /* 1 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : VLAN TAG. Set the DEI */
#define ETH_DU_TABLE_RD_VLAN_DEI_ADDR 0xa0f8 /* 41208 */
#define ETH_DU_TABLE_RD_VLAN_DEI_MASK 0x1000 /* 4096 */
#define ETH_DU_TABLE_RD_VLAN_DEI_OFFSET 0xc /* 12 */
#define ETH_DU_TABLE_RD_VLAN_DEI_WIDTH 0x1 /* 1 */
#define ETH_DU_TABLE_RD_VLAN_DEI_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : VLAN TAG. Set the PCP */
#define ETH_DU_TABLE_RD_VLAN_PCP_ADDR 0xa0f8 /* 41208 */
#define ETH_DU_TABLE_RD_VLAN_PCP_MASK 0xe000 /* 57344 */
#define ETH_DU_TABLE_RD_VLAN_PCP_OFFSET 0xd /* 13 */
#define ETH_DU_TABLE_RD_VLAN_PCP_WIDTH 0x3 /* 3 */
#define ETH_DU_TABLE_RD_VLAN_PCP_DEFAULT 0x7 /* 7 */

/* Type   = wPlsH  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Read the address/VLAN information to the table when high. Auto cleared */
#define ETH_DU_TABLE_RD_STROBE_ADDR 0xa0fc /* 41212 */
#define ETH_DU_TABLE_RD_STROBE_MASK 0x80000000 /* 2147483648 */
#define ETH_DU_TABLE_RD_STROBE_OFFSET 0x1f /* 31 */
#define ETH_DU_TABLE_RD_STROBE_WIDTH 0x1 /* 1 */
#define ETH_DU_TABLE_RD_STROBE_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Table address to use */
#define ETH_DU_TABLE_RD_TABLE_ADDR_ADDR 0xa0fc /* 41212 */
#define ETH_DU_TABLE_RD_TABLE_ADDR_MASK 0xf /* 15 */
#define ETH_DU_TABLE_RD_TABLE_ADDR_OFFSET 0x0 /* 0 */
#define ETH_DU_TABLE_RD_TABLE_ADDR_WIDTH 0x4 /* 4 */
#define ETH_DU_TABLE_RD_TABLE_ADDR_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v3_0_stats 
* with prefix stats_ @ address 0xc000
*------------------------------------------------------------------------------
*/
/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Total Received Good Packets counter (LSB) (includes both O-RAN and SW managed packets) */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_L_ADDR 0xc000 /* 49152 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_L_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_L_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Total Received Good Packets counter (HSB) (includes both O-RAN and SW managed packets) */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_H_ADDR 0xc004 /* 49156 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_H_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_H_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Total Received Bad Packets counter (LSB) Counts incoming Ethernet packets with an error (non-FCS) flagged by the Ethernet MAC to the IP. Refer to the Ethernet MAC IP for details of the error. */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_L_ADDR 0xc008 /* 49160 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_L_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_L_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Total Received Bad Packets counter (HSB) (includes both O-RAN and SW managed packets) */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_H_ADDR 0xc00c /* 49164 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_H_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_H_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Total Received Packets with Bad FCS counter (LSB) Counts incoming Ethernet packets with an error (FCS) flagged by the Ethernet MAC to the IP. Refer to the Ethernet MAC IP for details of the error. */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_L_ADDR 0xc010 /* 49168 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_L_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_L_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Total Received Packets with Bad FCS counter (HSB) (includes both O-RAN and SW managed packets) */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_H_ADDR 0xc014 /* 49172 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_H_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_H_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : Total Received Packets bit rate (the value read in the counter has to be multiplied by 64) */
#define STATS_ETH_STATS_TOTAL_RX_BIT_RATE_ADDR 0xc018 /* 49176 */
#define STATS_ETH_STATS_TOTAL_RX_BIT_RATE_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_TOTAL_RX_BIT_RATE_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_TOTAL_RX_BIT_RATE_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_TOTAL_RX_BIT_RATE_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN (either U- or C-Plane) Received Packets bit rate (the value read in the counter has to be multiplied by 64) */
#define STATS_ETH_STATS_ORAN_RX_BIT_RATE_ADDR 0xc01c /* 49180 */
#define STATS_ETH_STATS_ORAN_RX_BIT_RATE_MASK 0xffffffff /* 4294967295 */
#define STATS_ETH_STATS_ORAN_RX_BIT_RATE_OFFSET 0x0 /* 0 */
#define STATS_ETH_STATS_ORAN_RX_BIT_RATE_WIDTH 0x20 /* 32 */
#define STATS_ETH_STATS_ORAN_RX_BIT_RATE_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN (either U- or C-Plane) total received packets (LSB) */
#define STATS_ORAN_RX_TOTAL_L_ADDR 0xc020 /* 49184 */
#define STATS_ORAN_RX_TOTAL_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_TOTAL_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_TOTAL_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_TOTAL_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN (either U- or C-Plane) total received packets (HSB) */
#define STATS_ORAN_RX_TOTAL_H_ADDR 0xc024 /* 49188 */
#define STATS_ORAN_RX_TOTAL_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_TOTAL_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_TOTAL_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_TOTAL_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN U-Plane on_time received packets (LSB) */
#define STATS_ORAN_RX_ON_TIME_L_ADDR 0xc028 /* 49192 */
#define STATS_ORAN_RX_ON_TIME_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_ON_TIME_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_ON_TIME_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_ON_TIME_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN U-Plane on_time received packets (HSB) */
#define STATS_ORAN_RX_ON_TIME_H_ADDR 0xc02c /* 49196 */
#define STATS_ORAN_RX_ON_TIME_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_ON_TIME_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_ON_TIME_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_ON_TIME_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN U-Plane received packets arrived before the start of time window (LSB) */
#define STATS_ORAN_RX_EARLY_L_ADDR 0xc030 /* 49200 */
#define STATS_ORAN_RX_EARLY_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_EARLY_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_EARLY_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_EARLY_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN U-Plane received packets arrived before the start of time window (HSB) */
#define STATS_ORAN_RX_EARLY_H_ADDR 0xc034 /* 49204 */
#define STATS_ORAN_RX_EARLY_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_EARLY_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_EARLY_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_EARLY_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN U-Plane received packets arrived after the end of time window (LSB) */
#define STATS_ORAN_RX_LATE_L_ADDR 0xc038 /* 49208 */
#define STATS_ORAN_RX_LATE_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_LATE_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_LATE_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_LATE_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN U-Plane received packets arrived after the end of time window (HSB) */
#define STATS_ORAN_RX_LATE_H_ADDR 0xc03c /* 49212 */
#define STATS_ORAN_RX_LATE_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_LATE_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_LATE_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_LATE_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN C-Plane total received packets (LSB) */
#define STATS_ORAN_RX_TOTAL_C_L_ADDR 0xc040 /* 49216 */
#define STATS_ORAN_RX_TOTAL_C_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_TOTAL_C_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_TOTAL_C_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_TOTAL_C_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN C-Plane total received packets (HSB) */
#define STATS_ORAN_RX_TOTAL_C_H_ADDR 0xc044 /* 49220 */
#define STATS_ORAN_RX_TOTAL_C_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_TOTAL_C_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_TOTAL_C_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_TOTAL_C_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN C-Plane on_time received packets (LSB) */
#define STATS_ORAN_RX_ON_TIME_C_L_ADDR 0xc048 /* 49224 */
#define STATS_ORAN_RX_ON_TIME_C_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_ON_TIME_C_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_ON_TIME_C_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_ON_TIME_C_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN C-Plane on_time received packets (HSB) */
#define STATS_ORAN_RX_ON_TIME_C_H_ADDR 0xc04c /* 49228 */
#define STATS_ORAN_RX_ON_TIME_C_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_ON_TIME_C_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_ON_TIME_C_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_ON_TIME_C_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN C-Plane received packets arrived before the start of time window (LSB) */
#define STATS_ORAN_RX_EARLY_C_L_ADDR 0xc050 /* 49232 */
#define STATS_ORAN_RX_EARLY_C_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_EARLY_C_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_EARLY_C_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_EARLY_C_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN C-Plane received packets arrived before the start of time window (HSB) */
#define STATS_ORAN_RX_EARLY_C_H_ADDR 0xc054 /* 49236 */
#define STATS_ORAN_RX_EARLY_C_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_EARLY_C_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_EARLY_C_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_EARLY_C_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN C-Plane received packets arrived after the end of time window (LSB) */
#define STATS_ORAN_RX_LATE_C_L_ADDR 0xc058 /* 49240 */
#define STATS_ORAN_RX_LATE_C_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_LATE_C_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_LATE_C_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_LATE_C_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN C-Plane received packets arrived after the end of time window (HSB) */
#define STATS_ORAN_RX_LATE_C_H_ADDR 0xc05c /* 49244 */
#define STATS_ORAN_RX_LATE_C_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_LATE_C_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_LATE_C_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_LATE_C_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN (either U- or C-Plane) on_time received packets carrying corrupted transport header fields (LSB) */
#define STATS_ORAN_RX_CORRUPT_L_ADDR 0xc060 /* 49248 */
#define STATS_ORAN_RX_CORRUPT_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_CORRUPT_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_CORRUPT_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_CORRUPT_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN (either U- or C-Plane) on_time received packets carrying corrupted transport header fields (HSB) */
#define STATS_ORAN_RX_CORRUPT_H_ADDR 0xc064 /* 49252 */
#define STATS_ORAN_RX_CORRUPT_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_CORRUPT_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_CORRUPT_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_CORRUPT_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN (either U- or C-Plane) discarded received packets (LSB) */
#define STATS_ORAN_RX_ERROR_DROP_L_ADDR 0xc068 /* 49256 */
#define STATS_ORAN_RX_ERROR_DROP_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_ERROR_DROP_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_ERROR_DROP_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_ERROR_DROP_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN (either U- or C-Plane) discarded received packets (HSB) */
#define STATS_ORAN_RX_ERROR_DROP_H_ADDR 0xc06c /* 49260 */
#define STATS_ORAN_RX_ERROR_DROP_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_RX_ERROR_DROP_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_RX_ERROR_DROP_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_RX_ERROR_DROP_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN (either U- or C-Plane) total transmitted packets (LSB) */
#define STATS_ORAN_TX_TOTAL_L_ADDR 0xc070 /* 49264 */
#define STATS_ORAN_TX_TOTAL_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_TX_TOTAL_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_TX_TOTAL_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_TX_TOTAL_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN (either U- or C-Plane) total transmitted packets (HSB) */
#define STATS_ORAN_TX_TOTAL_H_ADDR 0xc074 /* 49268 */
#define STATS_ORAN_TX_TOTAL_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_TX_TOTAL_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_TX_TOTAL_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_TX_TOTAL_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN C-Plane total transmitted packets (LSB) */
#define STATS_ORAN_TX_TOTAL_C_L_ADDR 0xc078 /* 49272 */
#define STATS_ORAN_TX_TOTAL_C_L_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_TX_TOTAL_C_L_OFFSET 0x0 /* 0 */
#define STATS_ORAN_TX_TOTAL_C_L_WIDTH 0x20 /* 32 */
#define STATS_ORAN_TX_TOTAL_C_L_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : O-RAN C-Plane total transmitted packets (HSB) */
#define STATS_ORAN_TX_TOTAL_C_H_ADDR 0xc07c /* 49276 */
#define STATS_ORAN_TX_TOTAL_C_H_MASK 0xffffffff /* 4294967295 */
#define STATS_ORAN_TX_TOTAL_C_H_OFFSET 0x0 /* 0 */
#define STATS_ORAN_TX_TOTAL_C_H_WIDTH 0x20 /* 32 */
#define STATS_ORAN_TX_TOTAL_C_H_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : largest captured difference between the symbol number transported by a U-Plane packet and the internal timer counter */
#define STATS_OFFSET_EARLIEST_U_PKT_ADDR 0xc080 /* 49280 */
#define STATS_OFFSET_EARLIEST_U_PKT_MASK 0xfff /* 4095 */
#define STATS_OFFSET_EARLIEST_U_PKT_OFFSET 0x0 /* 0 */
#define STATS_OFFSET_EARLIEST_U_PKT_WIDTH 0xc /* 12 */
#define STATS_OFFSET_EARLIEST_U_PKT_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_ETH_W @ multiples of 0x100 */
/* sigena =  */
/* Doc    : largest captured difference between the symbol number transported by a C-Plane packet and the internal timer counter */
#define STATS_OFFSET_EARLIEST_C_PKT_ADDR 0xc084 /* 49284 */
#define STATS_OFFSET_EARLIEST_C_PKT_MASK 0xfff /* 4095 */
#define STATS_OFFSET_EARLIEST_C_PKT_OFFSET 0x0 /* 0 */
#define STATS_OFFSET_EARLIEST_C_PKT_WIDTH 0xc /* 12 */
#define STATS_OFFSET_EARLIEST_C_PKT_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v3_0_oran 
* with prefix oran_ @ address 0xe000
*------------------------------------------------------------------------------
*/
/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : If a CC is already operating, this control can be used to reload the timing parameters when safe */
#define ORAN_CC_RELOAD_ADDR 0xe000 /* 57344 */
#define ORAN_CC_RELOAD_MASK 0xffff /* 65535 */
#define ORAN_CC_RELOAD_OFFSET 0x0 /* 0 */
#define ORAN_CC_RELOAD_WIDTH 0x10 /* 16 */
#define ORAN_CC_RELOAD_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable this Component carrier */
#define ORAN_CC_ENABLE_ADDR 0xe004 /* 57348 */
#define ORAN_CC_ENABLE_MASK 0xffff /* 65535 */
#define ORAN_CC_ENABLE_OFFSET 0x0 /* 0 */
#define ORAN_CC_ENABLE_WIDTH 0x10 /* 16 */
#define ORAN_CC_ENABLE_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Set the number of symbols per slot for this Component carrier - 0 = 14, 1 = 12 */
#define ORAN_CC_SYMPERSLOT_ADDR 0xe100 /* 57600 */
#define ORAN_CC_SYMPERSLOT_MASK 0x1000000 /* 16777216 */
#define ORAN_CC_SYMPERSLOT_OFFSET 0x18 /* 24 */
#define ORAN_CC_SYMPERSLOT_WIDTH 0x1 /* 1 */
#define ORAN_CC_SYMPERSLOT_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Set the numerology for this Component carrier */
#define ORAN_CC_NUMEROLOGY_ADDR 0xe100 /* 57600 */
#define ORAN_CC_NUMEROLOGY_MASK 0x70000 /* 458752 */
#define ORAN_CC_NUMEROLOGY_OFFSET 0x10 /* 16 */
#define ORAN_CC_NUMEROLOGY_WIDTH 0x3 /* 3 */
#define ORAN_CC_NUMEROLOGY_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Number of PRBs for this Component Carrier */
#define ORAN_CC_NUMRBS_ADDR 0xe100 /* 57600 */
#define ORAN_CC_NUMRBS_MASK 0x1ff /* 511 */
#define ORAN_CC_NUMRBS_OFFSET 0x0 /* 0 */
#define ORAN_CC_NUMRBS_WIDTH 0x9 /* 9 */
#define ORAN_CC_NUMRBS_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Base offset for DL T1 Section messages. E.g. CC[0]=0, CC[1]=CC[0]+(SectionsPerSymbol[1]*Symbols[1]), etc. */
#define ORAN_CC_DL_CTRL_OFFSETS_ADDR 0xe104 /* 57604 */
#define ORAN_CC_DL_CTRL_OFFSETS_MASK 0xffff /* 65535 */
#define ORAN_CC_DL_CTRL_OFFSETS_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_CTRL_OFFSETS_WIDTH 0x10 /* 16 */
#define ORAN_CC_DL_CTRL_OFFSETS_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : NODOC Remove from core. Base offset for DL T1 Section unroll tables. CC[0]=0, CC[1]=CC[0]+(SectionsPerSymbol[1]), etc. */
#define ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_ADDR 0xe108 /* 57608 */
#define ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_MASK 0xffff /* 65535 */
#define ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_WIDTH 0x10 /* 16 */
#define ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Base offset for UL T1 Section messages. E.g. CC[0]=0, CC[1]=CC[0]+(SectionsPerSymbol[1]*Symbols[1]), etc. */
#define ORAN_CC_UL_CTRL_OFFSETS_ADDR 0xe10c /* 57612 */
#define ORAN_CC_UL_CTRL_OFFSETS_MASK 0xffff /* 65535 */
#define ORAN_CC_UL_CTRL_OFFSETS_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_CTRL_OFFSETS_WIDTH 0x10 /* 16 */
#define ORAN_CC_UL_CTRL_OFFSETS_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : NODOC Remove from core. Base offset for UL T1 Section unroll tables. CC[0]=0, CC[1]=CC[0]+(SectionsPerSymbol[1]), etc. */
#define ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_ADDR 0xe110 /* 57616 */
#define ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_MASK 0xffff /* 65535 */
#define ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_WIDTH 0x10 /* 16 */
#define ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Starting index for the shared Uplane data pointers for this CC. E.g. cc_dl_data_sym_start_index[0]=0, cc_dl_data_sym_start_index[1] = cc_dl_data_sym_start_index[0]+cc_dl_data_sym_num_index[0] */
#define ORAN_CC_DL_DATA_SYM_START_INDEX_ADDR 0xe114 /* 57620 */
#define ORAN_CC_DL_DATA_SYM_START_INDEX_MASK 0x3f /* 63 */
#define ORAN_CC_DL_DATA_SYM_START_INDEX_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_DATA_SYM_START_INDEX_WIDTH 0x6 /* 6 */
#define ORAN_CC_DL_DATA_SYM_START_INDEX_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Number of shared Uplane data pointers for this CC. Note the totals for all CC must less than or equal to the number set in the "DL Data Delay buffer pointers" GUI field */
#define ORAN_CC_DL_DATA_SYM_NUM_INDEX_ADDR 0xe114 /* 57620 */
#define ORAN_CC_DL_DATA_SYM_NUM_INDEX_MASK 0x3f00 /* 16128 */
#define ORAN_CC_DL_DATA_SYM_NUM_INDEX_OFFSET 0x8 /* 8 */
#define ORAN_CC_DL_DATA_SYM_NUM_INDEX_WIDTH 0x6 /* 6 */
#define ORAN_CC_DL_DATA_SYM_NUM_INDEX_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Number of DL Ctrl symbols for this CC */
#define ORAN_CC_DL_CTRL_SYM_NUM_INDEX_ADDR 0xe114 /* 57620 */
#define ORAN_CC_DL_CTRL_SYM_NUM_INDEX_MASK 0x3f0000 /* 4128768 */
#define ORAN_CC_DL_CTRL_SYM_NUM_INDEX_OFFSET 0x10 /* 16 */
#define ORAN_CC_DL_CTRL_SYM_NUM_INDEX_WIDTH 0x6 /* 6 */
#define ORAN_CC_DL_CTRL_SYM_NUM_INDEX_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Number of UL Ctrl symbols for this CC */
#define ORAN_CC_UL_CTRL_SYM_NUM_INDEX_ADDR 0xe114 /* 57620 */
#define ORAN_CC_UL_CTRL_SYM_NUM_INDEX_MASK 0x3f000000 /* 1056964608 */
#define ORAN_CC_UL_CTRL_SYM_NUM_INDEX_OFFSET 0x18 /* 24 */
#define ORAN_CC_UL_CTRL_SYM_NUM_INDEX_WIDTH 0x6 /* 6 */
#define ORAN_CC_UL_CTRL_SYM_NUM_INDEX_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Bit width of the chosen UL IQ representation (uncompressed or mantissa) */
#define ORAN_CC_UL_UD_IQ_WIDTH_ADDR 0xe118 /* 57624 */
#define ORAN_CC_UL_UD_IQ_WIDTH_MASK 0xf /* 15 */
#define ORAN_CC_UL_UD_IQ_WIDTH_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_UD_IQ_WIDTH_WIDTH 0x4 /* 4 */
#define ORAN_CC_UL_UD_IQ_WIDTH_DEFAULT 0x9 /* 9 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : UL Compression Method: 0=no_compression, 1=block_floating_point */
#define ORAN_CC_UL_UD_COMP_METH_ADDR 0xe118 /* 57624 */
#define ORAN_CC_UL_UD_COMP_METH_MASK 0xf0 /* 240 */
#define ORAN_CC_UL_UD_COMP_METH_OFFSET 0x4 /* 4 */
#define ORAN_CC_UL_UD_COMP_METH_WIDTH 0x4 /* 4 */
#define ORAN_CC_UL_UD_COMP_METH_DEFAULT 0x1 /* 1 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : 1=udCompHdr is Static and defined by M-Plane, 0=see Section Header */
#define ORAN_CC_UL_MPLANE_UDCOMP_HDR_SEL_ADDR 0xe118 /* 57624 */
#define ORAN_CC_UL_MPLANE_UDCOMP_HDR_SEL_MASK 0x100 /* 256 */
#define ORAN_CC_UL_MPLANE_UDCOMP_HDR_SEL_OFFSET 0x8 /* 8 */
#define ORAN_CC_UL_MPLANE_UDCOMP_HDR_SEL_WIDTH 0x1 /* 1 */
#define ORAN_CC_UL_MPLANE_UDCOMP_HDR_SEL_DEFAULT 0x1 /* 1 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Bit width of the chosen DL IQ representation (uncompressed or mantissa) */
#define ORAN_CC_DL_UD_IQ_WIDTH_ADDR 0xe11c /* 57628 */
#define ORAN_CC_DL_UD_IQ_WIDTH_MASK 0xf /* 15 */
#define ORAN_CC_DL_UD_IQ_WIDTH_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_UD_IQ_WIDTH_WIDTH 0x4 /* 4 */
#define ORAN_CC_DL_UD_IQ_WIDTH_DEFAULT 0x9 /* 9 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : DL Compression Method: 0=no_compression, 1=block_floating_point, 4=modulation */
#define ORAN_CC_DL_UD_COMP_METH_ADDR 0xe11c /* 57628 */
#define ORAN_CC_DL_UD_COMP_METH_MASK 0xf0 /* 240 */
#define ORAN_CC_DL_UD_COMP_METH_OFFSET 0x4 /* 4 */
#define ORAN_CC_DL_UD_COMP_METH_WIDTH 0x4 /* 4 */
#define ORAN_CC_DL_UD_COMP_METH_DEFAULT 0x1 /* 1 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : 1=udCompHdr is Static and defined by M-Plane, 0=see Section Header */
#define ORAN_CC_DL_MPLANE_UDCOMP_HDR_SEL_ADDR 0xe11c /* 57628 */
#define ORAN_CC_DL_MPLANE_UDCOMP_HDR_SEL_MASK 0x100 /* 256 */
#define ORAN_CC_DL_MPLANE_UDCOMP_HDR_SEL_OFFSET 0x8 /* 8 */
#define ORAN_CC_DL_MPLANE_UDCOMP_HDR_SEL_WIDTH 0x1 /* 1 */
#define ORAN_CC_DL_MPLANE_UDCOMP_HDR_SEL_DEFAULT 0x1 /* 1 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : UL, Specify the number of symbols the CTRL_TIME_ADV is offset from the 10ms strobe */
#define ORAN_CC_UL_SETUP_C_ABS_SYMBOL_ADDR 0xe120 /* 57632 */
#define ORAN_CC_UL_SETUP_C_ABS_SYMBOL_MASK 0xfff /* 4095 */
#define ORAN_CC_UL_SETUP_C_ABS_SYMBOL_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_SETUP_C_ABS_SYMBOL_WIDTH 0xc /* 12 */
#define ORAN_CC_UL_SETUP_C_ABS_SYMBOL_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : UL, Specify the number of clock cycles the CTRL_TIME_ADV is offset from abs_symbol offset above */
#define ORAN_CC_UL_SETUP_C_CYCLES_ADDR 0xe124 /* 57636 */
#define ORAN_CC_UL_SETUP_C_CYCLES_MASK 0x1ffff /* 131071 */
#define ORAN_CC_UL_SETUP_C_CYCLES_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_SETUP_C_CYCLES_WIDTH 0x11 /* 17 */
#define ORAN_CC_UL_SETUP_C_CYCLES_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : UL, Specify the end of the UPlane data accept window. This is offset from the 10ms strobe */
#define ORAN_CC_UL_SETUP_D_CYCLES_ADDR 0xe128 /* 57640 */
#define ORAN_CC_UL_SETUP_D_CYCLES_MASK 0x1ffff /* 131071 */
#define ORAN_CC_UL_SETUP_D_CYCLES_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_SETUP_D_CYCLES_WIDTH 0x11 /* 17 */
#define ORAN_CC_UL_SETUP_D_CYCLES_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : DL, as UL */
#define ORAN_CC_DL_SETUP_C_ABS_SYMBOL_ADDR 0xe130 /* 57648 */
#define ORAN_CC_DL_SETUP_C_ABS_SYMBOL_MASK 0xfff /* 4095 */
#define ORAN_CC_DL_SETUP_C_ABS_SYMBOL_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_SETUP_C_ABS_SYMBOL_WIDTH 0xc /* 12 */
#define ORAN_CC_DL_SETUP_C_ABS_SYMBOL_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : DL, as UL */
#define ORAN_CC_DL_SETUP_C_CYCLES_ADDR 0xe134 /* 57652 */
#define ORAN_CC_DL_SETUP_C_CYCLES_MASK 0x1ffff /* 131071 */
#define ORAN_CC_DL_SETUP_C_CYCLES_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_SETUP_C_CYCLES_WIDTH 0x11 /* 17 */
#define ORAN_CC_DL_SETUP_C_CYCLES_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : DL, as UL */
#define ORAN_CC_DL_SETUP_D_CYCLES_ADDR 0xe138 /* 57656 */
#define ORAN_CC_DL_SETUP_D_CYCLES_MASK 0x1ffff /* 131071 */
#define ORAN_CC_DL_SETUP_D_CYCLES_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_SETUP_D_CYCLES_WIDTH 0x11 /* 17 */
#define ORAN_CC_DL_SETUP_D_CYCLES_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : Offsets for the CC in the UL Ctrl, packet forming, Double buffer */
#define ORAN_CC_UL_BASE_OFFSET_ADDR 0xe140 /* 57664 */
#define ORAN_CC_UL_BASE_OFFSET_MASK 0xffff /* 65535 */
#define ORAN_CC_UL_BASE_OFFSET_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_BASE_OFFSET_WIDTH 0x10 /* 16 */
#define ORAN_CC_UL_BASE_OFFSET_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : UL, Specify the number of symbols the UL BID Forward is offset from the 10ms strobe */
#define ORAN_CC_UL_BIDF_C_ABS_SYMBOL_ADDR 0xe144 /* 57668 */
#define ORAN_CC_UL_BIDF_C_ABS_SYMBOL_MASK 0xfff /* 4095 */
#define ORAN_CC_UL_BIDF_C_ABS_SYMBOL_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_BIDF_C_ABS_SYMBOL_WIDTH 0xc /* 12 */
#define ORAN_CC_UL_BIDF_C_ABS_SYMBOL_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : UL, Specify the number of clock cycles the UL BID Forward is offset from abs_symbol offset above */
#define ORAN_CC_UL_BIDF_C_CYCLES_ADDR 0xe148 /* 57672 */
#define ORAN_CC_UL_BIDF_C_CYCLES_MASK 0x1ffff /* 131071 */
#define ORAN_CC_UL_BIDF_C_CYCLES_OFFSET 0x0 /* 0 */
#define ORAN_CC_UL_BIDF_C_CYCLES_WIDTH 0x11 /* 17 */
#define ORAN_CC_UL_BIDF_C_CYCLES_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : NODOC Remove from core. 10 times num_times_symperslot[cc] */
#define ORAN_CC_MAX_SYMBOLS_ADDR 0xe158 /* 57688 */
#define ORAN_CC_MAX_SYMBOLS_MASK 0xfff /* 4095 */
#define ORAN_CC_MAX_SYMBOLS_OFFSET 0x0 /* 0 */
#define ORAN_CC_MAX_SYMBOLS_WIDTH 0xc /* 12 */
#define ORAN_CC_MAX_SYMBOLS_DEFAULT 0x8c /* 140 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : DL number of CTRL sections per symbol per component carrier */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_DL_ADDR 0xe160 /* 57696 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_DL_MASK 0xffff /* 65535 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_DL_OFFSET 0x0 /* 0 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_DL_WIDTH 0x10 /* 16 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_DL_DEFAULT 0x10 /* 16 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : UL number of CTRL sections per symbol per component carrier */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_UL_ADDR 0xe164 /* 57700 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_UL_MASK 0xffff /* 65535 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_UL_OFFSET 0x0 /* 0 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_UL_WIDTH 0x10 /* 16 */
#define ORAN_CC_NUM_CTRL_PER_SYMBOL_UL_DEFAULT 0x10 /* 16 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : DL size of all sections for all symbols in a component carrier, cc_dl_ctrl_sym_num_index * cc_num_ctrl_per_symbol_dl */
#define ORAN_CC_MODVALS_DL_ADDR 0xe168 /* 57704 */
#define ORAN_CC_MODVALS_DL_MASK 0xffff /* 65535 */
#define ORAN_CC_MODVALS_DL_OFFSET 0x0 /* 0 */
#define ORAN_CC_MODVALS_DL_WIDTH 0x10 /* 16 */
#define ORAN_CC_MODVALS_DL_DEFAULT 0x40 /* 64 */

/* Type   = rw  repeats using C_MAX_COMP_CARR @ multiples of 0x70 */
/* sigena =  */
/* Doc    : UL size of all sections for all symbols in a component carrier, cc_dl_ctrl_sym_num_index * cc_num_ctrl_per_symbol_dl */
#define ORAN_CC_MODVALS_UL_ADDR 0xe16c /* 57708 */
#define ORAN_CC_MODVALS_UL_MASK 0xffff /* 65535 */
#define ORAN_CC_MODVALS_UL_OFFSET 0x0 /* 0 */
#define ORAN_CC_MODVALS_UL_WIDTH 0x10 /* 16 */
#define ORAN_CC_MODVALS_UL_DEFAULT 0x40 /* 64 */

/* Type   = rw  repeats using C_MAX_DL_SYMS @ multiples of 0x04 */
/* sigena =  */
/* Doc    : This register is repeated config_xran_max_dl_symbols times, with address offset 0x4. It holds the base address for the indexed data symbol in the DL data buffer. These pointers are shared between component carriers. */
#define ORAN_CC_DL_DATA_UNROLL_OFFSET_ADDR 0xe800 /* 59392 */
#define ORAN_CC_DL_DATA_UNROLL_OFFSET_MASK 0xffff /* 65535 */
#define ORAN_CC_DL_DATA_UNROLL_OFFSET_OFFSET 0x0 /* 0 */
#define ORAN_CC_DL_DATA_UNROLL_OFFSET_WIDTH 0x10 /* 16 */
#define ORAN_CC_DL_DATA_UNROLL_OFFSET_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v3_0_chan_proc_cfg 
* with prefix opxxch_cfg_ @ address 0x10000
*------------------------------------------------------------------------------
*/
/* Type   = roSig  */
/* sigena =  */
/* Doc    : Model parameters: Maximum number of CCs supported */
#define OPXXCH_CFG_MAX_NUM_CC_ADDR 0x10008 /* 65544 */
#define OPXXCH_CFG_MAX_NUM_CC_MASK 0xf /* 15 */
#define OPXXCH_CFG_MAX_NUM_CC_OFFSET 0x0 /* 0 */
#define OPXXCH_CFG_MAX_NUM_CC_WIDTH 0x4 /* 4 */
#define OPXXCH_CFG_MAX_NUM_CC_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Model parameters: Maximum number of antennas supported */
#define OPXXCH_CFG_MAX_NUM_ANTENNA_ADDR 0x10008 /* 65544 */
#define OPXXCH_CFG_MAX_NUM_ANTENNA_MASK 0xf00 /* 3840 */
#define OPXXCH_CFG_MAX_NUM_ANTENNA_OFFSET 0x8 /* 8 */
#define OPXXCH_CFG_MAX_NUM_ANTENNA_WIDTH 0x4 /* 4 */
#define OPXXCH_CFG_MAX_NUM_ANTENNA_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Model parameters: Maximum antenna innterleave supported */
#define OPXXCH_CFG_MAX_ANTENNA_INTERLEAVE_ADDR 0x10008 /* 65544 */
#define OPXXCH_CFG_MAX_ANTENNA_INTERLEAVE_MASK 0x70000 /* 458752 */
#define OPXXCH_CFG_MAX_ANTENNA_INTERLEAVE_OFFSET 0x10 /* 16 */
#define OPXXCH_CFG_MAX_ANTENNA_INTERLEAVE_WIDTH 0x3 /* 3 */
#define OPXXCH_CFG_MAX_ANTENNA_INTERLEAVE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Master Interrupt Enable, set high to enable interrupts and set low to clear all interrupts.  */
#define OPXXCH_CFG_CHAN_PROC_MASTER_INT_ENABLE_ADDR 0x10010 /* 65552 */
#define OPXXCH_CFG_CHAN_PROC_MASTER_INT_ENABLE_MASK 0x1 /* 1 */
#define OPXXCH_CFG_CHAN_PROC_MASTER_INT_ENABLE_OFFSET 0x0 /* 0 */
#define OPXXCH_CFG_CHAN_PROC_MASTER_INT_ENABLE_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_CHAN_PROC_MASTER_INT_ENABLE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable the downlink CC trigger update interrupt */
#define OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_EN_ADDR 0x10014 /* 65556 */
#define OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_EN_MASK 0x1 /* 1 */
#define OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_EN_OFFSET 0x0 /* 0 */
#define OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_EN_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_EN_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable the downlink sequence error interrupt */
#define OPXXCH_CFG_DL_SEQUENCE_ERROR_EN_ADDR 0x10014 /* 65556 */
#define OPXXCH_CFG_DL_SEQUENCE_ERROR_EN_MASK 0x2 /* 2 */
#define OPXXCH_CFG_DL_SEQUENCE_ERROR_EN_OFFSET 0x1 /* 1 */
#define OPXXCH_CFG_DL_SEQUENCE_ERROR_EN_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_DL_SEQUENCE_ERROR_EN_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable the downlink sequence table error interrupt */
#define OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_EN_ADDR 0x10014 /* 65556 */
#define OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_EN_MASK 0x4 /* 4 */
#define OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_EN_OFFSET 0x2 /* 2 */
#define OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_EN_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_EN_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable the uplink CC trigger update interrupt */
#define OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_EN_ADDR 0x10014 /* 65556 */
#define OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_EN_MASK 0x100 /* 256 */
#define OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_EN_OFFSET 0x8 /* 8 */
#define OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_EN_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_EN_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable the uplink sequence error interrupt */
#define OPXXCH_CFG_UL_SEQUENCE_ERROR_EN_ADDR 0x10014 /* 65556 */
#define OPXXCH_CFG_UL_SEQUENCE_ERROR_EN_MASK 0x200 /* 512 */
#define OPXXCH_CFG_UL_SEQUENCE_ERROR_EN_OFFSET 0x9 /* 9 */
#define OPXXCH_CFG_UL_SEQUENCE_ERROR_EN_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_UL_SEQUENCE_ERROR_EN_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Enable the uplink sequence table error interrupt */
#define OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_EN_ADDR 0x10014 /* 65556 */
#define OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_EN_MASK 0x400 /* 1024 */
#define OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_EN_OFFSET 0xa /* 10 */
#define OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_EN_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_EN_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Downlink: Reports when the CC update trigger has occurred. Clear using the master interrupt enable. */
#define OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_ADDR 0x10018 /* 65560 */
#define OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_MASK 0x1 /* 1 */
#define OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_OFFSET 0x0 /* 0 */
#define OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_DL_CC_UPDATE_TRIGGERED_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Downlink: Reports when the sequence has not completed in a symbol period. Clear using the master interrupt enable. */
#define OPXXCH_CFG_DL_SEQUENCE_ERROR_ADDR 0x10018 /* 65560 */
#define OPXXCH_CFG_DL_SEQUENCE_ERROR_MASK 0x2 /* 2 */
#define OPXXCH_CFG_DL_SEQUENCE_ERROR_OFFSET 0x1 /* 1 */
#define OPXXCH_CFG_DL_SEQUENCE_ERROR_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_DL_SEQUENCE_ERROR_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Downlink: Reports when data has been received when the sequence table is still updating. Clear using the master interrupt enable. */
#define OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_ADDR 0x10018 /* 65560 */
#define OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_MASK 0x4 /* 4 */
#define OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_OFFSET 0x2 /* 2 */
#define OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_DL_SEQUENCE_TABLE_ERROR_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Uplink: Reports when the CC update trigger has occurred. Clear using the master interrupt enable. */
#define OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_ADDR 0x10018 /* 65560 */
#define OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_MASK 0x100 /* 256 */
#define OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_OFFSET 0x8 /* 8 */
#define OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_UL_CC_UPDATE_TRIGGERED_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Uplink: Reports when the sequence has not completed in a symbol period. Clear using the master interrupt enable. */
#define OPXXCH_CFG_UL_SEQUENCE_ERROR_ADDR 0x10018 /* 65560 */
#define OPXXCH_CFG_UL_SEQUENCE_ERROR_MASK 0x200 /* 512 */
#define OPXXCH_CFG_UL_SEQUENCE_ERROR_OFFSET 0x9 /* 9 */
#define OPXXCH_CFG_UL_SEQUENCE_ERROR_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_UL_SEQUENCE_ERROR_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Uplink: Reports when data has been received when the sequence table is still updating. Clear using the master interrupt enable. */
#define OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_ADDR 0x10018 /* 65560 */
#define OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_MASK 0x400 /* 1024 */
#define OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_OFFSET 0xa /* 10 */
#define OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_UL_SEQUENCE_TABLE_ERROR_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Select the counter to read */
#define OPXXCH_CFG_CHANP_MONITOR_SELECT_READ_ADDR 0x10104 /* 65796 */
#define OPXXCH_CFG_CHANP_MONITOR_SELECT_READ_MASK 0x3f /* 63 */
#define OPXXCH_CFG_CHANP_MONITOR_SELECT_READ_OFFSET 0x0 /* 0 */
#define OPXXCH_CFG_CHANP_MONITOR_SELECT_READ_WIDTH 0x6 /* 6 */
#define OPXXCH_CFG_CHANP_MONITOR_SELECT_READ_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Sample the counter values in the their clock domain. */
#define OPXXCH_CFG_CHANP_MONITOR_SNAPSHOT_ADDR 0x10108 /* 65800 */
#define OPXXCH_CFG_CHANP_MONITOR_SNAPSHOT_MASK 0x1 /* 1 */
#define OPXXCH_CFG_CHANP_MONITOR_SNAPSHOT_OFFSET 0x0 /* 0 */
#define OPXXCH_CFG_CHANP_MONITOR_SNAPSHOT_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_CHANP_MONITOR_SNAPSHOT_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Sample the counter values in the AXI4-Lite clock domain. */
#define OPXXCH_CFG_CHANP_MONITOR_SAMPLE_ADDR 0x1010c /* 65804 */
#define OPXXCH_CFG_CHANP_MONITOR_SAMPLE_MASK 0x1 /* 1 */
#define OPXXCH_CFG_CHANP_MONITOR_SAMPLE_OFFSET 0x0 /* 0 */
#define OPXXCH_CFG_CHANP_MONITOR_SAMPLE_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_CHANP_MONITOR_SAMPLE_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Reset all the counters in the block */
#define OPXXCH_CFG_CHANP_MONITOR_CLEAR_ADDR 0x10110 /* 65808 */
#define OPXXCH_CFG_CHANP_MONITOR_CLEAR_MASK 0x1 /* 1 */
#define OPXXCH_CFG_CHANP_MONITOR_CLEAR_OFFSET 0x0 /* 0 */
#define OPXXCH_CFG_CHANP_MONITOR_CLEAR_WIDTH 0x1 /* 1 */
#define OPXXCH_CFG_CHANP_MONITOR_CLEAR_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Read value 31-0 */
#define OPXXCH_CFG_CHANP_MONITOR_READ_31__0_ADDR 0x10120 /* 65824 */
#define OPXXCH_CFG_CHANP_MONITOR_READ_31__0_MASK 0xffffffff /* 4294967295 */
#define OPXXCH_CFG_CHANP_MONITOR_READ_31__0_OFFSET 0x0 /* 0 */
#define OPXXCH_CFG_CHANP_MONITOR_READ_31__0_WIDTH 0x20 /* 32 */
#define OPXXCH_CFG_CHANP_MONITOR_READ_31__0_DEFAULT 0x0 /* 0 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Read value 63-32. This may extend to 64 bits, including the next 2 register locations. */
#define OPXXCH_CFG_CHANP_MONITOR_READ_63_32_ADDR 0x10124 /* 65828 */
#define OPXXCH_CFG_CHANP_MONITOR_READ_63_32_MASK 0xffffffff /* 4294967295 */
#define OPXXCH_CFG_CHANP_MONITOR_READ_63_32_OFFSET 0x0 /* 0 */
#define OPXXCH_CFG_CHANP_MONITOR_READ_63_32_WIDTH 0x20 /* 32 */
#define OPXXCH_CFG_CHANP_MONITOR_READ_63_32_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v3_0_chan_proc_fram 
* with prefix opxxch_fram_ @ address 0x12000
*------------------------------------------------------------------------------
*/
/* Type   = rwpdef  */
/* sigena =  */
/* Doc    : Framer Disable, if 1 flushes buffers and resets SN generation. 0 enables normal function */
#define OPXXCH_FRAM_CHANP_DL_DISABLE_ADDR 0x12000 /* 73728 */
#define OPXXCH_FRAM_CHANP_DL_DISABLE_MASK 0x1 /* 1 */
#define OPXXCH_FRAM_CHANP_DL_DISABLE_OFFSET 0x0 /* 0 */
#define OPXXCH_FRAM_CHANP_DL_DISABLE_WIDTH 0x1 /* 1 */
#define OPXXCH_FRAM_CHANP_DL_DISABLE_DEFAULT 0x1 /* 1 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Framer Ready, if 1 the Framer is running and generating Ethernet packets */
#define OPXXCH_FRAM_CHANP_DL_READY_ADDR 0x12000 /* 73728 */
#define OPXXCH_FRAM_CHANP_DL_READY_MASK 0x2 /* 2 */
#define OPXXCH_FRAM_CHANP_DL_READY_OFFSET 0x1 /* 1 */
#define OPXXCH_FRAM_CHANP_DL_READY_WIDTH 0x1 /* 1 */
#define OPXXCH_FRAM_CHANP_DL_READY_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  */
/* sigena =  */
/* Doc    : Framer Disable, if 1 flushes buffers and resets SN generation. 0 enables normal function */
#define OPXXCH_FRAM_CHANP_UL_DISABLE_ADDR 0x12000 /* 73728 */
#define OPXXCH_FRAM_CHANP_UL_DISABLE_MASK 0x4 /* 4 */
#define OPXXCH_FRAM_CHANP_UL_DISABLE_OFFSET 0x2 /* 2 */
#define OPXXCH_FRAM_CHANP_UL_DISABLE_WIDTH 0x1 /* 1 */
#define OPXXCH_FRAM_CHANP_UL_DISABLE_DEFAULT 0x1 /* 1 */

/* Type   = roSig  */
/* sigena =  */
/* Doc    : Framer Ready, if 1 the Framer is running and generating Ethernet packets */
#define OPXXCH_FRAM_CHANP_UL_READY_ADDR 0x12000 /* 73728 */
#define OPXXCH_FRAM_CHANP_UL_READY_MASK 0x8 /* 8 */
#define OPXXCH_FRAM_CHANP_UL_READY_OFFSET 0x3 /* 3 */
#define OPXXCH_FRAM_CHANP_UL_READY_WIDTH 0x1 /* 1 */
#define OPXXCH_FRAM_CHANP_UL_READY_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v3_0_chan_proc_setup 
* with prefix opxxch_ctrl_ @ address 0x14000
*------------------------------------------------------------------------------
*/
/* Type   = rwpdef  */
/* sigena =  */
/* Doc    : Downlink: Number of antennas to support */
#define OPXXCH_CTRL_DL_NUMBER_OF_ANTENNAS_ADDR 0x14004 /* 81924 */
#define OPXXCH_CTRL_DL_NUMBER_OF_ANTENNAS_MASK 0xf /* 15 */
#define OPXXCH_CTRL_DL_NUMBER_OF_ANTENNAS_OFFSET 0x0 /* 0 */
#define OPXXCH_CTRL_DL_NUMBER_OF_ANTENNAS_WIDTH 0x4 /* 4 */
#define OPXXCH_CTRL_DL_NUMBER_OF_ANTENNAS_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  */
/* sigena =  */
/* Doc    : Downlink: Antenna interleave factor */
#define OPXXCH_CTRL_DL_ANTENNA_INTERLEAVE_ADDR 0x14008 /* 81928 */
#define OPXXCH_CTRL_DL_ANTENNA_INTERLEAVE_MASK 0x7 /* 7 */
#define OPXXCH_CTRL_DL_ANTENNA_INTERLEAVE_OFFSET 0x0 /* 0 */
#define OPXXCH_CTRL_DL_ANTENNA_INTERLEAVE_WIDTH 0x3 /* 3 */
#define OPXXCH_CTRL_DL_ANTENNA_INTERLEAVE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Downlink: Sequence table update mode, 0 = immediate, 1 = triggered */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE_ADDR 0x1400c /* 81932 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE_MASK 0x1 /* 1 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE_OFFSET 0x0 /* 0 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE_WIDTH 0x1 /* 1 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Downlink: CC to update sequence table on */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_CC_SEL_ADDR 0x1400c /* 81932 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_CC_SEL_MASK 0xf0 /* 240 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_CC_SEL_OFFSET 0x4 /* 4 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_CC_SEL_WIDTH 0x4 /* 4 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_CC_SEL_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Downlink: Trigger enable */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_ENABLE_ADDR 0x1400c /* 81932 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_ENABLE_MASK 0x40000000 /* 1073741824 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_ENABLE_OFFSET 0x1e /* 30 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_ENABLE_WIDTH 0x1 /* 1 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_ENABLE_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Write the trigger information when high. Auto cleared */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_WR_STROBE_ADDR 0x1400c /* 81932 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_WR_STROBE_MASK 0x80000000 /* 2147483648 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_WR_STROBE_OFFSET 0x1f /* 31 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_WR_STROBE_WIDTH 0x1 /* 1 */
#define OPXXCH_CTRL_DL_SEQUENCE_TABLE_WR_STROBE_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR_OCP @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Downlink: Number of resource elements in each component carrier */
#define OPXXCH_CTRL_NEXT_DL_CC_NUMBER_OF_RES_ADDR 0x14010 /* 81936 */
#define OPXXCH_CTRL_NEXT_DL_CC_NUMBER_OF_RES_MASK 0x1fff /* 8191 */
#define OPXXCH_CTRL_NEXT_DL_CC_NUMBER_OF_RES_OFFSET 0x0 /* 0 */
#define OPXXCH_CTRL_NEXT_DL_CC_NUMBER_OF_RES_WIDTH 0xd /* 13 */
#define OPXXCH_CTRL_NEXT_DL_CC_NUMBER_OF_RES_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_MAX_COMP_CARR_OCP @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Downlink: Number of resource elements in each component carrier */
#define OPXXCH_CTRL_CURRENT_DL_CC_NUMBER_OF_RES_ADDR 0x14014 /* 81940 */
#define OPXXCH_CTRL_CURRENT_DL_CC_NUMBER_OF_RES_MASK 0x1fff /* 8191 */
#define OPXXCH_CTRL_CURRENT_DL_CC_NUMBER_OF_RES_OFFSET 0x0 /* 0 */
#define OPXXCH_CTRL_CURRENT_DL_CC_NUMBER_OF_RES_WIDTH 0xd /* 13 */
#define OPXXCH_CTRL_CURRENT_DL_CC_NUMBER_OF_RES_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  */
/* sigena =  */
/* Doc    : Uplink: Number of antennas to support */
#define OPXXCH_CTRL_UL_NUMBER_OF_ANTENNAS_ADDR 0x14204 /* 82436 */
#define OPXXCH_CTRL_UL_NUMBER_OF_ANTENNAS_MASK 0xf /* 15 */
#define OPXXCH_CTRL_UL_NUMBER_OF_ANTENNAS_OFFSET 0x0 /* 0 */
#define OPXXCH_CTRL_UL_NUMBER_OF_ANTENNAS_WIDTH 0x4 /* 4 */
#define OPXXCH_CTRL_UL_NUMBER_OF_ANTENNAS_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  */
/* sigena =  */
/* Doc    : Uplink: Antenna interleave factor */
#define OPXXCH_CTRL_UL_ANTENNA_INTERLEAVE_ADDR 0x14208 /* 82440 */
#define OPXXCH_CTRL_UL_ANTENNA_INTERLEAVE_MASK 0x7 /* 7 */
#define OPXXCH_CTRL_UL_ANTENNA_INTERLEAVE_OFFSET 0x0 /* 0 */
#define OPXXCH_CTRL_UL_ANTENNA_INTERLEAVE_WIDTH 0x3 /* 3 */
#define OPXXCH_CTRL_UL_ANTENNA_INTERLEAVE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Uplink: Sequence table update mode, 0 = immediate, 1 = triggered */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE_ADDR 0x1420c /* 82444 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE_MASK 0x1 /* 1 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE_OFFSET 0x0 /* 0 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE_WIDTH 0x1 /* 1 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Uplink: CC to update sequence table on */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_CC_SEL_ADDR 0x1420c /* 82444 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_CC_SEL_MASK 0xf0 /* 240 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_CC_SEL_OFFSET 0x4 /* 4 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_CC_SEL_WIDTH 0x4 /* 4 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_CC_SEL_DEFAULT 0x0 /* 0 */

/* Type   = rw  */
/* sigena =  */
/* Doc    : Uplink: Trigger enable */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_ENABLE_ADDR 0x1420c /* 82444 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_ENABLE_MASK 0x40000000 /* 1073741824 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_ENABLE_OFFSET 0x1e /* 30 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_ENABLE_WIDTH 0x1 /* 1 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_ENABLE_DEFAULT 0x0 /* 0 */

/* Type   = wPlsH  */
/* sigena =  */
/* Doc    : Write the trigger information when high. Auto cleared */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_WR_STROBE_ADDR 0x1420c /* 82444 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_WR_STROBE_MASK 0x80000000 /* 2147483648 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_WR_STROBE_OFFSET 0x1f /* 31 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_WR_STROBE_WIDTH 0x1 /* 1 */
#define OPXXCH_CTRL_UL_SEQUENCE_TABLE_WR_STROBE_DEFAULT 0x0 /* 0 */

/* Type   = rwpdef  repeats using C_MAX_COMP_CARR_OCP @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Uplink: Number of resource elements in each component carrier */
#define OPXXCH_CTRL_NEXT_UL_CC_NUMBER_OF_RES_ADDR 0x14210 /* 82448 */
#define OPXXCH_CTRL_NEXT_UL_CC_NUMBER_OF_RES_MASK 0x1fff /* 8191 */
#define OPXXCH_CTRL_NEXT_UL_CC_NUMBER_OF_RES_OFFSET 0x0 /* 0 */
#define OPXXCH_CTRL_NEXT_UL_CC_NUMBER_OF_RES_WIDTH 0xd /* 13 */
#define OPXXCH_CTRL_NEXT_UL_CC_NUMBER_OF_RES_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using C_MAX_COMP_CARR_OCP @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Uplink: Number of resource elements in each component carrier */
#define OPXXCH_CTRL_CURRENT_UL_CC_NUMBER_OF_RES_ADDR 0x14214 /* 82452 */
#define OPXXCH_CTRL_CURRENT_UL_CC_NUMBER_OF_RES_MASK 0x1fff /* 8191 */
#define OPXXCH_CTRL_CURRENT_UL_CC_NUMBER_OF_RES_OFFSET 0x0 /* 0 */
#define OPXXCH_CTRL_CURRENT_UL_CC_NUMBER_OF_RES_WIDTH 0xd /* 13 */
#define OPXXCH_CTRL_CURRENT_UL_CC_NUMBER_OF_RES_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank oran_radio_if_v3_0_chan_proc_sequence_table 
* with prefix sequence_tab_ @ address 0x16000
*------------------------------------------------------------------------------
*/
/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Downlink: Number of REs in this section of the CC */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_NUM_RES_ADDR 0x16000 /* 90112 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_NUM_RES_MASK 0xfff /* 4095 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_NUM_RES_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_NUM_RES_WIDTH 0xc /* 12 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_NUM_RES_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Downlink: RE start position for this section of the CC */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_RE_START_ADDR 0x16000 /* 90112 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_RE_START_MASK 0xfff0000 /* 268369920 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_RE_START_OFFSET 0x10 /* 16 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_RE_START_WIDTH 0xc /* 12 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_RE_START_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Downlink: DFE CC ID this section pertains to */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ID_ADDR 0x16004 /* 90116 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ID_MASK 0xf /* 15 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ID_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ID_WIDTH 0x4 /* 4 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ID_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Downlink: ORAN CC number this section pertains to */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_NUM_ADDR 0x16004 /* 90116 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_NUM_MASK 0xf00 /* 3840 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_NUM_OFFSET 0x8 /* 8 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_NUM_WIDTH 0x4 /* 4 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_NUM_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Downlink: Inter symbol gap for this CC */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ISG_ADDR 0x16004 /* 90116 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ISG_MASK 0xfff0000 /* 268369920 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ISG_OFFSET 0x10 /* 16 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ISG_WIDTH 0xc /* 12 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ISG_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Downlink: Last section in the sequence */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_FINAL_ADDR 0x16004 /* 90116 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_FINAL_MASK 0x40000000 /* 1073741824 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_FINAL_OFFSET 0x1e /* 30 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_FINAL_WIDTH 0x1 /* 1 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_FINAL_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Downlink: Last section in a symbol */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_LAST_ADDR 0x16004 /* 90116 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_LAST_MASK 0x80000000 /* 2147483648 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_LAST_OFFSET 0x1f /* 31 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_LAST_WIDTH 0x1 /* 1 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_LAST_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Downlink: Antenna configuration for this section */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_ANT_CONFIG_ADDR 0x16008 /* 90120 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_ANT_CONFIG_MASK 0xffffffff /* 4294967295 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_ANT_CONFIG_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_ANT_CONFIG_WIDTH 0x20 /* 32 */
#define SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_ANT_CONFIG_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Uplink: Number of REs in this section of the CC */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_NUM_RES_ADDR 0x16400 /* 91136 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_NUM_RES_MASK 0xfff /* 4095 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_NUM_RES_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_NUM_RES_WIDTH 0xc /* 12 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_NUM_RES_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Uplink: RE start position for this section of the CC */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_RE_START_ADDR 0x16400 /* 91136 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_RE_START_MASK 0xfff0000 /* 268369920 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_RE_START_OFFSET 0x10 /* 16 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_RE_START_WIDTH 0xc /* 12 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_RE_START_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Uplink: DFE CC ID this section pertains to */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_ID_ADDR 0x16404 /* 91140 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_ID_MASK 0xf /* 15 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_ID_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_ID_WIDTH 0x4 /* 4 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_ID_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Uplink: ORAN CC number this section pertains to */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_NUM_ADDR 0x16404 /* 91140 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_NUM_MASK 0xf00 /* 3840 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_NUM_OFFSET 0x8 /* 8 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_NUM_WIDTH 0x4 /* 4 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_NUM_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Uplink: Last section in the sequence */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_FINAL_ADDR 0x16404 /* 91140 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_FINAL_MASK 0x40000000 /* 1073741824 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_FINAL_OFFSET 0x1e /* 30 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_FINAL_WIDTH 0x1 /* 1 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_FINAL_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Uplink: Last section in a symbol */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_LAST_ADDR 0x16404 /* 91140 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_LAST_MASK 0x80000000 /* 2147483648 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_LAST_OFFSET 0x1f /* 31 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_LAST_WIDTH 0x1 /* 1 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_LAST_DEFAULT 0x0 /* 0 */

/* Type   = rw  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Next Uplink: Antenna configuration for this section */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_ANT_CONFIG_ADDR 0x16408 /* 91144 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_ANT_CONFIG_MASK 0xffffffff /* 4294967295 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_ANT_CONFIG_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_ANT_CONFIG_WIDTH 0x20 /* 32 */
#define SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_ANT_CONFIG_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Downlink: Number of REs in this section of the CC */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_NUM_RES_ADDR 0x16800 /* 92160 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_NUM_RES_MASK 0xfff /* 4095 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_NUM_RES_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_NUM_RES_WIDTH 0xc /* 12 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_NUM_RES_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Downlink: RE start position for this section of the CC */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_RE_START_ADDR 0x16800 /* 92160 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_RE_START_MASK 0xfff0000 /* 268369920 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_RE_START_OFFSET 0x10 /* 16 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_RE_START_WIDTH 0xc /* 12 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_RE_START_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Downlink: DFE CC ID this section pertains to */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ID_ADDR 0x16804 /* 92164 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ID_MASK 0xf /* 15 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ID_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ID_WIDTH 0x4 /* 4 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ID_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Downlink: ORAN CC number this section pertains to */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_NUM_ADDR 0x16804 /* 92164 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_NUM_MASK 0xf00 /* 3840 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_NUM_OFFSET 0x8 /* 8 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_NUM_WIDTH 0x4 /* 4 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_NUM_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Downlink: Inter symbol gap for this CC */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ISG_ADDR 0x16804 /* 92164 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ISG_MASK 0xfff0000 /* 268369920 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ISG_OFFSET 0x10 /* 16 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ISG_WIDTH 0xc /* 12 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_CC_ISG_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Downlink: Last section in the sequence */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_FINAL_ADDR 0x16804 /* 92164 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_FINAL_MASK 0x40000000 /* 1073741824 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_FINAL_OFFSET 0x1e /* 30 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_FINAL_WIDTH 0x1 /* 1 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_FINAL_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Downlink: Last section in a symbol */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_LAST_ADDR 0x16804 /* 92164 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_LAST_MASK 0x80000000 /* 2147483648 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_LAST_OFFSET 0x1f /* 31 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_LAST_WIDTH 0x1 /* 1 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_LAST_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Downlink: Antenna configuration for this section */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_ANT_CONFIG_ADDR 0x16808 /* 92168 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_ANT_CONFIG_MASK 0xffffffff /* 4294967295 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_ANT_CONFIG_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_ANT_CONFIG_WIDTH 0x20 /* 32 */
#define SEQUENCE_TAB_CURRENT_DL_CC_CONFIGURATION_ANT_CONFIG_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Uplink: Number of REs in this section of the CC */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_NUM_RES_ADDR 0x16c00 /* 93184 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_NUM_RES_MASK 0xfff /* 4095 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_NUM_RES_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_NUM_RES_WIDTH 0xc /* 12 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_NUM_RES_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Uplink: RE start position for this section of the CC */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_RE_START_ADDR 0x16c00 /* 93184 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_RE_START_MASK 0xfff0000 /* 268369920 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_RE_START_OFFSET 0x10 /* 16 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_RE_START_WIDTH 0xc /* 12 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_RE_START_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Uplink: DFE CC ID this section pertains to */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_ID_ADDR 0x16c04 /* 93188 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_ID_MASK 0xf /* 15 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_ID_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_ID_WIDTH 0x4 /* 4 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_ID_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Uplink: ORAN CC number this section pertains to */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_NUM_ADDR 0x16c04 /* 93188 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_NUM_MASK 0xf00 /* 3840 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_NUM_OFFSET 0x8 /* 8 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_NUM_WIDTH 0x4 /* 4 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_CC_NUM_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Uplink: Last section in the sequence */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_FINAL_ADDR 0x16c04 /* 93188 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_FINAL_MASK 0x40000000 /* 1073741824 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_FINAL_OFFSET 0x1e /* 30 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_FINAL_WIDTH 0x1 /* 1 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_FINAL_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Uplink: Last section in a symbol */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_LAST_ADDR 0x16c04 /* 93188 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_LAST_MASK 0x80000000 /* 2147483648 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_LAST_OFFSET 0x1f /* 31 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_LAST_WIDTH 0x1 /* 1 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_LAST_DEFAULT 0x0 /* 0 */

/* Type   = roSig  repeats using ADDR_TABLE_ENTRIES @ multiples of 0x010 */
/* sigena =  */
/* Doc    : Current Uplink: Antenna configuration for this section */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_ANT_CONFIG_ADDR 0x16c08 /* 93192 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_ANT_CONFIG_MASK 0xffffffff /* 4294967295 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_ANT_CONFIG_OFFSET 0x0 /* 0 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_ANT_CONFIG_WIDTH 0x20 /* 32 */
#define SEQUENCE_TAB_CURRENT_UL_CC_CONFIGURATION_ANT_CONFIG_DEFAULT 0x0 /* 0 */



