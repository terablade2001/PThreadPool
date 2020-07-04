# cmake -G "CodeLite - MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
# cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
# cmake -DCMAKE_BUILD_TYPE=Release ..
# set(CMAKE_C_COMPILER "c:/TDM-GCC-64/bin/x86_64-w64-mingw32-gcc-5.1.0.exe")
# set(CMAKE_CXX_COMPILER "c:/TDM-GCC-64/bin/x86_64-w64-mingw32-g++.exe")

cmake_minimum_required(VERSION 2.8.11)

project(ThreadPool)

if (MINGW)
	set(CXX_FLAGS "-std=c++11 -Wall")
endif(MINGW)
if (UNIX)
	set(CXX_FLAGS "-std=c++11 -Wall -Wno-unused-result")
endif(UNIX)

### Selection which type of threadpool to use. ###
set(THREADPOOL_TYPE "VKP_USE_PTHREAD_POOL")
# set(THREADPOOL_TYPE "VKP_USE_STDTHREAD_POOL")

set(CMAKE_CONFIGURATION_TYPES "Release")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS} -D${THREADPOOL_TYPE}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CXX_FLAGS} -D${THREADPOOL_TYPE}")
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -L. ")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set (CXX_SRCS
	${CMAKE_CURRENT_LIST_DIR}/C/src/include/PThreadPool.hpp
	${CMAKE_CURRENT_LIST_DIR}/C/src/PThreadPool.cpp
	${CMAKE_CURRENT_LIST_DIR}/C/src/include/STDThreadPool.hpp
	${CMAKE_CURRENT_LIST_DIR}/C/src/STDThreadPool.cpp
	${CMAKE_CURRENT_LIST_DIR}/C/test/Test_00.cpp
	${CMAKE_CURRENT_LIST_DIR}/C/test/Test_01.cpp
)

add_executable(ThreadPool ${CXX_SRCS})
target_link_libraries(ThreadPool ${LINK_OPTIONS})
