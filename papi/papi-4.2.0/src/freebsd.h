/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/
/* 
* File:    freebsd-libpmc.c
* CVS:     $Id: freebsd.h,v 1.7 2011/02/23 22:59:07 vweaver1 Exp $
* Author:  Kevin London
*          london@cs.utk.edu
* Mods:    Harald Servat
*          redcrash@gmail.com
*/

#ifndef _PAPI_FreeBSD_H
#define _PAPI_FreeBSD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "papi.h"
#include "papi_defines.h"
#include <pmc.h>

#include "freebsd-config.h"

#define MY_VECTOR			_papi_freebsd_vector
#define MAX_COUNTERS		HWPMC_NUM_COUNTERS
#define MAX_COUNTER_TERMS	MAX_COUNTERS

#undef hwd_siginfo_t
#undef hwd_register_t
#undef hwd_reg_alloc_t
#undef hwd_control_state_t
#undef hwd_context_t
#undef hwd_ucontext_t
#undef hwd_libpmc_context_t

typedef struct hwd_siginfo {
	int placeholder;
} hwd_siginfo_t;

typedef struct hwd_register {
	int placeholder;
} hwd_register_t;

typedef struct hwd_reg_alloc {
	int placeholder;
} hwd_reg_alloc_t;

typedef struct hwd_control_state {
	int n_counters;      /* Number of counters */
	int hwc_domain;      /* HWC domain {user|kernel} */
	unsigned *caps;      /* Capabilities for each counter */
	pmc_id_t *pmcs;      /* PMC identifiers */
	long long *values;   /* Stored values for each counter */
	char **counters;     /* Name of each counter (with mode) */
} hwd_control_state_t;

typedef struct hwd_context {
	int placeholder; 
} hwd_context_t;

typedef struct hwd_ucontext {
	int placeholder; 
} hwd_ucontext_t;

typedef struct hwd_libpmc_context {
	int CPUsubstrate;
	int use_rdtsc;
} hwd_libpmc_context_t;

#define _papi_hwd_lock_init() { ; }
#define _papi_hwd_lock(a) { ; }
#define _papi_hwd_unlock(a) { ; }
#define GET_OVERFLOW_ADDRESS(ctx) (0x80000000)

#endif /* _PAPI_FreeBSD_LIBPMC_H */
