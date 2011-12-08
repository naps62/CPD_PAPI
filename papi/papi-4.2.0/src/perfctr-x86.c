/* 
* File:    perfctr-x86.c
* CVS:     $Id: perfctr-x86.c,v 1.27 2011/09/21 20:23:54 vweaver1 Exp $
* Author:  Brian Sheely
*          bsheely@eecs.utk.edu
* Mods:    <your name here>
*          <your email address>
*/

#include "papi_memory.h"
#include "papi_internal.h"
#include "perfctr-x86.h"
#include "perfmon/pfmlib.h"
#include "papi_libpfm_events.h"

extern native_event_entry_t *native_table;
extern hwi_search_t *preset_search_map;
extern caddr_t _start, _init, _etext, _fini, _end, _edata, __bss_start;

#include "linux-memory.h"

/* Prototypes for entry points found in perfctr.c */
extern int _perfctr_init_substrate( int );
extern int _perfctr_ctl( hwd_context_t * ctx, int code,
					   _papi_int_option_t * option );
extern void _perfctr_dispatch_timer( int signal, hwd_siginfo_t * si,
								   void *context );

extern int _perfctr_init( hwd_context_t * ctx );
extern int _perfctr_shutdown( hwd_context_t * ctx );

#include "linux-common.h"
#include "linux-timer.h"

extern papi_mdi_t _papi_hwi_system_info;

extern papi_vector_t MY_VECTOR;

#if defined(PERFCTR26)
#define evntsel_aux p4.escr
#endif

#if defined(PAPI_PENTIUM4_VEC_MMX)
#define P4_VEC "MMX"
#else
#define P4_VEC "SSE"
#endif

#if defined(PAPI_PENTIUM4_FP_X87)
#define P4_FPU " X87"
#elif defined(PAPI_PENTIUM4_FP_X87_SSE_SP)
#define P4_FPU " X87 SSE_SP"
#elif defined(PAPI_PENTIUM4_FP_SSE_SP_DP)
#define P4_FPU " SSE_SP SSE_DP"
#else
#define P4_FPU " X87 SSE_DP"
#endif

/* CODE TO SUPPORT CUSTOMIZABLE FP COUNTS ON OPTERON */
#if defined(PAPI_OPTERON_FP_RETIRED)
#define AMD_FPU "RETIRED"
#elif defined(PAPI_OPTERON_FP_SSE_SP)
#define AMD_FPU "SSE_SP"
#elif defined(PAPI_OPTERON_FP_SSE_DP)
#define AMD_FPU "SSE_DP"
#else
#define AMD_FPU "SPECULATIVE"
#endif

static inline int is_pentium4(void) {
  if ( ( _papi_hwi_system_info.hw_info.vendor == PAPI_VENDOR_INTEL ) &&
       ( _papi_hwi_system_info.hw_info.cpuid_family == 15 )) {
    return 1;
  }

  return 0;

}

static int
_papi_hwd_fixup_vec( void )
{
	char table_name[PAPI_MIN_STR_LEN] = "Intel Pentium4 VEC ";
	char *str = getenv( "PAPI_PENTIUM4_VEC" );

	/* if the env variable isn't set, use the default */
	if ( ( str == NULL ) || ( strlen( str ) == 0 ) ) {
		strcat( table_name, P4_VEC );
	} else {
		strcat( table_name, str );
	}
	if ( ( _papi_libpfm_setup_presets( table_name, 0 ) ) != PAPI_OK ) {
		PAPIERROR
			( "Improper usage of PAPI_PENTIUM4_VEC environment variable.\nUse either SSE or MMX" );
		return ( PAPI_ESBSTR );
	}
	return ( PAPI_OK );
}

static int
_papi_p4_hwd_fixup_fp( void )
{
	char table_name[PAPI_MIN_STR_LEN] = "Intel Pentium4 FPU";
	char *str = getenv( "PAPI_PENTIUM4_FP" );

	/* if the env variable isn't set, use the default */
	if ( ( str == NULL ) || ( strlen( str ) == 0 ) ) {
		strcat( table_name, P4_FPU );
	} else {
		if ( strstr( str, "X87" ) )
			strcat( table_name, " X87" );
		if ( strstr( str, "SSE_SP" ) )
			strcat( table_name, " SSE_SP" );
		if ( strstr( str, "SSE_DP" ) )
			strcat( table_name, " SSE_DP" );
	}
	if ( ( _papi_libpfm_setup_presets( table_name, 0 ) ) != PAPI_OK ) {
		PAPIERROR
			( "Improper usage of PAPI_PENTIUM4_FP environment variable.\nUse one or two of X87,SSE_SP,SSE_DP" );
		return ( PAPI_ESBSTR );
	}
	return ( PAPI_OK );
}

static int
_papi_hwd_fixup_fp( char *name )
{
	char table_name[PAPI_MIN_STR_LEN];
	char *str = getenv( "PAPI_OPTERON_FP" );

	/* if the env variable isn't set, return the defaults */
	strcpy( table_name, name );
	strcat( table_name, " FPU " );
	if ( ( str == NULL ) || ( strlen( str ) == 0 ) ) {
		strcat( table_name, AMD_FPU );
	} else {
		strcat( table_name, str );
	}

	if ( ( _papi_libpfm_setup_presets( table_name, 0 ) ) != PAPI_OK ) {
		PAPIERROR
			( "Improper usage of PAPI_OPTERON_FP environment variable.\nUse one of RETIRED, SPECULATIVE, SSE_SP, SSE_DP" );
		return ( PAPI_ESBSTR );
	}
	return ( PAPI_OK );
}

#ifdef DEBUG
static void
print_alloc( X86_reg_alloc_t * a )
{
	SUBDBG( "X86_reg_alloc:\n" );
	SUBDBG( "  selector: 0x%x\n", a->ra_selector );
	SUBDBG( "  rank: 0x%x\n", a->ra_rank );
	SUBDBG( "  escr: 0x%x 0x%x\n", a->ra_escr[0], a->ra_escr[1] );
}

