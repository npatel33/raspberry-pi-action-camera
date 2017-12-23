#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stddef.h>
#include <gst/gst.h>
#include <glib.h>
#include <gio/gio.h>
#include "recorder/recorder.h"

/**
 * @file main.c
 * @brief Contains routines to create multi threads
 * @author Abhimanyu Chopra, Nisarg Patel
 */
enum user_command {
    NONE,
    START,
    PAUSE,
    STOP,
    EXIT
}status;


/**
 * @fn console_reader(void *nothing)
 * @brief function to read from console
 */
void* console_reader(void *args) {
    
    Recorder *recorder = (Recorder *) args;

    char key = 'a';

    status = NONE;

    printf("Welcome to the Raspberry Pi Action Cam. \nUser command\
            options \nStart - s \nPause - p \nStop - o \nExit - e \n");

    while(1) {

        printf("Enter command: ");
        scanf(" %c", &key);

        if(key == 's') {
            printf("Capturing is started!\n");
            recorder_start(recorder);
            status = START;
        }
        else if(key == 'p') {
            printf("Capturing is paused!\n");
            recorder_pause(recorder);
            status = PAUSE;
        }
        else if(key == 'o') {
            printf("Caputring is stopped!\n");
            recorder_stop(recorder);
            status = STOP;
            break;
        }
        else
            printf("Unknown command.\n");

    }

    /* return NULL; */
}


/**
 * @fn main(int argc, char *argv[])
 * @brief main entry function
 */
int main(int argc, char *argv[])
{

    Recorder recorder;
    pthread_t console_thread;

    gst_init(NULL, NULL);
    recorder_init(&recorder);

    // create console reader thread
    if((errno = pthread_create(&(console_thread), NULL, &console_reader, (void *) &recorder)!=0)){
        fprintf(stderr,"Error creating console reader thread. Error Code:%d %s\n",errno,strerror(errno) );
        exit(EXIT_FAILURE);
    }
    
    pthread_join(console_thread, NULL);

    printf("Thank You. Bye!\n");

    return 0;
}
