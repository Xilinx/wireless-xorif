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
 * @file xorif_common.h
 * @author Steven Dickinson
 * @brief Header file for libxorif common functions/definitions.
 * @addtogroup libxorif
 * @{
 */

#ifndef XORIF_COMMON_H
#define XORIF_COMMON_H

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef NO_HW
#include <metal/sys.h>
#include <metal/device.h>
#include <metal/io.h>
#include <metal/irq.h>
#else
#define METAL_IRQ_NOT_HANDLED 0
#define METAL_IRQ_HANDLED 1
#endif
#include "xorif_api.h"
#include "xorif_system.h"

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

/**
 * @brief Conditional compilation flags
 */
//#define AUTO_ENABLE /**< When set, component carriers are enabled after configuration */
#define ENABLE_INTERRUPTS /**< When set, interrupt code is enabled */

// Control codes for coloured text in console
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

// Debug / logging macros
#ifdef DEBUG
#define PERROR(format, ...)                                      \
    {                                                            \
        fprintf(stderr, ANSI_COLOR_RED);                         \
        fprintf(stderr, "XORIF> ERROR: " format, ##__VA_ARGS__); \
        fprintf(stderr, ANSI_COLOR_RESET);                       \
        fflush(stderr);                                          \
    }

#define TRACE(format, ...)                           \
    {                                                \
        if (xorif_trace >= 1)                        \
        {                                            \
            printf("XORIF> " format, ##__VA_ARGS__); \
            fflush(stdout);                          \
        }                                            \
    }

#define INFO(format, ...)                                   \
    {                                                       \
        if (xorif_trace >= 2)                               \
        {                                                   \
            printf("XORIF> DEBUG: " format, ##__VA_ARGS__); \
            fflush(stdout);                                 \
        }                                                   \
    }

#define ASSERT(expression)                                                 \
    {                                                                      \
        if (!(expression))                                                 \
        {                                                                  \
            PERROR("Assertion: %s:%d:%s\n", __FILE__, __LINE__, __func__); \
        }                                                                  \
    }

#define ASSERT_V(expression)                                               \
    {                                                                      \
        if (!(expression))                                                 \
        {                                                                  \
            PERROR("Assertion: %s:%d:%s\n", __FILE__, __LINE__, __func__); \
            return;                                                        \
        }                                                                  \
    }

#define ASSERT_NV(expression, value)                                       \
    {                                                                      \
        if (!(expression))                                                 \
        {                                                                  \
            PERROR("Assertion: %s:%d:%s\n", __FILE__, __LINE__, __func__); \
            return value;                                                  \
        }                                                                  \
    }

#else
#define PERROR(format, ...)
#define TRACE(format, ...)
#define INFO(format, ...)
#define ASSERT(expression)
#define ASSERT_V(expression)
#define ASSERT_NV(expression, value)
#endif

// Additional debug logging macros
#ifdef EXTRA_DEBUG
#define LOG(log_file, format, ...)                    \
    {                                                 \
        if (log_file)                                 \
        {                                             \
            fprintf(log_file, format, ##__VA_ARGS__); \
            fflush(log_file);                         \
        }                                             \
    }
#else
#define LOG(log_file, format, ...)
#endif

/**
 * @brief Structure for device related information, including libmetal pointers
 */
struct xorif_device_info
{
    uint16_t status;            /**< Status (0 = bad, 1 = good) */
#ifndef NO_HW
    struct metal_device *dev;   /**< Pointer to libmetal device */
    struct metal_io_region *io; /**< Pointer to libmetal IO region */
#endif
};

// Globals
extern uint16_t xorif_state;
extern int xorif_trace;
extern struct xorif_caps fhi_caps;
extern struct xorif_cc_config cc_config[MAX_NUM_CC];
extern struct xorif_device_info fh_device;
#ifdef NO_HW
extern uint32_t fake_reg_bank[0x10000 / 4];
#endif
#ifdef EXTRA_DEBUG
extern FILE *log_file;
#endif
extern struct xorif_system_constants fhi_sys_const;

/***************************/
/*** Function prototypes ***/
/***************************/

#endif /* XORIF_COMMON_H */

/** @} */