void
print_control( const struct perfctr_cpu_control *control )
{
	unsigned int i;
	SUBDBG( "Control used:\n" );
	SUBDBG( "tsc_on\t\t\t%u\n", control->tsc_on );
	SUBDBG( "nractrs\t\t\t%u\n", control->nractrs );
	SUBDBG( "nrictrs\t\t\t%u\n", control->nrictrs );

	for ( i = 0; i < ( control->nractrs + control->nrictrs ); ++i ) {
		if ( control->pmc_map[i] >= 18 ) {
			SUBDBG( "pmc_map[%u]\t\t0x%08X\n", i, control->pmc_map[i] );
		} else {
			SUBDBG( "pmc_map[%u]\t\t%u\n", i, control->pmc_map[i] );
		}
		SUBDBG( "evntsel[%u]\t\t0x%08X\n", i, control->evntsel[i] );
		if ( control->ireset[i] )
			SUBDBG( "ireset[%u]\t%d\n", i, control->ireset[i] );
	}
}
#endif

/* Assign the global native and preset table pointers, find the native
   table's size in memory and then call the preset setup routine. */
int
setup_x86_presets( int cputype )
{
	int retval = PAPI_OK;

        if ( ( retval = _papi_libpfm_init(&MY_VECTOR) ) != PAPI_OK ) {
	   return retval;
	}

	if ( is_pentium4() ) {
		/* load the baseline event map for all Pentium 4s */

		_papi_libpfm_setup_presets( "Intel Pentium4", 0 );	/* base events */

		/* fix up the floating point and vector ops */
		if ( ( retval = _papi_p4_hwd_fixup_fp(  ) ) != PAPI_OK )
			return ( retval );
		if ( ( retval = _papi_hwd_fixup_vec(  ) ) != PAPI_OK )
			return ( retval );

		/* install L3 cache events iff 3 levels of cache exist */
		if ( _papi_hwi_system_info.hw_info.mem_hierarchy.levels == 3 )
			_papi_libpfm_setup_presets( "Intel Pentium4 L3", 0 );

		/* overload with any model dependent events */
		if ( cputype == PERFCTR_X86_INTEL_P4 ) {
			/* do nothing besides the base map */
		} else if ( cputype == PERFCTR_X86_INTEL_P4M2 ) {
		}
#ifdef PERFCTR_X86_INTEL_P4M3
		else if ( cputype == PERFCTR_X86_INTEL_P4M3 ) {
		}
#endif
		else {
			PAPIERROR( MODEL_ERROR );
			return ( PAPI_ESBSTR );
		}
	} else {
		switch ( cputype ) {
		case PERFCTR_X86_GENERIC:
		case PERFCTR_X86_WINCHIP_C6:
		case PERFCTR_X86_WINCHIP_2:
		case PERFCTR_X86_VIA_C3:
		case PERFCTR_X86_INTEL_P5:
		case PERFCTR_X86_INTEL_P5MMX:
			SUBDBG( "This cpu is supported by the perfctr-x86 substrate\n" );
			PAPIERROR( MODEL_ERROR );
			return ( PAPI_ESBSTR );
		case PERFCTR_X86_INTEL_P6:
			_papi_libpfm_setup_presets( "Intel P6", 0 );	/* base events */
			break;
		case PERFCTR_X86_INTEL_PII:
			_papi_libpfm_setup_presets( "Intel P6", 0 );	/* base events */
			break;
		case PERFCTR_X86_INTEL_PIII:
			_papi_libpfm_setup_presets( "Intel P6", 0 );	/* base events */
			_papi_libpfm_setup_presets( "Intel PentiumIII", 0 );	/* events that differ from Pentium M */
			break;
#ifdef PERFCTR_X86_INTEL_PENTM
		case PERFCTR_X86_INTEL_PENTM:
			_papi_libpfm_setup_presets( "Intel P6", 0 );	/* base events */
			_papi_libpfm_setup_presets( "Intel PentiumM", 0 );	/* events that differ from PIII */
			break;
#endif
#ifdef PERFCTR_X86_INTEL_CORE
		case PERFCTR_X86_INTEL_CORE:
			_papi_libpfm_setup_presets( "Intel Core Duo/Solo", 0 );
			break;
#endif
#ifdef PERFCTR_X86_INTEL_CORE2
		case PERFCTR_X86_INTEL_CORE2:
			_papi_libpfm_setup_presets( "Intel Core2", 0 );
			break;
#endif
		case PERFCTR_X86_AMD_K7:
			_papi_libpfm_setup_presets( "AMD64 (K7)", 0 );
			break;
#ifdef PERFCTR_X86_AMD_K8	 /* this is defined in perfctr 2.5.x */
		case PERFCTR_X86_AMD_K8:
			_papi_libpfm_setup_presets( "AMD64", 0 );
			_papi_hwd_fixup_fp( "AMD64" );
			break;
#endif
#ifdef PERFCTR_X86_AMD_K8C	 /* this is defined in perfctr 2.6.x */
		case PERFCTR_X86_AMD_K8C:
			_papi_libpfm_setup_presets( "AMD64", 0 );
			_papi_hwd_fixup_fp( "AMD64" );
			break;
#endif
#ifdef PERFCTR_X86_AMD_FAM10 /* this is defined in perfctr 2.6.29 */
		case PERFCTR_X86_AMD_FAM10:
			_papi_libpfm_setup_presets( "AMD64 (Barcelona)", 0 );
			break;
#endif
#ifdef PERFCTR_X86_INTEL_ATOM	/* family 6 model 28 */
		case PERFCTR_X86_INTEL_ATOM:
			_papi_libpfm_setup_presets( "Intel Atom", 0 );
			break;
#endif
#ifdef PERFCTR_X86_INTEL_NHLM	/* family 6 model 26 */
		case PERFCTR_X86_INTEL_NHLM:
			_papi_libpfm_setup_presets( "Intel Nehalem", 0 );
			break;
#endif
#ifdef PERFCTR_X86_INTEL_WSTMR
		case PERFCTR_X86_INTEL_WSTMR:
			_papi_libpfm_setup_presets( "Intel Westmere", 0 );
			break;
#endif
		default:
			PAPIERROR( MODEL_ERROR );
			return PAPI_ESBSTR;
		}
		SUBDBG( "Number of native events: %d\n",
				MY_VECTOR.cmp_info.num_native_events );
	}
	return retval;
}

