#include <stdlib.h>
#include <pthread.h>
#include <cmath>

#include "parallel_design.h"

void work_for_init_generations(individual *current_generation,
							individual *next_generation, int id, int P,
							int object_count)
{
	int length = (int) ceil((double) object_count / (double) P);
	int begin = id * length;
	int end = fmin(begin + length - 1, object_count - 1);

	for (int i = begin; i <= end; i++) {
		current_generation[i].fitness = 0;
		current_generation[i].chromosomes = (int*) calloc(object_count,
				sizeof(int));
		current_generation[i].chromosomes[i] = 1;
		current_generation[i].index = i;
		current_generation[i].chromosome_length = object_count;
		// only chromosomes[i] is set (3 lines above)
		current_generation[i].set_chromosomes_count = 1;

		next_generation[i].fitness = 0;
		next_generation[i].chromosomes = (int*) calloc(object_count,
				sizeof(int));
		next_generation[i].index = i;
		next_generation[i].chromosome_length = object_count;
		next_generation[i].set_chromosomes_count = 0;
	}
}

void work_for_compute_fitness_function(const sack_object *objects,
		individual *generation, int N, int sack_capacity, int P, int id)
{
	int weight;
	int profit;

	int length = (int) ceil((double) N / (double) P);
	int begin = id * length;
	int end = fmin(begin + length - 1, N - 1);

	for (int i = begin; i <= end; i++) {
		weight = 0;
		profit = 0;
		generation[i].set_chromosomes_count = 0;

		for (int j = 0; j < generation[i].chromosome_length; j++) {
			if (generation[i].chromosomes[j]) {
				weight += objects[j].weight;
				profit += objects[j].profit;
				generation[i].set_chromosomes_count++;
			}
		}
		generation[i].fitness = (weight <= sack_capacity) ? profit : 0;
	}
}

void work_for_copy_individuals(individual *current_generation,
					individual *next_generation, int N, int id, int P)
{
	int count = 3 * N / 10;
	int length = (int) ceil((double) count / (double) P);
	int begin = id * length;
	int end = fmin(begin + length - 1, count - 1);

	for (int i = begin; i <= end; i++)
		copy_individual(current_generation + i, next_generation + i);
}

void work_for_mutate_bit_string(individual *current_generation,
		individual *next_generation, int N, int P, int id, int k)
{
	int count = N * 2 / 10;
	int length, i;
	int begin, end;

	int mid = P / 2;
	int rem = P % 2;

	int cursor;

	if (id <= mid + rem - 1) {
		// mutate 1
		length = (int) ceil((double) count / (double) (mid + rem));
		begin = id * length;
		end = fmin(begin + length - 1, count - 1);
		cursor = N * 3 / 10;

		for (i = begin; i <= end; i++) {
			copy_individual(current_generation + i,
					next_generation + cursor + i);
			mutate_bit_string_1(next_generation + cursor + i, k);
		}
	} else {
		// mutate 2
		length = (int) ceil((double) count / (double) mid);
		begin = (id - mid - rem) * length;
		end = fmin(begin + length - 1, count - 1);
		cursor = N * 3 / 10 + N * 2 / 10;
		for (i = begin; i <= end; i++) {
			copy_individual(current_generation + i + count,
				next_generation + cursor + i);
			mutate_bit_string_2(next_generation + cursor + i, k);
		}
	}
}

void work_for_crossover(individual *current_generation,
	individual *next_generation, int count, int cursor, int id, int P, int k)
{
	// count is even for sure
	int w = count / 2;
	int length = (int) ceil((double) w / (double) P);
	int begin = id * length;
	int end = fmin(begin + length - 1, w - 1);

	for (int i = begin; i <= end; i++)
		crossover(current_generation + 2 * i,
				next_generation + cursor + 2 * i, k);
}

