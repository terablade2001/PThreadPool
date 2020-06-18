// MIT License

// Copyright (c) 2017-2019 Vasileios Kon. Pothos (terablade2001)
// https://github.com/terablade2001/STDThreadPool

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "../src/include/STDThreadPool.hpp"
#include <stdint.h>

#define MAX_THREADS (4)

using namespace vkp;

typedef struct SThread_data {
	int thread_id;
	int val;
} SThread_data;

static SThread_data Thread_Data[MAX_THREADS];

void* Thread_Execute(void* data) {
	SThread_data* ithread_data = (SThread_data*)data;
	int thread_id = ithread_data->thread_id;
	printf("%i_", thread_id);
	return NULL;
}


int main(int argc, char** argv) {
	STDThreadPool TPool;
	TPool.Initialize(Thread_Execute, MAX_THREADS-1);

	for (int tid = 0; tid < MAX_THREADS; tid++) {
		Thread_Data[tid].thread_id = tid;
		Thread_Data[tid].val       =   1;
	}

	for (int tid = MAX_THREADS-1; tid >= 1; tid--)
		TPool.AddTask(&Thread_Data[tid]);

	TPool.Execute(&Thread_Data[0]);

	TPool.Wait();

	TPool.Shutdown();

	return 0;
}