static int
_x86_init_control_state( hwd_control_state_t * ptr )
{
	int i, def_mode = 0;

	if ( is_pentium4() ) {
		if ( MY_VECTOR.cmp_info.default_domain & PAPI_DOM_USER )
			def_mode |= ESCR_T0_USR;
		if ( MY_VECTOR.cmp_info.default_domain & PAPI_DOM_KERNEL )
			def_mode |= ESCR_T0_OS;

		for ( i = 0; i < MY_VECTOR.cmp_info.num_cntrs; i++ ) {
			ptr->control.cpu_control.evntsel_aux[i] |= def_mode;
		}
		ptr->control.cpu_control.tsc_on = 1;
		ptr->control.cpu_control.nractrs = 0;
		ptr->control.cpu_control.nrictrs = 0;

#ifdef VPERFCTR_CONTROL_CLOEXEC
		ptr->control.flags = VPERFCTR_CONTROL_CLOEXEC;
		SUBDBG( "close on exec\t\t\t%u\n", ptr->control.flags );
#endif
	} else {

		if ( MY_VECTOR.cmp_info.default_domain & PAPI_DOM_USER )
			def_mode |= PERF_USR;
		if ( MY_VECTOR.cmp_info.default_domain & PAPI_DOM_KERNEL )
			def_mode |= PERF_OS;

		ptr->allocated_registers.selector = 0;
		switch ( _papi_hwi_system_info.hw_info.model ) {
		case PERFCTR_X86_GENERIC:
		case PERFCTR_X86_WINCHIP_C6:
		case PERFCTR_X86_WINCHIP_2:
		case PERFCTR_X86_VIA_C3:
		case PERFCTR_X86_INTEL_P5:
		case PERFCTR_X86_INTEL_P5MMX:
		case PERFCTR_X86_INTEL_PII:
		case PERFCTR_X86_INTEL_P6:
		case PERFCTR_X86_INTEL_PIII:
#ifdef PERFCTR_X86_INTEL_CORE
		case PERFCTR_X86_INTEL_CORE:
#endif
#ifdef PERFCTR_X86_INTEL_PENTM
		case PERFCTR_X86_INTEL_PENTM:
#endif
			ptr->control.cpu_control.evntsel[0] |= PERF_ENABLE;
			for ( i = 0; i < MY_VECTOR.cmp_info.num_cntrs; i++ ) {
				ptr->control.cpu_control.evntsel[i] |= def_mode;
				ptr->control.cpu_control.pmc_map[i] = ( unsigned int ) i;
			}
			break;
#ifdef PERFCTR_X86_INTEL_CORE2
		case PERFCTR_X86_INTEL_CORE2:
#endif
#ifdef PERFCTR_X86_INTEL_ATOM
		case PERFCTR_X86_INTEL_ATOM:
#endif
#ifdef PERFCTR_X86_INTEL_NHLM
		case PERFCTR_X86_INTEL_NHLM:
#endif
#ifdef PERFCTR_X86_INTEL_WSTMR
		case PERFCTR_X86_INTEL_WSTMR:
#endif
#ifdef PERFCTR_X86_AMD_K8
		case PERFCTR_X86_AMD_K8:
#endif
#ifdef PERFCTR_X86_AMD_K8C
		case PERFCTR_X86_AMD_K8C:
#endif
#ifdef PERFCTR_X86_AMD_FAM10H	/* this is defined in perfctr 2.6.29 */
		case PERFCTR_X86_AMD_FAM10H:
#endif
		case PERFCTR_X86_AMD_K7:
			for ( i = 0; i < MY_VECTOR.cmp_info.num_cntrs; i++ ) {
				ptr->control.cpu_control.evntsel[i] |= PERF_ENABLE | def_mode;
				ptr->control.cpu_control.pmc_map[i] = ( unsigned int ) i;
			}
			break;
		}
#ifdef VPERFCTR_CONTROL_CLOEXEC
		ptr->control.flags = VPERFCTR_CONTROL_CLOEXEC;
		SUBDBG( "close on exec\t\t\t%u\n", ptr->control.flags );
#endif

		/* Make sure the TSC is always on */
		ptr->control.cpu_control.tsc_on = 1;
	}
	return ( PAPI_OK );
}

