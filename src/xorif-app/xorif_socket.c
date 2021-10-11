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
 * @file xorif_file.c
 * @author Steven Dickinson
 * @brief Source file for the xorif-app "socket mode" functions.
 * @addtogroup xorif-app
 * @{
 */

// TODO sort out these headers, are they all needed?
#include <inttypes.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <linux/net_tstamp.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <linux/sockios.h>
#include "xorif_app.h"
#include "ecpri_proto.h"

int ip_socket_fd = -1;
int udp_socket_fd = -1;
int new_socket_fd = -1;

// Enable LEGACY_ECPRI for the legacy EPCI message processing, etc.
//#define LEGACY_ECPRI

#ifdef LEGACY_ECPRI
#define NUM_LISTEN_SOCKETS 2
#else
#define NUM_LISTEN_SOCKETS 1
#endif
#define IP_POLL_FD 0
#define UDP_POLL_FD 1
struct pollfd fds[NUM_LISTEN_SOCKETS];

/**
 * @brief Open sockets connections.
 * @returns
 *      - 0 if successful
 *      - Error code on failure
 * @details
 * The open connections are added to a "pollfd" structure.
 */
static int open_connections(void)
{
    struct sockaddr_in addr;
    int err;
    int opt;
#ifdef LEGACY_ECPRI
    struct ifreq ifreq;
    struct hwtstamp_config cfg;
#endif

    // Creating TCP/IP socket file descriptor
    if ((ip_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        PERROR("Socket creation failed\n");
        return COMMS_ERROR;
    }

    // Set socket options for address & port re-use
    // Note:
    // This code used to use SO_REUSEPORT, but this can causes orphan servers.
    // However, keeping the SO_REUSEADDR to avoid blocking during TIME_WAIT.
    opt = 1;
    //err = setsockopt(ip_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    err = setsockopt(ip_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (err < 0)
    {
        PERROR("Socket options failed (SO_REUSEADDR): %x\n", err);
        return COMMS_ERROR;
    }

    // Set-up address
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // Bind socket to the port
    if (bind(ip_socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        PERROR("Socket bind failed (check that the port is not already in use)\n");
        return COMMS_ERROR;
    }

    // Start listening on port
    if (listen(ip_socket_fd, 3) < 0)
    {
        PERROR("Socket listen failed\n");
        return COMMS_ERROR;
    }

    // Add to poll structure
    fds[IP_POLL_FD].fd = ip_socket_fd;
    fds[IP_POLL_FD].events = POLLIN;

#ifdef LEGACY_ECPRI
    // Creating UDP socket file descriptor
    if ((udp_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        PERROR("Socket creation failed\n");
        return COMMS_ERROR;
    }

    // Set socket options for address & port re-use
    opt = 1;
    err = setsockopt(udp_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (err < 0)
    {
        PERROR("Socket options failed (SO_REUSEADDR | SO_REUSEPORT): %x\n", err);
        return COMMS_ERROR;
    }

    // Set socket options for time-stamping
    opt = 0;
    opt |= SOF_TIMESTAMPING_TX_HARDWARE | SOF_TIMESTAMPING_TX_SOFTWARE;
    opt |= SOF_TIMESTAMPING_RX_HARDWARE | SOF_TIMESTAMPING_RX_SOFTWARE;
    opt |= SOF_TIMESTAMPING_RAW_HARDWARE | SOF_TIMESTAMPING_SOFTWARE;
#ifdef SOF_TIMESTAMPING_OPT_TX_SWHW
    opt |= SOF_TIMESTAMPING_OPT_TX_SWHW;
#endif
#ifdef SOF_TIMESTAMPING_OPT_ID
    opt |= SOF_TIMESTAMPING_OPT_ID;
#endif
#ifdef SOF_TIMESTAMPING_OPT_TSONLY
    opt |= SOF_TIMESTAMPING_OPT_TSONLY;
#endif
#ifdef SOF_TIMESTAMPING_OPT_CMSG
    opt |= SOF_TIMESTAMPING_OPT_CMSG;
#endif
    err = setsockopt(udp_socket_fd, SOL_SOCKET, SO_TIMESTAMPING, &opt, sizeof(opt));
    if (err < 0)
    {
        PERROR("Socket options failed (SO_TIMESTAMPING): %x\n", err);
        return COMMS_ERROR;
    }

    // Set up the ifreq structure
    memset(&ifreq, 0, sizeof(ifreq));
    snprintf(ifreq.ifr_name, sizeof(ifreq.ifr_name) - 1, "%s", eth_device_name);
    ifreq.ifr_data = (void *)&cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.tx_type = HWTSTAMP_TX_ON;
    cfg.rx_filter = HWTSTAMP_FILTER_ALL;

    // Configure socket using IOCTL
    err = ioctl(udp_socket_fd, SIOCSHWTSTAMP, &ifreq);
    if (err < 0)
    {
        // Treat as "warning" rather than "error"
        err = errno;
        TRACE("Socket IOCTL failed (SIOCSHWTSTAMP): %x\n", err);
        if (err == ERANGE)
        {
            TRACE("The requested time stamping mode is not supported by the hardware.\n");
        }
    }

    // Bind this socket to a particular device!
    memset(&ifreq, 0, sizeof(ifreq));
    snprintf(ifreq.ifr_name, sizeof(ifreq.ifr_name) - 1, "%s", eth_device_name);
    if (setsockopt(udp_socket_fd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifreq, sizeof(ifreq)) < 0)
    {
        PERROR("UDP device bind failed (SO_BINDTODEVICE): %x\n", errno);
        return COMMS_ERROR;
    }

    // Set-up address
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // Bind socket to the port
    if (bind(udp_socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        PERROR("Socket bind failed\n");
        return COMMS_ERROR;
    }

    // Add to poll structure
    fds[UDP_POLL_FD].fd = sock_ip;
    fds[UDP_POLL_FD].events = POLLIN;
#endif // LEGACY_ECPRI

    return SUCCESS;
}

/**
 * @brief Closes open socket connections.
 */
static void close_connections(void)
{
    if (ip_socket_fd != -1)
    {
        close(ip_socket_fd);
    }

    if (udp_socket_fd != -1)
    {
        close(udp_socket_fd);
    }
}

int send_to_target(const char *request, char *response)
{
    struct sockaddr_in addr;
    int sock_fd;

    // Set-up server address
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr_name);
    addr.sin_port = htons(port);

    // Creating TCP/IP socket file descriptor
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        PERROR("Socket creation failed\n");
        return COMMS_ERROR;
    }

    // Connect to server
    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        PERROR("Socket connection failed\n");
        return COMMS_ERROR;
    }

    // Send request message to server
    if (write(sock_fd, request, strlen(request)) < 0)
    {
        PERROR("Socket write message failed\n");
        return COMMS_ERROR;
    }

    // Read response message from server
    int n = read(sock_fd, response, MAX_BUFF_SIZE - 1);
    if (n < 0)
    {
        PERROR("Socket read message failed\n");
        return COMMS_ERROR;
    }

    // Terminate the string in the buffer
    response[n] = '\0';

    // Close connection
    close(sock_fd);

    return SUCCESS;
}

int send_to_host(const char *response)
{
    if (write(new_socket_fd, response, strlen(response)) < 0)
    {
        PERROR("Socket write message failed\n");
        return COMMS_ERROR;
    }

    return SUCCESS;
}

int do_socket(void)
{
    struct sockaddr_in addr;
    int addr_length = sizeof(addr);
    static char buff[MAX_BUFF_SIZE];
    int quit = 0;
    int ret;

    // Open connections
    if (open_connections() != SUCCESS)
    {
        PERROR("Failed to open connection\n");
        return COMMS_ERROR;
    }

    // TODO need to decide how to handle errors here, stop or continue?
    while (!quit)
    {
        // Poll the connections
        if ((ret = poll(fds, NUM_LISTEN_SOCKETS, -1)) < 0)
        {
            PERROR("Comms connection polling failed\n");
            return COMMS_ERROR;
        }

        if (fds[IP_POLL_FD].revents & POLLIN)
        {
            // Handle TCP/IP socket events...

            if ((new_socket_fd = accept(ip_socket_fd, (struct sockaddr *)&addr, (socklen_t *)&addr_length)) < 0)
            {
                PERROR("Socket accept failed\n");
                return COMMS_ERROR;
            }

            // Read message
            int n = read(new_socket_fd, buff, MAX_BUFF_SIZE - 1);
            if (n < 0)
            {
                PERROR("Socket read message failed\n");
                return COMMS_ERROR;
            }

            // Terminate the string in the buffer
            buff[n] = '\0';

            // Process message as a command
            int result = do_command(buff);
            if (result < 0)
            {
                PERROR("(%d) whilst processing socket comms\n", result);
                PERROR("'%s'\n", buff);
            }
            else if (result == TERMINATE)
            {
                TRACE("Terminating application\n");
                quit = 1;
            }

            // Close connection
            close(new_socket_fd);
        }
#ifdef LEGACY_ECPRI
        else if (fds[UDP_POLL_FD].revents & (POLLIN | POLLERR))
        {
            // Handle UDP socket events...
            int result = proto_ecpri_handle_incoming_msg(fds[UDP_POLL_FD].fd, fds[UDP_POLL_FD].revents, NULL);
            if (result < 0)
            {
                PERROR("Error processing ECPRI protocol (%d)\n", result);
            }
        }
#endif
    }

    // Close connections
    close_connections();

    return SUCCESS;
}

/** @} */