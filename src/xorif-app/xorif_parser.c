/*
 * Copyright 2020 - 2022 Xilinx, Inc.
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
 * @file xorif_parser.c
 * @author Steven Dickinson
 * @brief Source file for the xorif-app command parser.
 * @addtogroup xorif-app
 * @{
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "xorif_app.h"
#include "xorif_parser.h"

// Variables
char *token[MAX_TOKENS];
int num_tokens = 0;

int match(const char *s1, const char *s2)
{
    // Case-insensitive comparison
    while (*s1 != '\0' && *s2 != '\0')
    {
        if (toupper(*s1) != toupper(*s2))
        {
            return 0;
        }
        ++s1;
        ++s2;
    }

    // Check for same length
    if ((*s1 == '\0') && (*s2 == '\0'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned int parse_integer(int tok_id, unsigned int *ptr)
{
    unsigned int u;
    if (sscanf(token[tok_id], "0x%X", &u) == 1)
    {
        //printf("Hex: %d\n", u);
        *ptr = u;
        return 1;
    }
    else if (sscanf(token[tok_id], "%u", &u) == 1)
    {
        //printf("Int: %d\n", u);
        *ptr = u;
        return 1;
    }
    return 0;
}

unsigned int parse_double(int tok_id, double *ptr)
{
    double d;
    if (sscanf(token[tok_id], "%lf", &d) == 1)
    {
        //printf("Double: %d\n", d);
        *ptr = d;
        return 1;
    }
    return 0;
}

int parse_string(int tok_id, const char **ptr)
{
    *ptr = token[tok_id];
    return 1;
}

/**
 * @brief Parses request into tokens, which are processed as a command.
 * @param[in] request Request input string
 * @param[in,out] response Response output string
 * @returns
 *      - 0 On success
 *      - Error code on failure
 */
static int parse_command(const char *request, char *response)
{
    static char buffer[LINE_BUFF_SIZE];

    // Check length limit
    if (strlen(request) >= LINE_BUFF_SIZE - 1)
    {
        return LINE_SIZE_LIMIT;
    }

    // Make a writeable copy of the request
    strncpy(buffer, request, LINE_BUFF_SIZE - 1);

    // Reset token count
    num_tokens = 0;

    // Tokenize (using whitespace)
    // TODO improve tokenizer to handle things like quoted strings
    char *tok = strtok(buffer, " \t\n\r");
    while (tok)
    {
        token[num_tokens++] = tok;
        tok = strtok(NULL, " \t\n\r");

        // Check token count limit
        if (num_tokens == MAX_TOKENS)
        {
            return TOKEN_COUNT_LIMIT;
        }
    }

    // Process tokens
    if (num_tokens > 0)
    {
        // Find the command (1st token)
        const struct command *ptr = &command_set[0];
        int count = 0;
        while (ptr->name)
        {
            if (match(token[0], ptr->name))
            {
                ++count;
                if (ptr->func)
                {
                    // Execute the command
                    int result = ptr->func(request, response);
                    if (result != UNKNOWN_COMMAND)
                    {
                        return result;
                    }
                }
            }
            ++ptr;
        }

        if (count > 0)
        {
            // We've had at least one "hit" that wasn't handled, so flag as malformed
            return MALFORMED_COMMAND;
        }
        else
        {
            // No handler for this command at all
            return UNKNOWN_COMMAND;
        }
    }

    return SUCCESS;
}

int do_command(const char *format, ...)
{
    static char buffer[LINE_BUFF_SIZE];
    static char response[MAX_BUFF_SIZE];

    // Variable argument handling using vsnprintf to construct the request
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(buffer, sizeof(buffer), format, argptr);
    va_end(argptr);

    // Clear response buffer
    response[0] = '\0';

    // Trim leading white space
    char *request = buffer;
    while(isspace(*request)) request++;

    // Trim trailing white space
    char *end = request + strlen(request) - 1;
    while((end > request) && (isspace(*end))) end--;
    end[1] = '\0';

    // Ignore any comments (identified by '#' character)
    char *p = strchr(request, '#');
    if (p)
    {
        // Terminate the string at the comment character
        *p = '\0';
    }

    // Return without further processing for empty requests strings
    if (strlen(request) == 0)
    {
        return SUCCESS;
    }

    // Parse the request
    int result = parse_command(request, response);

    // Add default 'status' response when none explicitly given
    if (strlen(response) == 0)
    {
        sprintf(response, "status = %d\n", result);
    }
    ASSERT(strlen(response) < MAX_BUFF_SIZE);

    if (remote_host)
    {
        // Send response back to remote host
        // TODO optional formating, e.g. JSON, etc.
        send_to_host(response);
    }

    if (remote_target)
    {
        // Extract "status" from response
        int status;
        if (sscanf(response, "status = %d", &status) == 1)
        {
            if (status == 0)
            {
                // Local host, send to standard output
                printf("%s => %s", request, response);
            }
            return status;
        }
        printf("%s", response);
        return 0;
    }

    return result;
}

/** @} */