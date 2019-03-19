//
// Created by zizitop on 14.03.19.
//

#pragma once

#include <cstdint>
#include <png.h>

typedef uint32_t uint;
typedef int64_t bint;

void init_mtrand(int threadsCount, unsigned long long seed);

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long random_int64(int thread_num);

/* generates a random number on [0,1)-real-interval */
double random_double(int thread_num);

int HSBtoRGB(float hue, float saturation, float brightness);

typedef struct {
	float x,y, z;
} Coords;


/*
    save_png
    -----------------------------------------------------

    A simple to save a png with a bit more flexibility. This function
    returns 0 on success otherwise -1.

    - filename:   the path where you want to save the png.
    - width:      width of the image
    - height:     height of the image
    - bitdepth:   how many bits per pixel (e.g. 8).
    - colortype:  PNG_COLOR_TYEP_GRAY
                  PNG_COLOR_TYPE_PALETTE
                  PNG_COLOR_TYPE_RGB
                  PNG_COLOR_TYPE_RGB_ALPHA
                  PNG_COLOR_TYPE_GRAY_ALPHA
                  PNG_COLOR_TYPE_RGBA          (alias for _RGB_ALPHA)
                  PNG_COLOR_TYPE_GA            (alias for _GRAY_ALPHA)
    - pitch:      The stride (e.g. '4 * width' for RGBA).
    - transform:  PNG_TRANSFORM_IDENTITY
                  PNG_TRANSFORM_PACKING
                  PNG_TRANSFORM_PACKSWAP
                  PNG_TRANSFORM_INVERT_MONO
                  PNG_TRANSFORM_SHIFT
                  PNG_TRANSFORM_BGR
                  PNG_TRANSFORM_SWAP_ALPHA
                  PNG_TRANSFORM_SWAP_ENDIAN
                  PNG_TRANSFORM_INVERT_ALPHA
                  PNG_TRANSFORM_STRIP_FILLER
/* ----------------------------------------------------------------------- */

int save_png(char* filename, int width, int height,
					int bitdepth, int colortype,
					unsigned char* data, int pitch, int transform);

