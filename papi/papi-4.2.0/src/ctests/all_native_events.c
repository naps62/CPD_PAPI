/*
 * File:    all_native_events.c
 * Author:  Haihang You
 *	        you@cs.utk.edu
 * Mods:    <Your name here>
 *          <Your email here>
 */

/* This file hardware info and performs the following test:
		- Start and stop all native events.
    This is a good preliminary way to validate native event tables.
	In its current form this test also stresses the number of 
	events sets the library can handle outstanding. 
*/

#include "papi_test.h"
extern int TESTS_QUIET;				   /* Declared in test_utils.c */
extern unsigned char PENTIUM4;

static int
check_event( int event_code, char *name )
{
	int retval;
	char errstring[PAPI_MAX_STR_LEN];
	long long values;
	int EventSet = PAPI_NULL;

   /* Is there an issue with older machines? */
   /* Disable for now, add back once we can reproduce */
//	if ( PENTIUM4 ) {
//		if ( strcmp( name, "REPLAY_EVENT:BR_MSP" ) == 0 ) {
//			return 1;
//		}
//	}

	retval = PAPI_create_eventset( &EventSet );
	if ( retval != PAPI_OK )
	   test_fail( __FILE__, __LINE__, "PAPI_create_eventset", retval );
   
	retval = PAPI_add_event( EventSet, event_code );
	if ( retval != PAPI_OK ) {
		printf( "Error adding %s\n", name );
		return 0;
	} else {
	  //		printf( "Added %s successfully ", name );
	}

	retval = PAPI_start( EventSet );
	if ( retval != PAPI_OK ) {
		PAPI_perror( retval, errstring, PAPI_MAX_STR_LEN );
		fprintf( stdout, "Error starting %s : %s\n", name, errstring );
	} else {
		retval = PAPI_stop( EventSet, &values );
		if ( retval != PAPI_OK ) {
			PAPI_perror( retval, errstring, PAPI_MAX_STR_LEN );
			fprintf( stdout, "Error stopping %s: %s\n", name, errstring );
			return 0;
		} else {
			printf( "Added and Stopped %s successfully.\n", name );
		}
	}


	retval=PAPI_cleanup_eventset( EventSet );
	if (retval != PAPI_OK ) {
	  test_warn( __FILE__, __LINE__, "PAPI_cleanup_eventset", retval);
	}
	retval=PAPI_destroy_eventset( &EventSet );
	if (retval != PAPI_OK ) {
	  test_warn( __FILE__, __LINE__, "PAPI_destroy_eventset", retval);
	}
	return ( 1 );
}

int
main( int argc, char **argv )
{
	int i, k, add_count = 0, err_count = 0, 
            unc_count = 0, offcore_count = 0;
	int retval;
	PAPI_event_info_t info, info1;
	const PAPI_hw_info_t *hwinfo = NULL;
	char *Intel_i7;
	int event_code;
	const PAPI_component_info_t *s = NULL;
	int numcmp, cid;

	tests_quiet( argc, argv );	/* Set TESTS_QUIET variable */

	retval = PAPI_library_init( PAPI_VER_CURRENT );
	if ( retval != PAPI_VER_CURRENT ) {
	   test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );
	}

	retval = papi_print_header( "Test case ALL_NATIVE_EVENTS: Available "
				    "native events and hardware "
				    "information.\n",
				    0, &hwinfo );
	if ( retval != PAPI_OK ) {
	   test_fail( __FILE__, __LINE__, "PAPI_get_hardware_info", 2 );
	}

	numcmp = PAPI_num_components(  );

	/* we need a little exception processing if it's a Core i7 */
	/* Unfortunately, this test never succeeds... */
	Intel_i7 = strstr( hwinfo->model_string, "Intel Core i7" );

	for ( cid = 0; cid < numcmp; cid++ ) {

		if ( ( s = PAPI_get_component_info( cid ) ) == NULL )
			test_fail( __FILE__, __LINE__, "PAPI_get_substrate_info", 2 );

		/* For platform independence, always ASK FOR the first event */
		/* Don't just assume it'll be the first numeric value */
		i = 0 | PAPI_NATIVE_MASK | PAPI_COMPONENT_MASK( cid );
		PAPI_enum_event( &i, PAPI_ENUM_FIRST );

		do {
			retval = PAPI_get_event_info( i, &info );
			if ( Intel_i7 || ( hwinfo->vendor == PAPI_VENDOR_INTEL ) ) {
				if ( !strncmp( info.symbol, "UNC_", 4 ) ) {
					unc_count++;
					continue;
				}
				if ( !strncmp( info.symbol, "OFFCORE_RESPONSE_0", 18 ) ) {
				        offcore_count++;
					continue;
				}
			}
			if ( s->cntr_umasks ) {
				k = i;
				if ( PAPI_enum_event( &k, PAPI_NTV_ENUM_UMASKS ) == PAPI_OK ) {
					do {
						retval = PAPI_get_event_info( k, &info1 );
						event_code = ( int ) info1.event_code;
						if ( check_event
							 ( event_code, info1.symbol ) )
							add_count++;
						else
							err_count++;
					} while ( PAPI_enum_event( &k, PAPI_NTV_ENUM_UMASKS ) ==
							  PAPI_OK );
				} else {
					event_code = ( int ) info.event_code;
					if ( check_event( event_code, info.symbol ) )
						add_count++;
					else
						err_count++;
				}
			} else {
				event_code = ( int ) info.event_code;
				if ( s->cntr_groups )
					event_code &= ~PAPI_NTV_GROUP_AND_MASK;
				if ( check_event( event_code, info.symbol ) )
					add_count++;
				else
					err_count++;
			}
		} while ( PAPI_enum_event( &i, PAPI_ENUM_EVENTS ) == PAPI_OK );

	}
	printf( "\n\nSuccessfully found and added %d events (in %d eventsets).\n",
			add_count , add_count);
	if ( err_count )
		printf( "Failed to add %d events.\n", err_count );
	if (( unc_count ) || (offcore_count)) {
	   char warning[BUFSIZ];
	   sprintf(warning,"%d Uncore and %d Offcore events were ignored",
		   unc_count,offcore_count);
	   test_warn( __FILE__, __LINE__, warning, 1 );
	}
	if ( add_count > 0 )
		test_pass( __FILE__, NULL, 0 );
	else
		test_fail( __FILE__, __LINE__, "No events added", 1 );
	exit( 1 );
}