int
_x86_set_domain( hwd_control_state_t * cntrl, int domain )
{
	int i, did = 0;
	int num_cntrs = MY_VECTOR.cmp_info.num_cntrs;

	/* Clear the current domain set for this event set */
	/* We don't touch the Enable bit in this code */
	if ( is_pentium4() ) {
		for ( i = 0; i < MY_VECTOR.cmp_info.num_cntrs; i++ ) {
			cntrl->control.cpu_control.evntsel_aux[i] &=
				~( ESCR_T0_OS | ESCR_T0_USR );
		}

		if ( domain & PAPI_DOM_USER ) {
			did = 1;
			for ( i = 0; i < MY_VECTOR.cmp_info.num_cntrs; i++ ) {
				cntrl->control.cpu_control.evntsel_aux[i] |= ESCR_T0_USR;
			}
		}

		if ( domain & PAPI_DOM_KERNEL ) {
			did = 1;
			for ( i = 0; i < MY_VECTOR.cmp_info.num_cntrs; i++ ) {
				cntrl->control.cpu_control.evntsel_aux[i] |= ESCR_T0_OS;
			}
		}
	} else {
		for ( i = 0; i < num_cntrs; i++ ) {
			cntrl->control.cpu_control.evntsel[i] &= ~( PERF_OS | PERF_USR );
		}

		if ( domain & PAPI_DOM_USER ) {
			did = 1;
			for ( i = 0; i < num_cntrs; i++ ) {
				cntrl->control.cpu_control.evntsel[i] |= PERF_USR;
			}
		}

		if ( domain & PAPI_DOM_KERNEL ) {
			did = 1;
			for ( i = 0; i < num_cntrs; i++ ) {
				cntrl->control.cpu_control.evntsel[i] |= PERF_OS;
			}
		}
	}

	if ( !did )
		return ( PAPI_EINVAL );
	else
		return ( PAPI_OK );
}

/* This function examines the event to determine
    if it can be mapped to counter ctr.
    Returns true if it can, false if it can't. */
static int
_x86_bpt_map_avail( hwd_reg_alloc_t * dst, int ctr )
{
	return ( int ) ( dst->ra_selector & ( 1 << ctr ) );
}

/* This function forces the event to
    be mapped to only counter ctr.
    Returns nothing.  */
static void
_x86_bpt_map_set( hwd_reg_alloc_t * dst, int ctr )
{
	dst->ra_selector = ( unsigned int ) ( 1 << ctr );
	dst->ra_rank = 1;

	if ( is_pentium4() ) {
		/* Pentium 4 requires that both an escr and a counter are selected.
		   Find which counter mask contains this counter.
		   Set the opposite escr to empty (-1) */
		if ( dst->ra_bits.counter[0] & dst->ra_selector )
			dst->ra_escr[1] = -1;
		else
			dst->ra_escr[0] = -1;
	}
}

/* This function examines the event to determine
   if it has a single exclusive mapping.
   Returns true if exlusive, false if non-exclusive.  */
static int
_x86_bpt_map_exclusive( hwd_reg_alloc_t * dst )
{
	return ( dst->ra_rank == 1 );
}

/* This function compares the dst and src events
    to determine if any resources are shared. Typically the src event
    is exclusive, so this detects a conflict if true.
    Returns true if conflict, false if no conflict.  */
static int
_x86_bpt_map_shared( hwd_reg_alloc_t * dst, hwd_reg_alloc_t * src )
{
  if ( is_pentium4() ) {
		int retval1, retval2;
		/* Pentium 4 needs to check for conflict of both counters and esc registers */
		/* selectors must share bits */
		retval1 = ( ( dst->ra_selector & src->ra_selector ) ||
					/* or escrs must equal each other and not be set to -1 */
					( ( dst->ra_escr[0] == src->ra_escr[0] ) &&
					  ( ( int ) dst->ra_escr[0] != -1 ) ) ||
					( ( dst->ra_escr[1] == src->ra_escr[1] ) &&
					  ( ( int ) dst->ra_escr[1] != -1 ) ) );
		/* Pentium 4 also needs to check for conflict on pebs registers */
		/* pebs enables must both be non-zero */
		retval2 =
			( ( ( dst->ra_bits.pebs_enable && src->ra_bits.pebs_enable ) &&
				/* and not equal to each other */
				( dst->ra_bits.pebs_enable != src->ra_bits.pebs_enable ) ) ||
			  /* same for pebs_matrix_vert */
			  ( ( dst->ra_bits.pebs_matrix_vert &&
				  src->ra_bits.pebs_matrix_vert ) &&
				( dst->ra_bits.pebs_matrix_vert !=
				  src->ra_bits.pebs_matrix_vert ) ) );
		if ( retval2 )
			SUBDBG( "pebs conflict!\n" );
		return ( retval1 | retval2 );
	}

	return ( int ) ( dst->ra_selector & src->ra_selector );
}

/* This function removes shared resources available to the src event
    from the resources available to the dst event,
    and reduces the rank of the dst event accordingly. Typically,
    the src event will be exclusive, but the code shouldn't assume it.
    Returns nothing.  */
static void
_x86_bpt_map_preempt( hwd_reg_alloc_t * dst, hwd_reg_alloc_t * src )
{
	int i;
	unsigned shared;

	if ( is_pentium4() ) {
#ifdef DEBUG
		SUBDBG( "src, dst\n" );
		print_alloc( src );
		print_alloc( dst );
#endif

		/* check for a pebs conflict */
		/* pebs enables must both be non-zero */
		i = ( ( ( dst->ra_bits.pebs_enable && src->ra_bits.pebs_enable ) &&
				/* and not equal to each other */
				( dst->ra_bits.pebs_enable != src->ra_bits.pebs_enable ) ) ||
			  /* same for pebs_matrix_vert */
			  ( ( dst->ra_bits.pebs_matrix_vert &&
				  src->ra_bits.pebs_matrix_vert )
				&& ( dst->ra_bits.pebs_matrix_vert !=
					 src->ra_bits.pebs_matrix_vert ) ) );
		if ( i ) {
			SUBDBG( "pebs conflict! clearing selector\n" );
			dst->ra_selector = 0;
			return;
		} else {
			/* remove counters referenced by any shared escrs */
			if ( ( dst->ra_escr[0] == src->ra_escr[0] ) &&
				 ( ( int ) dst->ra_escr[0] != -1 ) ) {
				dst->ra_selector &= ~dst->ra_bits.counter[0];
				dst->ra_escr[0] = -1;
			}
			if ( ( dst->ra_escr[1] == src->ra_escr[1] ) &&
				 ( ( int ) dst->ra_escr[1] != -1 ) ) {
				dst->ra_selector &= ~dst->ra_bits.counter[1];
				dst->ra_escr[1] = -1;
			}

			/* remove any remaining shared counters */
			shared = ( dst->ra_selector & src->ra_selector );
			if ( shared )
				dst->ra_selector ^= shared;
		}
		/* recompute rank */
		for ( i = 0, dst->ra_rank = 0; i < MAX_COUNTERS; i++ )
			if ( dst->ra_selector & ( 1 << i ) )
				dst->ra_rank++;
#ifdef DEBUG
		SUBDBG( "new dst\n" );
		print_alloc( dst );
#endif
	} else {
		shared = dst->ra_selector & src->ra_selector;
		if ( shared )
			dst->ra_selector ^= shared;
		for ( i = 0, dst->ra_rank = 0; i < MAX_COUNTERS; i++ )
			if ( dst->ra_selector & ( 1 << i ) )
				dst->ra_rank++;
	}
}

