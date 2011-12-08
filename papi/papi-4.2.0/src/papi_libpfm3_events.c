/*
* File:    papi_libpfm3_events.c
* Author:  Dan Terpstra: blantantly extracted from Phil's perfmon.c
*          mucci@cs.utk.edu
*/

/* TODO LIST:
    - Events for all platforms
xxx - Derived events for all platforms
xxx - hwd_ntv_name_to_code
xxx - Make native map carry major events, not umasks
xxx - Enum event uses native_map not pfm()
xxx - bits_to_info uses native_map not pfm()
*/

#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "papi.h"
#include "papi_internal.h"
#include "papi_vector.h"
#include "papi_memory.h"

#include "perfmon/pfmlib.h"

#include "papi_libpfm_events.h"

/* these define cccr and escr register bits, and the p4 event structure */
#include "perfmon/pfmlib_pentium4.h"
#include "../lib/pfmlib_pentium4_priv.h"

#define P4_REPLAY_REAL_MASK 0x00000003

extern pentium4_escr_reg_t pentium4_escrs[];
extern pentium4_cccr_reg_t pentium4_cccrs[];
extern pentium4_event_t pentium4_events[];

extern papi_vector_t MY_VECTOR;
extern unsigned int PAPI_NATIVE_EVENT_AND_MASK;
extern unsigned int PAPI_NATIVE_EVENT_SHIFT;
extern unsigned int PAPI_NATIVE_UMASK_AND_MASK;
extern unsigned int PAPI_NATIVE_UMASK_MAX;
extern unsigned int PAPI_NATIVE_UMASK_SHIFT;

/* Globals declared extern elsewhere */

volatile unsigned int _papi_hwd_lock_data[PAPI_MAX_LOCK];

/* NOTE: PAPI stores umask info in a variable sized (16 bit?) bitfield.
    Perfmon2 stores umask info in a large (48 element?) array of values.
    Native event encodings for perfmon2 contain array indices
    encoded as bits in this bitfield. These indices must be converted
    into a umask value before programming the counters. For Perfmon,
    this is done by converting back to an array of values; for 
    perfctr, it must be done by looking up the values.
*/

/* This routine is used to step through all possible combinations of umask
    values. It assumes that mask contains a valid combination of array indices
    for this event. */
static inline int
encode_native_event_raw( unsigned int event, unsigned int mask )
{
	unsigned int tmp = event << PAPI_NATIVE_EVENT_SHIFT;
	SUBDBG( "Old native index was 0x%08x with 0x%08x mask\n", tmp, mask );
	tmp = tmp | ( mask << PAPI_NATIVE_UMASK_SHIFT );
	SUBDBG( "New encoding is 0x%08x\n", tmp | PAPI_NATIVE_MASK );
	return ( int ) ( tmp | PAPI_NATIVE_MASK );
}

/* This routine converts array indices contained in the mask_values array
    into bits in the umask field that is OR'd into the native event code.
    These bits are NOT the mask values themselves, but indices into an array
    of mask values contained in the native event table. */
static inline int
encode_native_event( unsigned int event, unsigned int num_mask,
					 unsigned int *mask_values )
{
	unsigned int i;
	unsigned int tmp = event << PAPI_NATIVE_EVENT_SHIFT;
	SUBDBG( "Native base event is 0x%08x with %d masks\n", tmp, num_mask );
	for ( i = 0; i < num_mask; i++ ) {
		SUBDBG( "Mask index is 0x%08x\n", mask_values[i] );
		tmp = tmp | ( ( 1 << mask_values[i] ) << PAPI_NATIVE_UMASK_SHIFT );
	}
	SUBDBG( "Full native encoding is 0x%08x\n", tmp | PAPI_NATIVE_MASK );
	return ( int ) ( tmp | PAPI_NATIVE_MASK );
}

#if 0

/* was this ncessary?  Was used by the preset code */

static int
setup_preset_term( int *native, pfmlib_event_t * event )
{
	/* It seems this could be greatly simplified. If impl_cnt is non-zero,
	   the event lives on a counter. Therefore the entire routine could be:
	   if (impl_cnt!= 0) encode_native_event.
	   Am I wrong?
	 */
	pfmlib_regmask_t impl_cnt, evnt_cnt;
	unsigned int n;
	int ret;
	unsigned int j;

	/* find out which counters it lives on */
	if ( ( ret =
		   pfm_get_event_counters( event->event,
								   &evnt_cnt ) ) != PFMLIB_SUCCESS ) {
		PAPIERROR( "pfm_get_event_counters(%d,%p): %s", event->event, &evnt_cnt,
				   pfm_strerror( ret ) );
		return ( PAPI_EBUG );
	}
	if ( ( ret = pfm_get_impl_counters( &impl_cnt ) ) != PFMLIB_SUCCESS ) {
		PAPIERROR( "pfm_get_impl_counters(%p): %s", &impl_cnt,
				   pfm_strerror( ret ) );
		return ( PAPI_EBUG );
	}

	/* Make sure this event lives on some counter, if so, put in the description. If not, BUG */
	if ( ( ret = pfm_get_num_counters( &n ) ) != PFMLIB_SUCCESS ) {
		PAPIERROR( "pfm_get_num_counters(%d): %s", n, pfm_strerror( ret ) );
		return ( PAPI_EBUG );
	}

	for ( j = 0; n; j++ ) {
		if ( pfm_regmask_isset( &impl_cnt, j ) ) {
			n--;
			if ( pfm_regmask_isset( &evnt_cnt, j ) ) {
				*native =
					encode_native_event( event->event, event->num_masks,
										 event->unit_masks );
				return ( PAPI_OK );
			}
		}
	}

	PAPIERROR
		( "PAPI preset 0x%08x PFM event %d did not have any available counters",
		  event->event, j );
	return ( PAPI_ENOEVNT );
}
#endif

/* Break a PAPI native event code into its composite event code and pfm mask bits */
static inline int
_pfm_decode_native_event( unsigned int EventCode, unsigned int *event,
						  unsigned int *umask )
{
	unsigned int tevent, major, minor;

	tevent = EventCode & PAPI_NATIVE_AND_MASK;
	major = ( tevent & PAPI_NATIVE_EVENT_AND_MASK ) >> PAPI_NATIVE_EVENT_SHIFT;
	if ( ( int ) major >= MY_VECTOR.cmp_info.num_native_events )
		return ( PAPI_ENOEVNT );

	minor = ( tevent & PAPI_NATIVE_UMASK_AND_MASK ) >> PAPI_NATIVE_UMASK_SHIFT;
	*event = major;
	*umask = minor;
	SUBDBG( "EventCode 0x%08x is event %d, umask 0x%x\n", EventCode, major,
			minor );
	return ( PAPI_OK );
}

