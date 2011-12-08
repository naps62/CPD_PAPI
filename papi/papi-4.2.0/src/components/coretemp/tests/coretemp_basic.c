/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/

/** 
 * @author  Vince Weaver
 *
 * test case for coretemp component 
 * 
 *
 * @brief
 *   Tests basic coretemp functionality
 */

#include <stdio.h>
#include <stdlib.h>
#include "papi_test.h"

#define NUM_EVENTS 1

int main (int argc, char **argv)
{

        int retval,cid,numcmp;
	int EventSet = PAPI_NULL;
	long long values[NUM_EVENTS];
	int code;
	char event_name[PAPI_MAX_STR_LEN];
	int total_events=0;


        /* Set TESTS_QUIET variable */
        tests_quiet( argc, argv );      

	/* PAPI Initialization */
	retval = PAPI_library_init( PAPI_VER_CURRENT );
	if ( retval != PAPI_VER_CURRENT ) {
	   test_fail(__FILE__, __LINE__,"PAPI_library_init failed\n",retval);
	}

	if (!TESTS_QUIET) {
	   printf("Trying all hwmon* events\n");
	}

        numcmp = PAPI_num_components();

	for(cid=0; cid<numcmp; cid++) {

	   if (!TESTS_QUIET) printf("\tComponent %d\n",cid);

	   code = PAPI_NATIVE_MASK | PAPI_COMPONENT_MASK(cid);
           PAPI_enum_event( &code, PAPI_ENUM_FIRST );

	   while ( PAPI_enum_event( &code, PAPI_ENUM_EVENTS ) == PAPI_OK ) {
	      retval = PAPI_event_code_to_name( code, event_name );
	      if ( retval != PAPI_OK ) {
	         test_fail( __FILE__, __LINE__, 
                            "PAPI_event_code_to_name", retval );
	      }

	      if (!strncmp(event_name,"hwmon",5)) {
	         if (!TESTS_QUIET) printf("0x%x %s ",code,event_name);
	     
	         EventSet = PAPI_NULL;

	         retval = PAPI_create_eventset( &EventSet );
	         if (retval != PAPI_OK) {
	            test_fail(__FILE__, __LINE__, 
                              "PAPI_create_eventset()",retval);
		 }

	         retval = PAPI_add_event( EventSet, code );
	         if (retval != PAPI_OK) {
	            test_fail(__FILE__, __LINE__, 
                                 "PAPI_add_event()",retval);
		 }

	         retval = PAPI_start( EventSet);
	         if (retval != PAPI_OK) {
	            test_fail(__FILE__, __LINE__, "PAPI_start()",retval);
		 }

	         retval = PAPI_stop( EventSet, values);
	         if (retval != PAPI_OK) {
	            test_fail(__FILE__, __LINE__, "PAPI_start()",retval);
		 }

	         if (!TESTS_QUIET) printf(" value: %lld\n",values[0]);

	         retval = PAPI_cleanup_eventset( EventSet );
	         if (retval != PAPI_OK) {
	            test_fail(__FILE__, __LINE__, 
                              "PAPI_cleanup_eventset()",retval);
		 }

	         retval = PAPI_destroy_eventset( &EventSet );
	         if (retval != PAPI_OK) {
	            test_fail(__FILE__, __LINE__, 
                              "PAPI_destroy_eventset()",retval);
		 }

	         total_events++;
	      }
	   }
        }

	if (total_events==0) {

	   test_skip(__FILE__,__LINE__,"No coretemp events found",0);
	}

	test_pass( __FILE__, NULL, 0 );
		
	return 0;
}

