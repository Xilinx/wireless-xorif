/*
 * Copyright 2020 - 2022 Xilinx, Inc.
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
 * @file xocp.c
 * @author Steven Dickinson
 * @brief Source file for main libxocp functions/definitions.
 * @addtogroup libxocp
 * @{
 */

#include "xocp.h"
#include "xocp_registers.h"
#include "xocp_utils.h"

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

#define FAKE_IO_SIZE 0x10000
#define SW_MAJOR_VER 1
#define SW_MINOR_VER 0
#define SW_REVISION 0

// Aliases to the ISR status and enable registers
#define ISR_ENABLE CFG_DL_CC_UPDATE_TRIGGERED_EN
#define ISR_STATUS CFG_DL_CC_UPDATE_TRIGGERED

/************************/
/*** Data definitions ***/
/************************/

bool xocp_first_time_flag = false;
int xocp_trace = 0;
xocp_state_t xocp_state[XOCP_NUM_INSTANCES];

#ifndef HW
//static const char *compatible = "xlnx,o-pxxch-1.0";
static const char *compatible = "xlnx,xorif-chan-proc-1.0";
#endif

/****************************/
/*** Function definitions ***/
/****************************/

/**
 * @brief Initializes the data for an instance of the driver.
 * @param ptr Pointer to the instance data
 */
static void initialize_instance(xocp_state_t *ptr)
{
    ASSERT_V(ptr);

    // Perform any initialization here...
    ptr->callback = NULL;
    ptr->events = 0;

    // Initialize component carrier configuration
    for (int i = 0; i < XOCP_MAX_NUM_CC; ++i)
    {
        ptr->cc_cfg[i].enable = 0;
        ptr->cc_cfg[i].num_rbs = 0;
        ptr->cc_cfg[i].numerology = 0;
        ptr->cc_cfg[i].ccid = 0;
        ptr->cc_cfg[i].inter_sym_gap = 0;
    }

    // Initialize antenna configuration
    // Defaults to 2 AGs: (0, 1, 2, 3) and (4, 5, 6, 7)
    ptr->antenna_cfg.num_antennas = 8;
    ptr->antenna_cfg.interleave = 2;
    for (int i = 0; i < XOCP_MAX_ANTENNA; ++i)
    {
        ptr->antenna_cfg.data[i] = i;
    }

    // Initialize the "prime CC" (lowest numerology component carrier)
    ptr->prime_cc = 0;

#ifdef NO_HW
    // Allocate fake register bank memory
    if (!ptr->io)
    {
        ptr->io = calloc(FAKE_IO_SIZE / sizeof(uint32_t), sizeof(uint32_t));
    }

    // Write required default values
    WR_REG(ptr->instance, CFG_MAJOR_REVISION, 1);
    WR_REG(ptr->instance, CFG_MINOR_REVISION, 2);
    WR_REG(ptr->instance, CFG_VERSION_REVISION, 3);
    WR_REG(ptr->instance, CFG_INTERNAL_REVISION, 4);
    WR_REG(ptr->instance, CFG_MAX_NUM_CC, 8);
    WR_REG(ptr->instance, CFG_MAX_NUM_ANTENNA, 8);
    WR_REG(ptr->instance, CFG_MAX_ANTENNA_INTERLEAVE, 4);
#endif

    // Read capabilities
    ptr->caps.max_num_cc = RD_REG(ptr->instance, CFG_MAX_NUM_CC);
    ptr->caps.max_num_antenna = RD_REG(ptr->instance, CFG_MAX_NUM_ANTENNA);
    ptr->caps.max_interleave = RD_REG(ptr->instance, CFG_MAX_ANTENNA_INTERLEAVE);
}

