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
 * @file xorif_common.c
 * @author Steven Dickinson
 * @brief Source file for libxorif common functions.
 * @addtogroup libxorif
 * @{
 *
 * "C" code for the Xilinx ORAN Radio Interface (libxorif)
 */

#include "xorif_common.h"
#include "xorif_fh_func.h"
#include "xorif_utils.h"
#include "xorif_registers.h"

// Globals variables
uint16_t xorif_state = 0;
int xorif_trace = 0;
struct xorif_caps fhi_caps;
struct xorif_cc_config cc_config[MAX_NUM_CC];
struct xorif_device_info fh_device;
#ifdef EXTRA_DEBUG
FILE *log_file = NULL;
#endif

// Local variables
static const char *compatible = "xlnx,oran-radio-if-3."; // Only checking major version

// System "constants" (can be changed with API)
struct xorif_system_constants fhi_sys_const =
{
    .FH_DECAP_DLY = DEFAULT_FH_DECAP_DLY,      // Downlink delay estimate (see PG370)
};

#ifndef NO_HW
/**
 * @brief Retrieve libmetal device info.
 * @param dev Pointer to write-back the libmetal device pointer
 * @param io Pointer to write-back the libmetal io pointer
 * @returns
 *      - XORIF_SUCCESS on success
 *      - Error code on failure
 * @note
 * This function must be called after the FHI API s/w has been initialized,
 * i.e. xorif_init() called successfully.
*/
int xorif_retrieve_device_info(struct metal_device **dev, struct metal_io_region **io)
{
    if (xorif_state)
    {
        *dev = fh_device.dev;
        *io = fh_device.io;
        return XORIF_SUCCESS;
    }
    else
    {
        return XORIF_LIBMETAL_ERROR;
    }
}
#endif

/**
 * @brief Initialize the configuration data.
 */
static void initialize_configuration(void)
{
    // Initialize the component carrier state and configuration
    memset(cc_config, 0, sizeof(cc_config));
    for (int i = 0; i < MAX_NUM_CC; ++i)
    {
        // Populate structure with defaults
        cc_config[i].num_rbs = 0;
        cc_config[i].numerology = 0;
        cc_config[i].extended_cp = 0;
        cc_config[i].num_rbs_ssb = 20;
        cc_config[i].numerology_ssb = 0;
        cc_config[i].extended_cp_ssb = 0;
        cc_config[i].iq_comp_meth_ul = IQ_COMP_NONE;
        cc_config[i].iq_comp_width_ul = 16;
        cc_config[i].iq_comp_mplane_ul = 1;
        cc_config[i].iq_comp_meth_dl = IQ_COMP_NONE;
        cc_config[i].iq_comp_width_dl = 16;
        cc_config[i].iq_comp_mplane_dl = 1;
        cc_config[i].iq_comp_meth_ssb = IQ_COMP_NONE;
        cc_config[i].iq_comp_width_ssb = 16;
        cc_config[i].iq_comp_mplane_ssb = 1;
        cc_config[i].iq_comp_meth_prach = IQ_COMP_NONE;
        cc_config[i].iq_comp_width_prach = 16;
        cc_config[i].iq_comp_mplane_prach = 1;
        cc_config[i].delay_comp_cp_ul = DEFAULT_DELAY_COMP;
        cc_config[i].delay_comp_cp_dl = DEFAULT_DELAY_COMP;
        cc_config[i].delay_comp_up = DEFAULT_DELAY_COMP;
        cc_config[i].advance_ul = DEFAULT_ADVANCE_UL;
        cc_config[i].advance_dl = DEFAULT_ADVANCE_DL;
        cc_config[i].ul_bid_forward = DEFAULT_UL_BID_FWD;
        cc_config[i].ul_radio_ch_dly = DEFAULT_UL_RADIO_CH_DLY;
        cc_config[i].num_ctrl_per_sym_ul = DEFAULT_CTRL_PER_SYM;
        cc_config[i].num_ctrl_per_sym_dl = DEFAULT_CTRL_PER_SYM;
        cc_config[i].num_ctrl_per_sym_ssb = DEFAULT_CTRL_PER_SYM_SSB;
        cc_config[i].num_sect_per_sym = DEFAULT_SECT_PER_SYM;
        cc_config[i].num_sect_per_sym_ssb = DEFAULT_SECT_PER_SYM_SSB;
        cc_config[i].num_frames_per_sym = DEFAULT_FRAMES_PER_SYM;
        cc_config[i].num_frames_per_sym_ssb = DEFAULT_FRAMES_PER_SYM_SSB;
    }
}

int xorif_get_state(void)
{
    TRACE("xorif_get_state()\n");
    return xorif_state;
}

void xorif_debug(int level)
{
    TRACE("xorif_debug(%d)\n", level);
    xorif_trace = level;

#ifdef EXTRA_DEBUG
    if ((level >= 3) && (level <= 4))
    {
        if (!log_file)
        {
            // Create log-file, ignore errors
            log_file = fopen("libxorif.log", "w");
        }
    }
    else
    {
        if (log_file)
        {
            fclose(log_file);
            log_file = NULL;
        }
    }
#endif
}

int xorif_init(const char *device_name)
{
    TRACE("xorif_init(%s)\n", device_name ? device_name : "NULL");

    // See if we're already initialized
    if (xorif_state != 0)
    {
        // Just warning here, doesn't appear to be an error case
        INFO("Libmetal framework is already running\n");
        return XORIF_SUCCESS;
    }
    else
    {
        // Reset the state to 'not operational'
        xorif_state = 0;
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
            return XORIF_LIBMETAL_ERROR;
        }
    }
#endif

    // Get best-match device name
    device_name = get_device_name(device_name, compatible);
    if (device_name == NULL)
    {
        PERROR("No FHI device found\n");
        return XORIF_LIBMETAL_ERROR;
    }
    else
    {
        // Add FHI device
        INFO("FHI device is '%s'\n", device_name);

        if (add_device(&fh_device, "platform", device_name, fhi_irq_handler) != XORIF_SUCCESS)
        {
            PERROR("Failed to add FHI device '%s'\n", device_name);
            return XORIF_LIBMETAL_ERROR;
        }
    }

    // Initialize FHI device
    xorif_fhi_init_device();

    // Initialize the default component configuration
    initialize_configuration();

    // Update state to 'operational'
    xorif_state = 1;

    // Success
    return XORIF_SUCCESS;
}

