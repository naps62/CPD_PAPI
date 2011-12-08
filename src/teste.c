#include <stdio.h>

union {
	long i;
	double x;
} sqrt_val;

int main() {

	sqrt_val.x = 2;
	sqrt_val.i = (((long)1)<<61) + (sqrt_val.i >> 1) - (((long)1)<<51);
	printf("%lf\n", sqrt_val.x);
	/* calcs the intermidiate mass of object j */
	//invr  = 1.0 / fast_sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
}
