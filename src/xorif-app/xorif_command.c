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

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "xorif_app.h"
#include "xorif_parser.h"
#ifndef NO_HW
#include "xorif_api.h"
#ifdef BF_INCLUDED
#include "xobf_api.h"
#endif
#ifdef SRS_INCLUDED
#include "xsrs_api.h"
#endif
#ifdef EXAMPLE_INCLUDED
#include "example_api.h"
#endif
#endif

// Command prototypes...
// General commands
static int help(const char *request, char *response);
static int version(const char *request, char *response);
static int magic(const char *request, char *response);
static int terminate(const char *request, char *response);
static int quit(const char *request, char *response);
static int wait(const char *request, char *response);
static int run(const char *request, char *response);
static int peek(const char *request, char *response);
static int poke(const char *request, char *response);
static int test1(const char *request, char *response);
static int test2(const char *request, char *response);

// O-RAN-IF commands (some combination / generic commands)
static int debug(const char *request, char *response);
static int init(const char *request, char *response);
static int finish(const char *request, char *response);
static int reset(const char *request, char *response);
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
static int dump(const char *request, char *response);
static int monitor(const char *request, char *response);
static int stall(const char *request, char *response);
#ifdef EXTRA_DEBUG
static int test_fhi(const char *request, char *response);
#endif // EXTRA_DEBUG

// Beamformer commands
#ifdef BF_INCLUDED
static int schedule_bf(const char *request, char *response);
static int load(const char *request, char *response);
#ifdef EXTRA_DEBUG
static int test_bf(const char *request, char *response);
#endif // EXTRA_DEBUG
#endif // BF_INCLUDED

// SRS commands
#ifdef SRS_INCLUDED
//static int schedule_bf(const char *request, char *response);
//static int load(const char *request, char *response);
//#ifdef EXTRA_DEBUG
//static int test_bf(const char *request, char *response);
//#endif // EXTRA_DEBUG
static int load_ru_table(const char *request, char *response);
#endif // SRS_INCLUDED

// Other function prototypes...
static int dump_fhi_register(char *response, const char *reg_name);
static char * dump_reg_fhi(char *response);
#ifdef BF_INCLUDED
static int dump_bf_register(int bank, char *response, const char *reg_name);
static char * dump_reg_bf(int bank, char *response);
static int load_beam_weight_file(const char *name);
#endif // BF_INCLUDED

/**
 * @brief Command set defines the supported commands, functions and help text
 * @note
 * Instructions...
 * 1) Add commands to this data strucure
 * 2) Format of each line is {name, function, text}
 * 3) Multiple names can have same function (i.e. aliases)
 * 3) Same name can have different functions (e.g. to split processing)
 * 4) Functions can be NULL (used to provide additional help fields)
 * 5) Text can be NULL (when no help is available, or for hidden commands)
 * 6) If the first character of the text is '?' it is displayed as a
 *    'usage' string. Otherwise the whole text is displayed as is.
 */
