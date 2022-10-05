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
 * @file xocp_registers.h
 * @author Steven Dickinson
 * @brief Header file for libxocp register access functions/definitions.
 * @addtogroup libxocp
 * @{
 */

#ifndef XOCP_REGISTERS_H
#define XOCP_REGISTERS_H

#include "xocp.h"
#include "xorif_chan_proc_ctrl.h"

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

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
#define DEV(i)   (xocp_state[(i)].io)
#define NAME(a)  (#a)
#define ADDR(a)  (a##_ADDR)
#define MASK(a)  (a##_MASK)
#define SHIFT(a) (a##_OFFSET)
#define WIDTH(a) (a##_WIDTH)

// Macros to read/write register fields
#define RD_REG(i, a) read_reg(DEV(i), NAME(a), ADDR(a), MASK(a), SHIFT(a), WIDTH(a))
#define WR_REG(i, a, v) write_reg(DEV(i), NAME(a), ADDR(a), MASK(a), SHIFT(a), WIDTH(a), v)
#define RD_REG_OFFSET(i, a, o) read_reg(DEV(i), NAME(a), ADDR(a) + (o), MASK(a), SHIFT(a), WIDTH(a))
#define WR_REG_OFFSET(i, a, o, v) write_reg(DEV(i), NAME(a), ADDR(a) + (o), MASK(a), SHIFT(a), WIDTH(a), v)

// Macros to read/write whole register
#define RD_REG_ALT(i, a) read_reg(DEV(i), NAME(a), ADDR(a), 0xFFFFFFFF, 0, 32)
#define WR_REG_ALT(i, a, v) write_reg(DEV(i), NAME(a), ADDR(a), 0xFFFFFFFF, 0, 32, v)
#define RD_REG_OFFSET_ALT(i, a, o) read_reg(DEV(i), NAME(a), ADDR(a) + (o), 0xFFFFFFFF, 0, 32)
#define WR_REG_OFFSET_ALT(i, a, o, v) write_reg(DEV(i), NAME(a), ADDR(a) + (o), 0xFFFFFFFF, 0, 32, v)

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
uint32_t read_reg(void *io,
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
void write_reg(void *io,
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
const reg_info_t *find_register(const char *name);

#endif // XOCP_REGISTERS_H

/** @} */