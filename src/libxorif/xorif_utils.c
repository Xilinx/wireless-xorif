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
 * @file xorif_utils.c
 * @author Steven Dickinson
 * @brief Source file for libxorif common utility functions.
 * @addtogroup libxorif
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <endian.h>
#include "xorif_api.h"
#include "xorif_system.h"
#include "xorif_common.h"
#include "xorif_fh_func.h"
#include "xorif_utils.h"

// Structure to hold memory allocation info
struct block
{
    uint16_t offset;    // Start offset of allocated memory
    uint16_t size;      // Size of allocated memory
    int tag;            // Tag: -1 = free; >= 0 user identifier (e.g. CC#)
    struct block *next; // Pointer to next block
};

// Typedef for memory allocation info structure
typedef struct block block_t;

// Tag value for free blocks
#define FREE (-1)

// Fake base address for debug "devmem"
#define FAKE_BASE_ADDR 0xA0000000

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

uint32_t read_reg_raw(void *io, const char *name, uint32_t addr)
{
#ifndef NO_HW
    ASSERT(io);
    if (!io)
    {
        // The device and/or IO region is not active, return 0
        PERROR("Libmetal device and/or IO region is not initialized\n");
        return 0;
    }

    // Libmetal read
    uint32_t value = metal_io_read32((struct metal_io_region *)io, addr);
#else
    // Fake read
    uint32_t value = ((uint32_t *)io)[addr / 4];
#endif

    TRACE("READ_REG: %s (0x%04X) => 0x%X (%u)\n", name, addr, value, value);

#ifdef EXTRA_DEBUG
    if (xorif_trace == 3)
    {
        LOG(log_file, "sw_driver_read(32'h%08X, 32'h%08X); // %s\n", addr, value, name);
    }
    else if (xorif_trace == 4)
    {
        LOG(log_file, "devmem 0x%08X # %s\n", (FAKE_BASE_ADDR + addr), name);
    }
#endif
    return value;
}

void write_reg_raw(void *io, const char *name, uint32_t addr, uint32_t value)
{
#ifndef NO_HW
    ASSERT(io);
    if (!io)
    {
        // The device and/or IO region is not active, return
        PERROR("Libmetal device and/or IO region is not initialized\n");
        return;
    }

    // Libmetal write
    metal_io_write32((struct metal_io_region *)io, addr, value);
#else
    // Fake write
    ((uint32_t *)io)[addr / 4] = value;
#endif

    TRACE("WRITE_REG: %s (0x%04X) <= 0x%X (%u)\n", name, addr, value, value);

#ifdef EXTRA_DEBUG
    if (xorif_trace == 3)
    {
        LOG(log_file, "sw_driver_write(32'h%08X, 32'h%08X); // %s\n", addr, value, name);
    }
    else if (xorif_trace == 4)
    {
        LOG(log_file, "devmem 0x%08X 32 0x%08X # %s\n", (FAKE_BASE_ADDR + addr), value, name);
    }
#endif
}

uint32_t read_reg(void *io, const char *name, uint32_t addr, uint32_t mask, uint16_t shift, uint16_t width)
{
#ifndef NO_HW
    ASSERT(io);
    if (!io)
    {
        // The device and/or IO region is not active, return
        PERROR("Libmetal device and/or IO region is not initialized\n");
        return 0;
    }

    // Libmetal read
    uint32_t x = metal_io_read32((struct metal_io_region *)io, addr);
#else
    // Fake read
    uint32_t x = ((uint32_t *)io)[addr / 4];
#endif
    uint32_t value = (x & mask) >> shift;

    TRACE("READ_REG: %s (0x%04X)[%d:%d] => 0x%X (%u)\n", name, addr, shift, shift + width - 1, value, value);

#ifdef EXTRA_DEBUG
    if (xorif_trace == 3)
    {
        LOG(log_file, "sw_driver_read(32'h%08X, 32'h%08X); // %s\n", addr, x, name);
    }
    else if (xorif_trace == 4)
    {
        LOG(log_file, "devmem 0x%08X # %s\n", (FAKE_BASE_ADDR + addr), name);
    }
#endif

    return value;
}

void write_reg(void *io, const char *name, uint32_t addr, uint32_t mask, uint16_t shift, uint16_t width, uint32_t value)
{
#ifndef NO_HW
    ASSERT(io);
    if (!io)
    {
        // The device and/or IO region is not active, return
        PERROR("Libmetal device and/or IO region is not initialized\n");
        return;
    }

    // Libmetal read / modify / write
    uint32_t x = metal_io_read32((struct metal_io_region *)io, addr);
    x &= ~mask;
    x |= (value << shift) & mask;
    metal_io_write32((struct metal_io_region *)io, addr, x);
#else
    // Fake read / modify / write
    uint32_t x = ((uint32_t *)io)[addr / 4];
    x &= ~mask;
    x |= (value << shift) & mask;
    ((uint32_t *)io)[addr / 4] = x;
#endif

    TRACE("WRITE_REG: %s (0x%04X)[%d:%d] <= 0x%X (%u)\n", name, addr, shift, shift + width - 1, value, value);

#ifdef EXTRA_DEBUG
    if (xorif_trace == 3)
    {
        LOG(log_file, "sw_driver_write(32'h%08X, 32'h%08X); // %s\n", addr, x, name);
    }
    else if (xorif_trace == 4)
    {
        LOG(log_file, "devmem 0x%08X 32 0x%08X # %s\n", (FAKE_BASE_ADDR + addr), value, name);
    }
#endif
}