/**
 * @brief Program a single entry of the schedule table.
 * @param instance Device instance ID (0..N)
 * @param dl Downlink if true, uplink if false
 * @param offset Memory offset in the schedule table
 * @param cc Component carrier (O-RAN side)
 * @param ccid Componeny carrier ID (DFE side)
 * @param start_re The starting RE number
 * @param num_res The number of REs
 * @param antennas Bit-map containing the antennas for the antenna group
 * @param inter_sym_gap Inter-symbol gap (in IP core cycles)
 * @param last Flag indicating last section for the symbol
 * @param final Flag indicating the symbol
 */
void program_schedule_entry(int instance, bool dl, uint32_t offset,
                            uint8_t cc, uint8_t ccid,
                            uint16_t start_re, uint16_t num_res,
                            uint32_t antennas, uint16_t inter_sym_gap,
                            bool last, bool final)
{
    // Each entry consists of 3 words
    uint32_t word1 = (num_res & 0xFFF) | (start_re & 0xFFF) << 16;
    uint32_t word2 = (ccid & 0xF) | (cc & 0xF) << 8 |
                     ((dl ? inter_sym_gap : 0) & 0xFFF) << 16 |
                     (final ? 1U : 0) << 30 | (last ? 1U : 0) << 31;
    uint32_t word3 = antennas;

#if 0
    INFO("num_res = %d, start_re = %d\n", num_res, start_re);
    INFO("ccid = %d, cc = %d, final = %d, last = %d\n", ccid, cc, final, last);
    INFO("antennas = 0x%08X\n", antennas);
#endif

    if (dl)
    {
        write_reg(DEV(instance), "SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_A",
                  SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_NUM_RES_ADDR + offset,
                  0xFFFFFFFF, 0, 32, word1);
        write_reg(DEV(instance), "SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_B",
                  SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_CC_ID_ADDR + offset,
                  0xFFFFFFFF, 0, 32, word2);
        write_reg(DEV(instance), "SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_C",
                  SEQUENCE_TAB_NEXT_DL_CC_CONFIGURATION_ANT_CONFIG_ADDR + offset,
                  0xFFFFFFFF, 0, 32, word3);
    }
    else
    {
        write_reg(DEV(instance), "SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_A",
                  SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_NUM_RES_ADDR + offset,
                  0xFFFFFFFF, 0, 32, word1);
        write_reg(DEV(instance), "SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_B",
                  SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_CC_ID_ADDR + offset,
                  0xFFFFFFFF, 0, 32, word2);
        write_reg(DEV(instance), "SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_C",
                  SEQUENCE_TAB_NEXT_UL_CC_CONFIGURATION_ANT_CONFIG_ADDR + offset,
                  0xFFFFFFFF, 0, 32, word3);
    }
}

