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
 * @file xorif_utils.c
 * @author Steven Dickinson
 * @brief Source file for libxorif common utility functions.
 * @addtogroup libxorif
 * @{
 */

#include <stdio.h>
#include <dirent.h>
#include <endian.h>
#include "xorif_api.h"
#include "xorif_system.h"
#include "xorif_fh_func.h"
#include "xorif_common.h"
#include "xorif_utils.h"

/**
 * @brief Comparator function for bsearch algorithm.
 * @param[in] key Key to look for
 * @param[in] data Data element that we're comparing against
 * @returns
 *      - <0 if key is before data
 *      - =0 if key is equal to data
 *      - >0 if key is after data
 */
static int reg_comparator(const void *key, const void *data)
{
    return strcmp((const char *)key, ((const reg_info_t *)data)->name);
}

const reg_info_t *find_register(const reg_info_t *reg_map, int num, const char *name)
{
    void *result = bsearch((const void *)name,
                           (const void *)reg_map,
                           num,
                           sizeof(reg_info_t),
                           reg_comparator);

    return (const reg_info_t *)result;
}

uint32_t read_reg_raw(struct metal_io_region *io, const char *name, uint32_t addr)
{
    ASSERT(io);
    if (!io)
    {
        // The device and/or IO region is not active, return 0
        PERROR("Libmetal device and/or IO region is not initialized\n");
        return 0;
    }

    // Libmetal read
    uint32_t value = metal_io_read32(io, addr);

#ifdef DEBUG
    if (xorif_trace == 3)
    {
        // Special formatting option
        printf("READ_REG:  %-35s (0x%04X)[%2d:%-2d] => 0x%X (%u)\n", name, addr, 0, 31, value, value);
    }
    else
    {
        TRACE("READ_REG: %s (0x%04X) => 0x%X (%u)\n", name, addr, value, value);
    }
#endif
    return value;
}

void write_reg_raw(struct metal_io_region *io, const char *name, uint32_t addr, uint32_t value)
{
    ASSERT(io);
    if (!io)
    {
        // The device and/or IO region is not active, return
        PERROR("Libmetal device and/or IO region is not initialized\n");
        return;
    }

    // Libmetal write
    metal_io_write32(io, addr, value);

#ifdef DEBUG
    if (xorif_trace == 3)
    {
        // Special formatting option
        printf("WRITE_REG: %-35s (0x%04X)[%2d:%-2d] <= 0x%X (%u)\n", name, addr, 0, 31, value, value);
    }
    else
    {
        TRACE("WRITE_REG: %s (0x%04X) <= 0x%X (%u)\n", name, addr, value, value);
    }
#endif
}

uint32_t read_reg(struct metal_io_region *io, const char *name, uint32_t addr, uint32_t mask, uint16_t shift, uint16_t width)
{
    ASSERT(io);
    if (!io)
    {
        // The device and/or IO region is not active, return
        PERROR("Libmetal device and/or IO region is not initialized\n");
        return 0;
    }

    // Libmetal read
    uint32_t value = (metal_io_read32(io, addr) & mask) >> shift;

#ifdef DEBUG
    if (xorif_trace == 3)
    {
        // Special formatting option
        printf("READ_REG:  %-35s (0x%04X)[%2d:%-2d] => 0x%X (%u)\n", name, addr, shift, shift + width - 1, value, value);
    }
    else
    {
        TRACE("READ_REG: %s (0x%04X)[%d:%d] => 0x%X (%u)\n", name, addr, shift, shift + width - 1, value, value);
    }
#endif

    return value;
}

void write_reg(struct metal_io_region *io, const char *name, uint32_t addr, uint32_t mask, uint16_t shift, uint16_t width, uint32_t value)
{
    ASSERT(io);
    if (!io)
    {
        // The device and/or IO region is not active, return
        PERROR("Libmetal device and/or IO region is not initialized\n");
        return;
    }

    // Libmetal read / modify / write
    uint32_t x = metal_io_read32(io, addr);
    x &= ~mask;
    x |= (value << shift) & mask;
    metal_io_write32(io, addr, x);

#ifdef DEBUG
    if (xorif_trace == 3)
    {
        // Special formatting option
        printf("WRITE_REG: %-35s (0x%04X)[%2d:%-2d] <= 0x%X (%u)\n", name, addr, shift, shift + width - 1, value, value);
    }
    else
    {
        TRACE("WRITE_REG: %s (0x%04X)[%d:%d] <= 0x%X (%u)\n", name, addr, shift, shift + width - 1, value, value);
    }
#endif
}

const char *get_device_name(const char *short_name)
{
    DIR *folder;
    struct dirent *entry;
    static char buff[256];

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
        // Check for matching short name
        char *s = strstr(entry->d_name, short_name);
        if (s != NULL && strlen(s) == strlen(short_name))
        {
            // Match found, copy the full device name
            strncpy(buff, entry->d_name, 255);
            buff[255] = '\0';

            // Close the directory and return result
            closedir(folder);
            return &buff[0];
        }
    }

    // Close the directory and return NULL
    closedir(folder);
    return NULL;
}

