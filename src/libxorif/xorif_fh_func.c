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
 * @file xorif_fh_func.c
 * @author Steven Dickinson
 * @brief Source file for libxorif miscellaneous functions.
 * @addtogroup libxorif
 * @{
 */

#include <math.h>
#include "xorif_common.h"
#include "xorif_fh_func.h"
#include "xorif_utils.h"
#include "xorif_registers.h"

// FHI alarm flags and counters
static uint32_t fhi_alarm_status = 0;

// FHI ISR callback function
static isr_func_t fhi_callback = NULL;

#ifdef NO_HW
// Fake register bank
uint32_t fake_reg_bank[0x10000 / 4];
#endif

// Clock default value (gets set later from register)
static double XRAN_TIMER_CLK = 2500;

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
static uint16_t num_ru_bits = 0;
static uint16_t num_bs_bits = 0;

// Unknown "stream type" for RU port mapping (used to de-allocate the address)
#define UNKNOWN_STREAM_TYPE 0x3F

// Local function prototypes...
static uint16_t calc_sym_num(uint16_t numerology, uint16_t extended_cp, double time);
static uint16_t calc_data_buff_size(uint16_t num_rbs,
                                    enum xorif_iq_comp comp_mode,
                                    uint16_t comp_width,
                                    uint16_t mplane,
                                    uint16_t num_sect,
                                    uint16_t num_frames);
static void deallocate_memory(int cc);
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

        // Reset the memory allocation pointers
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

    // Poll status register for latest errors
    uint32_t status = READ_REG_RAW(FHI_INTR_STATUS_ADDR) & FHI_INTR_MASK;
    fhi_alarm_status |= status;
    return fhi_alarm_status;
}

void xorif_clear_fhi_alarms(void)
{
    TRACE("xorif_clear_fhi_alarms()\n");

    // Clear interrupts by writing to the "master interrupt"
    WRITE_REG(CFG_MASTER_INT_ENABLE, 0);
    WRITE_REG(CFG_MASTER_INT_ENABLE, 1);

    // Clear the alarm status
    fhi_alarm_status = 0;
}

