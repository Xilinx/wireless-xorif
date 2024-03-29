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
 * @file xorif_app.c
 * @author Steven Dickinson
 * @brief Main source file for the Xilinx ORAN Radio Interface example application (xorif-app).
 * @addtogroup xorif-app
 * @{
 *
 * Example application for the Xilinx ORAN Radio Interface (xorif-app)
 */

#include "xorif_app.h"

// Operational mode
enum op_mode
{
    SERVER_MODE = 0,
    CMD_LINE_MODE,
    FILE_MODE,
    INTERACTIVE_MODE,
};

// Global variables
int app_trace = 0;
int remote_host = 0;
int remote_target = 0;
int port = 5001; // Default port
const char *ip_addr_name = "127.0.0.1"; // Default IP address
const char *pid_file = "/var/run/xorif-app.pid";
const char *copy_right = "(c) Copyright 2019 - 2022 Advanced Micro Devices, Inc. All rights reserved.";
#ifdef XNRAAS_APP
const char * const app_server_str = "XNRAAS-APP-SRV";
const char * const app_client_str = "XNRAAS-APP-CLI";
#else
const char * const app_server_str = "XORIF-APP-SRV";
const char * const app_client_str = "XORIF-APP-CLI";
#endif
const char * const app_version_str = "2.0"; // Version string
const char *app_name;
int no_fhi = 0;
int no_bf = 0;
int no_srs = 0;

#ifdef TEST_CALLBACK
static void isr_callback1(uint32_t status)
{
    TRACE("isr_callback1 %d\n", status);
}

static void isr_callback2(uint32_t status)
{
    TRACE("isr_callback2 %d\n", status);
}
#endif

/**
 * @brief Main entry point for XORIF-APP.
 *
 * @param[in] argc Number of command line arguments
 * @param[in] argv Array of C-style strings containing the command line arguments
 *
 * @returns
 *      - EXIT_SUCCESS on success
 *      - EXIT_FAILURE on error
 */
int main(int argc, char *argv[])
{
    int mode = SERVER_MODE;
    int do_help = 0;
    int do_banner = 1;
    int do_init = 0;
    int do_version = 0;
    const char *file = "";
    int opt;

    // Process command line options
    opterr = 0;
    while ((opt = getopt(argc, argv, "bcf:hin:p:svSBFIV")) != -1)
    {
        switch (opt)
        {
        case 'b':
            do_banner = 0;
            break;
        case 'c':
            mode = CMD_LINE_MODE;
            do_banner = 0;
            break;
        case 'f':
            mode = FILE_MODE;
            file = optarg;
            break;
        case 'h':
            do_help = 1;
            break;
        case 'i':
            do_init = 1;
            break;
        case 'n':
            ip_addr_name = optarg;
            break;
        case 'p':
            if (sscanf(optarg, "%d", &port) != 1)
            {
                fprintf(stderr, "Invalid value for '-p' option\n");
                do_help = -1;
            }
            break;
        case 's':
            mode = SERVER_MODE;
            break;
        case 'v':
            app_trace = 1;
            break;
        case 'B':
            no_bf = 1;
            break;
        case 'S':
            no_srs = 1;
            break;
        case 'F':
            no_fhi = 1;
            break;
        case 'I':
            mode = INTERACTIVE_MODE;
            break;
        case 'V':
            do_version = 1;
            break;
        case '?':
            if (optopt == 'e' || optopt == 'f' || optopt == 'n' || optopt == 'p')
            {
                fprintf(stderr, "Option '-%c' requires an argument\n", optopt);
            }
            else if (isprint(optopt))
            {
                fprintf(stderr, "Unknown option '-%c'\n", optopt);
            }
            else
            {
                fprintf(stderr, "Unknown option character '\\x%x'\n", optopt);
            }
            do_help = -1;
            break;
        }
    }

    // Process command line non-option arguments
    if (mode != CMD_LINE_MODE)
    {
        for (; optind < argc; ++optind)
        {
            fprintf(stderr, "Unknown argument '%s'\n", argv[optind]);
            do_help = -1;
        }
    }

    if (do_help)
    {
        printf("Usage: %s [-c | -f <file> |-I | -s] [-n <ip_addr>] [-p <port>] [<options>] {\"<command> {<arguments>}\"}\n", argv[0]);
        printf("\t-h Show help\n");
        printf("\t-c Client mode using the command line\n");
        printf("\t-f <file> Client mode using the specified file\n");
        printf("\t-I Client mode using interactive input (type exit to leave)\n");
        printf("\t-s Server mode (default)\n");
        printf("\t-n <ip_addr> Specified IP address (client mode only) (defaults to localhost)\n");
        printf("\t-p <port> Specified port (defaults to 5001)\n");

        printf("\t-b Disable banner on start\n");
        printf("\t-i Auto-initialize (server mode only)\n");
        printf("\t-v Verbose\n");
        printf("\t-V Display version of the application\n");

        printf("\t<command> {<arguments>} For command line mode only\n");

        return (do_help > 0) ? SUCCESS : FAILURE;
    }

    if (do_banner)
    {
        // Banner
        printf("\n");
#ifdef XNRAAS_APP
        printf(" __  ___   _ ____      _        _    ____          _    ____  ____  \n");
        printf(" \\ \\/ / \\ | |  _ \\    / \\      / \\  / ___|        / \\  |  _ \\|  _ \\ \n");
        printf("  \\  /|  \\| | |_) |  / _ \\    / _ \\ \\___ \\ _____ / _ \\ | |_) | |_) |\n");
        printf("  /  \\| |\\  |  _ <  / ___ \\  / ___ \\ ___) |_____/ ___ \\|  __/|  __/ \n");
        printf(" /_/\\_\\_| \\_|_| \\_\\/_/   \\_\\/_/   \\_\\____/     /_/   \\_\\_|   |_|    \n");
#else
        printf(" __  _____  ____  ___ _____       _    ____  ____  \n");
        printf(" \\ \\/ / _ \\|  _ \\|_ _|  ___|     / \\  |  _ \\|  _ \\ \n");
        printf("  \\  / | | | |_) || || |_ _____ / _ \\ | |_) | |_) |\n");
        printf("  /  \\ |_| |  _ < | ||  _|_____/ ___ \\|  __/|  __/ \n");
        printf(" /_/\\_\\___/|_| \\_\\___|_|      /_/   \\_\\_|   |_|    \n");
#endif
        printf("%s\n", copy_right);
        printf("\n");
    }

    // Set name according to server or client
    app_name = (mode == SERVER_MODE) ? app_server_str : app_client_str;

    if (do_version)
    {
        printf("Version %s\n", app_version_str);
        return SUCCESS;
    }

    TRACE("Version = %s\n", app_version_str);
    TRACE("Mode = %d\n", mode);
    TRACE("IP addr = %s, Port = %d\n", ip_addr_name, port);

    int result = 0;
    switch (mode)
    {
    case CMD_LINE_MODE:
        // Command line mode
        TRACE("Command line mode\n");
        remote_host = 0;
        remote_target = 1;
        if (optind < argc)
        {
            // Process commands
            for (; optind < argc; ++optind)
            {
                result = do_command(argv[optind]);
                if ((result < 0) || (result == FAILURE))
                {
                    PERROR("(%d) whilst processing command line\n", result);
                    PERROR("'%s'\n", argv[optind]);
                    return result;
                }
                else if (result == TERMINATE)
                {
                    return SUCCESS;
                }
            }
        }
        break;

    case FILE_MODE:
        // File mode
        TRACE("File mode (%s)\n", file);
        remote_host = 0;
        remote_target = 1;
        return do_file(file);

    case SERVER_MODE:
        // Server mode
        TRACE("Server mode\n");
#ifdef NO_HW
        fprintf(stderr, "No hardware\n");
        (void)do_init; // Prevent warning 'unused-but-set-variable'
        return FAILURE;
#else
        remote_target = 0;
        if (do_init)
        {
            // Send "init" command locally
            TRACE("Sending 'init' command locally\n");
            remote_host = 0;
            do_command("init");
        }
        remote_host = 1;

#ifdef TEST_CALLBACK
        // Test register call-backs
        xorif_register_fhi_isr(isr_callback1);
#ifdef BF_INCLUDED
        xobf_register_bf_isr(isr_callback2);
#endif
#endif

#ifdef USE_PIDFILE
        // Create pid file for server mode operation
        FILE *fp = fopen(pid_file, "w");
        if (fp)
        {
            int pid = getpid();
            fprintf(fp, "%d\n", pid);
            fclose(fp);
        }
#endif

        result = do_socket();

#ifdef USE_PIDFILE
        // Delete pid file before exit
        remove(pid_file);
#endif

        return result;
#endif

    case INTERACTIVE_MODE:
        // Interactive mode
        TRACE("Interactive mode\n");

        remote_host = 0;
        remote_target = 1;
        return do_interactive("> ");

    default:
        // No mode selected
        TRACE("No mode selected\n");
        break;
    }

    return SUCCESS;
}

/** @} */