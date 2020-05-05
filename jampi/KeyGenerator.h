#ifndef PARALLEL_KEYGENERATOR
#define PARALLEL_KEYGENERATOR

#include <functional>
#include <string>
#include <time.h>
#include <stdio.h>

namespace parallel { 

static bool initialized;

int generate_key(int key=0) { 
	if(! initialized) {
		initialized = true;
		
		srand(time(NULL));
	}
	
	int abs = (rand() % 2); 
	abs = (abs==1) ? 1 : -1; 
	return ( ((rand() % 1000000) + (rand() % 999999999)) * abs) + key;
}

};

#endif
