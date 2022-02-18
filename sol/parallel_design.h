#ifndef PARALLEL_DESIGN_H
#define PARALLEL_DESIGN_H

#include <pthread.h>
#include "utils.h"
#include "sack_object.h"
#include "individual.h"

struct shared_memory {
	// the total number of threads used
	int P;
	// the total number of individuals
	int object_count;
	int generations_count;
	// the barrier who synchronizes the threads
	pthread_barrier_t *barrier;
	individual *current_generation, *next_generation;
	const sack_object *objects;
	int sack_capacity;

	shared_memory(int P, int object_count, int generations_count,
				pthread_barrier_t *barrier, individual *current_generation,
				individual *next_generation, const sack_object *objects,
				int sack_capacity)
		: P(P), object_count(object_count),
			generations_count(generations_count), barrier(barrier),
			current_generation(current_generation),
			next_generation(next_generation), objects(objects),
			sack_capacity(sack_capacity)
	{}
};

struct argument_list {
	// thread id
	int id;
	// data used by all threads
	const shared_memory *info_center;
};

void work_for_init_generations(individual *current_generation,
							individual *next_generation, int id, int P,
							int object_count);

void work_for_compute_fitness_function(const sack_object *objects,
		individual *generation, int N, int sack_capacity, int P, int id);

void work_for_copy_individuals(individual *current_generation,
					individual *next_generation, int N, int id, int P);

void work_for_mutate_bit_string(individual *current_generation,
		individual *next_generation, int N, int P, int id, int k);

void work_for_crossover(individual *current_generation,
	individual *next_generation, int count, int cursor, int id, int P,	int k);

void work_for_reindexing(individual *current_generation, int N, int id,	int P);

// the function executed by every thread
void *work(void *arg);

// the algorithm run in parallel (using threads)
void run_algorithm_in_parallel(const sack_object *objects, int object_count,
		int generations_count, int sack_capacity, int P);

#endif
