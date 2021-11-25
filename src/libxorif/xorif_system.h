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
#define MIN_NUM_RBS 1    /**< Minimum number of RBs supported per CC */
#define SSB_NUM_RBS 20   /**< Number of RBs for SSB */
#define RE_PER_RB 12     /**< Number of REs per RB */

// Configuration defaults
#define DEFAULT_DELAY_COMP 30.0      /**< Default delay compensation time in microseconds */
#define DEFAULT_ADVANCE_UL 90.0      /**< Default uplink control time advance in microseconds */
#define DEFAULT_ADVANCE_DL 90.0      /**< Default downlink control time advance in microseconds */
#define DEFAULT_UL_BID_FWD 90.0      /**< Default uplink beam-id forward time in microseconds */
#define DEFAULT_CTRL_PER_SYM 64      /**< Default control words per symbol */
#define DEFAULT_SECT_PER_SYM 64      /**< Default sections per symbol */
#define DEFAULT_FRAMES_PER_SYM 15    /**< Default Ethernet frames per symbol */
#define DEFAULT_CTRL_PER_SYM_SSB 5   /**< Default control words per symbol (SSB) */
#define DEFAULT_SECT_PER_SYM_SSB 5   /**< Default sections per symbol (SSB) */
#define DEFAULT_FRAMES_PER_SYM_SSB 3 /**< Default Ethernet frames per symbol (SSB) */

// Timing delay defaults
#define DEFAULT_FH_DECAP_DLY 5.0    /**< Estimate of downlink delay (in microseconds). See PG370. */
#define DEFAUT_UL_RADIO_CH_DLY 30.0 /**< Estimate of uplink delay (in microseconds). See PG370.*/

#endif // XORIF_SYSTEM_H

/** @} */