int get_device_property_u32(const char *dev_name, const char *prop_name, uint32_t *value)
{
    FILE *fp;
    char buff[256];

    // Create filepath for property
    sprintf(buff, "/sys/bus/platform/devices/%s/of_node/%s", dev_name, prop_name);

    // Try to open file
    fp = fopen(buff, "rb");
    if (fp == NULL)
    {
        PERROR("Unable to open '%s'\n", buff);
        return 0;
    }

    uint32_t temp;
    if (fread(&temp, sizeof(uint32_t), 1, fp) == 1)
    {
        // Value read, convert endianness
        *value = be32toh(temp);

        // Close the file and return success
        fclose(fp);
        return 1;
    }

    // Close the file and return failure
    fclose(fp);
    return 0;
}

int add_device(struct xorif_device_info *device, const char *bus_name, const char *dev_name)
{
    // Initialize device info structure
    device->bus_name = bus_name;
    device->dev_name = dev_name;
    device->dev = NULL;
    device->io = NULL;
    device->status = 0;

    // Open a device
    INFO("Opening device '%s'\n", dev_name);
    if (metal_device_open(bus_name, dev_name, &(device->dev)))
    {
        PERROR("Failed to open device '%s'\n", dev_name);
        return XORIF_FAILURE;
    }

    // Map the device to IO region
    INFO("Mapping IO region for device '%s'\n", dev_name);
    device->io = metal_device_io_region(device->dev, 0);
    if (device->io == NULL)
    {
        PERROR("Failed to map IO region for device '%s'\n", dev_name);
        return XORIF_FAILURE;
    }

    // Update device status
    device->status = 1;

    // Success
    return XORIF_SUCCESS;
}

int check_numerology(uint16_t numerology, uint16_t extended_cp)
{
    if (extended_cp && (numerology != 2))
    {
        // Extended CP requested for numerology other than 2
        return 0;
    }
    else if (extended_cp && !caps.extended_cp)
    {
        // Extended CP requested when it's not supported
        return 0;
    }
    else
    {
        // Check requested numerology against support mask
        return ((1 << numerology) & caps.numerologies);
    }
}

int check_iq_comp_mode(uint16_t bit_width, enum xorif_iq_comp comp_method)
{
    if (bit_width > 16)
    {
        // Unsupported compression width
        return 0;
    }
    else
    {
        // Check requested compression mode against support mask
        return ((1 << comp_method) & caps.iq_comp_methods);
    }
}

int check_bw_comp_mode(uint16_t bit_width, enum xorif_bw_comp comp_method)
{
    if (bit_width != 12)
    {
        // Unsupported compression width
        return 0;
    }
    else
    {
        // Check requested compression mode against support mask
        return ((1 << comp_method) & caps.bw_comp_methods);
    }
}

const char *binary_string(uint32_t value, uint16_t length)
{
    static char s[33];

    for (int i = 0; i < length; ++i)
    {
        s[length - 1 - i] = (value & 1) ? '1' : '0';
        value >>= 1;
    }
    s[length] = '\0';

    return s;
}

const char *binary_mask_string(uint32_t value, uint32_t mask, uint16_t length)
{
    static char s[33];

    for (int i = 0; i < length; ++i)
    {
        s[length - 1 - i] = (mask & 1) ? ((value & 1) ? '1' : '0') : '-';
        value >>= 1;
        mask >>= 1;
    }
    s[length] = '\0';

    return s;
}

/** @} */