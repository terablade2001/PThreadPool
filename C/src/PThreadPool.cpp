// MIT License

// Copyright (c) 2017 Vasileios Kon. Pothos (terablade2001)
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

#include "include/PThreadPool.hpp"

namespace vkp {

SQueue::SQueue(void* data_) {
	data = data_;
	next = NULL;
}

void SQueue::set_next(SQueue *next_) {
	next = next_;
}

SQueue* SQueue::get_next(void) {
	return next;
}

void* SQueue::get_data(void) {
	return data;
}

void SQueue::Shutdown(void) {
	data = NULL; next = NULL;
}

SQueue::SQueue() {}







PThreadPool::PThreadPool() {
	IsCancelled = false;
	remaining_tasks = 0;
	total_threads = 0;
	q = NULL;
	lastq = NULL;
	threads = NULL;
}

PThreadPool::~PThreadPool() {
	Shutdown();
}
	
void PThreadPool::Initialize(
	void* (*task_func)(void*),
	size_t number_of_threads,
	int priority_
) {
	Shutdown();

	pthread_mutex_init(&q_mtx, NULL);
	pthread_cond_init(&q_cnd, NULL);
	total_threads = number_of_threads;
	fn = task_func;
	IsCancelled = false;
	remaining_tasks = 0;
	q = NULL;
	lastq = NULL;
	priority = priority_;
	threads = new pthread_t[total_threads];

	pthread_attr_init(&attr);

	if (priority >= 0) SetPriority();

	for (size_t i = 0; i < total_threads; i++) {
		pthread_create(&threads[i], &attr, &ThreadHandler, this);
	}
}


void PThreadPool::AddTask(
	void* data
) {
	SQueue *new_q = new SQueue(data);

	pthread_mutex_lock(&q_mtx);
	if (q == NULL) {
		q = new_q;
		lastq = new_q;
	} else {
		lastq->set_next(new_q);
		lastq = new_q;
	}
	remaining_tasks++;
	pthread_cond_signal(&q_cnd);
	pthread_mutex_unlock(&q_mtx);
}



void PThreadPool::Execute(
	void* data
) {
	if (fn == NULL) return;
	fn(data);
}





void PThreadPool::Wait() {
	pthread_mutex_lock(&q_mtx);
	while ((!IsCancelled) && (remaining_tasks > 0)) {
		pthread_cond_wait(&q_cnd, &q_mtx);
	}
	pthread_mutex_unlock(&q_mtx);
}






void PThreadPool::Shutdown() {
	if (threads == NULL) return;

	IsCancelled = true;

	pthread_mutex_lock(&q_mtx);
	pthread_cond_broadcast(&q_cnd);
	pthread_mutex_unlock(&q_mtx);

	for (size_t i = 0; i < total_threads; i++)
		pthread_join(threads[i], NULL);

	SQueue *curr_q;
	SQueue *next_q;
	curr_q = q;
	while (curr_q != NULL) {
		next_q = curr_q->get_next();
		curr_q->Shutdown();
		delete curr_q;
		curr_q = next_q;
	}

	pthread_attr_destroy(&attr);

	total_threads = 0;
	fn = NULL;
	IsCancelled = false;
	remaining_tasks = 0;
	q = NULL;
	lastq = NULL;
	delete[] threads;
	threads = NULL;
}


void PThreadPool::SetPriority() {
	sched_param param;
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	param.sched_priority = priority;
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	pthread_attr_setschedparam(&attr, &param);
}



void* PThreadPool::ThreadHandler(void *data) {
	PThreadPool &p = *(PThreadPool*)data;

	while (!p.IsCancelled) {
		pthread_mutex_lock(&p.q_mtx);
		while ((!p.IsCancelled) && (p.q == NULL)) {
			pthread_cond_wait(&p.q_cnd, &p.q_mtx);
		}
		if (p.IsCancelled) {
			pthread_mutex_unlock(&p.q_mtx);
			break;
		}

		SQueue *curr_q = p.q;
		p.q = p.q->get_next();
		pthread_mutex_unlock(&p.q_mtx);

		p.fn(curr_q->get_data());
		curr_q->Shutdown();
		delete curr_q;

		pthread_mutex_lock(&p.q_mtx);
		p.remaining_tasks--;
		pthread_cond_broadcast(&p.q_cnd);
		pthread_mutex_unlock(&p.q_mtx);
	}

	return NULL;
}

}; // namespace vkp