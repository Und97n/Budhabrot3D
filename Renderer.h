//
// Created by zizitop on 17.03.19.
//

#ifndef BUDHABROT_RENDERER_H
#define BUDHABROT_RENDERER_H


#include "Budhabrot.h"

class Renderer {
	const int width, height;
	float* screenBuffer;
	float cameraRotationX, cameraRotationY;
public:
	Renderer(const int frameWidth, const int frameHeight);
	~Renderer();

	void renderToBuffer(Budhabrot& budhabrot, int startZ = 0, int deltaZ = 1);
	void drawToBitmap(int* dest, int destPitch, float exposure, float gamma);

	void rotateCamera(float rotationX, float rotationY);

	void clearScreenBuffer();
	void renderToPng(char* filename, Budhabrot& budhabrot, float exposure, float gamma);

	float getCameraRotationX() const;
	float getCameraRotationY() const;
};


#endif //BUDHABROT_RENDERER_H
