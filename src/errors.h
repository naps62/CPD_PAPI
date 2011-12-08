#pragma once

#include <stdio.h>
#include <stdlib.h>

#define ERR_PAPI_INIT			0
#define ERR_PAPI_INIT2			1
#define ERR_PAPI_EVENT_SET		2
#define ERR_PAPI_ADD_EVENT		3
#define ERR_PAPI_REMOVE_EVENT	4
#define ERR_PAPI_GET_EVENT_INFO	5
#define ERR_PAPI_START			6
#define ERR_PAPI_STOP			7

#define papi_safe(func, err)	if ((func) != PAPI_OK) handle_error(err)
#define safe(func, err) 		if (func) handle_error(err)

void handle_error(int err) {
	fprintf(stderr, "\nerror: %d\n", err);
	exit(1);
}
