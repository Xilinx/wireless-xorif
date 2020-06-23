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
 * @file xorif_utils.h
 * @author Steven Dickinson
 * @brief Header file for libxorif common utility functions.
 * @addtogroup libxorif
 * @{
 */

#ifndef XORIF_UTILS_H
#define XORIF_UTILS_H

#include <inttypes.h>

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

/**
 * @brief Structure contains information on device register field./
 */
struct reg_info
{
    const char *name; /**< Register field name */
    uint32_t addr;    /**< Register field address */
    uint32_t mask;    /**< Register field mask */
    uint16_t shift;   /**< Register field shift */
};

typedef struct reg_info reg_info_t;

/***************************/
/*** Function prototypes ***/
/***************************/

/**
 * @brief Find register field in register-map.
 * @param[in] ptr Pointer to regsiter map
 * @param[in] name Name of register
 * @returns
 *      - Pointer to register field info (or NULL if not found)
 */
const reg_info_t *find_register(const reg_info_t *ptr, const char *name);

/**
 * @brief Utility function to read a field from the specified IO region.
 * @param[in] io Pointer to the IO region structure
 * @param[in] addr Address to use
 * @param[in] mask Mask to be used with the field
 * @param[in] shift Number of bits to shift the field
 * @returns
 *      - Register field value
 */
uint32_t read_reg_raw(struct metal_io_region *io, uint32_t addr, uint32_t mask, uint16_t shift);

/**
 * @brief Utility function to read a field from the specified IO region.
 * @param[in] io Pointer to the IO region structure
 * @param[in] name Register field name (used for debug)
 * @param[in] addr Address to use
 * @param[in] mask Mask to be used with the field
 * @param[in] shift Number of bits to shift the field
 * @returns
 *      - Register field value
 */
uint32_t read_reg(struct metal_io_region *io, const char *name, uint32_t addr, uint32_t mask, uint16_t shift);

/**
 * @brief Utility function to read a field from the specified IO region, with given offset.
 * @param[in] io Pointer to the IO region structure
 * @param[in] name Register field name (used for debug)
 * @param[in] addr Address to use
 * @param[in] offset Offset to use
 * @param[in] mask Mask to be used with the field
 * @param[in] shift Number of bits to shift the field
 * @returns
 *      - Register field value
 */
uint32_t read_reg_offset(struct metal_io_region *io, const char *name, uint32_t addr, uint16_t offset, uint32_t mask, uint16_t shift);

/**
 * @brief Utility function to write a field from the specified IO region.
 * @param[in] io Pointer to the IO region structure
 * @param[in] addr Address to use
 * @param[in] mask Mask to be used with the field
 * @param[in] shift Number of bits to shift the field
 * @param[in] value The value to write to the register
 */
void write_reg_raw(struct metal_io_region *io, uint32_t addr, uint32_t mask, uint16_t shift, uint32_t value);

/**
 * @brief Utility function to write a field from the specified IO region.
 * @param[in] io Pointer to the IO region structure
 * @param[in] name Register field name (used for debug)
 * @param[in] addr Address to use
 * @param[in] mask Mask to be used with the field
 * @param[in] shift Number of bits to shift the field
 * @param[in] value The value to write to the register
 */
void write_reg(struct metal_io_region *io, const char *name, uint32_t addr, uint32_t mask, uint16_t shift, uint32_t value);

/**
 * @brief Utility function to write a field from the specified IO region, with given offset.
 * @param[in] io Pointer to the IO region structure
 * @param[in] name Register field name (used for debug)
 * @param[in] addr Address to use
 * @param[in] offset Offset to use
 * @param[in] mask Mask to be used with the field
 * @param[in] shift Number of bits to shift the field
 * @param[in] value The value to write to the register
 */
void write_reg_offset(struct metal_io_region *io, const char *name, uint32_t addr, uint16_t offset, uint32_t mask, uint16_t shift, uint32_t value);

/**
 * @brief Get the device's full name from given path and short name.
 * @param[in] path Path to search for devices (typically /sys/bus/platform/devices)
 * @param[in] short_name Device name to look for
 * @param[in,out] full_name Pointer to buffer to write-back the full device name
 * @param[in] max_chars Maximum number of characters in name_out buffer
 * @returns
 *      - 0 if not found
 *      - 1 if found
 */
int get_device_name(const char *path, const char *short_name, char *full_name, int max_chars);

/**
 * @brief Adds a device to libmetal framework
 * @param[in,out] device Pointer to structure containing device info
 * @param[in] bus_name Bus name
 * @param[in] dev_name Device name
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int add_device(struct xorif_device_info *device, const char *bus_name, const char *dev_name);

/**
 * @brief Check that the given numerology is supported.
 * @param[in] numerology Numerology
 * @returns
 *      - 0 if numerology not suported
 *      - 1 if numerology supported
 */
int check_numerology(uint16_t numerology);

/**
 * @brief Checks to see if specified IQ compression mode is supported.
 * @param[in] comp_mode Compression mode
 * @returns
 *      - 0 if mode not supported
 *      - 1 if mode is supported
 */
int check_iq_comp_mode(enum xorif_iq_comp comp_mode);

/**
 * @brief Checks to see if specified beamweight compression mode is supported.
 * @param[in] comp_mode Compression mode
 * @returns
 *      - 0 if mode not supported
 *      - 1 if mode is supported
 */
int check_bw_comp_mode(enum xorif_bw_comp comp_mode);

#endif /* XORIF_UTILS_H */

/** @} */