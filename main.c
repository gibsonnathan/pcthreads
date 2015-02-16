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
static int total;
static struct timespec period;
static pthread_cond_t queueNotFull;
static pthread_cond_t queueNotEmpty;
static pthread_mutex_t lock;

int main(int argc, const char * argv[]) {
    
    if (argc != 5) {
        printf("Incorrect arguments supplied\n");
        exit(-1);
    }
    pthread_cond_init(&queueNotFull, NULL);
    pthread_cond_init(&queueNotEmpty, NULL);
    pthread_mutex_init(&lock, NULL);
    head = NULL;
    numberProduced = 0;
    numberConsumed = 0;
    numberOfProducers = atoi(argv[1]);
    numberOfConsumers = atoi(argv[2]);
    numberOfWidgets = atoi(argv[3]);
    timeToWait = atoi(argv[4]);
    total = (numberOfWidgets * numberOfProducers);
    pthread_t arrayOfProducerThreads[numberOfProducers];
    pthread_t arrayOfConsumerThreads[numberOfConsumers];
    
    int i;
    for (i = 0; i < numberOfProducers; i++) {
        if(pthread_create(&arrayOfProducerThreads[i], NULL, producer, NULL)){
            printf("Error creating producer thread\n");
            exit(3);
        }
    }
    
    for (i = 0; i < numberOfConsumers; i++) {
        if(pthread_create(&arrayOfConsumerThreads[i], NULL, consumer, NULL)){
            printf("Error creating consumer thread\n");
            exit(3);
        }
    }
    
    for (i = 0; i < numberOfProducers; i++) {
        if(pthread_join(arrayOfProducerThreads[i], NULL)){
            printf("Error joining producer thread");
            exit(3);
        }
    }
    
    for (i = 0; i < numberOfConsumers; i++) {
        if(pthread_join(arrayOfConsumerThreads[i], NULL)){
            printf("Error joining consumer thread");
            exit(3);
        }
    }
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&queueNotFull);
    pthread_cond_destroy(&queueNotEmpty);
    
    return 0;
}

void* producer(){
    pthread_mutex_lock(&lock);
    int i;
    for (i = 0; i < numberOfWidgets; i++) {
        if(isFull()){
            pthread_cond_wait(&queueNotFull,&lock);
        }
        struct widget* widg = (struct widget*)malloc(sizeof(struct widget));
        widg -> producersID = pthread_self();
        widg -> widgetNumber = i;
        enqueue(widg);
        numberProduced++;
	period.tv_nsec = (((rand() % timeToWait) % 1000) * 1000 * 1000);
   	period.tv_sec = ((rand() % timeToWait) / 1000);
        nanosleep(&period, NULL);
        pthread_mutex_unlock(&lock);
        pthread_cond_signal(&queueNotEmpty); 
    }
    pthread_exit(0);
}

void* consumer(){
    pthread_mutex_lock(&lock);
    if((numberConsumed == total && isEmpty()) || (numberProduced == total && isEmpty())){
    	pthread_exit(0);
    }
    while(numberConsumed < total){
        if(isEmpty()){
            pthread_cond_wait(&queueNotEmpty,&lock);
        }
        struct widget* widg = dequeue();
        numberConsumed++;
        printf("consumer (thread id: %u): widget %u from thread %u\n",
               (int)pthread_self(), widg -> widgetNumber,
               (int)widg ->producersID);
        free(widg);
	period.tv_nsec = (((rand() % timeToWait) % 1000) * 1000 * 1000);
	period.tv_sec = ((rand() % timeToWait) / 1000);
        nanosleep(&period, NULL);
        pthread_mutex_unlock(&lock);
        pthread_cond_signal(&queueNotFull);
    }
    pthread_exit(0);
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
    return head ? 0 : 1;
}

int isFull(){
    struct widget* returnStatus = (struct widget*)malloc(sizeof(struct widget));
    int result = returnStatus ? 0 : 1;
    if (result == 1) {
        free(returnStatus);
    }
    return result;
}