static void
_x86_bpt_map_update( hwd_reg_alloc_t * dst, hwd_reg_alloc_t * src )
{
	dst->ra_selector = src->ra_selector;

	if ( is_pentium4() ) {
		dst->ra_escr[0] = src->ra_escr[0];
		dst->ra_escr[1] = src->ra_escr[1];
	}
}

/* Register allocation */
static int
_x86_allocate_registers( EventSetInfo_t * ESI )
{
	int i, j, natNum;
	hwd_reg_alloc_t event_list[MAX_COUNTERS];
	hwd_register_t *ptr;

	/* Initialize the local structure needed
	   for counter allocation and optimization. */
	natNum = ESI->NativeCount;

	if ( is_pentium4() )
		SUBDBG( "native event count: %d\n", natNum );

	for ( i = 0; i < natNum; i++ ) {
		/* retrieve the mapping information about this native event */
		_papi_libpfm_ntv_code_to_bits( ( unsigned int ) ESI->NativeInfoArray[i].
							   ni_event, &event_list[i].ra_bits );

		if ( is_pentium4() ) {
			/* combine counter bit masks for both esc registers into selector */
			event_list[i].ra_selector =
				event_list[i].ra_bits.counter[0] | event_list[i].ra_bits.
				counter[1];
		} else {
			/* make sure register allocator only looks at legal registers */
			event_list[i].ra_selector =
				event_list[i].ra_bits.selector & ALLCNTRS;
#ifdef PERFCTR_X86_INTEL_CORE2
			if ( _papi_hwi_system_info.hw_info.model ==
				 PERFCTR_X86_INTEL_CORE2 )
				event_list[i].ra_selector |=
					( ( event_list[i].ra_bits.
						selector >> 16 ) << 2 ) & ALLCNTRS;
#endif
		}
		/* calculate native event rank, which is no. of counters it can live on */
		event_list[i].ra_rank = 0;
		for ( j = 0; j < MAX_COUNTERS; j++ ) {
			if ( event_list[i].ra_selector & ( 1 << j ) ) {
				event_list[i].ra_rank++;
			}
		}

		if ( is_pentium4() ) {
			event_list[i].ra_escr[0] = event_list[i].ra_bits.escr[0];
			event_list[i].ra_escr[1] = event_list[i].ra_bits.escr[1];
#ifdef DEBUG
			SUBDBG( "i: %d\n", i );
			print_alloc( &event_list[i] );
#endif
		}
	}
	if ( _papi_hwi_bipartite_alloc( event_list, natNum, ESI->CmpIdx ) ) {	/* successfully mapped */
		for ( i = 0; i < natNum; i++ ) {
#ifdef PERFCTR_X86_INTEL_CORE2
			if ( _papi_hwi_system_info.hw_info.model ==
				 PERFCTR_X86_INTEL_CORE2 )
				event_list[i].ra_bits.selector = event_list[i].ra_selector;
#endif
#ifdef DEBUG
			if ( is_pentium4() ) {
				SUBDBG( "i: %d\n", i );
				print_alloc( &event_list[i] );
			}
#endif
			/* Copy all info about this native event to the NativeInfo struct */
			ptr = ESI->NativeInfoArray[i].ni_bits;
			*ptr = event_list[i].ra_bits;

			if ( is_pentium4() ) {
				/* The selector contains the counter bit position. Turn it into a number
				   and store it in the first counter value, zeroing the second. */
				ptr->counter[0] = ffs( event_list[i].ra_selector ) - 1;
				ptr->counter[1] = 0;
			}

			/* Array order on perfctr is event ADD order, not counter #... */
			ESI->NativeInfoArray[i].ni_position = i;
		}
		return 1;
	} else
		return 0;
}

static void
clear_cs_events( hwd_control_state_t * this_state )
{
	unsigned int i, j;

	/* total counters is sum of accumulating (nractrs) and interrupting (nrictrs) */
	j = this_state->control.cpu_control.nractrs +
		this_state->control.cpu_control.nrictrs;

	/* Remove all counter control command values from eventset. */
	for ( i = 0; i < j; i++ ) {
		SUBDBG( "Clearing pmc event entry %d\n", i );
		if ( is_pentium4() ) {
			this_state->control.cpu_control.pmc_map[i] = 0;
			this_state->control.cpu_control.evntsel[i] = 0;
			this_state->control.cpu_control.evntsel_aux[i] =
				this_state->control.cpu_control.
				evntsel_aux[i] & ( ESCR_T0_OS | ESCR_T0_USR );
		} else {
			this_state->control.cpu_control.pmc_map[i] = i;
			this_state->control.cpu_control.evntsel[i]
				= this_state->control.cpu_control.
				evntsel[i] & ( PERF_ENABLE | PERF_OS | PERF_USR );
		}
		this_state->control.cpu_control.ireset[i] = 0;
	}

	if ( is_pentium4() ) {
		/* Clear pebs stuff */
		this_state->control.cpu_control.p4.pebs_enable = 0;
		this_state->control.cpu_control.p4.pebs_matrix_vert = 0;
	}

	/* clear both a and i counter counts */
	this_state->control.cpu_control.nractrs = 0;
	this_state->control.cpu_control.nrictrs = 0;

#ifdef DEBUG
	if ( is_pentium4() )
		print_control( &this_state->control.cpu_control );
#endif
}