/* convert a collection of pfm mask bits into an array of pfm mask indices */
static inline int
prepare_umask( unsigned int foo, unsigned int *values )
{
	unsigned int tmp = foo, i;
	int j = 0;

	SUBDBG( "umask 0x%x\n", tmp );
	while ( ( i = ( unsigned int ) ffs( ( int ) tmp ) ) ) {
		tmp = tmp ^ ( 1 << ( i - 1 ) );
		values[j] = i - 1;
		SUBDBG( "umask %d is %d\n", j, values[j] );
		j++;
	}
	return ( j );
}

/* convert the mask values in a pfm event structure into a PAPI unit mask */
static inline unsigned int
convert_pfm_masks( pfmlib_event_t * gete )
{
	int ret;
	unsigned int i, code, tmp = 0;

	for ( i = 0; i < gete->num_masks; i++ ) {
		if ( ( ret =
			   pfm_get_event_mask_code( gete->event, gete->unit_masks[i],
										&code ) ) == PFMLIB_SUCCESS ) {
			SUBDBG( "Mask value is 0x%08x\n", code );
			tmp |= code;
		} else {
			PAPIERROR( "pfm_get_event_mask_code(0x%x,%d,%p): %s", gete->event,
					   i, &code, pfm_strerror( ret ) );
		}
	}
	return ( tmp );
}

/* convert an event code and pfm unit mask into a PAPI unit mask */
static inline unsigned int
_pfm_convert_umask( unsigned int event, unsigned int umask )
{
	pfmlib_event_t gete;
	memset( &gete, 0, sizeof ( gete ) );
	gete.event = event;
	gete.num_masks = ( unsigned int ) prepare_umask( umask, gete.unit_masks );
	return ( convert_pfm_masks( &gete ) );
}

/* convert libpfm error codes to PAPI error codes for 
	more informative error reporting */
int
_papi_libpfm_error( int pfm_error )
{
	switch ( pfm_error ) {
		case PFMLIB_SUCCESS:		return PAPI_OK;			/* success */
		case PFMLIB_ERR_NOTSUPP:	return PAPI_ENOSUPP;	/* function not supported */
		case PFMLIB_ERR_INVAL:		return PAPI_EINVAL;		/* invalid parameters */
		case PFMLIB_ERR_NOINIT:		return PAPI_ENOINIT;	/* library was not initialized */
		case PFMLIB_ERR_NOTFOUND:	return PAPI_ENOEVNT;	/* event not found */
		case PFMLIB_ERR_NOASSIGN:	return PAPI_ECNFLCT;	/* cannot assign events to counters */
		case PFMLIB_ERR_FULL:		return PAPI_EBUF;		/* buffer is full or too small */
		case PFMLIB_ERR_EVTMANY:	return PAPI_EMISC;		/* event used more than once */
		case PFMLIB_ERR_MAGIC:		return PAPI_EBUG;		/* invalid library magic number */
		case PFMLIB_ERR_FEATCOMB:	return PAPI_ECOMBO;		/* invalid combination of features */
		case PFMLIB_ERR_EVTSET:		return PAPI_ENOEVST;	/* incompatible event sets */
		case PFMLIB_ERR_EVTINCOMP:	return PAPI_ECNFLCT;	/* incompatible event combination */
		case PFMLIB_ERR_TOOMANY:	return PAPI_ECOUNT;		/* too many events or unit masks */
		case PFMLIB_ERR_BADHOST:	return PAPI_ESYS;		/* not supported by host CPU */
		case PFMLIB_ERR_UMASK:		return PAPI_EATTR;		/* invalid or missing unit mask */
		case PFMLIB_ERR_NOMEM:		return PAPI_ENOMEM;		/* out of memory */

		/* Itanium only */
		case PFMLIB_ERR_IRRTOOBIG:		/* code range too big */
		case PFMLIB_ERR_IRREMPTY:		/* empty code range */
		case PFMLIB_ERR_IRRINVAL:		/* invalid code range */
		case PFMLIB_ERR_IRRTOOMANY:		/* too many code ranges */
		case PFMLIB_ERR_DRRINVAL:		/* invalid data range */
		case PFMLIB_ERR_DRRTOOMANY:		/* too many data ranges */
		case PFMLIB_ERR_IRRALIGN:		/* bad alignment for code range */
		case PFMLIB_ERR_IRRFLAGS:		/* code range missing flags */
		default:
			return PAPI_EINVAL;
	}
}

int
_papi_libpfm_ntv_name_to_code( char *name, unsigned int *event_code )
{
	pfmlib_event_t event;
	unsigned int i;
	int ret;

	SUBDBG( "pfm_find_full_event(%s,%p)\n", name, &event );
	ret = pfm_find_full_event( name, &event );
	if ( ret == PFMLIB_SUCCESS ) {
		SUBDBG( "Full event name found\n" );
		/* we can only capture PAPI_NATIVE_UMASK_MAX or fewer masks */
		if ( event.num_masks > PAPI_NATIVE_UMASK_MAX ) {
			SUBDBG( "num_masks (%d) > max masks (%d)\n", event.num_masks,
					PAPI_NATIVE_UMASK_MAX );
			return PAPI_ENOEVNT;
		} else {
			/* no mask index can exceed PAPI_NATIVE_UMASK_MAX */
			for ( i = 0; i < event.num_masks; i++ ) {
				if ( event.unit_masks[i] > PAPI_NATIVE_UMASK_MAX ) {
					SUBDBG( "mask index (%d) > max masks (%d)\n",
							event.unit_masks[i], PAPI_NATIVE_UMASK_MAX );
					return PAPI_ENOEVNT;
				}
			}
			*event_code =
				encode_native_event( event.event, event.num_masks,
									 event.unit_masks );
			return PAPI_OK;
		}
	} else if ( ret == PFMLIB_ERR_UMASK ) {
		SUBDBG( "UMASK error, looking for base event only\n" );
		ret = pfm_find_event( name, &event.event );
		if ( ret == PFMLIB_SUCCESS ) {
			*event_code = encode_native_event( event.event, 0, 0 );
			return PAPI_EATTR;
		}
	}
	return PAPI_ENOEVNT;
}

