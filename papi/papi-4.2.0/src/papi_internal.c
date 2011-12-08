/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/

/* 
* File:    papi_internal.c
* CVS:     $Id: papi_internal.c,v 1.171 2011/10/26 15:48:48 vweaver1 Exp $
* Author:  Philip Mucci
*          mucci@cs.utk.edu
* Mods:    dan terpstra
*          terpstra@cs.utk.edu
* Mods:    Min Zhou
*          min@cs.utk.edu
* Mods:    Kevin London
*	   london@cs.utk.edu
* Mods:    Per Ekman
*          pek@pdc.kth.se
* Mods:    Haihang You
*          you@cs.utk.edu
* Mods:    Maynard Johnson
*          maynardj@us.ibm.com
* Mods:    Brian Sheely
*          bsheely@eecs.utk.edu
* Mods:    <Gary Mohr>
*          <gary.mohr@bull.com>
* Mods:    <your name here>
*          <your email address>
*/

#include "papi.h"
#include "papi_internal.h"
#include "papi_memory.h"
#ifdef USER_EVENTS
#include "papi_user_events.h"
#endif
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

static int default_debug_handler( int errorCode );
static long long handle_derived( EventInfo_t * evi, long long *from );

#ifdef NO_CPU_COUNTERS
extern void set_freq(  );
#endif

/* Defined in papi_data.c */
extern hwi_presets_t _papi_hwi_presets;

/* Machine dependent info structure */
extern papi_mdi_t _papi_hwi_system_info;

int _papi_hwi_error_level = PAPI_QUIET;
PAPI_debug_handler_t _papi_hwi_debug_handler = default_debug_handler;

/* Utility functions */

void
PAPIERROR( char *format, ... )
{
	va_list args;
	if ( ( _papi_hwi_error_level != PAPI_QUIET ) ||
		 ( getenv( "PAPI_VERBOSE" ) ) ) {
		va_start( args, format );
		fprintf( stderr, "PAPI Error: " );
		vfprintf( stderr, format, args );
		fprintf( stderr, ".\n" );
		va_end( args );
	}
}

static int
default_debug_handler( int errorCode )
{
	char str[PAPI_HUGE_STR_LEN];

	if ( errorCode == PAPI_OK )
		return ( errorCode );
	if ( ( errorCode > 0 ) || ( -errorCode > PAPI_NUM_ERRORS ) ) {
		PAPIERROR( "%s %d,%s,Bug! Unknown error code", PAPI_ERROR_CODE_str,
				   errorCode, "" );
		return ( PAPI_EBUG );
	}

	switch ( _papi_hwi_error_level ) {
	case PAPI_VERB_ECONT:
	case PAPI_VERB_ESTOP:
		/* gcc 2.96 bug fix, do not change */
		/* fprintf(stderr,"%s %d: %s: %s\n",PAPI_ERROR_CODE_str,errorCode,_papi_hwi_err[-errorCode].name,_papi_hwi_err[-errorCode].descr); */

		sprintf( str, "%s %d,%s,%s", PAPI_ERROR_CODE_str, errorCode,
				 _papi_hwi_err[-errorCode].name,
				 _papi_hwi_err[-errorCode].descr );
		if ( errorCode == PAPI_ESYS )
			sprintf( str + strlen( str ), ": %s", strerror( errno ) );

		PAPIERROR( str );

		if ( _papi_hwi_error_level == PAPI_VERB_ESTOP )
			abort(  );		 /* patch provided by will cohen of redhat */
		else
			return errorCode;
		break;

	case PAPI_QUIET:
	default:
		return errorCode;
	}
	return ( PAPI_EBUG );	 /* Never get here */
}

static int
allocate_eventset_map( DynamicArray_t * map )
{
	/* Allocate and clear the Dynamic Array structure */
	if ( map->dataSlotArray != NULL )
		papi_free( map->dataSlotArray );
	memset( map, 0x00, sizeof ( DynamicArray_t ) );

	/* Allocate space for the EventSetInfo_t pointers */

	map->dataSlotArray =
		( EventSetInfo_t ** ) papi_malloc( PAPI_INIT_SLOTS *
										   sizeof ( EventSetInfo_t * ) );
	if ( map->dataSlotArray == NULL ) {
		return ( PAPI_ENOMEM );
	}
	memset( map->dataSlotArray, 0x00,
			PAPI_INIT_SLOTS * sizeof ( EventSetInfo_t * ) );
	map->totalSlots = PAPI_INIT_SLOTS;
	map->availSlots = PAPI_INIT_SLOTS;
	map->fullSlots = 0;

	return ( PAPI_OK );
}

static void
free_eventset_map( DynamicArray_t * map )
{
	papi_free( map->dataSlotArray );
	memset( map, 0x00, sizeof ( DynamicArray_t ) );
}

static int
expand_dynamic_array( DynamicArray_t * DA )
{
	int number;
	EventSetInfo_t **n;

	/*realloc existing PAPI_EVENTSET_MAP.dataSlotArray */

	number = DA->totalSlots * 2;
	n = ( EventSetInfo_t ** ) papi_realloc( DA->dataSlotArray,
											( size_t ) number *
											sizeof ( EventSetInfo_t * ) );
	if ( n == NULL )
		return ( PAPI_ENOMEM );

	/* Need to assign this value, what if realloc moved it? */

	DA->dataSlotArray = n;

	memset( DA->dataSlotArray + DA->totalSlots, 0x00,
			( size_t ) DA->totalSlots * sizeof ( EventSetInfo_t * ) );

	DA->totalSlots = number;
	DA->availSlots = number - DA->fullSlots;

	return ( PAPI_OK );
}

/*========================================================================*/
/* This function allocates space for one EventSetInfo_t structure and for */
/* all of the pointers in this structure.  If any malloc in this function */
/* fails, all memory malloced to the point of failure is freed, and NULL  */
/* is returned.  Upon success, a pointer to the EventSetInfo_t data       */
/* structure is returned.                                                 */
/*========================================================================*/

static int
EventInfoArrayLength( const EventSetInfo_t * ESI )
{
	if ( ESI->state & PAPI_MULTIPLEXING )
		return ( _papi_hwd[ESI->CmpIdx]->cmp_info.num_mpx_cntrs );
	else
		return ( _papi_hwd[ESI->CmpIdx]->cmp_info.num_cntrs );
}

static void
initialize_EventInfoArray( EventSetInfo_t * ESI )
{
	int i, j, limit;
	EventInfo_t tmp;

	limit = _papi_hwd[ESI->CmpIdx]->cmp_info.num_mpx_cntrs;

	/* This is an optimization */

	memset( &tmp, 0x0, sizeof ( tmp ) );
	tmp.event_code = ( unsigned int ) PAPI_NULL;
	tmp.ops = NULL;
	tmp.derived = NOT_DERIVED;
	for ( j = 0; j < MAX_COUNTER_TERMS; j++ )
		tmp.pos[j] = -1;

	for ( i = 0; i < limit; i++ ) {
		memcpy( &ESI->EventInfoArray[i], &tmp, sizeof ( tmp ) );
	}
}


static void
initialize_NativeInfoArray( EventSetInfo_t * ESI )
{
	int i;
	/* xxxx should these arrays be num_mpx_cntrs or num_cntrs in size?? */
	int max_counters;
	int sz;
	char *ptr;

	max_counters = _papi_hwd[ESI->CmpIdx]->cmp_info.num_mpx_cntrs;
	sz = _papi_hwd[ESI->CmpIdx]->size.reg_value;

	/* ugh this is ugly.  Why don't we have a proper date type here? */
        /* NativeInfoArray is allocated in _papi_hwi_assign_eventset     */
        /*   as an array of NativeInfo_t, with an array of hwd_register_t */
        /*   just tacked onto the end.  Then we point to them.  Wouldn't it */
        /*   be better to have the hwd_register_t values in-line?  vmw    */
	ptr =
		( ( ( char * ) ESI->NativeInfoArray ) +
		  ( ( size_t ) max_counters * sizeof ( NativeInfo_t ) ) );

	for ( i = 0; i < max_counters; i++ ) {
		ESI->NativeInfoArray[i].ni_event = -1;
		ESI->NativeInfoArray[i].ni_position = -1;
		ESI->NativeInfoArray[i].ni_owners = 0;
		ESI->NativeInfoArray[i].ni_bits = ( hwd_register_t * ) ptr;
		ptr += sz;
	}
	ESI->NativeCount = 0;
}

