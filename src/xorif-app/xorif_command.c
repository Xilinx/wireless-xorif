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
 * @file xorif_command.c
 * @author Steven Dickinson
 * @brief Source file for the xorif-app command parser.
 * @addtogroup xorif-app
 * @{
 */

#include "xorif_app.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

// Constants, enums, typedefs, structures, etc.
#define MAX_TOKENS 64
#define PRACH_CONFIG_LENGTH 64

// Signature of function pointer used for all parsed commands
typedef int (*func_ptr)(const char *, char *);

// Command structure
struct command
{
    const char *name;
    func_ptr func;
    const char *usage;
};

// Function prototypes...
static int help(const char *request, char *response);
static int quit(const char *request, char *response);
static int init(const char *request, char *response);
static int finish(const char *request, char *response);
static int reset(const char *request, char *response);
static int get(const char *request, char *response);
static int has(const char *request, char *response);
static int get(const char *request, char *response);
static int set(const char *request, char *response);
static int clear(const char *request, char *response);
static int configure(const char *request, char *response);
static int enable(const char *request, char *response);
static int disable(const char *request, char *response);
static int read_reg(const char *request, char *response);
static int read_reg_offset(const char *request, char *response);
static int write_reg(const char *request, char *response);
static int write_reg_offset(const char *request, char *response);
static int dump_reg(const char *request, char *response);
#ifdef LEGACY_ECPRI
static int ecpri(const char *request, char *response);
#endif // LEGACY_ECPRI
static int peek(const char *request, char *response);
static int poke(const char *request, char *response);
static int debug(const char *request, char *response);
#ifdef LEGACY_ECPRI
int ecpri_func(int argc, char **argv, char *resp);
#endif // LEGACY_ECPRI
#ifdef BF_INCLUDED
static int schedule_bf(const char *request, char *response);
static int load(const char *request, char *response);
static int load_beam_weight_file(const char *name);
#ifdef PRACH_CONFIG
static int load_prach_config(const char *name);
#endif // PRACH_CONFIG
#endif // BF_INCLUDED
static int dump_fhi_register(char *response, const char *reg_name);
static char * dump_reg_fhi(char *response);
#ifdef BF_INCLUDED
static int dump_bf_register(int bank, char *response, const char *reg_name);
static char * dump_reg_bf(int bank, char *response);
#endif // BF_INCLUDED

// Variables
static char *token[MAX_TOKENS];
static int num_tokens = 0;

// Usage strings
#define GET_USAGE_1 "get..."                                \
                    "\n\tget fhi_sw_version"                \
                    "\n\tget fhi_hw_version"                \
                    "\n\tget fhi_hw_internal_rev"           \
                    "\n\tget [fhi_capabilities | fhi_caps]" \
                    "\n\tget fhi_cc_config <cc>"            \
                    "\n\tget fhi_cc_alloc <cc>"             \
                    "\n\tget fhi_stats <port>"              \
                    "\n\tget fhi_alarms"                    \
                    "\n\tget fhi_state"                     \
                    "\n\tget fhi_enabled"

#ifdef BF_INCLUDED
#define GET_USAGE_2 "\n\tget bf_sw_version"               \
                    "\n\tget bf_hw_version"               \
                    "\n\tget bf_hw_internal_rev"          \
                    "\n\tget [bf_capabilities | bf_caps]" \
                    "\n\tget bf_cc_config <cc>"           \
                    "\n\tget bf_cc_alloc <cc>"            \
                    "\n\tget bf_stats"                    \
                    "\n\tget bf_alarms"                   \
                    "\n\tget bf_state"                    \
                    "\n\tget bf_enabled"
#endif

#define SET_USAGE_1 "set..."                                                                                \
                    "\n\tset num_rbs <cc> <number_of_rbs>"                                                  \
                    "\n\tset numerology <cc> <numerology> <extended_cp = 0 | 1>"                            \
                    "\n\tset numerology_ssb <cc> <numerology> <extended_cp = 0 | 1>"                        \
                    "\n\tset time_advance <cc> <deskew> <advance_uplink> <advance_downlink> (deprecated)"   \
                    "\n\tset ul_timing_params <cc> <delay_comp> <advance> <radio_ch_delay>"                 \
                    "\n\tset dl_timing_params <cc> <delay_comp_cp> <delay_comp_up> <advance>"               \
                    "\n\tset ul_bid_forward <cc> <time>"                                                    \
                    "\n\tset ul_bid_forward_fine <cc> <symbols> <cycles>"                                   \
                    "\n\tset ul_radio_ch_dly <cc> <delay> (deprecated)"                                     \
                    "\n\tset [dl_iq_compression | dl_iq_comp] <cc> <width> <method> <mplane = 0 | 1>"       \
                    "\n\tset [ul_iq_compression | ul_iq_comp] <cc> <width> <method> <mplane = 0 | 1>"       \
                    "\n\tset [ssb_iq_compression | ssb_iq_comp] <cc> <width> <method> <mplane = 0 | 1>"     \
                    "\n\tset [prach_iq_compression | prach_iq_comp] <cc> <width> <method> <mplane = 0 | 1>" \
                    "\n\tset dl_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>"          \
                    "\n\tset ul_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>"          \
                    "\n\tset ssb_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>"         \
                    "\n\tset frames_per_sym <cc> <number_of_frames>"                                        \
                    "\n\tset frames_per_sym_ssb <cc> <number_of_frames>"                                    \
                    "\n\tset dest_mac_addr <port> <address>"                                                \
                    "\n\tset src_mac_addr <port> <address>"                                                 \
                    "\n\tset protocol <ECPRI | 1914.3> <VLAN = 0 | 1> <RAW | IPv4 | IPv6>"                  \
                    "\n\tset vlan <port> <id> <dei> <pcp>"                                                  \
                    "\n\tset eaxc_id <DU bits> <BS bits> <CC bits> <RU bits>"                               \
                    "\n\tset ru_ports <RU bits> <ss bits> <mask> <user_value> <prach_value> <ssb_value>"    \
                    "\n\tset ru_ports_table_mode <mode>"                                                    \
                    "\n\tset ru_ports_table <address> <port> <type> [<number>]"                             \
                    "\n\tset system_constants <fh_decap_dly> [...]"

#ifdef BF_INCLUDED
#define SET_USAGE_2 "\n\tset [bw_compression | bw_comp] <cc> <width> <method>"
#endif

#define ECPRI_USAGE "(Use 'ecpri help' for additional help and usage information)"

// Set of commands (must be terminated with NULLs)
const static struct command command_set[] =
{
    {"help", help, "help [<topic>]"},
    {"debug", debug, "debug <level = 0..2>"},
#ifdef BF_INCLUDED
    {"init", init, "init [fhi | bf] [<device_name>]"},
    {"finish", finish, "finish [fhi | bf]"},
    {"get", get, GET_USAGE_1 GET_USAGE_2},
    {"set", set, SET_USAGE_1 SET_USAGE_2},
    {"reset", reset, "reset [fhi | bf] <mode>"},
    {"clear", clear, "clear [fhi_alarms | fhi_stats | bf_alarms | bf_stats]"},
    {"has", has, "has [fhi | bf]"},
    {"configure", configure, "configure [fhi | bf] <cc>"},
    {"enable", enable, "enable [fhi | bf] <cc>"},
    {"disable", disable, "disable [fhi | bf] <cc>"},
    {"schedule_bf", schedule_bf, "schedule_bf"},
    {"load", load, "load [beam_weights] <file>"},
    {"read_reg", read_reg, "read_reg [fhi | bf0 | bf1] <name>"},
    {"read_reg_offset", read_reg_offset, "read_reg_offset [fhi | bf0 | bf1] <name> <offset>"},
    {"write_reg", write_reg, "write_reg [fhi | bf0 | bf1] <name> <value>"},
    {"write_reg_offset", write_reg_offset, "write_reg_offset [fhi | bf0 | bf1] <name> <offset> <value>"},
    {"dump_reg", dump_reg, "dump_reg [fhi | bf0 | bf1]"},
#else
    {"init", init, "init [<device_name>]"}, 
    {"finish", finish, "finish"},
    {"get", get, GET_USAGE_1},
    {"set", set, SET_USAGE_1},
    {"reset", reset, "reset <mode>"},
    {"clear", clear, "clear [fhi_alarms | fhi_stats]"},
    {"has", has, "has fhi"},
    {"configure", configure, "configure <cc>"},
    {"enable", enable, "enable <cc>"},
    {"disable", disable, "disable <cc>"},
    {"read_reg", read_reg, "read_reg [fhi] <name>"},
    {"read_reg_offset", read_reg_offset, "read_reg_offset [fhi] <name> <offset>"},
    {"write_reg", write_reg, "write_reg [fhi] <name> <value>"},
    {"write_reg_offset", write_reg_offset, "write_reg_offset [fhi] <name> <offset> <value>"},
    {"dump_reg", dump_reg, "dump_reg [fhi]"},
#endif
#ifdef LEGACY_ECPRI
    {"ecpri", ecpri, ECPRI_USAGE},
#endif
    {"peek", peek, "peek <address>"},
    {"poke", poke, "poke <address> <value>"},
    {"quit", quit, "quit"},
    // Last line must be all NULL's
    {NULL, NULL, NULL}
};

#ifndef NO_HW
/**
 * @brief Has FHI device been detected.
 * @returns
 *      - 0 if no
 *      - 1 if yes
 */
static int has_fhi(void)
{
    static int has_fhi_flag = -1;

    if (has_fhi_flag == -1)
    {
        has_fhi_flag = no_fhi ? 0 : xorif_has_front_haul_interface();
    }
    return has_fhi_flag;
}

#ifdef BF_INCLUDED
/**
 * @brief Has BF device been detected.
 * @returns
 *      - 0 if no
 *      - 1 if yes
 */
static int has_bf(void)
{
    static int has_bf_flag = -1;

    if (has_bf_flag == -1)
    {
        has_bf_flag = no_bf ? 0 : xobf_has_beamformer();
    }
    return has_bf_flag;
}
#endif // BF_INCLUDED
#endif // NO_HW

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
static int match(const char *s1, const char *s2)
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

/**
 * @brief Simple parser for unsigned integer numbers.
 * @param[in] tok_id The token id
 * @param[in] ptr Pointer to integer variable
 * @returns
 *      - 0 if no valid token was converted
 *      - 1 if successful
 */
static unsigned int parse_integer(int tok_id, unsigned int *ptr)
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

/**
 * @brief Simple parser for double precision floating point numbers.
 * @param[in] tok_id The token id
 * @param[in] ptr Pointer to double variable
 * @returns
 *      - 0 if no valid token was converted
 *      - 1 if successful
 */
static unsigned int parse_double(int tok_id, double *ptr)
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

/**
 * @brief Simple parser for strings.
 * @param[in] tok_id The token id
 * @param[in] ptr Pointer to string variable
 * @returns
 *      - 0 if no valid token was converted
 *      - 1 if successful
 */
static int parse_string(int tok_id, const char **ptr)
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
    static char buffer[MAX_BUFF_SIZE];

    // Check length limit
    if (strlen(request) >= MAX_BUFF_SIZE - 1)
    {
        return LINE_SIZE_LIMIT;
    }

    // Make a writeable copy of the request
    strncpy(buffer, request, MAX_BUFF_SIZE - 1);

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
        while (ptr->name)
        {
            if (match(token[0], ptr->name))
            {
                if (ptr->func)
                {
                    // Execute the command
                    return ptr->func(request, response);
                }
            }
            ++ptr;
        }

        return UNKNOWN_COMMAND;
    }

    return SUCCESS;
}

