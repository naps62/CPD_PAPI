/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/
/** 
* @file    papi_internal.h
* @author  Philip Mucci
*          mucci@cs.utk.edu
* @author  Dan Terpstra
*          terpstra.utk.edu
* @author  Kevin London
*	       london@cs.utk.edu
* @author  Haihang You
*          you@cs.utk.edu
* CVS:     $Id: papi_internal.h,v 1.201 2011/08/05 21:43:26 vweaver1 Exp $
*/

#ifndef _PAPI_INTERNAL_H
#define _PAPI_INTERNAL_H

/* AIX's C compiler does not recognize the inline keyword */
#ifdef _AIX
#define inline
#endif 

#include "papi_debug.h"

#define DEADBEEF 0xdedbeef
extern int papi_num_components;

  /********************************************************/
/* This block provides general strings used in PAPI     */
/* If a new string is needed for PAPI prompts           */
/* it should be placed in this file and referenced by   */
/* label.                                               */
/********************************************************/
#define PAPI_ERROR_CODE_str      "Error Code"
#define PAPI_SHUTDOWN_str	      "PAPI_shutdown: PAPI is not initialized"
#define PAPI_SHUTDOWN_SYNC_str	"PAPI_shutdown: other threads still have running EventSets"


/* some members of structs and/or function parameters may or may not be
   necessary, but at this point, we have included anything that might 
   possibly be useful later, and will remove them as we progress */

/* Signal used for overflow delivery */

/****WIN32 We'll need to figure out how to handle this for Windows */
#ifdef _WIN32
#define PAPI_INT_SIGNAL 1
#define PAPI_INT_ITIMER 1
#else
#define PAPI_INT_MPX_SIGNAL SIGPROF
#define PAPI_INT_SIGNAL SIGPROF
#define PAPI_INT_ITIMER ITIMER_PROF
#endif

#define PAPI_INT_ITIMER_MS 1
#if defined(linux)
#define PAPI_NSIG _NSIG
#else
#define PAPI_NSIG 128
#endif

/* Multiplex definitions */

#define PAPI_INT_MPX_DEF_US 10000	/*Default resolution in us. of mpx handler */

/* Commands used to compute derived events */

#define NOT_DERIVED      0x0    /**< Do nothing */
#define DERIVED_ADD      0x1    /**< Add counters */
#define DERIVED_PS       0x2    /**< Divide by the cycle counter and convert to seconds */
#define DERIVED_ADD_PS   0x4    /**< Add 2 counters then divide by the cycle counter and xl8 to secs. */
#define DERIVED_CMPD     0x8    /**< Event lives in operand index but takes 2 or more codes */
#define DERIVED_SUB      0x10   /**< Sub all counters from counter with operand_index */
#define DERIVED_POSTFIX  0x20   /**< Process counters based on specified postfix string */

/* Thread related: thread local storage */

#define LOWLEVEL_TLS		PAPI_NUM_TLS+0
#define NUM_INNER_TLS   	1
#define PAPI_MAX_TLS		(NUM_INNER_TLS+PAPI_NUM_TLS)

/* Thread related: locks */

#define INTERNAL_LOCK      	PAPI_NUM_LOCK+0	/* papi_internal.c */
#define MULTIPLEX_LOCK     	PAPI_NUM_LOCK+1	/* multiplex.c */
#define THREADS_LOCK		PAPI_NUM_LOCK+2	/* threads.c */
#define HIGHLEVEL_LOCK		PAPI_NUM_LOCK+3	/* papi_hl.c */
#define MEMORY_LOCK		PAPI_NUM_LOCK+4	/* papi_memory.c */
#define SUBSTRATE_LOCK          PAPI_NUM_LOCK+5	/* <substrate.c> */
#define GLOBAL_LOCK          	PAPI_NUM_LOCK+6	/* papi.c for global variable (static and non) initialization/shutdown */
#define CPUS_LOCK		PAPI_NUM_LOCK+7	/* cpus.c */
#define NAMELIB_LOCK            PAPI_NUM_LOCK+8 /* papi_pfm4_events.c */

/* extras related */

#define NEED_CONTEXT		1
#define DONT_NEED_CONTEXT 	0

