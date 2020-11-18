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
 * @file xorif_common.c
 * @author Steven Dickinson
 * @brief Source file for libxorif common functions.
 * @addtogroup libxorif
 * @{
 *
 * C Library for the Xilinx ORAN Radio Interface (libxorif)
 */

#include <stdio.h>
#include <string.h>
#include "xorif_api.h"
#include "xorif_system.h"
#include "xorif_fh_func.h"
#ifdef BF_INCLUDED
#include "xorif_bf_func.h"
#endif // BF_INCLUDED
#include "xorif_common.h"
#include "xorif_utils.h"

// Software version number
#define SW_MAJ_VER 1
#define SW_MIN_VER 1
#define SW_REVISION 1

// Supported IQ compression modes
#define DEFAULT_IQ_COMP (IQ_COMP_NONE | IQ_COMP_BLOCK_FP)
#define DEFAULT_BW_COMP (BW_COMP_BLOCK_FP)

// Globals
uint16_t xorif_state;
int xorif_trace = 0;
struct xorif_device_info fh_device;
struct xorif_fhi_caps fhi_caps;
#ifdef BF_INCLUDED
struct xorif_device_info bf_device;
struct xorif_bf_caps bf_caps;
#endif // BF_INCLUDED
struct xorif_cc_config cc_config[MAX_NUM_CC];
struct xorif_caps caps;
const int sc_spacing[NUM_NUMEROLOGY] = {15, 30, 60, 120, 240};
const int slots_per_subframe[NUM_NUMEROLOGY] = {1, 2, 4, 8, 16};

// Simple macro to return minimum of two values
#define MIN(a, b) (a) < (b) ? (a) : (b)

/**
 * @brief Initialize capabilities structure.
 */
static void initialize_capabilities(void)
{
    // Populate structure
    memset(&caps, 0, sizeof(struct xorif_caps));
#ifdef BF_INCLUDED
    if (xorif_has_beamformer())
    {
        // When beamformer is present use the minimum values
        caps.max_cc = MIN(xorif_fhi_get_max_cc(), xorif_bf_get_max_cc());
        caps.num_eth_ports = MIN(xorif_fhi_get_num_eth_ports(), xorif_bf_get_num_eth_ports());
        caps.numerologies = NUMEROLOGY_0_SUPPORT | NUMEROLOGY_1_SUPPORT;
    }
    else
#endif
    {
        caps.max_cc = xorif_fhi_get_max_cc();
        caps.num_eth_ports = xorif_fhi_get_num_eth_ports();
        caps.numerologies = NUMEROLOGY_0_SUPPORT | NUMEROLOGY_1_SUPPORT | NUMEROLOGY_2_SUPPORT |
                            NUMEROLOGY_3_SUPPORT | NUMEROLOGY_4_SUPPORT;
    }

    caps.extended_cp = 0;
    caps.iq_comp_methods = IQ_COMP_NONE_SUPPORT | IQ_COMP_BLOCK_FP_SUPPORT;
    caps.bw_comp_methods = BW_COMP_BLOCK_FP_SUPPORT;
}

/**
 * @brief Initialize the configuration data.
 */
static void initialize_configuration(void)
{
    // Initialize the component carrier state and configuration
    for (int i = 0; i < MAX_NUM_CC; ++i)
    {
        // Populate structure with defaults
        memset(&cc_config[i], 0, sizeof(struct xorif_cc_config));
        cc_config[i].num_rbs = 0;
        cc_config[i].numerology = 0;
        cc_config[i].extended_cp = 0;
        cc_config[i].deskew = DEFAULT_DESKEW_TIME;
        cc_config[i].advance_ul = DEFAULT_ADVANCE_UL;
        cc_config[i].advance_dl = DEFAULT_ADVANCE_DL;
        cc_config[i].iq_comp_meth_ul = IQ_COMP_NONE;
        cc_config[i].iq_comp_width_ul = 16;
        cc_config[i].iq_comp_meth_dl = IQ_COMP_NONE;
        cc_config[i].iq_comp_width_dl = 16;
        cc_config[i].bw_comp_meth = BW_COMP_BLOCK_FP;
        cc_config[i].bw_comp_width = 12;
        cc_config[i].num_ctrl_per_sym_ul = DEFAULT_CTRL_PER_SYM;
        cc_config[i].num_ctrl_per_sym_dl = DEFAULT_CTRL_PER_SYM;
        cc_config[i].num_frames_per_sym = DEFAULT_FRAMES_PER_SYM;
        cc_config[i].num_rbs_ssb = 20;
        cc_config[i].numerology_ssb = 0;
        cc_config[i].extended_cp_ssb = 0;
        cc_config[i].iq_comp_meth_ssb = IQ_COMP_NONE;
        cc_config[i].iq_comp_width_ssb = 16;
        cc_config[i].num_ctrl_per_sym_ssb = DEFAULT_CTRL_PER_SYM_SSB;
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
}

int xorif_init(const char *fh_dev_name, const char *bf_dev_name)
{
    TRACE("xorif_init(%s, %s)\n", fh_dev_name ? fh_dev_name : "NULL", bf_dev_name ? bf_dev_name : "NULL");

    // See if we're already initialized
    if (xorif_state != 0)
    {
        // Just warning here, doesn't appear to be an error case
        INFO("Libmetal framework is already running.\n");
        return XORIF_SUCCESS;
    }
    else
    {
        // Reset the state to 'not operational'
        xorif_state = 0;
    }

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

    // Get FHI device name
    if (fh_dev_name == NULL)
    {
        // No device name specified, so search for it in /sys/bus/platform/devices
        if ((fh_dev_name = get_device_name("oran_radio_if")) != NULL)
        {
            INFO("FHI device is '%s'\n", fh_dev_name);
        }
    }

    // Add FHI device
    if (fh_dev_name == NULL)
    {
        PERROR("No FHI device found\n");
        return XORIF_LIBMETAL_ERROR;
    }
    else if (add_device(&fh_device, "platform", fh_dev_name) == XORIF_SUCCESS)
    {
        // Initialize FHI device
        xorif_fhi_init_device();
    }
    else
    {
        PERROR("Failed to add FHI device '%s'\n", fh_dev_name);
        return XORIF_LIBMETAL_ERROR;
    }

#ifdef BF_INCLUDED
    // Get BF device name
    if (bf_dev_name == NULL)
    {
        // No device name specified, so search for it in /sys/bus/platform/devices
        if ((bf_dev_name = get_device_name("beamformer")) != NULL)
        {
            INFO("BF device is '%s'\n", bf_dev_name);
        }
    }

    // Add BF device
    if (bf_dev_name == NULL)
    {
        INFO("No BF device found\n");
    }
    else if (add_device(&bf_device, "platform", bf_dev_name) == XORIF_SUCCESS)
    {
        // Initialize BF device
        xorif_bf_init_device();
    }
    else
    {
        INFO("Failed to add BF device '%s'\n", bf_dev_name);
    }
#endif // BF_INCLUDED

    // Initialize the capabilities structure
    initialize_capabilities();

    // Initialize the configuration
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
        // Close FHI device
        if (fh_device.dev != NULL)
        {
            INFO("Closing FHI device %s\n", fh_device.dev_name);
            int irq = (intptr_t)fh_device.dev->irq_info;
            if (irq != -1)
            {
                metal_irq_disable(irq);
                metal_irq_unregister(irq);
                INFO("FHI IRQ de-registered\n");
            }
            metal_device_close(fh_device.dev);
        }

#ifdef BF_INCLUDED
        // Close BF device
        if (bf_device.dev != NULL)
        {
            INFO("Closing BF device %s\n", bf_device.dev_name);
            int irq = (intptr_t)bf_device.dev->irq_info;
            if (irq != -1)
            {
                metal_irq_disable(irq);
                metal_irq_unregister(irq);
                INFO("BF IRQ de-registered\n");
            }
            metal_device_close(bf_device.dev);
        }
#endif

        // Close libmetal
        INFO("Finishing libmetal framework\n");
        metal_finish();

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
    return &caps;
}

int xorif_has_front_haul_interface(void)
{
    TRACE("xorif_has_front_haul_interface()\n");

    if (fh_device.dev != NULL && fh_device.status != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int xorif_has_beamformer(void)
{
    TRACE("xorif_has_beamformer()\n");
#ifdef BF_INCLUDED
    if (bf_device.dev != NULL && bf_device.status != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
#else
    return 0;
#endif // BF_INCLUDED
}

int xorif_configure_cc(uint16_t cc)
{
    TRACE("xorif_configure_cc(%d)\n", cc);

    if (cc >= MAX_NUM_CC || cc >= caps.max_cc)
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }

    // Additional check for the beamformer configuration
#ifdef BF_INCLUDED
    if (xorif_has_beamformer())
    {
        // Check that the combination of numerology and number of RBs is valid
        const struct xorif_cc_config *ptr = &cc_config[cc];
        if (xorif_bf_get_config_index(ptr->numerology, ptr->num_rbs, CHAN_UL_DL) == -1)
        {
            PERROR("Invalid beamformer configuration (UL/DL)\n");
            return XORIF_INVALID_CONFIG;
        }
        if (xorif_bf_get_config_index(ptr->numerology_ssb, 20, CHAN_SSB) == -1)
        {
            PERROR("Invalid beamformer configuration (SSB)\n");
            return XORIF_INVALID_CONFIG;
        }
    }
#endif

    // Configure the FHI
    int result = xorif_fhi_configure_cc(cc);

#ifdef BF_INCLUDED
    if (result == XORIF_SUCCESS && xorif_has_beamformer())
    {
        // Configure the BF
        result = xorif_bf_configure_cc(cc);
    }
#endif

    return result;
}

int xorif_enable_cc(uint16_t cc)
{
    TRACE("xorif_enable_cc(%d)\n", cc);

    if (cc >= MAX_NUM_CC || cc >= caps.max_cc)
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

    if (cc >= MAX_NUM_CC || cc >= caps.max_cc)
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else
    {
        return xorif_fhi_cc_disable(cc);
    }
}

int xorif_set_cc_config(uint16_t cc, const struct xorif_cc_config *config)
{
    TRACE("xorif_set_cc_config(%d, ...)\n", cc);

    // Check for valid configuration
    if (config == NULL)
    {
        PERROR("Invalid configuration\n");
        return XORIF_NULL_POINTER;
    }
    else if (cc >= MAX_NUM_CC || cc >= caps.max_cc)
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
    else if (!check_iq_comp_mode(config->iq_comp_width_ul, config->iq_comp_meth_ul))
    {
        PERROR("IQ compression mode not supported (uplink)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }
    else if (!check_iq_comp_mode(config->iq_comp_width_dl, config->iq_comp_meth_dl))
    {
        PERROR("IQ compression mode not supported (downlink)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }
    else if (!check_bw_comp_mode(config->bw_comp_width, config->bw_comp_meth))
    {
        PERROR("Beam-weight compression mode not supported\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }
    else if (config->num_rbs_ssb  != SSB_NUM_RBS)
    {
        PERROR("Invalid number of RBs (SSB)\n");
        return XORIF_INVALID_RBS;
    }
    else if (!check_numerology(config->numerology_ssb, config->extended_cp_ssb))
    {
        PERROR("Invalid numerology (SSB)\n");
        return XORIF_NUMEROLOGY_NOT_SUPPORTED;
    }
    else if (!check_iq_comp_mode(config->iq_comp_width_ssb, config->iq_comp_meth_ssb))
    {
        PERROR("IQ compression mode not supported (SSB)\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }
#ifdef BF_INCLUDED
    else if (xorif_has_beamformer())
    {
        // Check that the combination of numerology and number of RBs is valid
        if (xorif_bf_get_config_index(config->numerology, config->num_rbs, CHAN_UL_DL) == -1)
        {
            PERROR("Invalid beamformer configuration (UL/DL)\n");
            return XORIF_INVALID_CONFIG;
        }
        if (xorif_bf_get_config_index(config->numerology_ssb, 20, CHAN_SSB) == -1)
        {
            PERROR("Invalid beamformer configuration (SSB)\n");
            return XORIF_INVALID_CONFIG;
        }
    }
#endif

    memcpy(&cc_config[cc], config, sizeof(struct xorif_cc_config));

    return XORIF_SUCCESS;
}

int xorif_set_cc_num_rbs(uint16_t cc, uint16_t num_rbs)
{
    TRACE("xorif_set_cc_num_rbs(%d, %d)\n", cc, num_rbs);

    if (cc >= MAX_NUM_CC || cc >= caps.max_cc)
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

    if (cc >= MAX_NUM_CC || cc >= caps.max_cc)
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

int xorif_set_cc_numerology_ssb(uint16_t cc, uint16_t numerology, uint16_t extended_cp)
{
    TRACE("xorif_set_cc_numerology_ssb(%d, %d, %d)\n", cc, numerology, extended_cp);

    if (cc >= MAX_NUM_CC || cc >= caps.max_cc)
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
                              uint32_t deskew,
                              uint32_t advance_ul,
                              uint32_t advance_dl)
{
    TRACE("xorif_set_cc_time_advance(%d, %u, %u, %u)\n", cc, deskew, advance_ul, advance_dl);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }

    cc_config[cc].deskew = deskew;
    cc_config[cc].advance_ul = advance_ul;
    cc_config[cc].advance_dl = advance_dl;

    return XORIF_SUCCESS;
}

int xorif_set_cc_dl_iq_compression(uint16_t cc,
                                   uint16_t bit_width,
                                   enum xorif_iq_comp comp_method)
{
    TRACE("xorif_set_cc_dl_iq_compression(%d, %d, %d)\n", cc, bit_width, comp_method);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!check_iq_comp_mode(bit_width, comp_method))
    {
        PERROR("IQ compression mode not supported\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }

    cc_config[cc].iq_comp_width_dl = bit_width;
    cc_config[cc].iq_comp_meth_dl = comp_method;

    return XORIF_SUCCESS;
}

int xorif_set_cc_ul_iq_compression(uint16_t cc,
                                   uint16_t bit_width,
                                   enum xorif_iq_comp comp_method)
{
    TRACE("xorif_set_cc_ul_iq_compression(%d, %d, %d)\n", cc, bit_width, comp_method);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!check_iq_comp_mode(bit_width, comp_method))
    {
        PERROR("IQ compression mode not supported\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }

    cc_config[cc].iq_comp_width_ul = bit_width;
    cc_config[cc].iq_comp_meth_ul = comp_method;

    return XORIF_SUCCESS;
}

int xorif_set_cc_iq_compression_ssb(uint16_t cc,
                                   uint16_t bit_width,
                                   enum xorif_iq_comp comp_method)
{
    TRACE("xorif_set_cc_iq_compression_ssb(%d, %d, %d)\n", cc, bit_width, comp_method);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!check_iq_comp_mode(bit_width, comp_method))
    {
        PERROR("IQ compression mode not supported\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }

    cc_config[cc].iq_comp_width_ssb = bit_width;
    cc_config[cc].iq_comp_meth_ssb = comp_method;

    return XORIF_SUCCESS;
}

int xorif_set_cc_bw_compression(uint16_t cc,
                                uint16_t bit_width,
                                enum xorif_bw_comp comp_method)
{
    TRACE("xorif_set_cc_bw_compression(%d, %d, %d)\n", cc, bit_width, comp_method);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    else if (!check_bw_comp_mode(bit_width, comp_method))
    {
        PERROR("Beam-weight compression mode not supported\n");
        return XORIF_COMP_MODE_NOT_SUPPORTED;
    }

    cc_config[cc].bw_comp_width = bit_width;
    cc_config[cc].bw_comp_meth = comp_method;

    return XORIF_SUCCESS;
}

int xorif_set_cc_dl_sections_per_symbol(uint16_t cc, uint16_t num_sections)
{
    TRACE("xorif_set_cc_dl_sections_per_symbol(%d, %d)\n", cc, num_sections);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    // No upper limit, only buffer space which is checked during configuration

    cc_config[cc].num_ctrl_per_sym_dl = num_sections;

    return XORIF_SUCCESS;
}

int xorif_set_cc_ul_sections_per_symbol(uint16_t cc, uint16_t num_sections)
{
    TRACE("xorif_set_cc_ul_sections_per_symbol(%d, %d)\n", cc, num_sections);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    // No upper limit, only buffer space which is checked during configuration

    cc_config[cc].num_ctrl_per_sym_ul = num_sections;

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

int xorif_set_cc_sections_per_symbol_ssb(uint16_t cc, uint16_t num_sections)
{
    TRACE("xorif_set_cc_sections_per_symbol_ssb(%d, %d)\n", cc, num_sections);

    if (cc >= MAX_NUM_CC || cc >= xorif_fhi_get_max_cc())
    {
        PERROR("Invalid CC value\n");
        return XORIF_INVALID_CC;
    }
    // No upper limit, only buffer space which is checked during configuration

    cc_config[cc].num_ctrl_per_sym_ssb = num_sections;

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