int do_command(const char *format, ...)
{
    static char buffer[MAX_BUFF_SIZE];
    static char response[MAX_BUFF_SIZE];

    // Variable argument handling using vsprintf to construct the request
    va_list argptr;
    va_start(argptr, format);
    vsprintf(buffer, format, argptr);
    va_end(argptr);

    // Clear response buffer
    response[0] = '\0';

    // Skip over any initial white space
    char *request = buffer;
    while (*request != '\0' && isspace(*request))
    {
        ++request;
    }

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
    }

    return result;
}

/**
 * @brief "help" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int help(const char *request, char *response)
{
    if (num_tokens == 1)
    {
        // help
        response += sprintf(response, "status = 0\n");
        const struct command *ptr = &command_set[0];
        while (ptr->name)
        {
            if (ptr->usage)
            {
                response += sprintf(response, "%s : %s\n", ptr->name, ptr->usage);
            }
            ++ptr;
        }
        return SUCCESS;
    }
    else if (num_tokens == 2)
    {
        // help <topic>
        const char *s;
        if (parse_string(1, &s))
        {
            const struct command *ptr = &command_set[0];
            while (ptr->name)
            {
                if (match(s, ptr->name))
                {
                    if (ptr->usage)
                    {
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "%s : %s\n", ptr->name, ptr->usage);
                        return SUCCESS;
                    }
                }
                ++ptr;
            }
        }
    }
    return MALFORMED_COMMAND;
}

/**
 * @brief "quit" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int quit(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 1)
        {
            // quit
            return TERMINATE;
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "init" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int init(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 1)
        {
            // init
            int result = xorif_init(NULL);
#ifdef BF_INCLUDED
            if (result == XORIF_SUCCESS)
            {
                if (has_bf())
                {
                    return xobf_init(NULL, NULL);
                }
            }
#endif
            return result;
        }
        else if (num_tokens == 2)
        {
            // init [fhi | bf]
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "fhi"))
                {
                    return xorif_init(NULL);
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf"))
                {
                    return xobf_init(NULL, NULL);
                }
#endif
            }
        }
        else if (num_tokens == 3)
        {
            // init [fhi | bf] <device_name>
            const char *s1;
            const char *s2;
            if (parse_string(1, &s1) && parse_string(2, &s2))
            {
                if (match(s2, "NULL"))
                {
                    s2 = NULL;
                }
                if (match(s1, "fhi"))
                {
                    return xorif_init(s2);
                }
#if BF_INCLUDED
                else if (match(s1, "bf"))
                {
                    return xobf_init(s2, NULL);
                }
#endif
            }
        }
#ifdef BF_INCLUDED
        else if (num_tokens == 4)
        {
            // init [bf] <device_name> <device name>
            const char *s1;
            const char *s2;
            const char *s3;
            if (parse_string(1, &s1) && parse_string(2, &s2) && parse_string(3, &s3))
            {
                if (match(s2, "NULL"))
                {
                    s2 = NULL;
                }
                if (match(s3, "NULL"))
                {
                    s3 = NULL;
                }
                if (match(s1, "bf"))
                {
                    return xobf_init(s2, s3);
                }
            }
        }
#endif
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "finish" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int finish(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 1)
        {
            // finish
            xorif_finish();
#ifdef BF_INCLUDED
            if (has_bf())
            {
                xobf_finish();
            }
#endif
            return SUCCESS;
        }
        else if (num_tokens == 2)
        {
            // finish [fhi | bf]
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "fhi"))
                {
                    xorif_finish();
                    return SUCCESS;
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf"))
                {
                    xobf_finish();
                    return SUCCESS;
                }
#endif
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "reset" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int reset(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 1)
        {
            // reset
            int result = xorif_reset_fhi(0);
#ifdef BF_INCLUDED
            if (result == XORIF_SUCCESS)
            {
                if (has_bf())
                {
                    return xobf_reset_bf(0);
                }
            }
#endif
            return result;
        }
        else if (num_tokens == 2)
        {
            // reset [fhi | bf]
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "fhi"))
                {
                    return xorif_reset_fhi(0);
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf"))
                {
                    return xobf_reset_bf(0);
                }
#endif
            }
        }
        else if (num_tokens == 3)
        {
            // reset [fhi | bf] <mode>
            const char *s;
            unsigned int mode;
            if (parse_string(1, &s) && parse_integer(2, &mode))
            {
                if (match(s, "fhi"))
                {
                    return xorif_reset_fhi(mode);
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf"))
                {
                    return xobf_reset_bf(mode);
                }
#endif
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "has" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int has(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 2)
        {
            // has [fhi | bf]
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "fhi"))
                {
                    int result = xorif_has_front_haul_interface();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = %s\n", result ? "true" : "false");
                    return SUCCESS;
                }
                else if (match(s, "bf"))
                {
                    int result;
#ifdef BF_INCLUDED
                    result = no_bf ? 0 : xobf_has_beamformer();
#else
                    result = 0;
#endif
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = %s\n", result ? "true" : "false");
                    return SUCCESS;
                }
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "get" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int get(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens >= 2)
        {
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "sw_version") && num_tokens == 2)
                {
                    // get sw_version
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "fhi_version = 0x%08X\n", xorif_get_sw_version());
#ifdef BF_INCLUDED
                    response += sprintf(response, "bf_version = 0x%08X\n", xobf_get_sw_version());
#endif
                    return SUCCESS;
                }
                else if (match(s, "fhi_sw_version") && num_tokens == 2)
                {
                    // get fhi_sw_version
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", xorif_get_sw_version());
                    return SUCCESS;
                }
                else if (match(s, "fhi_hw_version") && num_tokens == 2)
                {
                    // get fhi_hw_version
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", xorif_get_fhi_hw_version());
                    return SUCCESS;
                }
                else if (match(s, "fhi_hw_internal_rev") && num_tokens == 2)
                {
                    // get fhi_hw_internal_rev
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", xorif_get_fhi_hw_internal_rev());
                    return SUCCESS;
                }
                else if ((match(s, "fhi_capabilities") || match(s, "fhi_caps")) && num_tokens == 2)
                {
                    // get [fhi_capabilities | fhi_caps]
                    const struct xorif_caps *ptr = xorif_get_capabilities();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "max_cc = %d\n", ptr->max_cc);
                    response += sprintf(response, "num_eth_ports = %d\n", ptr->num_eth_ports);
                    response += sprintf(response, "numerologies = 0x%X\n", ptr->numerologies);
                    response += sprintf(response, "extended_cp = %s\n", ptr->extended_cp ? "true" : "false");
                    response += sprintf(response, "iq_de_comp_methods = 0x%X\n", ptr->iq_de_comp_methods);
                    response += sprintf(response, "iq_de_comp_bfp_widths = 0x%X\n", ptr->iq_de_comp_bfp_widths);
                    response += sprintf(response, "iq_de_comp_mod_widths = 0x%X\n", ptr->iq_de_comp_mod_widths);
                    response += sprintf(response, "iq_comp_methods = 0x%X\n", ptr->iq_comp_methods);
                    response += sprintf(response, "iq_comp_bfp_widths = 0x%X\n", ptr->iq_comp_bfp_widths);
                    response += sprintf(response, "no_framer_ss = %d\n", ptr->no_framer_ss);
                    response += sprintf(response, "no_deframer_ss = %d\n", ptr->no_deframer_ss);
                    response += sprintf(response, "max_framer_ethernet_pkt = %d\n", ptr->max_framer_ethernet_pkt);
                    response += sprintf(response, "max_deframer_ethernet_pkt = %d\n", ptr->max_deframer_ethernet_pkt);
                    response += sprintf(response, "max_subcarriers = %d\n", ptr->max_subcarriers);
                    response += sprintf(response, "max_data_symbols = %d\n", ptr->max_data_symbols);
                    response += sprintf(response, "max_ctrl_symbols = %d\n", ptr->max_ctrl_symbols);
                    response += sprintf(response, "max_ul_ctrl_1kwords = %d\n", ptr->max_ul_ctrl_1kwords);
                    response += sprintf(response, "max_dl_ctrl_1kwords = %d\n", ptr->max_dl_ctrl_1kwords);
                    response += sprintf(response, "max_dl_data_1kwords = %d\n", ptr->max_dl_data_1kwords);
                    response += sprintf(response, "max_ssb_ctrl_512words = %d\n", ptr->max_ssb_ctrl_512words);
                    response += sprintf(response, "max_ssb_data_512words = %d\n", ptr->max_ssb_data_512words);
                    response += sprintf(response, "timer_clk_ps = %d\n", ptr->timer_clk_ps);
                    response += sprintf(response, "num_unsolicited_ports = %d\n", ptr->num_unsolicited_ports);
                    response += sprintf(response, "du_id_limit = %d\n", ptr->du_id_limit);
                    response += sprintf(response, "bs_id_limit = %d\n", ptr->bs_id_limit);
                    response += sprintf(response, "cc_id_limit = %d\n", ptr->cc_id_limit);
                    response += sprintf(response, "ru_id_limit = %d\n", ptr->ru_id_limit);
                    response += sprintf(response, "ss_id_limit = %d\n", ptr->ss_id_limit);
                    response += sprintf(response, "ru_ports_map_width = %d\n", ptr->ru_ports_map_width);
                    return SUCCESS;
                }
                else if (match(s, "fhi_alarms") && num_tokens == 2)
                {
                    // get fhi_alarms
                    uint32_t result = xorif_get_fhi_alarms();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%X\n", result);
                    return SUCCESS;
                }
                else if (match(s, "fhi_stats") && num_tokens == 3)
                {
                    // get fhi_stats <port>
                    unsigned int port;
                    if (parse_integer(2, &port))
                    {
                        struct xorif_fhi_eth_stats stats;
                        int result = xorif_get_fhi_eth_stats(port, &stats);
                        response += sprintf(response, "status = %d\n", result);
                        if (result == XORIF_SUCCESS)
                        {
                            response += sprintf(response, "total_rx_good_pkt_cnt = %lu\n", stats.total_rx_good_pkt_cnt);
                            response += sprintf(response, "total_rx_bad_pkt_cnt = %lu\n", stats.total_rx_bad_pkt_cnt);
                            response += sprintf(response, "total_rx_bad_fcs_cnt = %lu\n", stats.total_rx_bad_fcs_cnt);
                            response += sprintf(response, "total_rx_bit_rate = %lu\n", stats.total_rx_bit_rate);
                            response += sprintf(response, "oran_rx_bit_rate = %lu\n", stats.oran_rx_bit_rate);
                            response += sprintf(response, "oran_rx_total = %lu\n", stats.oran_rx_total);
                            response += sprintf(response, "oran_rx_on_time = %lu\n", stats.oran_rx_on_time);
                            response += sprintf(response, "oran_rx_early = %lu\n", stats.oran_rx_early);
                            response += sprintf(response, "oran_rx_late = %lu\n", stats.oran_rx_late);
                            response += sprintf(response, "oran_rx_total_c = %lu\n", stats.oran_rx_total_c);
                            response += sprintf(response, "oran_rx_on_time_c = %lu\n", stats.oran_rx_on_time_c);
                            response += sprintf(response, "oran_rx_early_c = %lu\n", stats.oran_rx_early_c);
                            response += sprintf(response, "oran_rx_late_c = %lu\n", stats.oran_rx_late_c);
                            response += sprintf(response, "oran_rx_corrupt = %lu\n", stats.oran_rx_corrupt);
                            response += sprintf(response, "oran_rx_error_drop = %lu\n", stats.oran_rx_error_drop);
                            response += sprintf(response, "oran_tx_total = %lu\n", stats.oran_tx_total);
                            response += sprintf(response, "oran_tx_total_c = %lu\n", stats.oran_tx_total_c);
                            response += sprintf(response, "offset_earliest_u_pkt = %d\n", stats.offset_earliest_u_pkt);
                            response += sprintf(response, "offset_earliest_c_pkt = %d\n", stats.offset_earliest_c_pkt);
                            return SUCCESS;
                        }
                    }
                }
                else if (match(s, "fhi_cc_config") && num_tokens == 3)
                {
                    // get fhi_cc_config <cc>
                    unsigned int cc;
                    if (parse_integer(2, &cc))
                    {
                        struct xorif_cc_config config;
                        int result = xorif_get_cc_config(cc, &config);
                        if (result == XORIF_SUCCESS)
                        {
                            response += sprintf(response, "status = 0\n");
                            response += sprintf(response, "num_rbs = %d\n", config.num_rbs);
                            response += sprintf(response, "numerology = %d\n", config.numerology);
                            response += sprintf(response, "extended_cp = %s\n", config.extended_cp ? "true" : "false");
                            response += sprintf(response, "num_rbs_ssb = %d\n", config.num_rbs_ssb);
                            response += sprintf(response, "numerology_ssb = %d\n", config.numerology_ssb);
                            response += sprintf(response, "extended_cp_ssb = %s\n", config.extended_cp_ssb ? "true" : "false");
                            response += sprintf(response, "iq_comp_meth_ul = %d\n", config.iq_comp_meth_ul);
                            response += sprintf(response, "iq_comp_width_ul = %d\n", config.iq_comp_width_ul);
                            response += sprintf(response, "iq_comp_mplane_ul = %d\n", config.iq_comp_mplane_ul);
                            response += sprintf(response, "iq_comp_meth_dl = %d\n", config.iq_comp_meth_dl);
                            response += sprintf(response, "iq_comp_width_dl = %d\n", config.iq_comp_width_dl);
                            response += sprintf(response, "iq_comp_mplane_dl = %d\n", config.iq_comp_mplane_dl);
                            response += sprintf(response, "iq_comp_meth_ssb = %d\n", config.iq_comp_meth_ssb);
                            response += sprintf(response, "iq_comp_width_ssb = %d\n", config.iq_comp_width_ssb);
                            response += sprintf(response, "iq_comp_mplane_ssb = %d\n", config.iq_comp_mplane_ssb);
                            response += sprintf(response, "delay_comp_cp_ul = %g\n", config.delay_comp_cp_ul);
                            response += sprintf(response, "delay_comp_cp_dl = %g\n", config.delay_comp_cp_dl);
                            response += sprintf(response, "delay_comp_up = %g\n", config.delay_comp_up);
                            response += sprintf(response, "advance_ul = %g\n", config.advance_ul);
                            response += sprintf(response, "advance_dl = %g\n", config.advance_dl);
                            response += sprintf(response, "ul_bid_forward = %g\n", config.ul_bid_forward);
                            response += sprintf(response, "ul_radio_ch_dly = %g\n", config.ul_radio_ch_dly);
                            response += sprintf(response, "num_ctrl_per_sym_ul = %d\n", config.num_ctrl_per_sym_ul);
                            response += sprintf(response, "num_ctrl_per_sym_dl = %d\n", config.num_ctrl_per_sym_dl);
                            response += sprintf(response, "num_ctrl_per_sym_ssb = %d\n", config.num_ctrl_per_sym_ssb);
                            response += sprintf(response, "num_sect_per_sym = %d\n", config.num_sect_per_sym);
                            response += sprintf(response, "num_sect_per_sym_ssb = %d\n", config.num_sect_per_sym_ssb);
                            response += sprintf(response, "num_frames_per_sym = %d\n", config.num_frames_per_sym);
                            response += sprintf(response, "num_frames_per_sym_ssb = %d\n", config.num_frames_per_sym_ssb);
                            return SUCCESS;
                        }
                    }
                }
                else if (match(s, "fhi_cc_alloc") && num_tokens == 3)
                {
                    // get fhi_cc_alloc <cc>
                    unsigned int cc;
                    if (parse_integer(2, &cc))
                    {
                        struct xorif_cc_alloc alloc;
                        int result = xorif_get_fhi_cc_alloc(cc, &alloc);
                        if (result == XORIF_SUCCESS)
                        {
                            response += sprintf(response, "status = 0\n");
                            response += sprintf(response, "ul_ctrl_sym_num = %d\n", alloc.ul_ctrl_sym_num);
                            response += sprintf(response, "dl_ctrl_sym_num = %d\n", alloc.dl_ctrl_sym_num);
                            response += sprintf(response, "dl_data_sym_num = %d\n", alloc.dl_data_sym_num);
                            response += sprintf(response, "ssb_ctrl_sym_num = %d\n", alloc.ssb_ctrl_sym_num);
                            response += sprintf(response, "ssb_data_sym_num = %d\n", alloc.ssb_data_sym_num);
                            response += sprintf(response, "ul_ctrl_offset = %d\n", alloc.ul_ctrl_offset);
                            response += sprintf(response, "ul_ctrl_size = %d\n", alloc.ul_ctrl_size);
                            response += sprintf(response, "ul_ctrl_base_offset = %d\n", alloc.ul_ctrl_base_offset);
                            response += sprintf(response, "ul_ctrl_base_size = %d\n", alloc.ul_ctrl_base_size);
                            response += sprintf(response, "dl_ctrl_offset = %d\n", alloc.dl_ctrl_offset);
                            response += sprintf(response, "dl_ctrl_size = %d\n", alloc.dl_ctrl_size);
                            response += sprintf(response, "dl_data_ptrs_offset = %d\n", alloc.dl_data_ptrs_offset);
                            response += sprintf(response, "dl_data_ptrs_size = %d\n", alloc.dl_data_ptrs_size);
                            response += sprintf(response, "dl_data_buff_offset = %d\n", alloc.dl_data_buff_offset);
                            response += sprintf(response, "dl_data_buff_size = %d\n", alloc.dl_data_buff_size);
                            response += sprintf(response, "ssb_ctrl_offset = %d\n", alloc.ssb_ctrl_offset);
                            response += sprintf(response, "ssb_ctrl_size = %d\n", alloc.ssb_ctrl_size);
                            response += sprintf(response, "ssb_data_ptrs_offset = %d\n", alloc.ssb_data_ptrs_offset);
                            response += sprintf(response, "ssb_data_ptrs_size = %d\n", alloc.ssb_data_ptrs_size);
                            response += sprintf(response, "ssb_data_buff_offset = %d\n", alloc.ssb_data_buff_offset);
                            response += sprintf(response, "ssb_data_buff_size = %d\n", alloc.ssb_data_buff_size);
                            return SUCCESS;
                        }
                    }
                }
                else if (match(s, "fhi_state") && num_tokens == 2)
                {
                    // get fhi_state
                    uint32_t result = xorif_get_state();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", result);
                    return SUCCESS;
                }
                else if (match(s, "fhi_enabled") && num_tokens == 2)
                {
                    // get fhi_enabled
                    uint8_t result = xorif_get_enabled_cc_mask();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%02X\n", result);
                    return SUCCESS;
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf_sw_version") && num_tokens == 2)
                {
                    // get bf_sw_version
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "version = 0x%08X\n", xobf_get_sw_version());
                    return SUCCESS;
                }
                else if (match(s, "bf_hw_version") && num_tokens == 2)
                {
                    // get bf_hw_version
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", xobf_get_bf_hw_version());
                    return SUCCESS;
                }
                else if (match(s, "bf_hw_internal_rev") && num_tokens == 2)
                {
                    // get bf_hw_internal_rev
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", xobf_get_bf_hw_internal_rev());
                    return SUCCESS;
                }
                else if ((match(s, "bf_capabilities") || match(s, "bf_caps")) && num_tokens == 2)
                {
                    // get [bf_capabilities | bf_caps]
                    const struct xobf_caps *ptr = xobf_get_capabilities();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "max_cc = %d\n", ptr->max_cc);
                    response += sprintf(response, "num_eth_ports = %d\n", ptr->num_eth_ports);
                    response += sprintf(response, "numerologies = 0x%X\n", ptr->numerologies);
                    response += sprintf(response, "extended_cp = %s\n", ptr->extended_cp ? "true" : "false");
                    response += sprintf(response, "iq_comp_methods = 0x%X\n", ptr->iq_comp_methods);
                    response += sprintf(response, "iq_comp_bfp_widths = 0x%X\n", ptr->iq_comp_bfp_widths);
                    response += sprintf(response, "bw_comp_methods = 0x%X\n", ptr->bw_comp_methods);
                    response += sprintf(response, "bw_comp_bfp_widths = 0x%X\n", ptr->bw_comp_bfp_widths);
                    response += sprintf(response, "uram_cache = %d\n", ptr->uram_cache);
                    response += sprintf(response, "num_antennas = %d\n", ptr->num_antennas);
                    response += sprintf(response, "num_dfes = %d\n", ptr->num_dfes);
                    response += sprintf(response, "num_ss_dl = %d\n", ptr->num_ss_dl);
                    response += sprintf(response, "num_ss_ul = %d\n", ptr->num_ss_ul);
                    response += sprintf(response, "external_bw_store = %s\n", ptr->external_bw_store ? "true" : "false");
                    response += sprintf(response, "cache_clk = %s\n", ptr->cache_clk ? "true" : "false");
                    response += sprintf(response, "aie_if_clk = %s\n", ptr->aie_if_clk ? "true" : "false");
                    response += sprintf(response, "max_bw_ids = %d\n", ptr->max_bw_ids);
                    response += sprintf(response, "num_bfs = %d\n", ptr->num_bfs);
                    return SUCCESS;
                }
                else if (match(s, "bf_cc_config") && num_tokens == 3)
                {
                    // get bf_cc_config <cc>
                    unsigned int cc;
                    if (parse_integer(2, &cc))
                    {
                        struct xobf_cc_config config;
                        int result = xobf_get_cc_config(cc, &config);
                        if (result == XORIF_SUCCESS)
                        {
                            response += sprintf(response, "status = 0\n");
                            response += sprintf(response, "num_rbs = %d\n", config.num_rbs);
                            response += sprintf(response, "numerology = %d\n", config.numerology);
                            response += sprintf(response, "extended_cp = %s\n", config.extended_cp ? "true" : "false");
#ifdef MIXED_SSB_SUPPORT
                            response += sprintf(response, "num_rbs_ssb = %d\n", config.num_rbs_ssb);
                            response += sprintf(response, "numerology_ssb = %d\n", config.numerology_ssb);
                            response += sprintf(response, "extended_cp_ssb = %s\n", config.extended_cp_ssb ? "true" : "false");
#endif
                            response += sprintf(response, "iq_comp_meth_prach = %d\n", config.iq_comp_meth_prach);
                            response += sprintf(response, "iq_comp_width_prach = %d\n", config.iq_comp_width_prach);
                            response += sprintf(response, "iq_comp_mplane_prach = %d\n", config.iq_comp_mplane_prach);
                            response += sprintf(response, "bw_comp_meth = %d\n", config.bw_comp_meth);
                            response += sprintf(response, "bw_comp_width = %d\n", config.bw_comp_width);
                            return SUCCESS;
                        }
                    }
                }
                else if (match(s, "bf_cc_alloc") && num_tokens == 3)
                {
                    // get bf_cc_alloc <cc>
                    unsigned int cc;
                    if (parse_integer(2, &cc))
                    {
                        struct xobf_cc_alloc config;
                        int result = xobf_get_bf_cc_alloc(cc, &config);
                        if (result == XORIF_SUCCESS)
                        {
                            response += sprintf(response, "status = 0\n");
                            response += sprintf(response, "ud_symbol_offset_0 = %d\n", config.ud_symbol_offset_0);
                            response += sprintf(response, "ud_symbol_size_0 = %d\n", config.ud_symbol_size_0);
                            response += sprintf(response, "ud_symbol_offset_1 = %d\n", config.ud_symbol_offset_1);
                            response += sprintf(response, "ud_symbol_size_1 = %d\n", config.ud_symbol_size_1);
#ifdef MIXED_SSB_SUPPORT
                            response += sprintf(response, "ssb_symbol_offset = %d\n", config.ssb_symbol_offset);
                            response += sprintf(response, "ssb_symbol_size = %d\n", config.ssb_symbol_size);
#endif
                            return SUCCESS;
                        }
                    }
                }
                else if (match(s, "bf_alarms") && num_tokens == 2)
                {
                    // get bf_alarms
                    uint32_t result = xobf_get_bf_alarms();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%X\n", result);
                    return SUCCESS;
                }
                else if (match(s, "bf_stats") && num_tokens == 2)
                {
                    // get bf_stats
                    struct xobf_bf_stats stats;
                    int result = xobf_get_bf_stats(&stats);
                    if (result == XORIF_SUCCESS)
                    {
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "cache_dl_miss_count = %d\n", stats.cache_dl_miss_count);
                        response += sprintf(response, "cplane_dl_no_desc_count = %d\n", stats.cplane_dl_no_desc_count);
                        response += sprintf(response, "bf_dl_re_mutex_count = %d\n", stats.bf_dl_re_mutex_count);
                        response += sprintf(response, "aie_dl_count = %d\n", stats.aie_dl_count);
                        response += sprintf(response, "bwm_dl_next_bidn_uf_count = %d\n", stats.bwm_dl_next_bidn_uf_count);
                        response += sprintf(response, "cache_ul_miss_count = %d\n", stats.cache_ul_miss_count);
                        response += sprintf(response, "cplane_ul_no_desc_count = %d\n", stats.cplane_ul_no_desc_count);
                        response += sprintf(response, "bf_ul_re_mutex_count = %d\n", stats.bf_ul_re_mutex_count);
                        response += sprintf(response, "aie_ul_count = %d\n", stats.aie_ul_count);
                        response += sprintf(response, "dfe_ul_config_count = %d\n", stats.dfe_ul_config_count);
                        response += sprintf(response, "bwm_ul_next_bidn_uf_count = %d\n", stats.bwm_ul_next_bidn_uf_count);
                        response += sprintf(response, "prach_count = %d\n", stats.prach_count);
#ifdef MIXED_SSB_SUPPORT
                        response += sprintf(response, "ssb_count = %d\n", stats.ssb_count);
#endif
                        return SUCCESS;
                    }
                }
                else if (match(s, "bf_state") && num_tokens == 2)
                {
                    // get bf_state
                    uint32_t result = xobf_get_state();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", result);
                    return SUCCESS;
                }
                else if (match(s, "bf_enabled") && num_tokens == 2)
                {
                    // get bf_enabled
                    uint8_t result = xobf_get_enabled_cc_mask();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%02X\n", result);
                    return SUCCESS;
                }
#endif // BF_INCLUDED
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "set" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int set(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens >= 2)
        {
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "num_rbs") && num_tokens == 4)
                {
                    // set num_rbs <cc> <number of rbs>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        int result = xorif_set_cc_num_rbs(cc, val);
#ifdef BF_INCLUDED
                        if (result == XORIF_SUCCESS)
                        {
                            if (has_bf())
                            {
                                return xobf_set_cc_num_rbs(cc, val);
                            }
                        }
#endif
                        return result;
                    }
                }
                else if (match(s, "numerology") && num_tokens == 5)
                {
                    // set numerology <cc> <numerology> <extended CP = 0 | 1>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        int result = xorif_set_cc_numerology(cc, val1, val2);
#ifdef BF_INCLUDED
                        if (result == XORIF_SUCCESS)
                        {
                            if (has_bf())
                            {
                                return xobf_set_cc_numerology(cc, val1, val2);
                            }
                        }
#endif
                        return result;
                    }
                }
                else if (match(s, "numerology_ssb") && num_tokens == 5)
                {
                    // set numerology_ssb <cc> <numerology> <extended CP = 0 | 1>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        int result = xorif_set_cc_numerology_ssb(cc, val1, val2);
#ifdef BF_INCLUDED
#ifdef MIXED_SSB_SUPPORT
                        if (result == XORIF_SUCCESS)
                        {
                            if (has_bf())
                            {
                                return xobf_set_cc_numerology_ssb(cc, val1, val2);
                            }
                        }
#endif
#endif
                        return result;
                    }
                }
                else if (match(s, "time_advance") && num_tokens == 6)
                {
                    // set time_advance <cc> <deskew> <advance up-link> <advance down-link>
                    unsigned int cc;
                    double val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_double(3, &val1) && parse_double(4, &val2) && parse_double(5, &val3))
                    {
                        return xorif_set_cc_time_advance(cc, val1, val2, val3);
                    }
                }
                else if (match(s, "ul_timing_params") && num_tokens == 6)
                {
                    // set ul_timing_params <cc> <delay_comp> <advance> <radio_ch_delay>
                    unsigned int cc;
                    double val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_double(3, &val1) && parse_double(4, &val2) && parse_double(5, &val3))
                    {
                        return xorif_set_cc_ul_timing_parameters(cc, val1, val2, val3);
                    }
                }
                else if (match(s, "dl_timing_params") && num_tokens == 6)
                {
                    // set dl_timing_params <cc> <delay_comp_cp> <delay_comp_up> <advance>
                    unsigned int cc;
                    double val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_double(3, &val1) && parse_double(4, &val2) && parse_double(5, &val3))
                    {
                        return xorif_set_cc_dl_timing_parameters(cc, val1, val2, val3);
                    }
                }
                else if (match(s, "ul_bid_forward") && num_tokens == 4)
                {
                    // set ul_bid_forward <cc> <time>
                    unsigned int cc;
                    double val;
                    if (parse_integer(2, &cc) && parse_double(3, &val))
                    {
                        return xorif_set_ul_bid_forward(cc, val);
                    }
                }
                else if (match(s, "ul_bid_forward_fine") && num_tokens == 5)
                {
                    // set ul_bid_forward_fine <cc> <symbols> <cycles>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        // No API call for this, instead use register writes
                        xorif_write_fhi_reg_offset("ORAN_CC_UL_BIDF_C_ABS_SYMBOL", 0x70 * cc, val1);
                        xorif_write_fhi_reg_offset("ORAN_CC_UL_BIDF_C_CYCLES", 0x70 * cc, val2);
                        return XORIF_SUCCESS;
                    }
                }
                else if (match(s, "ul_radio_ch_dly") && num_tokens == 4)
                {
                    // set ul_radio_ch_dly <cc> <delay>
                    unsigned int cc;
                    double val;
                    if (parse_integer(2, &cc) && parse_double(3, &val))
                    {
                        return xorif_set_ul_radio_ch_dly(cc, val);
                    }
                }
                else if ((match(s, "dl_iq_compression") || match(s, "dl_iq_comp")) && num_tokens == 5)
                {
                    // set [dl_iq_compression | dl_iq_comp] <cc> <width> <method>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_dl_iq_compression(cc, val1, val2, 1);
                    }
                }
                else if ((match(s, "dl_iq_compression") || match(s, "dl_iq_comp")) && num_tokens == 6)
                {
                    // set [dl_iq_compression | dl_iq_comp] <cc> <width> <method> <mplane = 0 | 1>
                    unsigned int cc, val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2) && parse_integer(5, &val3))
                    {
                        return xorif_set_cc_dl_iq_compression(cc, val1, val2, val3);
                    }
                }
                else if ((match(s, "ul_iq_compression") || match(s, "ul_iq_comp")) && num_tokens == 5)
                {
                    // set [ul_iq_compression | ul_iq_comp] <cc> <width> <method>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_ul_iq_compression(cc, val1, val2, 1);
                    }
                }
                else if ((match(s, "ul_iq_compression") || match(s, "ul_iq_comp")) && num_tokens == 6)
                {
                    // set [ul_iq_compression | ul_iq_comp] <cc> <width> <method> <mplane = 0 | 1>
                    unsigned int cc, val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2) && parse_integer(5, &val3))
                    {
                        return xorif_set_cc_ul_iq_compression(cc, val1, val2, val3);
                    }
                }
                else if ((match(s, "ssb_iq_compression") || match(s, "ssb_iq_comp")) && num_tokens == 5)
                {
                    // set [ssb_iq_compression | ssb_iq_comp] <cc> <width> <method>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_iq_compression_ssb(cc, val1, val2, 1);
                    }
                }
                else if ((match(s, "ssb_iq_compression") || match(s, "ssb_iq_comp")) && num_tokens == 6)
                {
                    // set [ssb_iq_compression | ssb_iq_comp] <cc> <width> <method> <mplane = 0 | 1>
                    unsigned int cc, val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2) && parse_integer(5, &val3))
                    {
                        return xorif_set_cc_iq_compression_ssb(cc, val1, val2, val3);
                    }
                }
                else if ((match(s, "prach_iq_compression") || match(s, "prach_iq_comp")) && num_tokens == 5)
                {
                    // set [prach_iq_compression | prach_iq_comp] <cc> <width> <method>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
#ifdef BF_INCLUDED
                        return xobf_set_cc_iq_compression_prach(cc, val1, val2, 1);
#endif
                    }
                }
                else if ((match(s, "prach_iq_compression") || match(s, "prach_iq_comp")) && num_tokens == 6)
                {
                    // set [prach_iq_compression | prach_iq_comp] <cc> <width> <method> <mplane = 0 | 1>
                    unsigned int cc, val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2) && parse_integer(5, &val3))
                    {
#ifdef BF_INCLUDED
                        return xobf_set_cc_iq_compression_prach(cc, val1, val2, val3);
#endif
                    }
                }
#ifdef BF_INCLUDED
                else if ((match(s, "bw_compression") || match(s, "bw_comp")) && num_tokens == 5)
                {
                    // set [bw_compression | bw_comp] <cc> <width> <method>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xobf_set_cc_bw_compression(cc, val1, val2);
                    }
                }
#endif
                else if (match(s, "dl_sections_per_sym") && num_tokens == 4)
                {
                    // set dl_sections_per_sym <cc> <num sections>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_dl_sections_per_symbol(cc, val, val);
                    }
                }
                else if (match(s, "dl_sections_per_sym") && num_tokens == 5)
                {
                    // set dl_sections_per_sym <cc> <num sections> <num ctrl>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_dl_sections_per_symbol(cc, val1, val2);
                    }
                }
                else if (match(s, "ul_sections_per_sym") && num_tokens == 4)
                {
                    // set ul_sections_per_sym <cc> <num sections>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_ul_sections_per_symbol(cc, val, val);
                    }
                }
                else if (match(s, "ul_sections_per_sym") && num_tokens == 5)
                {
                    // set ul_sections_per_sym <cc> <num sections> <num ctrl>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_ul_sections_per_symbol(cc, val1, val2);
                    }
                }
                else if (match(s, "ssb_sections_per_sym") && num_tokens == 4)
                {
                    // set ssb_sections_per_sym <cc> <num sections>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_sections_per_symbol_ssb(cc, val, val);
                    }
                }
                else if (match(s, "ssb_sections_per_sym") && num_tokens == 5)
                {
                    // set ssb_sections_per_sym <cc> <num sections> <num ctrl>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_sections_per_symbol_ssb(cc, val1, val2);
                    }
                }
                else if (match(s, "frames_per_sym") && num_tokens == 4)
                {
                    // set frames_per_sym <cc> <num frames>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_frames_per_symbol(cc, val);
                    }
                }
                else if (match(s, "frames_per_sym_ssb") && num_tokens == 4)
                {
                    // set frames_per_sym_ssb <cc> <num frames>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_frames_per_symbol_ssb(cc, val);
                    }
                }
                else if (match(s, "dest_mac_addr") && num_tokens == 4)
                {
                    // set dest_mac_addr <port> <address>
                    unsigned int eth;
                    const char *s;
                    if (parse_integer(2, &eth) && parse_string(3, &s))
                    {
                        unsigned int val[6];
                        if (sscanf(s, "%x:%x:%x:%x:%x:%x", &val[0], &val[1], &val[2], &val[3], &val[4], &val[5]) == 6)
                        {
                            // Pack the int's into byte array
                            uint8_t address[6];
                            address[0] = val[0];
                            address[1] = val[1];
                            address[2] = val[2];
                            address[3] = val[3];
                            address[4] = val[4];
                            address[5] = val[5];
                            return xorif_set_fhi_dest_mac_addr(eth, address);
                        }
                    }
                }
                else if (match(s, "src_mac_addr") && num_tokens == 4)
                {
                    // set src_mac_addr <port> <address>
                    unsigned int eth;
                    const char *s;
                    if (parse_integer(2, &eth) && parse_string(3, &s))
                    {
                        unsigned int val[6];
                        if (sscanf(s, "%x:%x:%x:%x:%x:%x", &val[0], &val[1], &val[2], &val[3], &val[4], &val[5]) == 6)
                        {
                            // Pack the int's into byte array
                            uint8_t address[6];
                            address[0] = val[0];
                            address[1] = val[1];
                            address[2] = val[2];
                            address[3] = val[3];
                            address[4] = val[4];
                            address[5] = val[5];
                            return xorif_set_fhi_src_mac_addr(eth, address);
                        }
                    }
                }
                else if (match(s, "protocol") && num_tokens == 5)
                {
                    // set protocol <ECPRI | 1914.3> <VLAN = 0 | 1> <RAW | IPv4 | IPv6>
                    unsigned int vlan;
                    const char *s1;
                    const char *s2;
                    if (parse_string(2, &s1) && parse_integer(3, &vlan) && parse_string(4, &s2))
                    {
                        int transport = -1;
                        if (match(s1, "ECPRI"))
                        {
                            transport = PROTOCOL_ECPRI;
                        }
                        else if (match(s1, "1914.3"))
                        {
                            transport = PROTOCOL_IEEE_1914_3;
                        }

                        int ip_mode = -1;
                        if (match(s2, "RAW"))
                        {
                            ip_mode = IP_MODE_RAW;
                        }
                        else if (match(s2, "IPv4"))
                        {
                            ip_mode = IP_MODE_IPV4;
                        }
                        else if (match(s2, "IPv6"))
                        {
                            ip_mode = IP_MODE_IPV6;
                        }

                        if ((transport >= 0) && (vlan == 0 || vlan == 1) && (ip_mode >= 0))
                        {
                            return xorif_set_fhi_protocol(transport, vlan, ip_mode);
                        }
                    }
                }
                else if (match(s, "vlan") && num_tokens == 6)
                {
                    // set vlan <port> <id> <dei> <pcp>
                    unsigned int port;
                    unsigned int id;
                    unsigned int dei;
                    unsigned int pcp;
                    if (parse_integer(2, &port) && parse_integer(3, &id) &&
                        parse_integer(4, &dei) && parse_integer(5, &pcp))
                    {
                        return xorif_set_fhi_vlan_tag(port, id, dei, pcp);
                    }
                }
                else if (match(s, "eAxC_id") && num_tokens == 6)
                {
                    // set eAxC_id <DU bits> <BS bits> <CC bits> <RU bits>
                    unsigned int du_bits;
                    unsigned int bs_bits;
                    unsigned int cc_bits;
                    unsigned int ru_bits;
                    if (parse_integer(2, &du_bits) && parse_integer(3, &bs_bits) &&
                        parse_integer(4, &cc_bits) && parse_integer(5, &ru_bits))
                    {
                        return xorif_set_fhi_eaxc_id(du_bits, bs_bits, cc_bits, ru_bits);
                    }
                }
                else if (match(s, "ru_ports") && num_tokens == 8)
                {
                    // set ru_ports <RU bits> <ss bits> <mask> <user value> <PRACH value> <SSB value>
                    unsigned int ru_bits;
                    unsigned int ss_bits;
                    unsigned int mask;
                    unsigned int user_val;
                    unsigned int prach_val;
                    unsigned int ssb_val;
                    if (parse_integer(2, &ru_bits) && parse_integer(3, &ss_bits) &&
                        parse_integer(4, &mask) && parse_integer(5, &user_val) &&
                        parse_integer(6, &prach_val) && parse_integer(7, &ssb_val))
                    {
                        return xorif_set_ru_ports(ru_bits, ss_bits, mask, user_val, prach_val, ssb_val);
                    }
                }
                else if (match(s, "ru_ports") && num_tokens == 9)
                {
                    // set ru_ports <RU bits> <ss bits> <mask> <user value> <PRACH value> <SSB value> <LTE value>
                    unsigned int ru_bits;
                    unsigned int ss_bits;
                    unsigned int mask;
                    unsigned int user_val;
                    unsigned int prach_val;
                    unsigned int ssb_val;
                    unsigned int lte_val;
                    if (parse_integer(2, &ru_bits) && parse_integer(3, &ss_bits) &&
                        parse_integer(4, &mask) && parse_integer(5, &user_val) &&
                        parse_integer(6, &prach_val) && parse_integer(7, &ssb_val) &&
                        parse_integer(8, &lte_val))
                    {
                        return xorif_set_ru_ports_alt1(ru_bits, ss_bits, mask, user_val, prach_val, ssb_val, lte_val);
                    }
                }
                else if (match(s, "ru_ports_table_mode") && num_tokens == 3)
                {
                    // set ru_ports_table_mode <mode>
                    unsigned int mode;
                    if (parse_integer(2, &mode))
                    {
                        return xorif_set_ru_ports_table_mode(mode);
                    }
                }
                else if (match(s, "ru_ports_table") && num_tokens == 5)
                {
                    // set ru_ports_table <address> <port> <type>
                    unsigned int address;
                    unsigned int port;
                    unsigned int type;
                    if (parse_integer(2, &address) && parse_integer(3, &port) &&
                        parse_integer(4, &type))
                    {
                        return xorif_set_ru_ports_table(address, port, type, 1);
                    }
                }
                else if (match(s, "ru_ports_table") && num_tokens == 6)
                {
                    // set ru_ports_table <address> <port> <type> [<number>]
                    unsigned int address;
                    unsigned int port;
                    unsigned int type;
                    unsigned int number;
                    if (parse_integer(2, &address) && parse_integer(3, &port) &&
                        parse_integer(4, &type) && parse_integer(5, &number))
                    {
                        return xorif_set_ru_ports_table(address, port, type, number);
                    }
                }
                else if (match(s, "system_constants") && num_tokens >= 3)
                {
                    // set system_constants <fh_decap_dly> [...]
                    double val1;
                    if (parse_double(2, &val1))
                    {
                        struct xorif_system_constants sys_const;
                        sys_const.FH_DECAP_DLY = val1;
                        // TBD Add more as required
                        return xorif_set_system_constants(&sys_const);
                    }
                }
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "clear" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int clear(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 2)
        {
            // clear [fhi_alarms | fhi_stats | bf_alarms | bf_stats | ru_ports_table]
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "fhi_alarms"))
                {
                    xorif_clear_fhi_alarms();
                    response += sprintf(response, "status = 0\n");
                    return SUCCESS;
                }
                else if (match(s, "fhi_stats"))
                {
                    xorif_clear_fhi_stats();
                    response += sprintf(response, "status = 0\n");
                    return SUCCESS;
                }
                else if (match(s, "ru_ports_table"))
                {
                    xorif_clear_ru_ports_table();
                    response += sprintf(response, "status = 0\n");
                    return SUCCESS;
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf_alarms"))
                {
                    xobf_clear_bf_alarms();
                    response += sprintf(response, "status = 0\n");
                    return SUCCESS;
                }
                else if (match(s, "bf_stats"))
                {
                    xobf_clear_bf_stats();
                    response += sprintf(response, "status = 0\n");
                    return SUCCESS;
                }
#endif // BF_INCLUDED
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "configure" command.
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int configure(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 2)
        {
            // Note: might deprecate this in favour of the separate commands
            // configure <cc>
            unsigned int cc;
            if (parse_integer(1, &cc))
            {
                int result = xorif_configure_cc(cc);
#ifdef BF_INCLUDED
                if (result == XORIF_SUCCESS)
                {
                    if (has_bf())
                    {
                        return xobf_configure_cc(cc);
                    }
                }
#endif
                return result;
            }
        }
        else if (num_tokens == 3)
        {
            // configure [fhi | bf] <cc>
            const char *s;
            unsigned int cc;
            if (parse_string(1, &s) && parse_integer(2, &cc))
            {
                if (match(s, "fhi"))
                {
                    return xorif_configure_cc(cc);
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf"))
                {
                    return xobf_configure_cc(cc);
                }
#endif
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "enable" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int enable(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 2)
        {
            // enable <cc>
            unsigned int cc;
            if (parse_integer(1, &cc))
            {
                int result = xorif_enable_cc(cc);
#ifdef BF_INCLUDED
                if (result == XORIF_SUCCESS)
                {
                    if (has_bf())
                    {
                        return xobf_enable_cc(cc);
                    }
                }
#endif
                return result;
            }
        }
        else if (num_tokens == 3)
        {
            // enable [fhi | bf] <cc>
            const char *s;
            unsigned int cc;
            if (parse_string(1, &s) && parse_integer(2, &cc))
            {
                if (match(s, "fhi"))
                {
                    return xorif_enable_cc(cc);
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf"))
                {
                    return xobf_enable_cc(cc);
                }
#endif
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "disable" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int disable(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 2)
        {
            // disable <cc>
            unsigned int cc;
            if (parse_integer(1, &cc))
            {
                int result = xorif_disable_cc(cc);
#ifdef BF_INCLUDED
                if (result == XORIF_SUCCESS)
                {
                    if (has_bf())
                    {
                        return xobf_disable_cc(cc);
                    }
                }
#endif
                return result;
            }
        }
        else if (num_tokens == 3)
        {
            // disable [fhi | bf] <cc>
            const char *s;
            unsigned int cc;
            if (parse_string(1, &s) && parse_integer(2, &cc))
            {
                if (match(s, "fhi"))
                {
                    return xorif_disable_cc(cc);
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf"))
                {
                    return xobf_disable_cc(cc);
                }
#endif
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "read_reg" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int read_reg(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 3)
        {
            // read_reg [fhi | bf | bf0 | bf1] <name>
            const char *s1;
            const char *s2;
            if (parse_string(1, &s1) && parse_string(2, &s2))
            {
                uint32_t val;
                int result = 0;
                if (match(s1, "fhi"))
                {
                    result = xorif_read_fhi_reg(s2, &val);
                }
#ifdef BF_INCLUDED
                else if (match(s1, "bf") || match(s1, "bf0"))
                {
                    result = xobf_read_bf_reg(0, s2, &val);
                }
                else if (match(s1, "bf1"))
                {
                    result = xobf_read_bf_reg(1, s2, &val);
                }
#endif // BF_INCLUDED
                else
                {
                    return MALFORMED_COMMAND;
                }

                response += sprintf(response, "status = %d\n", result);
                if (result == 0)
                {
                    response += sprintf(response, "result = 0x%X\n", val);
                }
                else
                {
                    response += sprintf(response, "result = N/A\n");
                }
                return result;
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "read_reg_offset" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int read_reg_offset(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 4)
        {
            // read_reg_offset [fhi | bf | bf0 | bf1] <name> <offset>
            const char *s1;
            const char *s2;
            unsigned int offset;
            if (parse_string(1, &s1) && parse_string(2, &s2) && parse_integer(3, &offset))
            {
                uint32_t val;
                int result = 0;
                if (match(s1, "fhi"))
                {
                    result = xorif_read_fhi_reg_offset(s2, offset, &val);
                }
#ifdef BF_INCLUDED
                else if (match(s1, "bf") || match(s1, "bf0"))
                {
                    result = xobf_read_bf_reg_offset(0, s2, offset, &val);
                }
                else if (match(s1, "bf1"))
                {
                    result = xobf_read_bf_reg_offset(1, s2, offset, &val);
                }
#endif // BF_INCLUDED
                else
                {
                    return MALFORMED_COMMAND;
                }

                response += sprintf(response, "status = %d\n", result);
                if (result == 0)
                {
                    response += sprintf(response, "result = 0x%X\n", val);
                }
                else
                {
                    response += sprintf(response, "result = N/A\n");
                }
                return result;
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "write_reg" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int write_reg(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 4)
        {
            // write_reg [fhi | bf | bf0 | bf1] <name> <value>
            const char *s1;
            const char *s2;
            uint32_t val;
            if (parse_string(1, &s1) && parse_string(2, &s2) && parse_integer(3, &val))
            {
                int result = 0;
                if (match(s1, "fhi"))
                {
                    result = xorif_write_fhi_reg(s2, val);
                }
#ifdef BF_INCLUDED
                else if (match(s1, "bf") || match(s1, "bf0"))
                {
                    result = xobf_write_bf_reg(0, s2, val);
                }
                else if (match(s1, "bf1"))
                {
                    result = xobf_write_bf_reg(1, s2, val);
                }
#endif // BF_INCLUDED
                else
                {
                    return MALFORMED_COMMAND;
                }

                response += sprintf(response, "status = %d\n", result);
                return result;
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "write_reg_offset" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int write_reg_offset(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 5)
        {
            // write_reg_offset [fhi | bf | bf0 | bf1] <name> <offset> <value>
            const char *s1;
            const char *s2;
            unsigned int offset;
            uint32_t val;
            if (parse_string(1, &s1) && parse_string(2, &s2) && parse_integer(3, &offset) && parse_integer(4, &val))
            {
                int result = 0;
                if (match(s1, "fhi"))
                {
                    result = xorif_write_fhi_reg_offset(s2, offset, val);
                }
#ifdef BF_INCLUDED
                else if (match(s1, "bf") || match(s1, "bf0"))
                {
                    result = xobf_write_bf_reg_offset(0, s2, offset, val);
                }
                else if (match(s1, "bf1"))
                {
                    result = xobf_write_bf_reg_offset(1, s2, offset, val);
                }
#endif // BF_INCLUDED
                else
                {
                    return MALFORMED_COMMAND;
                }

                response += sprintf(response, "status = %d\n", result);
                return result;
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "dump_reg" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int dump_reg(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 2)
        {
            // dump_reg [fhi | bf | bf0 | bf1]
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "fhi"))
                {
                    response = dump_reg_fhi(response);
                    return SUCCESS;
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf") || match(s, "bf0"))
                {
                    response = dump_reg_bf(0, response);
                    return SUCCESS;
                }
                else if (match(s, "bf1"))
                {
                    response = dump_reg_bf(1, response);
                    return SUCCESS;
                }
#endif // BF_INCLUDED
            }
        }
        return MALFORMED_COMMAND;
    }
#endif // NO_HW
}

#ifdef LEGACY_ECPRI
/**
 * @brief "ecpri" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 * @note This command is handled by a separate protocol module
 */