int
_papi_libpfm_ntv_code_to_name( unsigned int EventCode, char *ntv_name, int len )
{
	int ret;
	unsigned int event, umask;
	pfmlib_event_t gete;

	memset( &gete, 0, sizeof ( gete ) );

	if ( _pfm_decode_native_event( EventCode, &event, &umask ) != PAPI_OK )
		return ( PAPI_ENOEVNT );

	gete.event = event;
	gete.num_masks = ( unsigned int ) prepare_umask( umask, gete.unit_masks );
	if ( gete.num_masks == 0 )
		ret = pfm_get_event_name( gete.event, ntv_name, ( size_t ) len );
	else
		ret = pfm_get_full_event_name( &gete, ntv_name, ( size_t ) len );
	if ( ret != PFMLIB_SUCCESS ) {
		char tmp[PAPI_2MAX_STR_LEN];
		pfm_get_event_name( gete.event, tmp, sizeof ( tmp ) );
		/* Skip error message if event is not supported by host cpu;
		 * we don't need to give this info away for papi_native_avail util */
		if ( ret != PFMLIB_ERR_BADHOST )
			PAPIERROR
				( "pfm_get_full_event_name(%p(event %d,%s,%d masks),%p,%d): %d -- %s",
				  &gete, gete.event, tmp, gete.num_masks, ntv_name, len, ret,
				  pfm_strerror( ret ) );
		if ( ret == PFMLIB_ERR_FULL )
			return ( PAPI_EBUF );
		return ( PAPI_ESBSTR );
	}
	return ( PAPI_OK );
}

int
_papi_libpfm_ntv_code_to_descr( unsigned int EventCode, char *ntv_descr, int len )
{
	unsigned int event, umask;
	char *eventd, **maskd, *tmp;
	int i, ret;
	pfmlib_event_t gete;
	size_t total_len = 0;

	memset( &gete, 0, sizeof ( gete ) );

	if ( _pfm_decode_native_event( EventCode, &event, &umask ) != PAPI_OK )
		return ( PAPI_ENOEVNT );

	ret = pfm_get_event_description( event, &eventd );
	if ( ret != PFMLIB_SUCCESS ) {
		PAPIERROR( "pfm_get_event_description(%d,%p): %s",
				   event, &eventd, pfm_strerror( ret ) );
		return ( PAPI_ENOEVNT );
	}

	if ( ( gete.num_masks =
		   ( unsigned int ) prepare_umask( umask, gete.unit_masks ) ) ) {
		maskd = ( char ** ) malloc( gete.num_masks * sizeof ( char * ) );
		if ( maskd == NULL ) {
			free( eventd );
			return ( PAPI_ENOMEM );
		}
		for ( i = 0; i < ( int ) gete.num_masks; i++ ) {
			ret =
				pfm_get_event_mask_description( event, gete.unit_masks[i],
												&maskd[i] );
			if ( ret != PFMLIB_SUCCESS ) {
				PAPIERROR( "pfm_get_event_mask_description(%d,%d,%p): %s",
						   event, umask, &maskd, pfm_strerror( ret ) );
				free( eventd );
				for ( ; i >= 0; i-- )
					free( maskd[i] );
				free( maskd );
				return ( PAPI_EINVAL );
			}
			total_len += strlen( maskd[i] );
		}
		tmp =
			( char * ) malloc( strlen( eventd ) + strlen( ", masks:" ) +
							   total_len + gete.num_masks + 1 );
		if ( tmp == NULL ) {
			for ( i = ( int ) gete.num_masks - 1; i >= 0; i-- )
				free( maskd[i] );
			free( maskd );
			free( eventd );
		}
		tmp[0] = '\0';
		strcat( tmp, eventd );
		strcat( tmp, ", masks:" );
		for ( i = 0; i < ( int ) gete.num_masks; i++ ) {
			if ( i != 0 )
				strcat( tmp, "," );
			strcat( tmp, maskd[i] );
			free( maskd[i] );
		}
		free( maskd );
	} else {
		tmp = ( char * ) malloc( strlen( eventd ) + 1 );
		if ( tmp == NULL ) {
			free( eventd );
			return ( PAPI_ENOMEM );
		}
		tmp[0] = '\0';
		strcat( tmp, eventd );
		free( eventd );
	}
	strncpy( ntv_descr, tmp, ( size_t ) len );
	if ( ( int ) strlen( tmp ) > len - 1 )
		ret = PAPI_EBUF;
	else
		ret = PAPI_OK;
	free( tmp );
	return ( ret );
}

int
_papi_libpfm_ntv_enum_events( unsigned int *EventCode, int modifier )
{
	unsigned int event, umask, num_masks;
	int ret;

	if ( modifier == PAPI_ENUM_FIRST ) {
		*EventCode = PAPI_NATIVE_MASK;	/* assumes first native event is always 0x4000000 */
		return ( PAPI_OK );
	}

	if ( _pfm_decode_native_event( *EventCode, &event, &umask ) != PAPI_OK )
		return ( PAPI_ENOEVNT );

	ret = pfm_get_num_event_masks( event, &num_masks );
	if ( ret != PFMLIB_SUCCESS ) {
		PAPIERROR( "pfm_get_num_event_masks(%d,%p): %s", event, &num_masks,
				   pfm_strerror( ret ) );
		return ( PAPI_ENOEVNT );
	}
	if ( num_masks > PAPI_NATIVE_UMASK_MAX )
		num_masks = PAPI_NATIVE_UMASK_MAX;
	SUBDBG( "This is umask %d of %d\n", umask, num_masks );

	if ( modifier == PAPI_ENUM_EVENTS ) {
		if ( event < ( unsigned int ) MY_VECTOR.cmp_info.num_native_events - 1 ) {
			*EventCode =
				( unsigned int ) encode_native_event_raw( event + 1, 0 );
			return ( PAPI_OK );
		}
		return ( PAPI_ENOEVNT );
	} else if ( modifier == PAPI_NTV_ENUM_UMASK_COMBOS ) {
		if ( umask + 1 < ( unsigned int ) ( 1 << num_masks ) ) {
			*EventCode =
				( unsigned int ) encode_native_event_raw( event, umask + 1 );
			return ( PAPI_OK );
		}
		return ( PAPI_ENOEVNT );
	} else if ( modifier == PAPI_NTV_ENUM_UMASKS ) {
		int thisbit = ffs( ( int ) umask );

		SUBDBG( "First bit is %d in %08x\b\n", thisbit - 1, umask );
		thisbit = 1 << thisbit;

		if ( thisbit & ( ( 1 << num_masks ) - 1 ) ) {
			*EventCode =
				( unsigned int ) encode_native_event_raw( event,
														  ( unsigned int )
														  thisbit );
			return ( PAPI_OK );
		}
		return ( PAPI_ENOEVNT );
	} else
		return ( PAPI_EINVAL );
}


