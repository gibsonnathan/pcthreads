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

int main(int argc, const char * argv[]) {
    
    head = NULL; //queue is initially empty

    //get command line arguments
    int numberOfProducers = atoi(argv[1]);
    int numberOfConsumers = atoi(argv[2]);
    int numberOfWidgets = atoi(argv[3]);
    int timeToWait = atoi(argv[4]);
    
    //testing queue
    struct widget* widg1 = (struct widget*)malloc(sizeof(struct widget));
    struct widget* widg2 = (struct widget*)malloc(sizeof(struct widget));
    struct widget* widg3 = (struct widget*)malloc(sizeof(struct widget));
    struct widget* widg4 = (struct widget*)malloc(sizeof(struct widget));
    
    widg1 -> widgetNumber = 1;
    widg2 -> widgetNumber = 2;
    widg3 -> widgetNumber = 3;
    widg4 -> widgetNumber = 4;
    
    enqueue(widg1);
    enqueue(widg2);
    enqueue(widg3);
    enqueue(widg4);

    //have to make sure that head isn't NULL before
    //trying to access its members
    printf("%d", head ? dequeue() ->widgetNumber : -1);
    printf("%d", head ? dequeue() ->widgetNumber : -1);
    printf("%d", head ? dequeue() ->widgetNumber : -1);
    printf("%d", head ? dequeue() ->widgetNumber : -1);
    printf("%d", head ? dequeue() ->widgetNumber : -1);
    
    return 0;
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
    if (head) {
        return 1;
    }
    else{
        return 0;
    }
}