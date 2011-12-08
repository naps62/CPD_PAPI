/*
 * A simple example for the use of PAPI, using PAPI_ipc
 * -Kevin London
 */

#include "papi_test.h"


#ifdef _WIN32
#define INDEX 275			 /* This fixes stack overflow on Windows.
							    It shouldn't be needed using the Windows /F switch
							    but I can't seem to make that switch work */
#else
#define INDEX 500
#endif
extern int TESTS_QUIET;				   /* Declared in test_utils.c */


int
main( int argc, char **argv )
{
	extern void dummy( void * );
	float matrixa[INDEX][INDEX], matrixb[INDEX][INDEX], mresult[INDEX][INDEX];
	float real_time, proc_time, ipc;
	long long ins;
	int retval;
	int i, j, k;

	tests_quiet( argc, argv );	/* Set TESTS_QUIET variable */


	/* Initialize the Matrix arrays */
	for( i = 0; i < INDEX; i++ ) {
	   for( j= 0; j < INDEX; j++ ) {
	       mresult[i][j] = 0.0;
	       matrixa[i][j] = matrixb[i][j] = ( float ) rand(  ) * ( float ) 1.1;
	   }
	}

	/* Setup PAPI library and begin collecting data from the counters */
	if ( ( retval = PAPI_ipc( &real_time, &proc_time, &ins, &ipc ) ) < PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_ipc", retval );

	/* Matrix-Matrix multiply */
	for ( i = 0; i < INDEX; i++ )
		for ( j = 0; j < INDEX; j++ )
			for ( k = 0; k < INDEX; k++ )
				mresult[i][j] = mresult[i][j] + matrixa[i][k] * matrixb[k][j];

	/* Collect the data into the variables passed in */
	if ( ( retval = PAPI_ipc( &real_time, &proc_time, &ins, &ipc ) ) < PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_ipc", retval );
	dummy( ( void * ) mresult );

	if ( !TESTS_QUIET ) {
		printf( "Real_time: %f Proc_time: %f Total ins: ", real_time,
				proc_time );
		printf( LLDFMT, ins );
		printf( " IPC: %f\n", ipc );
	}
   
           /* This should not happen unless the optimizer */
           /* gets too good                               */
        if (ins < INDEX*INDEX) {
	   test_fail( __FILE__, __LINE__, "Instruction count too low.", 
		      5 );
	}
           /* Something is broken, or else you have a really */
           /* slow processor                                 */
        if (ipc<0.01 ) {
	   test_fail( __FILE__, __LINE__, "IPC equals zero.", 
		      5 );
	}
   
	test_pass( __FILE__, NULL, 0 );
	exit( 1 );
}