void xorif_clear_fhi_stats(void)
{
    TRACE("xorif_clear_fhi_stats()\n");

    // Take snapshot (with reset)
    WRITE_REG_RAW(DEFM_SNAP_SHOT_ADDR, 0xFFFFFFFF);
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

int xorif_set_fhi_protocol_alt(enum xorif_transport_protocol transport,
                               uint16_t vlan,
                               enum xorif_ip_mode ip_mode)
{
    TRACE("xorif_set_fhi_protocol_alt(%d, %d, %d)\n", transport, vlan, ip_mode);

    // Set up the global protocol, VLAN and IP mode
    WRITE_REG(FRAM_PROTOCOL_DEFINITION, transport);
    WRITE_REG(FRAM_GEN_VLAN_TAG, vlan);
    WRITE_REG(FRAM_SEL_IPV_ADDRESS_TYPE, ip_mode);

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

    if (prach_val > mask)
    {
        // Disable PRACH mapping (since masking with 0x00 can never == 0xFF)
        WRITE_REG(DEFM_CID_PRACH_MASK, 0x00);
        WRITE_REG(DEFM_CID_PRACH_VALUE, 0xFF);
    }
    else
    {
        WRITE_REG(DEFM_CID_PRACH_MASK, xx_mask);
        WRITE_REG(DEFM_CID_PRACH_VALUE, prach_val & xx_mask);
    }

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

    // Disable LTE mapping (since masking with 0x00 can never == 0xFF)
    WRITE_REG(DEFM_CID_LTE_MASK, 0x00);
    WRITE_REG(DEFM_CID_LTE_VALUE, 0xFF);

#ifdef DEBUG
    if (xorif_trace >= 2)
    {
        INFO("RU mask:     %s\n", binary_mask_string(ru_mask, ru_mask, 16));
        INFO("SS mask:     %s\n", binary_mask_string(ss_mask, ss_mask, 16));
        INFO("Mask:        %s\n", binary_mask_string(xx_mask, xx_mask, 16));
        INFO("User value:  %s\n", binary_mask_string(user_val & xx_mask, xx_mask, 16));
        if (prach_val <= mask)
        {
            INFO("PRACH value: %s\n", binary_mask_string(prach_val & xx_mask, xx_mask, 16));
        }
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

    if (prach_val > mask)
    {
        // Disable PRACH mapping (since masking with 0x00 can never == 0xFF)
        WRITE_REG(DEFM_CID_PRACH_MASK, 0x00);
        WRITE_REG(DEFM_CID_PRACH_VALUE, 0xFF);
    }
    else
    {
        WRITE_REG(DEFM_CID_PRACH_MASK, xx_mask);
        WRITE_REG(DEFM_CID_PRACH_VALUE, prach_val & xx_mask);
    }

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
        if (prach_val <= mask)
        {
            INFO("PRACH value: %s\n", binary_mask_string(prach_val & xx_mask, xx_mask, 16));
        }
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
        WRITE_REG(DEFM_CID_MAP_MODE, mode);
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
            WRITE_REG_RAW(DEFM_CID_MAP_WR_STROBE_ADDR, value);
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
                WRITE_REG_RAW(DEFM_CID_MAP_WR_STROBE_ADDR, value);
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

#ifdef ENABLE_INTERRUPTS
int fhi_irq_handler(int id, void *data)
{
    struct xorif_device_info *device = (struct xorif_device_info *)data;

    if (device)
    {
        // Check interrupt status
        uint32_t status = READ_REG_RAW(FHI_INTR_STATUS_ADDR) & FHI_INTR_MASK;
        INFO("fhi_irq_handler() status = 0x%X\n", status);

        if (status)
        {
            // Record the alarm status
            fhi_alarm_status |= status;

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
        }

        if (fhi_callback)
        {
            // Call registered call-back function
            (*fhi_callback)(status);
        }

        return METAL_IRQ_HANDLED;
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
    fhi_caps.iq_comp_bfp_widths = 0x5200); // 9, 12, 14
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
    if (get_device_property_u32(fh_device.dev->name, "xlnx,xran-max-ssb-ctrl-512words", &temp))
    {
        fhi_caps.max_ssb_ctrl_512words = temp;
    }
    if (get_device_property_u32(fh_device.dev->name, "xlnx,xran-max-ssb-data-512words", &temp))
    {
        fhi_caps.max_ssb_data_512words = temp;
    }
#endif

    // Reset everything
    xorif_reset_fhi(0);

#ifdef ENABLE_INTERRUPTS
    // Setup interrupts *** all disabled by default ***
    // Use xorif_enable_fhi_interrupts() to enable
    WRITE_REG_RAW(FHI_INTR_ENABLE_ADDR, 0);

    // Finally enable the master interrupt
    WRITE_REG(CFG_MASTER_INT_ENABLE, 1);
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

int xorif_fhi_set_cc_iq_compression_prach(uint16_t cc,
                                          uint16_t bit_width,
                                          enum xorif_iq_comp comp_meth,
                                          uint16_t mplane)
{
    WRITE_REG_OFFSET(ORAN_CC_PRACH_UD_IQ_WIDTH, cc * 0x70, (bit_width & 0xF));
    WRITE_REG_OFFSET(ORAN_CC_PRACH_UD_COMP_METH, cc * 0x70, comp_meth);
    WRITE_REG_OFFSET(ORAN_CC_PRACH_MPLANE_UDCOMP_HDR_SEL, cc * 0x70, (mplane & 0x1));
    return XORIF_SUCCESS;
}

int xorif_set_cc_dl_iq_compression_per_ss(uint16_t ss,
                                          uint16_t bit_width,
                                          enum xorif_iq_comp comp_method,
                                          uint16_t enable,
                                          uint16_t number)
{
    TRACE("xorif_set_cc_dl_iq_compression_per_ss(%d, %d, %d, %d, %d)\n",
          ss, bit_width, comp_method, enable, number);

    if (!check_iq_comp_mode(bit_width, comp_method, CHAN_DL))
    {
        PERROR("IQ compression method/width not supported (DL)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }
    else if ((ss + number) > fhi_caps.no_deframer_ss)
    {
        PERROR("Invalid spatial stream value\n");
        return XORIF_INVALID_SS;
    }

    // Construct the word to program
    uint32_t value = (1 << 31) |
                     (enable ? (1 << 16) : 0) |
                     (comp_method << 12) |
                     ((bit_width & 0xF) << 8);

    // Program the required number of times via the register interface
    for (int i = 0; i < number; ++i)
    {
        WRITE_REG_RAW(DEFM_DECOMP_SS_WR_ADDR, value | ((ss + i) & 0xFF));
    }

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
    uint16_t ssb_ctrl_sym_num = 0;
    uint16_t ssb_data_sym_num = 0;

    if (ptr->num_rbs_ssb)
    {
        ssb_ctrl_sym_num = calc_sym_num(ptr->numerology_ssb, ptr->extended_cp_ssb, ptr->delay_comp_cp_dl + ptr->advance_dl + fhi_sys_const.FH_DECAP_DLY);
        ssb_data_sym_num = calc_sym_num(ptr->numerology_ssb, ptr->extended_cp_ssb, ptr->delay_comp_up + fhi_sys_const.FH_DECAP_DLY);
    }

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
                                                     ptr->iq_comp_mplane_dl,
                                                     ptr->num_sect_per_sym,
                                                     ptr->num_frames_per_sym);

    // Calculate SSB data buffer size (per symbol)
    uint16_t ssb_data_buff_size = 0;
    if (ptr->num_rbs_ssb)
    {
        ssb_data_buff_size = calc_data_buff_size(ptr->num_rbs_ssb,
                                                 ptr->iq_comp_meth_ssb,
                                                 ptr->iq_comp_width_ssb,
                                                 ptr->iq_comp_mplane_ssb,
                                                 ptr->num_sect_per_sym_ssb,
                                                 ptr->num_frames_per_sym_ssb);
    }

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

#ifdef DEBUG
    if (xorif_trace >= 2)
    {
        // Temporarily suspend debug level
        int temp = xorif_trace;
        xorif_trace = 0;

        // Get versions
        uint32_t hw_version = xorif_get_fhi_hw_version();
        uint32_t hw_internal = xorif_get_fhi_hw_internal_rev();
        uint32_t sw_version = xorif_get_sw_version();

        // Restore debug level
        xorif_trace = temp;

        INFO("HW Version = %08x\n", hw_version);
        INFO("HW Internal Revision = %u\n", hw_internal);
        INFO("SW Version = %08x\n", sw_version);
    }
#endif

    // Program the h/w...

    // DL / UL
    xorif_fhi_init_cc_rbs(cc, ptr->num_rbs, ptr->numerology, ptr->extended_cp);
    xorif_fhi_init_cc_symbol_pointers(cc, dl_data_sym_num, dl_data_ptrs_offset, dl_ctrl_sym_num, ul_ctrl_sym_num);
    xorif_fhi_set_cc_dl_iq_compression(cc, ptr->iq_comp_width_dl, ptr->iq_comp_meth_dl, ptr->iq_comp_mplane_dl);
    xorif_fhi_set_cc_ul_iq_compression(cc, ptr->iq_comp_width_ul, ptr->iq_comp_meth_ul, ptr->iq_comp_mplane_ul);
    xorif_fhi_init_cc_dl_section_mem(cc, ptr->num_ctrl_per_sym_dl, dl_ctrl_offset);
    xorif_fhi_init_cc_ul_section_mem(cc, ptr->num_ctrl_per_sym_ul, ul_ctrl_offset, ul_ctrl_base_offset);
    xorif_fhi_init_cc_dl_data_offsets(cc, dl_data_sym_num, dl_data_ptrs_offset, dl_data_buff_offset, dl_data_buff_size);
    xorif_fhi_init_cc_ctrl_constants(cc, dl_ctrl_sym_num, ptr->num_ctrl_per_sym_dl, ul_ctrl_sym_num, ptr->num_ctrl_per_sym_ul);
    xorif_fhi_configure_time_advance_offsets(cc, ptr->numerology, ptr->extended_cp, ptr->advance_ul, ptr->advance_dl, ptr->ul_bid_forward);

    // SSB
    xorif_fhi_init_cc_rbs_ssb(cc, ptr->num_rbs_ssb, ptr->numerology_ssb, ptr->extended_cp_ssb);
    xorif_fhi_init_cc_symbol_pointers_ssb(cc, ssb_data_sym_num, ssb_data_ptrs_offset, ssb_ctrl_sym_num);
    xorif_fhi_set_cc_iq_compression_ssb(cc, ptr->iq_comp_width_ssb, ptr->iq_comp_meth_ssb, ptr->iq_comp_mplane_ssb);
    xorif_fhi_init_cc_section_mem_ssb(cc, ptr->num_ctrl_per_sym_ssb, ssb_ctrl_offset);
    xorif_fhi_init_cc_dl_data_offsets_ssb(cc, ssb_data_sym_num, ssb_data_ptrs_offset, ssb_data_buff_offset, ssb_data_buff_size);
    xorif_fhi_init_cc_ctrl_constants_ssb(cc, ssb_ctrl_sym_num, ptr->num_ctrl_per_sym_ssb);
    xorif_fhi_configure_time_advance_offsets_ssb(cc, ptr->numerology_ssb, ptr->extended_cp_ssb, ptr->advance_dl);

    // PRACH
    xorif_fhi_set_cc_iq_compression_prach(cc, ptr->iq_comp_width_prach, ptr->iq_comp_meth_prach, ptr->iq_comp_mplane_prach);

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
 * @param[in] mplane Flag indicating M-plane (1) or C-plane (0) configuration
 * @param[in] num_sect Number of sections
 * @param[in] num_frames Number of Ethernet frames
 * @returns
 *      - Downlink data buffer size (in 8-byte words)
 */
static uint16_t calc_data_buff_size(uint16_t num_rbs,
                                    enum xorif_iq_comp comp_mode,
                                    uint16_t comp_width,
                                    uint16_t mplane,
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
        // (n bits I + n bits Q) per RE and round-up
        size = CEIL_DIV((comp_width * 2 * RE_PER_RB), 8);
        break;
    }

    // Multiply by number of RBs
    size *= num_rbs;

    // Add bytes per section header
    // The number of additional bytes depends on whether static (i.e. M-Plane)
    // or dynamic compression is used (4 for static, 6 for dynamic)
    // TODO this could be 8 if compression with RE mask is supported
    size += (mplane ? 4 : 6) * num_sect;

    // Add 11 bytes per Ethernet frame (4 + 7 for worse-case rounding to 8-byte word)
    size += 11 * num_frames;

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
    WRITE_REG(CFG_CONFIG_NO_OF_DEFM_ANTS, 20);
    WRITE_REG(CFG_CONFIG_NO_OF_ETH_PORTS, 4);
    WRITE_REG(CFG_CONFIG_XRAN_MAX_CC, 8);
    WRITE_REG(CFG_CONFIG_XRAN_MAX_DL_SYMBOLS, 16);
    WRITE_REG(CFG_CONFIG_XRAN_FRAM_ETH_PKT_MAX, 8000);
    WRITE_REG(CFG_CONFIG_XRAN_DEFM_ETH_PKT_MAX, 8000);
    WRITE_REG(CFG_CONFIG_XRAN_MAX_SCS, 6600); // 3300, 6600, 13200
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
#endif // NO_HW

#ifdef EXTRA_DEBUG
/**
 * @brief Inject error for testing purposes.
 * @param status Interrupt status bits
 * @return
 *      - METAL_IRQ_HANDLED
 *      - METAL_IRQ_NOT_HANDLED
 * @note
 * This function is for testing only, and is not exposed in the
 * API header file.
 */
int xorif_test_error_injections(uint32_t status)
{
#ifdef NO_HW
    WRITE_REG_RAW(FHI_INTR_STATUS_ADDR, status);
    int result = fhi_irq_handler(99, (void *)&fh_device);
    WRITE_REG_RAW(FHI_INTR_STATUS_ADDR, 0);
    return result;
#else
    return XORIF_NOT_SUPPORTED;
#endif
}
#endif // EXTRA_DEBUG

/** @} */
