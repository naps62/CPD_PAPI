#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_FLOPS 10000

void dummy(void *c) {
	(void) c;
}

void do_flops(int n) {
	int i;
	volatile double a = 0.5, b = 2.2;
	double c = 0.11;

	for(i = 0; i < n; ++i) {
		c += a * b;
	}

	dummy((void *) &c);
}

int main() {
	int retval, EventSet=PAPI_NULL;
	long_long values[1];

	/* Initialize the PAPI library */
	retval = PAPI_library_init(PAPI_VER_CURRENT);
	if (retval != PAPI_VER_CURRENT) {
		fprintf(stderr, "PAPI library init error!\n");
		exit(1);
	}

	printf("maximum events: %d\n", PAPI_num_counters());

	/* Crete the Event Set */
	if (PAPI_create_eventset(&EventSet) != PAPI_OK)
		handle_error(1);

	/* Add Total Instructions Executed to our EventSet */
	if (PAPI_add_event(EventSet, PAPI_TOT_INS) != PAPI_OK)
		handle_error(1);

	/* Start counting events in the EventSet */
	if (PAPI_start(EventSet) != PAPI_OK)
		handle_error(1);

	/* Defined in tests/do_loops.c in the PAPI source distribution */
	do_flops(NUM_FLOPS);

	/* Read the counting events in the EventSet */
	if (PAPI_read(EventSet, values) != PAPI_OK)
		handle_error(1);

	printf("After reading the coutners: %lld\n", values[0]);

	/* Reset the counting events in the EventSet */
	if (PAPI_reset(EventSet) != PAPI_OK)
		handle_error(1);

	do_flops(NUM_FLOPS);

	/* Add the counters in the EventSet */
	if (PAPI_accum(EventSet, values) != PAPI_OK)
		handle_error(1);

	printf("After adding the counters: %lld\n", values[0]);

	do_flops(NUM_FLOPS);

	/* Stop the counting of events in the EventSet */
	if (PAPI_stop(EventSet, values) != PAPI_OK)
		handle_error;

	printf("After stopping the counters: %lld\n", values[0]);
}