#ifndef PERFCTR_PFM_EVENTS

int
_papi_libpfm_ntv_code_to_bits( unsigned int EventCode, hwd_register_t * bits )
{
	unsigned int event, umask;
	pfmlib_event_t gete;

	/* For PFM & Perfmon, native info is just an index into the PFM event table. */
	if ( _pfm_decode_native_event( EventCode, &event, &umask ) != PAPI_OK )
		return ( PAPI_ENOEVNT );

	memset( &gete, 0x0, sizeof ( pfmlib_event_t ) );

	gete.event = event;
	gete.num_masks = prepare_umask( umask, gete.unit_masks );

	memcpy( bits, &gete, sizeof ( pfmlib_event_t ) );
	return ( PAPI_OK );
}

static char *
_pmc_name( int i )
{
	( void ) i;				 /*unused */
	return "Event Code";
}

int
_papi_libpfm_ntv_bits_to_info( hwd_register_t * bits, char *names,
			       unsigned int *values, int name_len, int count )
{
	int ret;
	pfmlib_regmask_t selector;
	int j, n = MY_VECTOR.cmp_info.num_cntrs;
	int foo, did_something = 0;
	unsigned int umask;

	if ( ( ret =
		   pfm_get_event_counters( ( ( pfm_register_t * ) bits )->event,
								   &selector ) ) != PFMLIB_SUCCESS ) {
		PAPIERROR( "pfm_get_event_counters(%d,%p): %s",
				   ( ( pfm_register_t * ) bits )->event, &selector,
				   pfm_strerror( ret ) );
		return ( PAPI_ESBSTR );
	}

	for ( j = 0; n; j++ ) {
		if ( pfm_regmask_isset( &selector, j ) ) {
			if ( ( ret =
				   pfm_get_event_code_counter( ( ( pfm_register_t * ) bits )->
											   event, j,
											   &foo ) ) != PFMLIB_SUCCESS ) {
				PAPIERROR( "pfm_get_event_code_counter(%d,%d,%p): %s",
						   ( ( pfm_register_t * ) bits )->event, j, &foo,
						   pfm_strerror( ret ) );
				return ( PAPI_EBUG );
			}
			/* Overflow check */
			if ( ( int )
				 ( did_something * name_len + strlen( _pmc_name( j ) ) + 1 ) >=
				 count * name_len ) {
				SUBDBG( "Would overflow register name array." );
				return ( did_something );
			}
			values[did_something] = foo;
			strncpy( &names[did_something * name_len], _pmc_name( j ),
					 name_len );
			did_something++;
			if ( did_something == count )
				break;
		}
		n--;
	}
	/* assumes umask is unchanged, even if event code changes */
	umask = convert_pfm_masks( bits );
	if ( umask && ( did_something < count ) ) {
		values[did_something] = umask;
		if ( strlen( &names[did_something * name_len] ) )
			strncpy( &names[did_something * name_len], " Unit Mask", name_len );
		else
			strncpy( &names[did_something * name_len], "Unit Mask", name_len );
		did_something++;
	}
	return ( did_something );
}

#else

static pentium4_replay_regs_t p4_replay_regs[] = {
	/* 0 */ {.enb = 0,
			 /* dummy */
			 .mat_vert = 0,
			 },
	/* 1 */ {.enb = 0,
			 /* dummy */
			 .mat_vert = 0,
			 },
	/* 2 */ {.enb = 0x01000001,
			 /* 1stL_cache_load_miss_retired */
			 .mat_vert = 0x00000001,
			 },
	/* 3 */ {.enb = 0x01000002,
			 /* 2ndL_cache_load_miss_retired */
			 .mat_vert = 0x00000001,
			 },
	/* 4 */ {.enb = 0x01000004,
			 /* DTLB_load_miss_retired */
			 .mat_vert = 0x00000001,
			 },
	/* 5 */ {.enb = 0x01000004,
			 /* DTLB_store_miss_retired */
			 .mat_vert = 0x00000002,
			 },
	/* 6 */ {.enb = 0x01000004,
			 /* DTLB_all_miss_retired */
			 .mat_vert = 0x00000003,
			 },
	/* 7 */ {.enb = 0x01018001,
			 /* Tagged_mispred_branch */
			 .mat_vert = 0x00000010,
			 },
	/* 8 */ {.enb = 0x01000200,
			 /* MOB_load_replay_retired */
			 .mat_vert = 0x00000001,
			 },
	/* 9 */ {.enb = 0x01000400,
			 /* split_load_retired */
			 .mat_vert = 0x00000001,
			 },
	/* 10 */ {.enb = 0x01000400,
			  /* split_store_retired */
			  .mat_vert = 0x00000002,
			  },
};

/* this maps the arbitrary pmd index in libpfm/pentium4_events.h to the intel documentation */
static int pfm2intel[] =
	{ 0, 1, 4, 5, 8, 9, 12, 13, 16, 2, 3, 6, 7, 10, 11, 14, 15, 17 };

/* Reports the elements of the hwd_register_t struct as an array of names and a matching 
   array of values. Maximum string length is name_len. Maximum number of values is count. */
static void
copy_value( unsigned int val, char *nam, char *names, unsigned int *values,
			int len )
{
	*values = val;
	strncpy( names, nam, ( size_t ) len );
	names[len - 1] = 0;
}

static inline int is_pentium4(void) {
  if ( ( _papi_hwi_system_info.hw_info.vendor == PAPI_VENDOR_INTEL ) &&
       ( _papi_hwi_system_info.hw_info.cpuid_family == 15 )) {
    return 1;
  }

  return 0;

}


