#include <string.h>
#include <SDL/SDL.h>
#include <iostream>
#include "Budhabrot.h"
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

using namespace std;

#define BUDHA_SIZE 256
#define THREADS_COUNT 4

int main(int argc, char *argv[]) {
	init_mtrand(THREADS_COUNT, time(0));

	const int width = 1000;
	const int height = 700;
	SDL_Surface *screen;

	float* screenBuffer = new float[width*height];

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

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	float cameraRotationX = 0, cameraRotationY = 0;
	bool redraw = false, redrawFull = false;
	for(;;) {
		float deltaTime = 0.161;


		SDL_Flip(screen);
		SDL_LockSurface(screen);

		SDL_Event ev;

		const float rotateValue = glm::radians(20.0f);

		while(SDL_PollEvent(&ev)) {
			FILE* f;
			switch(ev.type) {
				case SDL_KEYDOWN:
					switch(ev.key.keysym.sym) {
						case 'q':
							cameraRotationX += rotateValue;
							redrawFull = true;
							break;
						case 'e':
							cameraRotationX -= rotateValue;
							redrawFull = true;
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
							gamma *= 2;
							redraw = true;
							break;
						case '4':
							gamma /= 2;
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

		int* pix = (int*)screen->pixels;
		int pth = screen->pitch/sizeof(int);

		// Model matrix : an identity matrix (model will be at the origin)
		modelMatrix      = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices

		// run while the window is open
		glm::vec3 cameraPos;
		cameraPos.x = 2*cosf(cameraRotationY) * cosf(cameraRotationX);
		cameraPos.y = 2*sinf(cameraRotationX);
		cameraPos.z = 2*sinf(cameraRotationY) * cosf(cameraRotationX);

		glm::mat4 MVP = projectionMatrix * glm::lookAt(
				cameraPos,
				glm::vec3(0, 0, 0),
				glm::vec3(0, 0, -1)
		) * modelMatrix;

		int* budha_data = budhabrot.getData();

		glm::vec4 pos, tmp;

		pos.w = 1;
		if(redrawFull) {
			if(zStep == 0) {
				for (int y = 0; y < height; ++y) {
					for(int x = 0; x < width; ++x) {
						screenBuffer[x + y*width] = 0;
					}
				}
			}

			for (int z = zStep; z < BUDHA_SIZE; z += zStepDelta) {
				pos.z = (float)z/BUDHA_SIZE - 0.5f;
				for (int y = 0; y < BUDHA_SIZE; ++y) {
					pos.y = (float)y/BUDHA_SIZE - 0.5f;

					for (int x = 0; x < BUDHA_SIZE; ++x) {
						pos.x = (float)x/BUDHA_SIZE - 0.5f;

						tmp = MVP*pos;

						int xx = (int)(width*(tmp.x + 0.5));
						int yy = (int)(height*(tmp.y + 0.5));

						if(xx >= 0 && yy >= 0 && xx < width && yy < height) {
							screenBuffer[xx + yy*width] += budha_data[x + (y + z*BUDHA_SIZE)*BUDHA_SIZE]/1000.0f/
									glm::length((glm::vec3)pos - cameraPos);
						}

					}
				}
			}

			++zStep;

			if(zStep >= zStepDelta) {
				zStep = 0;
				redrawFull = false;
			}
		}

		if(redraw || redrawFull) {
			float max = 0;

			for (int y = 0; y < height; ++y) {
				for(int x = 0; x < width; ++x) {
					float val = screenBuffer[x + y*width];
					max = std::max(val, max);
				}
			}

			for (int y = 1; y < height - 1; ++y) {
				for(int x = 1; x < width - 1; ++x) {
					float dd = screenBuffer[x + y*width]/max;
					dd += 0.25*screenBuffer[x + y*width + 1]/max;
					dd += 0.25*screenBuffer[x + y*width - 1]/max;
					dd += 0.25*screenBuffer[x + y*width + width]/max;
					dd += 0.25*screenBuffer[x + y*width - width]/max;
					float value = powf(dd, 1.0/gamma)*exposure;

					value = value > 1 ? 1 : value;
					pix[x + y * pth] = HSBtoRGB(value, 0.6, value);
				}
			}

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

  SDL_Quit();

  return 0;
}