#include <stdio.h>
#include <pthread.h>

typedef struct {
    pthread_mutex_t lock;
    int value;
    int valueIsAvailable;
} ThreadInfo;

void *sender(void *param) {
    int numWritten, valueToWrite;
    ThreadInfo *tinfo = (ThreadInfo *) param;

    numWritten = 0;
    valueToWrite = 10;

    pthread_mutex_lock(&tinfo->lock);


    printf("producer writes %d\n", valueToWrite);
    tinfo->value = valueToWrite;
    valueToWrite = valueToWrite + 10;

    numWritten = numWritten + 1;
    tinfo->valueIsAvailable = 1;


    pthread_mutex_unlock(&tinfo->lock);


    printf("producer is exiting\n");
    pthread_exit(NULL);
} // producer

//--------------------------------------------------

void *reciever(void *param) {
    ThreadInfo *tinfo = (ThreadInfo *) param;
    int done = 0;
    int valueRead;

    while ( ! done ) {
        pthread_mutex_lock(&tinfo->lock);
        if (tinfo->valueIsAvailable) {
            valueRead = tinfo->value;
            printf("consumer read %d\n", valueRead);
            tinfo->valueIsAvailable = 0;
            if (valueRead == -1)
                done = 1;
        }
        pthread_mutex_unlock(&tinfo->lock);
    }

    printf("consumer is exiting\n");
    pthread_exit(NULL);
} // consumer()

//--------------------------------------------------

int main() {
    ThreadInfo tinfo;
    pthread_t producerTid, consumerTid;

    pthread_mutex_init(&tinfo.lock, NULL);
    tinfo.valueIsAvailable = 0;

    pthread_create(&producerTid, NULL, sender, &tinfo);
    pthread_create(&consumerTid, NULL, reciever, &tinfo);

    pthread_join(producerTid, NULL);
    pthread_join(consumerTid, NULL);
    return 0;
} // main()

