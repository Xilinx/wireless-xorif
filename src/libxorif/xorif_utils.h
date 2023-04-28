/*
 * Copyright 2020 - 2023 Advanced Micro Devices, Inc.
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
 * @brief Header file for libxorif common utility functions/definitions.
 * @addtogroup libxorif
 * @{
 */

#ifndef XORIF_UTILS_H
#define XORIF_UTILS_H

#include "xorif_common.h"

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

/**
 * @brief Enumerated type for channel type.
 */
enum xorif_chan_type
{
    CHAN_UL = 0, /**< Uplink */
    CHAN_DL,     /**< Downlink */
    CHAN_SSB,    /**< SSB */
    CHAN_PRACH,  /**< PRACH */
};

typedef int (*irq_handler_t)(int id, void *data);

// Macro to perform ceil(x/y)
#define CEIL_DIV(x, y) (((x) + (y) - 1) / (y))

/***************************/
/*** Function prototypes ***/
/***************************/

/**
 * @brief Get the node property from device tree.
 * @param device Path name of the device (e.g. /sys/bus/platform/devices/XYZ)
 * @param property Name of the property
 * @param dest Pointer to destination buffer
 * @param max_len Maximum size of destination buffer
 * @returns
 *      1 - if the property was successfully retrieved
 *      0 - if the property was not found
 * @note
 * Finds the device tree property, e.g. .../devices/XYZ/of_node/property
 * which is written back to the supplied buffer.
 */
int get_node_property(const char *device,
                      const char *property,
                      void *dest,
                      size_t max_len);

/**
 * @brief Get device 'u32' property from device tree file system.
 * @param[in] dev_name Device name
 * @param[in] prop_name Property name
 * @param[in,out] value Pointer to return the read property value
 * @returns
 *      - 0 if not found
 *      - 1 if found
 */
int get_device_property_u32(const char *dev_name,
                            const char *prop_name,
                            uint32_t *value);

/**
 * @brief Get device name, given hint and/or compatible property.
 * @param dev_name The name of the devivce (or a hint/partial name)
 * @param compatible The compatible property
 * @returns
 *      - Pointer to full device name if a match is found
 *      - NULL if no match is found
 * @note
 * The "dev_name" can the full name, e.g. "a0000000.oran_radio_if" or a
 * partial name, e.g. "oran_radio", or can be left as NULL.
 * The "compatible" property is the Linux device compatible property of
 * the required device, or it can also be NULL.
 * Either "dev_name" or "compatible" must be non-NULL.
 */
const char *get_device_name(const char *dev_name, const char *compatible);

/**
 * @brief Adds a device to libmetal framework
 * @param[in,out] device Pointer to structure containing device info
 * @param[in] bus_name Bus name
 * @param[in] dev_name Device name
 * @param[in] irq_handler Interrupt handler (NULL if there is none)
 * @returns
 *      - XORIF_SUCCESS on success
 *      - XORIF_FAILURE on error
 */
int add_device(struct xorif_device_info *device,
               const char *bus_name,
               const char *dev_name,
               irq_handler_t irq_handler);

/**
 * @brief Check that the given numerology is supported.
 * @param[in] numerology Numerology
 * @param[in] extended_cp Extended CP mode (0 = no, 1 = yes) (numerology 2 only)
 * @returns
 *      - 0 if numerology not suported
 *      - 1 if numerology supported
 */
int check_numerology(uint16_t numerology, uint16_t extended_cp);

/**
 * @brief Checks to see if specified IQ (de-)compression mode is supported.
 * @param[in] bit_width Bit width (0-16)
 * @param[in] comp_method Compression method
 * @param[in] chan Channel type (UL, DL, SSB, PRACH)
 * @returns
 *      - 0 if mode not supported
 *      - 1 if mode is supported
 */
int check_iq_comp_mode(uint16_t bit_width, enum xorif_iq_comp comp_method, enum xorif_chan_type chan);

/**
 * @brief Convert number to binary string representation.
 * @param[in] value Input value
 * @param[in] length Length of input value in bits (0..32)
 * @returns
 *      - Pointer to c-string
 */
const char *binary_string(uint32_t value, uint16_t length);

/**
 * @brief Convert number to binary string representation, with mask.
 * @param[in] value Input value
 * @param[in] mask Mask value
 * @param[in] length Length of input value in bits (0..32)
 * @returns
 *      - Pointer to c-string
 */
const char *binary_mask_string(uint32_t value, uint32_t mask, uint16_t length);

/**
 * @brief Initialize a memory allocation pointer.
 * @param[in] ptr Pointer to memory allocation pointer
 * @param[in] offset Start value to use for memory offset
 * @param[in] size Size of total memory allocation
 * @returns
 *      - Memory allocation pointer
 *      - NULL pointer indicates failure to allocate
 * @note
 * The device has various internal memories e.g. symbol buffers, etc.
 * These are shared amongst different component carriers, and each
 * component carrier will require different amounts of memory due to
 * numerology, number of RBs, time advance, etc. To manage this shared
 * resource, a memory allocation system is used.
 * There are several separate memories, and a memory allocation pointer
 * is used to differentiate between them.
 * This function initializes a memory pointer for this purpose.
 * This function will also free any memory from a previous allocation.
 */
void *init_memory_allocator(void **ptr, uint16_t offset, uint16_t size);

/**
 * @brief Allocate a memory block from the total allocation.
 * @param[in,out] ptr Memory allocation pointer
 * @param[in] size Size of required block
 * @param[in] tag Tag for block (e.g. a component carrier ID)
 * @returns
 *      - Offset of allocated block
 *      - -1 if a block of the required size can't be allocated
 */
int alloc_block(void *ptr, uint16_t size, uint16_t tag);

/**
 * @brief Deallocate a memory block (using specified tag value).
 * @param[in] ptr Memory allocation pointer
 * @param[in] tag Tag value
 * @note
 * All allocated blocks associated with the specified tag shall be
 * freed.
 * Any adjacent free blocks shall be merged into a single free block,
 * to avoid memory fragmentation.
 */
void dealloc_block(void *ptr, uint16_t tag);

/**
 * @brief Find the memory allocation offset and size (for specified tag).
 * @param[in] ptr Pointer to allocation structure
 * @param[in] tag Tag value
 * @param[in,out] offset Pointer to write back the offset value
 * @param[in,out] size Pointer to write back the size value
 * @returns
 *      - 0 = no block allocated with specified tag
 *      - 1  = block allocated with specified tag
 * @note
 * Returns the first block - even multiple blocks are assigned.
 */
int get_alloc_block(void *ptr, uint16_t tag, uint16_t *offset, uint16_t *size);

#endif /* XORIF_UTILS_H */

/** @} */