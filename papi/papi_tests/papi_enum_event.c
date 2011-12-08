#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	int EventCode, retval;
	char EventCodeStr[PAPI_MAX_STR_LEN];

	/* Initialize the library */
	retval = PAPI_library_init(PAPI_VER_CURRENT);

	if (retval != PAPI_VER_CURRENT) {
		fprintf(stderr, "PAPI library init error!\n");
		exit(1);
	}

	EventCode = 0 | 0x40000000;
	do {
		/* Translate the integer code to a string */
		if (PAPI_event_code_to_name(EventCode, EventCodeStr) == PAPI_OK)

			/* Print all the native events for this platform */
			printf("Name: %s\nCode: %x\n", EventCodeStr, EventCode);
		
	} while (PAPI_enum_event(&EventCode, 0) == PAPI_OK);
}
