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
                  "\n\tget capabilities" \
                  "\n\tget fhi_caps" \
                  "\n\tget bf_caps" \
                  "\n\tget cc_config <cc>" \
                  "\n\tget fhi_cc_alloc <cc>" \
                  "\n\tget fhi_stats" \
                  "\n\tget bf_stats" \
                  "\n\tget state" \
                  "\n\tget fhi_alarms" \
                  "\n\tget bf_alarms"

#define SET_USAGE "set..." \
                  "\n\tset num_rbs <cc> <number of rbs>" \
                  "\n\tset numerology <cc> <numerology> <extended CP>" \
                  "\n\tset time_advance <cc> <deskew> <advance up-link> <advance down-link>" \
                  "\n\tdl_iq_compression <cc> <width> <compression method>" \
                  "\n\tul_iq_compression <cc> <width> <compression method>" \
                  "\n\tbw_compression <cc> <width> <compression method>" \
                  "\n\tdest_mac_addr <port> <address>" \
                  "\n\tsrc_mac_addr <port> <address>"

#define CLEAR_USAGE "clear..." \
                    "\n\tclear fhi_alarms" \
                    "\n\tclear bf_alarms"

#else

#define GET_USAGE "get..." \
                  "\n\tget sw_version" \
                  "\n\tget fhi_hw_version" \
                  "\n\tget fhi_hw_revision" \
                  "\n\tget capabilities" \
                  "\n\tget fhi_caps" \
                  "\n\tget cc_config <cc>" \
                  "\n\tget fhi_cc_alloc <cc>" \
                  "\n\tget fhi_stats" \
                  "\n\tget state" \
                  "\n\tget fhi_alarms" \

#define SET_USAGE "set..." \
                  "\n\tset num_rbs <cc> <number of rbs>" \
                  "\n\tset numerology <cc> <numerology> <extended CP>" \
                  "\n\tset time_advance <cc> <deskew> <advance up-link> <advance down-link>" \
                  "\n\tdl_iq_compression <cc> <width> <compression method>" \
                  "\n\tul_iq_compression <cc> <width> <compression method>" \
                  "\n\tbw_compression <cc> <width> <compression method>" \
                  "\n\tdest_mac_addr <port> <address>" \
                  "\n\tsrc_mac_addr <port> <address>"

#define CLEAR_USAGE "clear..." \
                    "\n\tclear fhi_alarms"

#endif // BF_INCLUDED

#define ECPRI_USAGE "(Use 'ecpri help' for additional help and usage information)"

// Set of commands (must be terminated with NULLs)
const static struct command command_set[] =
{
    { "help", help, "help [<topic>]" },
    { "init", init, "init [<device> <device>]" },
    { "finish", finish, "finish" },
#ifdef BF_INCLUDED
    { "reset", reset, "reset [fhi | bf]" },
    { "has", has, "has [fhi | bf]" },
    { "get", get, GET_USAGE },
    { "set", set, SET_USAGE },
    { "clear", clear, CLEAR_USAGE },
#else
    { "reset", reset, "reset [fhi]" },
    { "has", has, "has [fhi | bf]" },
    { "get", get, GET_USAGE },
    { "set", set, SET_USAGE },
    { "clear", clear, CLEAR_USAGE },
#endif // BF_INCLUDED
    { "configure", configure, "configure <cc>" },
    { "enable", enable, "enable <cc>" },
    { "disable", disable, "disable <c>" },
#ifdef BF_INCLUDED
    { "configure_bf", configure_bf, "configure_bf" },
    { "read_reg", read_reg, "read_reg [fhi | bf] <name>" },
    { "read_reg_offset", read_reg_offset, "read_reg_offset [fhi | bf] <name> <offset>" },
    { "write_reg", write_reg, "write_reg [fhi | bf] <name> <value>" },
    { "write_reg_offset", write_reg_offset, "write_reg_offset [fhi | bf] <name> <offset> <value>" },
#else
    { "read_reg", read_reg, "read_reg [fhi] <name>" },
    { "read_reg_offset", read_reg_offset, "read_reg_offset [fhi] <name> <offset>" },
    { "write_reg", write_reg, "write_reg [fhi] <name> <value>" },
    { "write_reg_offset", write_reg_offset, "write_reg_offset [fhi] <name> <offset> <value>" },
#endif // BF_INCLUDED
    { "quit", quit, "quit" },
    { "ecpri", ecpri, ECPRI_USAGE },
    { "peek", peek, "peek <address>" },
    { "poke", poke, "poke <address> <value>" },
    { "debug", debug, "debug <level>" },
    // Last line must be all NULL's
    { NULL, NULL, NULL }
};

