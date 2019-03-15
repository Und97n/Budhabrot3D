#include <string.h>
#include <SDL/SDL.h>
#include <iostream>
#include "Budhabrot.h"

using namespace std;

#define BUDHA_SIZE 512
#define THREADS_COUNT 4

int main(int argc, char *argv[]) {
	init_mtrand(THREADS_COUNT, time(0));

	const int width = BUDHA_SIZE + 10;
	const int height = BUDHA_SIZE + 30;
	SDL_Surface *screen;

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		cout << "Failed SDL_Init " << SDL_GetError() << endl;
		return 1;
	}

	screen=SDL_SetVideoMode(width,height,32,SDL_ANYFORMAT);
	if(screen==NULL) {
		cout << "Failed SDL_SetVideoMode: " << SDL_GetError() << endl;
		SDL_Quit();
		return 1;
	}

	Budhabrot budhabrot(BUDHA_SIZE, 1 << 10, 4);
	budhabrot.startWorkers();

	float exposure = 16;
	float gamma = 1;
	int z = BUDHA_SIZE/2;

	for(;;) {
		SDL_Flip(screen);
		SDL_LockSurface(screen);

		SDL_Event ev;

		while(SDL_PollEvent(&ev)) {
			FILE* f;
			switch(ev.type) {
				case SDL_KEYDOWN:
					switch(ev.key.keysym.sym) {
						case '1':
							exposure *= 2;
							break;
						case '2':
							exposure /= 2;
							break;
						case '3':
							gamma *= 2;
							break;
						case '4':
							gamma /= 2;
							break;
						case 'w':
							++z;
							cout << "Z = " << z << "\n";
							break;
						case 's':
							--z;
							cout << "Z = " << z << "\n";
							break;

					}
				break;
				case SDL_QUIT: goto done;
			}
		}

		int* pix = (int*)screen->pixels;
		int pth = screen->pitch/sizeof(int);

		for(int y = 0; y < BUDHA_SIZE; ++y) {
			for(int x = 0; x < BUDHA_SIZE; ++x) {
				int color = 0;

				for(int zz = 0; zz < 10; ++zz) {
					color += budhabrot.getPixel(exposure, gamma, x, y, z + zz);
				}

				pix[x + y * pth] = color;
			}
		}

		SDL_UnlockSurface(screen);
  }
  done:

  SDL_Quit();

  return 0;
}