#ifndef NO_HW
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
#endif

#ifndef NO_HW
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
#endif

#ifndef NO_HW
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
#endif

int check_numerology(uint16_t numerology, uint16_t extended_cp)
{
    if (extended_cp && (numerology != 2))
    {
        // Extended CP requested for numerology other than 2
        return 0;
    }
    else if (extended_cp && !fhi_caps.extended_cp)
    {
        // Extended CP requested when it's not supported
        return 0;
    }
    else
    {
        // Check requested numerology against support mask
        return ((1 << numerology) & fhi_caps.numerologies);
    }
}

int check_iq_comp_mode(uint16_t bit_width, enum xorif_iq_comp comp_method, enum xorif_chan_type chan)
{
    uint16_t methods;
    uint16_t bfp_widths;
    uint16_t mod_widths;

    // Get capabilities based on channel type
    if ((chan == CHAN_DL) || (chan == CHAN_SSB))
    {
        methods = fhi_caps.iq_de_comp_methods;
        bfp_widths = fhi_caps.iq_de_comp_bfp_widths;
        mod_widths = fhi_caps.iq_de_comp_mod_widths;
    }
    else if ((chan == CHAN_UL) || (chan == CHAN_PRACH))
    {
        methods = fhi_caps.iq_comp_methods;
        bfp_widths = fhi_caps.iq_comp_bfp_widths;
        mod_widths = 0;
    }
    else
    {
        methods = IQ_COMP_NONE_SUPPORT;
        bfp_widths = 0;
        mod_widths = 0;
    }

    if (bit_width > 16)
    {
        // Not supported
        return 0;
    }
    else if (bit_width == 16)
    {
        // A bit_width value of 16 is equivalent to 0
        bit_width = 0;
    }

    // Check configuration support
    switch (comp_method)
    {
    case IQ_COMP_NONE:
        return (methods & (1 << comp_method)) && (bit_width == 0);

    case IQ_COMP_BLOCK_FP:
        return (methods & (1 << comp_method)) && (bfp_widths & (1 << bit_width));

    case IQ_COMP_MODULATION:
        return (methods & (1 << comp_method)) && (mod_widths & (1 << bit_width));

    default:
        // All others are unsupported
        return 0;
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

void *init_memory_allocator(void **ptr, uint16_t offset, uint16_t size)
{
    block_t *p;

    // Free any existing blocks
    p = (block_t *)(*ptr);
    while (p != NULL)
    {
        block_t *next = p->next;
        free(p);
        p = next;
    }

    // Allocate a new block of the required size
    p = calloc(1, sizeof(block_t));
    if (p)
    {
        p->offset = offset;
        p->size = size;
        p->tag = FREE;
        p->next = NULL;
    }

    // Write back the pointer value
    *ptr = (void *)p;

    // Return the pointer value
    return (void *)p;
}

int alloc_block(void *ptr, uint16_t size, uint16_t tag)
{
    // Scan list of blocks
    block_t *p = (block_t *)ptr;
    while (p != NULL)
    {
        // Using "first fit" approach
        if ((p->tag == FREE) && (p->size >= size))
        {
            if (p->size == size)
            {
                // Free block is exact size
                p->tag = tag;
            }
            else
            {
                // Free block is larger, so split it into 'p' and 'q'
                // 'p' becomes the used block, and 'q' becomes the free block
                // with 'q' linked after 'p'
                block_t *q = calloc(1, sizeof(block_t));
                if (!q)
                {
                    // Unable to allocate!
                    return -1;
                }

                // Adjust 'p' & 'q' accordingly
                q->offset = p->offset + size;
                q->size = p->size - size;
                q->tag = FREE;
                q->next = p->next;
                p->size = size;
                p->tag = tag;
                p->next = q;
            }
            return p->offset;
        }
        p = p->next;
    }

    // Unable to find space!
    return -1;
}

void dealloc_block(void *ptr, uint16_t tag)
{
    block_t *p = (block_t *)ptr;
    block_t *prev = NULL;
    block_t *next;
    while (p != NULL)
    {
        next = p->next;

        if (p->tag == tag)
        {
            // Free block
            p->tag = FREE;

            if (next && next->tag == FREE)
            {
                // Next block is free, merge it into current
                p->size += next->size;
                p->next = next->next;
                free(next);
                next = p->next;
            }
            if (prev && prev->tag == FREE)
            {
                // Previous block was free, merge current into it
                prev->size += p->size;
                prev->next = p->next;
                free(p);
                p = prev;
            }
        }

        prev = p;
        p = next;
    }
}

int get_alloc_block(void *ptr, uint16_t tag, uint16_t *offset, uint16_t *size)
{
    block_t *p = (block_t *)ptr;

    while (p != NULL)
    {
        if (p->tag == tag)
        {
            *offset = p->offset;
            *size = p->size;
            return 1;
        }

        p = p->next;
    }

    *offset = 0;
    *size = 0;
    return 0;
}

/** @} */