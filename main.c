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
void* producer();
void* consumer();
static int numberOfProducers;
static int numberOfConsumers;
static int numberOfWidgets;
static int timeToWait;
static int numberProduced;
static pthread_mutex_t produceLock;
static pthread_mutex_t consumeLock;


int main(int argc, const char * argv[]) {
    //intialization
    pthread_mutex_init(&produceLock, NULL);
    pthread_mutex_init(&consumeLock, NULL);
    head = NULL; //queue is initially empty
    numberProduced = 0;
    numberOfProducers = atoi(argv[1]);
    numberOfConsumers = atoi(argv[2]);
    numberOfWidgets = atoi(argv[3]);
    timeToWait = atoi(argv[4]);
    pthread_t arrayOfProducerThreads[numberOfProducers];
    pthread_t arrayOfConsumerThreads[numberOfConsumers];
    
    
    for (int i = 0; i < numberOfProducers; i++) {
        pthread_create(&arrayOfProducerThreads[i], NULL, producer, NULL);
    }
    for (int i = 0; i < numberOfConsumers; i++) {
        pthread_create(&arrayOfConsumerThreads[i], NULL, consumer, NULL);
     }
    for (int i = 0; i < numberOfProducers; i++) {
        pthread_join(arrayOfProducerThreads[i], NULL);
    }
    for (int i = 0; i < numberOfConsumers; i++) {
        pthread_join(arrayOfConsumerThreads[i], NULL);
    }
    return 0;
}

void* producer(){
    long waitTime = rand() % (timeToWait + 1);
    struct timespec period;
    period.tv_nsec = waitTime / 1000;
    for (int i = 0; i < numberOfWidgets; i++) {
        pthread_mutex_lock(&produceLock);
        struct widget* widg = (struct widget*)malloc(sizeof(struct widget));
        widg -> producersID = pthread_self();
        widg -> widgetNumber = i;
        enqueue(widg);
        pthread_mutex_unlock(&produceLock);
    }
    pthread_exit(NULL);
    return NULL;
}

void* consumer(){
    
    while(1){
        pthread_mutex_lock(&consumeLock);
        if (numberProduced < numberOfWidgets) {
            struct widget* widg = dequeue();
            printf("consumer (thread id: %d): widget %d from thread %d\n", (int)pthread_self(), widg ->widgetNumber, (int)widg ->producersID);
            //nanosleep(&period, NULL);
            pthread_mutex_unlock(&consumeLock);
        }else{
            pthread_exit(NULL);
        }
    
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
    return head ? 1 : 0;
}