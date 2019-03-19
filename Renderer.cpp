//
// Created by zizitop on 17.03.19.
//

#include "Renderer.h"
#include "Budhabrot.h"
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Renderer::Renderer(const int frameWidth, const int frameHeight): width(frameWidth), height(frameHeight) {
	screenBuffer = new float[frameWidth*frameHeight];
}

void Renderer::renderToBuffer(Budhabrot& budhabrot, int startZ, int deltaZ) {
	glm::vec3 cameraPos;
	cameraPos.x = 3*cosf(cameraRotationY) * cosf(cameraRotationX);
	cameraPos.y = 3*sinf(cameraRotationY) * cosf(cameraRotationX);
	cameraPos.z = 3*sinf(cameraRotationX);

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 modelMatrix = glm::mat4(1.5f);

	glm::mat4 MVP = projectionMatrix * glm::lookAt(
			cameraPos,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 0, -1)
	) * modelMatrix;

	int** budha_data = budhabrot.getData();
	const int bufferSize = budhabrot.getBufferSize();
	glm::vec4 pos, tmp;

	pos.w = 1;

#pragma omp paralell for
	for (int z = startZ; z < bufferSize; z += deltaZ) {
		pos.z = (float) z / bufferSize - 0.5f;

		for (int y = 0; y < bufferSize; ++y) {
			pos.y = (float) y / bufferSize - 0.5f;

			for (int x = 0; x < bufferSize; ++x) {
				float nv = budha_data[z][x + y * bufferSize];

				if (nv == 0) {
					continue;
				}

				pos.x = (float) x / bufferSize - 0.5f;

				tmp = MVP * pos;

				int xx = (int) (width * (tmp.x + 0.5));
				int yy = (int) (height * (tmp.y + 0.5));

				if (xx >= 0 && yy >= 0 && xx < width && yy < height) {
					screenBuffer[xx + yy * width] = std::max(nv, screenBuffer[xx + yy * width]);
				}
			}
		}
	}
}

void Renderer::drawToBitmap(int* dest, int destPitch, float exposure, float gamma) {
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
//					dd += 0.25*screenBuffer[x + y*width + 1]/max;
//					dd += 0.25*screenBuffer[x + y*width - 1]/max;
//					dd += 0.25*screenBuffer[x + y*width + width]/max;
//					dd += 0.25*screenBuffer[x + y*width - width]/max;
//
//
//					dd += 0.1*screenBuffer[x + y*width + width + 1]/max;
//					dd += 0.1*screenBuffer[x + y*width - width - 1]/max;
//					dd += 0.1*screenBuffer[x + y*width + width - 1]/max;
//					dd += 0.1*screenBuffer[x + y*width - width + 1]/max;

			float value = powf(dd, 1.0f/gamma)*exposure;

			value = value > 1 ? 1 : value;
			dest[x + y * destPitch] = HSBtoRGB(value, 0.6, value);
		}
	}
}

void Renderer::rotateCamera(float rotationX, float rotationY) {
	cameraRotationX += rotationX;
	cameraRotationY += rotationY;

	float border = 0.999f*M_PI/2;

	if (cameraRotationX > border) {
		cameraRotationX = border;
	} else if (cameraRotationX < -border) {
		cameraRotationX = -border;
	}
}


void Renderer::clearScreenBuffer() {
	for (int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			screenBuffer[x + y*width] = 0;
		}
	}
}

void Renderer::renderToPng(char* filename, Budhabrot& budhabrot, float exposure, float gamma) {
	int* pdata = new int[width*height];

	std::cerr << "Making PNG\n";

	renderToBuffer(budhabrot);
	drawToBitmap(pdata, width, exposure, gamma);

	std::cerr << ("Pixels calculated. Writing.\n");

	save_png(filename, width, height, 8, PNG_COLOR_TYPE_RGBA, (unsigned char *) pdata, width * sizeof(int), PNG_TRANSFORM_BGR);

	std::cerr << ("PNG done\n");

	delete[] pdata;
}

float Renderer::getCameraRotationY() const {
	return cameraRotationY;
}

float Renderer::getCameraRotationX() const {
	return cameraRotationX;
}

Renderer::~Renderer() {
	delete[] screenBuffer;
}