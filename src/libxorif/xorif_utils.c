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
#include "xorif_api.h"
#include "xorif_fh_func.h"
#include "xorif_common.h"
#include "xorif_utils.h"

const reg_info_t *find_register(const reg_info_t *ptr, const char *name)
{
    // Note, this basic implementation performs linear search
    // TODO change to binary search for performance
    while (ptr->name)
    {
        if (strcmp(name, ptr->name) == 0)
        {
            return ptr;
        }
        ++ptr;
    }
    return NULL;
}

uint32_t read_reg_raw(struct metal_io_region *io, uint32_t addr, uint32_t mask, uint16_t shift)
{
    ASSERT(io);
    if (!io)
    {
        // The device and/or IO region is not active, return 0
        PERROR("Libmetal device and/or IO region is not initialized\n");
        return 0;
    }

    uint32_t value = (metal_io_read32(io, addr) & mask) >> shift;
    return value;
}

void write_reg_raw(struct metal_io_region *io, uint32_t addr, uint32_t mask, uint16_t shift, uint32_t value)
{
    ASSERT(io);
    if (!io)
    {
        // The device and/or IO region is not active, return
        PERROR("Libmetal device and/or IO region is not initialized\n");
        return;
    }

    uint32_t x = metal_io_read32(io, addr);
    x &= ~mask;
    x |= (value << shift) & mask;
    metal_io_write32(io, addr, x);
}

uint32_t read_reg(struct metal_io_region *io, const char *name, uint32_t addr, uint32_t mask, uint16_t shift)
{
    uint32_t value = read_reg_raw(io, addr, mask, shift);
    TRACE("READ_REG: %s [0x%x] => 0x%x (%d)\n", name, addr, value, value);
    return value;
}

uint32_t read_reg_offset(struct metal_io_region *io, const char *name, uint32_t addr, uint16_t offset, uint32_t mask, uint16_t shift)
{
    uint32_t value = read_reg_raw(io, addr + offset, mask, shift);
    TRACE("READ_REG_OFFSET: %s [0x%x] => 0x%x (%d)\n", name, addr + offset, value, value);
    return value;
}

void write_reg(struct metal_io_region *io, const char *name, uint32_t addr, uint32_t mask, uint16_t shift, uint32_t value)
{
    write_reg_raw(io, addr, mask, shift, value);
    TRACE("WRITE_REG: %s [0x%x] <= 0x%x (%d)\n", name, addr, value, value);
}

void write_reg_offset(struct metal_io_region *io, const char *name, uint32_t addr, uint16_t offset, uint32_t mask, uint16_t shift, uint32_t value)
{
    write_reg_raw(io, addr + offset, mask, shift, value);
    TRACE("WRITE_REG_OFFSET: %s [0x%x] <= 0x%x (%d)\n", name, addr + offset, value, value);
}

int get_device_name(const char *path, const char *short_name, char *full_name, int max_chars)
{
    DIR *folder;
    struct dirent *entry;
    int result = 0;

    // Open directory
    folder = opendir(path);
    if (folder == NULL)
    {
        PERROR("Unable to read directory %s\n", path);
        return 0;
    }

    // Iterate through files in directory
    while ((entry = readdir(folder)))
    {
        // Check for matching short name
        char *s = strstr(entry->d_name, short_name);
        if (s != NULL && strlen(s) == strlen(short_name))
        {
            // Match found, copy the full device name
            strncpy(full_name, entry->d_name, max_chars);
            result = 1;
            break;
        }
    }

    // Close the directory
    closedir(folder);

    return result;
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
    TRACE("Opening device '%s'\n", dev_name);
    if (metal_device_open(bus_name, dev_name, &(device->dev)))
    {
        PERROR("Failed to open device '%s'\n", dev_name);
        return XORIF_FAILURE;
    }

    // Map the device to IO region
    TRACE("Mapping IO region for device '%s'\n", dev_name);
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

int check_numerology(uint16_t numerology)
{
    return (caps.numerologies & (1 << numerology));
}

int check_iq_comp_mode(enum xorif_iq_comp comp_meth)
{
    return (caps.iq_comp_methods & (1 << comp_meth));
}

int check_bw_comp_mode(enum xorif_bw_comp comp_meth)
{
    return (caps.bw_comp_methods & (1 << comp_meth));
}

/** @} */