#include <assert.h>
#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"



typedef struct __node_t{
    int value;
    struct __node_t *next;
} node_t;

typedef struct __queue_t{
    node_t *head;
    node_t *tail;
    pthread_mutex_t head_lock, tail_lock;
} queue_t;

void Queue_Init(queue_t *q){
    node_t *tmp = malloc(sizeof (node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
}

void Queue_Enqueue(queue_t *q, int value){
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next = NULL;

    pthread_mutex_lock(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tail_lock);
}

int Queue_Dequeue(queue_t *q, int *value){
    pthread_mutex_lock(&q->head_lock);
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if(new_head == NULL){
        pthread_mutex_unlock(&q->head_lock);
        return -1;
    }
    *value = new_head->value;
    q->head = new_head;
    pthread_mutex_unlock(&q->head_lock);
    free(tmp);
    return 0;
}

queue_t myqueue;

void *mythreadenque(void *arg) {
    Queue_Enqueue(&myqueue, arg);
    return NULL;
}

void *mythreaddequeue(void *arg) {
    int result;
    Queue_Dequeue(&myqueue, &result);
    printf("Dequeue: %d\n", result);
    return NULL;
}

int main(int argc, char* argv[]){
    pthread_t p[20];
    int values[] = {23, 43, 21, 56, 12, 45, 67, 90, 44, 54, 21};

    Queue_Init(&myqueue);

    for (int i = 0; i < 10; ++i) {
        pthread_create(&p[i], NULL, mythreadenque, values[i]);
    }

    for (int i = 10; i < 20; ++i) {
        pthread_create(&p[i], NULL, mythreaddequeue, NULL);
    }

    return 0;
}
