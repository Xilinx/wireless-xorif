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
 * @brief Returns the number of uplink spatial streams (antenna ports).
 * @returns
 *      - Number of spatial streams (antenna ports)
 */
int xorif_fhi_get_num_ul_spatial_streams(void);

/**
 * @brief Returns the number of downlink spatial streams (antenna ports).
 * @returns
 *      - Number of spatial streams (antenna ports)
 */
int xorif_fhi_get_num_dl_spatial_streams(void);

/**
 * @brief Returns the number of supported compinent carriers.
 * @returns
 *      - Number of supported compinent carriers
 */
int xorif_fhi_get_max_cc(void);

/**
 * @brief Returns the number of Ethernet ports.
 * @returns
 *      - The number of Ethernet ports
 */
int xorif_fhi_get_num_eth_ports(void);

/**
 * @brief Returns the minimum supported numerology.
 * @returns
 *      - Numerology
 */
int xorif_fhi_get_min_numerology(void);

/**
 * @brief Returns the maximum supported numerology.
 * @returns
 *      - Numerology
 */
int xorif_fhi_get_max_numerology(void);

/**
 * @brief Returns the maximum number of supported number of downlink symbols.
 * @returns
 *      - Number of supported downlink symbols
 */
int xorif_fhi_get_max_dl_symbols(void);

/**
 * @brief Returns the number of supported sections per uplink symbol.
 * @returns
 *      - Number of uplink sections per symbol
 */
int xorif_fhi_get_num_sect_per_sym_ul(void);

/**
 * @brief Returns the number of supported sections per downlink symbol.
 * @returns
 *      - Number of downlink sections per symbol
 */
int xorif_fhi_get_num_sect_per_sym_dl(void);

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
uint16_t xorif_fhi_get_enabled_mask(void);

/**
 * @brief Set the "enabled" component carrier bit-map mask.
 * @param[in] mask Bit-map of enabled component carriers
 */
void xorif_fhi_set_enabled_mask(uint16_t mask);

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
 * @brief Configure the Numerology and RB count for the CC.
 * @param[in] cc Component carrier to configure
 * @param[in] numerology Numerology to set for this CC
 * @param[in] num_rbs Number of RBS to assign to this CC (max 275)
 * @param[in] sym_per_slot Number of symbols per slot (0 = 14 symbols, 1 = 12 symbols)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_rbs(uint16_t cc,
                          uint16_t numerology,
                          uint16_t num_rbs,
                          uint16_t sym_per_slot);

/**
 * @brief Configure the downlink IQ compression for the component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] bit_width Bit width
 * @param[in] comp_meth Compression method
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 */
int xorif_fhi_set_cc_dl_iq_compression(uint16_t cc,
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
int xorif_fhi_set_cc_ul_iq_compression(uint16_t cc,
                                       uint16_t bit_width,
                                       enum xorif_iq_comp comp_meth);

/**
 * @brief Configure the uplink control memories.
 * @param[in] cc Component carrier to configure
 * @param[in] ul_ctrl_offset Uplink control offset
 * @param[in] ul_ctrl_unrolled_offset Iplink control un-rolled offset
 * @param[in] ul_ctrl_base_offset Uplink control base offset
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_ul_section_mem(uint16_t cc,
                                     uint16_t ul_ctrl_offset,
                                     uint16_t ul_ctrl_unrolled_offset,
                                     uint16_t ul_ctrl_base_offset);

/**
 * @brief Configure the downlink control memories.
 * @param[in] cc Component carrier to configure
 * @param[in] dl_ctrl_offset Downlink control offset
 * @param[in] dl_ctrl_unrolled_offset Downlink control un-rolled offset
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_dl_section_mem(uint16_t cc,
                                     uint16_t dl_ctrl_offset,
                                     uint16_t dl_ctrl_unrolled_offset);

/**
 * @brief Configure the control constants.
 * @param[in] cc Component carrier to configure
 * @param[in] numerology Numerology for the specified CC
 * @param[in] num_ctrl_per_sym_dl Number of control sections per downlink symbol
 * @param[in] num_ctrl_per_sym_ul Number of control sections per uplink symbol
 * @param[in] dl_ctrl_sym_num Number of downlink control symbols
 * @param[in] ul_ctrl_sym_num Number of uplink control symbols
 * @param[in] sym_per_slot Number of symbols per slot (0 = 14 symbols, 1 = 12 symbols)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_init_cc_ctrl_constants(uint16_t cc,
                                     uint16_t numerology,
                                     uint16_t num_ctrl_per_sym_dl,
                                     uint16_t num_ctrl_per_sym_ul,
                                     uint16_t dl_ctrl_sym_num,
                                     uint16_t ul_ctrl_sym_num,
                                     uint16_t sym_per_slot);

/**
 * @brief Configure the specified component carrier.
 * @param[in] cc Component carrier to configure
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_configure_cc(uint16_t cc);

/**
 * @brief Retrieve the configured allocation for the specified component carrier.
 * @param[in] cc Component carrier
 * @param[in,out] ptr Pointer to component carrier allocation structure
 */
void xorif_fhi_get_cc_alloc(uint16_t cc, struct xorif_cc_alloc *ptr);

/**
 * @brief Compute timing advance offsets for the specified component carrier.
 * @param[in] cc Component carrier to configure
 * @param[in] numerology Numerology for the specified CC
 * @param[in] sym_per_slot Number of symbols per slot (0 = 14 symbols, 1 = 12 symbols)
 * @param[in] advance_ul Timing advance for uplink (equivalent to T2A_MIN_CP_UL)
 * @param[in] advance_dl Timing advance for downlink (equivalent to TCP_ADV_DL)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int xorif_fhi_configure_time_advance_offsets(uint16_t cc,
                                             uint16_t numerology,
                                             uint16_t sym_per_slot,
                                             uint32_t advance_ul,
                                             uint32_t advance_dl);

#endif /* XORIF_FH_FUNC_H */

/** @} */