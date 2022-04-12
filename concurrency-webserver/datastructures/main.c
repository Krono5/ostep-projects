#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"

typedef struct __counter_t{
    int value;
    pthread_mutex_t lock;
} counter_t;

void init(counter_t *c){
    c->value = 0;
    pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c){
    pthread_mutex_lock(&c->lock);
    c->value++;
    pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c){
    pthread_mutex_lock(&c->lock);
    c->value--;
    pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c){
    pthread_mutex_lock(&c->lock);
    int rc = c->value;
    pthread_mutex_unlock(&c->lock);
    return rc;
}

int main(int argc, char* argv[]){

    return 0;
}