void xorif_finish(void)
{
    TRACE("xorif_finish()\n");

    if (xorif_state != 0)
    {
#ifndef NO_HW
        // Close FHI device
        if (fh_device.dev != NULL)
        {
            INFO("Closing FHI device '%s'\n", fh_device.dev->name);
            int irq = (intptr_t)fh_device.dev->irq_info;
            if (irq != -1)
            {
                metal_irq_disable(irq);
                metal_irq_unregister(irq);
                INFO("FHI IRQ de-registered\n");
            }
            metal_device_close(fh_device.dev);
        }

#if 0
        // Close libmetal
        // Note, finishing libmetal may cause problems with multiple devices
        INFO("Finishing libmetal framework\n");
        metal_finish();
#endif
#endif
        // Set state to 'not operational'
        xorif_state = 0;
    }
}

uint32_t xorif_get_sw_version(void)
{
    TRACE("xorif_get_sw_version()\n");
    return (SW_MAJ_VER << 24 | SW_MIN_VER << 16 | SW_REVISION);
}

const struct xorif_caps *xorif_get_capabilities(void)
{
    TRACE("xorif_get_capabilities()\n");
    return &fhi_caps;
}

int xorif_has_front_haul_interface(void)
{
    TRACE("xorif_has_front_haul_interface()\n");

#ifdef NO_HW
    // Always fake it, when compiled with NO_HW
    return 1;
#else
    if (fh_device.dev != NULL)
    {
        // Device exists
        return 1;
    }
    else
    {
        return 0;
    }
#endif
}

int xorif_has_oran_channel_processor(void)
{
    TRACE("xorif_has_oran_channel_processor()\n");

#ifdef NO_HW
    // Always fake it, when compiled with NO_HW
    return 1;
#else
    if (fh_device.dev != NULL)
    {
        // Device exists
        return READ_REG(CFG_CONFIG_XRAN_OCP_IN_CORE);
    }
    else
    {
        return 0;
    }
#endif
}

int xorif_configure_cc(uint16_t cc)
{
    TRACE("xorif_configure_cc(%d)\n", cc);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }

    // Configure the FHI
    return xorif_fhi_configure_cc(cc);
}

