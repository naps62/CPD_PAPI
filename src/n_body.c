#include <papi.h>

#include "errors.h"

#define DT 1

struct s_coords {
	double x;
	double y;
	double z;
};

struct s_object {
	struct s_coords pos;
	struct s_coords speed;
	double mass;
	struct s_coords aux_pos;
};

int N;
double G = 6.6742867E-11;
union {
	long i;
	double x;
} sqrt_val;
struct s_object *objects;

extern int event_set;
extern long_long event_value;

inline double fast_sqrt(double val) {
	union {
		long i;
		double x;
	} u;
	u.x = val;
	u.i = (((long)1)<<61) + (u.i >> 1) - (((long)1)<<51);
	return u.x;
}

int n_body() {
	/* loop counters */
	int i = 0, j = 0;

	/* aux vectors */
	struct s_coords accel;
	struct s_coords diff;

	/* intermidiate values */
	double invr3, force;

	papi_safe(PAPI_start(event_set), ERR_PAPI_START);
	/* for each planet i */
	for(; i < N; ++i) {

		/* accel starts at 0 for each object */
		accel.x = 0;
		accel.y = 0;
		accel.z = 0;

		/* calculates the force that each object j has on object i */
		for(j = 0; j < N; ++j) {
			/* pos difference between i and j */
			diff.x = objects[j].pos.x - objects[i].pos.x;
			diff.y = objects[j].pos.y - objects[i].pos.y;
			diff.z = objects[j].pos.z - objects[i].pos.z;

			/* calcs the intermidiate mass of object j */
			//computes square root
			sqrt_val.x = diff.x*diff.x + diff.y*diff.y + diff.z*diff.z;
			sqrt_val.i = (((long)1)<<61) + (sqrt_val.i >> 1) - (((long)1)<<51);
			invr3 = 1.0 / (sqrt_val.x * sqrt_val.x * sqrt_val.x);
			force = objects[j].mass * invr3;
			
			/* acceleration that j creates on i */
			accel.x += force * diff.x;
			accel.y += force * diff.y;
			accel.z += force * diff.y;
		}

		/* calcs new position of object i, after DT time. saves to auxiliary array */
		objects[i].aux_pos.x = objects[i].pos.x + DT*objects[i].speed.x + 0.5*DT*DT*G*accel.x;
		objects[i].aux_pos.y = objects[i].pos.y + DT*objects[i].speed.y + 0.5*DT*DT*G*accel.y;
		objects[i].aux_pos.z = objects[i].pos.z + DT*objects[i].speed.z + 0.5*DT*DT*G*accel.z;
		/* updates speed of object i */
		objects[i].speed.x += DT*accel.x;
		objects[i].speed.y += DT*accel.y;
		objects[i].speed.z += DT*accel.z;
	}

	/* after all positions are calculated, updates the old values using the auxiliary array */
	for(i = 0; i < N; ++i) {
		objects[i].pos.x = objects[i].aux_pos.x;
		objects[i].pos.y = objects[i].aux_pos.y;
		objects[i].pos.z = objects[i].aux_pos.z;
	}

	papi_safe(PAPI_stop(event_set, &event_value), ERR_PAPI_STOP);

	return 0;
}