/* This function clears the current contents of the control structure and 
   updates it with whatever resources are allocated for all the native events
   in the native info structure array. */
static int
_x86_update_control_state( hwd_control_state_t * this_state,
						   NativeInfo_t * native, int count,
						   hwd_context_t * ctx )
{
	( void ) ctx;			 /*unused */
	unsigned int i, k, retval = PAPI_OK;
	hwd_register_t *bits;
	struct perfctr_cpu_control *cpu_control = &this_state->control.cpu_control;

	/* clear out the events from the control state */
	clear_cs_events( this_state );

	if ( is_pentium4() ) {
		/* fill the counters we're using */
		for ( i = 0; i < ( unsigned int ) count; i++ ) {
			/* dereference the mapping information about this native event */
			bits = native[i].ni_bits;

			/* Add counter control command values to eventset */
			cpu_control->pmc_map[i] = bits->counter[0];
			cpu_control->evntsel[i] = bits->cccr;
			cpu_control->ireset[i] = bits->ireset;
			cpu_control->pmc_map[i] |= FAST_RDPMC;
			cpu_control->evntsel_aux[i] |= bits->event;

			/* pebs_enable and pebs_matrix_vert are shared registers used for replay_events.
			   Replay_events count L1 and L2 cache events. There is only one of each for 
			   the entire eventset. Therefore, there can be only one unique replay_event 
			   per eventset. This means L1 and L2 can't be counted together. Which stinks.
			   This conflict should be trapped in the allocation scheme, but we'll test for it
			   here too, just in case. */
			if ( bits->pebs_enable ) {
				/* if pebs_enable isn't set, just copy */
				if ( cpu_control->p4.pebs_enable == 0 ) {
					cpu_control->p4.pebs_enable = bits->pebs_enable;
					/* if pebs_enable conflicts, flag an error */
				} else if ( cpu_control->p4.pebs_enable != bits->pebs_enable ) {
					SUBDBG
						( "WARNING: P4_update_control_state -- pebs_enable conflict!" );
					retval = PAPI_ECNFLCT;
				}
				/* if pebs_enable == bits->pebs_enable, do nothing */
			}
			if ( bits->pebs_matrix_vert ) {
				/* if pebs_matrix_vert isn't set, just copy */
				if ( cpu_control->p4.pebs_matrix_vert == 0 ) {
					cpu_control->p4.pebs_matrix_vert = bits->pebs_matrix_vert;
					/* if pebs_matrix_vert conflicts, flag an error */
				} else if ( cpu_control->p4.pebs_matrix_vert !=
							bits->pebs_matrix_vert ) {
					SUBDBG
						( "WARNING: P4_update_control_state -- pebs_matrix_vert conflict!" );
					retval = PAPI_ECNFLCT;
				}
				/* if pebs_matrix_vert == bits->pebs_matrix_vert, do nothing */
			}
		}
		this_state->control.cpu_control.nractrs = count;

		/* Make sure the TSC is always on */
		this_state->control.cpu_control.tsc_on = 1;

#ifdef DEBUG
		print_control( &this_state->control.cpu_control );
#endif
	} else {
		switch ( _papi_hwi_system_info.hw_info.model ) {
#ifdef PERFCTR_X86_INTEL_CORE2
		case PERFCTR_X86_INTEL_CORE2:
			/* fill the counters we're using */
			for ( i = 0; i < ( unsigned int ) count; i++ ) {
				for ( k = 0; k < MAX_COUNTERS; k++ )
					if ( native[i].ni_bits->selector & ( 1 << k ) ) {
						break;
					}
				if ( k > 1 )
					this_state->control.cpu_control.pmc_map[i] =
						( k - 2 ) | 0x40000000;
				else
					this_state->control.cpu_control.pmc_map[i] = k;

				/* Add counter control command values to eventset */
				this_state->control.cpu_control.evntsel[i] |=
					native[i].ni_bits->counter_cmd;
			}
			break;
#endif
		default:
			/* fill the counters we're using */
			for ( i = 0; i < ( unsigned int ) count; i++ ) {
				/* Add counter control command values to eventset */
				this_state->control.cpu_control.evntsel[i] |=
					native[i].ni_bits->counter_cmd;
			}
		}
		this_state->control.cpu_control.nractrs = ( unsigned int ) count;
	}
	return retval;
}

static int
_x86_start( hwd_context_t * ctx, hwd_control_state_t * state )
{
	int error;
#ifdef DEBUG
	print_control( &state->control.cpu_control );
#endif

	if ( state->rvperfctr != NULL ) {
		if ( ( error =
			   rvperfctr_control( state->rvperfctr, &state->control ) ) < 0 ) {
			SUBDBG( "rvperfctr_control returns: %d\n", error );
			PAPIERROR( RCNTRL_ERROR );
			return ( PAPI_ESYS );
		}
		return ( PAPI_OK );
	}

	if ( ( error = vperfctr_control( ctx->perfctr, &state->control ) ) < 0 ) {
		SUBDBG( "vperfctr_control returns: %d\n", error );
		PAPIERROR( VCNTRL_ERROR );
		return ( PAPI_ESYS );
	}
	return ( PAPI_OK );
}

static int
_x86_stop( hwd_context_t * ctx, hwd_control_state_t * state )
{
	int error;

	if ( state->rvperfctr != NULL ) {
		if ( rvperfctr_stop( ( struct rvperfctr * ) ctx->perfctr ) < 0 ) {
			PAPIERROR( RCNTRL_ERROR );
			return ( PAPI_ESYS );
		}
		return ( PAPI_OK );
	}

	error = vperfctr_stop( ctx->perfctr );
	if ( error < 0 ) {
		SUBDBG( "vperfctr_stop returns: %d\n", error );
		PAPIERROR( VCNTRL_ERROR );
		return ( PAPI_ESYS );
	}
	return ( PAPI_OK );
}

