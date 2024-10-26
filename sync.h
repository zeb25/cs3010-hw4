//
// Created by MasterOfKeys on 10/25/2024.
//

#ifndef UNTITLED6_SYNC_H
#define UNTITLED6_SYNC_H
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define NUM_RECEIVERS 4
#define MAX_MESSAGE_LEN 31
typedef struct {
    bool done;
    // whether or not the program is done
    pthread_mutex_t *mutex; // synchronization for this variable
    pthread_cond_t *doneCond;
} ControlInfo;
typedef struct {
    int recipientID;
    // id of intended recipient
    char message[MAX_MESSAGE_LEN+1]; // the message for the recipient
    char reply[MAX_MESSAGE_LEN+1]; // the reply from the recipient
    bool messageReady;
    // whether data is ready for the recipient
    bool replyReady;
    pthread_mutex_t *mutex;
    pthread_cond_t *messageCond;
    pthread_cond_t *replyCond;
} DataInfo;
typedef struct {
    int myID;
    // whether reply is ready for the sender
    // synchronization for these variables
    // id of sender or receiver
    DataInfo *data;
    // the data
    ControlInfo *control; // control
} ThreadInfo;
#endif //UNTITLED6_SYNC_H
