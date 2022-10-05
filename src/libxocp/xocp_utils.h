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
 * @file xocp_utils.h
 * @author Steven Dickinson
 * @brief Header file for libxocp utility functions/definitions.
 * @addtogroup libxocp
 * @{
 */

#ifndef XOCP_UTILS_H
#define XOCP_UTILS_H

#include "xocp.h"

/*******************************************/
/*** Constants / macros / structs / etc. ***/
/*******************************************/

#define MAX_PATH_LENGTH 256

/*****************************/
/*** Function declarations ***/
/*****************************/

/**
 * @brief Get the node property from device tree.
 * @param device Path name of the device (e.g. /sys/bus/platform/devices/XYZ)
 * @param property Name of the property
 * @param dest Pointer to destination buffer
 * @param max_len Maximum size of destination buffer
 * @returns
 *      1 - if the property was successfully retrieved
 *      0 - if the property was not found
 * @note
 * Finds the device tree property, e.g. .../devices/XYZ/of_node/property
 * which is written back to the supplied buffer.
 */
int get_node_property(const char *device,
                      const char *property,
                      void *dest,
                      size_t max_len);

/**
 * @brief Get device name, given hint and/or compatible property.
 * @param dev_name The name of the devivce (or a hint/partial name)
 * @param compatible The compatible property
 * @returns
 *      - Pointer to full device name if a match is found
 *      - NULL if no match is found
 * @note
 * The "dev_name" can the full name, e.g. "a0000000.oran_radio_if" or a
 * partial name, e.g. "oran_radio", or can be left as NULL.
 * The "compatible" property is the Linux device compatible property of
 * the required device, or it can also be NULL.
 * Either "dev_name" or "compatible" must be non-NULL.
 */
const char *get_device_name(const char *dev_name, const char *compatible);

#endif // XOCP_UTILS_H

/** @} */