int
_papi_libpfm_ntv_bits_to_info( hwd_register_t * bits, char *names,
							unsigned int *values, int name_len, int count )
{
	int i = 0;

	if ( is_pentium4() ) {
		copy_value( bits->cccr, "P4 CCCR", &names[i * name_len], &values[i],
					name_len );
		if ( ++i == count )
			return ( i );
		copy_value( bits->event, "P4 Event", &names[i * name_len], &values[i],
					name_len );
		if ( ++i == count )
			return ( i );
		copy_value( bits->pebs_enable, "P4 PEBS Enable", &names[i * name_len],
					&values[i], name_len );
		if ( ++i == count )
			return ( i );
		copy_value( bits->pebs_matrix_vert, "P4 PEBS Matrix Vertical",
					&names[i * name_len], &values[i], name_len );
		if ( ++i == count )
			return ( i );
		copy_value( bits->ireset, "P4 iReset", &names[i * name_len], &values[i],
					name_len );
	} else {
		copy_value( bits->selector, "Event Selector", &names[i * name_len],
					&values[i], name_len );
		if ( ++i == count )
			return ( i );
		copy_value( ( unsigned int ) bits->counter_cmd, "Event Code",
					&names[i * name_len], &values[i], name_len );
	}
	return ( ++i );
}


/* This call is broken. Selector can be much bigger than 32 bits. It should be a pfmlib_regmask_t - pjm */
/* Also, libpfm assumes events can live on different counters with different codes. This call only returns
    the first occurence found. */
/* Right now its only called by ntv_code_to_bits in perfctr-p3, so we're ok. But for it to be
    generally useful it should be fixed. - dkt */
static int
_pfm_get_counter_info( unsigned int event, unsigned int *selector, int *code )
{
	pfmlib_regmask_t cnt, impl;
	unsigned int num;
	unsigned int i, first = 1;
	int ret;

	if ( ( ret = pfm_get_event_counters( event, &cnt ) ) != PFMLIB_SUCCESS ) {
		PAPIERROR( "pfm_get_event_counters(%d,%p): %s", event, &cnt,
				   pfm_strerror( ret ) );
		return ( PAPI_ESBSTR );
	}
	if ( ( ret = pfm_get_num_counters( &num ) ) != PFMLIB_SUCCESS ) {
		PAPIERROR( "pfm_get_num_counters(%p): %s", num, pfm_strerror( ret ) );
		return ( PAPI_ESBSTR );
	}
	if ( ( ret = pfm_get_impl_counters( &impl ) ) != PFMLIB_SUCCESS ) {
		PAPIERROR( "pfm_get_impl_counters(%p): %s", &impl,
				   pfm_strerror( ret ) );
		return ( PAPI_ESBSTR );
	}

	*selector = 0;
	for ( i = 0; num; i++ ) {
		if ( pfm_regmask_isset( &impl, i ) )
			num--;
		if ( pfm_regmask_isset( &cnt, i ) ) {
			if ( first ) {
				if ( ( ret =
					   pfm_get_event_code_counter( event, i,
												   code ) ) !=
					 PFMLIB_SUCCESS ) {
					PAPIERROR( "pfm_get_event_code_counter(%d, %d, %p): %s",
						   event, i, code, pfm_strerror( ret ) );
					return ( PAPI_ESBSTR );
				}
				first = 0;
			}
			*selector |= 1 << i;
		}
	}
	return ( PAPI_OK );
}



/* perfctr-p3 assumes each event has only a single command code
       libpfm assumes each counter might have a different code. */
int
_papi_libpfm_ntv_code_to_bits( unsigned int EventCode, hwd_register_t * bits )
{
  if ( is_pentium4() ) {
		pentium4_escr_value_t escr_value;
		pentium4_cccr_value_t cccr_value;
		unsigned int umask, num_masks, replay_mask, unit_masks[12];
		unsigned int event, event_mask;
		unsigned int tag_value, tag_enable;
		unsigned int i;
		int j, escr, cccr, pmd;

		if ( _pfm_decode_native_event( EventCode, &event, &umask ) != PAPI_OK )
			return ( PAPI_ENOEVNT );

		/* for each allowed escr (1 or 2) find the allowed cccrs.
		   for each allowed cccr find the pmd index
		   convert to an intel counter number; or it into bits->counter */
		for ( i = 0; i < MAX_ESCRS_PER_EVENT; i++ ) {
			bits->counter[i] = 0;
			escr = pentium4_events[event].allowed_escrs[i];
			if ( escr < 0 ) {
				continue;
			}

			bits->escr[i] = escr;

			for ( j = 0; j < MAX_CCCRS_PER_ESCR; j++ ) {
				cccr = pentium4_escrs[escr].allowed_cccrs[j];
				if ( cccr < 0 ) {
					continue;
				}

				pmd = pentium4_cccrs[cccr].pmd;
				bits->counter[i] |= ( 1 << pfm2intel[pmd] );
			}
		}

		/* if there's only one valid escr, copy the values */
		if ( escr < 0 ) {
			bits->escr[1] = bits->escr[0];
			bits->counter[1] = bits->counter[0];
		}

		/* Calculate the event-mask value. Invalid masks
		 * specified by the caller are ignored. */
		tag_value = 0;
		tag_enable = 0;
		event_mask = _pfm_convert_umask( event, umask );

		if ( event_mask & 0xF0000 ) {
			tag_enable = 1;
			tag_value = ( ( event_mask & 0xF0000 ) >> EVENT_MASK_BITS );
		}

		event_mask &= 0x0FFFF;	/* mask off possible tag bits */

		/* Set up the ESCR and CCCR register values. */
		escr_value.val = 0;
		escr_value.bits.t1_usr = 0;	/* controlled by kernel */
		escr_value.bits.t1_os = 0;	/* controlled by kernel */
//    escr_value.bits.t0_usr       = (plm & PFM_PLM3) ? 1 : 0;
//    escr_value.bits.t0_os        = (plm & PFM_PLM0) ? 1 : 0;
		escr_value.bits.tag_enable = tag_enable;
		escr_value.bits.tag_value = tag_value;
		escr_value.bits.event_mask = event_mask;
		escr_value.bits.event_select = pentium4_events[event].event_select;
		escr_value.bits.reserved = 0;

		/* initialize the proper bits in the cccr register */
		cccr_value.val = 0;
		cccr_value.bits.reserved1 = 0;
		cccr_value.bits.enable = 1;
		cccr_value.bits.escr_select = pentium4_events[event].escr_select;
		cccr_value.bits.active_thread = 3;	/* FIXME: This is set to count when either logical
											 *        CPU is active. Need a way to distinguish
											 *        between logical CPUs when HT is enabled.
											 *        the docs say these bits should always 
											 *        be set.                                  */
		cccr_value.bits.compare = 0;	/* FIXME: What do we do with "threshold" settings? */
		cccr_value.bits.complement = 0;	/* FIXME: What do we do with "threshold" settings? */
		cccr_value.bits.threshold = 0;	/* FIXME: What do we do with "threshold" settings? */
		cccr_value.bits.force_ovf = 0;	/* FIXME: Do we want to allow "forcing" overflow
										 *        interrupts on all counter increments? */
		cccr_value.bits.ovf_pmi_t0 = 0;
		cccr_value.bits.ovf_pmi_t1 = 0;	/* PMI taken care of by kernel typically */
		cccr_value.bits.reserved2 = 0;
		cccr_value.bits.cascade = 0;	/* FIXME: How do we handle "cascading" counters? */
		cccr_value.bits.overflow = 0;

		/* these flags are always zero, from what I can tell... */
		bits->pebs_enable = 0;	/* flag for PEBS counting */
		bits->pebs_matrix_vert = 0;	/* flag for PEBS_MATRIX_VERT, whatever that is */

		/* ...unless the event is replay_event */
		if ( !strcmp( pentium4_events[event].name, "replay_event" ) ) {
			escr_value.bits.event_mask = event_mask & P4_REPLAY_REAL_MASK;
			num_masks = prepare_umask( umask, unit_masks );
			for ( i = 0; i < num_masks; i++ ) {
				replay_mask = unit_masks[i];
				if ( replay_mask > 1 && replay_mask < 11 ) {	/* process each valid mask we find */
					bits->pebs_enable |= p4_replay_regs[replay_mask].enb;
					bits->pebs_matrix_vert |=
						p4_replay_regs[replay_mask].mat_vert;
				}
			}
		}

		/* store the escr and cccr values */
		bits->event = escr_value.val;
		bits->cccr = cccr_value.val;
		bits->ireset = 0;	 /* I don't really know what this does */
		SUBDBG( "escr: 0x%lx; cccr:  0x%lx\n", escr_value.val, cccr_value.val );
	} else {
		unsigned int event, umask;
		int ret, code;

		if ( _pfm_decode_native_event( EventCode, &event, &umask ) != PAPI_OK )
			return ( PAPI_ENOEVNT );

		if ( ( ret =
			   _pfm_get_counter_info( event, &bits->selector,
									  &code ) ) != PAPI_OK )
			return ( ret );

		bits->counter_cmd =
			( int ) ( code | ( ( _pfm_convert_umask( event, umask ) ) << 8 ) );

		SUBDBG( "selector: 0x%x\n", bits->selector );
		SUBDBG( "event: 0x%x; umask: 0x%x; code: 0x%x; cmd: 0x%x\n", event,
				umask, code, ( ( hwd_register_t * ) bits )->counter_cmd );
	}
	return ( PAPI_OK );
}

