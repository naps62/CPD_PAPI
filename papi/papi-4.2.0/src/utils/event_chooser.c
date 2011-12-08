/** @file event_chooser.c
  * @brief papi_event_chooser utility.
  *	@page papi_event_chooser
  *	@section NAME
  *		papi_event_chooser - given a list of named events, 
  *		lists other events that can be counted with them. 
  *
  *	@section Synopsis
  *		papi_event_chooser NATIVE | PRESET <event> <event> ...
  *
  *	@section Description
  *		papi_event_chooser is a PAPI utility program that reports information 
  *		about the current PAPI installation and supported preset events.
  *
  *	@section Options
  *		This utility has no command line options.
  *
  *	@section Bugs
  *		There are no known bugs in this utility. 
  *		If you find a bug, it should be reported to the 
  *		PAPI Mailing List at <ptools-perfapi@ptools.org>. 
 */
#include "papi_test.h"
#include <stdio.h>
#include <stdlib.h>

int EventSet = PAPI_NULL;
int retval;

static char *
is_derived( PAPI_event_info_t * info )
{
	if ( strlen( info->derived ) == 0 )
		return ( "No" );
	else if ( strcmp( info->derived, "NOT_DERIVED" ) == 0 )
		return ( "No" );
	else if ( strcmp( info->derived, "DERIVED_CMPD" ) == 0 )
		return ( "No" );
	else
		return ( "Yes" );
}

static int
add_remove_event( int EventSet, int evt )
{
	int retval;

	if ( ( retval = PAPI_add_event( EventSet, evt ) ) != PAPI_OK ) {
		printf( "Error adding event.\n" );
	} else {
		if ( ( retval = PAPI_remove_event( EventSet, evt ) ) != PAPI_OK )
			printf( "Error removing event.\n" );
	}
	return ( retval );
}

static int
show_event_info( int evt )
{
	int k;
	int retval;
	PAPI_event_info_t info;

	if ( ( retval = PAPI_get_event_info( evt, &info ) ) == PAPI_OK ) {
		printf( "%s\t0x%x\n |%s|\n",
				info.symbol, info.event_code, info.long_descr );

		for ( k = 0; k < ( int ) info.count; k++ )
			if ( strlen( info.name[k] ) )
				printf( " |Register Value[%d]: 0x%-10x  %s|\n", k, info.code[k],
						info.name[k] );
	}
	return ( retval );
}

void
papi_init( int argc, char **argv )
{
	const PAPI_hw_info_t *hwinfo = NULL;

	tests_quiet( argc, argv );	/* Set TESTS_QUIET variable */
	/*for(i=0;i<argc;i++) */

	retval = PAPI_library_init( PAPI_VER_CURRENT );
	if ( retval != PAPI_VER_CURRENT )
		test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

	retval = PAPI_set_debug( PAPI_VERB_ECONT );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_set_debug", retval );

	retval =
		papi_print_header
		( "Event Chooser: Available events which can be added with given events.\n",
		  0, &hwinfo );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_get_hardware_info", 2 );

	retval = PAPI_create_eventset( &EventSet );
	if ( retval != PAPI_OK ) {
		fprintf( stderr, "PAPI_create_eventset error\n" );
		exit( 1 );
	}
}

