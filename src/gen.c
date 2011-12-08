/**
 * Generates argv[1] objects for n_body problem saving them to argv[2]
 */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

double nextrand(double max) {
	return (double) ((double)rand() / RAND_MAX) * max;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Wrong parameters. Usage:\n\tgen N output_file\n");
		exit(-1);
	}

	srand((unsigned) time(NULL));
	
	FILE *out = fopen(argv[2], "w");
	int i, max = atoi(argv[1]);
	for(i = 0; i < max; ++i) {
		/* random vals for position, in [0..1000] range */
		fprintf(out, "%lf\t%lf\t%lf\t", nextrand(1000), nextrand(1000), nextrand(1000));
		/* random vals for speed, in [0..2] range */
		fprintf(out, "%lf\t%lf\t%lf\t", nextrand(2), nextrand(2), nextrand(2));
		/* random mass, in [1..1000] range */
		fprintf(out, "%lf\n", nextrand(1000));
	}

	fclose(out);

	return 0;
}
