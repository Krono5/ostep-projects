#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdlib.h>
#include <pthread.h>

#include "request.h"
#include "queue.h"
#include "io_helper.h"


queue_t jobs;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *worker(void *arg);

int thread_pool_init(int n);

#endif //THREADPOOL_H
