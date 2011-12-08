#include <stdio.h>
#include <unistd.h>
#if defined(_AIX)
#include <sys/wait.h>		 /* ARGH! */
#else
#include <wait.h>
#endif
#include "papi_test.h"

extern int TESTS_QUIET;

int
main( int argc, char **argv )
{
	int retval, pid, status, EventSet = PAPI_NULL;
	long long int values[2];
	PAPI_option_t opt;

        tests_quiet( argc, argv );

	if ( ( retval = PAPI_library_init( PAPI_VER_CURRENT ) ) != PAPI_VER_CURRENT )
		test_fail_exit( __FILE__, __LINE__, "PAPI_library_init", retval );

	if ( ( retval = PAPI_create_eventset( &EventSet ) ) != PAPI_OK )
		test_fail_exit( __FILE__, __LINE__, "PAPI_create_eventset", retval );

	if ( ( retval = PAPI_assign_eventset_component( EventSet, 0 ) ) != PAPI_OK )
		test_fail_exit( __FILE__, __LINE__, "PAPI_assign_eventset_component", retval );

	memset( &opt, 0x0, sizeof ( PAPI_option_t ) );
	opt.inherit.inherit = PAPI_INHERIT_ALL;
	opt.inherit.eventset = EventSet;
	if ( ( retval = PAPI_set_opt( PAPI_INHERIT, &opt ) ) != PAPI_OK ) {
		if ( retval == PAPI_ESBSTR) {
			test_skip( __FILE__, __LINE__, "Inherit not supported by current substrate.\n", retval );
		} else {
			test_fail_exit( __FILE__, __LINE__, "PAPI_set_opt", retval );
		}
	}

	if ( ( retval = PAPI_query_event( PAPI_TOT_CYC ) ) != PAPI_OK )
		test_fail_exit( __FILE__, __LINE__, "PAPI_query_event", retval );

	if ( ( retval = PAPI_add_event( EventSet, PAPI_TOT_CYC ) ) != PAPI_OK )
		test_fail_exit( __FILE__, __LINE__, "PAPI_add_event", retval );

	if ( ( retval = PAPI_query_event( PAPI_FP_INS ) ) != PAPI_OK )
		test_fail_exit( __FILE__, __LINE__, "PAPI_query_event", retval );

	if ( ( retval = PAPI_add_event( EventSet, PAPI_FP_INS ) ) != PAPI_OK )
		test_fail_exit( __FILE__, __LINE__, "PAPI_add_event", retval );

	if ( ( retval = PAPI_start( EventSet ) ) != PAPI_OK )
		test_fail_exit( __FILE__, __LINE__, "PAPI_start", retval );

	pid = fork(  );
	if ( pid == 0 ) {
		do_flops( NUM_FLOPS );
		exit( 0 );
	}
	if ( waitpid( pid, &status, 0 ) == -1 ) {
	  perror( "waitpid()" );
	  exit( 1 );
	}


	if ( ( retval = PAPI_stop( EventSet, values ) ) != PAPI_OK )
		test_fail_exit( __FILE__, __LINE__, "PAPI_stop", retval );

	if (!TESTS_QUIET) {
	   printf( "Test case inherit: parent starts, child works, parent stops.\n" );
	   printf( "------------------------------------------------------------\n" );

	   printf( "Test run    : \t1\n" );
	   printf( "PAPI_FP_INS : \t%lld\n", values[1] );
	   printf( "PAPI_TOT_CYC: \t%lld\n", values[0] );
	   printf( "------------------------------------------------------------\n" );

	   printf( "Verification:\n" );
	   printf( "Row 1 at least %d\n", NUM_FLOPS );
	   printf( "Row 2 greater than row 1\n");
	}

	if ( values[1] < NUM_FLOPS) {
		test_fail( __FILE__, __LINE__, "PAPI_FP_INS", 1 );
	}

	if ( values[0] < values[1]) {
		test_fail( __FILE__, __LINE__, "PAPI_TOT_CYC", 1 );
	}

	test_pass( __FILE__, NULL, 0 );
	exit( 1 );
}