static int
create_EventSet( EventSetInfo_t ** here )
{
	EventSetInfo_t *ESI;

	ESI = ( EventSetInfo_t * ) papi_malloc( sizeof ( EventSetInfo_t ) );
	if ( ESI == NULL )
		return PAPI_ENOMEM;
	memset( ESI, 0x00, sizeof ( EventSetInfo_t ) );

	*here = ESI;
	return PAPI_OK;
}

int
_papi_hwi_assign_eventset( EventSetInfo_t * ESI, int cidx )
{
        int retval;
	size_t max_counters;
	char *ptr;

	/* Assigned at create time */

	ESI->domain.domain = _papi_hwd[cidx]->cmp_info.default_domain;
	ESI->granularity.granularity =
		_papi_hwd[cidx]->cmp_info.default_granularity;
	ESI->CmpIdx = cidx;

	max_counters = ( size_t ) _papi_hwd[cidx]->cmp_info.num_mpx_cntrs;
	ESI->ctl_state =
		( hwd_control_state_t * ) papi_malloc( ( size_t ) _papi_hwd[cidx]->size.
											   control_state );
	ESI->sw_stop =
		( long long * ) papi_malloc( ( size_t ) max_counters *
									 sizeof ( long long ) );
	ESI->hw_start =
		( long long * ) papi_malloc( ( size_t ) max_counters *
									 sizeof ( long long ) );
	ESI->EventInfoArray =
		( EventInfo_t * ) papi_malloc( ( size_t ) max_counters *
									   sizeof ( EventInfo_t ) );
/* allocate room for the native events and for the component-private register structures */
/* xxxx should these arrays be num_mpx_cntrs or num_cntrs in size?? */
/* ugh is there a cleaner way to allocate this?  vmw */
	ESI->NativeInfoArray = ( NativeInfo_t * ) 
             papi_malloc( ( size_t ) max_counters * sizeof ( NativeInfo_t ) +
			  ( size_t ) max_counters * ( size_t ) _papi_hwd[cidx]->size.reg_value );

	/* NOTE: the next two malloc allocate blocks of memory that are later parcelled into overflow and profile arrays */
	ESI->overflow.deadline = ( long long * )
		papi_malloc( ( sizeof ( long long ) +
					   sizeof ( int ) * 3 ) * ( size_t ) max_counters );
	ESI->profile.prof = ( PAPI_sprofil_t ** )
		papi_malloc( ( sizeof ( PAPI_sprofil_t * ) * ( size_t ) max_counters +
					   ( size_t ) max_counters * sizeof ( int ) * 4 ) );

	if ( ( ESI->ctl_state == NULL ) ||
		 ( ESI->sw_stop == NULL ) || ( ESI->hw_start == NULL ) ||
		 ( ESI->NativeInfoArray == NULL ) || ( ESI->EventInfoArray == NULL ) ||
		 ( ESI->profile.prof == NULL ) || ( ESI->overflow.deadline == NULL ) ) {
		if ( ESI->sw_stop )
			papi_free( ESI->sw_stop );
		if ( ESI->hw_start )
			papi_free( ESI->hw_start );
		if ( ESI->EventInfoArray )
			papi_free( ESI->EventInfoArray );
		if ( ESI->NativeInfoArray )
			papi_free( ESI->NativeInfoArray );
		if ( ESI->ctl_state )
			papi_free( ESI->ctl_state );
		if ( ESI->overflow.deadline )
			papi_free( ESI->overflow.deadline );
		if ( ESI->profile.prof )
			papi_free( ESI->profile.prof );
		papi_free( ESI );
		return ( PAPI_ENOMEM );
	}
	memset( ESI->sw_stop, 0x00, max_counters * sizeof ( long long ) );
	memset( ESI->hw_start, 0x00, max_counters * sizeof ( long long ) );
	memset( ESI->ctl_state, 0x00,
			( size_t ) _papi_hwd[cidx]->size.control_state );

	/* Carve up the overflow block into separate arrays */
	ptr = ( char * ) ESI->overflow.deadline;
	ptr += sizeof ( long long ) * max_counters;
	ESI->overflow.threshold = ( int * ) ptr;
	ptr += sizeof ( int ) * max_counters;
	ESI->overflow.EventIndex = ( int * ) ptr;
	ptr += sizeof ( int ) * max_counters;
	ESI->overflow.EventCode = ( int * ) ptr;

	/* Carve up the profile block into separate arrays */
	ptr =
		( char * ) ESI->profile.prof +
		( sizeof ( PAPI_sprofil_t * ) * max_counters );
	ESI->profile.count = ( int * ) ptr;
	ptr += sizeof ( int ) * max_counters;
	ESI->profile.threshold = ( int * ) ptr;
	ptr += sizeof ( int ) * max_counters;
	ESI->profile.EventIndex = ( int * ) ptr;
	ptr += sizeof ( int ) * max_counters;
	ESI->profile.EventCode = ( int * ) ptr;

	initialize_EventInfoArray( ESI );
	initialize_NativeInfoArray( ESI );

	ESI->state = PAPI_STOPPED;

	/* these used to be init_config */
	retval = _papi_hwd[cidx]->init_control_state( ESI->ctl_state );	
	retval |= _papi_hwd[cidx]->set_domain( ESI->ctl_state, ESI->domain.domain);

	return retval;
}

/*========================================================================*/
/* This function should free memory for one EventSetInfo_t structure.     */
/* The argument list consists of a pointer to the EventSetInfo_t          */
/* structure, *ESI.                                                       */
/* The calling function should check  for ESI==NULL.                      */
/*========================================================================*/

void
_papi_hwi_free_EventSet( EventSetInfo_t * ESI )
{

	if ( ESI->EventInfoArray )
		papi_free( ESI->EventInfoArray );
	if ( ESI->NativeInfoArray )
		papi_free( ESI->NativeInfoArray );
	if ( ESI->overflow.deadline )
		papi_free( ESI->overflow.deadline );
	if ( ESI->profile.prof )
		papi_free( ESI->profile.prof );
	if ( ESI->ctl_state )
		papi_free( ESI->ctl_state );
	if ( ESI->sw_stop )
		papi_free( ESI->sw_stop );
	if ( ESI->hw_start )
		papi_free( ESI->hw_start );
	if ( ( ESI->state & PAPI_MULTIPLEXING ) && ESI->multiplex.mpx_evset )
		papi_free( ESI->multiplex.mpx_evset );
	if ( ( ESI->state & PAPI_CPU_ATTACH ) && ESI->CpuInfo )
		_papi_hwi_shutdown_cpu( ESI->CpuInfo );

#ifdef DEBUG
	memset( ESI, 0x00, sizeof ( EventSetInfo_t ) );
#endif
	papi_free( ESI );

}

static int
add_EventSet( EventSetInfo_t * ESI, ThreadInfo_t * master )
{
	DynamicArray_t *map = &_papi_hwi_system_info.global_eventset_map;
	int i, errorCode;

	_papi_hwi_lock( INTERNAL_LOCK );

	if ( map->availSlots == 0 ) {
		errorCode = expand_dynamic_array( map );
		if ( errorCode < PAPI_OK ) {
			_papi_hwi_unlock( INTERNAL_LOCK );
			return ( errorCode );
		}
	}

	i = 0;
	for ( i = 0; i < map->totalSlots; i++ ) {
		if ( map->dataSlotArray[i] == NULL ) {
			ESI->master = master;
			ESI->EventSetIndex = i;
			map->fullSlots++;
			map->availSlots--;
			map->dataSlotArray[i] = ESI;
			_papi_hwi_unlock( INTERNAL_LOCK );
			return ( PAPI_OK );
		}
	}

	_papi_hwi_unlock( INTERNAL_LOCK );
	return ( PAPI_EBUG );
}

int
_papi_hwi_create_eventset( int *EventSet, ThreadInfo_t * handle )
{
	EventSetInfo_t *ESI;
	int retval;

	/* Is the EventSet already in existence? */

	if ( ( EventSet == NULL ) || ( handle == NULL ) )
		return PAPI_EINVAL;

	if ( *EventSet != PAPI_NULL )
		return PAPI_EINVAL;

	/* Well, then allocate a new one. Use n to keep track of a NEW EventSet */

	retval = create_EventSet( &ESI );
	if ( retval != PAPI_OK )
		return retval;

	ESI->CmpIdx = -1;		 /* when eventset is created, it is not decided yet which component it belongs to, until first event is added */
	ESI->state = PAPI_STOPPED;

	/* Add it to the global table */

	retval = add_EventSet( ESI, handle );
	if ( retval < PAPI_OK ) {
		_papi_hwi_free_EventSet( ESI );
		return retval ;
	}

	*EventSet = ESI->EventSetIndex;

	INTDBG( "(%p,%p): new EventSet in slot %d\n",
			( void * ) EventSet, handle, *EventSet );

	return retval;
}

