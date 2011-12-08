/* This file performs the following test: hardware info and which native events are available */
/** @file component.c
  *	@page papi_component_avail
  * @brief papi_component_avail utility. 
  *	@section  NAME
  *		papi_native_avail - provides detailed information for PAPI native events. 
  *
  *	@section Synopsis
  *
  *	@section Description
  *		papi_component_avail is a PAPI utility program that reports information 
  *		about the components papi was built with.
  *
  *	@section Options
  *		-h help message
  *		-d provide detailed information about each component. 
  *
  *	@section Bugs
  *		There are no known bugs in this utility. 
  *		If you find a bug, it should be reported to the 
  *		PAPI Mailing List at <ptools-perfapi@ptools.org>. 
 */

#include "papi_test.h"
extern int TESTS_QUIET;				   /* Declared in test_utils.c */

#define EVT_LINE 80

typedef struct command_flags
{
	int help;
	int details;
	int named;
	char *name;
} command_flags_t;

static void
print_help( char **argv )
{
	printf( "This is the PAPI component avail program.\n" );
	printf
		( "It provides availability of installed PAPI components.\n" );
	printf( "Usage: %s [options]\n", argv[0] );
	printf( "Options:\n\n" );
	printf( "  --help, -h    print this help message\n" );
	printf( "  -d            print detailed information on each component\n" );
}

static void
parse_args( int argc, char **argv, command_flags_t * f )
{
	int i;

	/* Look for all currently defined commands */
	memset( f, 0, sizeof ( command_flags_t ) );
	for ( i = 1; i < argc; i++ ) {
		if ( !strcmp( argv[i], "-d" ) ) {
			f->details = 1;
		} else if ( strstr( argv[i], "-h" ) )
			f->help = 1;
		else
			printf( "%s is not supported\n", argv[i] );
	}

	/* if help requested, print and bail */
	if ( f->help ) {
		print_help( argv );
		exit( 1 );
	}

}

int
main( int argc, char **argv )
{
	int retval;
	const PAPI_hw_info_t *hwinfo = NULL;
	const PAPI_component_info_t* cmpinfo;
	command_flags_t flags;
	int numcmp, cid;

	tests_quiet( argc, argv );	/* Set TESTS_QUIET variable */

	/* Initialize before parsing the input arguments */
	retval = PAPI_library_init( PAPI_VER_CURRENT );
	if ( retval != PAPI_VER_CURRENT )
		test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

	parse_args( argc, argv, &flags );

	if ( !TESTS_QUIET ) {
		retval = PAPI_set_debug( PAPI_VERB_ECONT );
		if ( retval != PAPI_OK )
			test_fail( __FILE__, __LINE__, "PAPI_set_debug", retval );
	}

	retval =
		papi_print_header
		( "Available components and hardware information.\n", 0, &hwinfo );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_get_hardware_info", 2 );


	numcmp = PAPI_num_components(  );

	for ( cid = 0; cid < numcmp; cid++ ) {
	  cmpinfo = PAPI_get_component_info( cid );

	  printf( "Name:\t\t\t\t%s\n", cmpinfo->name );

	  if ( flags.details ) {
		printf( "Version:\t\t\t%s\n", cmpinfo->version );
		printf( "Number of native events:\t%d\n", cmpinfo->num_native_events);
		printf( "Number of preset events:\t%d\n", cmpinfo->num_preset_events); 
		printf("\n");
	  }
	}


	printf
	  ( "\n--------------------------------------------------------------------------------\n" );
	test_pass( __FILE__, NULL, 0 );
	exit( 0 );
}
