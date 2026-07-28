#include "include/buffer.h"
#include "stdlib.h"
#include "pthread.h"
#include "stdint.h"

void rbuf_init(RBuf* b) {
  void* ptr = malloc(b->capacity);

  b->ptr = ptr;
  b->read_ptr = ptr;
  b->write_ptr = ptr;
  b->size = 0;
  b->done = 0;

  pthread_mutex_init(&b->mutex, NULL);
  pthread_cond_init(&b->producer, NULL);
  pthread_cond_init(&b->consumer, NULL);
}

void rbuf_push(RBuf *b, uint8_t byte) {
    pthread_mutex_lock(&b->mutex);

    while (b->size == b->capacity)
        pthread_cond_wait(&b->producer, &b->mutex);

    if (b->done) {
      pthread_mutex_unlock(&b->mutex);
      return;
    }

    *b->write_ptr = byte;
    b->write_ptr++;

    if (b->write_ptr == b->ptr + b->capacity)
        b->write_ptr = b->ptr;

    b->size++;

    pthread_cond_signal(&b->consumer);

    pthread_mutex_unlock(&b->mutex);
}

int rbuf_pop(RBuf *b, uint8_t* out) {
  pthread_mutex_lock(&b->mutex);

  while (b->size == 0 && !b->done)
    pthread_cond_wait(&b->consumer, &b->mutex);

  if (b->size == 0) {
    pthread_mutex_unlock(&b->mutex);
    return 0;
  }

  *out = *b->read_ptr;
  b->read_ptr++;

  if (b->read_ptr == b->ptr + b->capacity)
      b->read_ptr = b->ptr;

  b->size--;

  pthread_cond_signal(&b->producer);

  pthread_mutex_unlock(&b->mutex);
  return 1;
}

void rbuf_destroy(RBuf* b){
  free(b->ptr);

  pthread_mutex_destroy(&b->mutex);

  pthread_cond_destroy(&b->producer);
  pthread_cond_destroy(&b->consumer);
}
