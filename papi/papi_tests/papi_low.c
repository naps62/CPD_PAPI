#include <papi.h>
#include <stdio.h>

int main() {
	/* Initialize the PAPI library */
	int retval = PAPI_library_init(PAPI_VER_CURRENT);

	if (retval != PAPI_VER_CURRENT && retval > 0) {
		fprintf(stderr, "PAPI library versin mismatch!\n");
		exit(1);
	}

	if (retval < 0) {
		fprintf(stderr, "Initialization error!\n");
		exit(1);
	}

	printf("PAPI Version Number\n");
	printf("MAJOR:	%d\n", PAPI_MAJOR(retval));
	printf("MINOR:	%d\n", PAPI_MINOR(retval));
	printf("REVISION:	%d\n", PAPI_REVISION(retval));
}