/* This function returns the index of the the next free slot
   in the EventInfoArray. If EventCode is already in the list,
   it returns PAPI_ECNFLCT. */

static int
get_free_EventCodeIndex( const EventSetInfo_t * ESI, unsigned int EventCode )
{
	int k;
	int lowslot = PAPI_ECNFLCT;
	int limit = EventInfoArrayLength( ESI );

	/* Check for duplicate events and get the lowest empty slot */

	for ( k = 0; k < limit; k++ ) {
		if ( ESI->EventInfoArray[k].event_code == EventCode )
			return ( PAPI_ECNFLCT );
		/*if ((ESI->EventInfoArray[k].event_code == PAPI_NULL) && (lowslot == PAPI_ECNFLCT)) */
		if ( ESI->EventInfoArray[k].event_code == ( unsigned int ) PAPI_NULL ) {
			lowslot = k;
			break;
		}
	}
	return ( lowslot );
}

/* This function returns the index of the EventCode or error */
/* Index to what? The index to everything stored EventCode in the */
/* EventSet. */

int
_papi_hwi_lookup_EventCodeIndex( const EventSetInfo_t * ESI,
				 unsigned int EventCode )
{
	int i;
	int limit = EventInfoArrayLength( ESI );

	for ( i = 0; i < limit; i++ ) {
	   if ( ESI->EventInfoArray[i].event_code == EventCode ) {
	      return i;
	   }
	}

	return PAPI_EINVAL;
}

/* This function only removes empty EventSets */

int
_papi_hwi_remove_EventSet( EventSetInfo_t * ESI )
{
	DynamicArray_t *map = &_papi_hwi_system_info.global_eventset_map;
	int i;

	i = ESI->EventSetIndex;

	_papi_hwi_free_EventSet( ESI );

	/* do bookkeeping for PAPI_EVENTSET_MAP */

	_papi_hwi_lock( INTERNAL_LOCK );

	map->dataSlotArray[i] = NULL;
	map->availSlots++;
	map->fullSlots--;

	_papi_hwi_unlock( INTERNAL_LOCK );

	return PAPI_OK;
}


/* this function try to find out whether native event has already been mapped. 
     Success, return hwd_native_t array index
     Fail,    return -1;                                                             
*/
static int
_papi_hwi_add_native_precheck( EventSetInfo_t * ESI, int nevt )
{
	int i;
	int cidx = PAPI_COMPONENT_INDEX( nevt );

	if ( _papi_hwi_invalid_cmp( cidx ) )
		return -1;

	/* to find the native event from the native events list */
	for ( i = 0; i < ESI->NativeCount; i++ ) {
		if ( nevt == ESI->NativeInfoArray[i].ni_event ) {
			ESI->NativeInfoArray[i].ni_owners++;
			INTDBG( "found native event already mapped: 0x%x\n", nevt );
			return i;
		}
	}
	return -1;
}

/* This function goes through the events in an EventSet's EventInfoArray */
/* And maps each event (whether native or part of a preset) to           */
/* an event in the EventSets NativeInfoArray.                            */
/* The looping is mysterious. */
/* When do we need to do this? */

void
_papi_hwi_remap_event_position( EventSetInfo_t * ESI, int thisindex, int total_events )
{
    (void) thisindex;

    EventInfo_t *head;
    int i, j, k, n, preset_index = 0, nevt;

    APIDBG("Remapping %d events in EventSet %d\n",total_events,ESI->EventSetIndex);

    head = ESI->EventInfoArray;
   
    /* Ugh I don't understand this i/j business.  vmw */
    j = 0;
    for( i = 0; i < total_events; i++ ) {

       /* find the added event in EventInfoArray    */
       /* find the first event that isn't PAPI_NULL */
       while ( head[j].event_code == ( unsigned int ) PAPI_NULL ) {
          j++;
       }
	   
       /* If it's a preset */
       if ( IS_PRESET(head[j].event_code) ) {
	  preset_index = ( int ) head[j].event_code & PAPI_PRESET_AND_MASK;

	  /* walk all sub-events in the preset */
	  for( k = 0; k < PAPI_MAX_COUNTER_TERMS; k++ ) {
	     nevt = _papi_hwi_presets.data[preset_index]->native[k];
	     if ( nevt == PAPI_NULL ) {
		break;
	     }
	     for( n = 0; n < ESI->NativeCount; n++ ) {
		if ( nevt == ESI->NativeInfoArray[n].ni_event ) {
		   head[j].pos[k] = ESI->NativeInfoArray[n].ni_position;
		   break;
		}
	     }
	  }
	  /*head[j].pos[k]=-1; */
       } 
       /* It's a native event */
       else if( IS_NATIVE(head[j].event_code) ) {
	  nevt = ( int ) head[j].event_code;

	  /* Look for the new event in the NativeInfoArray */

	  for( n = 0; n < ESI->NativeCount; n++ ) {
	     if ( nevt == ESI->NativeInfoArray[n].ni_event ) {
		head[j].pos[0] = ESI->NativeInfoArray[n].ni_position;
		/*head[j].pos[1]=-1; */
		break;
	     }
	  }
       } 
#ifdef USER_EVENTS 
	   else if ( IS_USER_DEFINED(head[j].event_code) ) {
		 for ( k = 0; k < PAPI_MAX_COUNTER_TERMS; k++ ) {
		   nevt = _papi_user_events[preset_index].events[k];
		   if ( nevt == PAPI_NULL )
			 break;
		   for ( n = 0; n < ESI->NativeCount; n++ ) {
			 if ( nevt == ESI->NativeInfoArray[n].ni_event ) {
			   head[j].pos[k] = ESI->NativeInfoArray[n].ni_position;
			 }
		   }
		 }
	   }
#endif
	   /* end of if */
       j++;
    }						 /* end of for loop */
}


static int
add_native_fail_clean( EventSetInfo_t * ESI, int nevt )
{
	int i, max_counters;
	int cidx = PAPI_COMPONENT_INDEX( nevt );

	if ( _papi_hwi_invalid_cmp( cidx ) )
		return -1;

	max_counters = _papi_hwd[cidx]->cmp_info.num_mpx_cntrs;

	/* to find the native event from the native events list */
	for ( i = 0; i < max_counters; i++ ) {
		if ( nevt == ESI->NativeInfoArray[i].ni_event ) {
			ESI->NativeInfoArray[i].ni_owners--;
			/* to clean the entry in the nativeInfo array */
			if ( ESI->NativeInfoArray[i].ni_owners == 0 ) {
				ESI->NativeInfoArray[i].ni_event = 0;
				ESI->NativeInfoArray[i].ni_position = -1;
				ESI->NativeCount--;
			}
			INTDBG
				( "add_events fail, and remove added native events of the event: 0x%x\n",
				  nevt );
			return i;
		}
	}
	return -1;
}

/* since update_control_state trashes overflow settings, this puts things
   back into balance. */
static int
update_overflow( EventSetInfo_t * ESI )
{
   int i, retval = PAPI_OK;

   if ( ESI->overflow.flags & PAPI_OVERFLOW_HARDWARE ) {
      for( i = 0; i < ESI->overflow.event_counter; i++ ) {
	 retval = _papi_hwd[ESI->CmpIdx]->set_overflow( ESI,
							ESI->overflow.EventIndex[i],
							ESI->overflow.threshold[i] );
	 if ( retval != PAPI_OK ) {
	    break;
	 }
      }
   }
   return retval;
}