#endif 



/* used by linux-timer.c for ia64 */
int _perfmon2_pfm_pmu_type = -1;


int
_papi_libpfm_init(papi_vector_t *MY_VECTOR) {

   int retval;
   unsigned int ncnt;
   unsigned int version;
   char pmu_name[PAPI_MIN_STR_LEN];


   /* The following checks the version of the PFM library
      against the version PAPI linked to... */
   SUBDBG( "pfm_initialize()\n" );
   if ( ( retval = pfm_initialize(  ) ) != PFMLIB_SUCCESS ) {
      PAPIERROR( "pfm_initialize(): %s", pfm_strerror( retval ) );
      return PAPI_ESBSTR;
   }

   /* Get the libpfm3 version */
   SUBDBG( "pfm_get_version(%p)\n", &version );
   if ( pfm_get_version( &version ) != PFMLIB_SUCCESS ) {
      PAPIERROR( "pfm_get_version(%p): %s", version, pfm_strerror( retval ) );
      return PAPI_ESBSTR;
   }

   /* Set the version */
   sprintf( MY_VECTOR->cmp_info.support_version, "%d.%d",
	    PFM_VERSION_MAJOR( version ), PFM_VERSION_MINOR( version ) );

   /* Complain if the compiled-against version doesn't match current version */
   if ( PFM_VERSION_MAJOR( version ) != PFM_VERSION_MAJOR( PFMLIB_VERSION ) ) {
      PAPIERROR( "Version mismatch of libpfm: compiled %x vs. installed %x\n",
				   PFM_VERSION_MAJOR( PFMLIB_VERSION ),
				   PFM_VERSION_MAJOR( version ) );
      return PAPI_ESBSTR;
   }

   /* Always initialize globals dynamically to handle forks properly. */

   _perfmon2_pfm_pmu_type = -1;

   /* Opened once for all threads. */
   SUBDBG( "pfm_get_pmu_type(%p)\n", &_perfmon2_pfm_pmu_type );
   if ( pfm_get_pmu_type( &_perfmon2_pfm_pmu_type ) != PFMLIB_SUCCESS ) {
      PAPIERROR( "pfm_get_pmu_type(%p): %s", _perfmon2_pfm_pmu_type,
				   pfm_strerror( retval ) );
      return PAPI_ESBSTR;
   }

   pmu_name[0] = '\0';
   if ( pfm_get_pmu_name( pmu_name, PAPI_MIN_STR_LEN ) != PFMLIB_SUCCESS ) {
      PAPIERROR( "pfm_get_pmu_name(%p,%d): %s", pmu_name, PAPI_MIN_STR_LEN,
				   pfm_strerror( retval ) );
      return PAPI_ESBSTR;
   }
   SUBDBG( "PMU is a %s, type %d\n", pmu_name, _perfmon2_pfm_pmu_type );

   /* Setup presets */
   retval = _papi_libpfm_setup_presets( pmu_name, _perfmon2_pfm_pmu_type );
   if ( retval )
      return retval;

   /* Fill in cmp_info */

   SUBDBG( "pfm_get_num_events(%p)\n", &ncnt );
   if ( ( retval = pfm_get_num_events( &ncnt ) ) != PFMLIB_SUCCESS ) {
      PAPIERROR( "pfm_get_num_events(%p): %s\n", &ncnt,
				   pfm_strerror( retval ) );
      return PAPI_ESBSTR;
   }
   SUBDBG( "pfm_get_num_events: %d\n", ncnt );
   MY_VECTOR->cmp_info.num_native_events = ncnt;

   pfm_get_num_counters( ( unsigned int * ) &MY_VECTOR->cmp_info.num_cntrs );
   SUBDBG( "pfm_get_num_counters: %d\n", MY_VECTOR->cmp_info.num_cntrs );


   MY_VECTOR->cmp_info.num_mpx_cntrs = PFMLIB_MAX_PMDS;

   return PAPI_OK;
}

