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

	friend BudhabrotThread;
	bool running;
public:
	Budhabrot(const int buffer_size, const int maxIterations, const int threadsCount);
	~Budhabrot();

	void startWorkers();
	void stopWorkers();

	inline int* getData() {
		return data;
	}

	inline int getBufferSize() const {
		return buffer_size;
	}
};