/* this function is called by _papi_hwi_add_event when adding native events 
nix: pointer to array of native event table indexes from the preset entry
size: number of native events to add
*/
static int
add_native_events( EventSetInfo_t * ESI, int *nevt, int size,
				   EventInfo_t * out )
{
	int nidx, i, j, remap = 0;
	int retval, retval2;
	int max_counters;
	hwd_context_t *context;

	if ( _papi_hwd[ESI->CmpIdx]->cmp_info.kernel_multiplex )
		max_counters = _papi_hwd[ESI->CmpIdx]->cmp_info.num_mpx_cntrs;
	else
		max_counters = _papi_hwd[ESI->CmpIdx]->cmp_info.num_cntrs;
	/* if the native event is already mapped, fill in */
	for ( i = 0; i < size; i++ ) {
		if ( ( nidx = _papi_hwi_add_native_precheck( ESI, nevt[i] ) ) >= 0 ) {
			out->pos[i] = ESI->NativeInfoArray[nidx].ni_position;
		} else {
			/* all counters have been used, add_native fail */
			if ( ESI->NativeCount == max_counters ) {
				/* to clean owners for previous added native events */
				for ( j = 0; j < i; j++ ) {
					if ( ( nidx = add_native_fail_clean( ESI, nevt[j] ) ) >= 0 ) {
						out->pos[j] = -1;
						continue;
					}
					INTDBG( "should not happen!\n" );
				}
				INTDBG( "counters are full!\n" );
				return -1;
			}
			/* there is an empty slot for the native event;
			   initialize the native index for the new added event */
			INTDBG( "Adding 0x%x\n", nevt[i] );
			ESI->NativeInfoArray[ESI->NativeCount].ni_event = nevt[i];
			ESI->NativeInfoArray[ESI->NativeCount].ni_owners = 1;
			ESI->NativeCount++;
			remap++;
		}
	}

	/* if remap!=0, we need reallocate counters */
	if ( remap ) {
		/* get the context we should use for this event set */
		context = _papi_hwi_get_context( ESI, NULL );
	   
		if ( _papi_hwd[ESI->CmpIdx]->allocate_registers( ESI ) ) {

			retval = _papi_hwd[ESI->CmpIdx]->update_control_state( ESI->ctl_state,
															  ESI->NativeInfoArray,
															  ESI->NativeCount,
															  context);
			if ( retval != PAPI_OK ) {
			  clean:
				for ( i = 0; i < size; i++ ) {
					if ( ( nidx = add_native_fail_clean( ESI, nevt[i] ) ) >= 0 ) {
						out->pos[i] = -1;
						continue;
					}
					INTDBG( "should not happen!\n" );
				}
				/* re-establish the control state after the previous error */
				retval2 = _papi_hwd[ESI->CmpIdx]->update_control_state( ESI->ctl_state,
																  ESI->NativeInfoArray,
																  ESI->NativeCount,
																  context);
				if ( retval2 != PAPI_OK ) {
					PAPIERROR
						( "update_control_state failed to re-establish working events!" );
					return retval2;
				}
				return ( retval );
			}
			return 1;
		} else {
			retval = -1;
			goto clean;
		}
	}
	return 0;
}


int
_papi_hwi_add_event( EventSetInfo_t * ESI, int EventCode )
{
    int i, j, thisindex, remap, retval = PAPI_OK;

    /* Sanity check that the new EventCode is from the same component */
    /* as previous events.                                            */
    
    if ( ESI->CmpIdx < 0 ) {
       if ( ( retval = _papi_hwi_assign_eventset( ESI,
					          PAPI_COMPONENT_INDEX( EventCode ) ) )
			                          != PAPI_OK )
       return retval;
    } else {
       if ( ESI->CmpIdx != PAPI_COMPONENT_INDEX( EventCode ) ) {
	  return PAPI_EINVAL;
       }
    }

    /* Make sure the event is not present and get the next free slot. */
    thisindex = get_free_EventCodeIndex( ESI, ( unsigned int ) EventCode );
    if ( thisindex < PAPI_OK ) {
       return thisindex;
    }

    APIDBG("Adding event to slot %d of EventSet %d\n",thisindex,ESI->EventSetIndex);

    /* If it is a software MPX EventSet, add it to the multiplex data structure */
    /* and this thread's multiplex list                                         */

    if ( !_papi_hwi_is_sw_multiplex( ESI ) ) {

       /* Handle preset case */
       if ( IS_PRESET(EventCode) ) {
	  int count;
	  int preset_index = EventCode & ( int ) PAPI_PRESET_AND_MASK & 
	                     ( int ) PAPI_COMPONENT_AND_MASK;

	  /* Check if it's within the valid range */
	  if ( ( preset_index < 0 ) || ( preset_index >= PAPI_MAX_PRESET_EVENTS ) ) {
	     return PAPI_EINVAL;
	  }

	  /* count the number of native events in this preset */
	  count = ( int ) _papi_hwi_presets.count[preset_index];

	  /* Check if event exists */
	  if ( !count ) {
	     return PAPI_ENOEVNT;
	  }
			
	  /* check if the native events have been used as overflow events */
	  /* this is not allowed                                          */
	  if ( ESI->state & PAPI_OVERFLOWING ) {
	     for( i = 0; i < count; i++ ) {
		for( j = 0; j < ESI->overflow.event_counter; j++ ) {
		   if ( ESI->overflow.EventCode[j] ==
			( _papi_hwi_presets.data[preset_index]->native[i] ) ) {
		      return PAPI_ECNFLCT;
		   }
		}
	     }
	  }

	  /* Try to add the preset. */

	  remap = add_native_events( ESI,
				     _papi_hwi_presets.data[preset_index]->native,
				     count, &ESI->EventInfoArray[thisindex] );
	  if ( remap < 0 ) {
	     return PAPI_ECNFLCT;
	  }
          else {
	     /* Fill in the EventCode (machine independent) information */
	     ESI->EventInfoArray[thisindex].event_code = ( unsigned int ) EventCode;
	     ESI->EventInfoArray[thisindex].derived =
					_papi_hwi_presets.data[preset_index]->derived;
	     ESI->EventInfoArray[thisindex].ops =
					_papi_hwi_presets.data[preset_index]->operation;
	     if ( remap ) {
		_papi_hwi_remap_event_position( ESI, thisindex, ESI->NumberOfEvents+1 );
	     }
	  }
       }
       /* Handle adding Native events */
       else if ( IS_NATIVE(EventCode) ) {

	  /* Check if native event exists */
	  if ( _papi_hwi_query_native_event( ( unsigned int ) EventCode ) != PAPI_OK ) {
	     return PAPI_ENOEVNT;
	  }
			
	  /* check if the native events have been used as overflow events */
	  /* This is not allowed                                          */
	  if ( ESI->state & PAPI_OVERFLOWING ) {
	     for( j = 0; j < ESI->overflow.event_counter; j++ ) {
	        if ( EventCode == ESI->overflow.EventCode[j] ) {
		   return PAPI_ECNFLCT;
		}
	     }
	  }

	  /* Try to add the native event. */

	  remap = add_native_events( ESI, &EventCode, 1,
				     &ESI->EventInfoArray[thisindex] );

	  if ( remap < 0 ) {
	     return PAPI_ECNFLCT;
	  } else {

	     /* Fill in the EventCode (machine independent) information */
	     ESI->EventInfoArray[thisindex].event_code = ( unsigned int ) EventCode;
	     if ( remap ) {
		_papi_hwi_remap_event_position( ESI, thisindex,ESI->NumberOfEvents+1 );
	     }
	  }
       } 
#ifdef USER_EVENTS
	   else if ( IS_USER_DEFINED( EventCode ) ) {
		 int count;
		 int index = EventCode & PAPI_UE_AND_MASK;

		 if ( index < 0 || index >= (int)_papi_user_events_count )
		   return ( PAPI_EINVAL );

		 count = ( int ) _papi_user_events[index].count;

		 for ( i = 0; i < count; i++ ) {
		   for ( j = 0; j < ESI->overflow.event_counter; j++ ) {
			 if ( ESI->overflow.EventCode[j] ==
				 _papi_user_events[index].events[i] ) {
			   return ( PAPI_EBUG );
			 }
		   }
		 }

		 remap = add_native_events( ESI,
			 _papi_user_events[index].events,
			 count, &ESI->EventInfoArray[thisindex] );

		 if ( remap < 0 )
		   return ( PAPI_ECNFLCT );
		 else {
		   ESI->EventInfoArray[thisindex].event_code       = (unsigned int) EventCode;
		   ESI->EventInfoArray[thisindex].derived          = DERIVED_POSTFIX;
		   ESI->EventInfoArray[thisindex].ops                      = _papi_user_events[index].operation;
		   if ( remap )
			 _papi_hwi_remap_event_position( ESI, thisindex, ESI->NumberOfEvents+1 );
		 }
	   } 
#endif
	   else {

	  /* not Native or Preset events */

	  return PAPI_EBUG;
       }
    }
    else {
		
       /* Multiplexing is special. See multiplex.c */

       retval = mpx_add_event( &ESI->multiplex.mpx_evset, EventCode,
			       ESI->domain.domain, ESI->granularity.granularity );


       if ( retval < PAPI_OK ) {
	  return retval;
       }

       /* Relevant (???) */
       ESI->EventInfoArray[thisindex].event_code = ( unsigned int ) EventCode;	
       ESI->EventInfoArray[thisindex].derived = NOT_DERIVED;

       /* event is in the EventInfoArray but not mapped to the NativeEvents */
       /* this causes issues if you try to set overflow on the event.       */
       /* in theory this wouldn't matter anyway.                            */
    }

    /* Bump the number of events */
    ESI->NumberOfEvents++;

    /* reinstate the overflows if any */
    retval=update_overflow( ESI );

    return retval;
}