long long generate_p4_event(long long escr,
			    long long cccr,
			    long long escr_addr) {
		   
/*
 * RAW events specification
 *
 * Bits                Meaning
 * -----       -------
 *  0-6        Metric value from enum P4_PEBS_METRIC (if needed)
 *  7-11       Reserved, set to 0
 * 12-31       Bits 12-31 of CCCR register (Intel SDM Vol 3)
 * 32-56       Bits  0-24 of ESCR register (Intel SDM Vol 3)
 * 57-62       Event key from enum P4_EVENTS
 *    63       Reserved, set to 0
 */
		   
 enum P4_EVENTS {
      P4_EVENT_TC_DELIVER_MODE,
      P4_EVENT_BPU_FETCH_REQUEST,
      P4_EVENT_ITLB_REFERENCE,
      P4_EVENT_MEMORY_CANCEL,
      P4_EVENT_MEMORY_COMPLETE,
      P4_EVENT_LOAD_PORT_REPLAY,
      P4_EVENT_STORE_PORT_REPLAY,
      P4_EVENT_MOB_LOAD_REPLAY,
      P4_EVENT_PAGE_WALK_TYPE,
      P4_EVENT_BSQ_CACHE_REFERENCE,
      P4_EVENT_IOQ_ALLOCATION,
      P4_EVENT_IOQ_ACTIVE_ENTRIES,
      P4_EVENT_FSB_DATA_ACTIVITY,
      P4_EVENT_BSQ_ALLOCATION,
      P4_EVENT_BSQ_ACTIVE_ENTRIES,
      P4_EVENT_SSE_INPUT_ASSIST,
      P4_EVENT_PACKED_SP_UOP,
      P4_EVENT_PACKED_DP_UOP,
      P4_EVENT_SCALAR_SP_UOP,
      P4_EVENT_SCALAR_DP_UOP,
      P4_EVENT_64BIT_MMX_UOP,
      P4_EVENT_128BIT_MMX_UOP,
      P4_EVENT_X87_FP_UOP,
      P4_EVENT_TC_MISC,
      P4_EVENT_GLOBAL_POWER_EVENTS,
      P4_EVENT_TC_MS_XFER,
      P4_EVENT_UOP_QUEUE_WRITES,
      P4_EVENT_RETIRED_MISPRED_BRANCH_TYPE,
      P4_EVENT_RETIRED_BRANCH_TYPE,
      P4_EVENT_RESOURCE_STALL,
      P4_EVENT_WC_BUFFER,
      P4_EVENT_B2B_CYCLES,
      P4_EVENT_BNR,
      P4_EVENT_SNOOP,
      P4_EVENT_RESPONSE,
      P4_EVENT_FRONT_END_EVENT,
      P4_EVENT_EXECUTION_EVENT,
      P4_EVENT_REPLAY_EVENT,
      P4_EVENT_INSTR_RETIRED,
      P4_EVENT_UOPS_RETIRED,
      P4_EVENT_UOP_TYPE,
      P4_EVENT_BRANCH_RETIRED,
      P4_EVENT_MISPRED_BRANCH_RETIRED,
      P4_EVENT_X87_ASSIST,
      P4_EVENT_MACHINE_CLEAR,
      P4_EVENT_INSTR_COMPLETED,
   };
		   
		  		   
    int eventsel=(escr>>25)&0x3f;
    int cccrsel=(cccr>>13)&0x7;
    int event_key=-1;
    long long pe_event;
		   
    switch(eventsel) {
       case 0x1: if (cccrsel==1) {
		    if (escr_addr>0x3c8) {
		       // tc_escr0,1 0x3c4 
		       event_key=P4_EVENT_TC_DELIVER_MODE; 
		    }
		    else {
		       // alf_escr0, 0x3ca    
		       event_key=P4_EVENT_RESOURCE_STALL;
		    }
		 }
		 if (cccrsel==4) {	    
		    if (escr_addr<0x3af) {
		       // pmh_escr0,1 0x3ac
		       event_key=P4_EVENT_PAGE_WALK_TYPE;
		    }
		    else {
		       // cru_escr0, 3b8 cccr=04
		       event_key=P4_EVENT_UOPS_RETIRED;
		    }
		 }
		 break;
		    case 0x2: if (cccrsel==5) {
		                 if (escr_addr<0x3a8) { 
		                    // MSR_DAC_ESCR0 / MSR_DAC_ESCR1
		                    event_key=P4_EVENT_MEMORY_CANCEL; 
				 } else {
				   //MSR_CRU_ESCR2, MSR_CRU_ESCR3
				   event_key=P4_EVENT_MACHINE_CLEAR;
				 }
			      } else if (cccrsel==1) {
		      	         event_key=P4_EVENT_64BIT_MMX_UOP;
			      } else if (cccrsel==4) {
			         event_key=P4_EVENT_INSTR_RETIRED;
			      } else if (cccrsel==2) {
			         event_key=P4_EVENT_UOP_TYPE;
			      }
			      break;
		    case 0x3: if (cccrsel==0) {
		                 event_key=P4_EVENT_BPU_FETCH_REQUEST;
		              }
                              if (cccrsel==2) {
		                 event_key=P4_EVENT_MOB_LOAD_REPLAY;
			      }
		              if (cccrsel==6) {
			         event_key=P4_EVENT_IOQ_ALLOCATION;
			      }
		              if (cccrsel==4) {
			         event_key=P4_EVENT_MISPRED_BRANCH_RETIRED;
		              }
			      if (cccrsel==5) { 
				 event_key=P4_EVENT_X87_ASSIST;
		              }
			      break;
		    case 0x4: if (cccrsel==2) {
		                 if (escr_addr<0x3b0) {
				    // saat, 0x3ae 
		                    event_key=P4_EVENT_LOAD_PORT_REPLAY; 
		                 }
		                 else {
				    // tbpu 0x3c2
		                    event_key=P4_EVENT_RETIRED_BRANCH_TYPE;
				 }
		              }
		              if (cccrsel==1) {
		      	         event_key=P4_EVENT_X87_FP_UOP;
		              }
			      if (cccrsel==3) {
			         event_key=P4_EVENT_RESPONSE;
		              }
			      break;
                    case 0x5: if (cccrsel==2) {
		                 if (escr_addr<0x3b0) {
		                    // saat, 0x3ae 
		                    event_key=P4_EVENT_STORE_PORT_REPLAY;
				 }
		                 else {
		                    // tbpu, 0x3c2
		                    event_key=P4_EVENT_RETIRED_MISPRED_BRANCH_TYPE;
				 }
		              }
		              if (cccrsel==7) {
		      	         event_key=P4_EVENT_BSQ_ALLOCATION;
		              }
		              if (cccrsel==0) {
			         event_key=P4_EVENT_TC_MS_XFER;
		              }
			      if (cccrsel==5) {
			         event_key=P4_EVENT_WC_BUFFER;
		              }
			      break;
		    case 0x6: if (cccrsel==7) {
		                 event_key=P4_EVENT_BSQ_ACTIVE_ENTRIES; 
		              }
		              if (cccrsel==1) {
		      	         event_key=P4_EVENT_TC_MISC;
			      }
			      if (cccrsel==3) {
				 event_key=P4_EVENT_SNOOP;
			      }
		              if (cccrsel==5) {
			         event_key=P4_EVENT_BRANCH_RETIRED;
			      }
			      break;
		    case 0x7: event_key=P4_EVENT_INSTR_COMPLETED; break;
		    case 0x8: if (cccrsel==2) {
		                 event_key=P4_EVENT_MEMORY_COMPLETE; 
		              }
		      	      if (cccrsel==1) {
				 event_key=P4_EVENT_PACKED_SP_UOP;
			      }
			      if (cccrsel==3) {
				 event_key=P4_EVENT_BNR;
		              }
			      if (cccrsel==5) {
				 event_key=P4_EVENT_FRONT_END_EVENT;
		              }
			      break;
                    case 0x9: if (cccrsel==0) {
		                 event_key=P4_EVENT_UOP_QUEUE_WRITES; 
		              }
		      	      if (cccrsel==5) {
				 event_key=P4_EVENT_REPLAY_EVENT;
			      }
			      break;
                    case 0xa: event_key=P4_EVENT_SCALAR_SP_UOP; break;
                    case 0xc: if (cccrsel==7) {
		                 event_key=P4_EVENT_BSQ_CACHE_REFERENCE; 
		              }
		              if (cccrsel==1) {
		      	         event_key=P4_EVENT_PACKED_DP_UOP;
			      }
			      if (cccrsel==5) {
				 event_key=P4_EVENT_EXECUTION_EVENT;
			      }
			      break;
		    case 0xe: event_key=P4_EVENT_SCALAR_DP_UOP; break;
		    case 0x13: event_key=P4_EVENT_GLOBAL_POWER_EVENTS; break;
                    case 0x16: event_key=P4_EVENT_B2B_CYCLES; break;
		    case 0x17: event_key=P4_EVENT_FSB_DATA_ACTIVITY; break;
		    case 0x18: event_key=P4_EVENT_ITLB_REFERENCE; break;
                    case 0x1a: if (cccrsel==6) {
		                  event_key=P4_EVENT_IOQ_ACTIVE_ENTRIES; 
		               }
		               if (cccrsel==1) {
			          event_key=P4_EVENT_128BIT_MMX_UOP;
		  }
		  break;
       case 0x34: event_key= P4_EVENT_SSE_INPUT_ASSIST; break;
    }
		   
    pe_event=(escr&0x1ffffff)<<32;
    pe_event|=(cccr&0xfffff000);		    
    pe_event|=(((long long)(event_key))<<57);
   
    return pe_event;
}

