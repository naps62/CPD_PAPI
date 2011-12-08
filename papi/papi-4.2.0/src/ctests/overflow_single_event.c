/* 
* File:    overflow_single_event.c
* CVS:     $Id: overflow_single_event.c,v 1.48 2010/02/22 18:36:03 jagode Exp $
* Author:  Philip Mucci
*          mucci@cs.utk.edu
* Mods:    <your name here>
*          <your email address>
*/

/* This file performs the following test: overflow dispatch of an eventset
   with just a single event. 

     The Eventset contains:
     + PAPI_FP_INS (overflow monitor)

   - Start eventset 1
   - Do flops
   - Stop and measure eventset 1
   - Set up overflow on eventset 1
   - Start eventset 1
   - Do flops
   - Stop eventset 1
*/

#include "papi_test.h"

#if defined(_WIN32)
#define OVER_FMT	"handler(%d ) Overflow at %p! overflow_vector=0x%x!\n"
#define OUT_FMT		"%-12s : %16I64d%16I64d\n"
#else
#define OVER_FMT	"handler(%d ) Overflow at %p overflow_vector=0x%llx!\n"
#define OUT_FMT		"%-12s : %16lld%16lld\n"
#endif

static int total = 0;				   /* total overflows */

void
handler( int EventSet, void *address, long long overflow_vector, void *context )
{
	( void ) context;

	if ( !TESTS_QUIET ) {
		fprintf( stderr, OVER_FMT, EventSet, address, overflow_vector );
	}

	total++;
}

int
main( int argc, char **argv )
{
	int EventSet = PAPI_NULL;
	long long values[2] = { 0, 0 };
	long long min, max;
	int num_flops = NUM_FLOPS, retval;
	int PAPI_event = 0, mythreshold;
	char event_name[PAPI_MAX_STR_LEN];
	const PAPI_hw_info_t *hw_info = NULL;

	tests_quiet( argc, argv );	/* Set TESTS_QUIET variable */

	retval = PAPI_library_init( PAPI_VER_CURRENT );
	if ( retval != PAPI_VER_CURRENT )
		test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

	hw_info = PAPI_get_hardware_info(  );
	if ( hw_info == NULL )
		test_fail( __FILE__, __LINE__, "PAPI_get_hardware_info", 2 );

	if ( ( !strncmp( hw_info->model_string, "UltraSPARC", 10 ) &&
		   !( strncmp( hw_info->vendor_string, "SUN", 3 ) ) ) ||
		 ( !strncmp( hw_info->model_string, "AMD K7", 6 ) ) ||
		 ( !strncmp( hw_info->vendor_string, "Cray", 4 ) ) ||
		 ( strstr( hw_info->model_string, "POWER3" ) ) ) {
		/* query and set up the right instruction to monitor */
		if ( PAPI_query_event( PAPI_TOT_INS ) == PAPI_OK ) {
			PAPI_event = PAPI_TOT_INS;
		} else {
			test_fail( __FILE__, __LINE__,
					   "PAPI_TOT_INS not available on this Sun platform!", 0 );
		}
	} else {
		/* query and set up the right instruction to monitor */
		if ( PAPI_query_event( PAPI_FP_INS ) == PAPI_OK ) {
			PAPI_event = PAPI_FP_INS;
		} else {
			if ( PAPI_query_event( PAPI_FP_OPS ) == PAPI_OK )
				PAPI_event = PAPI_FP_OPS;
			else
				PAPI_event = PAPI_TOT_INS;
		}
	}

	if ( PAPI_event == PAPI_FP_INS )
		mythreshold = THRESHOLD;
	else
#if defined(linux)
		mythreshold = ( int ) hw_info->mhz * 20000;
#else
		mythreshold = THRESHOLD * 2;
#endif

	retval = PAPI_create_eventset( &EventSet );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_create_eventset", retval );

	retval = PAPI_add_event( EventSet, PAPI_event );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_add_event", retval );

	retval = PAPI_start( EventSet );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_start", retval );

	do_flops( NUM_FLOPS );

	retval = PAPI_stop( EventSet, &values[0] );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_stop", retval );

	retval = PAPI_overflow( EventSet, PAPI_event, mythreshold, 0, handler );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_overflow", retval );

	retval = PAPI_start( EventSet );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_start", retval );

	do_flops( NUM_FLOPS );

	retval = PAPI_stop( EventSet, &values[1] );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_stop", retval );

#if defined(linux) || defined(__ia64__) || defined(_WIN32) || defined(_POWER4)
	num_flops *= 2;
#endif

	if ( !TESTS_QUIET ) {
		if ( ( retval =
			   PAPI_event_code_to_name( PAPI_event, event_name ) ) != PAPI_OK )
			test_fail( __FILE__, __LINE__, "PAPI_event_code_to_name", retval );

		printf
			( "Test case: Overflow dispatch of 1st event in set with 1 event.\n" );
		printf
			( "--------------------------------------------------------------\n" );
		printf( "Threshold for overflow is: %d\n", mythreshold );
		printf( "Using %d iterations of c += a*b\n", NUM_FLOPS );
		printf( "-----------------------------------------------\n" );

		printf( "Test type    : %16d%16d\n", 1, 2 );
		printf( OUT_FMT, event_name, values[0], values[1] );
		printf( "Overflows    : %16s%16d\n", "", total );
		printf( "-----------------------------------------------\n" );

		printf( "Verification:\n" );
/*
	if (PAPI_event == PAPI_FP_INS)
		printf("Row 1 approximately equals %d %d\n", num_flops, num_flops);
	printf("Column 1 approximately equals column 2\n");
*/
		printf( "Row 3 approximately equals %u +- %u %%\n",
				( unsigned ) ( ( values[0] ) / ( long long ) mythreshold ),
				( unsigned ) ( OVR_TOLERANCE * 100.0 ) );

	}

/*
  min = (long long)(values[0]*(1.0-TOLERANCE));
  max = (long long)(values[0]*(1.0+TOLERANCE));
  if ( values[1] > max || values[1] < min )
  	test_fail(__FILE__, __LINE__, event_name, 1);
*/

	min =
		( long long ) ( ( ( double ) values[0] * ( 1.0 - OVR_TOLERANCE ) ) /
						( double ) mythreshold );
	max =
		( long long ) ( ( ( double ) values[0] * ( 1.0 + OVR_TOLERANCE ) ) /
						( double ) mythreshold );
	if ( total > max || total < min )
		test_fail( __FILE__, __LINE__, "Overflows", 1 );

	test_pass( __FILE__, NULL, 0 );
	exit( 1 );
}
