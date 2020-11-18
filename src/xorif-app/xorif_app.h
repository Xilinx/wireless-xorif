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
 * @file xorif_app.h
 * @author Steven Dickinson
 * @brief Main header file for the Xilinx ORAN Radio Interface example application (xorif-app).
 * @addtogroup xorif-app
 * @{
 */

#ifndef XORIF_APP_H
#define XORIF_APP_H

#include <inttypes.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <unistd.h>
#ifdef NO_HW
#include "../libxorif/xorif_api.h"
#else
#include <xorif/xorif_api.h>
//#include "../libxorif/xorif_api.h"
#endif
#include "xorif_app.h"

// Control codes for coloured text in console
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#ifdef DEBUG
#define PERROR(format, ...)                                          \
    {                                                                \
        fprintf(stderr, ANSI_COLOR_RED);                             \
        fprintf(stderr, "XORIF-APP> ERROR: " format, ##__VA_ARGS__); \
        fprintf(stderr, ANSI_COLOR_RESET);                           \
        fflush(stderr);                                              \
    }

#define TRACE(format, ...)                               \
    {                                                    \
        if (trace >= 1)                                  \
        {                                                \
            printf("XORIF-APP> " format, ##__VA_ARGS__); \
            fflush(stdout);                              \
        }                                                \
    }

#define INFO(format, ...)                                       \
    {                                                           \
        if (trace >= 2)                                         \
        {                                                       \
            printf("XORIF-APP> DEBUG: " format, ##__VA_ARGS__); \
            fflush(stdout);                                     \
        }                                                       \
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

// Constants, enums, typedefs, structures, etc.
#define MAX_BUFF_SIZE 2048

/**
 * @brief Error codes.
 */
enum error_codes
{
    PARSER_ERRORS = -100, /**< (Place holder for parser errors) */
    LINE_SIZE_LIMIT,      /**< Line length limit exceeded */
    TOKEN_COUNT_LIMIT,    /**< Token count limit exceeded */
    UNKNOWN_COMMAND,      /**< Unknown command */
    MALFORMED_COMMAND,    /**< Malformed command */
    SYSTEM_ERRORS = -50,  /**< (Place holder for system errors) */
    COMMS_ERROR,          /**< Communication error */
    NO_HARDWARE,          /**< No hardware present */
    SYSTEM_CMD_FAILURE,   /**< Attempt to perform system command failed */
    FILE_NOT_FOUND,       /**< File not found */
    SUCCESS = 0,          /**< Success! No error! */
    FAILURE = 1,          /**< Failure return code */
    TERMINATE,            /**< Termination return code */
    NO_RESULT,            /**< Code indicate that there was no result */
};

// Global variables...
extern int trace;
extern int remote_host;
extern int remote_target;
extern int port;
extern const char *ip_addr_name;
extern const char *eth_device_name;
extern const char *fhi_dev_name;
extern const char *bf_dev_name;

// Function prototypes...
/**
 * @brief Entry point for menu mode.
 * @returns
 *      - SUCCESS on success
 *      - Error code on failure
 */
int do_menu(void);

/**
 * @brief Entry point for file mode.
 * @param[in] name File-name
 * @returns
 *      - SUCCESS on success
 *      - Error code on failure
 */
int do_file(const char *name);

/**
 * @brief Main entry point for the command parser / handler.
 * @param[in] format Format
 * @returns
 *      - SUCCESS on success
 *      - Error code on failure
 * @note This is a variadic function, which handles arguments for "vsprintf".
 */
int do_command(const char *format, ...);

/**
 * @brief Entry point for socket communications (server side).
 * @returns
 *      - SUCCESS on success
 *      - Error code on failure
 */
int do_socket(void);

/**
 * @brief Utility to send request message to the target (i.e. client -> server)
 * @param[in] request Request input string
 * @param[in,out] response Response output string
 * @returns
 *      - SUCCESS on success
 *      - Error code on failure
 */
int send_to_target(const char *request, char *response);

/**
 * @brief Utility to send response message to the host (i.e. server -> client)
 * @param[in] response Response string
 * @returns
 *      - SUCCESS on success
 *      - Error code on failure
 */
int send_to_host(const char *response);

#endif /* XORIF_APP_H */

/** @} */