/**
 * @brief Match two strings.
 * @param[in] s1 First string
 * @param[in] s2 Second string
 * @returns
 *      - 0 if they don't match
 *      - 1 if both strings match
 * @todo Add options for case-insensitivity and partial matching
 */
static int match(const char *s1, const char *s2)
{
    return strcmp(s1, s2) == 0;
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
    if (sscanf(token[tok_id], "0x%x", &u) == 1)
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
    else
    {
        // Local host, send to standard output
        // TODO optional formating, e.g. error code string expansion, pretty-formating, etc.
        printf("%s => %s", request, response);
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
        // Help for all topics
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
        // Help for specified topic
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
            // No device names specified
            return xorif_init(NULL, NULL);
        }
        else if (num_tokens == 2)
        {
            const char *s;
            if (parse_string(1, &s))
            {
                // Device names specified
                return xorif_init(s, NULL);
            }
        }
        else if (num_tokens == 3)
        {
            const char *s1;
            const char *s2;
            if (parse_string(1, &s1) && parse_string(2, &s2))
            {
                // Device names specified
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
        if (num_tokens == 2)
        {
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "fhi"))
                {
                    xorif_reset_fhi();
                    return SUCCESS;
                }
#ifdef BF_INCLUDED
                else if (match(s, "bf"))
                {
                    xorif_reset_bf();
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
                if (num_tokens == 2)
                {
                    if (match(s, "state"))
                    {
                        int result = xorif_get_state();
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "state = %d\n", result);
                        return SUCCESS;
                    }
                    else if (match(s, "sw_version"))
                    {
                        uint32_t result = xorif_get_sw_version();
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "result = 0x%08x\n", result);
                        return SUCCESS;
                    }
                    else if (match(s, "fhi_hw_version"))
                    {
                        uint32_t result = xorif_get_fhi_hw_version();
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "result = 0x%08x\n", result);
                        return SUCCESS;
                    }
                    else if (match(s, "fhi_hw_revision"))
                    {
                        uint32_t result = xorif_get_fhi_hw_revision();
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "result = 0x%08x\n", result);
                        return SUCCESS;
                    }
                    else if (match(s, "capabilities"))
                    {
                        const struct xorif_caps *ptr = xorif_get_capabilities();
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "max_cc = %d\n", ptr->max_cc);
                        response += sprintf(response, "num_eth_ports = %d\n", ptr->num_eth_ports);
                        response += sprintf(response, "numerologies = 0x%x\n", ptr->numerologies);
                        response += sprintf(response, "extended_cp = %s\n", ptr->extended_cp ? "true" : "false");
                        response += sprintf(response, "iq_comp_methods = 0x%x\n", ptr->iq_comp_methods);
                        response += sprintf(response, "bw_comp_methods = 0x%x\n", ptr->bw_comp_methods);
                        return SUCCESS;
                    }
                    else if (match(s, "fhi_caps"))
                    {
                        const struct xorif_fhi_caps *ptr = xorif_get_fhi_capabilities();
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "max_cc = %d\n", ptr->max_cc);
                        response += sprintf(response, "num_eth_ports = %d\n", ptr->num_eth_ports);
                        response += sprintf(response, "no_framer_antenna = %d\n", ptr->no_framer_antenna);
                        response += sprintf(response, "no_deframer_antennas = %d\n", ptr->no_deframer_antennas);
                        response += sprintf(response, "max_dl_symbols = %d\n", ptr->max_dl_symbols);
                        response += sprintf(response, "max_framer_ethernet_pkt = %d\n", ptr->max_framer_ethernet_pkt);
                        response += sprintf(response, "max_deframer_ethernet_pkt = %d\n", ptr->max_deframer_ethernet_pkt);
                        response += sprintf(response, "max_subcarriers = %d\n", ptr->max_subcarriers);
                        response += sprintf(response, "max_ctrl_symbols = %d\n", ptr->max_ctrl_symbols);
                        response += sprintf(response, "max_ul_ctrl_1kwords = %d\n", ptr->max_ul_ctrl_1kwords);
                        response += sprintf(response, "max_dl_ctrl_1kwords = %d\n", ptr->max_dl_ctrl_1kwords);
                        response += sprintf(response, "max_dl_data_1kwords = %d\n", ptr->max_dl_data_1kwords);
                        response += sprintf(response, "timer_clk_ps = %d\n", ptr->timer_clk_ps);
                        response += sprintf(response, "num_unsolicited_ports = %d\n", ptr->num_unsolicited_ports);
                        response += sprintf(response, "num_prach_ports = %d\n", ptr->num_prach_ports);
                        return SUCCESS;
                    }
                    else if (match(s, "fhi_alarms"))
                    {
                        uint32_t result = xorif_get_fhi_alarms();
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "result = 0x%x\n", result);
                        return SUCCESS;
                    }
                    else if (match(s, "fhi_stats"))
                    {
                        struct xorif_fhi_stats stats;
                        int result = xorif_get_fhi_stats(&stats);
                        if (result == XORIF_SUCCESS)
                        {
                            response += sprintf(response, "status = 0\n");
                            response += sprintf(response, "total_rx_good_pkt_cnt = %d\n", stats.total_rx_good_pkt_cnt);
                            response += sprintf(response, "total_rx_bad_pkt_cnt = %d\n", stats.total_rx_bad_pkt_cnt);
                            response += sprintf(response, "total_rx_bad_fcs_cnt = %d\n", stats.total_rx_bad_fcs_cnt);
                            response += sprintf(response, "user_data_rx_packets_cnt = %d\n", stats.user_data_rx_packets_cnt);
                            response += sprintf(response, "user_data_rx_good_pkt_cnt = %d\n", stats.user_data_rx_good_pkt_cnt);
                            response += sprintf(response, "user_data_rx_bad_pkt_cnt = %d\n", stats.user_data_rx_bad_pkt_cnt);
                            response += sprintf(response, "user_data_rx_bad_fcs_cnt = %d\n", stats.user_data_rx_bad_fcs_cnt);
                            response += sprintf(response, "user_ctrl_rx_packets_cnt = %d\n", stats.user_ctrl_rx_packets_cnt);
                            response += sprintf(response, "user_ctrl_rx_good_pkt_cnt = %d\n", stats.user_ctrl_rx_good_pkt_cnt);
                            response += sprintf(response, "user_ctrl_rx_bad_pkt_cnt = %d\n", stats.user_ctrl_rx_bad_pkt_cnt);
                            response += sprintf(response, "user_ctrl_rx_bad_fcs_cnt = %d\n", stats.user_ctrl_rx_bad_fcs_cnt);
                            response += sprintf(response, "user_data_rx_pkts_rate = %d\n", stats.user_data_rx_pkts_rate);
                            response += sprintf(response, "user_ctrl_rx_pkts_rate = %d\n", stats.user_ctrl_rx_pkts_rate);
                            return SUCCESS;
                        }
                    }
