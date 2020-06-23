// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file ecpri_str.h
* @addtogroup protocol_ecpri
* @{
*
*  A sample command parser for the RoE Framer software modules.
*
******************************************************************************/

/**
 * ECPRI_USAGE_STR Help text for the ecpri module.
 */
#define ECPRI_USAGE_STR "Usage \"ecpri [args]\", try \"ecpri help\" for command list\n"

/**
 * ECPRI_HELP_STR Help text for the ecpri module "help" option.
 */
#define ECPRI_HELP_STR "generates this list of commands\n"

/**
 * ECPRI_OWDM_REQ_STR Help text for the ecpri module "owdm_req" option.
 */
#define ECPRI_OWDM_REQ_STR "ecpri owdm_req <addr> <type> - Request an OWDM measurement to <addr> of type <to_remote|from_remote>\n"

/**
 * ECPRI_OWDM_RES_STR Help text for the ecpri module "owdm_res" option.
 */
#define ECPRI_OWDM_RES_STR "ecpri owdm_res - Returns the result of the last OWDM measurement, or \"Pending\" if still underway\n"

/**
 * ECPRI_OWDM_LIMIT_STR Help text for the ecpri module "owdm_limit" option.
 */
#define ECPRI_OWDM_LIMIT_STR "ecpri owdm_limit - Sets the report threshold of the OWDM measurement (nsecs), or disables reporting if 0\n"

/**
 * ECPRI_RMA_READ_STR Help text for the ecpri module "rma_read" option.
 */
#define ECPRI_RMA_READ_STR "ecpri rma_read <ip_addr> <mem_addr> <length> - Request a read of <length> bytes at <mem_addr> from <ip_addr>\n"

/**
 * ECPRI_RMA_WRITE_STR Help text for the ecpri module "rma_write" option.
 */
#define ECPRI_RMA_WRITE_STR "ecpri rma_write <ip_addr> <mem_addr> <length> \"<bytes 1..length>\" - Request a write of <length> <bytes> to <mem_addr> at <ip_addr>\n"

/**
 * ECPRI_TEST_MESG_STR Help text for the ecpri module "test_mesg" option.
 */
#define ECPRI_TEST_MESG_STR "ecpri test_mesg <ip_addr> <dest_port> - TESTING - Send a test message to <dest_port>\n"

/**
 * ECPRI_RMR_REQ_STR Help text for the ecpri module "rmr_req" option.
 */
#define ECPRI_RMR_REQ_STR "ecpri rmr_req <ip_addr> - Request a remote reset of <ip_addr>\n"
/** @} */