int
native(  )
{
	int i, j, k;
	int retval, added;
	const PAPI_component_info_t *c = NULL;
	PAPI_event_info_t info;

	j = 0;
	c = PAPI_get_component_info( 0 );

	/* For platform independence, always ASK FOR the first event */
	/* Don't just assume it'll be the first numeric value */
	i = 0 | PAPI_NATIVE_MASK;
	PAPI_enum_event( &i, PAPI_ENUM_FIRST );

	do {
		/* If unit masks are supported, perfmon requires that at least one
		   mask bit be set before adding the event.
		 */
		if ( c->cntr_umasks ) {
			k = i;
			/* modifier = PAPI_NTV_ENUM_UMASKS returns an event code for each
			   unit mask bit defined for this native event. This can be used
			   to get event info for that mask bit. It terminates
			   with PAPI_ENOEVNT at the end of the list.
			 */
			if ( PAPI_enum_event( &k, PAPI_NTV_ENUM_UMASKS ) == PAPI_OK ) {
				if ( ( added = add_remove_event( EventSet, k ) ) == PAPI_OK ) {
					show_event_info( i );
					do {
						retval = PAPI_get_event_info( k, &info );
						if ( retval == PAPI_OK ) {
							printf( "    0x%-10x%s  |%s|\n", info.event_code,
									strchr( info.symbol, ':' ),
									strchr( info.long_descr, ':' ) + 1 );
						}
					} while ( PAPI_enum_event( &k, PAPI_NTV_ENUM_UMASKS ) ==
							  PAPI_OK );
					j++;
				}
			} else {
				if ( ( added = add_remove_event( EventSet, i ) ) == PAPI_OK ) {
					show_event_info( i );
					j++;
				}
			}
		} else {
			if ( ( added = add_remove_event( EventSet, i ) ) == PAPI_OK ) {
				show_event_info( i );
				j++;
			}
		}
		if ( added == PAPI_OK ) {
			/* modifier = PAPI_NTV_ENUM_GROUPS returns event codes with a
			   groups id for each group in which this
			   native event lives, in bits 16 - 23 of event code
			   terminating with PAPI_ENOEVNT at the end of the list.
			 */
			if ( c->cntr_groups ) {
				k = i;
				if ( PAPI_enum_event( &k, PAPI_NTV_ENUM_GROUPS ) == PAPI_OK ) {
					printf( "Groups: " );
					do {
						printf( "%4d",
								( ( k & PAPI_NTV_GROUP_AND_MASK ) >>
								  PAPI_NTV_GROUP_SHIFT ) - 1 );
					} while ( PAPI_enum_event( &k, PAPI_NTV_ENUM_GROUPS ) ==
							  PAPI_OK );
					printf( "\n" );
				}
			}
			printf
				( "-------------------------------------------------------------------------\n" );
		}
	} while ( PAPI_enum_event( &i, PAPI_ENUM_EVENTS ) == PAPI_OK );

	printf
		( "-------------------------------------------------------------------------\n" );
	printf( "Total events reported: %d\n", j );
	test_pass( __FILE__, NULL, 0 );
	exit( 1 );
}

int
preset(  )
{
	int i, j = 0;
	int retval;
	PAPI_event_info_t info;

	printf( "    Name        Code    " );
	printf( "Deriv Description (Note)\n" );

	/* For consistency, always ASK FOR the first event */
	i = 0 | PAPI_PRESET_MASK;
	PAPI_enum_event( &i, PAPI_ENUM_FIRST );

	do {
		retval = PAPI_add_event( EventSet, i );
		if ( retval == PAPI_OK ) {
			if ( PAPI_get_event_info( i, &info ) == PAPI_OK ) {
				printf( "%-13s0x%x  %-5s%s",
						info.symbol,
						info.event_code, is_derived( &info ), info.long_descr );
				if ( info.note[0] )
					printf( " (%s)", info.note );
				printf( "\n" );
			}
			if ( ( retval = PAPI_remove_event( EventSet, i ) ) != PAPI_OK )
				printf( "Error in PAPI_remove_event\n" );
			j++;
		}
	} while ( PAPI_enum_event( &i, PAPI_PRESET_ENUM_AVAIL ) == PAPI_OK );

	printf
		( "-------------------------------------------------------------------------\n" );
	printf( "Total events reported: %d\n", j );
	test_pass( __FILE__, NULL, 0 );
	exit( 1 );
}

int
main( int argc, char **argv )
{
	int i;
	int pevent;

	if ( argc < 3 )
		goto use_exit;

	papi_init( argc, argv );

	for ( i = 2; i < argc; i++ ) {
		PAPI_event_name_to_code( argv[i], &pevent );
		retval = PAPI_add_event( EventSet, pevent );
		if ( retval != PAPI_OK ) {
			fprintf( stderr, "Event %s can't be counted with others\n",
					 argv[i] );
			exit( 1 );
		}
	}

	if ( !strcmp( "NATIVE", argv[1] ) )
		native(  );
	else if ( !strcmp( "PRESET", argv[1] ) )
		preset(  );
	else
		goto use_exit;
	exit( 0 );
  use_exit:
	fprintf( stderr,
			 "Usage: papi_event_chooser NATIVE|PRESET evt1 evt2 ... \n" );
	exit( 1 );
}