static int ecpri(const char *request, char *response)
{
    // Command is passed directly to the eCPRI module for parsing and execution
    int result = ecpri_func(num_tokens - 1, &token[1], response);
    return result;
}
#endif

/**
 * @brief "peek" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int peek(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 2)
        {
            // peek <address>
            unsigned int addr;
            if (parse_integer(1, &addr))
            {
#if 0 // Using "peek" tool
                int result = FAILURE;

                char buff[256];

                // Create name for temporary file which will store the output
                // Closing file immediately, because we only really want the name
                // Note: Using mkstemp since tmpnam is deprecated
                char tmpName[] = "/tmp/xorifXXXXXX";
                int fd = mkstemp(tmpName);
                if (fd < 0)
                {
                    return SYSTEM_CMD_FAILURE;
                }
                close(fd);

                // Construct the command string for the 'system' call
                // The command output is redirected to the temporary file
                sprintf(buff, "peek 0x%X > %s", addr, tmpName);

                // Perform the 'system' call
                if (system(buff) == 0)
                {
                    // Get the result from the temporary file
                    FILE *fp = fopen(tmpName, "r");
                    if (fp)
                    {
                        // Only reading the 1st line of the file
                        char *s = fgets(buff, 256 - 1, fp);
                        fclose(fp);

                        // Remove any trailing newline character
                        if (buff[strlen(buff) - 1] == '\n')
                        {
                            buff[strlen(buff) - 1] = '\0';
                        }

                        // Success
                        result = SUCCESS;
                        response += sprintf(response, "status = %d\n", result);
                        response += sprintf(response, "result = %s\n", s);
                    }
                }

                if (result != SUCCESS)
                {
                    // Failure
                    result = SYSTEM_CMD_FAILURE;
                    response += sprintf(response, "status = %d\n", result);
                    response += sprintf(response, "result = N/A\n");
                }

                // Remove any temporary file generated by the 'system' call
                remove(tmpName);

                return result;

#else // Using mmap

                int result = FAILURE;
                int devmem = -1;
                unsigned int value = 0;
                unsigned char *buffer;
                unsigned int page_size;
                unsigned int mapped_size;
                unsigned int offset;
                unsigned int width = 4; // 32 bit memory access

                devmem = open("/dev/mem", O_RDONLY | O_SYNC);
                if (devmem >= 0)
                {
                    mapped_size = page_size = getpagesize();
                    offset = addr & (page_size - 1);
                    if ((offset + width) > page_size)
                    {
                        // Memory access spans 2 pages
                        mapped_size *= 2;
                    }
                    buffer = mmap(0, mapped_size, PROT_READ, MAP_SHARED, devmem, addr & ~(page_size - 1));
                    if (buffer != MAP_FAILED)
                    {
                        value = *(unsigned int *)(buffer + offset);
                        munmap(buffer, mapped_size);

                        // Success
                        result = SUCCESS;
                        response += sprintf(response, "status = %d\n", result);
                        response += sprintf(response, "result = 0x%08X\n", value);
                    }
                    close(devmem);
                }

                if (result != SUCCESS)
                {
                    // Failure
                    result = MEMORY_ACCESS_ERROR;
                    response += sprintf(response, "status = %d\n", result);
                    response += sprintf(response, "result = N/A\n");
                }
                return result;
#endif
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "poke" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int poke(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 3)
        {
            // poke <address> <value>
            unsigned int addr;
            unsigned int value;
            if (parse_integer(1, &addr) && parse_integer(2, &value))
            {
#if 0 // Using "poke" tool
                int result = FAILURE;
                char buff[256];

                // Construct the command string for the 'system' call
                sprintf(buff, "poke 0x%X 0x%X", addr, value);

                // Perform the 'system' call
                if (system(buff) == 0)
                {
                    // Success
                    result = SUCCESS;
                    response += sprintf(response, "status = %d\n", result);
                }
                else
                {
                    // Failure
                    result = SYSTEM_CMD_FAILURE;
                    response += sprintf(response, "status = %d\n", result);
                }

                return result;

#else // Using mmap

                int result = FAILURE;
                int devmem = -1;
                unsigned char *buffer;
                unsigned int page_size;
                unsigned int mapped_size;
                unsigned int offset;
                unsigned int width = 4; // 32 bit memory access

                devmem = open("/dev/mem", O_RDWR | O_SYNC);
                if (devmem >= 0)
                {
                    mapped_size = page_size = getpagesize();
                    offset = addr & (page_size - 1);
                    if ((offset + width) > page_size)
                    {
                        // Memory access spans 2 pages
                        mapped_size *= 2;
                    }
                    buffer = mmap(0, mapped_size, PROT_READ | PROT_WRITE, MAP_SHARED, devmem, addr & ~(page_size - 1));
                    if (buffer != MAP_FAILED)
                    {
                        *(unsigned int *)(buffer + offset) = value;
                        munmap(buffer, mapped_size);

                        // Success
                        result = SUCCESS;
                        response += sprintf(response, "status = %d\n", result);
                    }
                    close(devmem);
                }

                if (result != SUCCESS)
                {
                    // Failure
                    result = MEMORY_ACCESS_ERROR;
                    response += sprintf(response, "status = %d\n", result);
                }
                return result;
#endif
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

/**
 * @brief "debug" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int debug(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 2)
        {
            // debug <level = 0..2>
            unsigned int level;
            if (parse_integer(1, &level))
            {
                xorif_debug(level);
#ifdef BF_INCLUDED
                xobf_debug(level);
#endif
                return SUCCESS;
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

#ifdef BF_INCLUDED
/**
 * @brief "schedule_bf" command.
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int schedule_bf(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 1)
        {
            // schedule_bf
            return xobf_schedule_bf();
        }
        else if (num_tokens >= 2)
        {
            // schedule_bf {CxAx | Cx}
            // e.g. schedule_bf C0 C1 C0
            // e.g. schedule_bf C0A0 C0A1 C1 C0A0 C0A1

            struct xobf_schedule_info *ptr;
            int count = num_tokens - 1;
            ptr = calloc(count, sizeof(struct xobf_schedule_info));
            if (ptr)
            {
                for (int i = 0; i < count; ++i)
                {
                    int cc, ag;
                    if (sscanf(token[i + 1], "C%dA%d", &cc, &ag) == 2)
                    {
                        ptr[i].cc = cc;
                        ptr[i].ag = ag;
                    }
                    else if (sscanf(token[i + 1], "C%d", &cc) == 1)
                    {
                        ptr[i].cc = cc;
                        ptr[i].ag = 2; // NUM_ANTENNA_GROUPS
                    }
                    else
                    {
                        return MALFORMED_COMMAND;
                    }
                }

                int result = xobf_populate_bf_schedule(count, ptr);

                free(ptr);
                return result;
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}
#endif // BF_INCLUDED

#ifdef BF_INCLUDED
/**
 * @brief "load" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int load(const char *request, char *response)
{
    if (remote_target)
    {
        return send_to_target(request, response);
    }
#ifdef NO_HW
    return NO_HARDWARE;
#else
    else
    {
        if (num_tokens == 3)
        {
            // load [beam_weights] <file>
            const char *s1;
            const char *s2;
            if (parse_string(1, &s1) && parse_string(2, &s2))
            {
                if (match(s1, "beam_weights") || match(s1, "bf_weights"))
                {
                    return load_beam_weight_file(s2);
                }
#ifdef PRACH_CONFIG
                else if (match(s1, "prach_config"))
                {
                    if (match(s2, "null"))
                    {
                        // Using NULL for default config
                        return xobf_load_bf_prach_config(NULL);
                    }
                    else
                    {
                        return load_prach_config(s2);
                    }
                }
#endif
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}
#endif // BF_INCLUDED

#ifdef BF_INCLUDED
#ifndef NO_HW
/**
 * @brief Load text file containing beam weight vectors
 * @param name Name of file
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 * @note
 * The input file should contain an arbitrary number of beam-weight vectors descriptors.
 * The format of the beam-weight vector descriptor is shown in the following example...
 * 
 * beam_id = 1
 * comp_width = 12
 * comp_method = 1
 * comp_param = 7
 * num_weights = 32
 * 454 -407
 * -397 -431
 * -257 -438
 * -276 297
 * ...
 * 65 -223
 * -195 -36
 * -377 112
 * 
 * The comp_width, comp_mode and comp_param values are encoded the same as in the ORAN spec.
 * After the "num_weights" line the file should contain a number of pairs of values.
 * For example, for num_weights = 32, there should be 32 pairs of values, with the 1st value
 * of each pair being the in-phase weight component, and the 2nd value of the pair being the
 * quadrature weight component.
 * 
 * This function does not perform any compression of the data.
 * 
 * The current version supports 3 "types" of data file:
 * 1) normal beam-weight vectors (for when external store is present)
 * 2) cache-based beam-weight vectors (for loading directly into specific cache, when no external
 *    store is present). These BMVs also contain the SS, DIR, TYPE fields.
 * 3) cache-based SSB beam-weight vectors (for loading directly into SSB cache, when no external
 *    store is present). These BMVs don't have beam-id, but have additional into for CC, SYM, SRB
 *    and NRB which are used by the SSB cache system.
 * 
 * @todo The file parsing is not particularly robust. Probably better to use more robust
 * format and parser, e.g. JSON file or similar.
 */
