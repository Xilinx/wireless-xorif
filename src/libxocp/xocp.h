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
 * @file xocp.h
 * @author Steven Dickinson
 * @brief Header file for main libxocp functions/definitions.
 * @addtogroup libxocp
 * @{
 */

#ifndef XOCP_H
#define XOCP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#ifndef NO_HW
#include <metal/sys.h>
#include <metal/device.h>
#include <metal/io.h>
#include <metal/irq.h>
#else
#define METAL_IRQ_NOT_HANDLED 0
#define METAL_IRQ_HANDLED 1
#endif
#include "xocp_api.h"

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

#define MAX_NAME_LENGTH 64
#define MAX_RBS 275
#define MAX_NUMEROLOGY 4
#define RE_PER_RB 12
#define MAX_SCHEDULE_LENGTH 32 // Entries, 3 words per entry

// Control codes for coloured text in console
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

// Debug / logging macros, etc.
#ifdef DEBUG
#define PERROR(format, ...)                                     \
    {                                                           \
        fprintf(stderr, ANSI_COLOR_RED);                        \
        fprintf(stderr, "XOCP> ERROR: " format, ##__VA_ARGS__); \
        fprintf(stderr, ANSI_COLOR_RESET);                      \
        fflush(stderr);                                         \
    }

#define TRACE(format, ...)                          \
    {                                               \
        if (xocp_trace >= 1)                        \
        {                                           \
            printf("XOCP> " format, ##__VA_ARGS__); \
            fflush(stdout);                         \
        }                                           \
    }

#define INFO(format, ...)                                  \
    {                                                      \
        if (xocp_trace >= 2)                               \
        {                                                  \
            printf("XOCP> DEBUG: " format, ##__VA_ARGS__); \
            fflush(stdout);                                \
        }                                                  \
    }

#define ASSERT(expression) assert(expression)

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
#define ASSERT(expression) assert(expression)
#define ASSERT_V(expression) assert(expression)
#define ASSERT_NV(expression, value) assert(expression)
#endif

/**
 * @brief Structure typedef for holding driver instance state data.
 */
typedef struct
{
    uint16_t instance;          /**< Instance ID */
    uint16_t state;             /**< State (see #xocp_states) */
#ifndef NO_HW
    struct metal_device *dev;   /**< Pointer to libmetal device */
    struct metal_io_region *io; /**< Pointer to libmetal IO region */
#else
    uint32_t *io;               /**< Pointer to fake reg-bank when no HW */
    char name[MAX_NAME_LENGTH]; /**< Storage for fake device name */
#endif
    xocp_isr_func_t callback;   /**< Event ISR callback */
    uint32_t events;            /**< Event flags */
    struct xocp_caps caps;      /**< Device capabilities */
    struct xocp_cc_data cc_cfg[XOCP_MAX_NUM_CC]; /**< Component carrier configuration */
    struct xocp_antenna_data antenna_cfg; /**< Antenna configuration */
    uint8_t prime_cc;           /**< Lowest numerology component carrier */
} xocp_state_t;

/*************************/
/*** Data declarations ***/
/*************************/

extern bool xocp_first_time_flag;
extern int xocp_trace;
extern xocp_state_t xocp_state[XOCP_NUM_INSTANCES];

/*****************************/
/*** Function declarations ***/
/*****************************/

#endif // XOCP_H

/** @} */