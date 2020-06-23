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

#include <inttypes.h>

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
    REGISTER_NOT_FOUND,                 /**< Unknown register name */
    XORIF_CONFIGURATION_ERRORS = -2000, /**< (Place holder for configuration errors) */
    XORIF_INVALID_CC,                   /**< Component carrier instance is not valid */
    XORIF_INVALID_RBS,                  /**< Number of RBs is not valid */
    XORIF_INVALID_CONFIG,               /**< Invalid configuration (general error) */
    XORIF_INVALID_SECT_PER_SYM,         /**< Invalid number of sections per symbol */
    XORIF_NUMEROLOGY_NOT_SUPPORTED,     /**< Requested numerology is not supported */
    XORIF_EXT_CP_NOT_SUPPORTED,         /**< Extended cyclic prefix mode is not suported */
    XORIF_COMP_MODE_NOT_SUPPORTED,      /**< Requested compression mode is not supported */
    XORIF_MAX_CTRL_SYM_EXCEEDED,        /**< Number of control symbols exceeds available space */
    XORIF_MAX_DATA_SYM_EXCEEDED,        /**< Number of data symbols exceeds available space */
    XORIF_AVAIL_BUFFER_SPACE_EXCEEDED,  /**< Required buffer space exceeds available space */
    XORIF_INVALID_ETH_PORT,             /**< Invalid ethernet port number */
    XORIF_SCHEDULE_TABLE_EXCEEDED,      /**< Out of space in beamformer schedule table */ 
    XORIF_FRAME_WORK_ERRORS = -1000,    /**< (Place holder for library / libmetal errors) */
    XORIF_LIBMETAL_ERROR,               /**< Error with libmetal framework */
    XORIF_LIBMETAL_DEV_ERROR,           /**< Error with libmetal device */
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
    uint16_t no_framer_antenna;         /**< Number of "framer" antennas */
    uint16_t no_deframer_antennas;      /**< Number of "de-framer" antennas */
    uint16_t max_dl_symbols;            /**< Maximum number of symbols for downlink U-plane */
    uint16_t max_framer_ethernet_pkt;   /**< Maximum size of ethernet payload for "framer" */
    uint16_t max_deframer_ethernet_pkt; /**< Maximum size of ethernet payload for "de-framer" */
    uint16_t max_subcarriers;           /**< Maximum number of subcarriers supported over all component carriers */
    uint16_t max_ctrl_symbols;          /**< Maximum number of control symbols per component carrier */
    uint16_t max_ul_ctrl_1kwords;       /**< Maximum up-link ctrl 1k words */
    uint16_t max_dl_ctrl_1kwords;       /**< Maximum down-link ctrl 1k words */
    uint16_t max_dl_data_1kwords;       /**< Maximum down-link data 1k words */
    uint16_t timer_clk_ps;              /**< Timer clk period in pico-seconds */
    uint16_t num_unsolicited_ports;     /**< Number of unsolicited port on the "framer" */
    uint16_t num_prach_ports;           /**< Number of PRACH ports on the "framer" */
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
    uint16_t num_rbs;             /**< Number of RBs */
    uint16_t numerology;          /**< Numerology */
    uint16_t extended_cp;         /**< Extended CP (numerology 2 only) */
    uint16_t iq_comp_meth_ul;     /**< IQ compression mode in uplink */
    uint16_t iq_comp_width_ul;    /**< IQ compressed width in uplink */
    uint16_t iq_comp_meth_dl;     /**< IQ compression mode in downlink */
    uint16_t iq_comp_width_dl;    /**< IQ compressed width in downlink */
    uint16_t bw_comp_meth;        /**< Beam-weight compression mode */
    uint16_t bw_comp_width;       /**< Beam-weight compressed width */
    uint32_t deskew;              /**< Maximum deskew time (in microseconds) */
    uint32_t advance_ul;          /**< Maximum control advance in uplink (in microseconds) */
    uint32_t advance_dl;          /**< Maximum control advance in downlink (in microseconds) */
    uint16_t num_ctrl_per_sym_ul; /**< Number of sections per symbol in uplink */
    uint16_t num_ctrl_per_sym_dl; /**< Number of sections per symbol in uplink */
};

/**
 * @brief Structure for the actual allocated component carrier configuration.
 */