#ifdef BF_INCLUDED
                    else if (match(s, "bf_hw_version"))
                    {
                        uint32_t result = xorif_get_bf_hw_version();
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "result = 0x%08x\n", result);
                        return SUCCESS;
                    }
                    else if (match(s, "bf_hw_revision"))
                    {
                        uint32_t result = xorif_get_bf_hw_revision();
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "result = 0x%08x\n", result);
                        return SUCCESS;
                    }
                    else if (match(s, "bf_caps"))
                    {
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

                    else if (match(s, "bf_stats"))
                    {
                        // TODO
                        return SUCCESS;
                    }
                    else if (match(s, "bf_alarms"))
                    {
                        uint32_t result = xorif_get_bf_alarms();
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "result = 0x%x\n", result);
                        return SUCCESS;
                    }
#endif // BF_INCLUDED
                }
                else if (num_tokens == 3)
                {
                    if (match(s, "cc_config"))
                    {
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
                                response += sprintf(response, "iq_comp_meth_ul = %d\n", config.iq_comp_meth_ul);
                                response += sprintf(response, "iq_comp_width_ul = %d\n", config.iq_comp_width_ul);
                                response += sprintf(response, "iq_comp_meth_dl = %d\n", config.iq_comp_meth_dl);
                                response += sprintf(response, "iq_comp_width_dl = %d\n", config.iq_comp_width_dl);
                                response += sprintf(response, "bw_comp_meth = %d\n", config.bw_comp_meth);
                                response += sprintf(response, "bw_comp_width = %d\n", config.bw_comp_width);
                                response += sprintf(response, "deskew = %d\n", config.deskew);
                                response += sprintf(response, "advance_ul = %d\n", config.advance_ul);
                                response += sprintf(response, "advance_dl = %d\n", config.advance_dl);
                                response += sprintf(response, "num_ctrl_per_sym_ul = %d\n", config.num_ctrl_per_sym_ul);
                                response += sprintf(response, "num_ctrl_per_sym_dl = %d\n", config.num_ctrl_per_sym_dl);
                                return SUCCESS;
                            }
                        }
                    }
                    else if (match(s, "fhi_cc_alloc"))
                    {
                        unsigned int cc;
                        if (parse_integer(2, &cc))
                        {
                            struct xorif_cc_alloc alloc;
                            int result = xorif_get_fhi_cc_alloc(cc, &alloc);
                            if (result == XORIF_SUCCESS)
                            {
                                response += sprintf(response, "status = 0\n");
                                response += sprintf(response, "enabled = %s\n", alloc.enabled ? "true" : "false");
                                response += sprintf(response, "num_rbs = %d\n", alloc.num_rbs);
                                response += sprintf(response, "numerology = %d\n", alloc.numerology);
                                response += sprintf(response, "ul_ctrl_sym_num = %d\n", alloc.ul_ctrl_sym_num);
                                response += sprintf(response, "ul_ctrl_offset = %d\n", alloc.ul_ctrl_offset);
                                response += sprintf(response, "ul_ctrl_unroll_offset = %d\n", alloc.ul_ctrl_unroll_offset);
                                response += sprintf(response, "ul_ctrl_base_offset = %d\n", alloc.ul_ctrl_base_offset);
                                response += sprintf(response, "dl_ctrl_sym_num = %d\n", alloc.dl_ctrl_sym_num);
                                response += sprintf(response, "dl_ctrl_offset = %d\n", alloc.dl_ctrl_offset);
                                response += sprintf(response, "dl_ctrl_unroll_offset = %d\n", alloc.dl_ctrl_unroll_offset);
                                response += sprintf(response, "dl_data_sym_num = %d\n", alloc.dl_data_sym_num);
                                response += sprintf(response, "dl_data_sym_start = %d\n", alloc.dl_data_sym_start);
                                response += sprintf(response, "dl_data_buff_size = %d\n", alloc.dl_data_buff_size);
                                response += sprintf(response, "dl_data_buff_start = %d\n", alloc.dl_data_buff_start);
                                return SUCCESS;
                            }
                        }
                    }
                }
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
        if (num_tokens >= 3)
        {
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "num_rbs") && num_tokens == 4)
                {
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_num_rbs(cc, val);
                    }
                }
                else if (match(s, "numerology") && num_tokens == 5)
                {
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_numerology(cc, val1, val2);
                    }
                }
                else if (match(s, "time_advance") && num_tokens == 6)
                {
                    unsigned int cc, val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2) && parse_integer(5, &val3))
                    {
                        return xorif_set_cc_time_advance(cc, val1, val2, val3);
                    }
                }
                else if (match(s, "dl_iq_compression") && num_tokens == 5)
                {
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_dl_iq_compression(cc, val1, val2);
                    }
                }
                else if (match(s, "ul_iq_compression") && num_tokens == 5)
                {
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_ul_iq_compression(cc, val1, val2);
                    }
                }
                else if (match(s, "bw_compression") && num_tokens == 5)
                {
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_bw_compression(cc, val1, val2);
                    }
                }
                else if (match(s, "dest_mac_addr") && num_tokens == 4)
                {
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
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "fhi_alarms"))
                {
                    xorif_clear_fhi_alarms();
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
                    response += sprintf(response, "result = 0x%x\n", val);
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
                    response += sprintf(response, "result = 0x%x\n", val);
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
                sprintf(buff, "peek 0x%0x > %s", addr, tmpName);

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
            unsigned int addr;
            unsigned int val;
            if (parse_integer(1, &addr) && parse_integer(2, &val))
            {
                int result = FAILURE;
                char buff[256];

                // Construct the command string for the 'system' call
                sprintf(buff, "poke 0x%0x 0x%0x", addr, val);

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
            return xorif_configure_bf();
        }
        return MALFORMED_COMMAND;
    }
#endif
}
#endif // BF_INCLUDED

/** @} */