int xorif_enable_cc(uint16_t cc)
{
    TRACE("xorif_enable_cc(%d)\n", cc);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else
    {
        return xorif_fhi_cc_enable(cc);
    }
}

int xorif_disable_cc(uint16_t cc)
{
    TRACE("xorif_disable_cc(%d)\n", cc);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else
    {
        return xorif_fhi_cc_disable(cc);
    }
}

uint8_t xorif_get_enabled_cc_mask(void)
{
    TRACE("xorif_get_enabled_cc_mask()\n");
    return xorif_fhi_get_enabled_mask();
}

int xorif_set_cc_config(uint16_t cc, const struct xorif_cc_config *config)
{
    TRACE("xorif_set_cc_config(%d, ...)\n", cc);

    // Check for valid configuration
    if (config == NULL)
    {
        PERROR("Null pointer\n");
        return XORIF_NULL_POINTER;
    }
    else if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (config->num_rbs < MIN_NUM_RBS || config->num_rbs > MAX_NUM_RBS)
    {
        PERROR("Invalid number of RBs\n");
        return XORIF_INVALID_RBS;
    }
    else if (!check_numerology(config->numerology, config->extended_cp))
    {
        PERROR("Invalid numerology\n");
        return XORIF_NUMEROLOGY_NOT_SUPPORTED;
    }
    else if (!check_iq_comp_mode(config->iq_comp_width_ul, config->iq_comp_meth_ul, CHAN_UL))
    {
        PERROR("IQ compression method/width not supported (UL)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }
    else if (!check_iq_comp_mode(config->iq_comp_width_dl, config->iq_comp_meth_dl, CHAN_DL))
    {
        PERROR("IQ compression method/width not supported (DL)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }
    else if (!(config->num_rbs_ssb == SSB_NUM_RBS || config->num_rbs_ssb == 0))
    {
        PERROR("Invalid number of RBs (SSB)\n");
        return XORIF_INVALID_RBS;
    }
    else if (!check_numerology(config->numerology_ssb, config->extended_cp_ssb))
    {
        PERROR("Invalid numerology (SSB)\n");
        return XORIF_NUMEROLOGY_NOT_SUPPORTED;
    }
    else if (!check_iq_comp_mode(config->iq_comp_width_ssb, config->iq_comp_meth_ssb, CHAN_SSB))
    {
        PERROR("IQ compression method/width not supported (SSB)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }
    else if (!check_iq_comp_mode(config->iq_comp_width_prach, config->iq_comp_meth_prach, CHAN_PRACH))
    {
        PERROR("IQ compression method/width not supported (PRACH)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }

    memcpy(&cc_config[cc], config, sizeof(struct xorif_cc_config));

    return XORIF_SUCCESS;
}

int xorif_set_cc_num_rbs(uint16_t cc, uint16_t num_rbs)
{
    TRACE("xorif_set_cc_num_rbs(%d, %d)\n", cc, num_rbs);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (num_rbs < MIN_NUM_RBS || num_rbs > MAX_NUM_RBS)
    {
        PERROR("Invalid number of RBs\n");
        return XORIF_INVALID_RBS;
    }

    cc_config[cc].num_rbs = num_rbs;

    return XORIF_SUCCESS;
}

int xorif_set_cc_numerology(uint16_t cc, uint16_t numerology, uint16_t extended_cp)
{
    TRACE("xorif_set_cc_numerology(%d, %d, %d)\n", cc, numerology, extended_cp);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!check_numerology(numerology, extended_cp))
    {
        PERROR("Invalid numerology\n");
        return XORIF_NUMEROLOGY_NOT_SUPPORTED;
    }

    cc_config[cc].numerology = numerology;
    cc_config[cc].extended_cp = extended_cp;

    return XORIF_SUCCESS;
}

int xorif_set_cc_num_rbs_ssb(uint16_t cc, uint16_t num_rbs)
{
    TRACE("xorif_set_cc_num_rbs_ssb(%d, %d)\n", cc, num_rbs);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!(num_rbs == SSB_NUM_RBS || num_rbs == 0))
    {
        PERROR("Invalid number of RBs\n");
        return XORIF_INVALID_RBS;
    }

    cc_config[cc].num_rbs_ssb = num_rbs;

    return XORIF_SUCCESS;
}

int xorif_set_cc_numerology_ssb(uint16_t cc, uint16_t numerology, uint16_t extended_cp)
{
    TRACE("xorif_set_cc_numerology_ssb(%d, %d, %d)\n", cc, numerology, extended_cp);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!check_numerology(numerology, extended_cp))
    {
        PERROR("Invalid numerology\n");
        return XORIF_NUMEROLOGY_NOT_SUPPORTED;
    }

    cc_config[cc].numerology_ssb = numerology;
    cc_config[cc].extended_cp_ssb = extended_cp;

    return XORIF_SUCCESS;
}

int xorif_set_cc_time_advance(uint16_t cc,
                              double deskew,
                              double advance_ul,
                              double advance_dl)
{
    // Note: This function is deprecated.

    TRACE("xorif_set_cc_time_advance(%d, %g, %g, %g) [deprecated]\n", cc, deskew, advance_ul, advance_dl);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }

    cc_config[cc].delay_comp_cp_ul = deskew;
    cc_config[cc].delay_comp_cp_dl = deskew;
    cc_config[cc].delay_comp_up = deskew;
    cc_config[cc].advance_ul = advance_ul;
    cc_config[cc].advance_dl = advance_dl;

    return XORIF_SUCCESS;
}

int xorif_set_cc_ul_timing_parameters(uint16_t cc,
                                      double delay_comp_cp,
                                      double advance,
                                      double radio_ch_delay)
{
    TRACE("xorif_set_cc_ul_timing_parameters(%d, %g, %g, %g)\n", cc, delay_comp_cp, advance, radio_ch_delay);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }

    cc_config[cc].delay_comp_cp_ul = delay_comp_cp;
    cc_config[cc].advance_ul = advance;
    cc_config[cc].ul_radio_ch_dly = radio_ch_delay;

    return XORIF_SUCCESS;
}

int xorif_set_cc_dl_timing_parameters(uint16_t cc,
                                      double delay_comp_cp,
                                      double delay_comp_up,
                                      double advance)
{
    TRACE("xorif_set_cc_dl_timing_parameters(%d, %g, %g, %g)\n", cc, delay_comp_cp, delay_comp_up, advance);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }

    cc_config[cc].delay_comp_cp_dl = delay_comp_cp;
    cc_config[cc].delay_comp_up = delay_comp_up;
    cc_config[cc].advance_dl = advance;

    return XORIF_SUCCESS;
}

int xorif_set_ul_bid_forward(uint16_t cc, double time)
{
    TRACE("xorif_set_ul_bid_forward(%d, %g)\n", cc, time);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }

    cc_config[cc].ul_bid_forward = time;

    return XORIF_SUCCESS;
}

int xorif_set_ul_radio_ch_dly(uint16_t cc, double delay)
{
    // Note: This function is deprecated.

    TRACE("xorif_set_ul_radio_ch_dly(%d, %g) [deprecated]\n", cc, delay);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }

    cc_config[cc].ul_radio_ch_dly = delay;

    return XORIF_SUCCESS;
}

