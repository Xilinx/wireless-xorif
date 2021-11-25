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
 * @file xorif_fh_func.h
 * @author Steven Dickinson
 * @brief Header file for libxorif Front-Haul Interface (FHI) functions.
 * @addtogroup libxorif
 * @{
 */

#ifndef XORIF_FH_FUNC_H
#define XORIF_FH_FUNC_H

#include <inttypes.h>

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

/***************************/
/*** Function prototypes ***/
/***************************/

/**
 * @brief Perform default initialization of the device.
 */
void xorif_fhi_init_device(void);

/**
 * @brief Returns the number of supported component carriers.
 * @returns
 *      - Number of supported component carriers
 */
int xorif_fhi_get_max_cc(void);

/**
 * @brief Returns the number of Ethernet ports.
 * @returns
 *      - The number of Ethernet ports
 */
int xorif_fhi_get_num_eth_ports(void);

/**
 * @brief Re-load / re-configure the component carrier configuration.
 * @param[in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS
 */
int xorif_fhi_cc_reload(uint16_t cc);

/**
 * @brief Enable the component carrier.
 * @param[in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS
 */
int xorif_fhi_cc_enable(uint16_t cc);

/**
 * @brief Disable the component carrier.
 * @param[in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS
 */
int xorif_fhi_cc_disable(uint16_t cc);

/**
 * @brief Get the "enabled" component carrier bit-map mask.
 * @returns
 *      - Bit-map of enabled component carriers
 */
uint8_t xorif_fhi_get_enabled_mask(void);

/**
 * @brief Set the "enabled" component carrier bit-map mask.
 * @param[in] mask Bit-map of enabled component carriers
 */
void xorif_fhi_set_enabled_mask(uint8_t mask);

/**
 * @brief Sets up the numbers of symbol buckets for each CC.
 * @param[in] cc Component carrier to configure
 * @param[in] dl_data_sym_num Max number of D/L Data symbols for the CC
 * @param[in] dl_data_sym_start_index Index for the start of the CC
 * @param[in] dl_ctrl_sym_num Max number of D/L Ctrl symbols for the CC
 * @param[in] ul_ctrl_sym_num Max number of U/L Ctrl symbols for the CC
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_symbol_pointers(uint16_t cc,
                                      uint16_t dl_data_sym_num,
                                      uint16_t dl_data_sym_start_index,
                                      uint16_t dl_ctrl_sym_num,
                                      uint16_t ul_ctrl_sym_num);

/**
 * @brief Sets up the numbers of symbol buckets for each CC SSB.
 * @param[in] cc Component carrier to configure
 * @param[in] ssb_data_sym_num Max number of SSB Data symbols for the CC
 * @param[in] ssb_data_sym_start_index Index for the start of the CC
 * @param[in] ssb_ctrl_sym_num Max number of SSB Ctrl symbols for the CC
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_symbol_pointers_ssb(uint16_t cc,
                                          uint16_t ssb_data_sym_num,
                                          uint16_t ssb_data_sym_start_index,
                                          uint16_t ssb_ctrl_sym_num);

/**
 * @brief Configure the data offsets for the DL data buffer.
 * @param[in] cc Component carrier to configure
 * @param[in] dl_data_sym_num Max number of D/L data symbols for the CC
 * @param[in] dl_data_sym_start_index Index for the start of the CC
 * @param[in] symbol_buffer_start Start of the D/L data buffer
 * @param[in] symbol_buffer_size Size of buffer assigned to each symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_dl_data_offsets(uint16_t cc,
                                      uint16_t dl_data_sym_num,
                                      uint16_t dl_data_sym_start_index,
                                      uint16_t symbol_buffer_start,
                                      uint16_t symbol_buffer_size);

/**
 * @brief Configure the data offsets for the SSB data buffer.
 * @param[in] cc Component carrier to configure
 * @param[in] ssb_data_sym_num Max number of SSB data symbols for the CC
 * @param[in] ssb_data_sym_start_index Index for the start of the CC
 * @param[in] symbol_buffer_start Start of the SSB data buffer
 * @param[in] symbol_buffer_size Size of buffer assigned to each symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_dl_data_offsets_ssb(uint16_t cc,
                                          uint16_t ssb_data_sym_num,
                                          uint16_t ssb_data_sym_start_index,
                                          uint16_t symbol_buffer_start,
                                          uint16_t symbol_buffer_size);

/**
 * @brief Configure the Numerology and RB count for the CC.
 * @param[in] cc Component carrier to configure
 * @param[in] num_rbs Number of RBS to assign to this CC (max 275)
 * @param[in] numerology Numerology to set for this CC
 * @param[in] sym_per_slot Number of symbols per slot (0 = 14 symbols, 1 = 12 symbols)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_rbs(uint16_t cc,
                          uint16_t num_rbs,
                          uint16_t numerology,
                          uint16_t sym_per_slot);

/**
 * @brief Configure the Numerology and RB count for the CC SSB.
 * @param[in] cc Component carrier to configure
 * @param[in] num_rbs Number of RBS to assign to this CC (max 275)
 * @param[in] numerology Numerology to set for this CC
 * @param[in] sym_per_slot Number of symbols per slot (0 = 14 symbols, 1 = 12 symbols)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_rbs_ssb(uint16_t cc,
                              uint16_t num_rbs,
                              uint16_t numerology,
                              uint16_t sym_per_slot);

/**
 * @brief Configure the downlink IQ compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width
 * @param[in] comp_meth Compression method
 * @param[in] mplane Flag indicating M-plane (1) or C-plane (0) configuration
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_fhi_set_cc_dl_iq_compression(uint16_t cc,
                                       uint16_t bit_width,
                                       enum xorif_iq_comp comp_meth,
                                       uint16_t mplane);

/**
 * @brief Configure the uplink IQ compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width
 * @param[in] comp_meth Compression method
 * @param[in] mplane Flag indicating M-plane (1) or C-plane (0) configuration
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_fhi_set_cc_ul_iq_compression(uint16_t cc,
                                       uint16_t bit_width,
                                       enum xorif_iq_comp comp_meth,
                                       uint16_t mplane);

/**
 * @brief Configure the uplink IQ compression for the component carrier SSB.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width
 * @param[in] comp_meth Compression method
 * @param[in] mplane Flag indicating M-plane (1) or C-plane (0) configuration
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_fhi_set_cc_iq_compression_ssb(uint16_t cc,
                                        uint16_t bit_width,
                                        enum xorif_iq_comp comp_meth,
                                        uint16_t mplane);

/**
 * @brief Configure the uplink control memories.
 * @param[in] cc Component carrier to configure
 * @param[in] num_ctrl_per_sym_ul Number of sections per symbol
 * @param[in] ul_ctrl_offset Uplink control offset
 * @param[in] ul_ctrl_base_offset Uplink control base offset
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_ul_section_mem(uint16_t cc,
                                     uint16_t num_ctrl_per_sym_ul,
                                     uint16_t ul_ctrl_offset,
                                     uint16_t ul_ctrl_base_offset);

/**
 * @brief Configure the downlink control memories.
 * @param[in] cc Component carrier to configure
 * @param[in] num_ctrl_per_sym_dl Number of sections per symbol
 * @param[in] dl_ctrl_offset Downlink control offset
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_dl_section_mem(uint16_t cc,
                                     uint16_t num_ctrl_per_sym_dl,
                                     uint16_t dl_ctrl_offset);

/**
 * @brief Configure the SSB control memories.
 * @param[in] cc Component carrier to configure
 * @param[in] num_ctrl_per_sym_ssb Number of sections per symbol
 * @param[in] ssb_ctrl_offset Downlink control offset
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_section_mem_ssb(uint16_t cc,
                                      uint16_t num_ctrl_per_sym_ssb,
                                      uint16_t ssb_ctrl_offset);

/**
 * @brief Configure the control constants.
 * @param[in] cc Component carrier to configure
 * @param[in] dl_ctrl_sym_num Number of downlink control symbols
 * @param[in] num_ctrl_per_sym_dl Number of control sections per downlink symbol
 * @param[in] ul_ctrl_sym_num Number of uplink control symbols
 * @param[in] num_ctrl_per_sym_ul Number of control sections per uplink symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_ctrl_constants(uint16_t cc,
                                     uint16_t dl_ctrl_sym_num,
                                     uint16_t num_ctrl_per_sym_dl,
                                     uint16_t ul_ctrl_sym_num,
                                     uint16_t num_ctrl_per_sym_ul);

/**
 * @brief Configure the control constants for SSB.
 * @param[in] cc Component carrier to configure
 * @param[in] ssb_ctrl_sym_num Number of downlink control symbols
 * @param[in] num_ctrl_per_sym_ssb Number of control sections per SSB symbol
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_ctrl_constants_ssb(uint16_t cc,
                                         uint16_t ssb_ctrl_sym_num,
                                         uint16_t num_ctrl_per_sym_ssb);

/**
 * @brief Configure the specified component carrier.
 * @param[in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_configure_cc(uint16_t cc);

/**
 * @brief Compute timing advance for the specified component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] numerology Numerology for the specified CC
 * @param[in] sym_per_slot Number of symbols per slot (0 = 14 symbols, 1 = 12 symbols)
 * @param[in] advance_ul Timing advance for uplink (in microseconds, c.f. T2A_MIN_CP_UL)
 * @param[in] advance_dl Timing advance for downlink (in microseconds, c.f. TCP_ADV_DL)
 * @param[in] ul_bid_forward Uplink beam-id forward time (in microseconds)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_configure_time_advance_offsets(uint16_t cc,
                                             uint16_t numerology,
                                             uint16_t sym_per_slot,
                                             double advance_ul,
                                             double advance_dl,
                                             double ul_bid_forward);

/**
 * @brief Compute SSB timing advance for the specified component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] numerology Numerology for the specified CC
 * @param[in] sym_per_slot Number of symbols per slot (0 = 14 symbols, 1 = 12 symbols)
 * @param[in] advance_dl Timing advance for SSB (in microseconds, c.f. TCP_ADV_DL)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_configure_time_advance_offsets_ssb(uint16_t cc,
                                                 uint16_t numerology,
                                                 uint16_t sym_per_slot,
                                                 double advance_dl);

#endif /* XORIF_FH_FUNC_H */

/** @} */