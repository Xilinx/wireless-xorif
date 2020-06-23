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
 * @file xorif_menu.c
 * @author Steven Dickinson
 * @brief Source file for the xorif-app "menu mode" functions.
 * @addtogroup xorif-app
 * @{
 */

#include "xorif_app.h"

static char buff[MAX_BUFF_SIZE];

/**
 * @brief Prompts user for integer, scans valid input and range.
 * @param[in] prompt Prompt string to display
 * @param[in] min Minimum allowed value
 * @param[in] max Maximum allowed value
 * @returns
 *      - Returns the validated integer from user input
 * @note Function will keep requesting input from user until valid.
 */
static unsigned int get_integer(const char *prompt, unsigned int min, unsigned int max)
{
    unsigned int number;

    while (1)
    {
        // Print the prompt
        printf("%s : ", prompt);

        // Fill the input buffer
        if (fgets(buff, MAX_BUFF_SIZE, stdin) == NULL)
        {
            PERROR("Failed to read input\n");
            exit(1);
        }

        // Note: using scanf here, checking for the number AND a whitespace
        // This ensures that we will only successfully parse whole numbers
        // eg. '10', '0x10', '1e4' are ok, but not '10a', etc.
        // Note: Although floats are allowed as input these are returned as
        // integers. This allows for input such as 1.23e5 (i.e. 123000)
        unsigned int u;
        float f;
        char c;
        int valid = 0;
        if (sscanf(buff, "0x%x%c", &u, &c) == 2 && isspace(c))
        {
            number = (unsigned int)u;
            //printf("Hex: %d\n", number);
            valid = 1;
        }
        else if (sscanf(buff, "%u%c", &u, &c) == 2 && isspace(c))
        {
            number = (unsigned int)u;
            //printf("Int: %d\n", number);
            valid = 1;
        }
        else if (sscanf(buff, "%f%c", &f, &c) == 2 && isspace(c))
        {
            number = (unsigned int)f;
            //printf("Float: %d\n", number);
            valid = 1;
        }

        if (valid && (number >= min && number <= max))
        {
            return number;
        }
        else
        {
            printf("Valid values are [%d..%d]\n", min, max);
        }
    }
}

/**
 * @brief Prompts user for string.
 * @param[in] prompt Prompt string to display
 * @returns
 *      - Returns the string
 */
static char *get_string(const char *prompt)
{
    static char buff2[MAX_BUFF_SIZE]; // TODO FIX this

    while (1)
    {
        // Print the prompt
        printf("%s : ", prompt);

        // Fill the input buffer
        if (fgets(buff, MAX_BUFF_SIZE, stdin) == NULL)
        {
            PERROR("Failed to read input\n");
            exit(1);
        }

        // Attempt to scan input as an string
        if (sscanf(buff, "%s", buff2) == 1)
        {
            // Remove any trailing newline character
            if (buff2[strlen(buff2) -1] == '\n')
            {
                buff2[strlen(buff2) -1] = '\0';
            }
            return buff2;
        }
    }
}

/**
 * @brief Handle menu for component carrier configuration.
 * @returns
 *      0 - Indicates to return to previous menu
 *      1 - Indicates to exit whole menu system
 */
static int do_cc_config_menu(void)
{
    int cc = 0;
    unsigned int val1, val2, val3;

    while (1)
    {
        printf("\n");
        printf("XORIF-APP Component Carrier Configuration Menu\n");
        printf("==============================================\n");
        printf("  1) Set component carrier number (current = %d)\n", cc);
        printf("  2) Set number of RBs\n");
        printf("  3) Set numerology\n");
        printf("  4) Set time advance offsets\n");
        printf("  5) Set uplink IQ compression\n");
        printf("  6) Set downlink IQ compression\n");
        printf("  7) Configure component carrier\n");
        printf("  8) Enable component carrier\n");
        printf("  9) Disable component carrier\n");
        printf(" 10) Get component carrier configuration\n");
        printf(" 11) Get component carrier allocation\n");
        printf(" 99) Back to previous menu\n");
        printf("100) Exit\n");
        printf("\n");

        // Get user choice
        int choice = get_integer("Enter", 0, 100);

        // Process choice
        switch (choice)
        {
        case 100:
            // Exit
            return 1;
        case 99:
            // Back to previous menu
            return 0;
        case 1:
            // Set component carrier
            cc = get_integer("Enter component carrier number", 0, 7);
            break;
        case 2:
            // Set number of RBs
            val1 = get_integer("Enter number of RBs", 2, 275);
            do_command("set num_rbs %d %d", cc, val1);
            break;
        case 3:
            // Set numerology
            val1 = get_integer("Enter numerology", 0, 5);
            do_command("set numerology %d %d 0", cc, val1);
            break;
        case 4:
            // Set time advance offsets
            val1 = get_integer("Enter deskew (ps)", 0, UINT32_MAX);
            val2 = get_integer("Enter time advance uplink (ps)", 0, UINT32_MAX);
            val3 = get_integer("Enter time advance downlink (ps)", 0, UINT32_MAX);
            do_command("set time_advance %d %d %d %d", cc, val1, val2, val3);
            break;
        case 5:
            // Set uplink IQ compression
            val1 = get_integer("Enter IQ compression width", 0, 32);
            val2 = get_integer("Enter IQ compression mode", 0, 4);
            do_command("set ul_iq_compression %d %d %d", cc, val1, val2);
            break;
        case 6:
            // Set downlink IQ compression
            val1 = get_integer("Enter IQ compression width", 0, 32);
            val2 = get_integer("Enter IQ compression mode", 0, 4);
            do_command("set dl_iq_compression %d %d %d", cc, val1, val2);
            break;
        case 7:
            // Configure component carrier
            do_command("configure %d", cc);
            break;
        case 8:
            // Enable component carrier
            do_command("enable %d", cc);
            break;
        case 9:
            // Disable component carrier
            do_command("disable %d", cc);
            break;
        case 10:
            // Get componet carrier configuration
            do_command("get cc_config %d", cc);
            break;
        case 11:
            // Get component carrier allocation
            do_command("get fhi_cc_alloc %d", cc);
            break;
        default:
            printf("Invalid option '%d'\n", choice);
            break;
        }
    }
}

/**
 * @brief Handle menu for ethernet port configuration.
 * @returns
 *      0 - Indicates to return to previous menu
 *      1 - Indicates to exit whole menu system
 */
static int do_eth_config_menu(void)
{
    int port = 0;
    const char *s;

    while (1)
    {
        printf("\n");
        printf("XORIF-APP Ethernet Configuration Menu\n");
        printf("=====================================\n");
        printf("  1) Set ethernet port number (current = %d)\n", port);
        printf("  2) Set destination MAC address\n");
        printf("  3) Set source MAC address\n");
        printf(" 99) Back to previous menu\n");
        printf("100) Exit\n");
        printf("\n");

        // Get user choice
        int choice = get_integer("Enter", 0, 100);

        // Process choice
        switch (choice)
        {
        case 100:
            // Exit
            return 1;
        case 99:
            // Back to previous menu
            return 0;
        case 1:
            // Set port number
            port = get_integer("Enter port number", 0, 3);
            break;
        case 2:
            // Set ethernet destination MAC address
            s = get_string("Enter destination MAC address (e.g. 01:02:03:04:05:06)");
            do_command("set dest_mac_addr %x %s", port, s);
            break;
        case 3:
            // Set ethernet source MAC address
            s = get_string("Enter destination MAC address (e.g. 01:02:03:04:05:06)");
            do_command("set src_mac_addr %x %s", port, s);
            break;
        default:
            printf("Invalid option '%d'\n", choice);
            break;
        }
    }
}

/**
 * @brief Handle menu for read/write registers.
 * @returns
 *      0 - Indicates to return to previous menu
 *      1 - Indicates to exit whole menu system
 */
static int do_register_menu(void)
{
    const char *s;
    unsigned int val1, val2;

    while (1)
    {
        printf("\n");
        printf("XORIF-APP Read/Write Register Menu\n");
        printf("==================================\n");
        printf("  1) Peek memory address\n");
        printf("  2) Poke memory address\n");
        printf("  3) Read FHI register\n");
        printf("  4) Read FHI register with offset\n");
        printf("  5) Write FHI register\n");
        printf("  6) Write FHI register with offset\n");
        printf(" 99) Back to previous menu\n");
        printf("100) Exit\n");
        printf("\n");

        // Get user choice
        int choice = get_integer("Enter", 0, 100);

        // Process choice
        switch (choice)
        {
        case 100:
            // Exit
            return 1;
        case 99:
            // Back to previous menu
            return 0;
        case 1:
            // Peek memory address
            val1 = get_integer("Enter address", 0, UINT32_MAX);
            do_command("peek 0x%x", val1);
            break;
        case 2:
            // Poke memory address
            val1 = get_integer("Enter address", 0, UINT32_MAX);
            val2 = get_integer("Enter value to write", 0, UINT32_MAX);
            do_command("poke 0x%x 0x%x", val1, val2);
            break;
        case 3:
            // Read FHI register
            s = get_string("Enter register name");
            do_command("read_reg fhi %s", s);
            break;
        case 4:
            // Read FHI register with offset
            s = get_string("Enter register name");
            val1 = get_integer("Enter offset", 0, UINT32_MAX);
            do_command("read_reg_offset fhi %s %d", s, val1);
            break;
        case 5:
            // Write FHI register
            s = get_string("Enter register name");
            val1 = get_integer("Enter value to write", 0, UINT32_MAX);
            do_command("write_reg fhi %s %d", s, val1);
            break;
        case 6:
            // Write FHI register with offset
            s = get_string("Enter register name");
            val1 = get_integer("Enter offset", 0, UINT32_MAX);
            val2 = get_integer("Enter value to write", 0, UINT32_MAX);
            do_command("write_reg_offset fhi %s %d %d", s, val1, val2);
            break;

        default:
            printf("Invalid option '%d'\n", choice);
            break;
        }
    }
}

/**
 * @brief Handle menu for reading stats/counters.
 * @returns
 *      0 - Indicates to return to previous menu
 *      1 - Indicates to exit whole menu system
 */
static int do_stats_menu(void)
{
    while (1)
    {
        printf("\n");
        printf("XORIF-APP Read Stats/Counters Menu\n");
        printf("==================================\n");
        printf("  1) Read FHI stats\n");
#ifdef BF_INCLUDED
        printf("  2) Read BF stats\n");
#endif // BF_INCLUDED
        printf(" 99) Back to previous menu\n");
        printf("100) Exit\n");
        printf("\n");

        // Get user choice
        int choice = get_integer("Enter", 0, 100);

        // Process choice
        switch (choice)
        {
        case 100:
            // Exit
            return 1;
        case 99:
            // Back to previous menu
            return 0;
        case 1:
            // Read FHI stats
            do_command("get fhi_stats");
            break;
#if BF_INCLUDED
        case 2:
            // Read BF stats
            do_command("get bf_stats");
            break;
#endif // BF_INCLUDED
        default:
            printf("Invalid option '%d'\n", choice);
            break;
        }
    }
}

/**
 * @brief Handle top level menu.
 * @returns
 *      0 - Indicates to return to previous menu
 *      1 - Indicates to exit whole menu system
 */
static int do_main_menu(void)
{
    unsigned int val;

    while (1)
    {
        printf("\n");
        printf("XORIF-APP Main Menu\n");
        printf("===================\n");
        printf("  1) Versions\n");
        printf("  2) Capabilities\n");
        printf("  3) Component carrier configuration\n");
        printf("  4) Ethernet configuration\n");
        printf("  5) Read/write registers and memory\n");
        printf("  6) Read stats / counters\n");
        printf("  7) Debug trace\n");
        printf("  8) Reset FHI\n");
#ifdef BF_INCLUDED
        printf("  9) Reset BF\n");
#endif // BF_INCLUDED
        printf("100) Exit\n");
        printf("\n");

        // Get user choice
        int choice = get_integer("Enter", 0, 100);

        // Process choice
        int exit = 0;
        switch (choice)
        {
        case 100:
            // Exit
            return 1;
        case 1:
            // Versions
            do_command("get sw_version");
            do_command("get fhi_hw_version");
            do_command("get fhi_hw_revision");
#ifdef BF_INCLUDED
            do_command("get bf_hw_version");
            do_command("get bf_hw_revision");
#endif // BF_INCLUDED
            break;
        case 2:
            // Capabilities
            do_command("get capabilities");
            do_command("get fhi_caps");
#ifdef BF_INCLUDED
            do_command("get bf_caps");
#endif // BF_INCLUDED
            break;
        case 3:
            // Component carrier configuration
            exit = do_cc_config_menu();
            break;
        case 4:
            // Ethernet configuration
            exit = do_eth_config_menu();
            break;
        case 5:
            // Read/write registers
            exit = do_register_menu();
            break;
        case 6:
            // Read stats / counters
            exit = do_stats_menu();
            break;
        case 7:
            // Set debug trace level
            val = get_integer("Enter trace level", 0, 5);
            do_command("debug %d", val);
            break;
        case 8:
            // Reset FHI
            do_command("reset fhi");
            break;
#ifdef BF_INCLUDED
        case 9:
            // Reset beamformer
            do_command("reset bf");
            break;
#endif
        default:
            printf("Invalid option '%d'\n", choice);
            break;
        }

        // Exit menu?
        if (exit == 1)
        {
            return 1;
        }
    }
}

int do_menu()
{
    // Initialize library
    do_command("init %s %s", fhi_dev_name ? fhi_dev_name : "", bf_dev_name ? bf_dev_name : "");

    // Do menu
    do_main_menu();

    // Finalize library
    //do_command("finish");

    return SUCCESS;
}

/** @} */
