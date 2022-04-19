cmake_minimum_required(VERSION 3.16)
project(concurrency_webserver C)

set(CMAKE_C_STANDARD 99)

#add_executable(concurrency_webserver src/wserver.c src/io_helper.h src/request.h)

add_executable(concurrency_webserver src/wserver.c src/io_helper.h src/request.h src/threadpool.c src/queue.c src/queue.h src/threadpool.h2)