struct xorif_cc_alloc
{
    uint16_t enabled;               /**< Enabled (0 = not enabled, 1 = enabled) */
    uint16_t num_rbs;               /**< Number of RBs */
    uint16_t numerology;            /**< Numerology */
    uint16_t ul_ctrl_sym_num;       /**< Number of symbols required in uplink ctrl buffers */
    uint16_t ul_ctrl_offset;        /**< Uplink ctrl offset */
    uint16_t ul_ctrl_unroll_offset; /**< Uplink ctrl unroll offset */
    uint16_t ul_ctrl_base_offset;   /**< Uplink ctrl base offset */
    uint16_t dl_ctrl_sym_num;       /**< Number of symbols required in downlink ctrl buffers */
    uint16_t dl_ctrl_offset;        /**< Downlink ctrl offset */
    uint16_t dl_ctrl_unroll_offset; /**< Downlink ctrl unroll offset */
    uint16_t dl_data_sym_num;       /**< Number of symbols required in downlink data buffers */
    uint16_t dl_data_sym_start;     /**< Downlink data symbol start */
    uint16_t dl_data_buff_size;     /**< Downlink data buffer size */
    uint16_t dl_data_buff_start;    /**< Downlink data buffer start */
};

/**
 * @brief Structure for Front-Haul Interface statistic information.
 */
struct xorif_fhi_stats
{
    uint32_t total_rx_good_pkt_cnt;     /**< Total Received Good Packets counter */
    uint32_t total_rx_bad_pkt_cnt;      /**< Total Received Bad Packets counter */
    uint32_t total_rx_bad_fcs_cnt;      /**< Total Received Packets with Bad FCS counter */
    uint32_t user_data_rx_packets_cnt;  /**< Data Packets counter */
    uint32_t user_data_rx_good_pkt_cnt; /**< Good Data Packets counter */
    uint32_t user_data_rx_bad_pkt_cnt;  /**< Bad Data Packets counter */
    uint32_t user_data_rx_bad_fcs_cnt;  /**< Data Packets with Bad FCS counter */
    uint32_t user_ctrl_rx_packets_cnt;  /**< Control Packets counter */
    uint32_t user_ctrl_rx_good_pkt_cnt; /**< Good Control Packets counter */
    uint32_t user_ctrl_rx_bad_pkt_cnt;  /**< Bad Control Packets counter */
    uint32_t user_ctrl_rx_bad_fcs_cnt;  /**< Control Packets with Bad FCS counter */
    uint32_t user_data_rx_pkts_rate;    /**< Data Packets Rate */
    uint32_t user_ctrl_rx_pkts_rate;    /**< Control Packets Rate */
};

/**
 * @brief Structure for Front-Haul Interface alarm/status information.
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
    AXU_TIMEOUT = 0x80000000,          /**< AXI time-out */
};

/**********************************************/
/*** Function prototypes (common interface) ***/
/**********************************************/

/**
 * @brief Returns the current state of the library.
 * @returns
 *      - 0 not operational
 *      - 1 operational
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
 *      - 0 if false (no)
 *      - 1 if true (yes)
 */
int xorif_has_front_haul_interface(void);

/**
 * @brief Check if there is a beamformer in the system.
 * @returns
 *      - 0 if false (no)
 *      - 1 if true (yes)
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
 * @brief Enables the specified compinent carrier
 * @param [in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_enable_cc(uint16_t cc);

/**
 * @brief Disables the specified compinent carrier
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
 */
int xorif_set_cc_numerology(uint16_t cc, uint16_t numerology, uint16_t extended_cp);

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
int xorif_set_cc_time_advance(
    uint16_t cc,
    uint32_t deskew,
    uint32_t advance_ul,
    uint32_t advance_dl);

/**
 * @brief Configure the downlink IQ compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width
 * @param[in] comp_meth Compression method
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_dl_iq_compression(
    uint16_t cc,
    uint16_t bit_width,
    enum xorif_iq_comp comp_meth);

/**
 * @brief Configure the uplink IQ compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width
 * @param[in] comp_meth Compression method
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_ul_iq_compression(
    uint16_t cc,
    uint16_t bit_width,
    enum xorif_iq_comp comp_meth);

/**
 * @brief Configure the beam-weight compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width
 * @param[in] comp_meth Compression method
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_cc_bw_compression(
    uint16_t cc,
    uint16_t bit_width,
    enum xorif_bw_comp comp_meth);

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
 * @brief Get statistics from Front-Haul Interface.
 * @param[in,out] ptr Pointer to component statistics data structure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_get_fhi_stats(struct xorif_fhi_stats *ptr);

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
 * @param[in] TBD
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_set_bf_beam_weights(void);

/**
 * @brief Get alarms for Beamformer.
 * @returns
 *      - TBD
 */
uint32_t xorif_get_bf_alarms(void);

/**
 * @brief Clear alarms for Beamformer.
 */
void xorif_clear_bf_alarms(void);

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

#endif // BF_INCLUDED

#endif /* XORIF_API_H */

/** @} */