/**
 * @brief Program the next schedule for DL or UL.
 * @param instance Device instance ID (0..N)
 * @param dl Downlink if true, uplink if false
 * @param length Length of sequence
 * @param sequence Pointer to array defining the symbol sequence
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
static int program_schedule(int instance,
                            bool dl,
                            uint8_t length,
                            const uint8_t sequence[])
{
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV((length == 0) || sequence, XOCP_SCHEDULING_ERROR);

    if (length == 0)
    {
        // TODO - need some way to indicate nothing scheduled
        program_schedule_entry(instance, dl, 0, 0, 0, 0, 0, 0, 0, true, true);
        return XOCP_SUCCESS;
    }

    // Number of antennas per antenna group
    uint16_t num_ant = ptr->antenna_cfg.num_antennas / ptr->antenna_cfg.interleave;

    // Loop over the sequence
    uint16_t entries = 0;
    uint16_t offset = 0;
    for (int i = 0; i < length; ++i)
    {
        // Get the component carrier number and CCID
        uint8_t cc = sequence[i];
        uint8_t ccid = ptr->cc_cfg[cc].ccid;

        // Is this the last one?
        bool final = (i == (length - 1));

        // Compute antenna list (4-bits per antenna)
        uint32_t antennas = 0;
        for (int j = 0; j < num_ant; ++j)
        {
            uint8_t ant = ptr->antenna_cfg.data[j];
            antennas |= (ant & 0xF) << (j * 4);
        }

        // Inter-symbol gap
        uint16_t inter_sym_gap = ptr->cc_cfg[cc].inter_sym_gap;

        // REs are processed in 2 rounds, starting from the mid-point
        uint16_t num_res = ptr->cc_cfg[cc].num_rbs * RE_PER_RB;
        uint16_t re_mid = num_res / 2;

        // 1st round: N/2..N-1
        INFO("cc[%d] re[%d..%d] ant(0x%08X)\n",
                cc, re_mid, num_res - 1, antennas);
        program_schedule_entry(instance, dl, offset, cc, ccid,
                               re_mid, num_res / 2,
                               antennas, inter_sym_gap,
                               false, false);
        ++entries;
        ASSERT_NV(entries < MAX_SCHEDULE_LENGTH, XOCP_SCHEDULING_ERROR);
        offset += (4 * 4); // 4 words allocated per entry

        // 2nd rounds: 0..N/2-1
        INFO("cc[%d] re[%d..%d] ant(0x%08X)\n",
                cc, 0, re_mid - 1, antennas);
        program_schedule_entry(instance, dl, offset, cc, ccid,
                               0, num_res / 2,
                               antennas, inter_sym_gap,
                               true, final);
        ++entries;
        ASSERT_NV(entries < MAX_SCHEDULE_LENGTH, XOCP_SCHEDULING_ERROR);
        offset += (4 * 4); // 4 words allocated per entry
    }

    return XOCP_SUCCESS;
}

/**
 * @brief Interrupt handler for the device.
 * @param id IRQ identity
 * @param data Driver data
 * @returns
 *      - METAL_IRQ_HANDLED if the interrupt was handled
 *      - METAL_IRQ_NOT_HANDLED if the interrupt was not handled
 */
int xocp_irq_handler(int id, void *data)
{
    xocp_state_t *ptr = (xocp_state_t *)data;

    if (ptr)
    {
        TRACE("xocp_irq_handler(%d)\n", ptr->instance);

        // Read and store the latest event status
        uint32_t isr_status = RD_REG_ALT(ptr->instance, ISR_STATUS);
        ptr->events |= isr_status;

        INFO("ISR status = 0x%X\n", isr_status);

        if (ptr->callback)
        {
            ptr->callback(ptr->instance, isr_status);
        }

        // Clear all interrupts by toggling master enable
        WR_REG(ptr->instance, CFG_MASTER_INT_ENABLE, 0);
        WR_REG(ptr->instance, CFG_MASTER_INT_ENABLE, 1);

        return METAL_IRQ_HANDLED;
    }

    return METAL_IRQ_NOT_HANDLED;
}

void xocp_debug(uint16_t level)
{
    TRACE("xocp_debug(%d)\n", level);
    xocp_trace = level;
}

