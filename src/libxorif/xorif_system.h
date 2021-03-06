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
 * @file xorif_system.h
 * @author Steven Dickinson
 * @brief Header file defines various system constants and configuration parameters.
 * @addtogroup libxorif
 * @{
 *
 * C Library API for the Xilinx ORAN Radio Interface (libxorif)
 */

#ifndef XORIF_SYSTEM_H
#define XORIF_SYSTEM_H

#include <inttypes.h>

// System constants
#define NUM_NUMEROLOGY 5 /**< Number of numerologies */
#define MAX_NUM_CC 8     /**< Maximum number of component carriers */
#define MAX_NUM_RBS 275  /**< Maximum number of RBs supported per CC */
#define MIN_NUM_RBS 2    /**< Minimum number of RBs supported per CC */
#define SSB_NUM_RBS 20   /**< Number of RBs for SSB */
#define RE_PER_RB 12     /**< Number of REs per RB */

// Configuration defaults
#define DEFAULT_DESKEW_TIME 30       /**< Default deskew time in microseconds */
#define DEFAULT_ADVANCE_UL 90        /**< Default uplink time advance in microseconds */
#define DEFAULT_ADVANCE_DL 90        /**< Default downlink time advance in microseconds */
#define DEFAULT_CTRL_PER_SYM 64      /**< Default sections per symbol */
#define DEFAULT_FRAMES_PER_SYM 15    /**< Default Ethernet frames per symbol */
#define DEFAULT_CTRL_PER_SYM_SSB 5   /**< Default sections per symbol (SSB) */
#define DEFAULT_FRAMES_PER_SYM_SSB 3 /**< Default Ethernet frames per symbol (SSB) */

// Timing delay parameters
#define FH_DECAP_DLY_PS 5000000     /**< Estimate of downlink delay (in picoseconds). See PG370. */
#define UL_RADIO_CH_DLY_PS 30000000 /**< Estimate of uplink delay (in picoseconds). See PG370.*/

#endif // XORIF_SYSTEM_H

/** @} */