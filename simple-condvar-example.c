// jdh CS3010 Fall 2024
// example of condition variables:
// - one after another, the producer thread will put NUM_TO_SEND integers
//   in the shared storage location (value, in ThreadInfo)
// - after the producer has put its last integer in the shared storage
//   location, it will put in one more element, -1, and then exit
// - consumer will read each integer written by the producer
// - when the consumer thread reads a -1, it will exit

#include <stdio.h>
#include <pthread.h>

#define NUM_TO_SEND 10

typedef struct {
  int value;
  pthread_mutex_t lock;
  int isEmpty;
  int isFull;
  pthread_cond_t emptyCondition;
  pthread_cond_t fullCondition;
} ThreadInfo;

void *producer(void *param) {
  int i;
  ThreadInfo *tinfo = (ThreadInfo *) param;

  for (i=0; i<=NUM_TO_SEND; ++i) {
    pthread_mutex_lock(&tinfo->lock);

    while ( ! tinfo->isEmpty)
      pthread_cond_wait(&tinfo->emptyCondition, &tinfo->lock);

    if (i == NUM_TO_SEND) {
      tinfo->value = -1;
      printf("producer writes %d\n", -1);
    } else {
      tinfo->value = i;
      printf("producer writes %d\n", i);
    }

    tinfo->isEmpty = 0;
    pthread_cond_signal(&tinfo->fullCondition);
    pthread_mutex_unlock(&tinfo->lock);
  }

  printf("producer is exiting\n");
  pthread_exit(NULL);
} // producer

//--------------------------------------------------

void *consumer(void *param) {
  ThreadInfo *tinfo = (ThreadInfo *) param;
  int done = 0;
  int val;

  while ( ! done ) {
    pthread_mutex_lock(&tinfo->lock);
    while ( tinfo->isEmpty )
      pthread_cond_wait(&tinfo->fullCondition, &tinfo->lock);

    val = tinfo->value;
    printf("consumer read %d\n", val);
    tinfo->isEmpty = 1;
    if (val == -1)
      done = 1;
    pthread_cond_signal(&tinfo->emptyCondition);
    pthread_mutex_unlock(&tinfo->lock);
  }

  printf("consumer is exiting\n");
  pthread_exit(NULL);
} // consumer()

//--------------------------------------------------

int main() {
  ThreadInfo tinfo;
  pthread_t producerTid, consumerTid;

  tinfo.isEmpty = 1;
  pthread_mutex_init(&tinfo.lock, NULL);
  pthread_cond_init(&tinfo.fullCondition, NULL);
  pthread_cond_init(&tinfo.emptyCondition, NULL);

  pthread_create(&producerTid, NULL, producer, &tinfo);
  pthread_create(&consumerTid, NULL, consumer, &tinfo);

  pthread_join(producerTid, NULL);
  pthread_join(consumerTid, NULL);
  return 0;
}
