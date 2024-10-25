#include <stdio.h>
#include <pthread.h>
#include "sync.h"


void *controller(void *param){

}
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
    //create mutex lock
    pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
    //create dataInfo's
    DataInfo dataArray[2] = {
            {
                    .recipientID = 12345,
                    .message = "Hello, this is the first message.",
                    .reply = "",
                    .messageReady = true,
                    .replyReady = false,
                    .mutex = &mutex1
            },
            {
                    .recipientID = 67890,
                    .message = "Hello, this is the second message.",
                    .reply = "",
                    .messageReady = true,
                    .replyReady = false,
                    .mutex = &mutex1
            }
    };
    pthread_t producerTid, consumerTid;

    //create producer and consumers
    pthread_create(&producerTid, NULL, sender, &dataArray[0]);
    pthread_create(&consumerTid, NULL, reciever, &dataArray[1]);

    pthread_join(producerTid, NULL);
    pthread_join(consumerTid, NULL);
    return 0;
} // main()