static int
_x86_read( hwd_context_t * ctx, hwd_control_state_t * spc, long long **dp,
		   int flags )
{
	if ( flags & PAPI_PAUSED ) {
		vperfctr_read_state( ctx->perfctr, &spc->state, NULL );
		if ( !is_pentium4() ) {
			unsigned int i = 0;
			for ( i = 0;
				  i <
				  spc->control.cpu_control.nractrs +
				  spc->control.cpu_control.nrictrs; i++ ) {
				SUBDBG( "vperfctr_read_state: counter %d =  %lld\n", i,
						spc->state.pmc[i] );
			}
		}
	} else {
		SUBDBG( "vperfctr_read_ctrs\n" );
		if ( spc->rvperfctr != NULL ) {
			rvperfctr_read_ctrs( spc->rvperfctr, &spc->state );
		} else {
			vperfctr_read_ctrs( ctx->perfctr, &spc->state );
		}
	}
	*dp = ( long long * ) spc->state.pmc;
#ifdef DEBUG
	{
		if ( ISLEVEL( DEBUG_SUBSTRATE ) ) {
			unsigned int i;
			if ( is_pentium4() ) {
				for ( i = 0; i < spc->control.cpu_control.nractrs; i++ ) {
					SUBDBG( "raw val hardware index %d is %lld\n", i,
							( long long ) spc->state.pmc[i] );
				}
			} else {
				for ( i = 0;
					  i <
					  spc->control.cpu_control.nractrs +
					  spc->control.cpu_control.nrictrs; i++ ) {
					SUBDBG( "raw val hardware index %d is %lld\n", i,
							( long long ) spc->state.pmc[i] );
				}
			}
		}
	}
#endif
	return ( PAPI_OK );
}

static int
_x86_reset( hwd_context_t * ctx, hwd_control_state_t * cntrl )
{
	return ( _x86_start( ctx, cntrl ) );
}

/* Perfctr requires that interrupting counters appear at the end of the pmc list
   In the case a user wants to interrupt on a counter in an evntset that is not
   among the last events, we need to move the perfctr virtual events around to
   make it last. This function swaps two perfctr events, and then adjust the
   position entries in both the NativeInfoArray and the EventInfoArray to keep
   everything consistent. */
static void
swap_events( EventSetInfo_t * ESI, struct hwd_pmc_control *contr, int cntr1,
			 int cntr2 )
{
	unsigned int ui;
	int si, i, j;

	for ( i = 0; i < ESI->NativeCount; i++ ) {
		if ( ESI->NativeInfoArray[i].ni_position == cntr1 )
			ESI->NativeInfoArray[i].ni_position = cntr2;
		else if ( ESI->NativeInfoArray[i].ni_position == cntr2 )
			ESI->NativeInfoArray[i].ni_position = cntr1;
	}

	for ( i = 0; i < ESI->NumberOfEvents; i++ ) {
		for ( j = 0; ESI->EventInfoArray[i].pos[j] >= 0; j++ ) {
			if ( ESI->EventInfoArray[i].pos[j] == cntr1 )
				ESI->EventInfoArray[i].pos[j] = cntr2;
			else if ( ESI->EventInfoArray[i].pos[j] == cntr2 )
				ESI->EventInfoArray[i].pos[j] = cntr1;
		}
	}

	ui = contr->cpu_control.pmc_map[cntr1];
	contr->cpu_control.pmc_map[cntr1] = contr->cpu_control.pmc_map[cntr2];
	contr->cpu_control.pmc_map[cntr2] = ui;

	ui = contr->cpu_control.evntsel[cntr1];
	contr->cpu_control.evntsel[cntr1] = contr->cpu_control.evntsel[cntr2];
	contr->cpu_control.evntsel[cntr2] = ui;

	if ( is_pentium4() ) {
		ui = contr->cpu_control.evntsel_aux[cntr1];
		contr->cpu_control.evntsel_aux[cntr1] =
			contr->cpu_control.evntsel_aux[cntr2];
		contr->cpu_control.evntsel_aux[cntr2] = ui;
	}

	si = contr->cpu_control.ireset[cntr1];
	contr->cpu_control.ireset[cntr1] = contr->cpu_control.ireset[cntr2];
	contr->cpu_control.ireset[cntr2] = si;
}

