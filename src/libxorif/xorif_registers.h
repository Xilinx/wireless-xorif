/*
 * Copyright 2020 - 2022 Advanced Micro Devices, Inc.
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

#include <inttypes.h>
#include "oran_radio_if_v2_2_ctrl.h"

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

// Macros to access register map header file
#define NAME(a) (#a)
#define ADDR(a) (a##_ADDR)
#define MASK(a) (a##_MASK)
#define SHIFT(a) (a##_OFFSET)
#define WIDTH(a) (a##_WIDTH)

#ifdef NO_HW
uint32_t fake_reg_bank[0x10000 / 4]; // Fake register bank
#define DEVICE (fake_reg_bank)
#else
#define DEVICE (fh_device.io)
#endif

/**
 * @brief Macro handles read from specified register field.
 */
#define READ_REG(a) read_reg(DEVICE, #a, ADDR(a), MASK(a), SHIFT(a), WIDTH(a))

/**
 * @brief Macro handles read from specified register field (address + offset).
 */
#define READ_REG_OFFSET(a, o) read_reg(DEVICE, #a, ADDR(a) + (o), MASK(a), SHIFT(a), WIDTH(a))

/**
 * @brief Macro handles raw read from specified register address.
 */
#define READ_REG_RAW(a) read_reg_raw(DEVICE, #a, a)
#define READ_REG_RAW_ALT(s, a) read_reg_raw(DEVICE, s, a)

/**
 * @brief Macro handles write to specified register field.
 */
#define WRITE_REG(a, v) write_reg(DEVICE, #a, ADDR(a), MASK(a), SHIFT(a), WIDTH(a), v)

/**
 * @brief Macro handles write to specified register field (address + offset).
 */
#define WRITE_REG_OFFSET(a, o, v) write_reg(DEVICE, #a, ADDR(a) + (o), MASK(a), SHIFT(a), WIDTH(a), v)

/**
 * @brief Macro handles raw write to specified register address.
 */
#define WRITE_REG_RAW(a, v) write_reg_raw(DEVICE, #a, a, v)
#define WRITE_REG_RAW_ALT(s, a, v) write_reg_raw(DEVICE, s, a, v)

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

/***************************/
/*** Function prototypes ***/
/***************************/

#endif /* XORIF_REGISTERS_H */

/** @} */