/* Replacement for bogus supports_multiple_threads item */
/* xxxx Should this be at the component level or the hw info level? I think it's system wide... */
#define SUPPORTS_MULTIPLE_THREADS(cmp_info) (cmp_info.available_granularities & PAPI_GRN_THR)

/* This was defined by each substrate as = (MAX_COUNTERS < 8) ? MAX_COUNTERS : 8 
    Now it's defined globally as 8 for everything. Mainly applies to max terms in
    derived events.
*/
#ifdef __bgp__
#define PAPI_MAX_COUNTER_TERMS	19
#else
#define PAPI_MAX_COUNTER_TERMS	8
#endif

/* these vestigial pointers are to structures defined in the components
    they are opaque to the framework and defined as void at this level
    they are remapped to real data in the component routines that use them */
#define hwd_context_t		void
#define hwd_control_state_t	void
#define hwd_reg_alloc_t		void
#define hwd_register_t		void
#define hwd_siginfo_t		void
#define hwd_ucontext_t		void

/* DEFINES END HERE */

#if !(defined(_WIN32) || defined(NO_CONFIG))
#include "config.h"
#endif

//#include OS_HEADER
#include SUBSTRATE
#include "papi_preset.h"

#ifndef inline_static
#define inline_static inline static
#endif

typedef struct _EventSetDomainInfo {
   int domain;
} EventSetDomainInfo_t;

typedef struct _EventSetGranularityInfo {
   int granularity;
} EventSetGranularityInfo_t;

typedef struct _EventSetOverflowInfo {
   int flags;
   int event_counter;
   PAPI_overflow_handler_t handler;
   long long *deadline;
   int *threshold;
   int *EventIndex;
   int *EventCode;
} EventSetOverflowInfo_t;

typedef struct _EventSetAttachInfo {
  unsigned long tid;
} EventSetAttachInfo_t;

typedef struct _EventSetCpuInfo {
  unsigned int cpu_num;
} EventSetCpuInfo_t;

typedef struct _EventSetInheritInfo
{
	int inherit;
} EventSetInheritInfo_t;

/** @internal */
typedef struct _EventSetProfileInfo {
   PAPI_sprofil_t **prof;
   int *count;     /**< Number of buffers */
   int *threshold;
   int *EventIndex;
   int *EventCode;
   int flags;
   int event_counter;
} EventSetProfileInfo_t;

/** This contains info about an individual event added to the EventSet.
  The event can be either PRESET or NATIVE, and either simple or derived.
  If derived, it can consist of up to MAX_COUNTER_TERMS native events.
  An EventSet contains a pointer to an array of these structures to define
  each added event.
  @internal
 */
typedef struct _EventInfo {
   unsigned int event_code;     /**< Preset or native code for this event as passed to PAPI_add_event() */
   /* should this be MAX_COUNTER_TERMS instead of MAX_COUNTERS ?? (dkt 10/9/03) */
   int pos[MAX_COUNTER_TERMS];   /**< position in the counter array for this events components */
   char *ops;                   /**< operation string of preset (points into preset event struct) */
   int derived;                 /**< Counter derivation command used for derived events */
} EventInfo_t;

/** This contains info about each native event added to the EventSet.
  An EventSet contains an array of MAX_COUNTERS of these structures 
  to define each native event in the set.
  @internal 
 */
typedef struct _NativeInfo {
   int ni_event;                /**< native event code; always non-zero unless empty */
   int ni_position;             /**< counter array position where this native event lives */
   int ni_owners;               /**< specifies how many owners share this native event */
   hwd_register_t *ni_bits;     /**< Substrate defined resources used by this native event */
} NativeInfo_t;


/* Multiplex definitions */

/** This contains only the information about an event that
 *	would cause two events to be counted separately.  Options
 *	that don't affect an event aren't included here.
 *	@internal 
 */
typedef struct _papi_info {
   long long event_type;
   int domain;
   int granularity;
} PapiInfo;

typedef struct _masterevent {
   int uses;
   int active;
   int is_a_rate;
   int papi_event;
   PapiInfo pi;
   long long count;
   long long cycles;
   long long handler_count;
   long long prev_total_c;
   long long count_estimate;
   double rate_estimate;
   struct _threadlist *mythr;
   struct _masterevent *next;
} MasterEvent;

