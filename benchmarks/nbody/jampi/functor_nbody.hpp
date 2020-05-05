#pragma once

#include "nbody.hpp"

#include <cmath>

/*
 * Functions prototypes
 */
void calculate_force(Particle* this_particle1, Particle* this_particle2,
					 float* force_x, float* force_y, float* force_z);

void serial_nbody(Particle* d_particles, Particle *output);

void parallel_nbody(Particle* d_particles, Particle *output, int idx_ini, int idx_end);

void compute_serial(Particle* serial_particles, Particle* result_particles, double time_parallel);
