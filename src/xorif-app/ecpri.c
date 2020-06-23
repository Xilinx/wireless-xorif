// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file ecpri.c
* @addtogroup protocol_ecpri
* @{
*
*  A sample command parser for the RoE Framer software modules.
*
******************************************************************************/

/***************************** Include Files *********************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <syslog.h>

//#include <xroe_types.h>
#include <ecpri_str.h>
#include <ecpri_proto.h>
//#include <comms.h>

/**
 * ECPRI_MAX_COMMANDS Number of commands handled by the ecpri module.
 */
#define ECPRI_MAX_COMMANDS 9

/**
 * RMA_READ Flag to indicate an RMA read operation.
 */
#define RMA_READ 1

/**
 * RMA_WRITE Flag to indicate an RMA write operation.
 */
#define RMA_WRITE 2

/************************** Function Prototypes ******************************/
int ecpri_help_func(int argc, char **argv, char *resp);
int ecpri_owdm_req_func(int argc, char **argv, char *resp);
int ecpri_owdm_res_func(int argc, char **argv, char *resp);
int ecpri_owdm_limit_func(int argc, char **argv, char *resp);
int ecpri_rma_read_func(int argc, char **argv, char *resp);
int ecpri_rma_write_func(int argc, char **argv, char *resp);
int ecpri_test_mesg_func(int argc, char **argv, char *resp);
int ecpri_rmr_req_func(int argc, char **argv, char *resp);

/**
 * ecpri_cmds The commands handled by the disable module.
 */
commands_t ecpri_cmds[ECPRI_MAX_COMMANDS] = {
	/* Keep this first */
	{"help", ECPRI_HELP_STR, ecpri_help_func},  /**< "help" command */
	/* Insert commands here */
	{"rma_read", ECPRI_RMA_READ_STR, ecpri_rma_read_func},  /**< "rma_read" command */
	{"rma_write", ECPRI_RMA_WRITE_STR, ecpri_rma_write_func},  /**< "rma_write" command */
	{"owdm_req", ECPRI_OWDM_REQ_STR, ecpri_owdm_req_func},  /**< "owdm_req" command */
	{"owdm_res", ECPRI_OWDM_RES_STR, ecpri_owdm_res_func},  /**< "owdm_res" command */
	{"owdm_limit", ECPRI_OWDM_LIMIT_STR, ecpri_owdm_limit_func},  /**< "owdm_limit" command */
	{"test_mesg", ECPRI_TEST_MESG_STR, ecpri_test_mesg_func},  /**< "test_msg" command */
	{"rmr_req", ECPRI_RMR_REQ_STR, ecpri_rmr_req_func},  /**< "rmr_req" command */
	/* Keep this last - insert commands above */
	{NULL, NULL, NULL} /**< NULL command to terminate array */ 
};

/*****************************************************************************/
/**
*
* Sends a One-Way Delay Measurement request to a remote node.
* This function calls the eCPRI protocol module to format and send the message.
* 
*
* @param [in]	type   		The direction, either TO_REMOTE or FROM_REMOTE.
* @param [in]	dest_addr	IP address of remote node.
*
* @return
*		- 0 if address is not valid.
*		- Return value of proto_ecpri_owdm_send_request().
*
******************************************************************************/
int owdm_send_request(uint8_t type, char *dest_addr)
{
	struct sockaddr_in dest;
	int retval = 0;
	
	dest.sin_family = AF_INET;
	dest.sin_port = port_ip;
	if(inet_aton(dest_addr, (struct in_addr *)&dest.sin_addr) != 0)
	{
		retval = proto_ecpri_owdm_send_request(type, &dest);
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Sends a Remote Memory Access request to a remote node.
* This function calls the eCPRI protocol module to format and send the message.
* 
*
* @param [in]	type   		Can be read or write.
* @param [in]	dest_addr	IP address of remote node.
* @param [in]	addr   		The remote memory address to access.
* @param [in]	length   	The number of bytes to access.
* @param [in]	values		Array of byte values to write, or NULL on read.
*
* @return
*		- -1 if malloc failed.
*		- 0 if address is not valid.
*		- Return value of proto_ecpri_rma_send_request().
*
******************************************************************************/
int rma_send_request(int type, char *dest_addr, char *addr, char *length, char *values)
{
	struct sockaddr_in dest;
	uint64_t mem_addr;
	uint16_t data_length;
	char *val, *next_val;
	int i;
	uint8_t *ptr;
	int retval = 0;

	mem_addr = strtoll(addr, NULL, 0);
	data_length = (uint16_t)strtol(length, NULL, 0);

	if(type==RMA_READ)
	{
		type = ECPRI_RMA_MSG_READ;
		ptr = NULL;
	}
	else
	{
		type = ECPRI_RMA_MSG_WRITE;

		ptr = malloc(data_length);
		if(ptr)
		{
			val = values;
			for(i = 0; i < data_length; i++)
			{
				ptr[i] = (uint8_t)strtol(val, &next_val, 0);
				val = next_val;
			}		
		}
		else
		{
			retval = -1;
		}
	}
	
	if(retval == 0)
	{
		dest.sin_family = AF_INET;
		dest.sin_port = port_ip;
		if(inet_aton(dest_addr, (struct in_addr *)&dest.sin_addr) != 0)
		{
			retval = proto_ecpri_rma_send_request(type, data_length, &dest, mem_addr, ptr);
		}
	}

	if(ptr)
	{
		free(ptr);
	}
	
	return retval;
}



/*****************************************************************************/
/**
*
* Sends a test message (generic data) to a remote node.
* This function calls the eCPRI protocol module to format and send the message.
* 
*
* @param [in]	dest_addr	IP address of remote node.
* @param [in]	dest_port	IP port address of the remote node.
*
* @return
*		- 0 if address is not valid.
*		- Return value of proto_ecpri_test_mesg_send().
*
******************************************************************************/
int test_mesg_send_request(char *dest_addr, char *dest_port)
{
	struct sockaddr_in dest;
	int retval = 0;
	int port = 0;

	port = strtoll(dest_port, NULL, 0);
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	if(inet_aton(dest_addr, (struct in_addr *)&dest.sin_addr) != 0)
	{
		retval = proto_ecpri_test_mesg_send(&dest);
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Reads a Remote Memory Access response from a remote node.
* This function calls the eCPRI protocol module to receive the message.
* 
*
* @param [in]	type   		Can be read or write.
* @param [in]	addr   		Ignored.
* @param [in]	length   	The number of bytes read/written.
* @param [in]	values		Array of byte values read, or NULL on write.
*
* @return
*		- Return value of proto_ecpri_rma_get_response().
*
******************************************************************************/
int rma_get_response(int type, char *addr, int *length, char **values)
{
	struct sockaddr_in src;
	char *val;
	int i;
	uint8_t *ptr=NULL;
	int retval = 0;

	if(type==RMA_READ)
	{
		type = ECPRI_RMA_MSG_READ;
	}
	else
	{
		type = ECPRI_RMA_MSG_WRITE;
	}

	retval = proto_ecpri_rma_get_response(type, length, &src, &ptr);
	if(retval==0)
	{		
		if(type==ECPRI_RMA_MSG_READ)
		{
			val = *values;
			for(i = 0; i < *length; i++)
			{
				val+=sprintf(val, " 0x%02x", ptr[i]);
			}
			sprintf(val, "\n");
		}
		else
		{
			sprintf(*values, "got OK response, length %d\n", *length);
		}
		
		if(ptr)
		{
			free(ptr);
		}
	}
	
	return retval;
}

/*****************************************************************************/
/**
*
* Sends a Remote Reset request to a remote node.
* This function calls the eCPRI protocol module to format and send the message.
* 
*
* @param [in]	dest_addr	Address of the remote node to reset.
*
* @return
*		- 0 if address is not valid.
*		- Return value of proto_ecpri_rmr_send_request().
*
******************************************************************************/
int rmr_send_request(char *dest_addr)
{
	struct sockaddr_in dest;
	int retval = 0;
	
	dest.sin_family = AF_INET;
	dest.sin_port = port_ip;
	if(inet_aton(dest_addr, (struct in_addr *)&dest.sin_addr) != 0)
	{
		retval = proto_ecpri_rmr_send_request(&dest);
	}

	return retval;
}

/*****************************************************************************/
/**
*
* Gets a Remote Reset response from a remote node.
* This function calls the eCPRI protocol module to receive the message.
* 
*
* @param [in]	addr	Address of the remote node to reset.
* @param [out]	values	Pointer to string to place the response into.
*
* @return
*		- Return value of proto_ecpri_rmr_get_response().
*
******************************************************************************/
int rmr_get_response(char *addr, char **values)
{
	struct sockaddr_in src;
	int retval = 0;

	retval = proto_ecpri_rmr_get_response(&src);
	if(retval==0)
	{		
		sprintf(*values, "got OK response\n");		
	}
	
	return retval;
}

/*****************************************************************************/
/**
*
* Returns help strings for eCPRI commands.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0
*
******************************************************************************/
int ecpri_help_func(int argc, char **argv, char *resp)
{
	int i;
	char *str = resp;
	
	str += sprintf(str, "ecpri help:\n");
	
	for(i=0; ecpri_cmds[i].cmd != NULL; i++)
	{
		str += sprintf(str, "\t%s\t : %s", ecpri_cmds[i].cmd, ecpri_cmds[i].helptxt);
	}
	return 0;
}

/*****************************************************************************/
/**
*
* Requests a Remote Memory read from a remote node.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0
*
******************************************************************************/
int ecpri_rma_read_func(int argc, char **argv, char *resp)
{
	char *str = resp;
	int read_length;
	
	if(argc != 3)
	{
		sprintf(str, "%s", ECPRI_RMA_READ_STR);
	}
	else
	{
		str += sprintf(str, "Sending RMA read request of %s bytes at %s to %s:\n", argv[2], argv[1], argv[0]);
		rma_send_request(RMA_READ, argv[0], argv[1], argv[2], NULL);
		str += sprintf(str, "RMA read response: ");
		rma_get_response(RMA_READ, argv[0], &read_length, &str);
	}	
	return 0;
}

/*****************************************************************************/
/**
*
* Requests a Remote Memory write to a remote node.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0
*
******************************************************************************/
int ecpri_rma_write_func(int argc, char **argv, char *resp)
{
	char *str = resp;
	int read_length;
	char numbers[1024];
	int i;
	
	if((argc < 4) || (atoi(argv[2]) != argc-3))
	{
		sprintf(str, "%s", ECPRI_RMA_WRITE_STR);
	}
	else
	{
		numbers[0] = 0;
		for(i = 0; i < argc-3; i++)
		{
			strcat(numbers, argv[i+3]);
			strcat(numbers, " ");
		}
		str += sprintf(str, "Sending RMA write request of %s to %s:\n", argv[1], argv[0]);
		
		rma_send_request(RMA_WRITE, argv[0], argv[1], argv[2], numbers);

		rma_get_response(RMA_WRITE, argv[0], &read_length, &str);
	}	
	return 0;
}


/*****************************************************************************/
/**
*
* Requests a One-Way Delay Measurement to/from a remote node.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0
*
******************************************************************************/
int ecpri_owdm_req_func(int argc, char **argv, char *resp)
{
	char *str = resp;
	uint8_t owdm_type;
	
	if(argc != 2)
	{
		sprintf(str, "%s\n", ECPRI_OWDM_REQ_STR);
	}
	else
	{
		str += sprintf(str, "Starting OWDM measurement to %s (type %s):\n", argv[0], argv[1]);

		if(strncmp(argv[1], "to_remote", 9)==0)
		{
			owdm_type = ECPRI_OWDM_MSG_ACTION_REQ_FOL_UP;
			owdm_send_request(owdm_type, argv[0]);
		}
		else if(strncmp(argv[1], "from_remote", 11)==0)
		{
			owdm_type = ECPRI_OWDM_MSG_ACTION_REM_REQ_FOL_UP;
			owdm_send_request(owdm_type, argv[0]);
		}
		else
		{
			sprintf(str, "%s\n", ECPRI_OWDM_REQ_STR);
		}
	}

	return 0;
}

/*****************************************************************************/
/**
*
* Gets the result of the latest One-Way Delay Measurement.
* This function calls the eCPRI protocol module to retrieve the result.
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0
*
******************************************************************************/
int ecpri_owdm_res_func(int argc, char **argv, char *resp)
{
	char *str = resp;
	
	if(argc != 0)
	{
		sprintf(str, "%s\n", ECPRI_OWDM_RES_STR);
	}
	else
	{
		int req_no = 0;
		int resp_no = 0;
		ecpri_owdm_direction_type direction = TO_REMOTE;
		struct in_addr node;
		unsigned long long secs = 0;
		unsigned long nsecs = 0;
		
		proto_ecpri_get_owdm_result(&req_no, &resp_no, &direction, &node, &secs, &nsecs);
		
		if(req_no != resp_no)
		{
			sprintf(str, "OWDM result: pending\n");
		}
		else
		{
			sprintf(str, "OWDM result #%d: direction %s (%s), %llu.%09lu\n", 
						 resp_no,
						 direction==TO_REMOTE?"TO_REMOTE":"FROM_REMOTE",
						 inet_ntoa(node),
						 secs,
						 nsecs);
		}
	}
	return 0;
}

/*****************************************************************************/
/**
*
* Sets the reporting limit of the latest One-Way Delay Measurement.
* This function calls the eCPRI protocol module to set the limit.
* If the specified threshold value is 0, reporting is disabled, otherwise all
* OWDM measurements over the limit (in nsecs) trigger an ILA write.
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0
*
******************************************************************************/
int ecpri_owdm_limit_func(int argc, char **argv, char *resp)
{
	char *str = resp;
	
	if(argc != 1)
	{
		sprintf(str, "%s\n", ECPRI_OWDM_LIMIT_STR);
	}
	else
	{
		long limit = strtol(argv[0], NULL, 0);

		proto_ecpri_set_owdm_limit(limit);
		sprintf(str, "OWDM report limit set to %ld\n", limit);
	}
	return 0;
}

/*****************************************************************************/
/**
*
* Requests a Remote Reset of a remote node.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0
*
******************************************************************************/
int ecpri_rmr_req_func(int argc, char **argv, char *resp)
{
	char *str = resp;
	
	if(argc != 1)
	{
		sprintf(str, "%s", ECPRI_RMR_REQ_STR);
	}
	else
	{
		str += sprintf(str, "Sending RMR request to %s:\n", argv[0]);
		rmr_send_request(argv[0]);
		str += sprintf(str, "RMR response: ");
		rmr_get_response(argv[0], &str);
	}	
	return 0;
}

/*****************************************************************************/
/**
*
* Parses the input command string and calls a handler function (if found).
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 1 if no commands tokens found.
*		- 2 if no handler found for command.
*		- 0 otherwise.
*
******************************************************************************/
int ecpri_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	char *str = resp;
	
	if(argc == 0)
	{
		sprintf(str, "\t%s", ECPRI_USAGE_STR);
		return(1);
	}

	for(count=0; ecpri_cmds[count].cmd != NULL; count++)
	{
		if(strcmp(argv[0], ecpri_cmds[count].cmd)==0)
		{
			found = 1;
			/* Call the handler function for the command given */
			ecpri_cmds[count].func(argc-1, &argv[1], str);
		}
	}
	
	if(!found)
	{
		sprintf(str, "Command %s not found, try \"help\"\n", argv[0]);
		return(2);
	}

	return 0;
}

/*****************************************************************************/
/**
*
* Sends a test message (generic data) to a remote node.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0
*
******************************************************************************/
int ecpri_test_mesg_func(int argc, char **argv, char *resp)
{
	char *str = resp;

	if(argc != 2)
	{
		sprintf(str, "%s", ECPRI_TEST_MESG_STR);
		return(1);
	}

	str += sprintf(str, "Sending test message to %s:%s\n", argv[0], argv[1]);
	test_mesg_send_request(argv[0], argv[1]);
	return 0;
}
/** @} */