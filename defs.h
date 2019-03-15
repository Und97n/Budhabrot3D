//
// Created by zizitop on 14.03.19.
//

#pragma once

#include <cstdint>

typedef uint32_t uint;
typedef int64_t bint;

void init_mtrand(int threadsCount, unsigned long long seed);

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long random_int64(int thread_num);

/* generates a random number on [0,1)-real-interval */
double random_double(int thread_num);

int HSBtoRGB(float hue, float saturation, float brightness);

