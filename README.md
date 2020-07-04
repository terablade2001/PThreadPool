# PThreadPool

PThreadPool is an open source library that it defines, creates and utilize 
pThreads thread-pools. The Library comes with example code in `Test_00.cpp`.

**STDThreadPool**
Version (0.006)+ contains also a C++11 std::thread implementation for cases where
pThreads are not an option.
Use definition `VKP_USE_PTHREAD_POOL` or `VKP_USE_STDTHREAD_POOL` to select which one to compile/use.

The following scheduler is used by default, and may not be suitable for some 
platforms. If there are problems, replace it to your choise:
```c++
	sched_param param;
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	param.sched_priority = priority;
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	pthread_attr_setschedparam(&attr, &param);
```