int
_papi_hwi_add_pevent( EventSetInfo_t * ESI, int EventCode, void *inout )
{
	int thisindex, retval;


	if ( ESI->CmpIdx < 0 ) {
		if ( ( retval =
			   _papi_hwi_assign_eventset( ESI,
										  PAPI_COMPONENT_INDEX( EventCode ) ) )
			 != PAPI_OK )
			return retval;
	}

	/* Make sure the event is not present and get a free slot. */

	thisindex = get_free_EventCodeIndex( ESI, ( unsigned int ) EventCode );
	if ( thisindex < PAPI_OK )
		return ( thisindex );

	/* Fill in machine depending info including the EventInfoArray. */

	retval =
		_papi_hwd[ESI->CmpIdx]->add_prog_event( ESI->ctl_state,
												( unsigned int ) EventCode,
												inout,
												&ESI->
												EventInfoArray[thisindex] );
	if ( retval < PAPI_OK )
		return ( retval );

	/* Initialize everything left over. */

	/* ESI->sw_stop[thisindex]     = 0; */
	/* ESI->hw_start[thisindex]   = 0; */

	ESI->NumberOfEvents++;
	return ( retval );
}


static int
remove_native_events( EventSetInfo_t * ESI, int *nevt, int size )
{
	NativeInfo_t *native = ESI->NativeInfoArray;
	hwd_context_t *context;
	int i, j, zero = 0, retval;

	/* Remove the references to this event from the native events:
	   for all the metrics in this event,
	   compare to each native event in this event set,
	   and decrement owners if they match  */
	for ( i = 0; i < size; i++ ) {
		for ( j = 0; j < ESI->NativeCount; j++ ) {
			if ( native[j].ni_event == nevt[i] ) {
				native[j].ni_owners--;
				if ( native[j].ni_owners == 0 ) {
					zero++;
				}
				break;
			}
		}
	}

	/* Remove any native events from the array if owners dropped to zero.
	   The NativeInfoArray must be dense, with no empty slots, so if we
	   remove an element, we must compact the list */
	for ( i = 0; i < ESI->NativeCount; i++ ) {
		if ( native[i].ni_event == 0 )
			continue;

		if ( native[i].ni_owners == 0 ) {
			int copy = 0;
			int sz = _papi_hwd[ESI->CmpIdx]->size.reg_value;
			for ( j = ESI->NativeCount - 1; j > i; j-- ) {
				if ( native[j].ni_event == 0 || native[j].ni_owners == 0 )
					continue;
				else {
					/* copy j into i */
					native[i].ni_event = native[j].ni_event;
					native[i].ni_position = native[j].ni_position;
					native[i].ni_owners = native[j].ni_owners;
					/* copy opaque [j].ni_bits to [i].ni_bits */
					memcpy( native[i].ni_bits, native[j].ni_bits,
							( size_t ) sz );
					/* reset j to initialized state */
					native[j].ni_event = -1;
					native[j].ni_position = -1;
					native[j].ni_owners = 0;
					copy++;
					break;
				}
			}

			if ( copy == 0 ) {
				/* set this structure back to empty state */
				/* ni_owners is already 0 and contents of ni_bits doesn't matter */
				native[i].ni_event = -1;
				native[i].ni_position = -1;
			}
		}
	}

	/* to reset hwd_control_state values */
	ESI->NativeCount -= zero;

	/* If we removed any elements, 
	   clear the now empty slots, reinitialize the index, and update the count.
	   Then send the info down to the substrate to update the hwd control structure. */
	retval = PAPI_OK;
	if ( zero ) {
      /* get the context we should use for this event set */
      context = _papi_hwi_get_context( ESI, NULL );
		retval = _papi_hwd[ESI->CmpIdx]->update_control_state( ESI->ctl_state,
														  native, ESI->NativeCount, context);
		if ( retval == PAPI_OK )
			retval = update_overflow( ESI );
	}
	return ( retval );
}

int
_papi_hwi_remove_event( EventSetInfo_t * ESI, int EventCode )
{
	int j = 0, retval, thisindex;
	EventInfo_t *array;

	thisindex =
		_papi_hwi_lookup_EventCodeIndex( ESI, ( unsigned int ) EventCode );
	if ( thisindex < PAPI_OK )
		return ( thisindex );

	/* If it is a MPX EventSet, remove it from the multiplex data structure and
	   this threads multiplex list */

	if ( _papi_hwi_is_sw_multiplex( ESI ) ) {
		retval = mpx_remove_event( &ESI->multiplex.mpx_evset, EventCode );
		if ( retval < PAPI_OK )
			return ( retval );
	} else
		/* Remove the events hardware dependent stuff from the EventSet */
	{
		if ( IS_PRESET(EventCode) ) {
			int preset_index = EventCode & PAPI_PRESET_AND_MASK;

			/* Check if it's within the valid range */
			if ( ( preset_index < 0 ) ||
				 ( preset_index >= PAPI_MAX_PRESET_EVENTS ) )
				return ( PAPI_EINVAL );

			/* Check if event exists */
			if ( !_papi_hwi_presets.count[preset_index] )
				return ( PAPI_ENOEVNT );

			/* Remove the preset event. */
			for ( j = 0; _papi_hwi_presets.data[preset_index]->native[j] != 0;
				  j++ );
			retval =
				remove_native_events( ESI,
									  _papi_hwi_presets.data[preset_index]->
									  native, j );
			if ( retval != PAPI_OK )
				return ( retval );
		} else if ( IS_NATIVE(EventCode) ) {
			/* Check if native event exists */
			if ( _papi_hwi_query_native_event( ( unsigned int ) EventCode ) !=
				 PAPI_OK )
				return ( PAPI_ENOEVNT );

			/* Remove the native event. */
			retval = remove_native_events( ESI, &EventCode, 1 );
			if ( retval != PAPI_OK )
				return ( retval );
		} 
#ifdef USER_EVENTS
		else if ( IS_USER_DEFINED( EventCode ) ) {
		  int index = EventCode & PAPI_UE_AND_MASK;

		  if ( (index < 0) || (index >= (int)_papi_user_events_count) )
			return ( PAPI_EINVAL );

		  for( j = 0; j < PAPI_MAX_COUNTER_TERMS &&
			  _papi_user_events[index].events[j] != 0; j++ ) {
			retval = remove_native_events( ESI,
				_papi_user_events[index].events, j);

			if ( retval != PAPI_OK )
			  return ( retval );
		  }
		} 
#endif
		else
			return ( PAPI_ENOEVNT );
	}
	array = ESI->EventInfoArray;

	/* Compact the Event Info Array list if it's not the last event */
	/* clear the newly empty slot in the array */
	for ( ; thisindex < ESI->NumberOfEvents - 1; thisindex++ )
		array[thisindex] = array[thisindex + 1];


	array[thisindex].event_code = ( unsigned int ) PAPI_NULL;
	for ( j = 0; j < MAX_COUNTER_TERMS; j++ )
		array[thisindex].pos[j] = -1;
	array[thisindex].ops = NULL;
	array[thisindex].derived = NOT_DERIVED;
	ESI->NumberOfEvents--;

	return ( PAPI_OK );
}

int
_papi_hwi_read( hwd_context_t * context, EventSetInfo_t * ESI,
				long long *values )
{
	int retval;
	long long *dp = NULL;
	int i, index;

	retval = _papi_hwd[ESI->CmpIdx]->read( context, ESI->ctl_state, &dp, ESI->state );
	if ( retval != PAPI_OK )
		return ( retval );

	/* This routine distributes hardware counters to software counters in the
	   order that they were added. Note that the higher level
	   EventInfoArray[i] entries may not be contiguous because the user
	   has the right to remove an event.
	   But if we do compaction after remove event, this function can be 
	   changed.  
	 */

	for ( i = 0; i != ESI->NumberOfEvents; i++ ) {
		index = ESI->EventInfoArray[i].pos[0];
		if ( index == -1 )
			continue;

		INTDBG( "Event index %d, position is 0x%x\n", i, index );

		/* If this is not a derived event */

		if ( ESI->EventInfoArray[i].derived == NOT_DERIVED ) {
			values[i] = dp[index];
			INTDBG( "value: 0x%llx\n", values[i] );
		} else {			 /* If this is a derived event */
			values[i] = handle_derived( &ESI->EventInfoArray[i], dp );
#ifdef DEBUG
			if ( values[i] < ( long long ) 0 ) {
				INTDBG( "Derived Event is negative!!: %lld\n", values[i] );
			}
			INTDBG( "derived value: 0x%llx \n", values[i] );
#endif
		}
	}

	return PAPI_OK;
}

