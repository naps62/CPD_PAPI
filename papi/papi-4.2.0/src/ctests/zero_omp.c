/* 
* File:    zero_omp.c
* CVS:     $Id: zero_omp.c,v 1.36 2011/07/11 18:54:03 vweaver1 Exp $
* Author:  Philip Mucci
*          mucci@cs.utk.edu
* Mods:    Nils Smeds
*          smeds@pdc.kth.se
*          Anders Nilsson
*          anni@pdc.kth.se
*/

/* This file performs the following test: start, stop and timer
functionality for 2 slave OMP threads

   - It attempts to use the following two counters. It may use less
depending on hardware counter resource limitations. These are counted
in the default counting domain and default granularity, depending on
the platform. Usually this is the user domain (PAPI_DOM_USER) and
thread context (PAPI_GRN_THR).

     + PAPI_FP_INS
     + PAPI_TOT_CYC

Each thread inside the Thread routine:
   - Get cyc.
   - Get us.
   - Start counters
   - Do flops
   - Stop and read counters
   - Get us.
   - Get cyc.

Master serial thread:
   - Get us.
   - Get cyc.
   - Run parallel for loop
   - Get us.
   - Get cyc.
*/

#include "papi_test.h"

#ifdef _OPENMP
#include <omp.h>
#else
#error "This compiler does not understand OPENMP"
#endif

extern int TESTS_QUIET;				   /* Declared in test_utils.c */
const PAPI_hw_info_t *hw_info = NULL;

void
Thread( int n )
{
	int retval, num_tests = 1;
	int EventSet1 = PAPI_NULL;
	int PAPI_event, mask1;
	int num_events1;
	long long **values;
	long long elapsed_us, elapsed_cyc;
	char event_name[PAPI_MAX_STR_LEN];

	printf( "Thread 0x%x started\n", omp_get_thread_num(  ) );
	num_events1 = 2;

	/* add PAPI_TOT_CYC and one of the events in 
	   PAPI_FP_INS, PAPI_FP_OPS or PAPI_TOT_INS, 
	   depending on the availability of the event 
	   on the platform */
	EventSet1 = add_two_events( &num_events1, &PAPI_event, hw_info, &mask1 );

	retval = PAPI_event_code_to_name( PAPI_event, event_name );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_event_code_to_name", retval );

	values = allocate_test_space( num_tests, num_events1 );

	elapsed_us = PAPI_get_real_usec(  );

	elapsed_cyc = PAPI_get_real_cyc(  );

	retval = PAPI_start( EventSet1 );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_start", retval );

	do_flops( n );

	retval = PAPI_stop( EventSet1, values[0] );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_stop", retval );

	elapsed_us = PAPI_get_real_usec(  ) - elapsed_us;

	elapsed_cyc = PAPI_get_real_cyc(  ) - elapsed_cyc;

	remove_test_events( &EventSet1, mask1 );

	if ( !TESTS_QUIET ) {
		printf( "Thread 0x%x %-12s : \t%lld\n", omp_get_thread_num(  ),
				event_name, values[0][1] );
		printf( "Thread 0x%x PAPI_TOT_CYC: \t%lld\n", omp_get_thread_num(  ),
				values[0][0] );
		printf( "Thread 0x%x Real usec   : \t%lld\n", omp_get_thread_num(  ),
				elapsed_us );
		printf( "Thread 0x%x Real cycles : \t%lld\n", omp_get_thread_num(  ),
				elapsed_cyc );
	}

	/* It is illegal for the threads to exit in OpenMP */
	/* test_pass(__FILE__,0,0); */
	free_test_space( values, num_tests );

	PAPI_unregister_thread(  );
	printf( "Thread 0x%x finished\n", omp_get_thread_num(  ) );
}

int
main( int argc, char **argv )
{
	int maxthr, retval;
	long long elapsed_us, elapsed_cyc;

	tests_quiet( argc, argv );	/* Set TESTS_QUIET variable */

	retval = PAPI_library_init( PAPI_VER_CURRENT );
	if ( retval != PAPI_VER_CURRENT )
		test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

	hw_info = PAPI_get_hardware_info(  );
	if ( hw_info == NULL )
		test_fail( __FILE__, __LINE__, "PAPI_get_hardware_info", 2 );

	elapsed_us = PAPI_get_real_usec(  );

	elapsed_cyc = PAPI_get_real_cyc(  );


	retval =
		PAPI_thread_init( ( unsigned
							long ( * )( void ) ) ( omp_get_thread_num ) );
	if ( retval != PAPI_OK ) {
		if ( retval == PAPI_ESBSTR )
			test_skip( __FILE__, __LINE__, "PAPI_thread_init", retval );
		else
			test_fail( __FILE__, __LINE__, "PAPI_thread_init", retval );
	}
#pragma omp parallel private(maxthr)
	{
		maxthr = omp_get_num_threads(  );
		Thread( 1000000 * ( omp_get_thread_num(  ) + 1 ) );
	}
	omp_set_num_threads( 1 );
	Thread( 1000000 * ( omp_get_thread_num(  ) + 1 ) );
	omp_set_num_threads( omp_get_max_threads(  ) );
#pragma omp parallel private(maxthr)
	{
		maxthr = omp_get_num_threads(  );
		Thread( 1000000 * ( omp_get_thread_num(  ) + 1 ) );
	}

	elapsed_cyc = PAPI_get_real_cyc(  ) - elapsed_cyc;

	elapsed_us = PAPI_get_real_usec(  ) - elapsed_us;

	if ( !TESTS_QUIET ) {
		printf( "Master real usec   : \t%lld\n", elapsed_us );
		printf( "Master real cycles : \t%lld\n", elapsed_cyc );
	}

	test_pass( __FILE__, NULL, 0 );
	exit( 0 );
}
