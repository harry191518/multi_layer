#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"
#include "timecount.h"

unsigned long long int begin, end, total;
unsigned long long int *clock;

inline unsigned long long int rdtsc() {
	unsigned long long int x;
	asm   volatile ("rdtsc" : "=A" (x));
	return x;
}

void CountClock() {
	unsigned int i;
	unsigned int* NumCntClock = (unsigned int* ) malloc (50 * sizeof(unsigned int ));
	for(i = 0; i < 50; i++) NumCntClock[i] = 0;

	unsigned long long MinClock = 10000000, MaxClock = 0;
	for(i = 0; i < num_query; i++) {
        if(clock[i] == 10000000)
            continue;
		if(clock[i] > MaxClock) MaxClock = clock[i];
		if(clock[i] < MinClock) MinClock = clock[i];
		if(clock[i] / 100 < 50) NumCntClock[clock[i] / 100]++;
		else NumCntClock[49]++;
	}
	printf("(MaxClock, MinClock) = (%5llu, %5llu)\n", MaxClock, MinClock);

	return;
}