int
_papi_hwi_cleanup_eventset( EventSetInfo_t * ESI )
{
   int i, j, num_cntrs, retval;
   hwd_context_t *context;
   int EventCode;
   NativeInfo_t *native;

   num_cntrs = _papi_hwd[ESI->CmpIdx]->cmp_info.num_mpx_cntrs;

   for(i=0;i<num_cntrs;i++) {

      EventCode=ESI->EventInfoArray[i].event_code;     

      /* skip if event not there */
      if ( EventCode == PAPI_NULL ) continue;

      /* If it is a MPX EventSet, remove it from the multiplex */
      /* data structure and this thread's multiplex list */

      if ( _papi_hwi_is_sw_multiplex( ESI ) ) {
	 retval = mpx_remove_event( &ESI->multiplex.mpx_evset, EventCode );
	 if ( retval < PAPI_OK )
	    return retval;
      } else {

	  native = ESI->NativeInfoArray;

	  /* clear out ESI->NativeInfoArray */
	  /* do we really need to do this, seeing as we free() it later? */

	  for( j = 0; j < ESI->NativeCount; j++ ) {
	     native[j].ni_event = -1;
	     native[j].ni_position = -1;
	     native[j].ni_owners = 0;
	     /* native[j].ni_bits?? */
	  }
      }

      /* do we really need to do this, seeing as we free() it later? */
      ESI->EventInfoArray[i].event_code= ( unsigned int ) PAPI_NULL;
      for( j = 0; j < MAX_COUNTER_TERMS; j++ ) {
	  ESI->EventInfoArray[i].pos[j] = -1;
      }
      ESI->EventInfoArray[i].ops = NULL;
      ESI->EventInfoArray[i].derived = NOT_DERIVED;
   }

   context = _papi_hwi_get_context( ESI, NULL );
   /* calling with count of 0 equals a close? */
   retval = _papi_hwd[ESI->CmpIdx]->update_control_state( ESI->ctl_state,
			       NULL, 0, context);
   if (retval!=PAPI_OK) {
     return retval;
   }

   ESI->CmpIdx = -1;
   ESI->NumberOfEvents = 0;
   ESI->NativeCount = 0;

   if ( ESI->ctl_state )
      papi_free( ESI->ctl_state );

   if ( ESI->sw_stop )
      papi_free( ESI->sw_stop );

   if ( ESI->hw_start )
      papi_free( ESI->hw_start );
	
   if ( ESI->EventInfoArray )
      papi_free( ESI->EventInfoArray );
	
   if ( ESI->NativeInfoArray ) 
      papi_free( ESI->NativeInfoArray );
	
   if ( ESI->overflow.deadline )
      papi_free( ESI->overflow.deadline );
	
   if ( ESI->profile.prof )
      papi_free( ESI->profile.prof );

   ESI->ctl_state = NULL;
   ESI->sw_stop = NULL;
   ESI->hw_start = NULL;
   ESI->EventInfoArray = NULL;
   ESI->NativeInfoArray = NULL;

   memset( &ESI->domain, 0x0, sizeof(EventSetDomainInfo_t) );
   memset( &ESI->granularity, 0x0, sizeof(EventSetGranularityInfo_t) );
   memset( &ESI->overflow, 0x0, sizeof(EventSetOverflowInfo_t) );
   memset( &ESI->multiplex, 0x0, sizeof(EventSetMultiplexInfo_t) );
   memset( &ESI->attach, 0x0, sizeof(EventSetAttachInfo_t) );
   memset( &ESI->cpu, 0x0, sizeof(EventSetCpuInfo_t) );
   memset( &ESI->profile, 0x0, sizeof(EventSetProfileInfo_t) );
   memset( &ESI->inherit, 0x0, sizeof(EventSetInheritInfo_t) );

   ESI->CpuInfo = NULL;

   return PAPI_OK;
}

int
_papi_hwi_convert_eventset_to_multiplex( _papi_int_multiplex_t * mpx )
{
	int retval, i, j = 0, *mpxlist = NULL;
	EventSetInfo_t *ESI = mpx->ESI;
	int flags = mpx->flags;

	/* If there are any events in the EventSet, 
	   convert them to multiplex events */

	if ( ESI->NumberOfEvents ) {

		mpxlist =
			( int * ) papi_malloc( sizeof ( int ) *
								   ( size_t ) ESI->NumberOfEvents );
		if ( mpxlist == NULL )
			return ( PAPI_ENOMEM );

		/* Build the args to MPX_add_events(). */

		/* Remember the EventInfoArray can be sparse
		   and the data can be non-contiguous */

		for ( i = 0; i < EventInfoArrayLength( ESI ); i++ )
			if ( ESI->EventInfoArray[i].event_code !=
				 ( unsigned int ) PAPI_NULL )
				mpxlist[j++] = ( int ) ESI->EventInfoArray[i].event_code;

		/* Resize the EventInfo_t array */

		if ( ( _papi_hwd[ESI->CmpIdx]->cmp_info.kernel_multiplex == 0 ) ||
			 ( ( _papi_hwd[ESI->CmpIdx]->cmp_info.kernel_multiplex ) &&
			   ( flags & PAPI_MULTIPLEX_FORCE_SW ) ) ) {
			retval =
				MPX_add_events( &ESI->multiplex.mpx_evset, mpxlist, j,
								ESI->domain.domain,
								ESI->granularity.granularity );
			if ( retval != PAPI_OK ) {
				papi_free( mpxlist );
				return ( retval );
			}
		}

		papi_free( mpxlist );
	}

	/* Update the state before initialization! */

	ESI->state |= PAPI_MULTIPLEXING;
	if ( _papi_hwd[ESI->CmpIdx]->cmp_info.kernel_multiplex &&
		 ( flags & PAPI_MULTIPLEX_FORCE_SW ) )
		ESI->multiplex.flags = PAPI_MULTIPLEX_FORCE_SW;
	ESI->multiplex.ns = ( int ) mpx->ns;

	return ( PAPI_OK );
}

#if 0
int
_papi_hwi_query( int preset_index, int *flags, char **note )
{
	if ( _papi_hwd_preset_map[preset_index].number == 0 )
		return ( 0 );
	INTDBG( "preset_index: %d derived: %d\n", preset_index,
			_papi_hwd_preset_map[preset_index].derived );
	if ( _papi_hwd_preset_map[preset_index].derived )
		*flags = PAPI_DERIVED;
	INTDBG( "note: %s\n", _papi_hwd_preset_map[preset_index].note );
	if ( _papi_hwd_preset_map[preset_index].note )
		*note = _papi_hwd_preset_map[preset_index].note;
	return ( 1 );
}
#endif

/*
 * Routine that initializes all available components.
 * A component is available if a pointer to its info vector
 * appears in the NULL terminated_papi_hwd table.
 */
int
_papi_hwi_init_global( void )
{
	int retval, i = 0;

	while ( _papi_hwd[i] ) {
		retval = _papi_hwi_innoculate_vector( _papi_hwd[i] );
		if ( retval != PAPI_OK )
			return ( retval );
		retval = _papi_hwd[i]->init_substrate( i );
		if ( retval != PAPI_OK )
			return ( retval );
		i++;
	}
	return ( PAPI_OK );
}

/* Machine info struct initialization using defaults */
/* See _papi_mdi definition in papi_internal.h       */

