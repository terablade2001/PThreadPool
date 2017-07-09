# cmake -G "CodeLite - MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
# cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
# cmake -DCMAKE_BUILD_TYPE=Release ..
set(CMAKE_C_COMPILER "c:/TDM-GCC-64/bin/x86_64-w64-mingw32-gcc-5.1.0.exe")
set(CMAKE_CXX_COMPILER "c:/TDM-GCC-64/bin/x86_64-w64-mingw32-g++.exe")

cmake_minimum_required(VERSION 2.8.11)

project(PThreadPool)

if (MINGW)
	set(CXX_FLAGS "-std=c++11 -Wall")
endif(MINGW)
if (UNIX)
	set(CXX_FLAGS "-std=c++11 -Wall -Wno-unused-result")
endif(UNIX)

set(LIB__MVECTOR ON)

set(CMAKE_CONFIGURATION_TYPES "Release")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CXX_FLAGS}")
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -L. ")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set (CXX_SRCS
	${CMAKE_CURRENT_LIST_DIR}/C/src/include/PThreadPool.hpp
	${CMAKE_CURRENT_LIST_DIR}/C/src/PThreadPool.cpp
	${CMAKE_CURRENT_LIST_DIR}/C/test/Test_00.cpp
)


add_executable(PThreadPool ${CXX_SRCS})
target_link_libraries(PThreadPool ${LINK_OPTIONS})