int xocp_start(const char *dev_name)
{
    TRACE("xocp_start(%s)\n", dev_name ? dev_name : "NULL");

    xocp_state_t *ptr = NULL;
    int instance;

    // Any one-time initialization?
    if (!xocp_first_time_flag)
    {
        xocp_first_time_flag = true;

        // Ensure all driver instances are IDLE
        for (int i = 0; i < XOCP_NUM_INSTANCES; ++i)
        {
            xocp_state[i].instance = i;
            xocp_state[i].state = XOCP_IDLE;
        }

#ifndef NO_HW
        // Only do metal_init() if not already initialized!
        // Note, next line is not fully portable
        extern struct metal_state _metal;
        if (_metal.pagemap_fd == 0)
        {
            // Parameters define log handlers and log level for libmetal
            struct metal_init_params init_param =
                {
                    .log_handler = metal_default_log_handler,
                    .log_level = METAL_LOG_ERROR,
                };

            // Initialize libmetal
            INFO("Initializing libmetal framework\n");
            if (metal_init(&init_param))
            {
                PERROR("Failed to initialize libmetal framework\n");
                return XOCP_LIBMETAL_ERROR;
            }
        }
#endif
    }

    // Get best-match device name
    dev_name = get_device_name(dev_name, compatible);
    if (!dev_name)
    {
        PERROR("No compatible device found\n");
        return XOCP_NO_SUCH_DEVICE;
    }

    // Look for an existing driver instance
    for (instance = 0; instance < XOCP_NUM_INSTANCES; ++instance)
    {
        if (xocp_state[instance].state != XOCP_IDLE)
        {
#ifndef NO_HW
            if (strcmp(dev_name, xocp_state[instance].dev->name) == 0)
#else
            if (strcmp(dev_name, xocp_state[instance].name) == 0)
#endif
            {
                INFO("Driver instance already exists (%d)\n", instance);
                ptr = &xocp_state[instance];
                break;
            }
        }
    }

    // Make new driver instance
    if ((instance == XOCP_NUM_INSTANCES) || !ptr)
    {
        // Find a free instance
        for (instance = 0; instance < XOCP_NUM_INSTANCES; ++instance)
        {
            if (xocp_state[instance].state == XOCP_IDLE)
            {
                ptr = &xocp_state[instance];
                break;
            }
        }
        if (instance == XOCP_NUM_INSTANCES)
        {
            PERROR("No free driver instances available\n");
            return XOCP_NO_RESOURCES;
        }

#ifndef NO_HW
        // Register the device with libmetal...
        INFO("Opening device '%s'\n", dev_name);
        if (metal_device_open("platform", dev_name, &(ptr->dev)))
        {
            PERROR("Failed to open device '%s'\n", dev_name);
            return XOCP_LIBMETAL_ERROR;
        }

        // Map the device IO region
        ptr->io = metal_device_io_region(ptr->dev, 0);
        if (!ptr->io)
        {
            PERROR("Failed to map IO region for device '%s'\n", dev_name);
            return XOCP_LIBMETAL_ERROR;
        }
        INFO("Mapped IO region for device '%s' (0x%lX)\n", dev_name, metal_io_phys(ptr->io, 0));

        // Register ISR
        if (ptr->dev->irq_num > 0)
        {
            int irq = (intptr_t)ptr->dev->irq_info;
            if (irq != -1)
            {
                metal_irq_register(irq, xocp_irq_handler, ptr);
                metal_irq_enable(irq);
                INFO("Device IRQ registered (%d)\n", irq);
            }
        }

#else

        INFO("Opening fake device '%s'\n", dev_name);

        // Keep copy of the fake device name
        strncpy(ptr->name, dev_name, MAX_NAME_LENGTH);
#endif
    }

    // Initialize the driver instance
    initialize_instance(ptr);

    // Assert the soft reset
    WR_REG(instance, FRAM_CHANP_UL_DISABLE, 1);
    WR_REG(instance, FRAM_CHANP_DL_DISABLE, 1);
    ptr->state = XOCP_RESET;

    return XOCP_SUCCESS;
}

void xocp_finish(uint16_t instance)
{
    TRACE("xocp_finish(%d)\n", instance);
    ASSERT_V(instance < XOCP_NUM_INSTANCES);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_V(ptr->state != XOCP_IDLE);

#ifndef NO_HW
    // Deregister the device from libmetal...
    if (ptr->dev)
    {
        INFO("Closing device '%s'\n", ptr->dev->name);
        int irq = (intptr_t)ptr->dev->irq_info;
        if (irq != -1)
        {
            metal_irq_disable(irq);
            metal_irq_unregister(irq);
            INFO("Device IRQ de-registered\n");
        }
        metal_device_close(ptr->dev);
        ptr->dev = NULL;
    }
    ptr->io = NULL;

#else

    INFO("Closing fake device '%s'\n", ptr->name);

    // Free fake register bank memory
    if (ptr->io)
    {
        free(ptr->io);
        ptr->io = NULL;
    }

    // Clear the fake device name
    ptr->name[0] = '\0';
#endif

    // Back to idle
    ptr->callback = NULL;
    ptr->state = XOCP_IDLE;
}

