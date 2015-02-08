//
//  main.c
//  Asn2
//
//  Created by Nathan Gibson on 1/29/15.
//  Copyright (c) 2015 Nathan Gibson. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

struct widget{
    pthread_t producersID;
    int widgetNumber;
    struct widget* next;
};

static struct widget* head;
void enqueue();
struct widget* dequeue();
int isEmpty();
int isFull();
void* producer();
void* consumer();
static int numberOfProducers;
static int numberOfConsumers;
static int numberOfWidgets;
static int timeToWait;
static int numberProduced;
static int numberConsumed;
static struct timespec period;
static pthread_mutex_t produceLock;
static pthread_mutex_t consumeLock;
static pthread_mutex_t isEmptyLock;
static pthread_mutex_t isFullLock;

int main(int argc, const char * argv[]) {
    //intialization
    if (argc != 5) {
        printf("Incorrect arguments supplied\n");
        exit(-1);
    }
    pthread_mutex_init(&produceLock, NULL);
    pthread_mutex_init(&consumeLock, NULL);
    pthread_mutex_init(&isEmptyLock, NULL);
    pthread_mutex_init(&isFullLock, NULL);
    head = NULL;
    numberProduced = 0;
    numberConsumed = 0;
    numberOfProducers = atoi(argv[1]);
    numberOfConsumers = atoi(argv[2]);
    numberOfWidgets = atoi(argv[3]);
    timeToWait = atoi(argv[4]);
    pthread_t arrayOfProducerThreads[numberOfProducers];
    pthread_t arrayOfConsumerThreads[numberOfConsumers];
    period.tv_sec = timeToWait / 1000;
    period.tv_nsec = (timeToWait % 1000) * 1000 * 1000;
    for (int i = 0; i < numberOfProducers; i++) {
        if(pthread_create(&arrayOfProducerThreads[i], NULL, producer, NULL)){
            printf("Error creating producer thread\n");
            exit(3);
        }
    }
    
    for (int i = 0; i < numberOfConsumers; i++) {
        if(pthread_create(&arrayOfConsumerThreads[i], NULL, consumer, NULL)){
            printf("Error creating consumer thread\n");
            exit(3);
        }
     }
    
    for (int i = 0; i < numberOfProducers; i++) {
        if(pthread_join(arrayOfProducerThreads[i], NULL)){
            printf("Error joining producer thread");
            exit(3);
        }
    }
    
    for (int i = 0; i < numberOfConsumers; i++) {
        if(pthread_join(arrayOfConsumerThreads[i], NULL)){
            printf("Error joining consumer thread");
            exit(3);
        }
    }
    return 0;
}

void* producer(){
    pthread_mutex_lock(&produceLock);
    for (int i = 0; i < numberOfWidgets && !isFull(); i++) {
        struct widget* widg = (struct widget*)malloc(sizeof(struct widget));
        widg -> producersID = pthread_self();
        widg -> widgetNumber = i;
        enqueue(widg);
        numberProduced++;
        pthread_mutex_unlock(&produceLock);
        nanosleep(&period, NULL);
    }
    
    pthread_exit(NULL);
    return NULL;
}

void* consumer(){
    while(1){
        pthread_mutex_lock(&consumeLock);
        if((numberConsumed >= numberOfWidgets * numberOfProducers) && isEmpty()){
            pthread_exit(NULL);
        }
        while(isEmpty()){
            pthread_yield_np();
        }
        struct widget* widg = dequeue();
        numberConsumed++;
        printf("consumer (thread id: %d): widget %d from thread %d\n",
               (int)pthread_self(), widg ->widgetNumber,
               (int)widg ->producersID);
        
        free(widg);
        pthread_mutex_unlock(&consumeLock);
        nanosleep(&period, NULL);
        
    }
    return NULL;
}

void enqueue(struct widget* widget){
    struct widget* newNode = (struct widget*)malloc(sizeof(struct widget));
    newNode -> next = NULL;
    newNode -> producersID = pthread_self();
    newNode -> widgetNumber = widget -> widgetNumber;
    if (head == NULL) {
        head = newNode;
    }else{
        struct widget* headCopy = head;
        while(headCopy -> next != NULL){
            headCopy = headCopy->next;
        }
        headCopy->next = newNode;
    }
}

struct widget* dequeue(){
    struct widget* returnValue = (struct widget*)malloc(sizeof(struct widget));
    if (head == NULL) {
        return NULL;
    }
    else if(head -> next == NULL){
        returnValue -> producersID = head -> producersID;
        returnValue -> widgetNumber = head -> widgetNumber;
        returnValue -> next = head -> next;
        head = NULL;
    }
    else{
        returnValue -> producersID = head -> producersID;
        returnValue -> widgetNumber = head -> widgetNumber;
        returnValue -> next = head -> next;
        head = head -> next;
    }
    return returnValue;
}

int isEmpty(){
    pthread_mutex_lock(&isEmptyLock);
    int result = head ? 0 : 1;
    pthread_mutex_unlock(&isEmptyLock);
    return result;
}

int isFull(){
    pthread_mutex_lock(&isFullLock);
    struct widget* returnStatus = (struct widget*)malloc(sizeof(struct widget));
    int result = returnStatus ? 0 : 1;
    if (result == 1) {
        free(returnStatus);
    }
    pthread_mutex_unlock(&isFullLock);
    return result;
    
}