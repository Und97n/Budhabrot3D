//
// Created by ??? on 24.02.19.
//

#include <stdlib.h>
#include <cmath>

#include "defs.h"

#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL /* Most significant 33 bits */
#define LM 0x7FFFFFFFULL /* Least significant 31 bits */

/* The array for the state vector */
static unsigned long long** mt_arr;
/* mti==NN+1 means mt[NN] is not initialized */
static int* mti_arr;

void init_mtrand(int threadsCount, unsigned long long seed) {
	mti_arr = static_cast<int *>(malloc(sizeof(int) * threadsCount));
	mt_arr = static_cast<unsigned long long int **>(malloc(sizeof(unsigned long long*) * threadsCount));

	for(int i = 0; i < threadsCount; ++i) {
		mt_arr[i] = static_cast<unsigned long long int *>(malloc(sizeof(unsigned long long) * NN));
		unsigned long long* mt = mt_arr[i];
		mt[0] = seed;

		int mti;

		for (mti = 1; mti < NN; mti++) {
			mt[mti] =  (6364136223846793005ULL * (mt[mti-1] ^ (mt[mti-1] >> 62)) + mti);
		}

		mti_arr[i] = mti;
	}
}

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long random_int64(int thread_num) {
	int i;
	unsigned long long x;
	static unsigned long long mag01[2]={0ULL, MATRIX_A};

	unsigned long long* mt = mt_arr[thread_num];

	if (mti_arr[thread_num] >= NN) { /* generate NN words at one time */
		for (i=0;i<NN-MM;i++) {
			x = (mt[i]&UM)|(mt[i+1]&LM);
			mt[i] = mt[i+MM] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
		}

		for (;i<NN-1;i++) {
			x = (mt[i]&UM)|(mt[i+1]&LM);
			mt[i] = mt[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
		}

		x = (mt[NN-1]&UM)|(mt[0]&LM);
		mt[NN-1] = mt[MM-1] ^ (x>>1) ^ mag01[(int)(x&1ULL)];

		mti_arr[thread_num] = 0;
	}

	x = mt[mti_arr[thread_num]++];

	x ^= (x >> 29) & 0x5555555555555555ULL;
	x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
	x ^= (x << 37) & 0xFFF7EEE000000000ULL;
	x ^= (x >> 43);

	return x;
}

/* generates a random number on [0,1)-real-interval */
double random_double(int thread_num) {
	return (random_int64(thread_num) >> 11) * (1.0/9007199254740992.0);
}

int HSBtoRGB(float hue, float saturation, float brightness) {
	int r = 0, g = 0, b = 0;
	if (saturation == 0) {
		r = g = b = (int) (brightness * 255.0f + 0.5f);
	} else {
		float h = (hue - (float)floorf(hue)) * 6.0f;
		float f = h - (float)floorf(h);
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));
		switch ((int) h) {
			case 0:
				r = (int) (brightness * 255.0f + 0.5f);
				g = (int) (t * 255.0f + 0.5f);
				b = (int) (p * 255.0f + 0.5f);
				break;
			case 1:
				r = (int) (q * 255.0f + 0.5f);
				g = (int) (brightness * 255.0f + 0.5f);
				b = (int) (p * 255.0f + 0.5f);
				break;
			case 2:
				r = (int) (p * 255.0f + 0.5f);
				g = (int) (brightness * 255.0f + 0.5f);
				b = (int) (t * 255.0f + 0.5f);
				break;
			case 3:
				r = (int) (p * 255.0f + 0.5f);
				g = (int) (q * 255.0f + 0.5f);
				b = (int) (brightness * 255.0f + 0.5f);
				break;
			case 4:
				r = (int) (t * 255.0f + 0.5f);
				g = (int) (p * 255.0f + 0.5f);
				b = (int) (brightness * 255.0f + 0.5f);
				break;
			case 5:
				r = (int) (brightness * 255.0f + 0.5f);
				g = (int) (p * 255.0f + 0.5f);
				b = (int) (q * 255.0f + 0.5f);
				break;
		}
	}
	return 0xff000000 | (r << 16) | (g << 8) | (b << 0);
}