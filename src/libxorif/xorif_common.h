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
 * @file xorif_common.h
 * @author Steven Dickinson
 * @brief Header file for libxorif common functions.
 * @addtogroup libxorif
 * @{
 */

#ifndef XORIF_COMMON_H
#define XORIF_COMMON_H

#include <inttypes.h>
#include <metal/sys.h>
#include <metal/device.h>
#include <metal/io.h>
#include <metal/irq.h>
#include "xorif_api.h"

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

// Control codes for coloured text in console
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#ifdef DEBUG
#define PERROR(format, ...)                                         \
    {                                                               \
        fprintf(stderr, ANSI_COLOR_RED);                            \
        fprintf(stderr, "LIBXORIF> ERROR: " format, ##__VA_ARGS__); \
        fprintf(stderr, ANSI_COLOR_RESET);                          \
        fflush(stderr);                                             \
    }

#define TRACE(format, ...)                              \
    {                                                   \
        if (xorif_trace >= 1)                           \
        {                                               \
            printf("LIBXORIF> " format, ##__VA_ARGS__); \
            fflush(stdout);                             \
        }                                               \
    }

#define INFO(format, ...)                                      \
    {                                                          \
        if (xorif_trace >= 2)                                  \
        {                                                      \
            printf("LIBXORIF> DEBUG: " format, ##__VA_ARGS__); \
            fflush(stdout);                                    \
        }                                                      \
    }

#define ASSERT(expression)                                         \
    {                                                              \
        if (!(expression))                                         \
        {                                                          \
            PERROR("Assertion fail: %s %d\n", __FILE__, __LINE__); \
        }                                                          \
    }
#else
#define PERROR(format, ...)
#define TRACE(format, ...)
#define INFO(format, ...)
#define ASSERT(expression)
#endif

/**
 * @brief Structure for device related information, including libmetal pointers
 */
struct xorif_device_info
{
    uint16_t status;            /**< Status (0 = bad, 1 = good) */
    const char *bus_name;       /**< Bus name */
    const char *dev_name;       /**< Device name */
    struct metal_device *dev;   /**< Pointer to libmetal device */
    struct metal_io_region *io; /**< Pointer to libmetal IO region */
};

// Globals
extern uint16_t xorif_state;
extern int xorif_trace;
extern struct xorif_device_info fh_device;
extern struct xorif_fhi_caps fhi_caps;
#ifdef BF_INCLUDED
extern struct xorif_device_info bf_device;
extern struct xorif_bf_caps bf_caps;
#endif // BF_INCLUDED
extern struct xorif_cc_config cc_config[MAX_NUM_CC];
extern struct xorif_caps caps;
extern const int sc_spacing[NUM_NUMEROLOGY];
extern const int slots_per_subframe[NUM_NUMEROLOGY];

/***************************/
/*** Function prototypes ***/
/***************************/
//TBD

#endif /* XORIF_COMMON_H */

/** @} */