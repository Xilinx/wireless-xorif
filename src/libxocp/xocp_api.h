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
 * @file xocp_api.h
 * @author Steven Dickinson
 * @brief Main header file for the Xilinx ORAN Channel Processor (libxocp).
 * @addtogroup libxocp-api
 * @{
 *
 * "C" API for the Xilinx ORAN Channel Processor (libxocp)
 */

#ifndef XOCP_API_H
#define XOCP_API_H

#ifndef CFFI_CDEF_HDR
#include <stdint.h>
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

#define XOCP_NUM_INSTANCES 1 /**< Number of driver instances */
#define XOCP_MAX_NUM_CC 8    /**< Maximum number of component carriers supported */
#define XOCP_MAX_ANTENNA 8   /**< Maximum number of antennas supported by SW */

/**
 * @brief Enumerated type for error/status codes.
 */
enum xocp_error_codes
{
    XOCP_OPERATIONAL_ERRORS = -3000,   /**< (Place-holder for operational errors) */
    XOCP_NULL_POINTER,                 /**< Attempt to access memory with null pointer */
    XOCP_REGISTER_NOT_FOUND,           /**< Unknown register name */
    XOCP_MEMORY_ALLOCATION_FAIL,       /**< Memory allocation fail */
    XOCP_TIMEOUT_FAIL,                 /**< Timeout fail */
    XOCP_CONFIGURATION_ERRORS = -2000, /**< (Place-holder for configuration errors) */
    XOCP_INVALID_CC,                   /**< Component carrier instance is not valid */
    XOCP_INVALID_RBS,                  /**< Number of RBs is not valid */
    XOCP_INVALID_NUMEROLOGY,           /**< Numerology is not valid */
    XOCP_INVALID_ANTENNAS,             /**< Antenna configuration is not valid */
    XOCP_SCHEDULING_ERROR,             /**< Error during scheduling */
    XOCP_FRAMEWORK_ERRORS = -1000,     /**< (Place holder for library / libmetal framework errors) */
    XOCP_LIBMETAL_ERROR,               /**< Error with libmetal framework */
    XOCP_NO_SUCH_DEVICE,               /**< Requested device node does not exist */
    XOCP_NO_RESOURCES,                 /**< Insufficient resources to complete request */
    XOCP_INVALID_INSTANCE,             /**< Invalid device instance specified */
    XOCP_INVALID_STATE,                /**< Incorrect state to handle request */
    XOCP_OTHER_ERRORS = -500,          /**< (Place-holder for other errors) */
    XOCP_NOT_SUPPORTED,                /**< Generic "not-supported" error code */
    XOCP_INVALID_RESULT = -1,          /**< Represents an invalid result */
    XOCP_SUCCESS = 0,                  /**< Success! No error! */
    XOCP_FAILURE = 1,                  /**< Failure return code */
};

/**
 * @brief Enumerations for error/event information.
 */
enum xocp_events
{
    XOCP_DL_CC_UPDATE_TRIGGERED = 0x1,    /**< Set when the DL CC update trigger has occurred */
    XOCP_DL_SEQUENCE_ERROR = 0x2,         /**< Set when the DL sequence has not completed in a symbol period */
    XOCP_DL_SEQUENCE_TABLE_ERROR = 0x4,   /**< Set when DL data has been received when the sequence table is still updating */
    XOCP_UL_CC_UPDATE_TRIGGERED = 0x100,  /**< Set when the UL CC update trigger has occurred */
    XOCP_UL_SEQUENCE_ERROR = 0x200,       /**< Set when the DL sequence has not completed in a symbol period */
    XOCP_UL_SEQUENCE_TABLE_ERROR = 0x400, /**< Set when DL data has been received when the sequence table is still updating */
};

/**
 * @brief Enumerations for state machine states.
 */
enum xocp_states
{
    XOCP_IDLE = 0,    /**< Initial state (after libary loading) */
    XOCP_RESET,       /**< State after instance creation / reset (see #xocp_start / #xocp_reset) */
    XOCP_READY,       /**< State after releasing reset (see #xocp_reset) */
    XOCP_OPERATIONAL, /**< State after activation (see #xocp_activate) */
};

/**
 * @brief Structure for supported capabilities.
 */
struct xocp_caps
{
    uint8_t max_num_cc;      /**< Maximum number of component carriers (1-16) */
    uint8_t max_num_antenna; /**< Maximum number of antennas (1-8) */
    uint8_t max_interleave;  /**< Maximum antenna interleave (1, 2, 4, 8) */
};

/**
 * @brief Structure for component carrier configuration used in #xocp_set_cc_cfg, etc.
 */
struct xocp_cc_data
{
    uint8_t enable;         /**< 0 = disabled, 1 = enabled */
    uint16_t num_rbs;       /**< Number of RBs (1-273) */
    uint8_t numerology;     /**< Numerology (0-4) */
    uint8_t ccid;           /**< CCID (used by DFE) that this instance maps to */
    uint16_t inter_sym_gap; /**< Inter-symbol gap (in IP core cycles) */
};

/**
 * @brief Structure for antenna configuration data used in #xocp_set_antenna_cfg, etc.
 */
struct xocp_antenna_data
{
    uint8_t num_antennas;           /**< Number of antennas (1, 2, 4, 8) */
    uint8_t interleave;             /**< Antenna interleave (1, 2, 4) */
    uint8_t data[XOCP_MAX_ANTENNA]; /**< Antenna order data */
};

/**
 * @brief Structure for trigger configuration.
 */
struct xocp_trigger_data
{
    uint8_t enable;           /**< 0 = disabled, 1 = enabled */
    uint8_t mode;             /**< 0 = immediate, 1 = single_shot */
    uint8_t tuser_edge_level; /**< 0 = low, 1 = high, 2 = falling, 3 = rising */
    uint8_t tuser_bit;        /**< TUSER bit trigger source (0-255) */
};

/**
 * @brief Structure for all triggers used in #xocp_set_trigger_cfg, etc.
 */
struct xocp_triggers
{
    struct xocp_trigger_data dl_update; /**< Trigger setting for DL "update" trigger */
    struct xocp_trigger_data ul_update; /**< Trigger setting for UL "update" trigger */
};

/**
 * @brief Type definition for event interrupts call-back function.
 */
typedef void (*xocp_isr_func_t)(uint16_t instance, uint32_t status);

/*****************************/
/*** Function declarations ***/
/*****************************/

/**
 * @brief Set debug/trace level.
 * @param level Debug level (0 = none, 1 = minimal, 2 = verbose)
 */
void xocp_debug(uint16_t level);

/**
 * @brief Start a driver instance for the device.
 * @param dev_name Device name of the O-RAN Channel Processor (or NULL)
 * @returns
 *      - Instance number (0..N)
 *      - Error code on failure
 * @note
 * If "dev_name" is left as NULL, then the library will search the devices
 * for the most appropriate one. Alternatively, "dev_name" can be used as
 * hint, e.g. "device_xyz" will match "a00000000.device_xyz".
 * Specify the exact device name if you want to override this behavior.
 */
int xocp_start(const char *dev_name);

/**
 * @brief Close/finalize the driver instance.
 * @param instance Device instance ID (0..N)
 */
void xocp_finish(uint16_t instance);

/**
 * @brief Returns the current state of the device.
 * @param instance Device instance ID (0..N)
 * @returns
 *      - State (see #xocp_states)
 *      - Error code on failure
 */
int xocp_get_state(uint16_t instance);

/**
 * @brief Return the s/w version.
 * @returns
 *      - Version
 *          major = bits[24..31]
 *          minor = bits[16..23]
 *          revision = bits[8..15]
 *          patch = bits[0..7]
 */
uint32_t xocp_get_sw_version(void);

/**
 * @brief Returns the h/w version.
 * @param instance Device instance ID (0..N)
 * @returns
 *      - Version
 *          major = bits[24..31]
 *          minor = bits[16..23]
 *          revision = bits[8..15]
 */
uint32_t xocp_get_hw_version(uint16_t instance);

/**
 * @brief Returns the internal h/w revision.
 * @param instance Device instance ID (0..N)
 * @returns
 *      - Internal revision
 */
uint32_t xocp_get_hw_internal_rev(uint16_t instance);

/**
 * @brief Get the device capabilities.
 * @param instance Device instance ID (0..N)
 * @param data Pointer to write-back the capabilities (see #xocp_caps)
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_get_capabilities(uint16_t instance,
                          struct xocp_caps *data);

/**
 * @brief Get the current event status (e.g. events/errors).
 * @param instance Device instance ID (0..N)
 * @param status Pointer to write-back the status (see #xocp_events)
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_get_event_status(uint16_t instance, uint32_t *status);

/**
 * @brief Clears the current event status (e.g. events/errors).
 * @param instance Device instance ID (0..N)
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_clear_event_status(uint16_t instance);

/**
 * @brief Enable/disable the event interrupts (according to supplied mask).
 * @param instance Device instance ID (0..N)
 * @param mask Bit-map mask for enabled interrupts (see #xocp_events)
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_enable_interrupts(uint16_t instance, uint32_t mask);

/**
 * @brief Register a call-back function for event interrupts.
 * @param instance Device instance ID (0..N)
 * @param callback Function pointer for call-back
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_register_event_callback(uint16_t instance,
                                 xocp_isr_func_t callback);

/**
 * @brief Function to read from a register / register field on the device.
 * @param instance Device instance ID (0..N)
 * @param name Register field name
 * @param value Pointer to write-back the read value
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 * @note
 * This utility function takes a C-string representation of the register name,
 * e.g. "MODEL_PARAM". Alternatively, an offset value can be provided, e.g.
 * "0x8". Ths API is intended primarily for debug/development and might not be
 * available in the released library. 
 */
int xocp_read_reg(uint16_t instance, const char *name, uint32_t *value);

/**
 * @brief Function to read from a register / register field on the device.
 * @param instance Device instance ID (0..N)
 * @param name Register field name
 * @param offset Offset value to be used with register
 * @param value Pointer to write-back the read value
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 * @note
 * See #xocp_read_reg.
 * This function takes an additional offset, which is useful for indexing into
 * a bank/block of registers.
 */
int xocp_read_reg_offset(uint16_t instance,
                         const char *name,
                         uint16_t offset,
                         uint32_t *value);

/**
 * @brief Function to write to a register / register field on the device.
 * @param instance Device instance ID (0..N)
 * @param name Register field name
 * @param value Value to write
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 * @note
 * See #xocp_read_reg.
 */
int  xocp_write_reg(uint16_t instance, const char *name, uint32_t value);

/**
 * @brief Function to write to a register / register field on the device.
 * @param instance Device instance ID (0..N)
 * @param name Register field name
 * @param offset Offset value to be used with register
 * @param value Value to write
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 * @note
 * See #xocp_read_reg.
 * This function takes an additional offset, which is useful for indexing into
 * a bank/block of registers.
 */
int xocp_write_reg_offset(uint16_t instance,
                          const char *name,
                          uint16_t offset,
                          uint32_t value);

/**
 * @brief Puts the device and driver instance into reset.
 * @param instance Device instance ID (0..N)
 * @param mode Mode (0 = reset and release, 1 = hold in reset)
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 * @note
 * When the mode = 1, the module is held in the reset state. Call the
 * function again with mode = 0 to release the reset and to reset normally.
 * This can be useful when coordinating the reset with other devices / modules.
 * Use mode = 0 to perform the reset immediately.
 */
int xocp_reset(uint16_t instance, uint8_t mode);

/**
 * @brief Activate the O-RAN Channel Processor operation.
 * @param instance Device instance ID (0..N)
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 * @note
 * Transitions the SW to operational state.
 */
int xocp_activate(uint16_t instance);

/**
 * @brief Get the component carrier configuration.
 * @param instance Device instance ID (0..N)
 * @param cc Component carrier instance
 * @param data Pointer to write-back the component carrier configuration
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_get_cc_cfg(uint16_t instance,
                    uint8_t cc,
                    struct xocp_cc_data *data);

/**
 * @brief Set the component carrier configuration.
 * @param instance Device instance ID (0..N)
 * @param cc Component carrier instance
 * @param data Pointer to the component carrier configuration
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_set_cc_cfg(uint16_t instance,
                    uint8_t cc,
                    const struct xocp_cc_data *data);

/**
 * @brief Get the antenna configuration.
 * @param instance Device instance ID (0..N)
 * @param data Pointer to write-back the antenna configuration
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_get_antenna_cfg(uint16_t instance,
                         struct xocp_antenna_data *data);

/**
 * @brief Set the antenna configuration.
 * @param instance Device instance ID (0..N)
 * @param data Pointer to the antenna configuration
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_set_antenna_cfg(uint16_t instance,
                         const struct xocp_antenna_data *data);

/**
 * @brief Set the "next" symbol processing schedule.
 * @param instance Device instance ID (0..N)
 * @param mode Mode (1 = DL, 2 = UL, 3 = both DL & UL)
 * @param length Length of sequence
 * @param sequence Pointer to array defining the symbol sequence
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 * @note
 * The HW supports separate schedules for DL and UL. This API call can
 * be used to program them separately (mode 1 or 2) or together (mode 3).
 * The "sequence" parameter is an array (pointer) defining the sequence of
 * component carrier symbols to process, e.g. {0, 0, 1} means a symbol
 * from CC0, another symbol from CC0, and a symbol from CC1.
 * Note, the component carrier IDs used here are the O-RAN CC IDs.
 * Note, the actual symbol sequence needs to be coordinated with the DFE
 * processing chain, and take into account the availability of symbols to/from
 * the Front Haul Interface. This is left to the application SW.
 */
int xocp_set_schedule(uint16_t instance,
                      uint8_t mode,
                      uint8_t length,
                      const uint8_t sequence[]);

/**
 * @brief Get the HW trigger configuration for DL/UL "update".
 * @param instance Device instance ID (0..N)
 * @param triggers Pointer to write-back the trigger configuration
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_get_trigger_cfg(uint16_t instance,
                         struct xocp_triggers *triggers);

/**
 * @brief Set the HW trigger configuration for DL/UL "update".
 * @param instance Device instance ID (0..N)
 * @param triggers Pointer to the trigger configuration
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_set_trigger_cfg(uint16_t instance,
                         const struct xocp_triggers *triggers);

/**
 * @brief Enable the "update" HW trigger.
 * @param instance Device instance ID (0..N)
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 * @note
 * This function enables the "update" HW trigger. On the configured trigger
 * event, the HW will update the "current" register configuration with the
 * values programmed into the "next" registers.
 */
int xocp_trigger_update(uint16_t instance);

/**
 * @brief Clear all "monitor block" counters.
 * @param instance Device instance ID (0..N)
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_monitor_clear(uint16_t instance);

/**
 * @brief Take snapshot of the "monitor block" counters.
 * @param instance Device instance ID (0..N)
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_monitor_snapshot(uint16_t instance);

/**
 * @brief Read specified counter from "monitor block".
 * @param instance Device instance ID (0..N)
 * @param counter Counter to read (see TBD for details)
 * @param value Pointer to write back the counter value
 * @returns
 *      - XOCP_SUCCESS on success
 *      - Error code on failure
 */
int xocp_monitor_read(uint16_t instance, uint8_t counter, uint64_t *value);

#ifdef __cplusplus
}
#endif

#endif // XOCP_API_H

/** @} */