//
//  main.c
//  Asn2
//
//  Created by Nathan Gibson on 1/29/15.
//  Copyright (c) 2015 Nathan Gibson. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

struct queueNode{
    int i;
    struct queueNode* next;
};

static struct queueNode* head;
void enqueue();
int dequeue();
int isEmpty();

int main(int argc, const char * argv[]) {

    return 0;
}

void enqueue(int num){
    struct queueNode* newNode = (struct queueNode*)malloc(sizeof(struct queueNode));
    newNode -> next = NULL;
    newNode -> i = num;
    
    if (head == NULL) {
        head = newNode;
    }else{
        struct queueNode* headCopy = head;
        while(headCopy -> next != NULL){
            headCopy = headCopy->next;
        }
        headCopy->next = newNode;
    }
}

int dequeue(){
    int returnValue = -1;
    if (head == NULL) {
        return returnValue;
    }
    else if(head -> next == NULL){
        returnValue = head -> i;
        head = NULL;
    }
    else{
        returnValue = head -> i;
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