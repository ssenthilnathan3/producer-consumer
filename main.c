#include "include/buffer.h"

#include <stdio.h>
#include <pthread.h>

#define CAPACITY 2
#define PRODUCERS 10
#define CONSUMERS 12

#define NUM_ITEMS 20

void* producer(void* arg) {
    RBuf* b = (RBuf*)arg;
    for (int i = 0; i < NUM_ITEMS; i++)
        rbuf_push(b, (uint8_t)i);
    return NULL;
}

void* consumer(void* arg) {
    RBuf* b = (RBuf*)arg;
    uint8_t val;
    while (1) {
        if (!rbuf_pop(b, &val)) break;
        printf("Consumed: %u\n", (unsigned)val);
    }
    return NULL;
}

int main(void) {
    RBuf b = { .capacity = CAPACITY };
    rbuf_init(&b);

    pthread_t prods[PRODUCERS];
    pthread_t cons[CONSUMERS];

    for (int i = 0; i < PRODUCERS; i++) {
      pthread_create(&prods[i], NULL, producer, &b);
    }

    for (int i = 0; i < CONSUMERS; i++) {
      pthread_create(&cons[i], NULL, consumer, &b);
    }

    for (int i = 0; i < PRODUCERS; i++) {
      pthread_join(prods[i], NULL);
    }

    pthread_mutex_lock(&b.mutex);
    b.done = 1;
    pthread_cond_broadcast(&b.consumer);
    pthread_mutex_unlock(&b.mutex);

    for (int i = 0; i < CONSUMERS; i++) {
      pthread_join(cons[i], NULL);
    }

    rbuf_destroy(&b);
    return 0;
}