/** @internal */
typedef struct _threadlist {
#ifdef PTHREADS
	pthread_t thr;
#else
	unsigned long int tid;
#endif
   /** Total cycles for this thread */
   long long total_c;
   /** Pointer to event in use */
   MasterEvent *cur_event;
   /** List of multiplexing events for this thread */
   MasterEvent *head;
   /** Pointer to next thread */
   struct _threadlist *next;
} Threadlist;

/* Structure contained in the EventSet structure that
   holds information about multiplexing. */

typedef enum
{ MPX_STOPPED, MPX_RUNNING } MPX_status;

/** Structure contained in the EventSet structure that
    holds information about multiplexing.
 @internal */
typedef struct _MPX_EventSet {
   MPX_status status;
   /** Pointer to this thread's structure */
   struct _threadlist *mythr;
   /** Pointers to this EventSet's MPX entries in the master list for this thread */
   struct _masterevent *(mev[PAPI_MPX_DEF_DEG]);
   /** Number of entries in above list */
   int num_events;
   /** Not sure... */
   long long start_c, stop_c;
   long long start_values[PAPI_MPX_DEF_DEG];
   long long stop_values[PAPI_MPX_DEF_DEG];
   long long start_hc[PAPI_MPX_DEF_DEG];
} MPX_EventSet;

typedef struct EventSetMultiplexInfo {
  MPX_EventSet *mpx_evset;
  int ns;
  int flags; 
} EventSetMultiplexInfo_t;

/** Opaque struct, not defined yet...due to threads.h <-> papi_internal.h 
 @internal */
struct _ThreadInfo;
struct _CpuInfo;

/** Fields below are ordered by access in PAPI_read for performance 
 @internal */
typedef struct _EventSetInfo {
  struct _ThreadInfo *master;  /**< Pointer to the thread that owns this EventSet */
  struct _CpuInfo    *CpuInfo; /**< Pointer to cpu that owns this EventSet */
  
  int state;                   /**< The state of this entire EventSet; can be
				  PAPI_RUNNING or PAPI_STOPPED plus flags */
  
  EventInfo_t *EventInfoArray; /**< This array contains the mapping from 
				  events added into the API into hardware 
				  specific encoding as returned by the 
				  kernel or the code that directly 
				  accesses the counters. */
  
  hwd_control_state_t *ctl_state; /**< This contains the encoding necessary for the 
                                   hardware to set the counters to the appropriate
                                   conditions */

  unsigned long int tid;       /**< Thread ID, only used if PAPI_thread_init() is called  */
  
  int EventSetIndex;           /**< Index of the EventSet in the array  */

   int CmpIdx;		    /**< Which Component this EventSet Belongs to */
  
  int NumberOfEvents;          /**< Number of events added to EventSet */
  
  long long *hw_start;         /**< Array of length _papi_hwi_system_info.num_cntrs that contains
				  unprocessed, out of order, long long counter registers */
  
  long long *sw_stop;          /**< Array of length ESI->NumberOfCounters that contains
				  processed, in order, PAPI counter values when used or stopped */
  
  int NativeCount;             /**< How many native events in the array below. */
  
  NativeInfo_t *NativeInfoArray;  /**< Info about each native event in the set */
  
  EventSetDomainInfo_t domain;
  EventSetGranularityInfo_t granularity;
  EventSetOverflowInfo_t overflow;
  EventSetMultiplexInfo_t multiplex;
  EventSetAttachInfo_t attach;
  EventSetCpuInfo_t cpu;
  EventSetProfileInfo_t profile;
  EventSetInheritInfo_t inherit;
} EventSetInfo_t;

/** @internal */
typedef struct _dynamic_array {
   EventSetInfo_t **dataSlotArray;      /**< array of ptrs to EventSets */
   int totalSlots;              /**< number of slots in dataSlotArrays      */
   int availSlots;              /**< number of open slots in dataSlotArrays */
   int fullSlots;               /**< number of full slots in dataSlotArray    */
   int lowestEmptySlot;         /**< index of lowest empty dataSlotArray    */
} DynamicArray_t;

/* Substrate option types for _papi_hwd_ctl. */