const struct command command_set[] =
{
    {"help", help, "Provide command help and syntax"},
    {"help", NULL, "?help [<topic>]"},
    {"?", help, NULL},
    {"version", version, "Display version of the application"},
    {"version", NULL, "?version"},
    {"magic", magic, NULL},
    {"terminate", terminate, "Terminate server"},
    {"terminate", NULL, "?terminate"},
    {"exit", quit, "Exit the client script / interactive session"},
    {"exit", NULL, "?exit"},
    {"quit", quit, NULL},
    {"wait", wait, "Wait for a short time"},
    {"wait", NULL, "?wait <seconds>"},
    {"run", run, "Run a command file"},
    {"run", NULL, "?run <file>"},
    {"peek", peek, "Read from memory address"},
    {"peek", NULL, "?peek <address>"},
    {"poke", poke, "Write to memory address"},
    {"poke", NULL, "?poke <address> <value>"},
    {"test1", test1, "?test <value>"},
    {"test2", test2, "?test <value> <value>"},
    {"debug", debug, "Set debugging level"},
    {"debug", NULL, "?debug <level = 0..2>"},
    {"init", init, "Start-up device driver libraries"},
    {"init", NULL, "?init"},
    {"init", NULL, "?init fhi [(<device> | NULL)]"},
    {"finish", finish, "Close-down device driver libraries"},
    {"finish", NULL, "?finish"},
    {"finish", NULL, "?finish fhi"},
    {"has", has, "Check for the presence of a device"},
    {"has", NULL, "?has fhi"},
    {"has", NULL, "?has bf"},
    {"reset", reset, "Reset devices"},
    {"reset", NULL, "?reset"},
    {"reset", NULL, "?reset fhi [<mode = 0|1>]"},
    {"get", get, "Get various configuration and status data from a device"},
    {"get", NULL, "?get (fhi_sw_version | fhi_hw_version | fhi_hw_internal_rev)"},
    {"get", NULL, "?get (fhi_capabilities | fhi_caps)"},
    {"get", NULL, "?get fhi_cc_config <cc>"},
    {"get", NULL, "?get fhi_cc_alloc <cc>"},
    {"get", NULL, "?get fhi_stats <port>"},
    {"get", NULL, "?get (fhi_alarms | fhi_state | fhi_enabled)"},
    {"set", set, "Set various configuration data for device"},
    {"set", NULL, "?set num_rbs <cc> <number_of_rbs>"},
    {"set", NULL, "?set numerology <cc> <numerology = 0..4> <extended_cp = 0|1>"},
    {"set", NULL, "?set num_rbs_ssb <cc> <number_of_rbs = 0|20>"},
    {"set", NULL, "?set numerology_ssb <cc> <numerology = 0..4> <extended_cp = 0|1>"},
    {"set", NULL, "?set time_advance <cc> <deskew> <advance_uplink> <advance_downlink> # deprecated"},
    {"set", NULL, "?set ul_timing_params <cc> <delay_comp> <advance> <radio_ch_delay>"},
    {"set", NULL, "?set dl_timing_params <cc> <delay_comp_cp> <delay_comp_up> <advance>"},
    {"set", NULL, "?set ul_bid_forward <cc> <time>"},
    {"set", NULL, "?set ul_bid_forward_fine <cc> <symbols> <cycles>"},
    {"set", NULL, "?set ul_radio_ch_dly <cc> <delay> # deprecated"},
    {"set", NULL, "?set (dl_iq_compression | dl_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>"},
    {"set", NULL, "?set (ul_iq_compression | ul_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>"},
    {"set", NULL, "?set (ssb_iq_compression | ssb_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>"},
    {"set", NULL, "?set (prach_iq_compression | prach_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>"},
    {"set", NULL, "?set (per_ss_decompression | per_ss_decomp) <ss> <width> <method = 0..4> <enable = 0|1> [<number>]"},
    {"set", NULL, "?set dl_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>"},
    {"set", NULL, "?set ul_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>"},
    {"set", NULL, "?set ssb_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>"},
    {"set", NULL, "?set frames_per_sym <cc> <number_of_frames>"},
    {"set", NULL, "?set frames_per_sym_ssb <cc> <number_of_frames>"},
    {"set", NULL, "?set dest_mac_addr <port> <address = XX:XX:XX:XX:XX:XX>"},
    {"set", NULL, "?set src_mac_addr <port> <address = XX:XX:XX:XX:XX:XX>"},
    {"set", NULL, "?set vlan <port> <id> <dei> <pcp>"},
    {"set", NULL, "?set modu_mode <0 = disabled | 1 = enabled>"},
    {"set", NULL, "?set modu_dest_mac_addr <du> <address = XX:XX:XX:XX:XX:XX> [<id> <dei> <pcp>]"},
    {"set", NULL, "?set mtu_size <size>"},
    {"set", NULL, "?set protocol <ECPRI | 1914.3> <VLAN = 0|1> <RAW | IPv4 | IPv6> # sets packet filter"},
    {"set", NULL, "?set protocol_alt <ECPRI | 1914.3> <VLAN = 0|1> <RAW | IPv4 | IPv6> # doesn't set packet filter"},
    {"set", NULL, "?set packet_filter <port> <filter = (16 x 32b)> <mask = (4 x 16b)>"},
    {"set", NULL, "?set eaxc_id <DU_bits> <BS_bits> <CC_bits> <RU_bits>"},
    {"set", NULL, "?set ru_ports <RU_bits> <SS_bits> <mask> <user_value> <prach_value> <ssb_value> [<lte_value>]"},
    {"set", NULL, "?set ru_ports_table_mode <mode> [<sub_mode>]"},
    {"set", NULL, "?set ru_ports_table <address> <port> <type> [<number>]"},
    {"set", NULL, "?set ru_ports_table_vcc <address> <port> <type> <ccid> [<number>] # for mode 2"},
    {"set", NULL, "?set system_constants <fh_decap_dly> {...}"},
    {"configure", configure, "Program component carrier configuration"},
    {"configure", NULL, "?configure <cc>"},
    {"configure", NULL, "?configure fhi <cc>"},
    {"enable", enable, "Enable component carrier"},
    {"enable", NULL, "?enable <cc>"},
    {"enable", NULL, "?enable fhi <cc>"},
    {"disable", disable, "Disable component carrier"},
    {"disable", NULL, "?disable <cc>"},
    {"disable", NULL, "?disable fhi <cc>"},
    {"clear", clear, "Clear various status, alarms, etc."},
    {"clear", NULL, "?clear (fhi_alarms | fhi_stats | ru_ports_table)"},
    {"read_reg", read_reg, "Read device registers"},
    {"read_reg", NULL, "?read_reg fhi <name>"},
    {"read_reg_offset", read_reg_offset, "Read device registers (with offsets)"},
    {"read_reg_offset", NULL, "?read_reg_offset fhi <name> <offset>"},
    {"write_reg", write_reg, "Write device registers"},
    {"write_reg", NULL, "?write_reg fhi <name> <value>"},
    {"write_reg_offset", write_reg_offset, "Write device registers (with offsets)"},
    {"write_reg_offset", NULL, "?write_reg_offset fhi <name> <offset> <value>"},
    {"dump", dump, "Dump debug information"},
    {"dump", NULL, "?dump fhi"},
    {"monitor", monitor, "Configure / use monitor block"},
    {"monitor", NULL, "?monitor fhi clear"},
    {"monitor", NULL, "?monitor fhi select <stream>"},
    {"monitor", NULL, "?monitor fhi snapshot"},
    {"monitor", NULL, "?monitor fhi read <counter>"},
    {"stall", stall, "Use the stall detection monitor"},
    {"stall", NULL, "?stall snapshot"},
    {"stall", NULL, "?stall read"},
#ifdef EXTRA_DEBUG
    {"test_fhi", test_fhi, "?test_fhi ..."},
#endif // EXTRA_DEBUG
#ifdef BF_INCLUDED
    {"init", NULL, "?init bf [(<device> | NULL) (<device> | NULL)]"},
    {"finish", NULL, "?finish bf"},
    {"reset", NULL, "?reset bf [<mode = 0|1>]"},
    {"get", NULL, "?get (bf_sw_version | bf_hw_version | bf_hw_internal_rev)"},
    {"get", NULL, "?get (bf_capabilities | bf_caps)"},
    {"get", NULL, "?get bf_cc_config <cc>"},
    {"get", NULL, "?get bf_cc_alloc <cc>"},
    {"get", NULL, "?get bf_stats | bf_error_flags"},
    {"get", NULL, "?get (bf_alarms | bf_state | bf_enabled)"},
    {"get", NULL, "?get (latency_dl | latency_dl_dfe | latency_ul)"},
    {"get", NULL, "?get ul_data_arrival_offset <dfe>"},
    {"set", NULL, "?set (bw_compression | bw_comp) <cc> <width> <method = 0..4>"},
    {"set", NULL, "?set bf_fft_shift_mode <mode = 0..3>"},
    {"set", NULL, "?set bf_scheduler_mode <mode = 0..2>"},
    {"configure", NULL, "?configure bf <cc>"},
    {"enable", NULL, "?enable bf <cc>"},
    {"disable", NULL, "?disable bf <cc>"},
    {"schedule_bf", schedule_bf, "Program the beamformer schedule table"},
    {"schedule_bf", NULL, "?schedule_bf [mask]"},
    {"schedule_bf", NULL, "?schedule_bf {(<CxAx> | <Cx>)} # e.g. C0A0 C0A1 or C0 C1"},
    {"clear", NULL, "?clear (bf_alarms | bf_stats)"},
    {"read_reg", NULL, "?read_reg (bf | bf0 | bf1) <name>"},
    {"read_reg_offset", NULL, "?read_reg_offset (bf | bf0 | bf1) <name> <offset>"},
    {"write_reg", NULL, "?write_reg (bf | bf0 | bf1) <name> <value>"},
    {"write_reg_offset", NULL, "?write_reg_offset (bf | bf0 | bf1) <name> <offset> <value>"},
    {"load", load, "Load data from file (e.g. M-Plane beam-weights)"},
    {"load", NULL, "?load beam_weights <file>"},
    {"dump", NULL, "?dump (bf | bf0 | bf1)"},
    {"dump", NULL, "?dump (bf | bf0 | bf1) errors"},
#ifdef EXTRA_DEBUG
    {"test_bf", test_bf, "?test_bf ..."},
#endif // EXTRA_DEBUG
#endif // BF_INCLUDED
#ifdef SRS_INCLUDED
    {"init", NULL, "?init srs [(<device> | NULL)]"},
    {"finish", NULL, "?finish srs"},
    {"reset", NULL, "?reset srs [<mode = 0|1>]"},
    {"get", NULL, "?get srs_hw_version"},
    {"get", NULL, "?get srs_cc_config <cc>"},
    {"get", NULL, "?get srs_eaxc"},
    {"set", NULL, "?set (srs_iq_compression | srs_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>"},
    {"set", NULL, "?set ruid_table_srs <ru_port_id> <antenna_id>"},
    {"configure", NULL, "?configure srs <cc>"},
    {"load_ru_table", load_ru_table, "Program the SRS RU Port ID Table"},
    {"load_ru_table", NULL, "?load_ru_table srs"},
    {"read_reg", NULL, "?read_reg srs <name>"},
    {"write_reg", NULL, "?write_reg srs <name> <value>"},
#endif // SRS_INCLUDED
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
#ifdef SRS_INCLUDED
/**
 * @brief Has SRS device been detected.
 * @returns
 *      - 0 if no
 *      - 1 if yes
 */
static int has_srs(void)
{
    static int has_srs_flag = -1;

    if (has_srs_flag == -1)
    {
        has_srs_flag = no_srs ? 0 : xsrs_has_srs();
    }
    return has_srs_flag;
}
#endif // SRS_INCLUDED
#endif // NO_HW

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
        response += sprintf(response, "Available commands...\n");
        const struct command *ptr = &command_set[0];
        while (ptr->name)
        {
            if (ptr->help)
            {
                if (ptr->help[0] != '?')
                {
                    // Display whole text
                    response += sprintf(response, "\t%-20s : %s\n", ptr->name, ptr->help);
                }
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
            int count = 0;
            while (ptr->name)
            {
                if (match(s, ptr->name))
                {
                    if (ptr->help)
                    {
                        ++count;
                         if (ptr->help[0] != '?')
                        {
                            // Display whole text
                            response += sprintf(response, "%s : %s\n",  ptr->name, ptr->help);
                        }
                        else
                        {
                            // Display usage string
                            response += sprintf(response, "\tusage: %s\n", &ptr->help[1]);
                        }
                    }
                }
                ++ptr;
            }

            if (count > 0)
            {
                return SUCCESS;
            }
        }
    }
    return UNKNOWN_COMMAND;
}

