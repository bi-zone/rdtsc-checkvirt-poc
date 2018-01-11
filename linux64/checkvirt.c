#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#define NumberOfCycles 10000
#define VirtualizationLimit (NumberOfCycles/100)
void speculate(const char* detector);
uint64_t timed_read(const char* target);

char* detector;

int rdtsc_vmexit_speculate() {
	for (uint32_t i = 0; i < 256; ++i) {
		_mm_clflush(detector + i*4096);
	}
	
	speculate(detector);
	uint64_t timings[256];
	for (uint32_t i = 0; i < 256; ++i) {
		timings[i] = timed_read(detector + i*4096);
	}

	uint64_t lowestVal=timings[0];
	int lowestInd=0;
	for (uint32_t i = 1; i < 256; ++i) {
		if (timings[i] < lowestVal) {
			lowestVal=timings[i];
			lowestInd=i;
		}
	}
	return lowestInd;
}

int main(int argc, char** argv) {
	detector = malloc(4096 * 257);
	detector = detector - ((uintptr_t)detector & 4095);

	memset(detector, 0, 4096 * 256);
	int stat[0x100];
	for(int i = 0; i < (sizeof(stat)/sizeof(stat[0])); ++i)
		stat[i] = 0;

	for(int i = 0; i < NumberOfCycles; ++i) {
		int res = rdtsc_vmexit_speculate();
		++stat[res];
	}
	//You can uncomment this region if you need detailed statistics
	/*
	for (int i=0; i<256;++i)
	{
		printf("[%02x] %d\n",i,stat[i]);
	}
	*/
	int miss_number=NumberOfCycles;
	for (int i=0; i<8; i++)
	{
		miss_number-=stat[32+i];
	}

	printf("%d out of %d cache misses\n", miss_number, NumberOfCycles);
	if (miss_number <= VirtualizationLimit) {
		printf("RDTSC is not virtualized\n");
	} else{
		printf("RDTSC is virtualized\n");
	}
}