typedef struct _papi_int_attach {
   unsigned long tid;
   EventSetInfo_t *ESI;
} _papi_int_attach_t;

typedef struct _papi_int_cpu {
   unsigned int cpu_num;
   EventSetInfo_t *ESI;
} _papi_int_cpu_t;

typedef struct _papi_int_multiplex {
   int flags;
   unsigned long ns;
   EventSetInfo_t *ESI;
} _papi_int_multiplex_t;

typedef struct _papi_int_defdomain {
   int defdomain;
} _papi_int_defdomain_t;

typedef struct _papi_int_domain {
   int domain;
   int eventset;
   EventSetInfo_t *ESI;
} _papi_int_domain_t;

typedef struct _papi_int_granularity {
   int granularity;
   int eventset;
   EventSetInfo_t *ESI;
} _papi_int_granularity_t;

typedef struct _papi_int_overflow {
   EventSetInfo_t *ESI;
   EventSetOverflowInfo_t overflow;
} _papi_int_overflow_t;

typedef struct _papi_int_profile {
   EventSetInfo_t *ESI;
   EventSetProfileInfo_t profile;
} _papi_int_profile_t;

typedef PAPI_itimer_option_t _papi_int_itimer_t;
/* These shortcuts are only for use code */
#undef multiplex_itimer_sig
#undef multiplex_itimer_num
#undef multiplex_itimer_us

typedef struct _papi_int_inherit
{
	EventSetInfo_t *ESI;
	int inherit;
} _papi_int_inherit_t;

/** @internal */
typedef struct _papi_int_addr_range { /* if both are zero, range is disabled */
   EventSetInfo_t *ESI;
   int domain;
   caddr_t start;                /**< start address of an address range */
   caddr_t end;                  /**< end address of an address range */
   int start_off;                /**< offset from start address as programmed in hardware */
   int end_off;                  /**< offset from end address as programmed in hardware */
                                 /**< if offsets are undefined, they are both set to -1 */
} _papi_int_addr_range_t;

typedef union _papi_int_option_t {
   _papi_int_overflow_t overflow;
   _papi_int_profile_t profile;
   _papi_int_domain_t domain;
   _papi_int_attach_t attach;
   _papi_int_cpu_t cpu;
   _papi_int_multiplex_t multiplex;
   _papi_int_itimer_t itimer;
	_papi_int_inherit_t inherit;
	_papi_int_granularity_t granularity;
	_papi_int_addr_range_t address_range;
} _papi_int_option_t;

/** Hardware independent context 
 *	@internal */
typedef struct {
   hwd_siginfo_t *si;
   hwd_ucontext_t *ucontext;
} _papi_hwi_context_t;

/** @internal */
typedef struct _papi_mdi {
   DynamicArray_t global_eventset_map;  /**< Global structure to maintain int<->EventSet mapping */
   pid_t pid;                   /**< Process identifier */
/*   PAPI_substrate_info_t sub_info; *//* See definition in papi.h */
   PAPI_hw_info_t hw_info;      /**< See definition in papi.h */
   PAPI_exe_info_t exe_info;    /**< See definition in papi.h */
/*   PAPI_mpx_info_t mpx_info; */   /* See definition in papi.h */
   PAPI_shlib_info_t shlib_info;    /**< See definition in papi.h */
   PAPI_preload_info_t preload_info; /**< See definition in papi.h */ 
} papi_mdi_t;

extern papi_mdi_t _papi_hwi_system_info;
extern int _papi_hwi_error_level;
extern const hwi_describe_t _papi_hwi_err[PAPI_NUM_ERRORS];
/*extern volatile int _papi_hwi_using_signal;*/
extern int _papi_hwi_using_signal[PAPI_NSIG];

#include "threads.h"
#include "cpus.h"
#include "papi_vector.h"
#include "papi_protos.h"

inline_static EventSetInfo_t *
_papi_hwi_lookup_EventSet( int eventset )
{
	const DynamicArray_t *map = &_papi_hwi_system_info.global_eventset_map;
	EventSetInfo_t *set;

	if ( ( eventset < 0 ) || ( eventset > map->totalSlots ) )
		return ( NULL );

	set = map->dataSlotArray[eventset];
#ifdef DEBUG
	if ( ( ISLEVEL( DEBUG_THREADS ) ) && ( _papi_hwi_thread_id_fn ) &&
		 ( set->master->tid != _papi_hwi_thread_id_fn(  ) ) )
		return ( NULL );
#endif

	return ( set );
}