int xocp_get_state(uint16_t instance)
{
    TRACE("xocp_get_state(%d)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];

    return ptr->state;
}

uint32_t xocp_get_sw_version(void)
{
    TRACE("xocp_get_sw_version()\n");

    return (SW_MAJOR_VER << 24) | (SW_MINOR_VER << 16) | (SW_REVISION << 8);
}

uint32_t xocp_get_hw_version(uint16_t instance)
{
    TRACE("xocp_get_hw_version(%d)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);

    uint8_t major = RD_REG(instance, CFG_MAJOR_REVISION);
    uint8_t minor = RD_REG(instance, CFG_MINOR_REVISION);
    uint8_t revision = RD_REG(instance, CFG_VERSION_REVISION);

    return (major << 24) | (minor << 16) | (revision << 8);
}

uint32_t xocp_get_hw_internal_rev(uint16_t instance)
{
    TRACE("xocp_get_hw_version(%d)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);

    return RD_REG(instance, CFG_INTERNAL_REVISION);
}

int xocp_get_capabilities(uint16_t instance,
                          struct xocp_caps *data)
{
    TRACE("xocp_get_capabilities(%d, ...)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);
    ASSERT_NV(data, XOCP_NULL_POINTER);

    memcpy(data, &ptr->caps, sizeof(struct xocp_caps));

    return XOCP_SUCCESS;
}

int xocp_get_event_status(uint16_t instance, uint32_t *status)
{
    TRACE("xocp_get_event_status(%d, ...)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);
    ASSERT_NV(status, XOCP_NULL_POINTER);

    // Read and store the latest event status
    uint32_t isr_status = RD_REG_ALT(ptr->instance, ISR_STATUS);
    ptr->events |= isr_status;
    *status = ptr->events;

    return XOCP_SUCCESS;
}

int xocp_clear_event_status(uint16_t instance)
{
    TRACE("xocp_clear_event_status(%d)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);

    // Clear all interrupts by toggling master enable
    WR_REG(ptr->instance, CFG_MASTER_INT_ENABLE, 0);
    WR_REG(ptr->instance, CFG_MASTER_INT_ENABLE, 1);

    // Clear stored events
    ptr->events = 0;

    return XOCP_SUCCESS;
}

int xocp_enable_interrupts(uint16_t instance, uint32_t mask)
{
    TRACE("xocp_enable_interrupts(%d, 0x%X)\n", instance, mask);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);

    // Write the new interrupt enable mask
    WR_REG_ALT(ptr->instance, CFG_DL_CC_UPDATE_TRIGGERED, 0);

    return XOCP_SUCCESS;
}

int xocp_register_event_callback(uint16_t instance,
                                 xocp_isr_func_t callback)
{
    TRACE("xocp_register_event_callback(%d, ...)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);

    ptr->callback = callback;

    return XOCP_SUCCESS;
}

int xocp_read_reg(uint16_t instance, const char *name, uint32_t *value)
{
    TRACE("xocp_read_reg(%d, %s, ...)\n", instance, name);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);
    ASSERT_NV(value, XOCP_NULL_POINTER);

    const reg_info_t *reg = find_register(name);
    if (!reg)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return XOCP_REGISTER_NOT_FOUND;
    }
    else
    {
        *value = read_reg(DEV(instance),
                          name,
                          reg->addr,
                          reg->mask,
                          reg->shift,
                          reg->width);
        return XOCP_SUCCESS;
    }
}

int xocp_read_reg_offset(uint16_t instance,
                         const char *name,
                         uint16_t offset,
                         uint32_t *value)
{
    TRACE("xocp_read_reg_offset(%d, %s, %d, ...)\n", instance, name, offset);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);
    ASSERT_NV(value, XOCP_NULL_POINTER);

    const reg_info_t *reg = find_register(name);
    if (!reg)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return XOCP_REGISTER_NOT_FOUND;
    }
    else
    {
        *value = read_reg(DEV(instance),
                          name,
                          (reg->addr + offset),
                          reg->mask,
                          reg->shift,
                          reg->width);
        return XOCP_SUCCESS;
    }
}

int xocp_write_reg(uint16_t instance, const char *name, uint32_t value)
{
    TRACE("xocp_write_reg(%d, %s, 0x%X)\n", instance, name, value);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);

    const reg_info_t *reg = find_register(name);
    if (!reg)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return XOCP_REGISTER_NOT_FOUND;
    }
    else
    {
        write_reg(DEV(instance),
                  name,
                  reg->addr,
                  reg->mask,
                  reg->shift,
                  reg->width,
                  value);
        return XOCP_SUCCESS;
    }
}