void work_for_reindexing(individual *current_generation, int N, int id,	int P)
{
	int length = (int) ceil((double) N / (double) P);
	int begin = id * length;
	int end = fmin(begin + length - 1, N - 1);
	for (int i = begin; i <= end; i++)
		current_generation[i].index = i;
}

void *work(void *arg)
{
	argument_list *param_list = (argument_list *) arg;
	int thread_id = param_list->id;
	int P = param_list->info_center->P;
	int N = param_list->info_center->object_count;
	int generations_count = param_list->info_center->generations_count;

	pthread_barrier_t *barrier = param_list->info_center->barrier;

	individual *current_generation = param_list->info_center
					->current_generation;
	individual *next_generation = param_list->info_center->next_generation;

	auto &objects = param_list->info_center->objects;
	auto &sack_capacity = param_list->info_center->sack_capacity;

	work_for_init_generations(current_generation, next_generation,
		thread_id, P, N);
	pthread_barrier_wait(barrier);

	for (int k = 0; k < generations_count; k++) {
		work_for_compute_fitness_function(objects, current_generation, N,
				sack_capacity, P, thread_id);
		pthread_barrier_wait(barrier);

		if (thread_id == 0)
			qsort(current_generation, N, sizeof(individual), cmpfunc);
		pthread_barrier_wait(barrier);

		work_for_copy_individuals(current_generation, next_generation, N,
				thread_id, P);
		pthread_barrier_wait(barrier);

		work_for_mutate_bit_string(current_generation, next_generation, N, P,
				thread_id, k);
		pthread_barrier_wait(barrier);

		int count = N * 3 / 10;
		int cursor = N * 3 / 10 + N * 2 / 10 + N * 2 / 10;

		if (thread_id == 0) {
			if (count % 2 == 1) {
				copy_individual(current_generation + N - 1,
							next_generation + cursor + count - 1);
				count--;
			}
		}
		pthread_barrier_wait(barrier);

		work_for_crossover(current_generation, next_generation, count, cursor,
			thread_id, P, k);
		pthread_barrier_wait(barrier);

		auto tmp = current_generation;
		current_generation = next_generation;
		next_generation = tmp;
		pthread_barrier_wait(barrier);

		work_for_reindexing(current_generation, N, thread_id, P);
		pthread_barrier_wait(barrier);

		if (thread_id == 0) {
			if (k % 5 == 0) {
				print_best_fitness(current_generation);
			}
		}
		pthread_barrier_wait(barrier);
	}

	work_for_compute_fitness_function(objects, current_generation, N,
		sack_capacity, P, thread_id);
	pthread_barrier_wait(barrier);

	if (thread_id == 0) {
		qsort(current_generation, N, sizeof(individual), cmpfunc);
		print_best_fitness(current_generation);
	}

	return NULL;
}

void run_algorithm_in_parallel(const sack_object *objects, int object_count,
		int generations_count, int sack_capacity, int P)
{
	individual *current_generation = (individual*) calloc(object_count,
		sizeof(individual));
	individual *next_generation = (individual*) calloc(object_count,
		sizeof(individual));

	int i, r;
	pthread_barrier_t barrier;
	pthread_t tid[P];
	argument_list parameters[P];
	// data used by all threads
	shared_memory container(P, object_count, generations_count, &barrier,
		current_generation, next_generation, objects, sack_capacity);

	r = pthread_barrier_init(&barrier, NULL, P);
	DIE(r, "barrier init");

	// creating threads
	for (i = 0; i < P; i++) {
		parameters[i].id = i;
		parameters[i].info_center = &container;
		r = pthread_create(&tid[i], NULL, work, &parameters[i]);
		DIE(r, "thread init");
	}

	// waiting for threads
	for (i = 0; i < P; i++) {
		r = pthread_join(tid[i], NULL);
		DIE(r, "thread join");
	}

	r = pthread_barrier_destroy(&barrier);
	DIE(r, "barrier destroy");

	// free resources
	free_generation(current_generation);
	free_generation(next_generation);
}
