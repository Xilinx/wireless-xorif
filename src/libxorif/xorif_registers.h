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
 * @file xorif_registers.h
 * @author Steven Dickinson
 * @brief Header file for libxorif register access functions/definitions.
 * @addtogroup libxorif
 * @{
 */

#ifndef XORIF_REGISTERS_H
#define XORIF_REGISTERS_H

#include "xorif_common.h"
#include "oran_radio_if_v3_1_ctrl.h"

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

// Interrupt macros
#define FHI_INTR_ENABLE_ADDR CFG_DEFM_INT_ENA_INFIFO_OF_ADDR
#define FHI_INTR_STATUS_ADDR CFG_DEFM_INT_INFIFO_OF_ADDR
#define FHI_INTR_MASK (CFG_DEFM_INT_INFIFO_OF_MASK |              \
                       CFG_DEFM_INT_INFIFO_UF_MASK |              \
                       CFG_DEFM_INT_ETH_PIPE_C_BUF_OF_MASK |      \
                       CFG_DEFM_INT_ETH_PIPE_TABLE_OF_MASK |      \
                       CFG_FRAM_INT_OUTFIFO_OF_MASK |             \
                       CFG_FRAM_INT_OUTFIFO_UF_MASK |             \
                       CFG_FRAM_INT_PRACH_SECTION_OVERFLOW_MASK | \
                       CFG_FRAM_INT_PRACH_SECTION_NOTFOUND_MASK | \
                       CFG_AXI_TIMEOUT_STATUS_MASK)

// Start address of packet filter word
#define DEFM_USER_DATA_FILTER_ADDR DEFM_USER_DATA_FILTER_W0_31_0_ADDR

/**
 * @brief Structure contains information on device register field./
 */
typedef struct reg_info
{
    const char *name; /**< Register field name */
    uint32_t addr;    /**< Register field address */
    uint32_t mask;    /**< Register field mask */
    uint8_t shift;    /**< Register field shift */
    uint8_t width;    /**< Register field width */
} reg_info_t;

// Macros to decipher generated reg-map header
#ifdef NO_HW
#define DEV      (fake_reg_bank)
#else
#define DEV      (fh_device.io)
#endif
#define ADDR(a)  (a##_ADDR)
#define MASK(a)  (a##_MASK)
#define SHIFT(a) (a##_OFFSET)
#define WIDTH(a) (a##_WIDTH)

// Macros to read/write register fields
#define READ_REG(a) xorif_read_reg_internal(DEV, #a, ADDR(a), MASK(a), SHIFT(a), WIDTH(a))
#define WRITE_REG(a, v) xorif_write_reg_internal(DEV, #a, ADDR(a), MASK(a), SHIFT(a), WIDTH(a), v)
#define READ_REG_OFFSET(a, o) xorif_read_reg_internal(DEV, #a, ADDR(a) + (o), MASK(a), SHIFT(a), WIDTH(a))
#define WRITE_REG_OFFSET(a, o, v) xorif_write_reg_internal(DEV, #a, ADDR(a) + (o), MASK(a), SHIFT(a), WIDTH(a), v)

// Macros to read/write whole register
#define READ_REG_RAW(a) xorif_read_reg_internal(DEV, #a, (a), 0xFFFFFFFF, 0, 32)
#define READ_REG_RAW_ALT(s, a) xorif_read_reg_internal(DEV, (s), (a), 0xFFFFFFFF, 0, 32)
#define WRITE_REG_RAW(a, v) xorif_write_reg_internal(DEV, #a, (a), 0xFFFFFFFF, 0, 32, v)
#define WRITE_REG_RAW_ALT(s, a, v) xorif_write_reg_internal(DEV, (s), (a), 0xFFFFFFFF, 0, 32, v)

/*****************************/
/*** Function declarations ***/
/*****************************/

/**
 * @brief Read a register / register field.
 * @param io IO region (i.e. memory-mapped register bank)
 * @param name Register name (for debug)
 * @param addr Register address offset (from the device's base address)
 * @param mask Register field mask
 * @param shift Register field shift (i.e. bit position of LSB)
 * @param width Register field width (for debug)
 * @returns
 *      - Value read
 */
uint32_t xorif_read_reg_internal(void *io,
                                 const char *name,
                                 uint32_t addr,
                                 uint32_t mask,
                                 uint8_t shift,
                                 uint8_t width);

/**
 * @brief Write a register / register field.
 * @param io IO region (i.e. memory-mapped register bank)
 * @param name Register name (for debug)
 * @param addr Register address offset (from the device's base address)
 * @param mask Register field mask
 * @param shift Register field shift (i.e. bit position of LSB)
 * @param width Register field width (for debug)
 * @param value Value to write
 */
void xorif_write_reg_internal(void *io,
                              const char *name,
                              uint32_t addr,
                              uint32_t mask,
                              uint8_t shift,
                              uint8_t width,
                              uint32_t value);

/**
 * @brief Find register field in register-map (for direct register access).
 * @param[in] name Name of register
 * @returns
 *      - Pointer to register field info (or NULL if not found)
 * @note
 * The name can also be a numerical value (decimal or hexadecimal) to access
 * the register by offset only (which assumes it is a 32 bit access).
 */
const reg_info_t *xorif_find_register(const char *name);

#endif /* XORIF_REGISTERS_H */

/** @} */