int xorif_set_cc_dl_iq_compression(uint16_t cc,
                                   uint16_t bit_width,
                                   enum xorif_iq_comp comp_method,
                                   uint16_t mplane)
{
    TRACE("xorif_set_cc_dl_iq_compression(%d, %d, %d, %d)\n", cc, bit_width, comp_method, mplane);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!check_iq_comp_mode(bit_width, comp_method, CHAN_DL))
    {
        PERROR("IQ compression method/width not supported (DL)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }

    cc_config[cc].iq_comp_width_dl = bit_width;
    cc_config[cc].iq_comp_meth_dl = comp_method;
    cc_config[cc].iq_comp_mplane_dl = mplane;

    return XORIF_SUCCESS;
}

int xorif_set_cc_ul_iq_compression(uint16_t cc,
                                   uint16_t bit_width,
                                   enum xorif_iq_comp comp_method,
                                   uint16_t mplane)
{
    TRACE("xorif_set_cc_ul_iq_compression(%d, %d, %d, %d)\n", cc, bit_width, comp_method, mplane);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!check_iq_comp_mode(bit_width, comp_method, CHAN_UL))
    {
        PERROR("IQ compression method/width not supported (UL)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }

    cc_config[cc].iq_comp_width_ul = bit_width;
    cc_config[cc].iq_comp_meth_ul = comp_method;
    cc_config[cc].iq_comp_mplane_ul = mplane;

    return XORIF_SUCCESS;
}

int xorif_set_cc_iq_compression_ssb(uint16_t cc,
                                    uint16_t bit_width,
                                    enum xorif_iq_comp comp_method,
                                    uint16_t mplane)
{
    TRACE("xorif_set_cc_iq_compression_ssb(%d, %d, %d, %d)\n", cc, bit_width, comp_method, mplane);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!check_iq_comp_mode(bit_width, comp_method, CHAN_SSB))
    {
        PERROR("IQ compression method/width not supported (SSB)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }

    cc_config[cc].iq_comp_width_ssb = bit_width;
    cc_config[cc].iq_comp_meth_ssb = comp_method;
    cc_config[cc].iq_comp_mplane_ssb = mplane;

    return XORIF_SUCCESS;
}

int xorif_set_cc_iq_compression_prach(uint16_t cc,
                                     uint16_t bit_width,
                                     enum xorif_iq_comp comp_method,
                                     uint16_t mplane)
{
    TRACE("xorif_set_cc_iq_compression_prach(%d, %d, %d, %d)\n", cc, bit_width, comp_method, mplane);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!check_iq_comp_mode(bit_width, comp_method, CHAN_PRACH))
    {
        PERROR("IQ compression method/width not supported (PRACH)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }

    cc_config[cc].iq_comp_width_prach = bit_width;
    cc_config[cc].iq_comp_meth_prach = comp_method;
    cc_config[cc].iq_comp_mplane_prach = mplane;

    return XORIF_SUCCESS;
}

int xorif_set_cc_dl_sections_per_symbol(uint16_t cc,
                                        uint16_t num_sect,
                                        uint16_t num_ctrl)
{
    TRACE("xorif_set_cc_dl_sections_per_symbol(%d, %d, %d)\n", cc, num_sect, num_ctrl);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    // No upper limit, only buffer space which is checked during configuration

    cc_config[cc].num_sect_per_sym = num_sect;
    cc_config[cc].num_ctrl_per_sym_dl = num_ctrl;

    return XORIF_SUCCESS;
}

int xorif_set_cc_ul_sections_per_symbol(uint16_t cc,
                                        uint16_t num_sect,
                                        uint16_t num_ctrl)
{
    TRACE("xorif_set_cc_ul_sections_per_symbol(%d, %d, %d)\n", cc, num_sect, num_ctrl);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    // No upper limit, only buffer space which is checked during configuration

    // Note, num_sect not needed for uplink currently
    cc_config[cc].num_ctrl_per_sym_ul = num_ctrl;

    return XORIF_SUCCESS;
}

int xorif_set_cc_frames_per_symbol(uint16_t cc, uint16_t num_frames)
{
    TRACE("xorif_set_cc_frames_per_symbol(%d, %d)\n", cc, num_frames);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    // No upper limit, only buffer space which is checked during configuration

    cc_config[cc].num_frames_per_sym = num_frames;

    return XORIF_SUCCESS;
}

int xorif_set_cc_sections_per_symbol_ssb(uint16_t cc,
                                         uint16_t num_sect,
                                         uint16_t num_ctrl)
{
    TRACE("xorif_set_cc_sections_per_symbol_ssb(%d, %d, %d)\n", cc, num_sect, num_ctrl);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    // No upper limit, only buffer space which is checked during configuration

    cc_config[cc].num_sect_per_sym_ssb = num_sect;
    cc_config[cc].num_ctrl_per_sym_ssb = num_ctrl;

    return XORIF_SUCCESS;
}

int xorif_set_cc_frames_per_symbol_ssb(uint16_t cc, uint16_t num_frames)
{
    TRACE("xorif_set_cc_frames_per_symbol_ssb(%d, %d)\n", cc, num_frames);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    // No upper limit, only buffer space which is checked during configuration

    cc_config[cc].num_frames_per_sym_ssb = num_frames;

    return XORIF_SUCCESS;
}

int xorif_get_cc_config(uint16_t cc, struct xorif_cc_config *ptr)
{
    TRACE("xorif_get_cc_config(%d, ...)\n", cc);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!ptr)
    {
        PERROR("Null pointer\n");
        return XORIF_NULL_POINTER;
    }

    memcpy(ptr, &cc_config[cc], sizeof(struct xorif_cc_config));
    return XORIF_SUCCESS;
}

/** @} */
