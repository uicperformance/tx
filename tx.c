#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<time.h>
#include<getopt.h>
#include<sys/mman.h>
#include<pthread.h>
#include <unistd.h>

#define MAXSIZE 1024*1024*1024
pthread_barrier_t wall;

int size=4096; // default size: control with -s command line argument

char shared[MAXSIZE];

void send(char* buf) {
    memcpy(shared,buf,size);
    pthread_barrier_wait(&wall);
}

void recv(char* buf) {
    pthread_barrier_wait(&wall);
    memcpy(buf,shared,size);  
}

void* sender(void* arg) {
    pthread_barrier_wait(&wall);
    uint64_t time=0;

    char buf[size];
    char buf2[size];
    memset(buf,0,size);
    for(int i=0;i<ITERATIONS;i++) {
        pthread_barrier_wait(&wall);

        struct timespec before,after;
        clock_gettime(CLOCK_MONOTONIC,&before);

    // SINGLE-BUFFERING: the sender first prepares the message, then sends it
    // DOUBLE-BUFFERING: the sender sends a message, then starts preparing the next one
    #ifndef DOUBLEBUFFER
        memset(buf,i,size);
        send(buf);
    #else
       send((i%2?buf:buf2));
       memset((i%2?buf2:buf),i+1,size);
    #endif

        clock_gettime(CLOCK_MONOTONIC,&after);
        time += ((after.tv_sec-before.tv_sec)*(uint64_t)1000000000+(after.tv_nsec-before.tv_nsec));
    }

    printf("Sender: %ld secs total, %lf GB/sec @ size %d\n",
    time/1000000000,1l*size*ITERATIONS*1000000000.0/time/1024/1024/1024,size);

}

void validate(char val,char* buf) {    
    for(int i=0;i<size;i++) {
        if(buf[i]!=val) {
            fprintf(stderr,"PANIC: incorrect value in received buffer: %d %d\n",(int)val,(int)buf[i]);
            exit(1);
        }
    }
}

void* receiver(void* arg) {
    pthread_barrier_wait(&wall);
    char buf[size];
    uint64_t time=0;

    for(int i=0;i<ITERATIONS;i++) {
        pthread_barrier_wait(&wall);

        struct timespec before,after;
        clock_gettime(CLOCK_MONOTONIC,&before);

        recv(buf);
        
        clock_gettime(CLOCK_MONOTONIC,&after);
        time += ((after.tv_sec-before.tv_sec)*(uint64_t)1000000000+(after.tv_nsec-before.tv_nsec));

        #ifndef NOVALIDATE
        validate(i,buf);
        #endif
    }

    printf("Receiver: %ld secs total, %lf GB/sec @ size %d\n",
    time/1000000000,1l*size*ITERATIONS*1000000000.0/time/1024/1024/1024,size);
}

int main(int argc, char** argv) {
	pthread_barrier_init(&wall,0,2);
    int opt;
    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
            case 's':
                size = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -n <array_size>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    struct timespec before,after;
    clock_gettime(CLOCK_MONOTONIC,&before);

    pthread_t one,two;
    pthread_create(&one,0,(void*)(void*)sender,0);
    pthread_create(&two,0,(void*)(void*)receiver,0);

    uint64_t retOne,retTwo;    
    if(pthread_join(one,(void**)&retOne) != 0) {
        perror("Error joining one\n");
        exit(1);
    }
    pthread_join(two,(void**)&retTwo);
   

}