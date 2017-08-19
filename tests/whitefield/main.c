/*
 * Copyright (C) 2017 Rahul Jadhav
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Whitefield Launcher Application
 *
 * @author      Rahul Jadhav <nyrahulATgmailDOTcom>
 */

#include <stdio.h>
#include "net/ipv6/addr.h"
#if 0
#include "shell.h"
#include "shell_commands.h"
#endif
#include "msg.h"
//#include "whitefield.h"

#define	MAIN_QUEUE_SIZE	(8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#if 0
int udp_cmd(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	printf("TODO\n");
	return 0;
}

static const shell_command_t shell_commands[] = {
	{ "udp", "send data over UDP and list on UDP ports", udp_cmd },
	{ NULL, NULL, NULL }
};
#endif

int main(void)
{
	msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
	printf("WHITEFIELD-RIOT network stack application\n");

#if 0
	char line_buf[SHELL_DEFAULT_BUFSIZE];
	shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
#endif

    return 0;
}
