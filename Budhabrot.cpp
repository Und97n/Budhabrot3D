//
// Created by zizitop on 14.03.19.
//

#include "Budhabrot.h"

#include <cmath>
#include <cstdio>
#include <pthread.h>
#include <signal.h>

/*
   Iterate the Mandelbrot and return TRUE if the point escapes
*/
static int Iterate(double x0,double y0, double _x0, double _y0, int *n,Coords *seq, int maxIterations) {
//	double xs = x0 - 0.25;
//	double xsqr = xs*xs;
//	double ysqr = y0*y0;
//	double dst = xsqr + ysqr;
//
//	if(dst*dst + xs*dst-0.25*ysqr < 0) {
//		return 0;
//	}

	int i;
	double x=_x0,y=_y0,xnew,ynew;

	*n = 0;

	double oldX, oldY;

	for (i = 0; i < maxIterations; i++) {
		// If i is a power of 2 - update old values
		if(!(i & (i - 1))) {
			oldX = x;
			oldY = y;
		}

		xnew = x*x - y*y + x0;
		ynew = 2*x*y + y0;

		seq[i].x = xnew;
		seq[i].y = ynew;
		seq[i].z = sqrt(xnew*xnew + ynew*ynew - x*x - y*y);

		*n = i;
		if(oldX == xnew && oldY == ynew) {
			// If repeats - this is a infinity loop
			return 0;
		}

		if (xnew*xnew + ynew*ynew > 10) {
			return 1;
		}

		x = xnew;
		y = ynew;
	}

	return 0;
}

static void* budhabrotThreadStarter(void* bt);

struct BudhabrotThread {
	pthread_t thread;
	Budhabrot* budhabrot;
	Coords* sequence;
	int threadID;

	void init(Budhabrot* budhabrot, int threadID) {
		this->budhabrot = budhabrot;
		this->threadID = threadID;

		sequence = new Coords[budhabrot->maxIterations];
	}

	void start() {
		if(pthread_create(&thread, NULL, budhabrotThreadStarter, this)) {
			perror("Error creating thread\n");
			exit(1003);
		}
	}

	void stop() {
		pthread_kill(thread, SIGKILL);
	}

	~BudhabrotThread() {
		pthread_kill(thread, SIGKILL);
		delete[] sequence;
	}

	void proceed() {
		int i,ix,iy, iz;
		long tt;
		const int bufferSize = budhabrot->buffer_size;
		int n;

		int NMAX = budhabrot->maxIterations;
		int NMIN = NMAX >> 5;

		int j = 0;

		while(budhabrot->running) {

			for (tt=0; tt < 128; tt++) {
				double x, y, z, w;

				// Choose a random point in same range
				x = 4 * random_double(threadID) - 2;
				y = 4 * random_double(threadID) - 2;
				z = 4 * random_double(threadID) - 2;
//				w = 4 * random_double(threadID) - 2;

				// Determine state of this point, draw if it escapes
				if (Iterate(x, y, z, 0, &n, sequence, budhabrot->maxIterations)) {
					if(n < NMIN) {
						continue;
					}

					int itEnd = std::min(n, NMAX);

					for (i = 0; i < itEnd; i++) {
						ix = 0.3 * bufferSize  * (sequence[i].x + 0.5) + bufferSize/2;
						iy = 0.3 * bufferSize * sequence[i].y + bufferSize/2;
						iz = 0.3 * bufferSize * sequence[i].z;

						if (ix >= 0 && iy >= 0 && ix < bufferSize && iy < bufferSize && iz >= 0 && iz < bufferSize) {
							budhabrot->data[(ix + iz*bufferSize)*bufferSize + iy]++;
						}
					}
				}
			}
		}
	}
};

static void* budhabrotThreadStarter(void* bt) {
	((BudhabrotThread*)bt)->proceed();

	return NULL;
}



Budhabrot::Budhabrot(const int buffer_size, const int maxIterations, const int threadsCount)
	: buffer_size(buffer_size), maxIterations(maxIterations), threadsCount(threadsCount) {

	data = new int[buffer_size*buffer_size*buffer_size];
	threads = new BudhabrotThread[threadsCount];

	for(int i = 0; i < threadsCount; ++i) {
		threads[i].init(this, i);
	}
}

Budhabrot::~Budhabrot() {
	delete[] data;
}

void Budhabrot::startWorkers() {
	for(int i = 0; i < threadsCount; ++i) {
		threads[i].start();
	}
}

void Budhabrot::stopWorkers() {
	running = false;
}