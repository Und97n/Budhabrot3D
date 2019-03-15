//
// Created by zizitop on 14.03.19.
//

#pragma once

#include "defs.h"

class BudhabrotThread;

class Budhabrot {
	int* data;
	const int buffer_size, maxIterations, threadsCount;

	BudhabrotThread* threads;

	mutable int maxValue;

	friend BudhabrotThread;
public:
	Budhabrot(const int buffer_size, const int maxIterations, const int threadsCount);
	~Budhabrot();

	void startWorkers();
	void stopWorkers();
	int getPixel(float exposure, float gamma, int x, int y) const;
};