inline_static int
_papi_hwi_is_sw_multiplex( EventSetInfo_t * ESI )
{
	/* Are we multiplexing at all */
	if ( ( ESI->state & PAPI_MULTIPLEXING ) == 0 )
		return ( 0 );
	/* Does the substrate support kernel multiplexing */
	if ( _papi_hwd[ESI->CmpIdx]->cmp_info.kernel_multiplex ) {
		/* Have we forced software multiplexing */
		if ( ESI->multiplex.flags == PAPI_MULTIPLEX_FORCE_SW )
			return ( 1 );
		return ( 0 );
	} else
		return ( 1 );
}

inline_static int
_papi_hwi_invalid_cmp( int cidx )
{
	return ( cidx < 0 || cidx >= papi_num_components );
}

inline_static hwd_context_t *
_papi_hwi_get_context( EventSetInfo_t * ESI, int *is_dirty )
{
	INTDBG("Entry: ESI: %p, is_dirty: %p\n", ESI, is_dirty);
	int dirty_ctx;
	hwd_context_t *ctx=NULL;

	/* assume for now the control state is clean (last updated by this ESI) */
	dirty_ctx = 0;
	
	/* get a context pointer based on if we are counting for a thread or for a cpu */
	if (ESI->state & PAPI_CPU_ATTACHED) {
		/* use cpu context */
		ctx = ESI->CpuInfo->context[ESI->CmpIdx];

		/* if the user wants to know if the control state was last set by the same event set, tell him */
		if (is_dirty != NULL) {
			if (ESI->CpuInfo->from_esi != ESI) {
				dirty_ctx = 1;
			}
			*is_dirty = dirty_ctx;
		}
		ESI->CpuInfo->from_esi = ESI;
	   
	} else {

		/* use thread context */
		ctx = ESI->master->context[ESI->CmpIdx];

		/* if the user wants to know if the control state was last set by the same event set, tell him */
		if (is_dirty != NULL) {
			if (ESI->master->from_esi != ESI) {
				dirty_ctx = 1;
			}
			*is_dirty = dirty_ctx;
		}
		ESI->master->from_esi = ESI;

	}
	return( ctx );
}

/* function declarations */

extern int _papi_hwi_error_level;
extern PAPI_debug_handler_t _papi_hwi_debug_handler;
void PAPIERROR( char *format, ... );
int _papi_hwi_assign_eventset( EventSetInfo_t * ESI, int cidx );
void _papi_hwi_free_EventSet( EventSetInfo_t * ESI );
int _papi_hwi_create_eventset( int *EventSet, ThreadInfo_t * handle );
int _papi_hwi_lookup_EventCodeIndex( const EventSetInfo_t * ESI,
				     unsigned int EventCode );
int _papi_hwi_remove_EventSet( EventSetInfo_t * ESI );
void _papi_hwi_remap_event_position( EventSetInfo_t * ESI, int thisindex, int total_events );
int _papi_hwi_add_event( EventSetInfo_t * ESI, int EventCode );
int _papi_hwi_add_pevent( EventSetInfo_t * ESI, int EventCode, void *inout );
int _papi_hwi_remove_event( EventSetInfo_t * ESI, int EventCode );
int _papi_hwi_read( hwd_context_t * context, EventSetInfo_t * ESI,
		    long long *values );
int _papi_hwi_cleanup_eventset( EventSetInfo_t * ESI );
int _papi_hwi_convert_eventset_to_multiplex( _papi_int_multiplex_t * mpx );
int _papi_hwi_init_global( void );
int _papi_hwi_init_global_internal( void );
void _papi_hwi_shutdown_global_internal( void );
void _papi_hwi_dummy_handler( int EventSet, void *address, long long overflow_vector,
			      void *context );
int _papi_hwi_bipartite_alloc( hwd_reg_alloc_t * event_list, int count, int cidx );
int _papi_hwi_get_event_info( int EventCode, PAPI_event_info_t * info );

#endif /* PAPI_INTERNAL_H */
