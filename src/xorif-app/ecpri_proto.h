// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file ecpri_proto.h
* @addtogroup protocol_ecpri
* @{
*
*  Sample eCPRI protocol library.
*
******************************************************************************/

/***************************** Include Files *********************************/
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * ECPRI_PROTO_MAGIC_BYTE Magic eCPRI message byte.
 */
#define ECPRI_PROTO_MAGIC_BYTE (0x10)

/**
 * ECPRI_PROTO_HEADER_SIZE Size of the eCPRI message header.
 */
#define ECPRI_PROTO_HEADER_SIZE (4)

/**
 * ecpri_message_type_t The types of eCPRI user-plane message.
 */
typedef enum ecpri_message_type_e
{
	ECPRI_MSG_IQ_DATA, /**< IQ Data */
	ECPRI_MSG_BIT_SEQ, /**< Bit Sequence */
	ECPRI_MSG_RTC_DATA, /**< Real-Time Control Data */
	ECPRI_MSG_GENERIC_DATA, /**< Generic Data */
	ECPRI_MSG_RMA, /**< Remote Memory Access */
	ECPRI_MSG_OWDM, /**< One-Way Delay Measurement */
	ECPRI_MSG_REM_RESET, /**< Remote Reset */
	ECPRI_MSG_EVENT /**< Event Indication */
} ecpri_message_type_t;

/**
 * ecpri_header_t The eCPRI user-plane message header.
 */
typedef struct ecpri_header_s 
{
	uint8_t magic; /**< Protocol revision and continuation byte */
	uint8_t type; /**< Message type @see ecpri_message_type_t */
	uint16_t length; /**< Payload length */
} ecpri_header_t;

/**
 * ECPRI_RMA_MSG_READ eCPRI RMA read flag.
 */
#define ECPRI_RMA_MSG_READ (0)

/**
 * ECPRI_RMA_MSG_WRITE eCPRI RMA write flag.
 */
#define ECPRI_RMA_MSG_WRITE (0x10)

/**
 * ECPRI_RMA_MSG_WR_NO_RESP eCPRI RMA write with no response flag.
 */
#define ECPRI_RMA_MSG_WR_NO_RESP (0x20)

/**
 * ECPRI_RMA_MSG_REQ eCPRI RMA request flag.
 */
#define ECPRI_RMA_MSG_REQ (0)

/**
 * ECPRI_RMA_MSG_RESP eCPRI RMA response flag.
 */
#define ECPRI_RMA_MSG_RESP (0x1)

/**
 * ECPRI_RMA_MSG_FAIL eCPRI RMA request fail flag.
 */
#define ECPRI_RMA_MSG_FAIL (0x2)

/**
 * ecpri_rma_msg_t eCPRI RMA message structure.
 */
typedef struct ecpri_rma_msg_s
{
	uint8_t id; /**< Access ID */
	uint8_t rd_wr_req_resp; /**< Read/Write nibble | Request/Response nibble */
	uint16_t element_id; /**< Element ID */
	uint8_t address[6]; /**< Memory Address */
	uint16_t length; /**< Length of read/write */
} ecpri_rma_msg_t;

/**
 * ECPRI_OWDM_MSG_ACTION_REQ eCPRI OWDM action request flag.
 */
#define ECPRI_OWDM_MSG_ACTION_REQ (0)

/**
 * ECPRI_OWDM_MSG_ACTION_REQ_FOL_UP eCPRI OWDM action req with follow-up flag.
 */
#define ECPRI_OWDM_MSG_ACTION_REQ_FOL_UP (0x1)

/**
 * ECPRI_OWDM_MSG_ACTION_RESP eCPRI OWDM action response flag.
 */
#define ECPRI_OWDM_MSG_ACTION_RESP (0x2)

/**
 * ECPRI_OWDM_MSG_ACTION_REM_REQ eCPRI OWDM remote action request flag.
 */
#define ECPRI_OWDM_MSG_ACTION_REM_REQ (0x3)

/**
 * ECPRI_OWDM_MSG_ACTION_REM_REQ_FOL_UP OWDM rem act req with follow-up flag.
 */
#define ECPRI_OWDM_MSG_ACTION_REM_REQ_FOL_UP (0x4)

/**
 * ECPRI_OWDM_MSG_ACTION_FOL_UP eCPRI OWDM follow-up flag.
 */
#define ECPRI_OWDM_MSG_ACTION_FOL_UP (0x5)

/**
 * ecpri_owdm_msg_t eCPRI OWDM message structure.
 */
typedef struct ecpri_owdm_msg_s
{
	uint8_t id; /**< Measurement ID */
	uint8_t action_type; /**< Action Type */
	uint8_t ts_sec[6]; /**< Time-stamp (seconds) */
	uint8_t ts_nsec[4]; /**< Time-stamp (nano-seconds) */
	uint8_t comp[8]; /**< Compensation value */
} ecpri_owdm_msg_t;

/**
 * ecpri_owdm_msg_t eCPRI OWDM direction.
 */
typedef enum ecpri_owdn_direction
{
	TO_REMOTE, /**< Measure to remote from originator */
	FROM_REMOTE /**< Measure to originator from remote */
} ecpri_owdm_direction_type;

/**
 * ECPRI_RMR_MSG_CODE_OP_REM_RESET_REQ eCPRI RMR request flag.
 */
#define ECPRI_RMR_MSG_CODE_OP_REM_RESET_REQ (0x1)

/**
 * ECPRI_RMR_MSG_CODE_OP_REM_RESET_RESP eCPRI RMR response flag.
 */
#define ECPRI_RMR_MSG_CODE_OP_REM_RESET_RESP (0x2)

/**
 * ecpri_rmr_msg_t eCPRI RMR message structure.
 */
typedef struct ecpri_rmr_msg_s
{
	uint16_t id; /**< Reset ID */
	uint8_t code_op; /**< Reset Operation Code (Request/Indication) */
} ecpri_rmr_msg_t;

/************************** Function Prototypes ******************************/
int proto_ecpri_rma_send_request(int type, uint16_t data_len, struct sockaddr_in *dest, uint64_t offset, uint8_t *values);
int proto_ecpri_rma_get_response(int type, int *length, struct sockaddr_in *src, uint8_t **values);
int proto_ecpri_owdm_send_request(uint8_t type, struct sockaddr_in *dest);
int proto_ecpri_handle_incoming_msg(int fd, short revents, char *command);
void proto_ecpri_get_owdm_result(int *req_no, int *resp_no, ecpri_owdm_direction_type *direction, 
								struct in_addr *node, unsigned long long *secs, unsigned long *nsecs);
void proto_ecpri_set_owdm_limit(long limit);
int proto_ecpri_test_mesg_send(struct sockaddr_in *dest);
int proto_ecpri_rmr_send_request(struct sockaddr_in *dest);
int proto_ecpri_rmr_get_response(struct sockaddr_in *src);

// HACK: Map these to the correct socket & port values
#define sock_ip udp_socket_fd
#define port_ip htons(port)
extern int udp_socket_fd;
extern int port;

// HACK: These functions are currently not needed
#define TRAFGEN_SYSFS_API_Write(a, b)
#define IP_API_Read(a, b, c) (*b)
#define IP_API_Write(a, b, c) (*b)

// HACK: Use this definition for the command structure
typedef int (*CommandFunc)(int argc, char **argv, char *resp);
typedef struct _commands_t
{
	const char *cmd;
	const char *helptxt;
	CommandFunc func;
} commands_t;

/** @} */
