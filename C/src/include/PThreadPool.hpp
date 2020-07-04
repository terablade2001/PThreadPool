// MIT License

// Copyright (c) 2017-2019 Vasileios Kon. Pothos (terablade2001)
// https://github.com/terablade2001/PThreadPool

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

#pragma once
#include "ThreadPoolSetup.hpp"

#ifdef VKP_USE_PTHREAD_POOL

#include <cstdlib>
#include <cstdio>
#include <pthread.h>

#ifdef __USE_GNU
	#include <sched.h>
#endif

#ifndef __FNAME__
	#include <cstring>
	#define __FNAMEBSL__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
	#define __FNAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FNAMEBSL__ )
#endif


namespace vkp {

class SQueue {
public:
	SQueue(void* data_);
	void set_next(SQueue *next_);
	SQueue* get_next(void);
	void* get_data();
	void Shutdown();
private:
	SQueue();
	void* data;
	SQueue *next;
};

class PThreadPool {
public:
	PThreadPool();
	~PThreadPool();

	int Initialize(
		void* (*task_func)(void*),
		size_t number_of_threads,
		int priority_ = 90,
		int scheduler_ = SCHED_FIFO
	);
	void AddTask(void* data);
	void Execute(void* data);
	void Wait(void);
	void Shutdown(void);
#ifdef __USE_GNU
	int SetAffinity(int core_id, int thread_id_ = -1);
#endif
private:
	void SetPriority();
	static void* ThreadHandler(void *data);
	void* (*fn)(void *);
	bool IsCancelled;
	size_t remaining_tasks;
	size_t total_threads;
	SQueue *q;
	SQueue *lastq;
	int priority;
	int scheduler;
	pthread_mutex_t q_mtx;
	pthread_cond_t q_cnd;
	pthread_attr_t attr;
	pthread_t *threads;
};

}; //namespace vkp

#endif // VKP_USE_PTHREAD_POOL
