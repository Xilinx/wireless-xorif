/*
 * Copyright 2020 - 2023 Advanced Micro Devices, Inc.
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

#include <dirent.h>
#include <endian.h>
#include "xorif_common.h"
#include "xorif_fh_func.h"
#include "xorif_utils.h"
#include "xorif_registers.h"

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

// Max string length for paths, etc.
#define MAX_PATH_LENGTH 256

#ifdef NO_HW
// Fake device
static const char *fake_device = "af800000.oran_radio_if";
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

int get_device_property_u32(const char *dev_name, const char *prop_name, uint32_t *value)
{
    FILE *fp;
    char buff[MAX_PATH_LENGTH];

    // Create filepath for property
    snprintf(buff, MAX_PATH_LENGTH, "/sys/bus/platform/devices/%s/of_node/%s", dev_name, prop_name);

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
    if (!dev_name || strstr(fake_device, dev_name))
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

int add_device(struct xorif_device_info *device,
               const char *bus_name,
               const char *dev_name,
               irq_handler_t irq_handler)
{
    // Initialize device info structure
    device->status = 0;
#ifndef NO_HW
    device->dev = NULL;
    device->io = NULL;

    // Open a device
    INFO("Opening device '%s'\n", dev_name);
    if (metal_device_open(bus_name, dev_name, &(device->dev)))
    {
        PERROR("Failed to open device '%s'\n", dev_name);
        return XORIF_FAILURE;
    }

    // Map the device to IO region
    device->io = metal_device_io_region(device->dev, 0);
    if (device->io != NULL)
    {
        INFO("Mapped IO region for device '%s' (0x%lX)\n", dev_name, metal_io_phys(device->io, 0));
    }
    else
    {
        PERROR("Failed to map IO region for device '%s'\n", dev_name);
        return XORIF_FAILURE;
    }

    if (irq_handler)
    {
        int num = device->dev->irq_num;
        if (num > 0)
        {
            int irq = (intptr_t)device->dev->irq_info;
            if (irq != -1)
            {
                metal_irq_register(irq, irq_handler, device);
                metal_irq_enable(irq);
                INFO("IRQ registered (%d)\n", irq);
            }
        }
    }
#endif

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
    uint16_t bfp_widths = 0;
    uint16_t mod_widths = 0;
    uint16_t nocomp_widths = 0xFFFF; // All widths supported!

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
    }
    else
    {
        methods = IQ_COMP_NONE_SUPPORT;
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
        return (methods & (1 << comp_method)) && (nocomp_widths & (1 << bit_width));

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