int xocp_write_reg_offset(uint16_t instance,
                             const char *name,
                             uint16_t offset,
                             uint32_t value)
{
    TRACE("xocp_write_reg_offset(%d, %s, 0x%X, 0x%X)\n", instance, name, offset, value);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);

    const reg_info_t *reg = find_register(name);
    if (!reg)
    {
        // The register could not be found
        PERROR("Register '%s' not found\n", name);
        return XOCP_REGISTER_NOT_FOUND;
    }
    else
    {
        write_reg(DEV(instance),
                  name,
                  (reg->addr + offset),
                  reg->mask,
                  reg->shift,
                  reg->width,
                  value);
        return XOCP_SUCCESS;
    }
}

int xocp_reset(uint16_t instance, uint8_t mode)
{
    TRACE("xocp_reset(%d, %d)\n", instance, mode);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);

    // Assert the soft reset
    WR_REG(instance, FRAM_CHANP_DL_DISABLE, 1);
    WR_REG(instance, FRAM_CHANP_UL_DISABLE, 1);

    if (mode == 0)
    {
        // Release the soft reset, and move to ready state
        WR_REG(instance, FRAM_CHANP_DL_DISABLE, 0);
        WR_REG(instance, FRAM_CHANP_UL_DISABLE, 0);
        ptr->state = XOCP_READY;
    }
    else
    {
        // Stay in reset state
        ptr->state = XOCP_RESET;
    }

    return XOCP_SUCCESS;
}

int xocp_activate(uint16_t instance)
{
    TRACE("xocp_activate(%d)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state == XOCP_READY, XOCP_INVALID_STATE);

    ptr->state = XOCP_OPERATIONAL;
    return XOCP_SUCCESS;
}

int xocp_get_cc_cfg(uint16_t instance,
                    uint8_t cc,
                    struct xocp_cc_data *data)
{
    TRACE("xocp_get_cc_cfg(%d, %d, ...)\n", instance, cc);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);
    ASSERT_NV(cc < ptr->caps.max_num_cc, XOCP_INVALID_CC);
    ASSERT_NV(data, XOCP_NULL_POINTER);

    // Copy the configuration
    memcpy(data, &ptr->cc_cfg[cc], sizeof(struct xocp_cc_data));

    return XOCP_SUCCESS;
}

int xocp_set_cc_cfg(uint16_t instance,
                    uint8_t cc,
                    const struct xocp_cc_data *data)
{
    TRACE("xocp_set_cc_cfg(%d, %d, ...)\n", instance, cc);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);
    ASSERT_NV(cc < ptr->caps.max_num_cc, XOCP_INVALID_CC);
    ASSERT_NV(data, XOCP_NULL_POINTER);
    ASSERT_NV(data->num_rbs <= MAX_RBS, XOCP_INVALID_RBS);
    ASSERT_NV(data->numerology <= MAX_NUMEROLOGY, XOCP_INVALID_NUMEROLOGY);

    // Save the configuration
    memcpy(&ptr->cc_cfg[cc], data, sizeof(struct xocp_cc_data));

    // Program the registers
    uint16_t num_res = ptr->cc_cfg[cc].num_rbs * RE_PER_RB;
    WR_REG_OFFSET(instance, CTRL_NEXT_DL_CC_NUMBER_OF_RES, 0x10 * cc, num_res);
    WR_REG_OFFSET(instance, CTRL_NEXT_UL_CC_NUMBER_OF_RES, 0x10 * cc, num_res);

    return XOCP_SUCCESS;
}

