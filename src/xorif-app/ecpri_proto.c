// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file ecpri_proto.c
* @addtogroup protocol_ecpri
* @{
*
*  Sample eCPRI protocol library.
*
******************************************************************************/

/***************************** Include Files *********************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <linux/net_tstamp.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <poll.h>
#include <errno.h>
#include <linux/errqueue.h>
#include <inttypes.h>
#include <ecpri_proto.h>


/************************** Function Prototypes ******************************/
int proto_ecpri_handle_incoming_rma(uint8_t *buffer, uint16_t data_len, int fd, struct sockaddr_in *src);
int proto_ecpri_handle_incoming_owdm(uint8_t *buffer, uint16_t data_len, int fd, struct sockaddr_in *src, struct timespec *ts);
int proto_ecpri_handle_incoming_rmr(uint8_t *buffer, uint16_t data_len, int fd, struct sockaddr_in *src);
int proto_ecpri_handle_incoming_event(uint8_t *buffer, uint16_t data_len, int fd, struct sockaddr_in *src);
int proto_ecpri_handle_incoming_test_mesg(uint8_t *buffer, uint16_t data_len, int fd, struct sockaddr_in *src);

/**
 * Sequence number for eCPRI test messages.
 */
int SEQ_NUM = 0;


/**
 * Structure for holding an OWDM result message.
 */
typedef struct owdm_result
{
	struct sockaddr_in node;  /**< Address of other node */
	ecpri_owdm_direction_type direction;  /**< Direction of measurement (TO_REMOTE or FROM_REMOTE)*/
	int resp_num;  /**< The current response number */
	uint8_t ts_sec[6];  /**< Seconds portion of delay */
	uint8_t ts_nsec[4];  /**< Nano-seconds portion of delay */
} owdm_result_type;
 
/**
 * owdm_ts_msg_type Structure for holding an OWDM timestamp.
 */
typedef struct owdm_ts_msg
{
	/*@{*/
	owdm_result_type ts;  /**< Time-stamp */
	uint8_t comp[8];  /**< Compensation value */
	/*}@*/
} owdm_ts_msg_type;

/**
 * Number of OWDM requests sent by this node.
 */
int owdm_req = 0;

/**
 * OWDM result storage.
 */
owdm_result_type owdm_result;

/**
 * OWDM message storage.
 */
owdm_ts_msg_type owdm_msg;

/**
 * OWDM local delay compensation value.
 */
uint8_t owdm_comp[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/**
 * OWDM measurement reporting limit (nsecs).
 */
long ecpri_report_limit = 0;

/*****************************************************************************/
/**
*
* Sends an eCPRI protocol message to a remote node.
* This takes the payload and adds the eCPRI message header before transmission.
* 
*
* @param [in]	data   		Pointer to the message payload.
* @param [in]	length		Length of the payload.
* @param [in]	type		eCPRI message type.
* @param [in]	sock_d		File handle of the outbound socket.
* @param [in]	dest		IP address of remote node.
*
* @return
*		- 0 if malloc fails.
*		- Return value of sendto().
*
******************************************************************************/
int proto_ecpri_send(uint8_t *data, uint16_t length, ecpri_message_type_t type, int sock_d, struct sockaddr_in *dest)
{
	uint8_t *buffer;
	ecpri_header_t *header;
	int retval = 0;
	int buflen = length + ECPRI_PROTO_HEADER_SIZE;

	buffer = malloc(buflen);

	if(buffer)
	{
		header = (ecpri_header_t *)buffer;

		header->magic = ECPRI_PROTO_MAGIC_BYTE;
		header->type = type;
		header->length = length;

		memcpy(buffer+ECPRI_PROTO_HEADER_SIZE, data, length);
		retval = sendto(sock_d, buffer, buflen, 0, (struct sockaddr *)dest, sizeof(struct sockaddr_in));

		free(buffer);
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Receives a message on the eCPRI protocol port.
* This retrieves a message from the given remote IP address, checks if it is a 
* valid eCPRI message, then returns the payload and the packet rx time-stamp 
* (if present).
* 
*
* @param [out]	data   		Pointer to memory buffer allocated by this function
*                           containing the payload data.
* @param [out]	length		Length of payload received.
* @param [out]	type		Received eCPRI message type.
* @param [in]	sock_d		File handle of the receiving eCPRI socket.
* @param [in]	src			IP address of remote node.
* @param [out]	ts			Pointer to packet rx time-stamp.
*
* @return
*		- 0 if short message received.
*		- -1 if short socket control message received or malloc fails.
*		- 1 if time-stamp received.
*		- length of payload otherwise.
*
******************************************************************************/
int proto_ecpri_recv(uint8_t **data, uint16_t *length, ecpri_message_type_t *type, int sock_d, struct sockaddr_in *src, uint8_t *ts)
{
	ecpri_header_t *header;
	uint32_t size = sizeof(struct sockaddr_in);
	int retval = 0;
	uint8_t buffer[1024];
	char control[256];
	int recv_len;
	struct cmsghdr *cm;
	struct msghdr msg;
	int level;
	int mtype;
	struct iovec iov = { buffer, 1024 };

	memset(control, 0, sizeof(control));
	memset(&msg, 0, sizeof(msg));

	msg.msg_name = src;
	msg.msg_namelen = size;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = control;
	msg.msg_controllen = 256;

	/* Get the incoming message(s) */
	recv_len = recvmsg(sock_d, &msg, MSG_DONTWAIT);

	if(recv_len >= ECPRI_PROTO_HEADER_SIZE)
	{
		header = (ecpri_header_t *)buffer;

		if(header->magic == ECPRI_PROTO_MAGIC_BYTE)
		{
			*type = header->type;
			*length = header->length;

			*data = malloc(header->length);
			if(*data)
			{
				memcpy(*data, buffer+sizeof(ecpri_header_t), header->length);
				retval = *length;
			}
			else
			{
				retval = -1;
			}
		}
	}

	if(recv_len >= 0)
	{
		for (cm = CMSG_FIRSTHDR(&msg); cm != NULL; cm = CMSG_NXTHDR(&msg, cm))
		{
			level = cm->cmsg_level;
			mtype  = cm->cmsg_type;

			if (SOL_SOCKET == level && SO_TIMESTAMPING == mtype)
			{
				if (cm->cmsg_len < sizeof(struct timespec) * 3)
				{
					syslog(LOG_ERR, "short SO_TIMESTAMPING message\n");
					return -1;
				}
				if(NULL != ts)
				{
					memcpy(ts, CMSG_DATA(cm), sizeof(struct timespec)*3);
					retval = 1;
				}
			}
			if (SOL_SOCKET == level && SO_TIMESTAMPNS == mtype)
			{
				if (cm->cmsg_len < sizeof(struct timespec))
				{
					syslog(LOG_ERR, "short SO_TIMESTAMPNS message\n");
					return -1;
				}
			}
			if (SOL_IP == level && IP_RECVERR == mtype)
			{
				if (cm->cmsg_len < sizeof(struct sock_extended_err))
				{
					syslog(LOG_ERR, "short IP_RECVERR message\n");
					return -1;
				}
			}
		}
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Receives a message on the eCPRI protocol port.
* This retrieves a message from the given remote IP address, checks if it is a 
* valid eCPRI message, then returns the payload and the packet rx time-stamp 
* (if present).
* 
*
* @param [in]	sock_d		File handle of the receiving eCPRI socket.
* @param [in]	errQueue	IP address of remote node.
* @param [out]	ts			Pointer to packet rx time-stamp.
*
* @return
*		- 0 if short message received.
*		- -1 if short socket control message received or malloc fails.
*		- 1 if time-stamp received.
*		- length of payload otherwise.
*
******************************************************************************/
int proto_ecpri_handle_timestamps(int sock_d, int errQueue, uint8_t *ts)
{
	char control[256];
	uint8_t buffer[1024];
	int recv_len = -1;
	int i;
	struct cmsghdr *cm;
	struct msghdr msg;
	int level;
	int mtype;
	struct iovec iov = { buffer, 1024 };
	int retval = 0;
	int flags = 0;

	flags |= (errQueue?MSG_ERRQUEUE:0);

	memset(control, 0, sizeof(control));
	memset(&msg, 0, sizeof(msg));

	msg.msg_name = NULL;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = control;
	msg.msg_controllen = 256;

	/* Get the incoming message(s) */
	for(i=0; (i<10000) && (recv_len < 0); i++)
	{
		recv_len = recvmsg(sock_d, &msg, flags);
		usleep(10);
	}
	if(recv_len >= 0)
	{
		for (cm = CMSG_FIRSTHDR(&msg); cm != NULL; cm = CMSG_NXTHDR(&msg, cm))
		{
			level = cm->cmsg_level;
			mtype  = cm->cmsg_type;

			if (SOL_SOCKET == level && SO_TIMESTAMPING == mtype)
			{
				if (cm->cmsg_len < sizeof(struct timespec) * 3)
				{
					syslog(LOG_ERR, "short SO_TIMESTAMPING message\n");
					return -1;
				}
				if(NULL != ts)
				{
					memcpy(ts, CMSG_DATA(cm), sizeof(struct timespec)*3);
				}
			}
			if (SOL_SOCKET == level && SO_TIMESTAMPNS == mtype)
			{
				if (cm->cmsg_len < sizeof(struct timespec))
				{
					syslog(LOG_ERR, "short SO_TIMESTAMPNS message\n");
					return -1;
				}
			}
			if (SOL_IP == level && IP_RECVERR == mtype)
			{
				if (cm->cmsg_len < sizeof(struct sock_extended_err))
				{
					syslog(LOG_ERR, "short IP_RECVERR message\n");
					return -1;
				}
			}
		}
	}
	else
	{
		syslog(LOG_ERR, "No message received, err %x\n", errno);
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Sends an RMA request to a remote node.
* 
*
* @param [in]		type		Type of RMA message (Read or Write).
* @param [in]		data_len	Length in bytes to read/write.
* @param [in]		dest		IP address of remote node.
* @param [in]		offset		Remote memory address to read/write.
* @param [in,out]	values 		Pointer to buffer for read/write.
*
* @return
*		- -1 if malloc fails.
*		- Return value of proto_ecpri_send().
*
******************************************************************************/
int proto_ecpri_rma_send_request(int type, uint16_t data_len, struct sockaddr_in *dest, uint64_t offset, uint8_t *values)
{
	uint8_t *buffer;
	int buflen = 0;
	ecpri_rma_msg_t *header;
	int retval = 0;

	if(type == ECPRI_RMA_MSG_WRITE)
	{
		buflen = data_len + sizeof(ecpri_rma_msg_t);
	}
	else
	{
		buflen = sizeof(ecpri_rma_msg_t);
	}

	buffer = malloc(buflen);

	if(buffer)
	{
		header = (ecpri_rma_msg_t *)buffer;
		header->id = 0;
		header->rd_wr_req_resp = type | ECPRI_RMA_MSG_REQ;
		header->element_id = 0;
		header->address[0] = (uint8_t)(offset & 0xff);
		header->address[1] = (uint8_t)((offset>>8) & 0xff);
		header->address[2] = (uint8_t)((offset>>16) & 0xff);
		header->address[3] = (uint8_t)((offset>>24) & 0xff);
		header->address[4] = (uint8_t)((offset>>32) & 0xff);
		header->address[5] = (uint8_t)((offset>>40) & 0xff);
		header->length = data_len;

		if(type == ECPRI_RMA_MSG_WRITE)
		{
			memcpy(buffer + sizeof(ecpri_rma_msg_t), values, data_len);
		}

		retval = proto_ecpri_send(buffer, buflen, ECPRI_MSG_RMA, sock_ip, dest);

		free(buffer);
	}
	else
	{
		retval = -1;
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Receives an RMA response from a remote node.
* 
*
* @param [in]	type		Type of RMA message expected (Read or Write).
* @param [out]	length		Length in bytes read/written.
* @param [in]	src			IP address of remote node to receive from.
* @param [out]	values 		Pointer to buffer to put read data in.
*
* @return
*		- -1 if malloc fails.
*		- 0 on success.
*		- Return value of proto_ecpri_recv() if not an RMA message.
*
******************************************************************************/
int proto_ecpri_rma_get_response(int type, int *length, struct sockaddr_in *src, uint8_t **values)
{
	uint16_t data_len;
	ecpri_message_type_t msg_type;
	uint8_t *buffer=NULL;
	int retval = 0;
	ecpri_rma_msg_t *header;

	while(retval==0)
	{
		retval = proto_ecpri_recv(&buffer, &data_len, &msg_type, sock_ip, src, NULL);
	}

	if(msg_type == ECPRI_MSG_RMA)
	{
		header = (ecpri_rma_msg_t *)buffer;
		if(header->rd_wr_req_resp == (type | ECPRI_RMA_MSG_RESP))
		{
			if(type == ECPRI_RMA_MSG_READ)
			{
				*values = malloc(header->length);
				if(*values)
				{
					memcpy(*values, buffer + sizeof(ecpri_rma_msg_t), header->length);
					*length = header->length;
					retval = 0;
				}
				else
				{
					retval = -1;
				}
			}
			else
			{
				*length = header->length;
				retval = 0;
			}
		}
	}
	else
	{
		/* Not for us - oops */
	}

	if(buffer)
	{
		free(buffer);
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Send an OWDM request and optionally the tx time-stamp.
* Sends a One-Way Delay Measurement request to a remote node, reads and saves
* the tx time-stamp for internal use, and if send_ts is non-zero sends a OWDM
* follow-up message to the remote node with the time-stamp.
*
* @param [in]	msg		Pointer to OWDM request message.
* @param [in]	len		Length of message.
* @param [in]	dest	IP address of remote node to send to.
* @param [in]	send_ts	Pointer to buffer to put read data in.
*
* @return
*		- Return value of proto_ecpri_send() for message if <= 0.
*		- Return value of proto_ecpri_handle_timestamps() if < 0 or !send_ts.
*		- Return value of proto_ecpri_send() for followup otherwise.
*
******************************************************************************/
int proto_ecpri_owdm_send_req_get_ts(uint8_t *msg, uint16_t len, struct sockaddr_in *dest, int send_ts)
{
	ecpri_owdm_msg_t *message = (ecpri_owdm_msg_t *)msg;
	int retval = 0;
	struct timespec ts[3];
	int i;

	retval = proto_ecpri_send(msg, len, ECPRI_MSG_OWDM, sock_ip, dest);

	if(send_ts && (retval > 0))
	{
		retval = proto_ecpri_handle_timestamps(sock_ip, 1, (uint8_t *)ts);

		if(retval >= 0)
		{
			for(i=0; i<6; i++)
			{
				message->ts_sec[i] = (uint8_t)(ts[2].tv_sec>>(8*i))&0xff;
			}
			for(i=0; i<4; i++)
			{
				message->ts_nsec[i] = (uint8_t)(ts[2].tv_nsec>>(8*i))&0xff;
			}
			memcpy(message->comp, owdm_comp, 8);

			/* Save TS for ourselves */
			memcpy(owdm_msg.ts.ts_sec, message->ts_sec, 6);
			memcpy(owdm_msg.ts.ts_nsec, message->ts_nsec, 4);
			memcpy(owdm_msg.comp, message->comp, 8);

			message->action_type = ECPRI_OWDM_MSG_ACTION_FOL_UP;
			retval = proto_ecpri_send(msg, len, ECPRI_MSG_OWDM, sock_ip, dest);
		}
		else
		{
			syslog(LOG_ERR, "proto_ecpri_owdm_send_req_get_ts: error getting timestamp\n");
		}
	}

	if(retval > 0)
	{
		/* Flush Tx timestamp from buffer */
		retval = proto_ecpri_handle_timestamps(sock_ip, 1, NULL);
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Send an OWDM request.
*
* @param [in]	type	Type of OWDM request message.
* @param [in]	dest	IP address of remote node to send to.
*
* @see ecpri_proto.h
* @return
*		- -1 if type not supported or recognised.
*		- Return value of proto_ecpri_owdm_send_req_get_ts() otherwise.
*
******************************************************************************/
int proto_ecpri_owdm_send_request(uint8_t type, struct sockaddr_in *dest)
{
	static uint8_t id = 0;
	ecpri_owdm_msg_t message;
	int retval = 0;

	memset(&message, 0, sizeof(message));
	message.id = id;
	message.action_type = type;

	switch(type)
	{
		case ECPRI_OWDM_MSG_ACTION_REQ:
		case ECPRI_OWDM_MSG_ACTION_REM_REQ:
			/* Not supported */
			retval = -1;
			break;

		case ECPRI_OWDM_MSG_ACTION_REQ_FOL_UP:
			retval = proto_ecpri_owdm_send_req_get_ts((uint8_t *)&message, (uint16_t)sizeof(message), dest, 1);
			owdm_req++;
			break;

			case ECPRI_OWDM_MSG_ACTION_REM_REQ_FOL_UP:
			retval = proto_ecpri_owdm_send_req_get_ts((uint8_t *)&message, (uint16_t)sizeof(message), dest, 0);
			owdm_req++;
			break;

		default:
			/* Unknown type */
			retval = -1;
			break;
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Calculates the difference between two time-stamps as an OWDM result.
*
* @param [in]	t1			Start time-stamp.
* @param [in]	t2			End time-stamp.
* @param [in]	owdm_result	Difference between t1 and t2.
*
* @return
*		- 1.
*
******************************************************************************/
int ecpri_owdm_calc_delay(owdm_ts_msg_type *t1, owdm_ts_msg_type *t2, owdm_result_type *owdm_result)
{
	unsigned long long t1_secs = 0;
	unsigned long long t2_secs = 0;
	unsigned long long res_secs = 0;
	unsigned long t1_nsecs = 0;
	unsigned long t2_nsecs = 0;
	unsigned long res_nsecs = 0;
	int i;

	for(i=0; (i<sizeof(long long)) && (i<6); i++)
	{
		t1_secs |= ((uint8_t)(t1->ts.ts_sec[i]&0xff))<<(8*i);
		t2_secs |= ((uint8_t)(t2->ts.ts_sec[i]&0xff))<<(8*i);
	}
	for(i=0; (i<sizeof(long)) && (i<4); i++)
	{
		t1_nsecs |= ((uint8_t)(t1->ts.ts_nsec[i]&0xff))<<(8*i);
		t2_nsecs |= ((uint8_t)(t2->ts.ts_nsec[i]&0xff))<<(8*i);
	}

	if(t2_nsecs > t1_nsecs)
	{
		t1_nsecs += 1000000000;
		t1_secs -= 1;
	}

	res_secs = t1_secs - t2_secs;
	res_nsecs = t1_nsecs - t2_nsecs;

	if(ecpri_report_limit)
	{
		if((res_secs>0) || (res_nsecs>ecpri_report_limit))
		{
			/* Hit that ILA here! */
			TRAFGEN_SYSFS_API_Write("sw_trigger", "1");
		}
	}

	for(i=0; (i<sizeof(long long)) && (i<6); i++)
	{
		owdm_result->ts_sec[i] = (uint8_t)(res_secs>>(8*i))&0xff;
	}
	for(i=0; (i<sizeof(long)) && (i<4); i++)
	{
		owdm_result->ts_nsec[i] = (uint8_t)(res_nsecs>>(8*i))&0xff;
	}

	return 1;
}

/*****************************************************************************/
/**
*
* Send a Remote Reset request to a remote node.
*
* @param [in]	dest	IP address of the remote node.
*
* @return
*		- -1 if malloc fails.
*		- return value of proto_ecpri_send() otherwise.
*
******************************************************************************/
int proto_ecpri_rmr_send_request(struct sockaddr_in *dest)
{
	uint8_t *buffer;
	int buflen = 0;
	ecpri_rmr_msg_t *header;
	int retval = 0;

	buflen = sizeof(ecpri_rmr_msg_t);

	buffer = malloc(buflen);

	if(buffer)
	{
		header = (ecpri_rmr_msg_t *)buffer;
		header->id = 0;
		header->code_op = ECPRI_RMR_MSG_CODE_OP_REM_RESET_REQ;

		retval = proto_ecpri_send(buffer, buflen, ECPRI_MSG_REM_RESET, sock_ip, dest);

		free(buffer);
	}
	else
	{
		retval = -1;
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Get a Remote Reset response from a remote node.
*
* @param [in]	src	IP address of the remote node.
*
* @return
*		- 0 on valid response.
*		- -1 other Remote Reset message.
*		- return value of proto_ecpri_recv() otherwise.
*
******************************************************************************/
int proto_ecpri_rmr_get_response(struct sockaddr_in *src)
{
	uint16_t data_len;
	ecpri_message_type_t msg_type;
	uint8_t *buffer=NULL;
	int retval = 0;
	ecpri_rmr_msg_t *header;

	while(retval==0)
	{
		retval = proto_ecpri_recv(&buffer, &data_len, &msg_type, sock_ip, src, NULL);
	}

	if(msg_type == ECPRI_MSG_REM_RESET)
	{
		header = (ecpri_rmr_msg_t *)buffer;
		if(header->code_op == ECPRI_RMR_MSG_CODE_OP_REM_RESET_RESP)
		{
			retval = 0;
		}
		else
		{
			retval = -1;
		}
	}
	else
	{
		/* Not for us - oops */
	}

	if(buffer)
	{
		free(buffer);
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Handle an eCPRI message received on the UDP/IP socket.
*
* @param [in]	fd		File handle of receiving socket.
* @param [in]	revents	receive events on socket.
* @param [in]	command	Ignored.
*
* @return
*		- return value of proto_ecpri_handle_timestamps() if revent is POLLERR.
*		- return value of proto_ecpri_recv() on POLLIN error.
*		- return value of appropriate handler function if recognised.
*		- 0 on unhandled message type or revent not POLLIN or POLLERR.
*
******************************************************************************/
int proto_ecpri_handle_incoming_msg(int fd, short revents, char *command)
{
	int retval = 0;
	uint8_t *buffer=NULL;
	uint16_t data_len;
	ecpri_message_type_t type;
	struct sockaddr_in src;
	struct timespec ts[3];
	(void)command; /* We might want to pass this message out to the system later? */

	if(revents & POLLIN)
	{
		retval = proto_ecpri_recv(&buffer, &data_len, &type, fd, &src, (uint8_t *)ts);

		if(retval > 0)
		{
			switch(type)
			{
				case ECPRI_MSG_RMA:
					retval = proto_ecpri_handle_incoming_rma(buffer, data_len, fd, &src);
					break;

				case ECPRI_MSG_OWDM:
					retval = proto_ecpri_handle_incoming_owdm(buffer, data_len, fd, &src, ts);
					break;

				case ECPRI_MSG_REM_RESET:
					retval = proto_ecpri_handle_incoming_rmr(buffer, data_len, fd, &src);
					break;

				case ECPRI_MSG_EVENT:
					retval = proto_ecpri_handle_incoming_event(buffer, data_len, fd, &src);
					break;

				case ECPRI_MSG_GENERIC_DATA:
					retval = proto_ecpri_handle_incoming_test_mesg(buffer, data_len, fd, &src);
					break;
				case ECPRI_MSG_IQ_DATA:
				case ECPRI_MSG_BIT_SEQ:
				case ECPRI_MSG_RTC_DATA:
				default:
					syslog(LOG_ERR, "proto_ecpri_handle_incoming_msg: unhandled message type %d received\n", type);
					retval = 0;
					break;
			}
		}
	}
	else if(revents & POLLERR)
	{
		retval = proto_ecpri_handle_timestamps(fd, 1, (uint8_t *)ts);
	}


	if(buffer)
	{
		free(buffer);
	}
	return retval;
}

/*****************************************************************************/
/**
*
* Handle an incoming eCPRI Remote Memory Access message.
*
* @param [in]	buffer		Buffer containing incoming message.
* @param [in]	data_len	Ignored.
* @param [in]	fd			File handle of receiving/response socket.
* @param [in]	src			IP address of remote node for replies.
*
* @return
*		- return value of IP_API_Write() on write.
*		- -1 on malloc failure.
*		- 0 on unhandled message type or read success.
*
******************************************************************************/
int proto_ecpri_handle_incoming_rma(uint8_t *buffer, uint16_t data_len, int fd, struct sockaddr_in *src)
{
	ecpri_rma_msg_t *header;
	uint8_t type;
	uint8_t id;
	uint16_t length;
	int addr;
	uint8_t *data_ptr;
	uint8_t *resp_msg;
	int resp_len = 0;
	int retval = 0;
	int ret = 0;

	(void)data_len;

	header = (ecpri_rma_msg_t *)buffer;
	type = header->rd_wr_req_resp;
	id = header->id;
	length = header->length;
	addr = header->address[0] | (header->address[1]<<8) | (header->address[2]<<16) | (header->address[3]<<24);

	switch(type)
	{
		case ECPRI_RMA_MSG_READ:
			resp_len = length + sizeof(ecpri_rma_msg_t);
			resp_msg = malloc(resp_len);
			if(resp_msg)
			{
				memset(resp_msg, 0, resp_len);
				data_ptr = resp_msg + sizeof(ecpri_rma_msg_t);

				ret = IP_API_Read(addr, data_ptr, length);
				if(ret)
				{
					syslog(LOG_ERR, "proto_ecpri_handle_incoming_rma: IP_API_Read() returned: 0x%x\n", ret);
				}

				/* Add RMA header... */
				header = (ecpri_rma_msg_t *)resp_msg;
				header->id = 0;
				header->rd_wr_req_resp = ECPRI_RMA_MSG_READ | ECPRI_RMA_MSG_RESP;
				header->element_id = id;
				header->address[0] = (uint8_t)(addr & 0xff);
				header->address[1] = (uint8_t)((addr>>8) & 0xff);
				header->address[2] = (uint8_t)((addr>>16) & 0xff);
				header->address[3] = (uint8_t)((addr>>24) & 0xff);
				header->address[4] = 0;
				header->address[5] = 0;
				header->length = length;

				/* Send RMA response */
				proto_ecpri_send(resp_msg, resp_len, ECPRI_MSG_RMA, fd, src);
				free(resp_msg);
			}
			else
			{
				retval = -1;
			}
			break;

		case ECPRI_RMA_MSG_WRITE:
			resp_len = sizeof(ecpri_rma_msg_t);
			data_ptr = buffer + sizeof(ecpri_rma_msg_t);

			resp_msg = malloc(resp_len);
			if(resp_msg)
			{
				retval = IP_API_Write(addr, data_ptr, length);

				/* Add RMA header... */
				header = (ecpri_rma_msg_t *)resp_msg;
				memcpy(resp_msg, buffer, sizeof(ecpri_rma_msg_t));
				header->rd_wr_req_resp = ECPRI_RMA_MSG_WRITE | ECPRI_RMA_MSG_RESP;

				/* Send RMA response */
				proto_ecpri_send(resp_msg, resp_len, ECPRI_MSG_RMA, fd, src);
				free(resp_msg);
			}
			else
			{
				retval = -1;
			}
			break;

		case ECPRI_RMA_MSG_WR_NO_RESP:
			break;
		default: /* Includes response and fail if we get them */
			syslog(LOG_ERR, "proto_ecpri_handle_incoming_rma: other message received\n");
			break;
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Handle an incoming eCPRI Remote Memory Access message.
*
* @param [in]	buffer		Buffer containing incoming message.
* @param [in]	data_len	Length of incoming message.
* @param [in]	fd			Ignored.
* @param [in]	src			IP address of remote node for replies and result.
* @param [in]	ts			Time-stamp of received message.
*
* @return
*		- -1 on short message received.
*		- 0 otherwise.
*
******************************************************************************/
int proto_ecpri_handle_incoming_owdm(uint8_t *buffer, uint16_t data_len, int fd, struct sockaddr_in *src, struct timespec *ts)
{
	ecpri_owdm_msg_t *message;
	owdm_ts_msg_type owdm_ts;
	int retval = 0;
	int i;
	(void)fd;

	if(data_len >= sizeof(ecpri_owdm_msg_t))
	{
		message = (ecpri_owdm_msg_t *)buffer;
		switch(message->action_type)
		{
			case ECPRI_OWDM_MSG_ACTION_REQ_FOL_UP:
				/* Store Rx timestamp for later */
				memcpy(&owdm_msg.ts.node, src, sizeof(owdm_result.node));

				for(i=0; i<6; i++)
				{
					owdm_msg.ts.ts_sec[i] = (uint8_t)(ts[2].tv_sec>>(8*i))&0xff;
				}
				for(i=0; i<4; i++)
				{
					owdm_msg.ts.ts_nsec[i] = (uint8_t)(ts[2].tv_nsec>>(8*i))&0xff;
				}
				memcpy(owdm_msg.comp, owdm_comp, 8);
				break;

			case ECPRI_OWDM_MSG_ACTION_RESP:
				/* Store result */
				memcpy(&owdm_result.node, src, sizeof(owdm_result.node));
				memcpy(owdm_ts.ts.ts_sec, message->ts_sec, 6);
				memcpy(owdm_ts.ts.ts_nsec, message->ts_nsec, 4);
				memcpy(owdm_ts.comp, message->comp, 8);

				ecpri_owdm_calc_delay(&owdm_ts, &owdm_msg, &owdm_result);
				owdm_result.direction = TO_REMOTE;
				owdm_result.resp_num = owdm_req;
				break;

			case ECPRI_OWDM_MSG_ACTION_REM_REQ_FOL_UP:
			{
				ecpri_owdm_msg_t new_msg;
				memset(&new_msg, 0, sizeof(new_msg));
				new_msg.id = message->id;
				new_msg.action_type = ECPRI_OWDM_MSG_ACTION_REQ_FOL_UP;

				/* Send request with follow-up back */
				retval = proto_ecpri_owdm_send_req_get_ts((uint8_t *)&new_msg, (uint16_t)sizeof(new_msg), src, 1);
				break;
			}
			case ECPRI_OWDM_MSG_ACTION_FOL_UP:
			{
				ecpri_owdm_msg_t new_msg;
				owdm_ts_msg_type t1;
				memset(&new_msg, 0, sizeof(new_msg));

				new_msg.id = message->id;
				new_msg.action_type = ECPRI_OWDM_MSG_ACTION_RESP;

				/* Copy saved TS from original message into response */
				memcpy(new_msg.ts_sec, owdm_msg.ts.ts_sec, 6);
				memcpy(new_msg.ts_nsec, owdm_msg.ts.ts_nsec, 4);
				memcpy(new_msg.comp, owdm_comp, 8);

				/* Calc delay for ourselves from the TS in the follow-up */
				memcpy(t1.ts.ts_sec, message->ts_sec, 6);
				memcpy(t1.ts.ts_nsec, message->ts_nsec, 4);
				memcpy(t1.comp, message->comp, 8);

				memcpy(&owdm_result.node, src, sizeof(owdm_result.node));
				ecpri_owdm_calc_delay(&owdm_msg, &t1, &owdm_result);
				owdm_result.direction = FROM_REMOTE;
				owdm_result.resp_num = owdm_req;

				/* send response */
				retval = proto_ecpri_owdm_send_req_get_ts((uint8_t *)&new_msg, (uint16_t)sizeof(new_msg), src, 0);
				break;
			}
			default:
				break;
		}

		retval = 0;
	}
	else
	{
		retval = -1;
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Handle an incoming eCPRI Remote Reset message.
* Simply sends a response message to acknowledge the RMR. Does not do a reset.
*
* @param [in]	buffer		Buffer containing incoming message.
* @param [in]	data_len	Ignored.
* @param [in]	fd			File handle of socket for response.
* @param [in]	src			IP address of remote node for replies.
*
* @return
*		- -1 on malloc failure.
*		- 0 otherwise.
*
******************************************************************************/
int proto_ecpri_handle_incoming_rmr(uint8_t *buffer, uint16_t data_len, int fd, struct sockaddr_in *src)
{
	ecpri_rmr_msg_t *header;
	uint8_t type;
	uint8_t id;
	uint8_t *resp_msg;
	int resp_len = 0;
	int retval = 0;

	(void)data_len;

	header = (ecpri_rmr_msg_t *)buffer;
	type = header->code_op;
	id = header->id;

	switch(type)
	{
		case ECPRI_RMR_MSG_CODE_OP_REM_RESET_REQ:
			/* Prepare response */
			resp_len = sizeof(ecpri_rmr_msg_t);
			resp_msg = malloc(resp_len);
			if(resp_msg)
			{
				/* Add RMR header... */
				header = (ecpri_rmr_msg_t *)resp_msg;
				header->id = id;
				header->code_op = ECPRI_RMR_MSG_CODE_OP_REM_RESET_RESP;

				/* Send RMR response */
				proto_ecpri_send(resp_msg, resp_len, ECPRI_MSG_REM_RESET, fd, src);
				free(resp_msg);
			}
			else
			{
				retval = -1;
			}
			/* Do the reset */
		break;
		
		default:
		/* What? */
		break;
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Handle an incoming eCPRI event message.
* Dummy function that does nothing.
*
* @param [in]	buffer		Ignored.
* @param [in]	data_len	Ignored.
* @param [in]	fd			Ignored.
* @param [in]	src			Ignored.
*
* @return
*		- 0.
*
******************************************************************************/
int proto_ecpri_handle_incoming_event(uint8_t *buffer, uint16_t data_len, int fd, struct sockaddr_in *src)
{
	(void)buffer;
	(void)data_len;
	(void)fd;
	(void)src;

	return 0;
}

/*****************************************************************************/
/**
*
* Return the current OWDM result values.
* 
*
* @param [out]	req_no		Latest OWDM request number.
* @param [out]	resp_no		Latest OWDM response number.
* @param [out]	direction	Direction of latest response.
* @param [out]	node		Remote node of latest measurement.
* @param [out]	secs		Seconds portion of latest delay value.
* @param [out]	nsecs		Nano-seconds portion of latest delay value.
*
******************************************************************************/
void proto_ecpri_get_owdm_result(int *req_no, int *resp_no, ecpri_owdm_direction_type *direction,
								struct in_addr *node, unsigned long long *secs, unsigned long *nsecs)
{
	int i;

	*req_no = owdm_req;
	*resp_no = owdm_result.resp_num;
	*direction = owdm_result.direction;
	*node = owdm_result.node.sin_addr;

	for(i=0; (i<sizeof(long long)) && (i<6); i++)
	{
		*secs |= (uint8_t)(owdm_result.ts_sec[i]&0xff)<<(8*i);
	}
	for(i=0; (i<sizeof(long)) && (i<4); i++)
	{
		*nsecs |= (uint8_t)(owdm_result.ts_nsec[i]&0xff)<<(8*i);
	}
}

/*****************************************************************************/
/**
*
* Send an eCPRI test message (generic data).
* Sends a generic data message containing an incrementing sequence number.
* 
*
* @param [in]	dest	IP address of the remote host to send to.
*
* @return
*		- -1 on malloc failure.
*		- return value of proto_ecpri_send() otherwise.
*
******************************************************************************/
int proto_ecpri_test_mesg_send(struct sockaddr_in *dest)
{
	uint8_t *buffer;
	int buflen = 0;
	int retval = 0;
	int PC_ID = 1;
	int REQ_ID = 2;

	buflen = sizeof(PC_ID) + sizeof(REQ_ID) + sizeof(SEQ_NUM);
	buffer = malloc(buflen);

	if(buffer)
	{
		memcpy(buffer, &PC_ID, sizeof(PC_ID));
		memcpy(buffer + sizeof(PC_ID), &REQ_ID, sizeof(REQ_ID));
		memcpy(buffer + sizeof(PC_ID) + sizeof(REQ_ID), &SEQ_NUM, sizeof(SEQ_NUM));
		SEQ_NUM++;

		retval = proto_ecpri_send(buffer, buflen, ECPRI_MSG_GENERIC_DATA, sock_ip, dest);

		free(buffer);
	}
	else
	{
		retval = -1;
	}
	return retval;
}

/*****************************************************************************/
/**
*
* Handle an incoming eCPRI test message (generic data).
* 
*
* @param [in]	buffer		Buffer containing incoming message.
* @param [in]	data_len	Ignored.
* @param [in]	fd			Ignored.
* @param [in]	src			Ignored.
*
* @return
*		- 0.
*
******************************************************************************/
int proto_ecpri_handle_incoming_test_mesg(uint8_t *buffer, uint16_t data_len, int fd, struct sockaddr_in *src)
{
	//(void)buffer;
	(void)data_len;
	(void)fd;
	(void)src;

	syslog(LOG_DEBUG, "proto_ecpri_handle_incoming_test_mesg: received SEQ_ID:%d\n", buffer[sizeof(int)*2]);

	return 0;
}

/*****************************************************************************/
/**
*
* Set the reporting threshold for eCPRI OWDM measurements.
* 
*
* @param [in]	limit		The limit value above which to issue a report.
*
******************************************************************************/
void proto_ecpri_set_owdm_limit(long limit)
{
	ecpri_report_limit = limit;
}
/** @} */