#ifndef THREADPOOL_H
#define THREADPOOL_H


#include "queue.h"
void *worker(void *arg);

int thread_pool_init(int n);

#endif //THREADPOOL_H
