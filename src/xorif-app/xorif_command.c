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
 * @file xorif_command.c
 * @author Steven Dickinson
 * @brief Source file for the xorif-app command parser.
 * @addtogroup xorif-app
 * @{
 */

#include "xorif_app.h"

// Constants, enums, typedefs, structures, etc.
#define MAX_TOKENS 64

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
static int ecpri(const char *request, char *response);
static int peek(const char *request, char *response);
static int poke(const char *request, char *response);
static int debug(const char *request, char *response);
int ecpri_func(int argc, char **argv, char *resp);
#ifdef BF_INCLUDED
static int configure_bf(const char *request, char *response);
#endif
static int load(const char *request, char *response);
static int load_beam_weight_file(const char *name);

// Variables
static char *token[MAX_TOKENS];
static int num_tokens = 0;

// Usage strings
#ifdef BF_INCLUDED
#define GET_USAGE "get..." \
                  "\n\tget sw_version" \
                  "\n\tget fhi_hw_version" \
                  "\n\tget fhi_hw_revision" \
                  "\n\tget bf_hw_version" \
                  "\n\tget bf_hw_revision" \
                  "\n\tget [capabilities | caps]" \
                  "\n\tget [fhi_capabilities | fhi_caps]" \
                  "\n\tget [bf_capabilities | bf_caps]" \
                  "\n\tget cc_config <cc>" \
                  "\n\tget fhi_cc_alloc <cc>" \
                  "\n\tget fhi_stats <port>" \
                  "\n\tget bf_stats" \
                  "\n\tget state" \
                  "\n\tget fhi_alarms" \
                  "\n\tget bf_alarms"

#define SET_USAGE "set..." \
                  "\n\tset num_rbs <cc> <number of rbs>" \
                  "\n\tset numerology <cc> <numerology> <extended CP = 0 | 1>" \
                  "\n\tset numerology_ssb <cc> <numerology> <extended CP = 0 | 1>" \
                  "\n\tset time_advance <cc> <deskew> <advance up-link> <advance down-link>" \
                  "\n\tset [dl_iq_compression | dl_iq_comp] <cc> <width> <method>" \
                  "\n\tset [ul_iq_compression | ul_iq_comp] <cc> <width> <method>" \
                  "\n\tset [ssb_iq_compression | ssb_iq_comp] <cc> <width> <method>" \
                  "\n\tset [bw_compression | bw_comp] <cc> <width> <method>" \
                  "\n\tset dl_sections_per_sym <cc> <num sections>" \
                  "\n\tset ul_sections_per_sym <cc> <num sections>" \
                  "\n\tset ssb_sections_per_sym <cc> <num sections>" \
                  "\n\tset frames_per_sym <cc> <num frames>" \
                  "\n\tset frames_per_sym_ssb <cc> <num frames>" \
                  "\n\tset dest_mac_addr <port> <address>" \
                  "\n\tset src_mac_addr <port> <address>" \
                  "\n\tset protocol <ECPRI | 1914.3> <VLAN = 0 | 1> <RAW | IPv4 | IPv6>" \
                  "\n\tset eAxC_id <DU bits> <BS bits> <CC bits> <RU bits>" \
                  "\n\tset ru_ports <RU bits> <ss bits> <mask> <user value> <PRACH value> <SSB value>"

#else

#define GET_USAGE "get..." \
                  "\n\tget sw_version" \
                  "\n\tget fhi_hw_version" \
                  "\n\tget fhi_hw_revision" \
                  "\n\tget [capabilities | caps]" \
                  "\n\tget [fhi_capabilities | fhi_caps]" \
                  "\n\tget cc_config <cc>" \
                  "\n\tget fhi_cc_alloc <cc>" \
                  "\n\tget fhi_stats <port>" \
                  "\n\tget state" \
                  "\n\tget fhi_alarms"

#define SET_USAGE "set..." \
                  "\n\tset num_rbs <cc> <number of rbs>" \
                  "\n\tset numerology <cc> <numerology> <extended CP = 0 | 1>" \
                  "\n\tset numerology_ssb <cc> <numerology> <extended CP = 0 | 1>" \
                  "\n\tset time_advance <cc> <deskew> <advance up-link> <advance down-link>" \
                  "\n\tset [dl_iq_compression | dl_iq_comp] <cc> <width> <method>" \
                  "\n\tset [ul_iq_compression | ul_iq_comp] <cc> <width> <method>" \
                  "\n\tset [ssb_iq_compression | ssb_iq_comp] <cc> <width> <method>" \
                  "\n\tset dl_sections_per_sym <cc> <num sections>" \
                  "\n\tset ul_sections_per_sym <cc> <num sections>" \
                  "\n\tset ssb_sections_per_sym <cc> <num sections>" \
                  "\n\tset frames_per_sym <cc> <num frames>" \
                  "\n\tset frames_per_sym_ssb <cc> <num frames>" \
                  "\n\tset dest_mac_addr <port> <address>" \
                  "\n\tset src_mac_addr <port> <address>" \
                  "\n\tset protocol <ECPRI | 1914.3> <VLAN = 0 | 1> <RAW | IPv4 | IPv6>" \
                  "\n\tset eAxC_id <DU bits> <BS bits> <CC bits> <RU bits>" \
                  "\n\tset ru_ports <RU bits> <ss bits> <mask> <user value> <PRACH value> <SSB value>"

#endif // BF_INCLUDED

#define ECPRI_USAGE "(Use 'ecpri help' for additional help and usage information)"

// Set of commands (must be terminated with NULLs)
const static struct command command_set[] =
{
#ifdef BF_INCLUDED
    { "help", help, "help [<topic>]" },
    { "init", init, "init [<fhi device> <bf device>]" },
    { "finish", finish, "finish" },
    { "reset", reset, "reset [fhi | bf] <mode>" },
    { "has", has, "has [fhi | bf]" },
    { "get", get, GET_USAGE },
    { "set", set, SET_USAGE },
    { "clear", clear, "clear [fhi_alarms | fhi_stats | bf_alarms | bf_stats]" },
    { "configure", configure, "configure <cc>" },
    { "enable", enable, "enable <cc>" },
    { "disable", disable, "disable <c>" },
    { "configure_bf", configure_bf, "configure_bf" },
    { "load", load, "load [bf_weights <file>]"},
    { "read_reg", read_reg, "read_reg [fhi | bf] <name>" },
    { "read_reg_offset", read_reg_offset, "read_reg_offset [fhi | bf] <name> <offset>" },
    { "write_reg", write_reg, "write_reg [fhi | bf] <name> <value>" },
    { "write_reg_offset", write_reg_offset, "write_reg_offset [fhi | bf] <name> <offset> <value>" },
    { "quit", quit, "quit" },
    { "ecpri", ecpri, ECPRI_USAGE },
    { "peek", peek, "peek <address>" },
    { "poke", poke, "poke <address> <value>" },
    { "debug", debug, "debug <level = 0..2>" },
    // Last line must be all NULL's
    { NULL, NULL, NULL }
#else
    { "help", help, "help [<topic>]" },
    { "init", init, "init [<fhi device>]" },
    { "finish", finish, "finish" },
    { "reset", reset, "reset [fhi] <mode>" },
    { "has", has, "has [fhi | bf]" },
    { "get", get, GET_USAGE },
    { "set", set, SET_USAGE },
    { "clear", clear, "clear [fhi_alarms | fhi_stats]" },
    { "configure", configure, "configure <cc>" },
    { "enable", enable, "enable <cc>" },
    { "disable", disable, "disable <c>" },
    { "read_reg", read_reg, "read_reg [fhi] <name>" },
    { "read_reg_offset", read_reg_offset, "read_reg_offset [fhi] <name> <offset>" },
    { "write_reg", write_reg, "write_reg [fhi] <name> <value>" },
    { "write_reg_offset", write_reg_offset, "write_reg_offset [fhi] <name> <offset> <value>" },
    { "quit", quit, "quit" },
    { "ecpri", ecpri, ECPRI_USAGE },
    { "peek", peek, "peek <address>" },
    { "poke", poke, "poke <address> <value>" },
    { "debug", debug, "debug <level = 0..2>" },
    // Last line must be all NULL's
    { NULL, NULL, NULL }
#endif
};

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
    if (num_tokens == 1)
    {
        // quit
        return TERMINATE;
    }
    return MALFORMED_COMMAND;
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
            return xorif_init(NULL, NULL);
        }
        else if (num_tokens == 2)
        {
            // init <fhi device>
            const char *s;
            if (parse_string(1, &s))
            {
                return xorif_init(s, NULL);
            }
        }
        else if (num_tokens == 3)
        {
            // init <fhi device> <bf device>
            const char *s1;
            const char *s2;
            if (parse_string(1, &s1) && parse_string(2, &s2))
            {
                return xorif_init(s1, s2);
            }
        }
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
            return SUCCESS;
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
        if (num_tokens == 3)
        {
            // reset [fhi | bf] <mode>
            const char *s;
            unsigned int mode;
            if (parse_string(1, &s) && parse_integer(2, &mode))
            {
                if (match(s, "fhi"))
                {
                    xorif_reset_fhi(mode);
                    return SUCCESS;
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf"))
                {
                    xorif_reset_bf(mode);
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
                    int result = xorif_has_beamformer();
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
                if (match(s, "state") && num_tokens == 2)
                {
                    // get state
                    int result = xorif_get_state();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "state = %d\n", result);
                    return SUCCESS;
                }
                else if (match(s, "sw_version") && num_tokens == 2)
                {
                    // get sw_version
                    uint32_t result = xorif_get_sw_version();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", result);
                    return SUCCESS;
                }
                else if (match(s, "fhi_hw_version") && num_tokens == 2)
                {
                    // get fhi_hw_version
                    uint32_t result = xorif_get_fhi_hw_version();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", result);
                    return SUCCESS;
                }
                else if (match(s, "fhi_hw_revision") && num_tokens == 2)
                {
                    // get fhi_hw_revision
                    uint32_t result = xorif_get_fhi_hw_revision();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", result);
                    return SUCCESS;
                }
                else if ((match(s, "capabilities") || match(s, "caps")) && num_tokens == 2)
                {
                    // get [capabilities | caps]
                    const struct xorif_caps *ptr = xorif_get_capabilities();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "max_cc = %d\n", ptr->max_cc);
                    response += sprintf(response, "num_eth_ports = %d\n", ptr->num_eth_ports);
                    response += sprintf(response, "numerologies = 0x%X\n", ptr->numerologies);
                    response += sprintf(response, "extended_cp = %s\n", ptr->extended_cp ? "true" : "false");
                    response += sprintf(response, "iq_comp_methods = 0x%X\n", ptr->iq_comp_methods);
                    response += sprintf(response, "bw_comp_methods = 0x%X\n", ptr->bw_comp_methods);
                    return SUCCESS;
                }
                else if ((match(s, "fhi_capabilities") || match(s, "fhi_caps")) && num_tokens == 2)
                {
                    // get [fhi_capabilities | fhi_caps]
                    const struct xorif_fhi_caps *ptr = xorif_get_fhi_capabilities();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "max_cc = %d\n", ptr->max_cc);
                    response += sprintf(response, "num_eth_ports = %d\n", ptr->num_eth_ports);
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
                else if (match(s, "cc_config") && num_tokens == 3)
                {
                    // get cc_config <cc>
                    unsigned int cc;
                    if (parse_integer(2, &cc))
                    {
                        struct xorif_cc_config config;
                        int result = xorif_get_cc_config(cc, &config);
                        if (result == XORIF_SUCCESS)
                        {
                            response += sprintf(response, "status = 0\n");
                            response += sprintf(response, "num_rbs = %d\n", config.num_rbs);
                            response += sprintf(response, "num_rbs_ssb = %d\n", config.num_rbs_ssb);
                            response += sprintf(response, "numerology = %d\n", config.numerology);
                            response += sprintf(response, "extended_cp = %s\n", config.extended_cp ? "true" : "false");
                            response += sprintf(response, "numerology_ssb = %d\n", config.numerology_ssb);
                            response += sprintf(response, "extended_cp_ssb = %s\n", config.extended_cp_ssb ? "true" : "false");
                            response += sprintf(response, "iq_comp_meth_ul = %d\n", config.iq_comp_meth_ul);
                            response += sprintf(response, "iq_comp_width_ul = %d\n", config.iq_comp_width_ul);
                            response += sprintf(response, "iq_comp_meth_dl = %d\n", config.iq_comp_meth_dl);
                            response += sprintf(response, "iq_comp_width_dl = %d\n", config.iq_comp_width_dl);
                            response += sprintf(response, "iq_comp_meth_ssb = %d\n", config.iq_comp_meth_ssb);
                            response += sprintf(response, "iq_comp_width_ssb = %d\n", config.iq_comp_width_ssb);
                            response += sprintf(response, "bw_comp_meth = %d\n", config.bw_comp_meth);
                            response += sprintf(response, "bw_comp_width = %d\n", config.bw_comp_width);
                            response += sprintf(response, "deskew = %d\n", config.deskew);
                            response += sprintf(response, "advance_ul = %d\n", config.advance_ul);
                            response += sprintf(response, "advance_dl = %d\n", config.advance_dl);
                            response += sprintf(response, "num_ctrl_per_sym_ul = %d\n", config.num_ctrl_per_sym_ul);
                            response += sprintf(response, "num_ctrl_per_sym_dl = %d\n", config.num_ctrl_per_sym_dl);
                            response += sprintf(response, "num_ctrl_per_sym_ssb = %d\n", config.num_ctrl_per_sym_ssb);
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
                            response += sprintf(response, "ul_ctrl_offset_size = %d\n", alloc.ul_ctrl_offset_size);
                            response += sprintf(response, "ul_ctrl_base_offset = %d\n", alloc.ul_ctrl_base_offset);
                            response += sprintf(response, "ul_ctrl_base_offset_size = %d\n", alloc.ul_ctrl_base_offset_size);
                            response += sprintf(response, "dl_ctrl_offset = %d\n", alloc.dl_ctrl_offset);
                            response += sprintf(response, "dl_ctrl_offset_size = %d\n", alloc.dl_ctrl_offset_size);
                            response += sprintf(response, "dl_data_sym_start = %d\n", alloc.dl_data_sym_start);
                            response += sprintf(response, "dl_data_buff_start = %d\n", alloc.dl_data_buff_start);
                            response += sprintf(response, "dl_data_buff_size = %d\n", alloc.dl_data_buff_size);
                            response += sprintf(response, "ssb_ctrl_offset = %d\n", alloc.ssb_ctrl_offset);
                            response += sprintf(response, "ssb_ctrl_offset_size = %d\n", alloc.ssb_ctrl_offset_size);
                            response += sprintf(response, "ssb_data_sym_start = %d\n", alloc.ssb_data_sym_start);
                            response += sprintf(response, "ssb_data_buff_start = %d\n", alloc.ssb_data_buff_start);
                            response += sprintf(response, "ssb_data_buff_size = %d\n", alloc.ssb_data_buff_size);
                            return SUCCESS;
                        }
                    }
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf_hw_version") && num_tokens == 2)
                {
                    // get bf_hw_version
                    uint32_t result = xorif_get_bf_hw_version();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", result);
                    return SUCCESS;
                }
                else if (match(s, "bf_hw_revision") && num_tokens == 2)
                {
                    // get bf_hw_revision
                    uint32_t result = xorif_get_bf_hw_revision();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", result);
                    return SUCCESS;
                }
                else if ((match(s, "bf_capabilities") || match(s, "bf_caps")) && num_tokens == 2)
                {
                    // get [bf_capabilities | bf_caps]
                    const struct xorif_bf_caps *ptr = xorif_get_bf_capabilities();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "max_cc = %d\n", ptr->max_cc);
                    response += sprintf(response, "num_eth_ports = %d\n", ptr->num_eth_ports);
                    response += sprintf(response, "uram_cache = %d\n", ptr->uram_cache);
                    response += sprintf(response, "num_antennas = %d\n", ptr->num_antennas);
                    response += sprintf(response, "num_dfes = %d\n", ptr->num_dfes);
                    response += sprintf(response, "num_ss_dl = %d\n", ptr->num_ss_dl);
                    response += sprintf(response, "num_ss_ul = %d\n", ptr->num_ss_ul);
                    response += sprintf(response, "external_bw_store = %s\n", ptr->external_bw_store ? "true" : "false");
                    response += sprintf(response, "cache_clk = %s\n", ptr->cache_clk ? "true" : "false");
                    response += sprintf(response, "aie_if_clk = %s\n", ptr->aie_if_clk ? "true" : "false");
                    return SUCCESS;
                }
                else if (match(s, "bf_alarms") && num_tokens == 2)
                {
                    // get bf_alarms
                    uint32_t result = xorif_get_bf_alarms();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%X\n", result);
                    return SUCCESS;
                }
                else if (match(s, "bf_stats") && num_tokens == 2)
                {
                    // get bf_stats
                    struct xorif_bf_stats stats;
                    int result = xorif_get_bf_stats(&stats);
                    if (result == XORIF_SUCCESS)
                    {
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "cache_miss_count = %d\n", stats.cache_miss_count);
                        return SUCCESS;
                    }
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
                        return xorif_set_cc_num_rbs(cc, val);
                    }
                }
                else if (match(s, "numerology") && num_tokens == 5)
                {
                    // set numerology <cc> <numerology> <extended CP = 0 | 1>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_numerology(cc, val1, val2);
                    }
                }
                else if (match(s, "numerology_ssb") && num_tokens == 5)
                {
                    // set numerology_ssb <cc> <numerology> <extended CP = 0 | 1>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_numerology_ssb(cc, val1, val2);
                    }
                }
                else if (match(s, "time_advance") && num_tokens == 6)
                {
                    // set time_advance <cc> <deskew> <advance up-link> <advance down-link>
                    unsigned int cc, val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2) && parse_integer(5, &val3))
                    {
                        return xorif_set_cc_time_advance(cc, val1, val2, val3);
                    }
                }
                else if ((match(s, "dl_iq_compression") || match(s, "dl_iq_comp")) && num_tokens == 5)
                {
                    // set [dl_iq_compression | dl_iq_comp] <cc> <width> <method>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_dl_iq_compression(cc, val1, val2);
                    }
                }
                else if ((match(s, "ul_iq_compression") || match(s, "ul_iq_comp")) && num_tokens == 5)
                {
                    // set [ul_iq_compression | ul_iq_comp] <cc> <width> <method>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_ul_iq_compression(cc, val1, val2);
                    }
                }
                else if ((match(s, "ssb_iq_compression") || match(s, "ssb_iq_comp")) && num_tokens == 5)
                {
                    // set [ssb_iq_compression | ssb_iq_comp] <cc> <width> <method>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_iq_compression_ssb(cc, val1, val2);
                    }
                }
                else if ((match(s, "bw_compression") || match(s, "bw_comp")) && num_tokens == 5)
                {
                    // set [bw_compression | bw_comp] <cc> <width> <method>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_bw_compression(cc, val1, val2);
                    }
                }
                else if (match(s, "dl_sections_per_sym") && num_tokens == 4)
                {
                    // set dl_sections_per_sym <cc> <num sections>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_dl_sections_per_symbol(cc, val);
                    }
                }
                else if (match(s, "ul_sections_per_sym") && num_tokens == 4)
                {
                    // set ul_sections_per_sym <cc> <num sections>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_ul_sections_per_symbol(cc, val);
                    }
                }
                else if (match(s, "ssb_sections_per_sym") && num_tokens == 4)
                {
                    // set ssb_sections_per_sym <cc> <num sections>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_sections_per_symbol_ssb(cc, val);
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
            // clear [fhi_alarms | fhi_stats | bf_alarms | bf_stats]
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
#ifdef BF_INCLUDED
                else if (match(s, "bf_alarms"))
                {
                    xorif_clear_bf_alarms();
                    response += sprintf(response, "status = 0\n");
                    return SUCCESS;
                }
                else if (match(s, "bf_stats"))
                {
                    xorif_clear_bf_stats();
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
            // configure <cc>
            unsigned int cc;
            if (parse_integer(1, &cc))
            {
                return xorif_configure_cc(cc);
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
                return xorif_enable_cc(cc);
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
                return xorif_disable_cc(cc);
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
            // read_reg [fhi | bf] <name>
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
                else if (match(s1, "bf"))
                {
                    result = xorif_read_bf_reg(s2, &val);
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
            // read_reg_offset [fhi | bf] <name> <offset>
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
                else if (match(s1, "bf"))
                {
                    result = xorif_read_bf_reg_offset(s2, offset, &val);
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
            // write_reg [fhi | bf] <name> <value>
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
                else if (match(s1, "bf"))
                {
                    result = xorif_write_bf_reg(s2, val);
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
            // write_reg_offset [fhi | bf] <name> <offset> <value>
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
                else if (match(s1, "bf"))
                {
                    result = xorif_write_bf_reg_offset(s2, offset, val);
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
            unsigned int val;
            if (parse_integer(1, &addr) && parse_integer(2, &val))
            {
                int result = FAILURE;
                char buff[256];

                // Construct the command string for the 'system' call
                sprintf(buff, "poke 0x%X 0x%X", addr, val);

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
                return SUCCESS;
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

#ifdef BF_INCLUDED
/**
 * @brief "configure_bf" command.
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int configure_bf(const char *request, char *response)
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
            // configure_bf
            return xorif_configure_bf();
        }
        return MALFORMED_COMMAND;
    }
#endif
}
#endif // BF_INCLUDED

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
            // load [bf_weights <file>]
            const char *s1;
            const char *s2;
            if (parse_string(1, &s1) && parse_string(2, &s2))
            {
#ifdef BF_INCLUDED
                if (match(s1, "bf_weights"))
                {
                    return load_beam_weight_file(s2);
                }
#endif
            }
        }
        return MALFORMED_COMMAND;
    }
#endif
}

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
    int result = XORIF_FAILURE;
    char buff[MAX_BUFF_SIZE];
    int mode = 0;
    int beam_id = 0;
    int comp_width = 12;
    int comp_method = 1;
    int comp_param = 0;
    int num_weights = 32;
    int ss = 0;
    int type = 0;
    int dir = 0;
    int cc = 0;
    int srb = 0;
    int nrb = 0;
    int sym = 0;
    uint32_t *data = NULL;
    int count;
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

            // Set "type" to -1. If it gets set to real value later then we'll
            // know that a cache-based BMV is being specified.
            type = -1;
        }
        else if (mode == 0)
        {
            // Info can be presented in any order, except for the "num_weights"
            sscanf(buff, "comp_width =%d", &comp_width);
            sscanf(buff, "comp_method =%d", &comp_method);
            sscanf(buff, "comp_param =%d", &comp_param);
            sscanf(buff, "ss =%d", &ss);
            sscanf(buff, "type =%d", &type);
            sscanf(buff, "dir =%d", &dir);
            sscanf(buff, "cc =%d", &cc);
            sscanf(buff, "srb =%d", &srb);
            sscanf(buff, "nrb =%d", &nrb);
            sscanf(buff, "sym =%d", &sym);

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
                if (beam_id == 0)
                {
                    // Cache-based SSB BMV (no external store)
                    result = xorif_load_bf_beam_weights_alt2(beam_id, comp_width, comp_method, comp_param, cc, sym, srb, nrb, num_weights, data);
                }
                else
                {
                    if (type == -1)
                    {
                        // Normal (external store)
                        result = xorif_load_bf_beam_weights(beam_id, comp_width, comp_method, comp_param, num_weights, data);
                    }
                    else
                    {
                        // Cache-based BMV (no external store)
                        result = xorif_load_bf_beam_weights_alt1(beam_id, comp_width, comp_method, comp_param, ss, dir, type, num_weights, data);
                    }
                }

                if (result == XORIF_SUCCESS)
                {
                    ++count;
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

    return result;
}
#endif // NO_HW
#endif // BF_INCLUDED

/** @} */