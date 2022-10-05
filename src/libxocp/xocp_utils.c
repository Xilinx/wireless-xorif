/*
 * Copyright 2020 - 2022 Advanced Micro Devices, Inc.
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
 * @file xocp_utils.c
 * @author Steven Dickinson
 * @brief Source file for libxocp utility functions/definitions.
 * @addtogroup libxocp
 * @{
 */

#include <dirent.h>
#include <endian.h>
#include "xocp.h"
#include "xocp_utils.h"

/************************/
/*** Data definitions ***/
/************************/

#ifdef NO_HW
static const char *fake_device = "fake_device.00000000";
#endif

/****************************/
/*** Function definitions ***/
/****************************/

int get_node_property(const char *device,
                      const char *property,
                      void *dest,
                      size_t max_len)
{
    char path[MAX_PATH_LENGTH];
    snprintf(path,
             MAX_PATH_LENGTH,
             "/sys/bus/platform/devices/%s/of_node/%s",
             device,
             property);
    FILE *fp = fopen(path, "r");
    int status = 0;
    if (fp)
    {
        if (fgets(dest, max_len, fp))
        {
            status = 1;
        }
        fclose(fp);
    }

    return status;
}

const char *get_device_name(const char *dev_name, const char *compatible)
{
    static char buff[MAX_PATH_LENGTH];
    bool match = false;
#ifndef NO_HW
    DIR *folder;
    struct dirent *entry;

    // Open directory
    folder = opendir("/sys/bus/platform/devices/");
    if (folder == NULL)
    {
        PERROR("Unable to open '%s'\n", "/sys/bus/platform/devices/");
        return NULL;
    }

    // Iterate through files in directory
    while ((entry = readdir(folder)))
    {
        bool name_match = false;
        bool comp_match = false;

        if (dev_name)
        {
            // Look for devices with the given device name
            // Partial match allowed, e.g. "devXYZ" matches "a0000000.devXYZ"
            if (strstr(entry->d_name, dev_name))
            {
                // Matched given device name
                INFO("Matched device name (%s)\n", entry->d_name);
                name_match = true;
            }
        }

        if (compatible)
        {
            char property[MAX_PATH_LENGTH];

            // Look for devices with the given compatible property
            if (get_node_property(entry->d_name,
                                  "compatible",
                                  property,
                                  MAX_PATH_LENGTH))
            {
                // Partial match allowed, e.g. "dev-xyz" matches "dev-xyz-1.0"
                if (strstr(property, compatible))
                {
                    // Matched compatible property
                    INFO("Matched compatible property (%s)\n", entry->d_name);
                    comp_match = true;
                }
            }
        }

        if ((dev_name && name_match) || (!dev_name && comp_match))
        {
            // TODO - could keep searching, e.g. for other matching devices
            // TODO - always match comp_match too? Or leave flexible?
            match = true;
            strncpy(buff, entry->d_name, MAX_PATH_LENGTH);
            break;
        }
    }

    // Close the directory
    closedir(folder);

#else
    // Fake it with NO_HW
    if (!dev_name || strncmp(dev_name, fake_device, strlen(dev_name)) == 0)
    {
        match = true;
        strncpy(buff, fake_device, MAX_PATH_LENGTH);
    }
#endif

    if (match)
    {
        return buff;
    }
    else
    {
        return NULL;
    }
}

/** @} */