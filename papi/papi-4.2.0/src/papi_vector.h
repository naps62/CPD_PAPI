/**
  * @file papi_vector.h
  */
#ifndef _PAPI_VECTOR_H
#define _PAPI_VECTOR_H

#include "papi.h"

/** Identifier for each component 
 *	@internal */
typedef struct cmp_id {
	char name[PAPI_MAX_STR_LEN];
	char descr[PAPI_MAX_STR_LEN];
} cmp_id_t;

/** Sizes of structure private to each component 
  */
typedef struct cmp_struct_sizes {
    int		context;
    int		control_state;
    int		reg_value;
    int		reg_alloc;
} cmp_struct_sizes_t;

/** Vector Table Stuff 
 *	@internal */
typedef struct papi_vectors {
/** Substrate specific data structure @see papi.h */
    PAPI_component_info_t   cmp_info;

/** Substrate specific structure sizes*/
    cmp_struct_sizes_t size;

/* List of exposed function pointers for this component */
#ifdef _WIN32 /* Windows requires a different callback format */
    void	(*timer_callback)	(UINT, UINT, DWORD, DWORD, DWORD);
#else
	void ( *dispatch_timer ) ( int, hwd_siginfo_t *, void * );
#endif
    void *	(*get_overflow_address)	(int, char *, int);						/**< */
    int		(*start)		(hwd_context_t *, hwd_control_state_t *);		/**< */
    int		(*stop)			(hwd_context_t *, hwd_control_state_t *);		/**< */
    int		(*read)			(hwd_context_t *, hwd_control_state_t *, long long **, int);	/**< */
    int		(*reset)		(hwd_context_t *, hwd_control_state_t *);		/**< */
    int		(*write)		(hwd_context_t *, hwd_control_state_t *, long long[]);			/**< */
	int			(*cleanup_eventset)	( hwd_control_state_t * );				/**< */
    long long	(*get_real_cycles)	(void);									/**< */
    long long	(*get_real_usec)	(void);									/**< */
    long long	(*get_virt_cycles)	(hwd_context_t *);				/**< */
    long long	(*get_virt_usec)	(hwd_context_t *);				/**< */
    int		(*stop_profiling)	(ThreadInfo_t *, EventSetInfo_t *);			/**< */
    int		(*init_substrate)	(int);										/**< */
    int		(*init)			(hwd_context_t *);								/**< */
    int		(*init_control_state)	(hwd_control_state_t * ptr);			/**< */
    int		(*update_shlib_info)	(papi_mdi_t * mdi);									/**< */
    int		(*get_system_info)	(papi_mdi_t * mdi);										/**< */
    int		(*get_memory_info)	(PAPI_hw_info_t *, int);					/**< */
    int		(*update_control_state)	(hwd_control_state_t *, NativeInfo_t *, int, hwd_context_t *);	/**< */
    int		(*ctl)			(hwd_context_t *, int , _papi_int_option_t *);	/**< */
    int		(*set_overflow)		(EventSetInfo_t *, int, int);				/**< */
    int		(*set_profile)		(EventSetInfo_t *, int, int);				/**< */
    int		(*add_prog_event)	(hwd_control_state_t *, unsigned int, void *, EventInfo_t *);		/**< */
    int		(*set_domain)		(hwd_control_state_t *, int);				/**< */
    int		(*ntv_enum_events)	(unsigned int *, int);						/**< */
    int		(*ntv_name_to_code)	(char *, unsigned int *);					/**< */
    int		(*ntv_code_to_name)	(unsigned int, char *, int);				/**< */
    int		(*ntv_code_to_descr)	(unsigned int, char *, int);			/**< */
    int		(*ntv_code_to_bits)	(unsigned int, hwd_register_t *);			/**< */
    int		(*ntv_bits_to_info)	(hwd_register_t *, char *, unsigned int *, int, int);		/**< */
    int		(*allocate_registers)	(EventSetInfo_t *);						/**< */
    int		(*bpt_map_avail)	(hwd_reg_alloc_t *, int);					/**< */
    void	(*bpt_map_set)		(hwd_reg_alloc_t *, int);					/**< */
    int		(*bpt_map_exclusive)	(hwd_reg_alloc_t *);					/**< */
    int		(*bpt_map_shared)	(hwd_reg_alloc_t *, hwd_reg_alloc_t *);		/**< */
    void	(*bpt_map_preempt)	(hwd_reg_alloc_t *, hwd_reg_alloc_t *);		/**< */
    void	(*bpt_map_update)	(hwd_reg_alloc_t *, hwd_reg_alloc_t *);		/**< */
    int		(*get_dmem_info)	(PAPI_dmem_info_t *);						/**< */
    int		(*shutdown)		(hwd_context_t *);								/**< */
    int		(*shutdown_substrate)	(void);									/**< */
    int		(*user)			(int, void *, void *);							/**< */
}papi_vector_t;

extern papi_vector_t *_papi_hwd[];

/* Prototypes */
int _papi_hwi_innoculate_vector( papi_vector_t * v );
void *vector_find_dummy( void *func, char **buf );
void vector_print_table( papi_vector_t * v, int print_func );

#endif /* _PAPI_VECTOR_H */
