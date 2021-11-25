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
 * @file xorif_api.h
 * @author Steven Dickinson
 * @brief Main header file for the Xilinx ORAN Radio Interface (libxorif).
 * @addtogroup libxorif-api
 * @{
 *
 * "C" API for the Xilinx ORAN Radio Interface (libxorif)
 */

#ifndef XORIF_API_H
#define XORIF_API_H

#ifndef CFFI_CDEF_HDR
#include <inttypes.h>
#endif

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

#ifndef XORIF_COMMON_ERROR_CODES
#define XORIF_COMMON_ERROR_CODES
/**
 * @brief Enumerated type for error/status codes.
 */
enum xorif_error_codes
{
    XORIF_OPERATIONAL_ERRORS = -3000,   /**< (Place-holder for operational errors) */
    XORIF_NULL_POINTER,                 /**< Attempt to access memory with null pointer */
    XORIF_REGISTER_NOT_FOUND,           /**< Unknown register name */
    XORIF_MEMORY_ALLOCATION_FAIL,       /**< Memory allocation fail */
    XORIF_TIMEOUT_FAIL,                 /**< Timeout fail */
    XORIF_CONFIGURATION_ERRORS = -2000, /**< (Place-holder for configuration errors) */
    XORIF_INVALID_CC,                   /**< Component carrier instance is not valid */
    XORIF_INVALID_RBS,                  /**< Number of RBs is not valid */
    XORIF_INVALID_CONFIG,               /**< Invalid configuration (general error) */
    XORIF_NUMEROLOGY_NOT_SUPPORTED,     /**< Requested numerology is not supported */
    XORIF_COMP_MODE_NOT_SUPPORTED,      /**< Requested compression mode is not supported */
    XORIF_FRONT_HAUL_ERRORS = -1500,    /**< (Place-holder for Front-Haul specific errors) */
    XORIF_MAX_CTRL_SYM_EXCEEDED,        /**< Number of control symbols exceeds available space */
    XORIF_MAX_DATA_SYM_EXCEEDED,        /**< Number of data symbols exceeds available space */
    XORIF_BUFFER_SPACE_EXCEEDED,        /**< Required buffer space exceeds available space */
    XORIF_INVALID_ETH_PORT,             /**< Invalid ethernet port number */
    XORIF_INVALID_EAXC_ID,              /**< The specified eAxC ID is invalid */
    XORIF_INVALID_RU_PORT_MAPPING,      /**< Invalid RU port mapping assignment */
    XORIF_BEAMFORMER_ERRORS = -1400,    /**< (Place-holder for Beamformer-specific errors) */
    XORIF_SCHEDULE_TABLE_EXCEEDED,      /**< Out of space in Beamformer schedule table */
    XORIF_INVALID_AG,                   /**< Antenna group is not valid */
    XORIF_FRAMEWORK_ERRORS = -1000,     /**< (Place holder for library / libmetal framwork errors) */
    XORIF_LIBMETAL_ERROR,               /**< Error with libmetal framework */
    XORIF_OTHER_ERRORS = -500,          /**< (Place-holder for other errors) */
    XORIF_INVALID_RESULT = -1,          /**< Represents an invalid result */
    XORIF_SUCCESS = 0,                  /**< Success! No error! */
    XORIF_FAILURE = 1,                  /**< Failure return code */
};
#endif // XORIF_COMMON_ERROR_CODES

#ifndef XORIF_COMMON_IQ_COMP_CODES
#define XORIF_COMMON_IQ_COMP_CODES
/**
 * @brief Enumerated type for IQ compression modes (ORAN values).
 */
enum xorif_iq_comp
{
    IQ_COMP_NONE = 0,    /**< No compression */
    IQ_COMP_BLOCK_FP,    /**< Block floating point compression */
    IQ_COMP_BLOCK_SCALE, /**< Block scaling compression */
    IQ_COMP_U_LAW,       /**< U-Law compression */
    IQ_COMP_MODULATION,  /**< Modulation compression */
};

/**
 * @brief Enumerated type for supported IQ compression modes (bit-map).
 */
enum xorif_iq_comp_flags
{
    IQ_COMP_NONE_SUPPORT = 0x01,        /**< No compression */
    IQ_COMP_BLOCK_FP_SUPPORT = 0x02,    /**< Block floating point compression */
    IQ_COMP_BLOCK_SCALE_SUPPORT = 0x04, /**< Block scaling compression */
    IQ_COMP_U_LAW_SUPPORT = 0x08,       /**< U-Law compression */
    IQ_COMP_MODULATION_SUPPORT = 0x10,  /**< Modulation compression */
};
#endif // XORIF_COMMON_IQ_COMP_CODES

/**
 * @brief Structure for supported capabilities.
 */
struct xorif_caps
{
    uint16_t max_cc;                    /**< Number of supported component carriers */
    uint16_t num_eth_ports;             /**< Number of supported Ethernet ports */
    uint16_t numerologies;              /**< Supported numerologies (bit-map: bit0 = u0, bit1 = u1, etc.) */
    uint16_t extended_cp;               /**< Extended CP supported (0 = no, 1 = yes) */
    uint16_t iq_de_comp_methods;        /**< Supported IQ de-compression modes (bit-map, see #xorif_iq_comp_flags) */
    uint16_t iq_de_comp_bfp_widths;     /**< Supported bit-widths for BFP de-compression */
    uint16_t iq_de_comp_mod_widths;     /**< Supported bit-widths for modulation de-compression */
    uint16_t iq_comp_methods;           /**< Supported IQ compression modes (bit-map, see #xorif_iq_comp_flags) */
    uint16_t iq_comp_bfp_widths;        /**< Supported bit-widths for BFP compression */
    uint16_t no_framer_ss;              /**< Number of "framer" spatial streams */
    uint16_t no_deframer_ss;            /**< Number of "de-framer" spatial streams */
    uint16_t max_framer_ethernet_pkt;   /**< Maximum size of ethernet payload for "framer" */
    uint16_t max_deframer_ethernet_pkt; /**< Maximum size of ethernet payload for "de-framer" */
    uint16_t max_subcarriers;           /**< Maximum number of subcarriers supported over all component carriers */
    uint16_t max_data_symbols;          /**< Maximum number of symbols for downlink U-plane */
    uint16_t max_ctrl_symbols;          /**< Maximum number of control symbols per component carrier */
    uint16_t max_ul_ctrl_1kwords;       /**< Maximum up-link ctrl 1k words */
    uint16_t max_dl_ctrl_1kwords;       /**< Maximum down-link ctrl 1k words */
    uint16_t max_dl_data_1kwords;       /**< Maximum down-link data 1k words */
    uint16_t max_ssb_ctrl_512words;     /**< Maximum SSB ctrl 512 words */
    uint16_t max_ssb_data_512words;     /**< Maximum SSB data 512 words */
    uint16_t timer_clk_ps;              /**< Timer clk period in pico-seconds */
    uint16_t num_unsolicited_ports;     /**< Number of unsolicited port on the "framer" */
    uint16_t num_prach_ports;           /**< Number of PRACH ports on the "framer" */
    uint16_t du_id_limit;               /**< Maximum size (in bits) of the DU id */
    uint16_t bs_id_limit;               /**< Maximum size (in bits) of the BS id */
    uint16_t cc_id_limit;               /**< Maximum size (in bits) of the CC id */
    uint16_t ru_id_limit;               /**< Maximum size (in bits) of the RU id */
    uint16_t ss_id_limit;               /**< Maximum size (in bits) of the spatial stream id */
    uint16_t ru_ports_map_width;        /**< Width of RU port mapping table */
};

/**
 * @brief Structure for the requested component carrier configuration.
 */
struct xorif_cc_config
{
    uint16_t num_rbs;                /**< Number of RBs */
    uint16_t num_rbs_ssb;            /**< Number of RBs for SSB (should always be 20) */
    uint16_t numerology;             /**< Numerology */
    uint16_t extended_cp;            /**< Extended CP (numerology 2 only) */
    uint16_t numerology_ssb;         /**< Numerology for SSB */
    uint16_t extended_cp_ssb;        /**< Extended CP for SSB (numerology 2 only) */
    uint16_t iq_comp_meth_ul;        /**< IQ compression method for uplink */
    uint16_t iq_comp_width_ul;       /**< IQ compressed width for uplink */
    uint16_t iq_comp_mplane_ul;      /**< Flag indicating M-plane compression for uplink */
    uint16_t iq_comp_meth_dl;        /**< IQ compression method for downlink */
    uint16_t iq_comp_width_dl;       /**< IQ compressed width for downlink */
    uint16_t iq_comp_mplane_dl;      /**< Flag indicating M-plane compression for downlink */
    uint16_t iq_comp_meth_ssb;       /**< IQ compression method for SSB */
    uint16_t iq_comp_width_ssb;      /**< IQ compressed width for SSB */
    uint16_t iq_comp_mplane_ssb;     /**< Flag indicating M-plane compression for SSB */
    double delay_comp_cp_ul;         /**< Delay compenstation (deskew) for uplink C-plane (in microseconds) */
    double delay_comp_cp_dl;         /**< Delay compenstation (deskew) for downlink C-plane (in microseconds) */
    double delay_comp_up;            /**< Delay compenstation (deskew) for downlink U-plane (in microseconds) */
    double advance_ul;               /**< Control time advance for uplink (in microseconds) */
    double advance_dl;               /**< Control time advance for downlink (in microseconds) */
    double ul_bid_forward;           /**< Uplink beam-id forward time (in microseconds) */
    double ul_radio_ch_dly;          /**< Uplink radio channel delay (in microseconds) */
    uint16_t num_ctrl_per_sym_ul;    /**< Number of control words per symbol for uplink */
    uint16_t num_ctrl_per_sym_dl;    /**< Number of control words per symbol for downlink */
    uint16_t num_ctrl_per_sym_ssb;   /**< Number of control words per symbol for SSB */
    uint16_t num_sect_per_sym;       /**< Number of sections per symbol for downlink */
    uint16_t num_sect_per_sym_ssb;   /**< Number of sections per symbol for SSB */
    uint16_t num_frames_per_sym;     /**< Number of Ethernet frames per symbol for downlink */
    uint16_t num_frames_per_sym_ssb; /**< Number of Ethernet frames per symbol for SSB */
};

/**
 * @brief Structure for the actual allocated component carrier configuration.
 */
struct xorif_cc_alloc
{
    uint16_t ul_ctrl_sym_num;      /**< Number of uplink ctrl symbols */
    uint16_t dl_ctrl_sym_num;      /**< Number of downlink ctrl symbols */
    uint16_t dl_data_sym_num;      /**< Number of downlink data symbols */
    uint16_t ssb_ctrl_sym_num;     /**< Number of SSB ctrl symbols */
    uint16_t ssb_data_sym_num;     /**< Number of SSB data symbols */
    uint16_t ul_ctrl_offset;       /**< Uplink ctrl (section memory) offset */
    uint16_t ul_ctrl_size;         /**< Uplink ctrl (section memory) size */
    uint16_t ul_ctrl_base_offset;  /**< Uplink base (packet forming buffer) offset */
    uint16_t ul_ctrl_base_size;    /**< Uplink base (packet forming buffer) size */
    uint16_t dl_ctrl_offset;       /**< Downlink ctrl (section memory) offset */
    uint16_t dl_ctrl_size;         /**< Downlink ctrl (section memory) offset size */
    uint16_t dl_data_ptrs_offset;  /**< Downlink data symbol pointers offset */
    uint16_t dl_data_ptrs_size;    /**< Downlink data symbol pointers size */
    uint16_t dl_data_buff_offset;  /**< Downlink data symbol buffer offset */
    uint16_t dl_data_buff_size;    /**< Downlink data symbol buffer size  */
    uint16_t ssb_ctrl_offset;      /**< SSB ctrl offset */
    uint16_t ssb_ctrl_size;        /**< SSB ctrl offset size */
    uint16_t ssb_data_ptrs_offset; /**< SSB data symbol pointers offset */
    uint16_t ssb_data_ptrs_size;   /**< SSB data symbol pointers size */
    uint16_t ssb_data_buff_offset; /**< SSB data buffer offset */
    uint16_t ssb_data_buff_size;   /**< SSB data buffer size */
};

/**
 * @brief Structure for Front-Haul Interface Ethernet statistic information.
 */
struct xorif_fhi_eth_stats
{
    uint64_t total_rx_good_pkt_cnt; /**< Total received good packets */
    uint64_t total_rx_bad_pkt_cnt;  /**< Total received bad packets */
    uint64_t total_rx_bad_fcs_cnt;  /**< Total received packets with bad FCS */
    uint64_t total_rx_bit_rate;     /**< Total received packets bit rate */
    uint64_t oran_rx_bit_rate;      /**< O-RAN (either U- or C-Plane) received packets bit rate */
    uint64_t oran_rx_total;         /**< O-RAN (either U- or C-Plane) total received packets */
    uint64_t oran_rx_on_time;       /**< O-RAN (either U- or C-Plane) on-time received packets */
    uint64_t oran_rx_early;         /**< O-RAN (either U- or C-Plane) received packets arrived before the start of time window */
    uint64_t oran_rx_late;          /**< O-RAN (either U- or C-Plane) received packets arrived after the end of time window */
    uint64_t oran_rx_total_c;       /**< O-RAN C-Plane total received packets */
    uint64_t oran_rx_on_time_c;     /**< O-RAN C-Plane on-time received packets */
    uint64_t oran_rx_early_c;       /**< O-RAN C-Plane received packets arrived before the start of time window */
    uint64_t oran_rx_late_c;        /**< O-RAN C-Plane received packets arrived after the end of time window */
    uint64_t oran_rx_corrupt;       /**< O-RAN (either U- or C-Plane) on_time received packets carrying corrupted transport header fields */
    uint64_t oran_rx_error_drop;    /**< O-RAN (either U- or C-Plane) discarded received packets */
    uint64_t oran_tx_total;         /**< O-RAN (either U- or C-Plane) total transmitted packets */
    uint64_t oran_tx_total_c;       /**< O-RAN C-Plane total transmitted packets */
    uint16_t offset_earliest_u_pkt; /**< Largest captured difference between the the symbol number transported by a U-Plane packet and the internal timer counter */
    uint16_t offset_earliest_c_pkt; /**< Largest captured difference between the the symbol number transported by a C-Plane packet and the internal timer counter */
};

/**
 * @brief Enumerations for Front-Haul Interface alarm/status information.
 */
enum xorif_fhi_alarms
{
    FRAMER_RESET_STATUS = 0x1,             /**< "Framer" reset status */
    DEFRAMER_RESET_STATUS = 0x2,           /**< "De-framer" reset status */
    DEFRAMER_IN_FIFO_OF = 0x100,           /**< "De-framer" input FIFO over-flow */
    DEFRAMER_IN_FIFO_UF = 0x200,           /**< "De-framer" input FIFO under-flow */
    DEFRAMER_ETH_CIRC_BUFF_OF = 0x400,     /**< "De-framer" Ethernet circular buffer over-flow */
    DEFRAMER_ETH_CIRC_BUFF_PTR_OF = 0x800, /**< "De-framer" Ethernet circular buffer pointer over-flow */
    FRAMER_OUT_FIFO_OF = 0x1000,           /**< "Framer" output FIFO over-flow */
    FRAMER_OUT_FIFO_UF = 0x2000,           /**< "Framer" output FIFO under-flow */
    FRAMER_PRACH_SECTION_OF = 0x4000,      /**< PRACH section over-flow */
    FRAMER_PRACH_SECTION_NF = 0x8000,      /**< PRACH section not-found */
    AXI_TIMEOUT = 0x80000000,              /**< AXI time-out */
};

/**
 * @brief Enumerations for supported Front-Haul Interface transport protocols.
 */
enum xorif_transport_protocol
{
    PROTOCOL_ECPRI = 0,       /**< eCPRI */
    PROTOCOL_IEEE_1914_3 = 1, /**< IEEE 1914.3 */
};

/**
 * @brief Enumerations for supported Front-Haul Interface IP modes.
 */
enum xorif_ip_mode
{
    IP_MODE_RAW = 0,  /**< Raw */
    IP_MODE_IPV4 = 1, /**< IPv4 */
    IP_MODE_IPV6 = 3, /**< IPv6 */
};

/**
 * @brief Type definition for alarm interrupt call-back function.
 */
typedef void (*isr_func_t)(uint32_t status);

/**
 * @brief Structure for Front-Haul specific system "constants".
 */
struct xorif_system_constants
{
    double FH_DECAP_DLY;    /**< See PG370. */
    // TBD Add more as required
};

/**********************************************/
/*** Function prototypes (common interface) ***/
/**********************************************/

/**
 * @brief Returns the current state of the library.
 * @returns
 *      - 0 = not operational
 *      - 1 = operational
 */
int xorif_get_state(void);

/**
 * @brief Set debug level for the library.
 * @param[in] level Debug level (0 = no debug, 1 = minimal, 2 = verbose)
 */
void xorif_debug(int level);

/**
 * @brief Initialize the API s/w.
 * @param[in] device_name Device name of the Front-Haul Interface (leave as NULL for automatic)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * If the device_name is left as NULL, then the library will search the devices
 * for the most appropriate one. Specify the exact device name if you want to
 * override this behavior.
 */
int xorif_init(const char *device_name);

/**
 * @brief Finalize / close the API s/w.
 */
void xorif_finish(void);

/**
 * @brief Return the s/w version.
 * @returns
 *      - Version (major = bits[24..31], minor = bits[16..23], version = bits[8..15])
 */
uint32_t xorif_get_sw_version(void);

/**
 * @brief Get Front-Haul Interface capabilities.
 * @returns
 *      - Pointer to capabilities structure
 */
const struct xorif_caps *xorif_get_capabilities(void);

/**
 * @brief Check if there is a Front-Haul Interface in the system.
 * @returns
 *      - 0 = false (no)
 *      - 1 = true (yes)
 */
int xorif_has_front_haul_interface(void);

/**
 * @brief Configure a component carrier.
 * @param[in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_configure_cc(uint16_t cc);

/**
 * @brief Enables the specified component carrier.
 * @param[in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_enable_cc(uint16_t cc);

/**
 * @brief Disables the specified component carrier.
 * @param[in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_disable_cc(uint16_t cc);

/**
 * @brief Gets a bit-map of enabled component carriers.
 * @returns
 *      - Bit-map of the enabled component carriers
 * @note
 * bit n == 0 means component carrier n disabled
 * bit n == 1 means component carrier n enabled
 */
uint8_t xorif_get_enabled_cc_mask(void);

/**
 * @brief Set the configuration for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] ptr Pointer to configuration structure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_config(uint16_t cc, const struct xorif_cc_config *ptr);

/**
 * @brief Get the configuration for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in,out] ptr Pointer to component carrier configuration structure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_get_cc_config(uint16_t cc, struct xorif_cc_config *ptr);

/**
 * @brief Set the number of RBs for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] num_rbs Number of RBs
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_num_rbs(uint16_t cc, uint16_t num_rbs);

/**
 * @brief Set the numerology for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] numerology Numerology
 * @param[in] extended_cp Extended CP mode (0 = no, 1 = yes) (numerology 2 only)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_numerology(uint16_t cc, uint16_t numerology, uint16_t extended_cp);

/**
 * @brief Set the numerology for the component carrier's SSB.
 * @param[in] cc Component carrier to configure
 * @param[in] numerology Numerology
 * @param[in] extended_cp Extended CP mode (0 = no, 1 = yes) (numerology 2 only)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_numerology_ssb(uint16_t cc, uint16_t numerology, uint16_t extended_cp);

/**
 * @brief Set the maximum deskew and control advance timing for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] deskew Maximum deskew time (in microseconds)
 * @param[in] advance_ul Control advance in uplink (in microseconds) (c.f. T2A_MIN_CP_UL)
 * @param[in] advance_dl Control advance in downlink (in microseconds) (c.f. TCP_ADV_DL)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * See PG370 for details.
 * Note: This function is deprecated. Use xorif_set_cc_ul_timing_parameters() and
 * xorif_set_cc_dl_timing_parameters().
 */
int xorif_set_cc_time_advance(uint16_t cc,
                              double deskew,
                              double advance_ul,
                              double advance_dl);

/**
 * @brief Set the uplink timing parameters for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] delay_comp_cp Delay compensation for C-Plane (in microseconds)
 * @param[in] advance Control time advance (in microseconds) (i.e. T2A_MIN_CP_UL)
 * @param[in] radio_ch_delay Total delay from output of FH to "air" (in microseconds)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * See PG370 for details.
 * The delay_comp_cp value is the size of the valid reception window for C-Plane
 * packets, and is equivalent to (i.e. T2A_MAX_CP_UL - T2A_MIN_CP_UL).
 * The advance value is equivalent to T2A_MIN_CP_UL.
 * The radio_ch_delay value defines the time offset between the 10ms strobe
 * and the actual "air" strobe; it compensates for the system delay for beam-
 * former, DFE, etc.
 */
int xorif_set_cc_ul_timing_parameters(uint16_t cc,
                                      double delay_comp_cp,
                                      double advance,
                                      double radio_ch_delay);

/**
 * @brief Set the downlink timing parameters for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] delay_comp_cp Delay compensation for C-Plane (in microseconds)
 * @param[in] delay_comp_up Delay compensation for U-Plane (in microseconds)
 * @param[in] advance Control time advance (in microseconds) (i.e. TCP_ADV_DL)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * See PG370 for details.
 * The delay_comp_cp value is the size of the valid reception window for C-Plane
 * packets, and is equivalent to (i.e. T2A_MAX_CP_DL - T2A_MIN_CP_DL).
 * The delay_comp_up value is the size of the valid reception window for U-Plane
 * packets, and is equivalent to (i.e. T2A_MAX_UP - T2A_MIN_UP).
 * The advance value is equivalent to TCP_ADV_DL.
 */
int xorif_set_cc_dl_timing_parameters(uint16_t cc,
                                      double delay_comp_cp,
                                      double delay_comp_up,
                                      double advance);

/**
 * @brief Set the uplink beam-id forward time for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] advance Uplink beam-id forward time (in microseconds)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure.
 * See PG370 for details.
 */
int xorif_set_ul_bid_forward(uint16_t cc, double time);

/**
 * @brief Set the uplink radio channel delay estimate for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] delay Delay estimate (in microseconds)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure.
 * See PG370 for details.
 * Note: This function is deprecated. Use xorif_set_cc_ul_timing_parameters().
 */
int xorif_set_ul_radio_ch_dly(uint16_t cc, double delay);

/**
 * @brief Configure the downlink IQ compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width (0-16)
 * @param[in] comp_method Compression method (see #xorif_iq_comp)
 * @param[in] mplane Flag indicating M-plane (1) or C-plane (0) configuration
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * For alignment with O-RAN standard, a bit_width value of 0 is equivalent to 16.
 * The 'mplane' indicates M-plane (static) or C-plane (dynamic) configuration.
 * With static configuration, the M-plane is used to configure the compression
 * mode using the supplied values (bit_width and comp_method). With dynamic
 * configuration, the C-plane is used to configure the compression. However, the
 * supplied values (bit_width and comp_method) are still used for internal
 * buffer sizing.
 */
int xorif_set_cc_dl_iq_compression(uint16_t cc,
                                   uint16_t bit_width,
                                   enum xorif_iq_comp comp_method,
                                   uint16_t mplane);

/**
 * @brief Configure the uplink IQ compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width (0-16)
 * @param[in] comp_method Compression method (see #xorif_iq_comp)
 * @param[in] mplane Flag indicating M-plane (1) or C-plane (0) configuration
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * For alignment with O-RAN standard, a bit_width value of 0 is equivalent to 16.
 * The 'mplane' indicates M-plane (static) or C-plane (dynamic) configuration.
 * With static configuration, the M-plane is used to configure the compression
 * mode using the supplied values (bit_width and comp_method). With dynamic
 * configuration, the C-plane is used to configure the compression. However, the
 * supplied values (bit_width and comp_method) are still used for internal
 * buffer sizing.
 */
int xorif_set_cc_ul_iq_compression(uint16_t cc,
                                   uint16_t bit_width,
                                   enum xorif_iq_comp comp_method,
                                   uint16_t mplane);

/**
 * @brief Configure the SSB compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width (0-16)
 * @param[in] comp_method Compression method (see #xorif_iq_comp)
 * @param[in] mplane Flag indicating M-plane (1) or C-plane (0) configuration
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * For alignment with O-RAN standard, a bit_width value of 0 is equivalent to 16.
 * The 'mplane' indicates M-plane (static) or C-plane (dynamic) configuration.
 * With static configuration, the M-plane is used to configure the compression
 * mode using the supplied values (bit_width and comp_method). With dynamic
 * configuration, the C-plane is used to configure the compression. However, the
 * supplied values (bit_width and comp_method) are still used for internal
 * buffer sizing.
 */
int xorif_set_cc_iq_compression_ssb(uint16_t cc,
                                    uint16_t bit_width,
                                    enum xorif_iq_comp comp_method,
                                    uint16_t mplane);

/**
 * @brief Configure the number of sections and ctrl words per downlink symbol.
 * @param[in] cc Component carrier to configure
 * @param[in] num_sect Maximmum number of sections per symbol
 * @param[in] num_ctrl Maximmum number of control words per symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * The num_sect and num_ctrl values are used in determining buffer sizes.
 * The num_ctrl value can vary depending on the Section Type, for example
 * 1 control word for Type 1, 2 words for Type 0 or Type 3, N words for
 * Type 11, etc.
 * See PG370 for details.
 */
int xorif_set_cc_dl_sections_per_symbol(uint16_t cc,
                                        uint16_t num_sect,
                                        uint16_t num_ctrl);

/**
 * @brief Configure the number of sections and ctrl words per uplink symbol.
 * @param[in] cc Component carrier to configure
 * @param[in] num_sect Maximmum number of sections per symbol
 * @param[in] num_ctrl Maximmum number of control words per symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * The num_sect and num_ctrl values are used in determining buffer sizes.
 * The num_ctrl value can vary depending on the Section Type, for example
 * 1 control word for Type 1, 2 words for Type 0 or Type 3, N words for
 * Type 11, etc.
 * See PG370 for details.
 */
int xorif_set_cc_ul_sections_per_symbol(uint16_t cc,
                                        uint16_t num_sect,
                                        uint16_t num_ctrl);

/**
 * @brief Configure the number of Ethernet frames per downlink symbol.
 * @param[in] cc Component carrier to configure
 * @param[in] num_frames Maximmum number of Ethernet frames per symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * The num_frames value is used in determining buffer sizes.
 * See PG370 for details.
 */
int xorif_set_cc_frames_per_symbol(uint16_t cc, uint16_t num_frames);

/**
 * @brief Configure the number of sections and ctrl words per SSB symbol.
 * @param[in] cc Component carrier to configure
 * @param[in] num_sect Maximmum number of sections per symbol
 * @param[in] num_ctrl Maximmum number of control words per symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * The num_sect and num_ctrl values are used in determining buffer sizes.
 * The num_ctrl value can vary depending on the Section Type, for example
 * 1 control word for Type 1, 2 words for Type 0 or Type 3, N words for
 * Type 11, etc.
 * See PG370 for details.
 */
int xorif_set_cc_sections_per_symbol_ssb(uint16_t cc,
                                         uint16_t num_sect,
                                         uint16_t num_ctrl);

/**
 * @brief Configure the number of Ethernet frames allowed per SSB symbol.
 * @param[in] cc Component carrier to configure
 * @param[in] num_frames Maximmum number of Ethernet frames per symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * The num_frames value is used in determining buffer sizes.
 * See PG370 for details.
 */
int xorif_set_cc_frames_per_symbol_ssb(uint16_t cc, uint16_t num_frames);

/**
 * @brief Reset the Front-Haul Interface.
 * @param[in] mode Reset mode (0 = immediate, 1 = hold in reset)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * When the mode = 1, the module is held in the reset state. Call the
 * function again with mode = 0 to release the reset and to reset normally.
 * This can be useful when coordinating the reset with other devices / modules.
 * Use mode = 0 to perform the reset immediately.
 * The reset operation should always succeed.
 */
int xorif_reset_fhi(uint16_t mode);

/**
 * @brief Returns the Front-Haul Interface h/w version.
 * @returns
 *      - Version (major = bits[24..31], minor = bits[16..23], version = bits[8..15])
 */
uint32_t xorif_get_fhi_hw_version(void);

/**
 * @brief Returns the Front-Haul Interface h/w internal revision number.
 * @returns
 *      - Internal revision
 */
uint32_t xorif_get_fhi_hw_internal_rev(void);

/**
 * @brief Get alarms for Front-Haul Interface.
 * @returns
 *      - Bit-map of alarm status (see enum xorif_fhi_alarms)
 */
uint32_t xorif_get_fhi_alarms(void);

/**
 * @brief Clear alarms for Front-Haul Interface.
 */
void xorif_clear_fhi_alarms(void);

/**
 * @brief Clear stats / counters for Front-Haul Interface.
 */
void xorif_clear_fhi_stats(void);

/**
 * @brief Retrieve the configured allocation for the specified component carrier.
 * @param[in] cc Component carrier
 * @param[in,out] ptr Pointer to component carrier allocation structure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note This is mainly for debug / testing.
 */
int xorif_get_fhi_cc_alloc(uint16_t cc, struct xorif_cc_alloc *ptr);

/**
 * @brief Utility function to read a field from the Front-Haul Interface register map.
 * @param[in] name Register field name
 * @param[in,out] val Pointer to write back the read value
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_read_fhi_reg(const char *name, uint32_t *val);

/**
 * @brief Utility function to read a field from the Front-Haul Interface register map.
 * @param[in] name Register field name
 * @param[in] offset Offset to be used with register (useful for repeated register banks)
 * @param[in,out] val Pointer to write back the read value
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_read_fhi_reg_offset(const char *name, uint16_t offset, uint32_t *val);

/**
 * @brief Utility function to write a field to the Front-Haul Interface regsiter map.
 * @param[in] name Register field name
 * @param[in] value The value to write to the register
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_write_fhi_reg(const char *name, uint32_t value);

/**
 * @brief Utility function to write a field to the Front-Haul Interface regsiter map.
 * @param[in] name Register field name
 * @param[in] offset Offset to be used with register (useful for repeated register banks)
 * @param[in] value The value to write to the register
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_write_fhi_reg_offset(const char *name, uint16_t offset, uint32_t value);

/**
 * @brief Get Front-Haul Interface Ethernet statistics for the specified port.
 * @param[in] port Ethernet port
 * @param[in,out] ptr Pointer to component statistics data structure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_get_fhi_eth_stats(int port, struct xorif_fhi_eth_stats *ptr);

/**
 * @brief Set the destination ethernet MAC address for the specified port.
 * @param[in] port Ethernet port
 * @param[in] address MAC address (6 byte array)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_fhi_dest_mac_addr(int port, const uint8_t address[]);

/**
 * @brief Set the source ethernet MAC address for the specified port.
 * @param[in] port Ethernet port
 * @param[in] address MAC address (6 byte array)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_fhi_src_mac_addr(int port, const uint8_t address[]);

/**
 * @brief Set the protocol and IP mode.
 * @param[in] transport Transport protocol (eCPRI, IEEE 1914.3)
 * @param[in] vlan VLAN tagging mode (0 = no VLAN, 1 = add VLAN tag)
 * @param[in] ip_mode IP mode (Raw, IPv4, IPv6)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * In addition to configuring the protocol, this function also configures
 * the packet filters appropriately for all the Ethernet ports.
 * If additional configuration of the packet filter is required,
 * then this should be done after the call to this function.
 */
int xorif_set_fhi_protocol(enum xorif_transport_protocol transport,
                           uint16_t vlan,
                           enum xorif_ip_mode ip_mode);

/**
 * @brief Set the VLAN tag for the specified port.
 * @param[in] port Ethernet port
 * @param[in] id VLAN ID
 * @param[in] dei VLAN DEI
 * @param[in] pcp VLAN PCP
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_fhi_vlan_tag(int port, uint16_t id, uint16_t dei, uint16_t pcp);

/**
 * @brief Set the packet filter for the specified port.
 * @param[in] port Ethernet port
 * @param[in] filter Pointer to an array of 16 x 32-bit words
 * @param[in] mask Pointer to an array of 4 x 16-bit words
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * The packet filter is defined by 4 filter words each 128-bits (16 bytes) wide.
 * In the IP core's register map, each filter word is arranged as 4 x 32-bit words.
 * Each filter word has an associated mask register which indicates whether a
 * specific byte of the filter is used or not (if bit[X] = 0 then byte[X] of the
 * filter word is used; if bit[x] = 1 then byte[x] of the filter word is not used).
 * (See pg370-oran-radio-if document for full details).
 */
int xorif_set_fhi_packet_filter(int port, const uint32_t filter[16], uint16_t mask[4]);

/**
 * @brief Set the eAxC ID (c.f. ecpriRtcid and ecpriPcid)
 * @param[in] du_bits DU ID length (in bits)
 * @param[in] bs_bits BS ID length (in bits)
 * @param[in] cc_bits CC ID length (in bits)
 * @param[in] ru_bits RU ID length (in bits)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * The eAxC ID is 16 bits, and the total length of the 4 fields should equal 16.
 * RU port ids are mapped by one of 2 methods: mask or table.
 * For mask-based mapping see #xorif_set_ru_ports & #xorif_set_ru_ports_alt1
 * For table-based mapping see #xorif_set_ru_ports_table_mode and 
 * #xorif_set_ru_ports_table
 */
int xorif_set_fhi_eaxc_id(uint16_t du_bits,
                          uint16_t bs_bits,
                          uint16_t cc_bits,
                          uint16_t ru_bits);

/**
 * @brief Set the RU port ID bits.
 * @param[in] ru_bits RU ID length (in bits)
 * @param[in] ss_bits Spatial stream length (in bits)
 * @param[in] mask Mask used when testing for user/PRACH/SSB value
 * @param[in] user_val User ID value (identifies regular U-plane spatial streams)
 * @param[in] prach_val PRACH ID value (identifies PRACH spatial streams)
 * @param[in] ssb_val SSB ID value (identifies SSB spatial streams)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * RU port ids are mapped by one of 2 methods: mask or table.
 * This API relates to mask-based RU port mapping.
 * The number of RU bits is set by #xorif_set_fhi_eaxc_id. The present function defines
 * additional masks/values which are used to split the RU ports into user/PRACH/SSB/...
 * spatial streams.
 * The ru_bits define the total size of the RU ID field; the ss_bits define the number of
 * LSBs that are used the specify the spatial stream ID; the remaining MSBs (ru_bits -
 * ss_bits) define a mask which identifies the spatial stream as user/PRACH/SSB/etc.
 * For example, if mask is 0xC0 and ssb_val is 0x80, then if the (ID & 0xC0) == 0x80
 * then the ID relates to SSB spatial streams.
 * ID mapping can be easily disabled for a particular stream, by simply setting the "val"
 * to be greater than "mask". For example, if mask is 0xC0, then setting ssb_val to 0xFFFF
 * causes SSB ID mapping to be disabled.
 */
int xorif_set_ru_ports(uint16_t ru_bits,
                       uint16_t ss_bits,
                       uint16_t mask,
                       uint16_t user_val,
                       uint16_t prach_val,
                       uint16_t ssb_val);

/**
 * @brief Set the RU port ID bits (alternative API, including LTE).
 * @param[in] ru_bits RU ID length (in bits)
 * @param[in] ss_bits Spatial stream length (in bits)
 * @param[in] mask Mask used when testing for user/PRACH/SSB value
 * @param[in] user_val User ID value (identifies regular U-plane spatial streams)
 * @param[in] prach_val PRACH ID value (identifies PRACH spatial streams)
 * @param[in] ssb_val SSB ID value (identifies SSB spatial streams)
 * @param[in] lte_val LTE ID value (identifies LTE spatial streams)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * RU port ids are mapped by one of 2 methods: mask or table.
 * This API relates to mask-based RU port mapping.
 * The number of RU bits is set by #xorif_set_fhi_eaxc_id. The present function defines
 * additional masks/values which are used to split the RU ports into user/PRACH/SSB/...
 * spatial streams.
 * This function is an alternative to #xorif_set_ru_ports, which also provides mapping for
 * LTE spatial streams.
 */
int xorif_set_ru_ports_alt1(uint16_t ru_bits,
                            uint16_t ss_bits,
                            uint16_t mask,
                            uint16_t user_val,
                            uint16_t prach_val,
                            uint16_t ssb_val,
                            uint16_t lte_val);

/**
 * @brief Set the table mapping mode for the RU port id.
 * @param[in] mode RU port id table mapping mode (see notes)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * RU port ids are mapped by one of 2 methods: mask or table.
 * This API relates to table-based RU port mapping.
 * The "mode" parameter allows flexibility in how addresses are mapped to ports.
 * For example, in mode 1 the address is constructed from 'direction + RU bits'.
 * See PG370 for details.
 * Mode 0 = 'RU bits'
 * Mode 1 = 'direction + RU bits'
 * Mode 2 = 'band-sector bits + RU bits'
 * Mode 3 = 'direction + band-sector bits + RU bits'
 * The number of band-sector / RU bits is defined by #xorif_set_fhi_eaxc_id.
 */
int xorif_set_ru_ports_table_mode(uint16_t mode);

/**
 * @brief Reset the RU ports mapping table.
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * This sets the whole mapping table "type" value to "UNKNOWN" (i.e. not-used).
 */
int xorif_clear_ru_ports_table(void);

/**
 * @brief Assign one or more RU port id mappings.
 * @param[in] address The base (external) address to use
 * @param[in] port The base (internal) port to use
 * @param[in] type The port type (see note for values)
 * @param[in] number The number of port mappings in this assignment
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * RU port ids are mapped by one of 2 methods: mask or table.
 * This API relates to table-based RU port mapping.
 * The "address" depends upon the mode (see #xorif_set_ru_ports_table_mode).
 * The "port" is the internal port number to use.
 * The "type" indicates which port group to use:
 *      - 0 = PDXCH
 *      - 1 = PUXCH
 *      - 2 = SSB
 *      - 3 = PRACH
 *      - 4 - LTE
 *      - 5..62 = user-defined
 *      - 63 = UNKNOWN (uses to indicate not-used)
 * The "number" indicates the number of mappings in the assignment.
 * When number > 1, the address and port values are incremented for each mapping.
 * For example, address 0 -> port 8, address 1 -> port 9, address 2 -> port 10,
 * etc.
 */
int xorif_set_ru_ports_table(uint16_t address,
                             uint16_t port,
                             uint16_t type,
                             uint16_t number);

/**
 * @brief Enable / disable Front-Haul Interface interrupts.
 * @param[in] mask Mask (bit-map of interrupt sources)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * The mask bits are the same as in the enum xorif_fhi_alarms.
 */
int xorif_enable_fhi_interrupts(uint32_t mask);

/**
 * @brief Register a call-back function for Front-Haul Interface alarm interrupts.
 * @param[in] callback Function pointer for call-back
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * When an alarm interrupt occurs, the interrupt handler calls the call-back
 * function (if it exists) and then performs default handling to log the error
 * and clear the alarm.
 * The call-back function should be short.
 */
int xorif_register_fhi_isr(isr_func_t callback);

/**
 * @brief Set system "constants".
 * @param[in] ptr Point to system constants structure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * These are system "constants" and should be set prior to configuring carriers.
 */
int xorif_set_system_constants(const struct xorif_system_constants *ptr);

/**
 * @brief Set the symbol strobe source (internal or external)
 * @param[in] source Source (0 = internally generated, 1 = externally generated)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_symbol_strobe_source(uint16_t source);

#endif // XORIF_API_H

/** @} */