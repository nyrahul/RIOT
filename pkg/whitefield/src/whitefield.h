#ifndef	_WHITEFIELD_H_
#define	_WHITEFIELD_H_

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

/*
	Configuration Options
*/
#define	WF_MAX_PKT_SZ	127

typedef struct _node_info_ {
	uint16_t id;	//Whitefield Node Index
}wf_node_info_t;

#ifndef	ENABLE_DEBUG
#define ENABLE_DEBUG    (1)
#endif
#include "debug.h"

#define	ERR(...)	fprintf(stderr,__VA_ARGS__)
#define	LOG(...)	DEBUG(__VA_ARGS__)

uint16_t wf_get_nodeid(void);
int wf_get_longaddr(uint8_t *value, unsigned int val_len);

#endif	//	_WHITEFIELD_H_
