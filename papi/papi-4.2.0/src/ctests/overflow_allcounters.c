/* 
* File:    overflow_allcounters.c
* CVS:     $Id: overflow_allcounters.c,v 1.16 2011/10/25 19:25:36 vweaver1 Exp $
* Author:  Haihang You
*          you@cs.utk.edu
* Mods:    Vince Weaver
*          vweaver1@eecs.utk.edu
* Mods:    <your name here>
*          <your email address>
*/

/* This file performs the following test: overflow all counters
   to test availability of overflow of all counters
  
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
#define OVER_FMT	"handler(%d ) Overflow at %p! bit=0x%llx \n"
#define OUT_FMT		"%-12s : %16I64d%16I64d\n"
#else
#define OVER_FMT	"handler(%d ) Overflow at %p! bit=0x%llx \n"
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
	long long *values;
	int num_flops, retval, i, j;
	int *events, mythreshold;
	char **names;
	const PAPI_hw_info_t *hw_info = NULL;
	int num_events, *ovt;
	char name[PAPI_MAX_STR_LEN];
	const PAPI_component_info_t *comp_info = NULL;
	int using_perfmon = 0;
	int using_aix = 0;

	tests_quiet( argc, argv );	/* Set TESTS_QUIET variable */

	retval = PAPI_library_init( PAPI_VER_CURRENT );
	if ( retval != PAPI_VER_CURRENT )
		test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

	hw_info = PAPI_get_hardware_info(  );
	if ( hw_info == NULL )
		test_fail( __FILE__, __LINE__, "PAPI_get_hardware_info", retval );

	comp_info = PAPI_get_component_info( 0 );
	if ( hw_info == NULL )
		test_fail( __FILE__, __LINE__, "PAPI_get_component_info", retval );

	if ( strstr( comp_info->name, "perfmon.c" ) )
		using_perfmon = 1;
	if ( strstr( comp_info->name, "aix.c" ) )
		using_aix = 1;


	/* add PAPI_TOT_CYC and one of the events in */
	/* PAPI_FP_INS, PAPI_FP_OPS PAPI_TOT_INS,    */
        /* depending on the availability of the event*/
	/* on the platform */
	EventSet = enum_add_native_events( &num_events, &events, 1 , 1);

	if (!TESTS_QUIET) printf("Trying %d events\n",num_events);

	names =
		( char ** ) calloc( ( unsigned int ) num_events, sizeof ( char * ) );
	for ( i = 0; i < num_events; i++ ) {
		if ( PAPI_event_code_to_name( events[i], name ) != PAPI_OK )
			test_fail( __FILE__, __LINE__, "PAPI_event_code_to_name", retval );
		else {
			names[i] = strdup( name );
			if (!TESTS_QUIET) printf("%i: %s\n",i,names[i]);
		}
	}
	values = ( long long * )
		calloc( ( unsigned int ) ( num_events * ( num_events + 1 ) ),
				sizeof ( long long ) );
	ovt = ( int * ) calloc( ( unsigned int ) num_events, sizeof ( int ) );

#if defined(linux)
	{
		char *tmp = getenv( "THRESHOLD" );
		if ( tmp ) {
			mythreshold = atoi( tmp );
		}
		else if (hw_info->mhz!=0) {
			mythreshold = ( int ) hw_info->mhz * 20000;
		}
		else {
			mythreshold = THRESHOLD;

		}
	}
#else
	mythreshold = THRESHOLD;
#endif

	num_flops = NUM_FLOPS * 2;

	/* initial test to make sure they all work */

       	retval = PAPI_start( EventSet );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_start", retval );

	do_flops( num_flops );

	retval = PAPI_stop( EventSet, values );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_stop", retval );

	/* done with initial test */

	/* keep adding events? */
	for ( i = 0; i < num_events; i++ ) {

	        /* Enable overflow */
		retval = PAPI_overflow( EventSet, events[i], 
					mythreshold, 0, handler );
		if ( retval != PAPI_OK )
			test_fail( __FILE__, __LINE__, 
				   "PAPI_overflow", retval );

       		retval = PAPI_start( EventSet );
		if ( retval != PAPI_OK )
			test_fail( __FILE__, __LINE__, 
				   "PAPI_start", retval );

		do_flops( num_flops );

		retval = PAPI_stop( EventSet, values + ( i + 1 ) * num_events );
		if ( retval != PAPI_OK )
			test_fail( __FILE__, __LINE__, "PAPI_stop", retval );

		/* Disable overflow */
		retval = PAPI_overflow( EventSet, events[i], 
					0, 0, handler );
		if ( retval != PAPI_OK )
			test_fail( __FILE__, __LINE__, 
				   "PAPI_overflow", retval );
		ovt[i] = total;
		total = 0;
	}

	if ( !TESTS_QUIET ) {

		printf( "Test Overflow on %d counters with %d events.\n", num_events,
				num_events );
		printf
			( "---------------------------------------------------------------\n" );
		printf( "Threshold for overflow is: %d\n", mythreshold );
		printf( "Using %d iterations of c += a*b\n", num_flops );
		printf( "-----------------------------------------------\n" );

		printf( "Test type                   : " );
		for ( i = 0; i < num_events + 1; i++ )
			printf( "%16d", i );
		printf( "\n" );
		for ( j = 0; j < num_events; j++ ) {
			printf( "%-27s : ", names[j] );
			for ( i = 0; i < num_events + 1; i++ )
				printf( "%16lld", *( values + j + num_events * i ) );
			printf( "\n" );
		}
		printf( "Overflows                   : %16s", "" );
		for ( i = 0; i < num_events; i++ )
			printf( "%16d", ovt[i] );
		printf( "\n" );
		printf( "-----------------------------------------------\n" );
	}

	/* validation */
	for ( j = 0; j < num_events; j++ ) {
		//printf("Validation: %lld / %d != %d (%lld)\n",
		//       *( values + j + num_events * (j+1) ) ,
		//       mythreshold,
		//       ovt[j],
		//       *(values+j+num_events*(j+1))/mythreshold);
		if ( *( values + j + num_events * ( j + 1 ) ) / mythreshold != ovt[j] ) {
			char error_string[BUFSIZ];

			if ( using_perfmon )
				test_warn( __FILE__, __LINE__,
						   "perfmon substrate handles overflow differently than perf_events",
						   1 );
			else if ( using_aix )
				test_warn( __FILE__, __LINE__,
						   "AIX (pmapi) substrate handles overflow differently than various other substrates",
						   1 );
			else {
				sprintf( error_string,
						 "Overflow value differs from expected %lld / %d != %d (%lld)",
						 *( values + j + num_events * ( j + 1 ) ), mythreshold,
						 ovt[j],
						 *( values + j +
							num_events * ( j + 1 ) ) / mythreshold );
				test_fail( __FILE__, __LINE__, error_string, 1 );
			}
		}
	}

	retval = PAPI_cleanup_eventset( EventSet );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_cleanup_eventset", retval );

	retval = PAPI_destroy_eventset( &EventSet );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_destroy_eventset", retval );

	free( ovt );
	for ( i = 0; i < num_events; i++ )
		free( names[i] );
	free( names );
	free( events );
	free( values );
	test_pass( __FILE__, NULL, 0 );
	exit( 1 );
}