static int load_beam_weight_file(const char *name)
{
    int result = FILE_READ_ERROR;
    char buff[MAX_BUFF_SIZE];
    int mode = 0;
    int beam_id = 0;
    int comp_width = 12;
    int comp_method = 1;
    int comp_param = 0;
    int num_weights = 64;
    uint32_t *data = NULL;
    int count = 0;
    int vector = 0;
    int i_val, q_val;

    // Open file
    FILE *fp = fopen(name, "r");
    if (fp == NULL)
    {
        return FILE_NOT_FOUND;
    }

    // Reset line count
    int num_lines = 0;

    // Read lines
    while (fgets(buff, MAX_BUFF_SIZE, fp) != NULL)
    {
        // Remove any trailing newline character
        if (buff[strlen(buff) - 1] == '\n')
        {
            buff[strlen(buff) - 1] = '\0';
        }

        // Count lines for debug / error reporting
        ++num_lines;

        // All BMVs start with a "beam_id" field
        if (sscanf(buff, "beam_id =%d", &beam_id) == 1)
        {
            // Info entry mode
            mode = 0;
        }
        else if (mode == 0)
        {
            // Info can be presented in any order, except for the "num_weights"
            sscanf(buff, "comp_width =%d", &comp_width);
            sscanf(buff, "comp_method =%d", &comp_method);
            sscanf(buff, "comp_param =%d", &comp_param);
            //sscanf(buff, "ss =%d", &ss);
            //sscanf(buff, "type =%d", &type);
            //sscanf(buff, "dir =%d", &dir);
            //sscanf(buff, "cc =%d", &cc);
            //sscanf(buff, "srb =%d", &srb);
            //sscanf(buff, "nrb =%d", &nrb);
            //sscanf(buff, "sym =%d", &sym);

            // The "num_weights" always precedes the weight data
            if (sscanf(buff, "num_weights =%d", &num_weights) == 1 && num_weights > 1)
            {
                // Weight entry mode
                mode = 1;
                count = 0;

                // Free storage if previously allocated
                if (data)
                {
                    free(data);
                }

                // Allocate storage for new weight values
                data = (uint32_t *)calloc(num_weights, sizeof(uint32_t));
            }
        }
        else if (mode == 1)
        {
            if (sscanf(buff, "%d %d", &i_val, &q_val) == 2)
            {
                data[count++] = (i_val & 0xFFFF) | ((q_val & 0xFFFF) << 16);
            }
            if (count == num_weights)
            {
                // Load beam-weight vector
                result = xobf_load_bf_beam_weights(beam_id, comp_width, comp_method, comp_param, num_weights, data);

                if (result == XORIF_SUCCESS)
                {
                    ++vector; // Count of successfully loaded vectors
                }
                else
                {
                    break;
                }
            }
        }
    }

    // Free storage if previously allocated
    if (data)
    {
        free(data);
    }

    // Close file
    fclose(fp);

    TRACE("%d beam-weight vectors successfully loaded\n", vector);

    return result;
}

