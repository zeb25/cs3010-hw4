#include <stdio.h>
#include <pthread.h>
#include "sync.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
//Mikey Hayes and Zoe Bell

void* controller(void* arg) {
    ControlInfo* controlInfo = (ControlInfo*)arg;

    // Wait for user input
    getchar();

    // Lock the mutex before modifying the done field
    pthread_mutex_lock(controlInfo->mutex);
    controlInfo->done = true;
    pthread_cond_broadcast(controlInfo->doneCond);
    pthread_mutex_unlock(controlInfo->mutex);
    return NULL;
}

void *sender(void *param) {
    ThreadInfo *tinfo = (ThreadInfo *) param;
    DataInfo *data = tinfo->data;
    ControlInfo *control = tinfo->control;

    while (true) {
        // acquire control lock
        pthread_mutex_lock(control->mutex);
        //check if control wants to terminate
        if (control->done) {
            pthread_mutex_unlock(control->mutex);
            break;
        }
        //release control lock
        pthread_mutex_unlock(control->mutex);

        // spin it
        for (int i = 0; i < 100; i++){
            for (int j = 0; j < 1000; j++)
            {
                //spin cycle
            }
        }
        //acquire mutex lock
        pthread_mutex_lock(data->mutex);
        //if reply is ready
        if (data->replyReady) {
            //print reply
            printf("\nreply: %s\n", data->reply);
            //signal reply has been read
            data->replyReady = false;
        }
        //if data has been signaled as read
        if (!data->messageReady) {
            //randomly choose reciever
            int randomInt = rand() % NUM_RECEIVERS + 1;
            data->recipientID = randomInt;
            //pack up message
            snprintf(data->message, MAX_MESSAGE_LEN, "message for %d", randomInt);
            //signal message is ready to be read
            data->messageReady = true;
            //signal message condition
            pthread_cond_broadcast(data->messageCond);
        }
        //release data lock
        pthread_mutex_unlock(data->mutex);
    }
    printf("you've killed the producer\n");
    pthread_exit(NULL);
}


void *receiver(void *param) {
    ThreadInfo *tinfo = (ThreadInfo *) param;
    DataInfo *data = tinfo->data;
    ControlInfo *control = tinfo->control;
    int receiverID = tinfo->myID;
    char valueRead[MAX_MESSAGE_LEN + 1];

    while (true) {
        //acquire data lock
        pthread_mutex_lock(data->mutex);
        // check if the message is ready
        while (!data->messageReady) {
            pthread_cond_wait(data->messageCond, data->mutex);
        }

        //if data was meant for this thread
        if (data->recipientID == receiverID) {
            //copy data
            strcpy(valueRead, data->message);
            //print copied value
            printf("\n%s \n", valueRead);
            //store reply
            snprintf(data->reply, MAX_MESSAGE_LEN, "%s - read by %d", valueRead, receiverID);
            //signal reply is ready
            data->replyReady = true;
            //signal data has been used
            data->messageReady = false;
        }
        //release data lock
        pthread_mutex_unlock(data->mutex);

        // check if control is done
        //get control lock
        pthread_mutex_lock(control->mutex);
        if (control->done) {
            pthread_mutex_unlock(control->mutex);
            break;
        }
        //release control lock
        pthread_mutex_unlock(control->mutex);
    }

    printf("you've killed receiver %d \n", receiverID);
    pthread_exit(NULL);
}

int main() {
    // Create mutex locks
    pthread_mutex_t dataLock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t controlLock = PTHREAD_MUTEX_INITIALIZER;
    // Create condition variables
    pthread_cond_t messageCond = PTHREAD_COND_INITIALIZER;
    pthread_cond_t replyCond = PTHREAD_COND_INITIALIZER;
    pthread_cond_t doneCond = PTHREAD_COND_INITIALIZER;

    //create ControlInfo for control thread

    ControlInfo control = {
            .done = false,
            .mutex = &controlLock,
            .doneCond = &doneCond
    };

    //create DataInfo for message passing
    DataInfo data = {
            .recipientID = 0,
            .message = "",
            .reply = "",
            .messageReady = false,
            .replyReady = false,
            .mutex = &dataLock,
            .messageCond = &messageCond,
            .replyCond = &replyCond
    };

    // Create ThreadInfo for senders and recievers
    ThreadInfo senderInfo = {
            .myID = 0,
            .data = &data,
            .control = &control
    };

    ThreadInfo receiverInfos[NUM_RECEIVERS];
    for (int i = 0; i < NUM_RECEIVERS; i++) {
        receiverInfos[i].myID = i + 1;
        receiverInfos[i].data = &data;
        receiverInfos[i].control = &control;
    }

    pthread_t senderTid, receiverTids[NUM_RECEIVERS], controllerTid;

    // Create threads for senders and receivers
    pthread_create(&senderTid, NULL, sender, &senderInfo);
    for (int i = 0; i < NUM_RECEIVERS; i++) {

        pthread_create(&receiverTids[i], NULL, receiver, &receiverInfos[i]);
    }
    //create controller thread
    pthread_create(&controllerTid, NULL, controller, &control);

    // Wait for threads to finish
    pthread_join(senderTid, NULL);
    for (int i = 0; i < NUM_RECEIVERS; i++) {
        pthread_join(receiverTids[i], NULL);
    }
    pthread_join(controllerTid, NULL);

    return 0;
}