/**
 * @brief "version" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int version(const char *request, char *response)
{
    if (num_tokens == 1)
    {
        response += sprintf(response, "Version %s\n", app_version_str);
        return SUCCESS;
    }
    return UNKNOWN_COMMAND;
}

/**
 * @brief "magic" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int magic(const char *request, char *response)
{
    if (num_tokens == 1)
    {
        response += sprintf(response, "Today's magic number is 42\n");
        return SUCCESS;
    }
    return UNKNOWN_COMMAND;
}

/**
 * @brief "terminate" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int terminate(const char *request, char *response)
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
            // terminate
            return TERMINATE;
        }
    }
    return UNKNOWN_COMMAND;
#endif
}

/**
 * @brief "exit" command.
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
        // exit
        return TERMINATE;
    }
    return UNKNOWN_COMMAND;
}

/**
 * @brief "wait" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int wait(const char *request, char *response)
{
    if (num_tokens == 2)
    {
        // wait <seconds>
        unsigned int val;
        if (parse_integer(1, &val))
        {
            sleep(val);
            return SUCCESS;
        }
    }
    return UNKNOWN_COMMAND;
}

/**
 * @brief "run" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int run(const char *request, char *response)
{
    if (num_tokens == 2)
    {
        // run <file>
        const char *s;
        if (parse_string(1, &s))
        {
            // Using "do_file()" will over-write the request string
            // So we make a temporary copy of the file-name
            char *temp = malloc(strlen(s) + 1);
            if (temp)
            {
                strcpy(temp, s);
            }

            // Run "do_file()" using the copied file-name
            int result = do_file(temp);
            sprintf(response, "status = %d\n", result);

            // Fix the original request string with the saved file-name
            if (temp)
            {
                sprintf((char*)request, "run %s", temp);
                free(temp);
            }
            return result;
        }
    }
    return UNKNOWN_COMMAND;
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
    }
    return UNKNOWN_COMMAND;
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
    }
    return UNKNOWN_COMMAND;
#endif
}

/**
 * @brief "test" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int test1(const char *request, char *response)
{
    // Example of a command handled by client
    if (num_tokens == 2)
    {
        // test1 <value>
        // Add code to parse parameters, and do functionality
        return SUCCESS;
    }
    return UNKNOWN_COMMAND;
}

/**
 * @brief "test" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int test2(const char *request, char *response)
{
    // Example of a command handled by server
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
            // test2 <value> <value>
#ifdef EXAMPLE_INCLUDED
            unsigned int x, y;
            if (parse_integer(1, &x) && parse_integer(2, &y))
            {
                return example_do_something_interesting(x, y);
            }
#endif
        }
    }
    return UNKNOWN_COMMAND;
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
#ifdef SRS_INCLUDED
                xsrs_debug(level);
#endif // SRS_INCLUDED
#ifdef EXAMPLE_INCLUDED
                example_debug(level);
#endif
                return SUCCESS;
            }
        }
    }
    return UNKNOWN_COMMAND;
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
                    result &= xobf_init(NULL);
                }
            }
#endif
#ifdef SRS_INCLUDED
            if (result == XORIF_SUCCESS)
            {
                if (has_srs())
                {
                    result &= xsrs_init(NULL);
                }
            }
#endif //SRS_INCLUDED
            return result;
        }
        else if (num_tokens == 2)
        {
            // init (fhi | bf)
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
                    return xobf_init(NULL);
                }
#endif
#ifdef SRS_INCLUDED
                else if (match(s, "srs"))
                {
                    return xsrs_init(NULL);
                }
#endif // SRS_INCLUDED
            }
        }
        else if (num_tokens == 3)
        {
            // init fhi <device_name>
            const char *s1;
            const char *s2;
            if (parse_string(1, &s1) && parse_string(2, &s2))
            {
                if (match(s1, "fhi"))
                {
                    if (match(s2, "NULL"))
                    {
                        s2 = NULL;
                    }
                    return xorif_init(s2);
                }
#ifdef EXAMPLE_INCLUDED
                if (match(s1, "example"))
                {
                    if (match(s2, "NULL"))
                    {
                        s2 = NULL;
                    }
                    return example_init(s2);
                }
#endif
#if BF_INCLUDED
                else if (match(s1, "bf"))
                {
                    if (match(s2, "NULL"))
                    {
                        s2 = NULL;
                    }
                    return xobf_init(s2);
                }
#endif
#ifdef SRS_INCLUDED
                if (match(s1, "srs"))
                {
                    if (match(s2, "NULL"))
                    {
                        s2 = NULL;
                    }
                    return xsrs_init(s2);
                }
#endif // SRS_INCLUDED

            }
        }
    }
    return UNKNOWN_COMMAND;
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
#ifdef SRS_INCLUDED
            if (has_srs())
            {
                xsrs_finish();
            }
#endif // SRS_INCLUDED
            return SUCCESS;
        }
        else if (num_tokens == 2)
        {
            // finish (fhi | bf)
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
#ifdef SRS_INCLUDED
                else if (match(s, "srs"))
                {
                    xsrs_finish();
                    return SUCCESS;
                }
#endif // SRS_INCLUDED
#ifdef EXAMPLE_INCLUDED
                else if (match(s, "example"))
                {
                    example_finish();
                    return SUCCESS;
                }
#endif
            }
        }
    }
    return UNKNOWN_COMMAND;
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
            // reset (fhi | bf)
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
            // reset (fhi | bf) <mode>
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
#ifdef SRS_INCLUDED
                else if (match(s, "srs"))
                {
                    return xsrs_reset_srs(mode);
                }
#endif // SRS_INCLUDED
            }
        }
    }
    return UNKNOWN_COMMAND;
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
            // has (fhi | bf)
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
                else if (match(s, "srs"))
                {
                    int result;
#ifdef SRS_INCLUDED
                    result = no_srs ? 0 : xsrs_has_srs();
#else
                    result = 0;
#endif // SRS_INCLUDED
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = %s\n", result ? "true" : "false");
                    return SUCCESS;
                }
            }
        }
    }
    return UNKNOWN_COMMAND;
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
                    response += sprintf(response, "result = %d\n", xorif_get_fhi_hw_internal_rev());
                    return SUCCESS;
                }
                else if ((match(s, "fhi_capabilities") || match(s, "fhi_caps")) && num_tokens == 2)
                {
                    // get (fhi_capabilities | fhi_caps)
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
                    response += sprintf(response, "extra_flags = 0x%X\n", ptr->extra_flags);
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
                            response += sprintf(response, "iq_comp_meth_prach = %d\n", config.iq_comp_meth_prach);
                            response += sprintf(response, "iq_comp_width_prach = %d\n", config.iq_comp_width_prach);
                            response += sprintf(response, "iq_comp_mplane_prach = %d\n", config.iq_comp_mplane_prach);
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
                    response += sprintf(response, "result = %d\n", xobf_get_bf_hw_internal_rev());
                    return SUCCESS;
                }
                else if ((match(s, "bf_capabilities") || match(s, "bf_caps")) && num_tokens == 2)
                {
                    // get (bf_capabilities | bf_caps)
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
                else if (match(s, "bf_error_flags") && num_tokens == 2)
                {
                    // get bf_error_flags
                    struct xobf_bf_error_flags flags;
                    int result = xobf_get_bf_error_flags(&flags);
                    if (result == XORIF_SUCCESS)
                    {
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "cache_dl_miss_flags = 0x%08X\n", flags.cache_dl_miss_flags);
                        response += sprintf(response, "cplane_dl_no_desc_flags = 0x%08X\n", flags.cplane_dl_no_desc_flags);
                        response += sprintf(response, "bf_dl_re_mutex_flags = 0x%08X\n", flags.bf_dl_re_mutex_flags);
                        response += sprintf(response, "aie_dl_flags = 0x%08X\n", flags.aie_dl_flags);
                        response += sprintf(response, "bwm_dl_next_bidn_uf_flags = 0x%08X\n", flags.bwm_dl_next_bidn_uf_flags);
                        response += sprintf(response, "cache_ul_miss_flags = 0x%08X\n", flags.cache_ul_miss_flags);
                        response += sprintf(response, "cplane_ul_no_desc_flags = 0x%08X\n", flags.cplane_ul_no_desc_flags);
                        response += sprintf(response, "bf_ul_re_mutex_flags = 0x%08X\n", flags.bf_ul_re_mutex_flags);
                        response += sprintf(response, "aie_ul_flags = 0x%08X\n", flags.aie_ul_flags);
                        response += sprintf(response, "dfe_ul_config_flags = 0x%08X\n", flags.dfe_ul_config_flags);
                        response += sprintf(response, "bwm_ul_next_bidn_uf_flags = 0x%08X\n", flags.bwm_ul_next_bidn_uf_flags);
                        response += sprintf(response, "prach_flags = 0x%08X\n", flags.prach_flags);
#ifdef MIXED_SSB_SUPPORT
                        response += sprintf(response, "ssb_flags = 0x%08X\n", flags.ssb_flags);
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
                else if (match(s, "latency_dl") && num_tokens == 2)
                {
                    // get latency_dl
                    unsigned int result = xobf_get_bf_latency_dl();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = %d\n", result);
                    return SUCCESS;
                }
                else if (match(s, "latency_dl_dfe") && num_tokens == 2)
                {
                    // get latency_dl_dfe
                    unsigned int result = xobf_get_bf_latency_dl_dfe();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = %d\n", result);
                    return SUCCESS;
                }
                else if (match(s, "latency_ul") && num_tokens == 2)
                {
                    // get latency_ul
                    unsigned int result = xobf_get_bf_latency_ul();
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = %d\n", result);
                    return SUCCESS;
                }
                else if (match(s, "ul_data_arrival_offset") && num_tokens == 3)
                {
                    unsigned int dfe;
                    if (parse_integer(2, &dfe))
                    {
                        // get ul_data_arrival_offset <dfe>
                        int16_t offset = 0;
                        uint16_t symbol = 0;
                        int result = xobf_get_bf_ul_data_arrival_offset(dfe, &offset, &symbol);
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "result = %d\n", result);
                        response += sprintf(response, "offset = %d\n", offset);
                        response += sprintf(response, "symbol = %d\n", symbol);
                        return SUCCESS;
                    }
                }
#endif // BF_INCLUDED
#ifdef SRS_INCLUDED
                else if (match(s, "srs_hw_version") && num_tokens == 2)
                {
                    // get bf_hw_version
                    response += sprintf(response, "status = 0\n");
                    response += sprintf(response, "result = 0x%08X\n", xsrs_get_srs_hw_version());
                    return SUCCESS;
                }
                else if (match(s, "srs_cc_config") && num_tokens == 3)
                {
                    unsigned int cc;
                    if (parse_integer(2, &cc))
                    {
                        struct xsrs_cc_config srs_cc_config;
                        int result = xsrs_get_cc_config(cc, &srs_cc_config);
                        if (result == XSRS_SUCCESS)
                        {
                            response += sprintf(response, "status = 0\n");
                            response += sprintf(response, "num_rbs        = %d\n", srs_cc_config.num_rbs);
                            response += sprintf(response, "numerology     = %d\n", srs_cc_config.numerology);
                            response += sprintf(response, "iq_comp_meth   = %d\n", srs_cc_config.iq_comp_meth);
                            response += sprintf(response, "iq_comp_width  = %d\n", srs_cc_config.iq_comp_width);
                            response += sprintf(response, "iq_comp_mplane = %d\n", srs_cc_config.iq_comp_mplane);
                            return SUCCESS;
                        }
                    }
                }
                else if (match(s, "srs_eaxc") && num_tokens == 2)
                {
                    struct xsrs_antenna_carrier_config srs_eaxc_config;
                    int display_counter = 0;
                    int result = xsrs_get_antenna_carrier_config(&srs_eaxc_config);
                    if (result == XSRS_SUCCESS)
                    {
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "du_width = 0x%08X\n", srs_eaxc_config.du_port_id_width);
                        response += sprintf(response, "bs_width = 0x%08X\n", srs_eaxc_config.bs_port_id_width);
                        response += sprintf(response, "cc_width = 0x%08X\n", srs_eaxc_config.cc_port_id_width);
                        response += sprintf(response, "ru_width = 0x%08X\n", srs_eaxc_config.ru_port_id_width);
                        response += sprintf(response, "RU PORT ID TABLE:\n");
                        for (int i = 0; i < 256; i++)
                        {
                            response += sprintf(response, "0x%02X ", srs_eaxc_config.ru_port_antenna[i]);
                            display_counter++;
                            if (display_counter == 8)
                            {
                                display_counter = 0;
                                response += sprintf(response, "\n");
                            }
                        }
                        response += sprintf(response, "\n");
                    }
                    return SUCCESS;
                }
#endif // SRS_INCLUDED
            }
        }
    }
    return UNKNOWN_COMMAND;
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
                    // set num_rbs <cc> <number_of_rbs>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        int result = xorif_set_cc_num_rbs(cc, val);
#ifdef BF_INCLUDED
                        if (result == XORIF_SUCCESS)
                        {
                            if (has_bf())
                            {
                                result &= xobf_set_cc_num_rbs(cc, val);
                            }
                        }
#endif
#ifdef SRS_INCLUDED
                        if (result == XORIF_SUCCESS)
                        {
                            if (has_srs())
                            {
                                result &= xsrs_set_cc_num_rbs(cc, val);
                            }
                        }
#endif // SRS_INCLUDED
                        return result;
                    }
                }
                if (match(s, "num_rbs_ssb") && num_tokens == 4)
                {
                    // set num_rbs_ssb <cc> <number_of_rbs = 0|20>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_num_rbs_ssb(cc, val);
                    }
                }
                else if (match(s, "numerology") && num_tokens == 5)
                {
                    // set numerology <cc> <numerology = 0..4> <extended_cp = 0|1>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        int result = xorif_set_cc_numerology(cc, val1, val2);
#ifdef BF_INCLUDED
                        if (result == XORIF_SUCCESS)
                        {
                            if (has_bf())
                            {
                                result &= xobf_set_cc_numerology(cc, val1, val2);
                            }
                        }
#endif
#ifdef SRS_INCLUDED
                        if (result == XORIF_SUCCESS)
                        {
                            if (has_srs())
                            {
                                result &= xsrs_set_cc_numerology(cc, val1, val2);
                            }
                        }
#endif // SRS_INCLUDED
                        return result;
                    }
                }
                else if (match(s, "numerology_ssb") && num_tokens == 5)
                {
                    // set numerology_ssb <cc> <numerology = 0..4> <extended_cp = 0|1>
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
                    // set time_advance <cc> <deskew> <advance_uplink> <advance_downlink> # deprecated
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
                    // set ul_radio_ch_dly <cc> <delay> # deprecated
                    unsigned int cc;
                    double val;
                    if (parse_integer(2, &cc) && parse_double(3, &val))
                    {
                        return xorif_set_ul_radio_ch_dly(cc, val);
                    }
                }
                else if ((match(s, "dl_iq_compression") || match(s, "dl_iq_comp")) && num_tokens == 5)
                {
                    // set (dl_iq_compression | dl_iq_comp) <cc> <width> <method = 0..4>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_dl_iq_compression(cc, val1, val2, 1);
                    }
                }
                else if ((match(s, "dl_iq_compression") || match(s, "dl_iq_comp")) && num_tokens == 6)
                {
                    // set (dl_iq_compression | dl_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>
                    unsigned int cc, val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2) && parse_integer(5, &val3))
                    {
                        return xorif_set_cc_dl_iq_compression(cc, val1, val2, val3);
                    }
                }
                else if ((match(s, "ul_iq_compression") || match(s, "ul_iq_comp")) && num_tokens == 5)
                {
                    // set (ul_iq_compression | ul_iq_comp) <cc> <width> <method = 0..4>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_ul_iq_compression(cc, val1, val2, 1);
                    }
                }
                else if ((match(s, "ul_iq_compression") || match(s, "ul_iq_comp")) && num_tokens == 6)
                {
                    // set (ul_iq_compression | ul_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>
                    unsigned int cc, val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2) && parse_integer(5, &val3))
                    {
                        return xorif_set_cc_ul_iq_compression(cc, val1, val2, val3);
                    }
                }
                else if ((match(s, "ssb_iq_compression") || match(s, "ssb_iq_comp")) && num_tokens == 5)
                {
                    // set (ssb_iq_compression | ssb_iq_comp) <cc> <width> <method = 0..4>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_iq_compression_ssb(cc, val1, val2, 1);
                    }
                }
                else if ((match(s, "ssb_iq_compression") || match(s, "ssb_iq_comp")) && num_tokens == 6)
                {
                    // set (ssb_iq_compression | ssb_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>
                    unsigned int cc, val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2) && parse_integer(5, &val3))
                    {
                        return xorif_set_cc_iq_compression_ssb(cc, val1, val2, val3);
                    }
                }
                else if ((match(s, "prach_iq_compression") || match(s, "prach_iq_comp")) && num_tokens == 5)
                {
                    // set (prach_iq_compression | prach_iq_comp) <cc> <width> <method = 0..4>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        int result = xorif_set_cc_iq_compression_prach(cc, val1, val2, 1);
#ifdef BF_INCLUDED
                        if (result == XORIF_SUCCESS)
                        {
                            result = xobf_set_cc_iq_compression_prach(cc, val1, val2, 1);
                        }
#endif
                        return result;
                    }
                }
                else if ((match(s, "prach_iq_compression") || match(s, "prach_iq_comp")) && num_tokens == 6)
                {
                    // set (prach_iq_compression | prach_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>
                    unsigned int cc, val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2) && parse_integer(5, &val3))
                    {
                        int result = xorif_set_cc_iq_compression_prach(cc, val1, val2, val3);
#ifdef BF_INCLUDED
                        if (result == XORIF_SUCCESS)
                        {
                            result = xobf_set_cc_iq_compression_prach(cc, val1, val2, val3);
                        }
#endif
                        return result;
                    }
                }
#ifdef SRS_INCLUDED
                else if ((match(s, "srs_iq_compression") || match(s, "srs_iq_comp")) && num_tokens == 5)
                {
                    // set (srs_iq_compression | srs_iq_comp) <cc> <width> <method = 0..4>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xsrs_set_cc_comp(cc, val1, val2, 1);
                    }
                }
                else if ((match(s, "srs_iq_compression") || match(s, "srs_iq_comp")) && num_tokens == 6)
                {
                    // set (dl_iq_compression | dl_iq_comp) <cc> <width> <method = 0..4> <mplane = 0|1>
                    unsigned int cc, val1, val2, val3;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2) && parse_integer(5, &val3))
                    {
                        return xsrs_set_cc_comp(cc, val1, val2, val3);
                    }
                }
#endif // SRS_INCLUDED
                else if ((match(s, "per_ss_decompression") || match(s, "per_ss_decomp")) && num_tokens == 6)
                {
                    // set (per_ss_decompression | per_ss_decomp) <ss> <width> <method = 0..4> <enable = 0|1>
                    unsigned int ss, width, method, enable;
                    if (parse_integer(2, &ss) && parse_integer(3, &width) && parse_integer(4, &method) &&
                        parse_integer(5, &enable))
                    {
                        return xorif_set_cc_dl_iq_compression_per_ss(ss, width, method, enable, 1);
                    }
                }
                else if ((match(s, "per_ss_decompression") || match(s, "per_ss_decomp")) && num_tokens == 7)
                {
                    // set (per_ss_decompression | per_ss_decomp) <ss> <width> <method = 0..4> <enable = 0|1> [<number>]
                    unsigned int ss, width, method, enable, number;
                    if (parse_integer(2, &ss) && parse_integer(3, &width) && parse_integer(4, &method) &&
                        parse_integer(5, &enable) && parse_integer(6, &number))
                    {
                        return xorif_set_cc_dl_iq_compression_per_ss(ss, width, method, enable, number);
                    }
                }
#ifdef BF_INCLUDED
                else if ((match(s, "bw_compression") || match(s, "bw_comp")) && num_tokens == 5)
                {
                    // set (bw_compression | bw_comp) <cc> <width> <method = 0..4>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xobf_set_cc_bw_compression(cc, val1, val2);
                    }
                }
#endif
                else if (match(s, "dl_sections_per_sym") && num_tokens == 4)
                {
                    // set dl_sections_per_sym <cc> <number_of_sections>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_dl_sections_per_symbol(cc, val, val);
                    }
                }
                else if (match(s, "dl_sections_per_sym") && num_tokens == 5)
                {
                    // set dl_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_dl_sections_per_symbol(cc, val1, val2);
                    }
                }
                else if (match(s, "ul_sections_per_sym") && num_tokens == 4)
                {
                    // set ul_sections_per_sym <cc> <number_of_sections>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_ul_sections_per_symbol(cc, val, val);
                    }
                }
                else if (match(s, "ul_sections_per_sym") && num_tokens == 5)
                {
                    // set ul_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_ul_sections_per_symbol(cc, val1, val2);
                    }
                }
                else if (match(s, "ssb_sections_per_sym") && num_tokens == 4)
                {
                    // set ssb_sections_per_sym <cc> <number_of_sections>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_sections_per_symbol_ssb(cc, val, val);
                    }
                }
                else if (match(s, "ssb_sections_per_sym") && num_tokens == 5)
                {
                    // set ssb_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>
                    unsigned int cc, val1, val2;
                    if (parse_integer(2, &cc) && parse_integer(3, &val1) && parse_integer(4, &val2))
                    {
                        return xorif_set_cc_sections_per_symbol_ssb(cc, val1, val2);
                    }
                }
                else if (match(s, "frames_per_sym") && num_tokens == 4)
                {
                    // set frames_per_sym <cc> <number_of_frames>
                    unsigned int cc, val;
                    if (parse_integer(2, &cc) && parse_integer(3, &val))
                    {
                        return xorif_set_cc_frames_per_symbol(cc, val);
                    }
                }
                else if (match(s, "frames_per_sym_ssb") && num_tokens == 4)
                {
                    // set frames_per_sym_ssb <cc> <number_of_frames>
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
                else if (match(s, "modu_mode") && num_tokens == 3)
                {
                    // set modu_mode <0 = disabled | 1 = enabled>
                    unsigned int enable;
                    if (parse_integer(2, &enable))
                    {
                        return xorif_set_modu_mode(enable);
                    }
                }
                else if (match(s, "modu_dest_mac_addr") && num_tokens == 4)
                {
                    // set modu_dest_mac_addr <du> <address = XX:XX:XX:XX:XX:XX>
                    unsigned int du;
                    const char *s;
                    if (parse_integer(2, &du) && parse_string(3, &s))
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
                            return xorif_set_modu_dest_mac_addr(du, address, 0, 0, 0);
                        }
                    }
                }
                else if (match(s, "modu_dest_mac_addr") && num_tokens == 7)
                {
                    // set modu_dest_mac_addr <du> <address = XX:XX:XX:XX:XX:XX> [<id> <dei> <pcp>]
                    unsigned int du;
                    const char *s;
                    unsigned int id;
                    unsigned int dei;
                    unsigned int pcp;
                    if (parse_integer(2, &du) && parse_string(3, &s) &&
                        parse_integer(4, &id) && parse_integer(5, &dei) &&
                        parse_integer(6, &pcp))
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
                            return xorif_set_modu_dest_mac_addr(du, address, id, dei, pcp);
                        }
                    }
                }
                else if (match(s, "mtu_size") && num_tokens == 3)
                {
                    // set mtu_size <size>
                    unsigned int size;
                    if (parse_integer(2, &size))
                    {
                        return xorif_set_mtu_size(size);
                    }
                }
                else if (match(s, "protocol") && num_tokens == 5)
                {
                    // set protocol <ECPRI | 1914.3> <VLAN = 0|1> <RAW | IPv4 | IPv6>
                    // Note: sets protocol and default packet filter
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
                else if (match(s, "protocol_alt") && num_tokens == 5)
                {
                    // set protocol <ECPRI | 1914.3> <VLAN = 0|1> <RAW | IPv4 | IPv6>
                    // Note: sets protocol only (not the packet filter)
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
                            return xorif_set_fhi_protocol_alt(transport, vlan, ip_mode);
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
                    // set eaxc_id <DU bits> <BS bits> <CC bits> <RU bits>
                    unsigned int du_bits;
                    unsigned int bs_bits;
                    unsigned int cc_bits;
                    unsigned int ru_bits;
                    if (parse_integer(2, &du_bits) && parse_integer(3, &bs_bits) &&
                        parse_integer(4, &cc_bits) && parse_integer(5, &ru_bits))
                    {
#ifdef SRS_INCLUDED
                        return xorif_set_fhi_eaxc_id(du_bits, bs_bits, cc_bits, ru_bits) &
                               xsrs_set_eaxc_widths(du_bits, bs_bits, cc_bits, ru_bits);
#else
                        return xorif_set_fhi_eaxc_id(du_bits, bs_bits, cc_bits, ru_bits);
#endif
                    }
                }
                else if (match(s, "ru_ports") && num_tokens == 8)
                {
                    // set ru_ports <RU bits> <ss bits> <mask> <user_value> <prach_value> <ssb_value>
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
                    // set ru_ports <RU bits> <ss bits> <mask> <user_value> <prach_value> <ssb_value> [<lte_value>]
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
                        return xorif_set_ru_ports_lte(ru_bits, ss_bits, mask, user_val, prach_val, ssb_val, lte_val);
                    }
                }
                else if (match(s, "ru_ports_table_mode") && num_tokens == 3)
                {
                    // set ru_ports_table_mode <mode>
                    unsigned int mode;
                    if (parse_integer(2, &mode))
                    {
                        return xorif_set_ru_ports_table_mode(mode, 0);
                    }
                }
                else if (match(s, "ru_ports_table_mode") && num_tokens == 4)
                {
                    // set ru_ports_table_mode <mode> <sub_mode>
                    unsigned int mode;
                    unsigned int sub_mode;
                    if (parse_integer(2, &mode) && parse_integer(3, &sub_mode))
                    {
                        return xorif_set_ru_ports_table_mode(mode, sub_mode);
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
                else if (match(s, "ru_ports_table_vcc") && num_tokens == 6)
                {
                    // set ru_ports_table_vcc <address> <port> <type> <ccid>
                    unsigned int address;
                    unsigned int port;
                    unsigned int type;
                    unsigned int ccid;
                    if (parse_integer(2, &address) && parse_integer(3, &port) &&
                        parse_integer(4, &type) && parse_integer(5, &ccid))
                    {
                        return xorif_set_ru_ports_table_vcc(address, port, type, ccid, 1);
                    }
                }
                else if (match(s, "ru_ports_table_vcc") && num_tokens == 7)
                {
                    // set ru_ports_table_vcc <address> <port> <type> <ccid> [<number>]
                    unsigned int address;
                    unsigned int port;
                    unsigned int type;
                    unsigned int ccid;
                    unsigned int number;
                    if (parse_integer(2, &address) && parse_integer(3, &port) &&
                        parse_integer(4, &type) && parse_integer(5, &ccid) &&
                        parse_integer(6, &number))
                    {
                        return xorif_set_ru_ports_table_vcc(address, port, type, ccid, number);
                    }
                }
                else if (match(s, "system_constants") && num_tokens >= 3)
                {
                    // set system_constants <fh_decap_dly> {...}
                    double val1;
                    if (parse_double(2, &val1))
                    {
                        struct xorif_system_constants sys_const;
                        sys_const.FH_DECAP_DLY = val1;
                        // TODO Add more as required
                        return xorif_set_system_constants(&sys_const);
                    }
                }
                else if (match(s, "packet_filter") && num_tokens == 23)
                {
                    // set packet_filter <port> <filter = (16 x 32b)> <mask = (4 x 16b)>
                    unsigned int port;
                    unsigned int val;
                    uint32_t filter[16];
                    uint16_t mask[4];

                    if (parse_integer(2, &port))
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            if (parse_integer(3 + i, &val))
                            {
                                filter[i] = val;
                            }
                            else
                            {
                                return MALFORMED_COMMAND;
                            }
                        }
                        for (int i = 0; i < 4; ++i)
                        {
                            if (parse_integer(19 + i, &val))
                            {
                                mask[i] = val;
                            }
                            else
                            {
                                return MALFORMED_COMMAND;
                            }
                        }
                        return xorif_set_fhi_packet_filter(port, filter, mask);
                    }
                }
#ifdef BF_INCLUDED
                else if ((match(s, "bf_fft_shift_mode")) && num_tokens == 3)
                {
                    // set bf_fft_shift_mode <mode = 0..3>
                    unsigned int mode;

                    if (parse_integer(2, &mode))
                    {
                        return xobf_set_bf_fft_shift_mode(mode);
                    }
                }
                else if ((match(s, "bf_scheduler_mode")) && num_tokens == 3)
                {
                    // set bf_scheduler_mode <mode = 0..2>
                    unsigned int mode;

                    if (parse_integer(2, &mode))
                    {
                        return xobf_set_bf_scheduling_mode(mode);
                    }
                }
#endif
#ifdef SRS_INCLUDED
                else if ((match(s, "ruid_table_srs")) && num_tokens == 4)
                {
                    // set ruid_table_srs <ru_port_id> <ant_id>
                    unsigned int ru_port;
                    unsigned int ant_id;

                    if (parse_integer(2, &ru_port) && parse_integer(3, &ant_id))
                    {
                        return xsrs_set_ru_port_id_table(ru_port, ant_id);
                    }
                }
#endif // SRS_INCLUDED
            }
        }
    }
    return UNKNOWN_COMMAND;
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
            // clear (fhi_alarms | fhi_stats | bf_alarms | bf_stats | ru_ports_table)
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
    }
    return UNKNOWN_COMMAND;
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
                        result &= xobf_configure_cc(cc);
                    }
                }
#endif
#ifdef SRS_INCLUDED
                if (result == XORIF_SUCCESS)
                {
                    if (has_srs())
                    {
                        result &= xsrs_configure_cc(cc);
                    }
                }
#endif // SRS_INCLUDED
                return result;
            }
        }
        else if (num_tokens == 3)
        {
            // configure (fhi | bf) <cc>
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
#ifdef SRS_INCLUDED
                else if (match(s, "srs"))
                {
                    return xsrs_configure_cc(cc);
                }
#endif // SRS_INCLUDED
            }
        }
    }
    return UNKNOWN_COMMAND;
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
            // enable (fhi | bf) <cc>
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
    }
    return UNKNOWN_COMMAND;
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
            // disable (fhi | bf) <cc>
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
    }
    return UNKNOWN_COMMAND;
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
            // read_reg (fhi | bf | bf0 | bf1) <name>
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
#ifdef SRS_INCLUDED
                else if (match(s1, "srs"))
                {
                    result = xsrs_read_srs_reg(s2, &val);
                }
#endif // SRS_INCLUDED
                else
                {
                    return UNKNOWN_COMMAND;
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
    }
    return UNKNOWN_COMMAND;
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
            // read_reg_offset (fhi | bf | bf0 | bf1) <name> <offset>
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
                    return UNKNOWN_COMMAND;
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
    }
    return UNKNOWN_COMMAND;
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
            // write_reg (fhi | bf | bf0 | bf1) <name> <value>
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
#ifdef SRS_INCLUDED
                else if (match(s1, "srs"))
                {
                    result = xsrs_write_srs_reg(s2, val);
                }
#endif // SRS_INCLUDED
                else
                {
                    return UNKNOWN_COMMAND;
                }

                response += sprintf(response, "status = %d\n", result);
                return result;
            }
        }
    }
    return UNKNOWN_COMMAND;
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
            // write_reg_offset (fhi | bf | bf0 | bf1) <name> <offset> <value>
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
                    return UNKNOWN_COMMAND;
                }

                response += sprintf(response, "status = %d\n", result);
                return result;
            }
        }
    }
    return UNKNOWN_COMMAND;
#endif
}

/**
 * @brief "dump" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int dump(const char *request, char *response)
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
            // dump (fhi | bf | bf0 | bf1)
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
#ifdef BF_INCLUDED
        else if (num_tokens == 3)
        {
            // dump bf errors
            const char *s1;
            const char *s2;
            if (parse_string(1, &s1) && parse_string(2, &s2))
            {
                if (match(s1, "bf") && match(s2, "errors"))
                {
                    uint32_t alarms = xobf_get_bf_alarms();
                    struct xobf_bf_error_flags flags;
                    int result = xobf_get_bf_error_flags(&flags);
                    if (result == XORIF_SUCCESS)
                    {
                        response += sprintf(response, "status = 0\n");

                        response += sprintf(response, "BF Alarm Status\n");
                        response += sprintf(response, "\tCACHE_DL_MISS       = %d\n", (alarms) & 1);
                        response += sprintf(response, "\tCPLANE_DL_NO_DESC   = %d\n", (alarms >> 1) & 1);
                        response += sprintf(response, "\tAIE_DL              = %d\n", (alarms >> 3) & 1);
                        response += sprintf(response, "\tBWM_DL_NEXT_BIDN_UF = %d\n", (alarms >> 5) & 1);
                        response += sprintf(response, "\tCACHE_UL_MISS       = %d\n", (alarms >> 12) & 1);
                        response += sprintf(response, "\tCPLANE_UL_NO_DESC   = %d\n", (alarms >> 13) & 1);
                        response += sprintf(response, "\tAIE_UL              = %d\n", (alarms >> 15) & 1);
                        response += sprintf(response, "\tDFE_UL_CONFIG       = %d\n", (alarms >> 16) & 1);
                        response += sprintf(response, "\tBWM_UL_NEXT_BIDN_UF = %d\n", (alarms >> 17) & 1);
                        response += sprintf(response, "\tPRACH               = %d\n", (alarms >> 24) & 1);
#ifdef MIXED_SSB_SUPPORT
                        response += sprintf(response, "SSB                   = %d\n", (alarms >> 28) & 1);
#endif

                        response += sprintf(response, "BF Spatial Stream Status Registers (Spatial Stream Mask)\n");
                        response += sprintf(response, "\tcache_dl_miss_flags     = 0x%05X\n", flags.cache_dl_miss_flags);
                        response += sprintf(response, "\tcplane_dl_no_desc_flags = 0x%05X\n", flags.cplane_dl_no_desc_flags);
                        response += sprintf(response, "\tcache_ul_miss_flags     = 0x%05X\n", flags.cache_ul_miss_flags);
                        response += sprintf(response, "\tcplane_ul_no_desc_flags = 0x%05X\n", flags.cplane_ul_no_desc_flags);

                        response += sprintf(response, "BF DFE Interface Status Registers (DFE Interface Mask)\n");
                        response += sprintf(response, "\tdfe_ul_config_flags = 0x%02X\n", flags.dfe_ul_config_flags);

                        response += sprintf(response, "BF AIE SYNC Status\n");
                        //response += sprintf(response, "\taie_dl_flags    = 0x%08X\n", flags.aie_dl_flags);
                        response += sprintf(response, "\taie_dl_in_sync  = %d\n", (flags.aie_dl_flags) & 1);
                        response += sprintf(response, "\taie_dl_out_sync = %d\n", (flags.aie_dl_flags >> 8) & 1);
                        response += sprintf(response, "\taie_dl_uf       = %d\n", (flags.aie_dl_flags >> 16) & 1);
                        //response += sprintf(response, "\taie_ul_flags    = 0x%08X\n", flags.aie_ul_flags);
                        response += sprintf(response, "\taie_ul_in_sync  = %d\n", (flags.aie_ul_flags) & 1);
                        response += sprintf(response, "\taie_ul_out_sync = %d\n", (flags.aie_ul_flags >> 8) & 1);
                        response += sprintf(response, "\taie_ul_uf       = %d\n", (flags.aie_ul_flags >> 16) & 1);

                        response += sprintf(response, "BF PRACH Status\n");
                        //response += sprintf(response, "\tprach_flags           = 0x%08X\n", flags.prach_flags);
                        response += sprintf(response, "\tprach_seq_len_err     = %d\n", (flags.prach_flags) & 1);
                        response += sprintf(response, "\tprach_format_err      = %d\n", (flags.prach_flags >> 1) & 1);
                        response += sprintf(response, "\tprach_scs_invalid     = %d\n", (flags.prach_flags >> 2) & 1);
                        response += sprintf(response, "\tprach_num_sym_invalid = %d\n", (flags.prach_flags >> 3) & 1);
                        response += sprintf(response, "\tprach_fd_ch_timeout   = %d\n", (flags.prach_flags >> 4) & 1);
                        response += sprintf(response, "\tprach_fd_alloc        = %d\n", (flags.prach_flags >> 5) & 1);
                        response += sprintf(response, "\tprach_startprbc_err   = %d\n", (flags.prach_flags >> 6) & 1);
                        response += sprintf(response, "\tprach_numprbc_err     = %d\n", (flags.prach_flags >> 7) & 1);
                        response += sprintf(response, "\tprach_dfe_mismatch    = %d\n", (flags.prach_flags >> 8) & 1);
                        response += sprintf(response, "\tprach_cmac_invalid    = %d\n", (flags.prach_flags >> 9) & 1);
#ifdef MIXED_SSB_SUPPORT
                        response += sprintf(response, "BF SSB Status\n");
                        response += sprintf(response, "ssb_flags = 0x%08X\n", flags.ssb_flags);
#endif
                        return SUCCESS;
                    }
                }
            }
        }
#endif // BF_INCLUDED
    }
    return UNKNOWN_COMMAND;
#endif // NO_HW
}

/**
 * @brief "monitor" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int monitor(const char *request, char *response)
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
            // monitor fhi clear
            // monitor fhi snapshot
            const char *s1;
            const char *s2;
            if (parse_string(1, &s1) && parse_string(2, &s2))
            {
                if (match(s1, "fhi"))
                {
                    if (match(s2, "clear"))
                    {
                        return xorif_monitor_clear();
                    }
                    else if (match(s2, "snapshot"))
                    {
                        return xorif_monitor_snapshot();
                    }
                }
            }
        }
        else if (num_tokens == 4)
        {
            // monitor fhi select <stream>
            // monitor fhi read <counter>
            const char *s1;
            const char *s2;
            unsigned int val1;
            if (parse_string(1, &s1) && parse_string(2, &s2) && parse_integer(3, &val1))
            {
                if (match(s1, "fhi"))
                {
                    if (match(s2, "select"))
                    {
                        return xorif_monitor_select(val1);
                    }
                    else if (match(s2, "read"))
                    {
                        unsigned long val2;
                        int result = xorif_monitor_read(val1, &val2);
                        if (result == XORIF_SUCCESS)
                        {
                            response += sprintf(response, "status = 0\n");
                            response += sprintf(response, "counter = %lu\n", val2);
                            return SUCCESS;
                        }
                    }
                }
            }
        }
    }
    return UNKNOWN_COMMAND;
#endif // NO_HW
}

/**
 * @brief "stall" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int stall(const char *request, char *response)
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
            // stall snapshot
            // stall read
            const char *s1;
            const char *s2;
            if (parse_string(1, &s1) && parse_string(2, &s2))
            {
                if (match(s1, "snapshot"))
                {
                    return xorif_stall_monitor_snapshot();
                }
                else if (match(s1, "read"))
                {
                    struct xorif_stall_monitor value;
                    int result = xorif_stall_monitor_read(&value);
                    if (result == XORIF_SUCCESS)
                    {
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "dl_ss = 0x%X\n", value.dl_ss);
                        response += sprintf(response, "ul_ss = 0x%X\n", value.ul_ss);
                        response += sprintf(response, "prach_ss = 0x%X\n", value.prach_ss);
                        response += sprintf(response, "ssb_ss = 0x%X\n", value.ssb_ss);
                        response += sprintf(response, "unsol_ss = 0x%X\n", value.unsol_ss);
                        return SUCCESS;
                    }
                }
            }
        }
    }
    return UNKNOWN_COMMAND;
#endif // NO_HW
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
            return xobf_schedule_bf(xobf_get_enabled_cc_mask());
        }
        else if (num_tokens == 2)
        {
            // schedule_bf [mask]
            unsigned int mask = 0;
            if (parse_integer(1, &mask))
            {
                return xobf_schedule_bf(mask);
            }
        }
#if 0
        if (num_tokens >= 2)
        {
            // schedule_bf {(<CxAx> | <Cx>)}
            // e.g. schedule_bf C0A0 C0A1 C1 C0A0 C0A1
            // e.g. schedule_bf C0 C1 C0
            // e.g. schedule_bf C0 C1 C0A0 C0A1+
            // e.g. schedule_bf C0- C1- C0A0- C0A1
            // Note, the "+" denotes with zero-padding
            // Note, the "-" denotes without zero-padding

            int offset = 1;
            struct xobf_schedule_info *ptr;
            int count = num_tokens - offset;
            ptr = calloc(count, sizeof(struct xobf_schedule_info));
            if (ptr)
            {
                for (int i = 0; i < count; ++i)
                {
                    int cc, ag;
                    char flags;
                    int valid = 0;
                    int padding = 0; // Default is to pad (might change)

                    if ((sscanf(token[i + offset], "C%dA%d%c", &cc, &ag, &flags) == 3) && (flags == '+'))
                    {
                        // Single AG, with zero-padding
                        padding = 1;
                        valid = 1;
                    }
                    else if ((sscanf(token[i + offset], "C%d%c", &cc, &flags) == 2) && (flags == '+'))
                    {
                        // Both AGs, with zero-padding
                        ag = 2;
                        padding = 1;
                        valid = 1;
                    }
                    if ((sscanf(token[i + offset], "C%dA%d%c", &cc, &ag, &flags) == 3) && (flags == '-'))
                    {
                        // Single AG, without zero-padding
                        padding = 0;
                        valid = 1;
                    }
                    else if ((sscanf(token[i + offset], "C%d%c", &cc, &flags) == 2) && (flags == '-'))
                    {
                        // Both AGs, without zero-padding
                        ag = 2;
                        padding = 0;
                        valid = 1;
                    }
                    else if (sscanf(token[i + offset], "C%dA%d", &cc, &ag) == 2)
                    {
                        // Single AG, with default zero-padding
                        valid = 1;
                    }
                    else if (sscanf(token[i + offset], "C%d", &cc) == 1)
                    {
                        // Both AGs, with default zero-padding
                        ag = 2;
                        valid = 1;
                    }

                    if (valid)
                    {
                        ptr[i].cc = cc;
                        ptr[i].ag = (ag & 0xFF) | (padding ? 0x0 : 0x100);
                    }
                    else
                    {
                        free(ptr);
                        return MALFORMED_COMMAND;
                    }
                }

                int result = xobf_populate_bf_schedule(count, ptr);

                free(ptr);
                return result;
            }
        }
#endif
    }
    return UNKNOWN_COMMAND;
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
            // load (beam_weights | prach_config) <file>
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
                    if (match(s2, "NULL"))
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
    }
    return UNKNOWN_COMMAND;
#endif
}
#endif // BF_INCLUDED

#ifdef EXTRA_DEBUG
/**
 * @brief "test_fhi" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int test_fhi(const char *request, char *response)
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
            // test_fhi <command> ...
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "errors"))
                {
                    // test_fhi errors <status>
                    unsigned int status;
                    if (parse_integer(2, &status))
                    {
                        extern int xorif_test_error_injections(uint32_t);
                        int result = xorif_test_error_injections(status);
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "result = %d\n", result);
                        return SUCCESS;
                    }
                }
            }
        }
    }
    return UNKNOWN_COMMAND;
#endif
}

#ifdef BF_INCLUDED
/**
 * @brief "test_bf" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int test_bf(const char *request, char *response)
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
            // test_bf <command> ...
            const char *s;
            if (parse_string(1, &s))
            {
                if (match(s, "errors"))
                {
                    // test_bf errors <instance> <status>
                    unsigned int instance, status;
                    if (parse_integer(2, &instance) && parse_integer(3, &status))
                    {
                        extern int xobf_test_error_injections(uint16_t, uint32_t);
                        int result = xobf_test_error_injections(instance, status);
                        response += sprintf(response, "status = 0\n");
                        response += sprintf(response, "result = %d\n", result);
                        return SUCCESS;
                    }
                }
            }
        }
    }
    return UNKNOWN_COMMAND;
#endif
}
#endif // BF_INCLUDED
#endif // EXTRA_DEBUG

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
    char buff[LINE_BUFF_SIZE];
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
    while (fgets(buff, LINE_BUFF_SIZE, fp) != NULL)
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
    char buff[LINE_BUFF_SIZE];
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
    while (fgets(buff, LINE_BUFF_SIZE, fp) != NULL)
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

#ifdef SRS_INCLUDED
/**
 * @brief "load_ru_table" command.
 * @param[in] request Pointer to request string
 * @param[in,out] response Pointer to response string
 * @returns
 *      - 0 if successful
 *      - Error code if not successful
 */
static int load_ru_table(const char *request, char *response)
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
            // load_ru_table srs
            const char *s1;
            if (parse_string(1, &s1))
            {
                if (match(s1, "srs"))
                {
                    return xsrs_configure_antenna_carrier();
                }
            }
        }
    }
    return UNKNOWN_COMMAND;
#endif
}
#endif // SRS_INCLUDED

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