int xocp_get_antenna_cfg(uint16_t instance,
                         struct xocp_antenna_data *data)
{
    TRACE("xocp_get_antenna_cfg(%d, ...)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);
    ASSERT_NV(data, XOCP_NULL_POINTER);

    // Copy the configuration
    memcpy(data, &ptr->antenna_cfg, sizeof(struct xocp_antenna_data));

    return XOCP_SUCCESS;
}

int xocp_set_antenna_cfg(uint16_t instance,
                         const struct xocp_antenna_data *data)
{
    TRACE("xocp_set_antenna_cfg(%d, ...)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);
    ASSERT_NV(data, XOCP_NULL_POINTER);
    ASSERT_NV(data->num_antennas <= ptr->caps.max_num_antenna, XOCP_INVALID_ANTENNAS);
    ASSERT_NV(data->interleave <= ptr->caps.max_interleave, XOCP_INVALID_ANTENNAS);

    // Save the configuration
    memcpy(&ptr->antenna_cfg, data, sizeof(struct xocp_antenna_data));

    // Program the registers
    WR_REG(instance, CTRL_DL_NUMBER_OF_ANTENNAS, ptr->antenna_cfg.num_antennas);
    WR_REG(instance, CTRL_DL_ANTENNA_INTERLEAVE, ptr->antenna_cfg.interleave);
    WR_REG(instance, CTRL_UL_NUMBER_OF_ANTENNAS, ptr->antenna_cfg.num_antennas);
    WR_REG(instance, CTRL_UL_ANTENNA_INTERLEAVE, ptr->antenna_cfg.interleave);

    return XOCP_SUCCESS;
}

int xocp_set_schedule(uint16_t instance,
                      uint8_t mode,
                      uint8_t length,
                      const uint8_t sequence[])
{
    TRACE("xocp_set_schedule(%d, %d, %d, ...)\n", instance, mode, length);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state == XOCP_OPERATIONAL, XOCP_INVALID_STATE);
    int result;

    // Find the lowest numerology symbol (save as "prime CC")
    uint8_t min_u = MAX_NUMEROLOGY;
    for (int i = 0; i < length; ++i)
    {
        uint8_t cc = sequence[i];
        if (ptr->cc_cfg[cc].numerology < min_u)
        {
            min_u = ptr->cc_cfg[cc].numerology;
            ptr->prime_cc = cc;
        }
    }

    if (mode & 0x01)
    {
        // Schedule DL
        INFO("Scheduling DL...\n");
        result = program_schedule(instance, true, length, sequence);
        if (result != XOCP_SUCCESS)
        {
            return result;
        }
    }

    if (mode & 0x02)
    {
        // Schedule UL
        INFO("Scheduling UL...\n");
        result = program_schedule(instance, false, length, sequence);
        if (result != XOCP_SUCCESS)
        {
            return result;
        }
    }

    return XOCP_SUCCESS;
}

int xocp_get_trigger_cfg(uint16_t instance,
                         struct xocp_triggers *triggers)
{
    TRACE("xocp_get_trigger_cfg(%d, ...)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);
    ASSERT_NV(triggers, XOCP_NULL_POINTER);

    // Get the "update" trigger configuration
    triggers->dl_update.enable = 0;
    triggers->dl_update.mode = RD_REG(instance, CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE);
    triggers->dl_update.tuser_edge_level = 0; // TODO
    triggers->dl_update.tuser_bit = 0; // TODO

    // Get the UL "update" trigger configuration
    triggers->ul_update.enable = 0;
    triggers->ul_update.mode = RD_REG(instance, CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE);
    triggers->ul_update.tuser_edge_level = 0; // TODO
    triggers->ul_update.tuser_bit = 0; // TODO

    return XOCP_SUCCESS;
}

int xocp_set_trigger_cfg(uint16_t instance,
                            const struct xocp_triggers *triggers)
{
    TRACE("xocp_set_trigger_cfg(%d, ...)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);
    ASSERT_NV(triggers, XOCP_NULL_POINTER);

    // Set the DL "update" trigger configuration
    WR_REG(instance, CTRL_DL_SEQUENCE_TABLE_UPDATE_MODE, triggers->dl_update.mode);
    
    // Set the UL "update" trigger configuration
    WR_REG(instance, CTRL_UL_SEQUENCE_TABLE_UPDATE_MODE, triggers->ul_update.mode);

    return XOCP_SUCCESS;
}

int xocp_trigger_update(uint16_t instance)
{
    TRACE("xocp_trigger_update(%d)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state == XOCP_OPERATIONAL, XOCP_INVALID_STATE);

    // Use the "prime CC" for the update trigger
    WR_REG(instance, CTRL_DL_SEQUENCE_TABLE_CC_SEL, ptr->prime_cc);
    WR_REG(instance, CTRL_UL_SEQUENCE_TABLE_CC_SEL, ptr->prime_cc);

    // Enable the triggers - TODO check this
    WR_REG(instance, CTRL_DL_SEQUENCE_TABLE_ENABLE, 1);
    WR_REG(instance, CTRL_UL_SEQUENCE_TABLE_ENABLE, 1);

    // Arm the triggers - TODO check this
    WR_REG(instance, CTRL_DL_SEQUENCE_TABLE_WR_STROBE, 1);
    WR_REG(instance, CTRL_UL_SEQUENCE_TABLE_WR_STROBE, 1);

    return XOCP_SUCCESS;
}

int xocp_monitor_clear(uint16_t instance)
{
    TRACE("xocp_monitor_clear(%d)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);

    WR_REG(instance, CFG_MONITOR_CLEAR, 1);

    return XOCP_SUCCESS;
}

int xocp_monitor_snapshot(uint16_t instance)
{
    TRACE("xocp_monitor_snapshot(%d)\n", instance);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);

    WR_REG(instance, CFG_MONITOR_SNAPSHOT, 1);

    return XOCP_SUCCESS;
}

int xocp_monitor_read(uint16_t instance, uint8_t counter, uint64_t *value)
{
    TRACE("xocp_monitor_read(%d, %d, ...)\n", instance, counter);
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];
    ASSERT_NV(ptr->state != XOCP_IDLE, XOCP_INVALID_STATE);
    ASSERT_NV(value, XOCP_NULL_POINTER);

    WR_REG(instance, CFG_MONITOR_SELECT_READ, counter);
    WR_REG(instance, CFG_MONITOR_SAMPLE, 1);
    uint64_t temp = (uint64_t)RD_REG(instance, CFG_MONITOR_READ_31__0) |
                    (uint64_t)RD_REG(instance, CFG_MONITOR_READ_63_32) << 32;
    *value = temp;

    return XOCP_SUCCESS;
}

#ifdef EXTRA_DEBUG
/**
 * @brief Inject error for testing purposes.
 * @param instance Device instance ID (0..N)
 * @param status Interrupt status bits
 * @return
 *      - METAL_IRQ_HANDLED
 *      - METAL_IRQ_NOT_HANDLED
 * @note
 * This function is for testing only, and is not exposed in the
 * API header file.
 */
int xocp_test_error_injections(uint16_t instance, uint32_t status)
{
#ifdef NO_HW
    ASSERT_NV(instance < XOCP_NUM_INSTANCES, XOCP_INVALID_INSTANCE);
    xocp_state_t *ptr = &xocp_state[instance];

    WR_REG_ALT(ptr->instance, ISR_STATUS, status);
    int result = xocp_irq_handler(99, (void *)ptr);
    WR_REG_ALT(ptr->instance, ISR_STATUS, 0);
    return result;
#else
    return XOCP_NOT_SUPPORTED;
#endif
}
#endif // EXTRA_DEBUG

/** @} */