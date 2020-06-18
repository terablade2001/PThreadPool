// MIT License

// Copyright (c) 2017-2020 Vasileios Kon. Pothos (terablade2001)
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

#include "include/STDThreadPool.hpp"

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







STDThreadPool::STDThreadPool() {
	IsCancelled = false;
	remaining_tasks = 0;
	total_threads = 0;
	q = NULL;
	lastq = NULL;
}

STDThreadPool::~STDThreadPool() {
	Shutdown();
}

int STDThreadPool::Initialize(
	void* (*task_func)(void*),
	size_t number_of_threads
) {
	if (number_of_threads <= 0) return -1;
	Shutdown();

	total_threads = number_of_threads;
	fn = task_func;
	IsCancelled = false;
	remaining_tasks = 0;
	q = NULL;
	lastq = NULL;
	threads.clear();
	threads.resize(total_threads);

	for (size_t i = 0; i < total_threads; i++) {
		threads[i] = std::thread(&ThreadHandler, this);
	}
	return 0;
}

void STDThreadPool::AddTask(
	void* data
) {
	SQueue *new_q = new SQueue(data);

	std::lock_guard<std::mutex> lock(q_mtx);
	if (q == NULL) {
		q = new_q;
		lastq = new_q;
	} else {
		lastq->set_next(new_q);
		lastq = new_q;
	}
	remaining_tasks++;
	q_cnd.notify_one();
}



void STDThreadPool::Execute(
	void* data
) {
	if (fn == NULL) return;
	fn(data);
}





void STDThreadPool::Wait() {
	std::unique_lock<std::mutex> lock(q_mtx);
	while ((!IsCancelled) && (remaining_tasks > 0)) {
		q_cnd.wait(lock);
	}
}






void STDThreadPool::Shutdown() {
	if (threads.size() == 0) return;

	IsCancelled = true;

	std::unique_lock<std::mutex> lock(q_mtx);
	q_cnd.notify_all();
	lock.unlock();

	for (auto& t : threads) t.join();

	SQueue *curr_q;
	SQueue *next_q;
	curr_q = q;
	while (curr_q != NULL) {
		next_q = curr_q->get_next();
		curr_q->Shutdown();
		delete curr_q;
		curr_q = next_q;
	}

	total_threads = 0;
	fn = NULL;
	IsCancelled = false;
	remaining_tasks = 0;
	q = NULL;
	lastq = NULL;
	threads.clear();
}


void* STDThreadPool::ThreadHandler(void *data) {
	STDThreadPool &p = *(STDThreadPool*)data;

	while (!p.IsCancelled) {
		std::unique_lock<std::mutex> lock(p.q_mtx);
		while ((!p.IsCancelled) && (p.q == NULL)) {
			p.q_cnd.wait(lock);
		}
		if (p.IsCancelled) {
			lock.unlock();
			break;
		}

		SQueue *curr_q = p.q;
		p.q = p.q->get_next();
		lock.unlock();

		p.fn(curr_q->get_data());
		curr_q->Shutdown();
		delete curr_q;

		lock.lock();
		p.remaining_tasks--;
		p.q_cnd.notify_all();
	}

	return NULL;
}

}; // namespace vkp