int
_papi_hwi_init_global_internal( void )
{
#ifdef NO_CPU_COUNTERS
	/* Developer's Note: In the current design, frequency and all other hardware info is set
	   by the various substrates. In the future, substrate code should be restricted to what
	   is directly related to counters. For now, the set_freq function is a temporary hack
	   which allows for the use of high-resolution timers when there is no CPU component. */
	set_freq(  );
#endif

	int retval;
	extern const hwi_preset_info_t
		_papi_hwi_preset_info[PAPI_MAX_PRESET_EVENTS];
	extern const unsigned int _papi_hwi_preset_type[PAPI_MAX_PRESET_EVENTS];

	memset( &_papi_hwi_presets, 0x0, sizeof ( _papi_hwi_presets ) );
	/* This member is static */
	_papi_hwi_presets.info = _papi_hwi_preset_info;
	_papi_hwi_presets.type = _papi_hwi_preset_type;

	memset( &_papi_hwi_system_info, 0x0, sizeof ( _papi_hwi_system_info ) );
#ifndef _WIN32
	memset( _papi_hwi_using_signal, 0x0, sizeof ( _papi_hwi_using_signal ) );
#endif

	/* Global struct to maintain EventSet mapping */
	retval =
		allocate_eventset_map( &_papi_hwi_system_info.global_eventset_map );
	if ( retval != PAPI_OK )
		return ( retval );

	_papi_hwi_system_info.pid = 0;	/* Process identifier */

	/* The PAPI_hw_info_t struct defined in papi.h */
	_papi_hwi_system_info.hw_info.ncpu = 0;	/* ncpu */
	_papi_hwi_system_info.hw_info.nnodes = 0;	/* nnodes */
	_papi_hwi_system_info.hw_info.totalcpus = 0;	/* totalcpus */
	_papi_hwi_system_info.hw_info.vendor = 0;	/* vendor */
	_papi_hwi_system_info.hw_info.vendor_string[0] = '\0';	/* vendor_string */
	_papi_hwi_system_info.hw_info.model = 0;	/* model */
	_papi_hwi_system_info.hw_info.model_string[0] = '\0';	/* model_string */
	_papi_hwi_system_info.hw_info.revision = 0.0;	/* revision */
	_papi_hwi_system_info.hw_info.mhz = 0.0;	/* mhz */
	_papi_hwi_system_info.hw_info.threads = 0;	/* hdw threads per core */
	_papi_hwi_system_info.hw_info.cores = 0;	/* cores per socket */
	_papi_hwi_system_info.hw_info.sockets = 0;	/* sockets */
	_papi_hwi_system_info.hw_info.cpuid_family = 0;	/* cpuid family */
	_papi_hwi_system_info.hw_info.cpuid_model = 0;	/* cpuid model */
	_papi_hwi_system_info.hw_info.cpuid_stepping = 0;	/* cpuid stepping */

	return ( PAPI_OK );
}

void
_papi_hwi_shutdown_global_internal( void )
{
	_papi_hwi_cleanup_all_presets(  );
	free_eventset_map( &_papi_hwi_system_info.global_eventset_map );
	if ( _papi_hwi_system_info.shlib_info.map ) {
		papi_free( _papi_hwi_system_info.shlib_info.map );
	}
	memset( &_papi_hwi_system_info, 0x0, sizeof ( _papi_hwi_system_info ) );
}

void
_papi_hwi_dummy_handler( int EventSet, void *address, long long overflow_vector,
						 void *context )
{
	/* This function is not used and shouldn't be called. */
	( void ) EventSet;		 /*unused */
	( void ) address;		 /*unused */
	( void ) overflow_vector;	/*unused */
	( void ) context;		 /*unused */
	return;
}

static long long
handle_derived_add( int *position, long long *from )
{
	int pos, i;
	long long retval = 0;

	i = 0;
	while ( i < MAX_COUNTER_TERMS ) {
		pos = position[i++];
		if ( pos == PAPI_NULL )
			break;
		INTDBG( "Compound event, adding %lld to %lld\n", from[pos], retval );
		retval += from[pos];
	}
	return ( retval );
}

static long long
handle_derived_subtract( int *position, long long *from )
{
	int pos, i;
	long long retval = from[position[0]];

	i = 1;
	while ( i < MAX_COUNTER_TERMS ) {
		pos = position[i++];
		if ( pos == PAPI_NULL )
			break;
		INTDBG( "Compound event, subtracting pos=%d  %lld from %lld\n", pos,
				from[pos], retval );
		retval -= from[pos];
	}
	return ( retval );
}

static long long
units_per_second( long long units, long long cycles )
{
	return ( ( units * ( long long ) _papi_hwi_system_info.hw_info.mhz *
			   ( long long ) 1000000 ) / cycles );
}

static long long
handle_derived_ps( int *position, long long *from )
{
	return ( units_per_second( from[position[1]], from[position[0]] ) );
}
static long long
handle_derived_add_ps( int *position, long long *from )
{
	long long tmp = handle_derived_add( position + 1, from );
	return ( units_per_second( tmp, from[position[0]] ) );
}

/* this function implement postfix calculation, it reads in a string where I use:
      |      as delimiter
      N2     indicate No. 2 native event in the derived preset
      +, -, *, /, %  as operator
      #      as MHZ(million hz) got from  _papi_hwi_system_info.hw_info.mhz*1000000.0

  Haihang (you@cs.utk.edu)
*/ 
static long long
_papi_hwi_postfix_calc( EventInfo_t * evi, long long *hw_counter )
{
	char *point = evi->ops, operand[16];
	double stack[PAPI_MAX_COUNTER_TERMS];
	int i, top = 0;

	memset(&stack,0,PAPI_MAX_COUNTER_TERMS*sizeof(double));

	while ( *point != '\0' ) {
		if ( *point == 'N' ) {	/* to get count for each native event */
			i = 0;
			point++;
			do {
				operand[i] = *point;
				point++;
				i++;
			} while ( *point != '|' );
			operand[i] = '\0';
			stack[top] = ( double ) hw_counter[evi->pos[atoi( operand )]];
			top++;
			point++;
		} else if ( *point == '#' ) {	/* to get mhz, ignore the rest char's */
			stack[top] = _papi_hwi_system_info.hw_info.mhz * 1000000.0;
			top++;
			do {
				point++;
			} while ( *point != '|' );
			point++;
		} else if ( isdigit( *point ) ) {	/* to get integer, I suppose only integer will be used, 
											   no error check here, please only use integer */
			i = 0;
			do {
				operand[i] = *point;
				point++;
				i++;
			} while ( *point != '|' );
			operand[i] = '\0';
			stack[top] = atoi( operand );
			top++;
			point++;
		} else if ( *point == '+' ) {	/* + calculation */
			stack[top - 2] += stack[top - 1];
			top--;
			do {
				point++;
			} while ( *point != '|' );
			point++;
		} else if ( *point == '-' ) {	/* - calculation */
			stack[top - 2] -= stack[top - 1];
			top--;
			do {
				point++;
			} while ( *point != '|' );
			point++;
		} else if ( *point == '*' ) {	/* * calculation */
			stack[top - 2] *= stack[top - 1];
			top--;
			do {
				point++;
			} while ( *point != '|' );
			point++;
		} else if ( *point == '/' ) {	/* / calculation */
			stack[top - 2] /= stack[top - 1];
			top--;
			do {
				point++;
			} while ( *point != '|' );
			point++;
		} else {			 /* do nothing */
			do {
				point++;
			} while ( *point != '|' );
			point++;
		}
	}
	return ( long long ) stack[0];
}

static long long
handle_derived( EventInfo_t * evi, long long *from )
{
	switch ( evi->derived ) {
	case DERIVED_ADD:
		return ( handle_derived_add( evi->pos, from ) );
	case DERIVED_ADD_PS:
		return ( handle_derived_add_ps( evi->pos, from ) );
	case DERIVED_SUB:
		return ( handle_derived_subtract( evi->pos, from ) );
	case DERIVED_PS:
		return ( handle_derived_ps( evi->pos, from ) );
	case DERIVED_POSTFIX:
		return ( _papi_hwi_postfix_calc( evi, from ) );
	case DERIVED_CMPD:		 /* This type has existed for a long time, but was never implemented.
							    Probably because its a no-op. However, if it's in a header, it
							    should be supported. As I found out when I implemented it in 
							    Pentium 4 for testing...dkt */
		return ( from[evi->pos[0]] );
	default:
		PAPIERROR( "BUG! Unknown derived command %d, returning 0",
				   evi->derived );
		return ( ( long long ) 0 );
	}
}

#if 0
static void
print_state( EventSetInfo_t * ESI, int cidx )
{
	int i;

	APIDBG( "\n\n-----------------------------------------\n" );
	APIDBG( "numEvent: %d    numNative: %d\n", ESI->NumberOfEvents,
			ESI->NativeCount );

	APIDBG( "\nnative_event code       " );
	for ( i = 0; i < _papi_hwd[cidx]->cmp_info.num_cntrs; i++ )
		APIDBG( "0x%15x", ESI->NativeInfoArray[i].ni_event );
	APIDBG( "\n" );

	APIDBG( "native_event_position     " );
	for ( i = 0; i < _papi_hwd[cidx]->cmp_info.num_cntrs; i++ )
		APIDBG( "%15d", ESI->NativeInfoArray[i].ni_position );
	APIDBG( "\n" );

#if 0						 /* This code is specific to POWER */
	APIDBG( "native_event_selectors    " );
	for ( i = 0; i < _papi_hwd[cidx]->cmp_info.num_cntrs; i++ )
		APIDBG( "%15d",
				native_table[ESI->NativeInfoArray[i].ni_event].resources.
				selector );
	APIDBG( "\n" );

	APIDBG( "counter_cmd               " );
	for ( i = 0; i < _papi_hwd[cidx]->cmp_info.num_cntrs; i++ )
		APIDBG( "%15d", ESI->ctl_state->counter_cmd.events[i] );
	APIDBG( "\n" );
#endif

	APIDBG( "native links              " );
	for ( i = 0; i < _papi_hwd[cidx]->cmp_info.num_cntrs; i++ )
		APIDBG( "%15d", ESI->NativeInfoArray[i].ni_owners );
	APIDBG( "\n" );

}
#endif

