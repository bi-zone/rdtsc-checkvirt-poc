#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <string.h>

#define NumberOfCycles 10000
#define VirtualizationLimit (NumberOfCycles/100)

_declspec(naked) void herring() {
	__asm {
		xorps xmm0, xmm0
		sqrtpd xmm0, xmm0
		sqrtpd xmm0, xmm0
		sqrtpd xmm0, xmm0
		sqrtpd xmm0, xmm0
		sqrtpd xmm0, xmm0
		sqrtpd xmm0, xmm0
		sqrtpd xmm0, xmm0
		sqrtpd xmm0, xmm0
		movd eax, xmm0
		lea esp, [esp+eax+4]
		ret
	}
}

_declspec(naked) void __fastcall speculate(const char* detector) {
	__asm {
		mfence
		mov esi, ecx
		call herring
		rdtsc
		and eax, 7
		or eax, 32
		shl eax, 12
		movzx eax, byte ptr [esi+eax]
	}
}

_declspec(naked) uint32_t __fastcall timed_read(const char* target) {
	__asm {
		push edi
		mov edi, ecx
		rdtscp
		mov esi, eax
		mov al, [edi]
		rdtscp
		sub eax, esi
		pop edi
		ret
	}
}

char* detector;

int rdtsc_vmexit_speculate() {
	for (uint32_t i = 0; i < 256; ++i) {
		_mm_clflush(detector + i*4096);
	}
	
	speculate(detector);
	uint32_t timings[256];
	for (int i = 0; i < 256; ++i) {
		timings[i] = timed_read(detector + i*4096);
	}

	uint32_t lowestVal=timings[0];
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
	detector = (char *)malloc(4096 * 259);
	detector = detector + 4096 * 2 - ((uintptr_t)detector & 4095);

	memset(detector, 0, 4096 * 256);

	int stat[0x100];
	for(int i = 0; i < (sizeof(stat)/sizeof(stat[0])); ++i)
		stat[i] = 0;

	for(int i = 0; i < NumberOfCycles; ++i) {
		int res = rdtsc_vmexit_speculate();
		++stat[res];
	}

	for (int i=0; i<256;++i)
	{
		printf("[%02x] %d\n",i,stat[i]);
	}

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
