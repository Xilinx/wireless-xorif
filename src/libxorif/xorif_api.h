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
 * @file xorif_api.h
 * @author Steven Dickinson
 * @brief Main header file for the Xilinx ORAN Radio Interface (libxorif).
 * @addtogroup libxorif-api
 * @{
 *
 * C Library API for the Xilinx ORAN Radio Interface (libxorif)
 */

#ifndef XORIF_API_H
#define XORIF_API_H

#ifndef CFFI_CDEF_HDR
#include <inttypes.h>
#endif

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

/**
 * @brief Enumerated type for error/status codes
 */
enum xorif_error_codes
{
    XORIF_OPERATIONAL_ERRORS = -3000,   /**< (Place holder for operational errors) */
    XORIF_NULL_POINTER,                 /**< Attempt to access memory with null pointer */
    XORIF_REGISTER_NOT_FOUND,           /**< Unknown register name */
    XORIF_MEMORY_ALLOCATION_FAIL,       /**< Memory allocation fail */
    XORIF_CONFIGURATION_ERRORS = -2000, /**< (Place holder for configuration errors) */
    XORIF_INVALID_CC,                   /**< Component carrier instance is not valid */
    XORIF_INVALID_RBS,                  /**< Number of RBs is not valid */
    XORIF_INVALID_CONFIG,               /**< Invalid configuration (general error) */
    XORIF_NUMEROLOGY_NOT_SUPPORTED,     /**< Requested numerology is not supported */
    XORIF_COMP_MODE_NOT_SUPPORTED,      /**< Requested compression mode is not supported */
    XORIF_MAX_CTRL_SYM_EXCEEDED,        /**< Number of control symbols exceeds available space */
    XORIF_MAX_DATA_SYM_EXCEEDED,        /**< Number of data symbols exceeds available space */
    XORIF_BUFFER_SPACE_EXCEEDED,        /**< Required buffer space exceeds available space */
    XORIF_INVALID_ETH_PORT,             /**< Invalid ethernet port number */
    XORIF_INVALID_EAXC_ID,              /**< The specified eAxC ID is invalid */
    XORIF_BEAMFORMER_ERRORS = -1500,    /**< (Place holder for beamformer-specific errors) */
    XORIF_SCHEDULE_TABLE_EXCEEDED,      /**< Out of space in beamformer schedule table */
    XORIF_FRAME_WORK_ERRORS = -1000,    /**< (Place holder for library / libmetal errors) */
    XORIF_LIBMETAL_ERROR,               /**< Error with libmetal framework */
    XORIF_OTHER_ERRORS = -500,          /**< (Place holder for other errors) */
    XORIF_INVALID_RESULT = -1,          /**< Represents an invalid result */
    XORIF_SUCCESS = 0,                  /**< Success! No error! */
    XORIF_FAILURE = 1,                  /**< Failure return code */
};

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
 * @brief Enumerated type for beam-weight compression modes (ORAN values).
 */
enum xorif_bw_comp
{
    BW_COMP_NONE = 0,    /**< No compression */
    BW_COMP_BLOCK_FP,    /**< Block floating point compression */
    BW_COMP_BLOCK_SCALE, /**< Block scaling compression */
    BW_COMP_U_LAW,       /**< U-Law compression */
    BW_COMP_BEAMSPACE,   /**< Beamspace compression */
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

/**
 * @brief Enumerated type for supported beam-weight compression modes (bit-map).
 */
enum xorif_bw_comp_flags
{
    BW_COMP_NONE_SUPPORT = 0x01,        /**< No compression */
    BW_COMP_BLOCK_FP_SUPPORT = 0x02,    /**< Block floating point compression */
    BW_COMP_BLOCK_SCALE_SUPPORT = 0x04, /**< Block scaling compression */
    BW_COMP_U_LAW_SUPPORT = 0x08,       /**< U-Law compression */
    BW_COMP_BEAMSPACE_SUPPORT = 0x10,   /**< Beamspace compression */
};

/**
 * @brief Enumerated type for supported numerologies (bit-map)
 */
enum xorif_numerology_flags
{
    NUMEROLOGY_0_SUPPORT = 0x01, /**< Numerology 0 (15kHz) supported */
    NUMEROLOGY_1_SUPPORT = 0x02, /**< Numerology 1 (30kHz) supported */
    NUMEROLOGY_2_SUPPORT = 0x04, /**< Numerology 2 (60kHz) supported */
    NUMEROLOGY_3_SUPPORT = 0x08, /**< Numerology 3 (120kHz) supported */
    NUMEROLOGY_4_SUPPORT = 0x10, /**< Numerology 4 (240kHz) supported */
};

/**
 * @brief Structure for supported capabilities.
 */
struct xorif_caps
{
    uint16_t max_cc;          /**< Maximum number of supported component carriers */
    uint16_t num_eth_ports;   /**< Maximum number of Ethernet ports */
    uint16_t numerologies;    /**< Supported numerologies (bit-map, see #xorif_numerology_flags) */
    uint16_t extended_cp;     /**< Extended CP supported (0 = no, 1 = yes) */
    uint16_t iq_comp_methods; /**< Supported IQ compression modes (bit-map, see #xorif_iq_comp_flags) */
    uint16_t bw_comp_methods; /**< Supported beam-weight compression modes (bit-map, see #xorif_bw_comp_flags) */
};

/**
 * @brief Structure for Front-Haul Interface supported capabilities.
 */
struct xorif_fhi_caps
{
    uint16_t max_cc;                    /**< Number of supported component carriers */
    uint16_t num_eth_ports;             /**< Number of Ethernet ports */
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
};

#ifdef BF_INCLUDED
/**
 * @brief Structure for Beamformer supported capabilities.
 */
struct xorif_bf_caps
{
    uint16_t max_cc;            /**< Number of supported component carriers */
    uint16_t num_eth_ports;     /**< Number of Ethernet ports */
    uint16_t uram_cache;        /**< Number of URAMs per beamweight cache */
    uint16_t num_antennas;      /**< Number of supported antennas */
    uint16_t num_dfes;          /**< Number of supported DFEs */
    uint16_t num_ss_dl;         /**< Number of supported down-link spatial streams */
    uint16_t num_ss_ul;         /**< Number of supportedup-link spatial streams */
    uint16_t external_bw_store; /**< Flag indicates if external beamweight store is supported (0 = no, 1 = yes) */
    uint16_t cache_clk;         /**< Flag indicates that the cache clock is driven by beamformer (0 = no, 1 = yes) */
    uint16_t aie_if_clk;        /**< Flag indicates that the AIE interface clock is driven by beamformer (0 = no, 1 = yes) */
    uint16_t max_bw_ids;        /**< Maximum number of beam weights ids */
};
#endif

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
    uint16_t iq_comp_meth_ul;        /**< IQ compression method in uplink */
    uint16_t iq_comp_width_ul;       /**< IQ compressed width in uplink */
    uint16_t iq_comp_meth_dl;        /**< IQ compression method in downlink */
    uint16_t iq_comp_width_dl;       /**< IQ compressed width in downlink */
    uint16_t iq_comp_meth_ssb;       /**< IQ compression method for SSB */
    uint16_t iq_comp_width_ssb;      /**< IQ compressed width for SSB */
    uint16_t bw_comp_meth;           /**< Beam-weight compression method */
    uint16_t bw_comp_width;          /**< Beam-weight compressed width */
    uint32_t deskew;                 /**< Maximum deskew time (in microseconds) */
    uint32_t advance_ul;             /**< Maximum control advance in uplink (in microseconds) */
    uint32_t advance_dl;             /**< Maximum control advance in downlink (in microseconds) */
    uint16_t num_ctrl_per_sym_ul;    /**< Number of sections per symbol in uplink */
    uint16_t num_ctrl_per_sym_dl;    /**< Number of sections per symbol in downlink */
    uint16_t num_ctrl_per_sym_ssb;   /**< Number of sections per symbol for SSB */
    uint16_t num_frames_per_sym;     /**< Number of Ethernet frames per symbol in downlink */
    uint16_t num_frames_per_sym_ssb; /**< Number of Ethernet frames per symbol for SSB */
};

/**
 * @brief Structure for the actual allocated component carrier configuration.
 */
struct xorif_cc_alloc
{
    uint16_t ul_ctrl_sym_num;          /**< Number of uplink ctrl symbols */
    uint16_t dl_ctrl_sym_num;          /**< Number of downlink ctrl symbols */
    uint16_t dl_data_sym_num;          /**< Number of downlink data symbols */
    uint16_t ssb_ctrl_sym_num;         /**< Number of SSB ctrl symbols */
    uint16_t ssb_data_sym_num;         /**< Number of SSB data symbols */
    uint16_t ul_ctrl_offset;           /**< Uplink ctrl offset */
    uint16_t ul_ctrl_offset_size;      /**< Size of uplink ctrl offset (per symbol) */
    uint16_t ul_ctrl_base_offset;      /**< Uplink base offset */
    uint16_t ul_ctrl_base_offset_size; /**< Size of uplink base offset */
    uint16_t dl_ctrl_offset;           /**< Downlink ctrl offset */
    uint16_t dl_ctrl_offset_size;      /**< Size of downlink ctrl offset (per symbol) */
    uint16_t dl_data_sym_start;        /**< Downlink data symbol pointer start */
    uint16_t dl_data_buff_start;       /**< Downlink data buffer start */
    uint16_t dl_data_buff_size;        /**< Downlink data buffer size (per symbol) */
    uint16_t ssb_ctrl_offset;          /**< SSB ctrl offset */
    uint16_t ssb_ctrl_offset_size;     /**< Size of SSB ctrl offset */
    uint16_t ssb_data_sym_start;       /**< SSB data symbol pointer start */
    uint16_t ssb_data_buff_start;      /**< SSB data buffer start */
    uint16_t ssb_data_buff_size;       /**< SSB data buffer size (per symbol) */
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
    FRAMER_RESET_STATUS = 0x1,         /**< "Framer" reset status */
    DEFRAMER_RESET_STATUS = 0x2,       /**< "De-framer" reset status */
    DEFRAMER_FIFO_OF = 0x100,          /**< "De-framer" FIFO over-flow */
    DEFRAMER_FIFO_UF = 0x200,          /**< "De-framer" FIFO under-flow */
    DEFRAMER_CIRC_BUFF_OF = 0x400,     /**< "De-framer" circular buffer over-flow */
    DEFRAMER_CIRC_BUFF_PTR_OF = 0x800, /**< "De-framer" circular buffer pointer over-flow */
    FRAMER_FIFO_OF = 0x1000,           /**< "Framer" FIFO over-flow */
    FRAMER_FIFO_UF = 0x2000,           /**< "Framer" FIFO under-flow */
    FRAMER_PRACH_SECTION_OF = 0x4000,  /**< PRACH section over-flow */
    FRAMER_PRACH_SECTION_NF = 0x8000,  /**< PRACH section not-found */
    AXI_TIMEOUT = 0x80000000,          /**< AXI time-out */
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
 * @brief Enumerations for supported Front-Haul IP modes.
 */
enum xorif_ip_mode
{
    IP_MODE_RAW = 0,  /**< Raw */
    IP_MODE_IPV4 = 1, /**< IPv4 */
    IP_MODE_IPV6 = 3, /**< IPv6 */
};

#ifdef BF_INCLUDED
/**
 * @brief Structure for Beamformer statistic information.
 */
struct xorif_bf_stats
{
    uint32_t cache_miss_count; /**< Cache miss counter */
};

/**
 * @brief Enumerations for Beamformer alarm/status information.
 */
enum xorif_bf_alarms
{
    CACHE_DL_MISS = 0x1,            /**< Requested downlink beamweights was not found in the cache */
    CACHE_UL_MISS = 0x2,            /**< Requested uplink beamweights was not found in the cache */
    FHI_DL_U_OF = 0x4,              /**< Downlink U-plane FIFO for FHI data was full when a packet was received */
    FHI_DL_C_OF = 0x8,              /**< Downlink C-plane FIFO for FHI input was full when a packet was received */
    FHI_UL_U_OF = 0x10,             /**< Uplink U-plane FIFO for FHI data requests was full when a packet was received */
    FHI_UL_C_OF = 0x20,             /**< Uplink C-plane FIFO for FHI input was full when a packet was received */
    CPLANE_DL_NO_DESC = 0x40,       /**< Downlink C-plane packet exceeded the system limit on overlapping descriptors */
    CPLANE_UL_NO_DESC = 0x80,       /**< Uplink C-plane packet exceeded the system limit on overlapping descriptors */
    BF_DL_RE_MUTEX = 0x100,         /**< Downlink RB had multiple overlapping RE masks which were not mutually exclusive */
    BF_UL_RE_MUTEX = 0x200,         /**< Uplink RB had multiple overlapping RE masks which were not mutually exclusive */
    BWM_DL_BID_UF = 0x400,          /**< Downlink beamweight manager beam ID interface buffer experienced an underflow condition */
    BWM_DL_BID_OF = 0x800,          /**< Downlink beamweight manager beam ID interface buffer experienced an overflow condition */
    BWM_DL_BW_UF = 0x1000,          /**< Downlink beamweight manager beamweight interface buffer experienced an underflow condition */
    BWM_DL_BW_OF = 0x2000,          /**< Downlink beamweight manager beamweight interface buffer experienced an overflow condition */
    BWM_DL_NEXT_BIDN_UF = 0x4000,   /**< Downlink beamweight manager ran out of spare cache addresses */
    BWM_DL_NEXT_BIDN_OF = 0x8000,   /**< Downlink beamweight manager cache address FIFO overflowed */
    BWM_DL_CACHE_W_UF = 0x10000,    /**< Cache write FIFO experienced an underflow condition */
    BWM_DL_CACHE_W_OF = 0x20000,    /**< Cache write FIFO experienced an overflow condition */
    BWM_UL_BID_UF = 0x40000,        /**< Uplink beamweight manager beam ID interface buffer experienced an underflow condition */
    BWM_UL_BID_OF = 0x80000,        /**< Uplink beamweight manager beam ID interface buffer experienced an overflow condition */
    BWM_UL_BW_UF = 0x100000,        /**< Uplink beamweight manager beamweight interface buffer experienced an underflow condition */
    BWM_UL_BW_OF = 0x200000,        /**< Uplink beamweight manager beamweight interface buffer experienced an overflow condition */
    BWM_UL_NEXT_BIDN_UF = 0x400000, /**< Uplink beamweight manager ran out of spare cache addresses */
    BWM_UL_NEXT_BIDN_OF = 0x800000, /**< Uplink beamweight manager cache address FIFO overflowed */
    BWM_UL_CACHE_W_UF = 0x1000000,  /**< Cache write FIFO experienced an underflow condition */
    BWM_UL_CACHE_W_OF = 0x2000000,  /**< Cache write FIFO experienced an overflow condition */
    BWC_DL_BIDN_OF = 0x4000000,     /**< Downlink cache FIFO experienced an overflow condition */
    BWC_UL_BIDN_OF = 0x8000000,     /**< Uplink cache FIFO experienced an overflow condition */
    PRACH = 0x10000000,             /**< PRACH error condition */
    SSB = 0x20000000,               /**< SSB error condition */
};
#endif

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
 * @param[in] fh_dev_name Device name of the Front Haul Interface (leave as NULL for default)
 * @param[in] bf_dev_name Device name of the Beamformer (leave as NULL for default)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_init(const char *fh_dev_name, const char *bf_dev_name);

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
 * @brief Get ORAN Radio Interface capabilities.
 * @returns
 *      - Pointer to capabilities structure
 */
const struct xorif_caps *xorif_get_capabilities(void);

/**
 * @brief Check if there is a front-haul interface in the system.
 * @returns
 *      - 0 = false (no)
 *      - 1 = true (yes)
 */
int xorif_has_front_haul_interface(void);

/**
 * @brief Check if there is a beamformer in the system.
 * @returns
 *      - 0 = false (no)
 *      - 1 = true (yes)
 */
int xorif_has_beamformer(void);

/**
 * @brief Configure a component carrier.
 * @param[in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_configure_cc(uint16_t cc);

/**
 * @brief Enables the specified component carrier
 * @param [in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_enable_cc(uint16_t cc);

/**
 * @brief Disables the specified component carrier
 * @param [in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_disable_cc(uint16_t cc);

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
 * @note This function also sets the same numerology for the SSB. If SSB requires a
 * different numerology, then this can be specified explicitly.
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
 * @note This function is used to specify a numerology when different to the UL/DL numerology.
 */
int xorif_set_cc_numerology_ssb(uint16_t cc, uint16_t numerology, uint16_t extended_cp);

/**
 * @brief Set the maximum deskew and control advance timing for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] deskew Maximum deskew time (in microseconds)
 * @param[in] advance_ul Maximum control advance in uplink (in microseconds)
 * @param[in] advance_dl Maximum control advance in downlink (in microseconds)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_time_advance(uint16_t cc,
                              uint32_t deskew,
                              uint32_t advance_ul,
                              uint32_t advance_dl);

/**
 * @brief Configure the downlink IQ compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width (0-16)
 * @param[in] comp_method Compression method
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * For alignment with O-RAN standard, a bit_width value of 0 is equivalent to 16.
 */
int xorif_set_cc_dl_iq_compression(uint16_t cc,
                                   uint16_t bit_width,
                                   enum xorif_iq_comp comp_method);

/**
 * @brief Configure the uplink IQ compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width (0-16)
 * @param[in] comp_method Compression method
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * For alignment with O-RAN standard, a bit_width value of 0 is equivalent to 16.
 */
int xorif_set_cc_ul_iq_compression(uint16_t cc,
                                   uint16_t bit_width,
                                   enum xorif_iq_comp comp_method);

/**
 * @brief Configure the SSB compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width (0-16)
 * @param[in] comp_method Compression method
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * For alignment with O-RAN standard, a bit_width value of 0 is equivalent to 16.
 */
int xorif_set_cc_iq_compression_ssb(uint16_t cc,
                                   uint16_t bit_width,
                                   enum xorif_iq_comp comp_method);

/**
 * @brief Configure the beam-weight compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width (0-16)
 * @param[in] comp_method Compression method
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * For alignment with O-RAN standard, a bit_width value of 0 is equivalent to 16.
 */
int xorif_set_cc_bw_compression(uint16_t cc,
                                uint16_t bit_width,
                                enum xorif_bw_comp comp_method);

/**
 * @brief Configure the number of sections allowed per downlink symbol.
 * @param[in] cc Component carrier to configure
 * @param[in] num_sections Maximmum number of sections per symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_dl_sections_per_symbol(uint16_t cc, uint16_t num_sections);

/**
 * @brief Configure the number of sections allowed per uplink symbol.
 * @param[in] cc Component carrier to configure
 * @param[in] num_sections Maximmum number of sections per symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_ul_sections_per_symbol(uint16_t cc, uint16_t num_sections);

/**
 * @brief Configure the number of Ethernet frames allowed per (downlink) symbol.
 * @param[in] cc Component carrier to configure
 * @param[in] num_frames Maximmum number of Ethernet frames per symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_frames_per_symbol(uint16_t cc, uint16_t num_frames);

/**
 * @brief Configure the number of sections allowed per SSB symbol.
 * @param[in] cc Component carrier to configure
 * @param[in] num_sections Maximmum number of sections per symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_sections_per_symbol_ssb(uint16_t cc, uint16_t num_sections);

/**
 * @brief Configure the number of Ethernet frames allowed per SSB symbol.
 * @param[in] cc Component carrier to configure
 * @param[in] num_frames Maximmum number of Ethernet frames per symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_frames_per_symbol_ssb(uint16_t cc, uint16_t num_frames);

/***********************************************************/
/*** Function prototypes (Front-Haul Interface specific) ***/
/***********************************************************/

/**
 * @brief Get capabilities from Front-Haul Interface.
 * @returns
 *      - Pointer to capabilities structure
 */
const struct xorif_fhi_caps *xorif_get_fhi_capabilities(void);

/**
 * @brief Reset the Front Haul Interface.
 */
void xorif_reset_fhi(void);

/**
 * @brief Returns the Front Haul Interface h/w version.
 * @returns
 *      - Version (major = bits[24..31], minor = bits[16..23], version = bits[8..15])
 */
uint32_t xorif_get_fhi_hw_version(void);

/**
 * @brief Returns the Front Haul Interface h/w internal revision number.
 * @returns
 *      - Revision
 */
uint32_t xorif_get_fhi_hw_revision(void);

/**
 * @brief Get alarms for Front Haul Interface.
 * @returns
 *      - Bit-map of alarm status (see enum xorif_fhi_alarms)
 */
uint32_t xorif_get_fhi_alarms(void);

/**
 * @brief Clear alarms for Front Haul Interface.
 */
void xorif_clear_fhi_alarms(void);

/**
 * @brief Clear stats / counters for Front Haul Interface.
 */
void xorif_clear_fhi_stats(void);

/**
 * @brief Retrieve the configured allocation for the specified component carrier.
 * @param[in] cc Component carrier
 * @param[in,out] ptr Pointer to component carrier allocation structure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_get_fhi_cc_alloc(uint16_t cc, struct xorif_cc_alloc *ptr);

/**
 * @brief Utility function to read a field from the Front Haul Interface register map.
 * @param[in] name Register field name
 * @param[in,out] val Pointer to write back the read value
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_read_fhi_reg(const char *name, uint32_t *val);

/**
 * @brief Utility function to read a field from the Front Haul Interface register map.
 * @param[in] name Register field name
 * @param[in] offset Offset to be used with register (useful for repeated register banks)
 * @param[in,out] val Pointer to write back the read value
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_read_fhi_reg_offset(const char *name, uint16_t offset, uint32_t *val);

/**
 * @brief Utility function to write a field to the Front Haul Interface regsiter map.
 * @param[in] name Register field name
 * @param[in] value The value to write to the register
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_write_fhi_reg(const char *name, uint32_t value);

/**
 * @brief Utility function to write a field to the Front Haul Interface regsiter map.
 * @param[in] name Register field name
 * @param[in] offset Offset to be used with register (useful for repeated register banks)
 * @param[in] value The value to write to the register
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_write_fhi_reg_offset(const char *name, uint16_t offset, uint32_t value);

/**
 * @brief Get Front Haul Interface Ethernet statistics for the specified port.
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
 * @param[in] tag VLAN tag (ID bits 0-11; DEI bit 12; PCP bits 13-15)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_fhi_vlan_tag(int port, uint16_t tag);

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
 * @brief Enable / disable Front Haul Interface interrupts.
 * @param[in] mask Mask (bit-map of interrupt sources)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * The mask bits are the same as in the enum xorif_fhi_alarms.
 */
int xorif_enable_fhi_interrupts(uint32_t mask);

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
 * See #xorif_set_ru_ports for setting the spatial stream mask/values.
 */
int xorif_set_fhi_eaxc_id(uint16_t du_bits,
                          uint16_t bs_bits,
                          uint16_t cc_bits,
                          uint16_t ru_bits);

/**
 * @brief Set the RU port ID bits (as either user/PRACH/SSB spatial streams).
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
 * The RU bits defined by the call to #xorif_set_fhi_eaxc_id. The present function defines
 * additional masks/values which are used to split the RU ports into user/PRACH/SSB spatial
 * streams.
 * The ru_bits define the total size of the RU ID field; the ss_bits define the number of
 * LSBs that are used the specify the spatial stream ID; the remaining MSBs (ru_bits - ss_bits)
 * define a mask which identifies the spatial stream as user/PRACH/SSB/...
 */
int xorif_set_ru_ports(uint16_t ru_bits,
                       uint16_t ss_bits,
                       uint16_t mask,
                       uint16_t user_val,
                       uint16_t prach_val,
                       uint16_t ssb_val);

/*************************************************/
/*** Function prototypes (Beamformer specific) ***/
/*************************************************/

#ifdef BF_INCLUDED

/**
 * @brief Get capabilities from Beamformer.
 * @returns
 *      - Pointer to capabilities structure
 */
const struct xorif_bf_caps *xorif_get_bf_capabilities(void);

/**
 * @brief Reset the Beaformer.
 */
void xorif_reset_bf(void);

/**
 * @brief Returns the Beamformer h/w version.
 * @returns
 *      - Version (major = bits[24..31], minor = bits[16..23], version = bits[8..15])
 */
uint32_t xorif_get_bf_hw_version(void);

/**
 * @brief Returns the Beamformer h/w internal revision number.
 * @returns
 *      - Revision
 */
uint32_t xorif_get_bf_hw_revision(void);

/**
 * @brief Load beam-weight set.
 * @note This function takes un-packed beam-weight data and control information.
 * @note The function does not compress the data.
 * @param[in] bid Beam-id
 * @param[in] comp_width Compression width (e.g. 12 bits)
 * @param[in] comp_meth Compression method (e.g. BW_COMP_BLOCK_FP = 1)
 * @param[in] comp_param Compression paratemer (related to the compression method)
 * @param[in] num_weights Number of beam-weights to load
 * @param[in] data Pointer to data (array of 32-bit words; I-value as 16 LSBs, and Q-value as 16 MSBs)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_load_bf_beam_weights(uint16_t bid,
                               uint16_t comp_width,
                               uint16_t comp_meth,
                               uint16_t comp_param,
                               uint16_t num_weights,
                               const uint32_t *data);

/**
 * @brief Load beam-weight set (alternative API, for test/debug when no external store)
 * @note This function takes un-packed beam-weight data and control information.
 * @note The function does not compress the data.
 * @param[in] bid Beam-id
 * @param[in] comp_width Compression width (e.g. 12 bits)
 * @param[in] comp_meth Compression method (e.g. BW_COMP_BLOCK_FP = 1)
 * @param[in] comp_param Compression paratemer (related to the compression method)
 * @param[in] ss Spatial stream
 * @param[in] dir Direction (0 = uplink, 1 = downlink)
 * @param[in] type (0 = U-plane, 1 = PRACH, 2 = SSB)
 * @param[in] num_weights Number of beam-weights to load
 * @param[in] data Pointer to data (array of 32-bit words; I-value as 16 LSBs, and Q-value as 16 MSBs)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_load_bf_beam_weights_alt1(uint16_t bid,
                                    uint16_t comp_width,
                                    uint16_t comp_meth,
                                    uint16_t comp_param,
                                    uint16_t ss,
                                    uint16_t dir,
                                    uint16_t type,
                                    uint16_t num_weights,
                                    const uint32_t *data);

/**
 * @brief Load beam-weight set (alternative API, for SSB test/debug when no external store)
 * @note This function takes un-packed beam-weight data and control information.
 * @note The function does not compress the data.
 * @param[in] bid Beam-id (note, beam-id is ignored for this use-case)
 * @param[in] comp_width Compression width (e.g. 12 bits)
 * @param[in] comp_meth Compression method (e.g. BW_COMP_BLOCK_FP = 1)
 * @param[in] comp_param Compression paratemer (related to the compression method)
 * @param[in] cc Component carrier
 * @param[in] sym Symbol number
 * @param[in] srb Start RB
 * @param[in] nrb Number of RBs
 * @param[in] num_weights Number of beam-weights to load
 * @param[in] data Pointer to data (array of 32-bit words; I-value as 16 LSBs, and Q-value as 16 MSBs)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_load_bf_beam_weights_alt2(uint16_t bid,
                                    uint16_t comp_width,
                                    uint16_t comp_meth,
                                    uint16_t comp_param,
                                    uint16_t cc,
                                    uint16_t sym,
                                    uint16_t srb,
                                    uint16_t nrb,
                                    uint16_t num_weights,
                                    const uint32_t *data);

/**
 * @brief Get alarms for Beamformer.
 * @returns
 *      - Bit-map of alarm status (see enum xorif_bf_alarms)
 */
uint32_t xorif_get_bf_alarms(void);

/**
 * @brief Clear alarms for Beamformer.
 */
void xorif_clear_bf_alarms(void);

/**
 * @brief Clear stats / counters for Beamformer.
 */
void xorif_clear_bf_stats(void);

/**
 * @brief Utility function to read a field from the Beamformer register map.
 * @param[in] name Register field name
 * @param[in,out] val Pointer to write back the read value
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_read_bf_reg(const char *name, uint32_t *val);

/**
 * @brief Utility function to read a field from the Beamformer register map.
 * @param[in] name Register field name
 * @param[in] offset Offset to be used with register (useful for repeated register banks)
 * @param[in,out] val Pointer to write back the read value
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_read_bf_reg_offset(const char *name, uint16_t offset, uint32_t *val);

/**
 * @brief Utility function to write a field to the Beamformer regsiter map.
 * @param[in] name Register field name
 * @param[in] value The value to write to the register
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_write_bf_reg(const char *name, uint32_t value);

/**
 * @brief Utility function to write a field to the Beamformer regsiter map.
 * @param[in] name Register field name
 * @param[in] offset Offset to be used with register (useful for repeated register banks)
 * @param[in] value The value to write to the register
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_write_bf_reg_offset(const char *name, uint16_t offset, uint32_t value);

/**
 * @brief Configure beamformer (including component carriers and schedule table).
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_configure_bf(void);

/**
 * @brief Load static PRACH configuration.
 * @param ptr Pointer to configuration data array
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @details
 * The PRACH configuration is static, and default values are loaded during initialization
 * of the beam-former. This API allows alternative values to be used. The function should
 * be called after beam-former initialization, but before configuration and use.
 * The configuration data consists of an array of 64 x 32-bit words. Each pair of words
 * represents the configuration data for a particular PRACH format.
 * Word N
 *      - bits [4:0] - CP length
 *      - bits [5:8] - Number of channels
 *      - bits [9:12] - Number of sequence repetitions
 *      - bits [13:22] - Sequence length
 *      - bits [23:27] - Format ID
 *      - bits [28:31] - Reservered (set to 0)
 * Word N+1
 *      - bits [0:7] - Timeout value 15kHz
 *      - bits [8:15] - Timeout value 30kHz
 *      - bits [16:23] - Computed number of PRBs 15kHz
 *      - bits [24:31] - Computed number of PRBs 30kHz
 */
int xorif_load_bf_prach_config(const uint32_t *ptr);

/**
 * @brief Get statistics from Beamformer.
 * @param[in,out] ptr Pointer to component statistics data structure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_get_bf_stats(struct xorif_bf_stats *ptr);

/**
 * @brief Enable / disable Beamformer interrupts.
 * @param[in] mask Mask (bit-map of interrupt sources)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * The mask bits are the same as in the enum xorif_bf_alarms.
 */
int xorif_enable_bf_interrupts(uint32_t mask);

#endif // BF_INCLUDED

#endif // XORIF_API_H

/** @} */