/* this function recusively does Modified Bipartite Graph counter allocation 
    success  return 1
    fail     return 0
*/
int
_papi_hwi_bipartite_alloc( hwd_reg_alloc_t * event_list, int count, int cidx )
{
	int i, j;
	char *ptr = ( char * ) event_list;
	int idx_q[count];				   /* queue of indexes of lowest rank events */
	int map_q[count];				   /* queue of mapped events (TRUE if mapped) */
	int head, tail;
	int size = _papi_hwd[cidx]->size.reg_alloc;

	/* build a queue of indexes to all events 
	   that live on one counter only (rank == 1) */
	head = 0;				 /* points to top of queue */
	tail = 0;				 /* points to bottom of queue */
	for ( i = 0; i < count; i++ ) {
		map_q[i] = 0;
		if ( _papi_hwd[cidx]->
			 bpt_map_exclusive( ( hwd_reg_alloc_t * ) & ptr[size * i] ) )
			idx_q[tail++] = i;
	}
	/* scan the single counter queue looking for events that share counters.
	   If two events can live only on one counter, return failure.
	   If the second event lives on more than one counter, remove shared counter
	   from its selector and reduce its rank. 
	   Mark first event as mapped to its counter. */
	while ( head < tail ) {
		for ( i = 0; i < count; i++ ) {
			if ( i != idx_q[head] ) {
				if ( _papi_hwd[cidx]->
					 bpt_map_shared( ( hwd_reg_alloc_t * ) & ptr[size * i],
									 ( hwd_reg_alloc_t * ) & ptr[size *
																 idx_q
																 [head]] ) ) {
					/* both share a counter; if second is exclusive, mapping fails */
					if ( _papi_hwd[cidx]->
						 bpt_map_exclusive( ( hwd_reg_alloc_t * ) &
											ptr[size * i] ) )
						return 0;
					else {
						_papi_hwd[cidx]->
							bpt_map_preempt( ( hwd_reg_alloc_t * ) &
											 ptr[size * i],
											 ( hwd_reg_alloc_t * ) & ptr[size *
																		 idx_q
																		 [head]] );
						if ( _papi_hwd[cidx]->
							 bpt_map_exclusive( ( hwd_reg_alloc_t * ) &
												ptr[size * i] ) )
							idx_q[tail++] = i;
					}
				}
			}
		}
		map_q[idx_q[head]] = 1;	/* mark this event as mapped */
		head++;
	}
	if ( tail == count ) {
		return 1;			 /* idx_q includes all events; everything is successfully mapped */
	} else {
		char *rest_event_list;
		char *copy_rest_event_list;
		int remainder;

		rest_event_list =
			papi_calloc(  _papi_hwd[cidx]->cmp_info.num_cntrs, 
				      size );

		copy_rest_event_list =
		        papi_calloc( _papi_hwd[cidx]->cmp_info.num_cntrs,
				     size );

		if ( !rest_event_list || !copy_rest_event_list ) {
			if ( rest_event_list )
				papi_free( rest_event_list );
			if ( copy_rest_event_list )
				papi_free( copy_rest_event_list );
			return ( 0 );
		}

		/* copy all unmapped events to a second list and make a backup */
		for ( i = 0, j = 0; i < count; i++ ) {
			if ( map_q[i] == 0 ) {
				memcpy( &copy_rest_event_list[size * j++], &ptr[size * i],
						( size_t ) size );
			}
		}
		remainder = j;

		memcpy( rest_event_list, copy_rest_event_list,
				( size_t ) size * ( size_t ) remainder );

		/* try each possible mapping until you fail or find one that works */
		for ( i = 0; i < _papi_hwd[cidx]->cmp_info.num_cntrs; i++ ) {
			/* for the first unmapped event, try every possible counter */
			if ( _papi_hwd[cidx]->
				 bpt_map_avail( ( hwd_reg_alloc_t * ) rest_event_list, i ) ) {
				_papi_hwd[cidx]->
					bpt_map_set( ( hwd_reg_alloc_t * ) rest_event_list, i );
				/* remove selected counter from all other unmapped events */
				for ( j = 1; j < remainder; j++ ) {
					if ( _papi_hwd[cidx]->
						 bpt_map_shared( ( hwd_reg_alloc_t * ) &
										 rest_event_list[size * j],
										 ( hwd_reg_alloc_t * )
										 rest_event_list ) )
						_papi_hwd[cidx]->
							bpt_map_preempt( ( hwd_reg_alloc_t * ) &
											 rest_event_list[size * j],
											 ( hwd_reg_alloc_t * )
											 rest_event_list );
				}
				/* if recursive call to allocation works, break out of the loop */
				if ( _papi_hwi_bipartite_alloc
					 ( ( hwd_reg_alloc_t * ) rest_event_list, remainder,
					   cidx ) )
					break;

				/* recursive mapping failed; copy the backup list and try the next combination */
				memcpy( rest_event_list, copy_rest_event_list,
						( size_t ) size * ( size_t ) remainder );
			}
		}
		if ( i == _papi_hwd[cidx]->cmp_info.num_cntrs ) {
			papi_free( rest_event_list );
			papi_free( copy_rest_event_list );
			return 0;		 /* fail to find mapping */
		}
		for ( i = 0, j = 0; i < count; i++ ) {
			if ( map_q[i] == 0 )
				_papi_hwd[cidx]->
					bpt_map_update( ( hwd_reg_alloc_t * ) & ptr[size * i],
									( hwd_reg_alloc_t * ) & rest_event_list[size
																			*
																			j++] );
		}
		papi_free( rest_event_list );
		papi_free( copy_rest_event_list );
		return 1;
	}
}

/* _papi_hwi_get_event_info:
   Assumes EventCode contains a valid preset code.
   But defensive programming says check for NULL pointers.
   Returns a filled in PAPI_event_info_t structure containing
   descriptive strings and values for the specified preset event.
*/
int
_papi_hwi_get_event_info( int EventCode, PAPI_event_info_t * info )
{
	int i = EventCode & PAPI_PRESET_AND_MASK;
	int j;

	if ( _papi_hwi_presets.info[i].symbol ) {	/* if the event is in the preset table */
		memset( info, 0, sizeof ( *info ) );
		info->event_code = ( unsigned int ) EventCode;
		info->event_type = _papi_hwi_presets.type[i];
		info->count = _papi_hwi_presets.count[i];
		strcpy( info->symbol, _papi_hwi_presets.info[i].symbol );
		if ( _papi_hwi_presets.info[i].short_descr != NULL )
			strncpy( info->short_descr, _papi_hwi_presets.info[i].short_descr,
					 sizeof ( info->short_descr ) );
		if ( _papi_hwi_presets.info[i].long_descr != NULL )
			strncpy( info->long_descr, _papi_hwi_presets.info[i].long_descr,
					 sizeof ( info->long_descr ) );
		info->derived[0] = '\0';
		info->postfix[0] = '\0';
		if ( _papi_hwi_presets.data[i] ) {	/* if the event exists on this platform */
			strncpy( info->postfix, _papi_hwi_presets.data[i]->operation,
					 sizeof ( info->postfix ) );
			_papi_hwi_derived_string( _papi_hwi_presets.data[i]->derived,
									  info->derived, sizeof ( info->derived ) );
			for ( j = 0; j < ( int ) info->count; j++ ) {
				info->code[j] =
					( unsigned int ) _papi_hwi_presets.data[i]->native[j];
				_papi_hwi_native_code_to_name( info->code[j], info->name[j],
											   sizeof ( info->name[j] ) );
			}
		}
		if ( _papi_hwi_presets.dev_note[i] ) {	/* if a developer's note exists for this event */
			strncpy( info->note, _papi_hwi_presets.dev_note[i],
					 sizeof ( info->note ) );
		} else
			info->note[0] = '\0';

		return ( PAPI_OK );
	} else {
		return ( PAPI_ENOEVNT );
	}
}
