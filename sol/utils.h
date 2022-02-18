/**
 * Header with functions got from the secvential implementation for designing
 * the algorithm in parallel
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "sack_object.h"
#include "individual.h"

/*
 * Macro for errors checking
 * Example:
 *     int fd = open(file_name, O_RDONLY);
 *     DIE(fd == -1, "open failed");
 */
#define DIE(assertion, call_description)    \
    do {                                    \
        if (assertion) {                    \
            fprintf(stderr, "(%s, %d): ",   \
                    __FILE__, __LINE__);    \
            perror(call_description);       \
            exit(EXIT_FAILURE);             \
        }                                   \
    } while(0)


// reads input from a given file
int read_input(sack_object **objects, int *object_count, int *sack_capacity,
	int *generations_count, int *no_threads, int argc, char *argv[]);

// displays all the objects that can be placed in the sack
void print_objects(const sack_object *objects, int object_count);

// displays all or a part of the individuals in a generation
void print_generation(const individual *generation, int limit);

// displays the individual with the best fitness in a generation
void print_best_fitness(const individual *generation);

// compares two individuals by fitness and then number of objects in the sack
// (to be used with qsort)
int cmpfunc(const void *a, const void *b);

// copies one individual
void copy_individual(const individual *from, const individual *to);

// releases memory for a generation
void free_generation(individual *generation);

// performs a variant of bit string mutation
void mutate_bit_string_1(const individual *ind, int generation_index);

// performs a different variant of bit string mutation
void mutate_bit_string_2(const individual *ind, int generation_index);

// performs one-point crossover
void crossover(individual *parent1, individual *child1, int generation_index);

#endif
