#include "functor_nbody.hpp"

#include "../../../jampi/default_algorithm.h"
#include "../../../jampi/default_thread.h"
#include "../../../jampi/SchedulingPolicy.h"

using namespace parallel;

using TASK = Task<void, Particle*, Particle*, int, int>;
using Scheduler = SchedulingPolicy<CustomAlgorithm, DefaultThread<TASK>>;
Scheduler scheduler;

/* main */
int main (int argc, char** argv) { 
	if(argc < 3) {
		std::cout << "Usage: ./nbody <input_file> <thread_number>\n";
		std::abort();
	}
	
	Particle* particle_array  = nullptr;
	Particle* particle_array2 = nullptr;
	
	Particle* particle_array_serial = nullptr;
	
	int num_trheads = atoi(argv[2]);
	
	FILE *input_data = fopen(argv[1], "r");
	Particle_input_arguments(input_data);

	particle_array  = Particle_array_construct(number_of_particles);
	particle_array2 = Particle_array_construct(number_of_particles);
	Particle_array_initialize(particle_array, number_of_particles);
	
	execute_serial = false;//true;
	if(execute_serial) {
		particle_array_serial = Particle_array_construct(number_of_particles);
		for(int i=0; i<number_of_particles; i++)
			particle_array_serial[i] = particle_array[i];
	}

	long start = wtime();
	
	for(int timestep = 1; timestep <= number_of_timesteps; timestep++) {
#ifdef VERBOSE
		if((timestep % timesteps_between_outputs) == 0) 
			std::cout << "Starting timestep #" << timestep << std::endl;
#endif
		
		int idx_start = 0;
		int idx_end   = idx_start + block_size;
		while(idx_start < number_of_particles) {
			TASK nbody(parallel_nbody, particle_array, particle_array2,  
					   idx_start, (idx_end > number_of_particles ? number_of_particles : idx_end));
			scheduler.spawn_group(nbody, timestep, 0);
			
			idx_start = idx_end;
			idx_end   = idx_start + block_size;
		}

		scheduler.sync_group(timestep);
		
		//Swap arrays
		Particle * tmp  = particle_array;
		particle_array  = particle_array2; //Results must be in particle_array
		particle_array2 = tmp;
	}

	long end = wtime();
	
	double time = (end - start) / 1000000.0;

#ifdef VERBOSE
	printf("Time in seconds: %g s.\n", time);
	printf("Particles per second: %g \n", (number_of_particles*number_of_timesteps)/time);
#else
	printf("nbody;%s;%d;%d;%d;%d;%.8f\n", 
		"THREAD", number_of_particles, block_size, number_of_timesteps, num_trheads, time);
#endif

	if(execute_serial)
		compute_serial(particle_array_serial, particle_array, time);

#ifdef VERBOSE
	//Particle_array_output_xyz(fileptr, particle_array, number_of_particles);
#endif

	particle_array  = Particle_array_destruct(particle_array, number_of_particles);
	particle_array2 = Particle_array_destruct(particle_array2, number_of_particles);
	if(execute_serial)
		particle_array_serial = Particle_array_destruct(particle_array_serial, number_of_particles);

#ifdef VERBOSE
	/*
	if(fclose(fileptr) != 0) {
		std::cout << "ERROR: can't close the output file.\n";
		std::abort();
	}*/
#endif

	return PROGRAM_SUCCESS_CODE;
}