#ifdef PRACH_CONFIG
/**
 * @brief Load config data for beamformer PRACH processing.
 * @param name Name of file
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 * @note
 * The input file should contain 64 32-bit hexadecimal numbers.
 */
static int load_prach_config(const char *name)
{
    int result = FILE_READ_ERROR;
    char buff[MAX_BUFF_SIZE];
    uint32_t data[PRACH_CONFIG_LENGTH];
    int count = 0;
    uint32_t number1, number2;

    // Open file
    FILE *fp = fopen(name, "r");
    if (fp == NULL)
    {
        return FILE_NOT_FOUND;
    }

    // Reset line count
    int num_lines = 0;

    // Read lines
    while (fgets(buff, MAX_BUFF_SIZE, fp) != NULL)
    {
        // Remove any trailing newline character
        if (buff[strlen(buff) - 1] == '\n')
        {
            buff[strlen(buff) - 1] = '\0';
        }

        // Count lines for debug / error reporting
        ++num_lines;

        if (sscanf(buff, "0x%x 0x%x", &number1, &number2) == 2)
        {
            // 2 words per line (low / high)
            if (count < (PRACH_CONFIG_LENGTH - 1))
            {
                data[count++] = number1;
                data[count++] = number2;
            }
        }
        else if (sscanf(buff, "0x%x", &number1) == 1)
        {
            // 1 word per line (low / high)
            if (count < PRACH_CONFIG_LENGTH)
            {
                data[count++] = number1;
            }
        }

        if (count >= PRACH_CONFIG_LENGTH)
        {
            result = xobf_load_bf_prach_config(data);
            break;
        }
    }

    // Close file
    fclose(fp);

    return result;
}
#endif // PRACH_CONFIG
#endif // NO_HW
#endif // BF_INCLUDED

