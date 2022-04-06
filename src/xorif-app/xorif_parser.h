/*
 * Copyright 2020 - 2021 Xilinx, Inc.
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
 * @file xorif_parser.h
 * @author Steven Dickinson
 * @brief Header file for the xorif-app command parser..
 * @addtogroup xorif-app
 * @{
 */

#ifndef XORIF_CMD_PARSER_H
#define XORIF_CMD_PARSER_H

// Constants, enums, typedefs, structures, etc.
#define MAX_TOKENS 64

// Signature of function pointer used for all parsed commands
typedef int (*func_ptr)(const char *, char *);

// Command structure
struct command
{
    const char *name;
    func_ptr func;
    const char *help;
};

// Global variables
extern char *token[MAX_TOKENS];
extern int num_tokens;
extern const struct command command_set[];

// Function definitions

void register_commands(int num, const struct command commands[]);

/**
 * @brief Match two strings.
 * @param[in] s1 First string
 * @param[in] s2 Second string
 * @returns
 *      - 0 if they don't match
 *      - 1 if both strings match
 * @note
 * The strings are compared in a case-insensitive manner.
 */
int match(const char *s1, const char *s2);

/**
 * @brief Simple parser for unsigned integer numbers.
 * @param[in] tok_id The token id
 * @param[in] ptr Pointer to integer variable
 * @returns
 *      - 0 if no valid token was converted
 *      - 1 if successful
 */
unsigned int parse_integer(int tok_id, unsigned int *ptr);

/**
 * @brief Simple parser for double precision floating point numbers.
 * @param[in] tok_id The token id
 * @param[in] ptr Pointer to double variable
 * @returns
 *      - 0 if no valid token was converted
 *      - 1 if successful
 */
unsigned int parse_double(int tok_id, double *ptr);

/**
 * @brief Simple parser for strings.
 * @param[in] tok_id The token id
 * @param[in] ptr Pointer to string variable
 * @returns
 *      - 0 if no valid token was converted
 *      - 1 if successful
 */
int parse_string(int tok_id, const char **ptr);

#endif /* XORIF_CMD_PARSER_H */

/** @} */