#pragma once

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>

typedef struct RBuf {
  uint8_t* ptr;
  size_t size;

  size_t capacity;
  uint8_t* write_ptr;
  uint8_t* read_ptr;
  pthread_mutex_t mutex;
  pthread_cond_t producer;
  pthread_cond_t consumer;
  int done;
} RBuf;

void rbuf_push(RBuf* b, uint8_t byte);
int rbuf_pop(RBuf* b, uint8_t* out);
void rbuf_destroy(RBuf* b);
void rbuf_init(RBuf* b);
