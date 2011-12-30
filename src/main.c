#include <stdio.h>
#include <string.h>
#include <papi.h>

#include "errors.h"
#include "n_body.c"


#define NUM_EVENTS 12			/* Number of events */
int event_set;					/* Array of EventSets, one for each run */
int current_event;				/* Event index to be run in this execution */
long_long event_value;			/* Stores the value of each test run */

/* Array of all the events to run */
int events[NUM_EVENTS] = {
	PAPI_FML_INS,	/* Total number of multiplications */
	PAPI_FDV_INS,	/* Total number of divisions */
	PAPI_TOT_CYC, 	/* Total number of cycles */
	PAPI_TOT_INS,	/* Instructions completed */
	PAPI_LD_INS,	/* number of load instructions */
	PAPI_SR_INS,	/* number of store instructions */

	PAPI_VEC_INS,	/* SIMD instructions */
	PAPI_FP_OPS,	/* Floating point operations */

	PAPI_L1_DCA,	/* L1 data cache accesses */
	PAPI_L1_DCM,	/* L1 data cache misses */
	PAPI_L2_DCA,	/* L2 data cache accesses */
	PAPI_L2_DCM,	/* L2 data cache misses */
};

void arg_read(int argc, char *argv[]);
void papi_run();
void terminate();

int main(int argc, char *argv[]) {
	arg_read(argc, argv);
	papi_run();
	terminate();

	return 0;
}

void arg_read(int argc, char *argv[]) {
	if (argc < 4) {
		fprintf(stderr, "Usage:\n\tn_body N input_file event_index\n");
		exit(-1);
	}

	current_event = atoi(argv[3]);
	N = atoi(argv[1]);
	FILE *in = fopen(argv[2], "r");

	objects	= (struct s_object *) malloc(sizeof(struct s_object) * N);

	int i;
	for(i = 0; i < N; ++i) {
		int x;
		x = fscanf(in, "%lf %lf %lf", &objects[i].pos.x, &objects[i].pos.y, &objects[i].pos.z);
		x = fscanf(in, "%lf %lf %lf", &objects[i].speed.x, &objects[i].speed.y, &objects[i].speed.z);
		x = fscanf(in, "%lf", &objects[i].mass);
	}
}

void terminate() {
	free(objects);
}

void papi_run() {
	int papi_version = PAPI_library_init(PAPI_VER_CURRENT);
	unsigned long long int start_time, end_time;

	safe(papi_version != PAPI_VER_CURRENT && papi_version > 0, ERR_PAPI_INIT);
	safe(papi_version < 0, ERR_PAPI_INIT2);

	/* Creates the event set */
	event_set = PAPI_NULL;
	papi_safe(PAPI_create_eventset(&event_set), ERR_PAPI_EVENT_SET);

	/* get event info to log */
	PAPI_event_info_t info;
	papi_safe(PAPI_get_event_info(events[current_event], &info), ERR_PAPI_GET_EVENT_INFO);

	/* Runs algorithm with the current event */
	papi_safe(PAPI_add_event(event_set, events[current_event]), ERR_PAPI_ADD_EVENT);
	start_time = PAPI_get_virt_usec();
	n_body();
	end_time = PAPI_get_virt_usec();
	fprintf(stdout, "%s\t", info.symbol);
	fprintf(stdout, "\t%llu\t", event_value);
	fprintf(stdout, "\t%llu\n", end_time - start_time);
	papi_safe(PAPI_remove_event(event_set, events[current_event]), ERR_PAPI_REMOVE_EVENT);
}
