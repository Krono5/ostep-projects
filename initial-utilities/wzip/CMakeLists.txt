cmake_minimum_required(VERSION 3.16)
project(wzip C)

set(CMAKE_C_STANDARD 99)


add_executable(wzip wzip.c)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(wzip PRIVATE Threads::Threads)