static int
_x86_set_overflow( EventSetInfo_t * ESI, int EventIndex, int threshold )
{
	struct hwd_pmc_control *contr = &ESI->ctl_state->control;
	int i, ncntrs, nricntrs = 0, nracntrs = 0, retval = 0;
	OVFDBG( "EventIndex=%d\n", EventIndex );

#ifdef DEBUG
	if ( is_pentium4() )
		print_control( &ESI->ctl_state->control.cpu_control );
#endif

	/* The correct event to overflow is EventIndex */
	ncntrs = MY_VECTOR.cmp_info.num_cntrs;
	i = ESI->EventInfoArray[EventIndex].pos[0];

	if ( i >= ncntrs ) {
		PAPIERROR( "Selector id %d is larger than ncntrs %d", i, ncntrs );
		return PAPI_EINVAL;
	}

	if ( threshold != 0 ) {	 /* Set an overflow threshold */
		retval = _papi_hwi_start_signal( MY_VECTOR.cmp_info.hardware_intr_sig,
										 NEED_CONTEXT,
										 MY_VECTOR.cmp_info.CmpIdx );
		if ( retval != PAPI_OK )
			return ( retval );

		/* overflow interrupt occurs on the NEXT event after overflow occurs
		   thus we subtract 1 from the threshold. */
		contr->cpu_control.ireset[i] = ( -threshold + 1 );

		if ( is_pentium4() )
			contr->cpu_control.evntsel[i] |= CCCR_OVF_PMI_T0;
		else
			contr->cpu_control.evntsel[i] |= PERF_INT_ENABLE;

		contr->cpu_control.nrictrs++;
		contr->cpu_control.nractrs--;
		nricntrs = ( int ) contr->cpu_control.nrictrs;
		nracntrs = ( int ) contr->cpu_control.nractrs;
		contr->si_signo = MY_VECTOR.cmp_info.hardware_intr_sig;

		/* move this event to the bottom part of the list if needed */
		if ( i < nracntrs )
			swap_events( ESI, contr, i, nracntrs );
		OVFDBG( "Modified event set\n" );
	} else {
	  if ( is_pentium4() && contr->cpu_control.evntsel[i] & CCCR_OVF_PMI_T0 ) {
			contr->cpu_control.ireset[i] = 0;
			contr->cpu_control.evntsel[i] &= ( ~CCCR_OVF_PMI_T0 );
			contr->cpu_control.nrictrs--;
			contr->cpu_control.nractrs++;
	  } else if ( !is_pentium4() &&
					contr->cpu_control.evntsel[i] & PERF_INT_ENABLE ) {
			contr->cpu_control.ireset[i] = 0;
			contr->cpu_control.evntsel[i] &= ( ~PERF_INT_ENABLE );
			contr->cpu_control.nrictrs--;
			contr->cpu_control.nractrs++;
		}

		nricntrs = ( int ) contr->cpu_control.nrictrs;
		nracntrs = ( int ) contr->cpu_control.nractrs;

		/* move this event to the top part of the list if needed */
		if ( i >= nracntrs )
			swap_events( ESI, contr, i, nracntrs - 1 );

		if ( !nricntrs )
			contr->si_signo = 0;

		OVFDBG( "Modified event set\n" );

		retval = _papi_hwi_stop_signal( MY_VECTOR.cmp_info.hardware_intr_sig );
	}

#ifdef DEBUG
	if ( is_pentium4() )
		print_control( &ESI->ctl_state->control.cpu_control );
#endif
	OVFDBG( "End of call. Exit code: %d\n", retval );
	return ( retval );
}

static int
_x86_stop_profiling( ThreadInfo_t * master, EventSetInfo_t * ESI )
{
	( void ) master;		 /*unused */
	( void ) ESI;			 /*unused */
	return ( PAPI_OK );
}


papi_vector_t _x86_vector = {
	.cmp_info = {
				 /* default component information (unspecified values are initialized to 0) */
				 .num_mpx_cntrs = PAPI_MPX_DEF_DEG,
				 .default_domain = PAPI_DOM_USER,
				 .available_domains = PAPI_DOM_USER | PAPI_DOM_KERNEL,
				 .default_granularity = PAPI_GRN_THR,
				 .available_granularities = PAPI_GRN_THR,
				 .itimer_sig = PAPI_INT_MPX_SIGNAL,
				 .itimer_num = PAPI_INT_ITIMER,
				 .itimer_res_ns = 1,
				 .hardware_intr_sig = PAPI_INT_SIGNAL,

				 /* component specific cmp_info initializations */
				 .fast_real_timer = 1,
				 .fast_virtual_timer = 1,
				 .attach = 1,
				 .attach_must_ptrace = 1,
				 .cntr_umasks = 1,
				 }
	,

	/* sizes of framework-opaque component-private structures */
	.size = {
			 .context = sizeof ( X86_perfctr_context_t ),
			 .control_state = sizeof ( X86_perfctr_control_t ),
			 .reg_value = sizeof ( X86_register_t ),
			 .reg_alloc = sizeof ( X86_reg_alloc_t ),
			 }
	,

	/* function pointers in this component */
	.init_control_state = _x86_init_control_state,
	.start = _x86_start,
	.stop = _x86_stop,
	.read = _x86_read,
	.bpt_map_set = _x86_bpt_map_set,
	.bpt_map_avail = _x86_bpt_map_avail,
	.bpt_map_exclusive = _x86_bpt_map_exclusive,
	.bpt_map_shared = _x86_bpt_map_shared,
	.bpt_map_preempt = _x86_bpt_map_preempt,
	.bpt_map_update = _x86_bpt_map_update,
	.allocate_registers = _x86_allocate_registers,
	.update_control_state = _x86_update_control_state,
	.set_domain = _x86_set_domain,
	.reset = _x86_reset,
	.set_overflow = _x86_set_overflow,
	.stop_profiling = _x86_stop_profiling,

	.init_substrate = _perfctr_init_substrate,
	.ctl =            _perfctr_ctl,
	.dispatch_timer = _perfctr_dispatch_timer,
	.init =           _perfctr_init,
	.shutdown =       _perfctr_shutdown,


	/* from OS */
	.update_shlib_info = _linux_update_shlib_info,
	.get_dmem_info =     _linux_get_dmem_info,
	.get_memory_info =   _linux_get_memory_info,
	.get_system_info =   _linux_get_system_info,
	.get_real_usec =     _linux_get_real_usec,
	.get_real_cycles =   _linux_get_real_cycles,
	.get_virt_cycles =   _linux_get_virt_cycles,
	.get_virt_usec =     _linux_get_virt_usec,

	/* from libpfm */
	.ntv_enum_events   = _papi_libpfm_ntv_enum_events,
	.ntv_name_to_code  = _papi_libpfm_ntv_name_to_code,
	.ntv_code_to_name  = _papi_libpfm_ntv_code_to_name,
	.ntv_code_to_descr = _papi_libpfm_ntv_code_to_descr,
	.ntv_code_to_bits  = _papi_libpfm_ntv_code_to_bits,
	.ntv_bits_to_info  = _papi_libpfm_ntv_bits_to_info,

};
