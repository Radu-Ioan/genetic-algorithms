#include <stdlib.h>

#include "utils.h"
#include "parallel_design.h"

int main(int argc, char *argv[]) {
	// array with all the objects that can be placed in the sack
	sack_object *objects = NULL;

	// number of objects
	int object_count = 0;

	// maximum weight that can be carried in the sack
	int sack_capacity = 0;

	// number of generations
	int generations_count = 0;

	// number of threads used
	int P = 1;

	if (!read_input(&objects, &object_count, &sack_capacity,
			&generations_count, &P, argc, argv)) {
		return 0;
	}

	run_algorithm_in_parallel(objects, object_count, generations_count,
		sack_capacity, P);

	free(objects);

	return 0;
}