#ifndef NO_HW
static int dump_fhi_register(char *response, const char *reg_name)
{
    int num = 0;
    int result;
    uint32_t val = 0;
    result = xorif_read_fhi_reg(reg_name, &val);
    if (result == XORIF_SUCCESS)
    {
        num = sprintf(response, "%s = %d\n", reg_name, val);
    }

    return num;
}

static char * dump_reg_fhi(char *response)
{
    response += sprintf(response, "status = 0\n");

    // Add registers to dump here...
    // TODO

    return response;
}

#ifdef BF_INCLUDED
static int dump_bf_register(int bank, char *response, const char *reg_name)
{
    int num = 0;
    int result;
    uint32_t val = 0;
    result = xobf_read_bf_reg(bank, reg_name, &val);
    if (result == XORIF_SUCCESS)
    {
        num = sprintf(response, "%s = %d\n", reg_name, val);
    }

    return num;
}

static char * dump_reg_bf(int bank, char *response)
{
    response += sprintf(response, "status = 0\n");

    // Add registers to dump here...
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_CACHE_DL_MISS");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_CPLANE_DL_NO_DESC");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_BF_DL_RE_MUTEX");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_AIE_DL_IN_SYNC");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_AIE_DL_OUT_SYNC");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_AIE_DL_UF");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_BWM_DL_NEXT_BIDN_UF");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_CACHE_UL_MISS");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_CPLANE_UL_NO_DESC");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_BF_UL_RE_MUTEX");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_AIE_UL_IN_SYNC");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_AIE_UL_OUT_SYNC");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_AIE_UL_UF");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_DFE_UL_CONFIG");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_BWM_UL_NEXT_BIDN_UF");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_PRACH_SEQ_LEN_ERR");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_PRACH_FORMAT_ERR");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_PRACH_SCS_INVALID");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_PRACH_NUM_SYM_INVALID");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_PRACH_FD_CH_TIMEOUT");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_PRACH_FD_ALLOC");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_PRACH_STARTPRBC_ERR");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_PRACH_NUMPRBC_ERR");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_PRACH_DFE_MISMATCH");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_PRACH_CMAC_INVALID");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_SSB_CACHE_MISS");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_SSB_CPLANE_NO_DESC");
    response += dump_bf_register(bank, response, "BF_ERRORS_ERROR_SSB_BF_RE_MUTEX");

    return response;
}
#endif // BF_INCLUDED
#endif // NO_HW

/** @} */