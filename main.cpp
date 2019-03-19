#include <string.h>
#include <SDL/SDL.h>
#include <iostream>
#include "Budhabrot.h"
#include "Renderer.h"
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

using namespace std;

#define BUDHA_SIZE 1024
#define THREADS_COUNT 4

int main(int argc, char *argv[]) {
	init_mtrand(THREADS_COUNT, time(0));

	const int width = 1000;
	const int height = 700;
	SDL_Surface *screen;

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		cout << "Failed SDL_Init " << SDL_GetError() << endl;
		return 1;
	}

	screen=SDL_SetVideoMode(width,height,32,SDL_ANYFORMAT);

	if(screen == NULL) {
		cout << "Failed SDL_SetVideoMode: " << SDL_GetError() << endl;
		SDL_Quit();
		return 1;
	}

	Budhabrot budhabrot(BUDHA_SIZE, 1 << 10, 4);
	budhabrot.startWorkers();

	float exposure = 16;
	float gamma = 1;

	int zStep = 0, zStepDelta = 32;

	bool redraw = false, redrawFull = false;

	Renderer renderer(width, height);
	while(true) {
		bool clearScreenBuffer = false;

		SDL_Flip(screen);
		SDL_LockSurface(screen);

		SDL_Event ev;

		const float rotateValue = glm::radians(30.0f);

		while(SDL_PollEvent(&ev)) {
			FILE* f;
			switch(ev.type) {
				case SDL_KEYDOWN:
					switch(ev.key.keysym.sym) {
						case 'a':
							renderer.rotateCamera(0, rotateValue);
							redrawFull = clearScreenBuffer = true;
							break;
						case 'd':
							renderer.rotateCamera(0, -rotateValue);
							redrawFull = clearScreenBuffer = true;
							break;
						case 'w':
							renderer.rotateCamera(rotateValue, 0);
							redrawFull = clearScreenBuffer = true;
							break;
						case 's':
							renderer.rotateCamera(-rotateValue, 0);
							redrawFull = clearScreenBuffer = true;
							break;
						case ' ':
							redrawFull = clearScreenBuffer = true;
							break;
						case 'i':
						{
							Renderer r(2048, 2048);
							r.rotateCamera(renderer.getCameraRotationX(), renderer.getCameraRotationY());
							r.renderToPng("shit3D.png", budhabrot, exposure, gamma);
						}
							break;
//						case 'a':
//							camera.ProcessKeyboard(LEFT, deltaTime);
//							redraw = true;
//							break;
//						case 'd':
//							redraw = true;
//							camera.ProcessKeyboard(RIGHT, deltaTime);
//							break;
//						case 'w':
//							redraw = true;
//							camera.ProcessKeyboard(FORWARD, deltaTime);
//							break;
//						case 's':
//							redraw = true;
//							camera.ProcessKeyboard(BACKWARD, deltaTime);
//							break;
						case '1':
							exposure *= 2;
							redraw = true;
							break;
						case '2':
							exposure /= 2;
							redraw = true;
							break;
						case '3':
							gamma *= sqrtf(2);
							redraw = true;
							break;
						case '4':
							gamma /= sqrtf(2);
							redraw = true;
							break;
						case 'p':
							budhabrot.stopWorkers();
							break;
						case 'o':
							budhabrot.startWorkers();
							break;

					}
					break;
				case SDL_QUIT: goto done;
			}
		}

		if(clearScreenBuffer) {
			renderer.clearScreenBuffer();

			zStep = 0;
		}

		int* pix = (int*)screen->pixels;
		int pth = screen->pitch/sizeof(int);

		if(redrawFull) {
			renderer.renderToBuffer(budhabrot, zStep, zStepDelta);

			++zStep;

			if(zStep >= zStepDelta) {
				zStep = 0;
				redrawFull = false;
				redraw = true;
			}
		}

		if(redraw || redrawFull) {
			renderer.drawToBitmap(pix, pth, exposure, gamma);
			redraw = false;
		}

//		for(int y = 0; y < BUDHA_SIZE; ++y) {
//			for(int x = 0; x < BUDHA_SIZE; ++x) {
//				Coords pos, dir;
//
////				// Ray direction vector
////				dir.x = camDirX + camPlaneX * cameraX;
////				dir.y = camDirY + camPlaneY * cameraX;
////				dir.z = 0;
////
////				pos.x = BUDHA_SIZE/2;
////				pos.y = BUDHA_SIZE/2;
////				pos.z = 0;
////
////				double dd = rt.traceRay(pos, dir)/100.0f;
//////				double dd = 0;
//////
//////				for(int zz = 0; zz < k; ++zz) {
//////					dd += budhabrot.getPixelValue(x, y, z + zz);
//////				}
////
//////				dd/=k;
////				double value = powf(dd, 1.0/gamma)*exposure;
////
////				value = value > 1 ? 1 : 0;
////				pix[x + y * pth] = value;
////			}
////		}		// X on the interval [-1; 1]
//				double cameraX = 2 * x / (double) (BUDHA_SIZE) - 1;
//

		SDL_UnlockSurface(screen);
  }
  done:
	budhabrot.stopWorkers();

  SDL_Quit();

  exit(0);
}