int
_papi_libpfm_setup_counters( struct perf_event_attr *attr, 
			   hwd_register_t *ni_bits ) {

  int ret,pe_event;

    /*
     * We need an event code that is common across all counters.
     * The implementation is required to know how to translate the supplied
     * code to whichever counter it ends up on.
     */

#if defined(__powerpc__)
    int code;
    ret = pfm_get_event_code_counter( ( ( pfm_register_t * ) ni_bits )->event, 0, &code );
    if ( ret ) {
       /* Unrecognized code, but should never happen */
       return PAPI_EBUG;
    }
    pe_event = code;
    SUBDBG( "Stuffing native event index (code 0x%x, raw code 0x%x) into events array.\n",
				  ( ( pfm_register_t * ) ni_bits )->event, code );
#else

   pfmlib_input_param_t inp;
   pfmlib_output_param_t outp;

   memset( &inp, 0, sizeof ( inp ) );
   memset( &outp, 0, sizeof ( outp ) );
   inp.pfp_event_count = 1;
   inp.pfp_dfl_plm = PAPI_DOM_USER;
   pfm_regmask_set( &inp.pfp_unavail_pmcs, 16 );	// mark fixed counters as unavailable

    inp.pfp_events[0] = *( ( pfm_register_t * ) ni_bits );
    ret = pfm_dispatch_events( &inp, NULL, &outp, NULL );
    if (ret != PFMLIB_SUCCESS) {
       SUBDBG( "Error: pfm_dispatch_events returned: %d\n", ret);
       return PAPI_ESBSTR;
    }
		   	
       /* Special case p4 */
    if (( _papi_hwi_system_info.hw_info.vendor == PAPI_VENDOR_INTEL ) && 
        ( _papi_hwi_system_info.hw_info.cpuid_family == 15)) {

	pe_event=generate_p4_event( outp.pfp_pmcs[0].reg_value, /* escr */  
		                    outp.pfp_pmcs[1].reg_value, /* cccr */
		                    outp.pfp_pmcs[0].reg_addr); /* escr_addr */
    }
    else {
        pe_event = outp.pfp_pmcs[0].reg_value;   
    }
    SUBDBG( "pe_event: 0x%llx\n", outp.pfp_pmcs[0].reg_value );
#endif

    attr->config=pe_event;

    /* for libpfm3 we currently only handle RAW type */
    attr->type=PERF_TYPE_RAW;

    return PAPI_OK;
}

int 
_papi_libpfm_shutdown(void) {

  SUBDBG("shutdown\n");

  return PAPI_OK;
}


