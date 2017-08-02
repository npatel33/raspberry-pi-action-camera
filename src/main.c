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
#include "recorder/gstreamer.h"

/**
* @file main.c
* @brief Contains routines to create multi threads
* @author Abhimanyu Chopra
*/

enum user_command{
	NONE,
	START,
	PAUSE,
	STOP,
	EXIT
}status;


/**
* @fn video_capture(void *nothing)
* @brief function to setup and initialize GStreamer
*/
void* video_capture(void *nothing){
	/* Initialize GStreamer */
	gst_init(NULL,NULL);
	//gst_init(&argc, &argv);	//Do we need the arguments ?
	gstreamer_setup();
	return NULL;
}

/**
* @fn console_reader(void *nothing)
* @brief function to read from console
*/
void* console_reader(void *nothing){
	
	char key = 'a';
	status = NONE;
	
	printf("Welcome to the Raspberry Pi Action Cam. \nUser command\
			options \nStart - s \nPause - p \nStop - o \nExit - e \n");

	while(key!='e'){
		printf("Enter command: ");
		scanf(" %c", &key);
		
		if(key == 's') {
			printf("Capturing is started!\n");
			gst_element_set_state(pipeline, GST_STATE_PLAYING);
			status = START;
		}
		else if(key == 'p') {
			printf("Capturing is paused!\n");
			gst_element_set_state(pipeline, GST_STATE_PAUSED);
			status = PAUSE;
		}
		else if(key == 'o')
			status = STOP;
		else if(key == 'e')
			status = EXIT;
		else
			printf("Unknown command.\n");
		
	}

	return NULL;
}

/**
* @fn secure_server(void *nothing)
* @brief function to host secure server
*/
void* secure_server(void *nothing){

	// call server related functions
	return NULL;
}

/**
* @fn main(int argc, char *argv[])
* @brief main entry function
*/
int main(int argc, char *argv[])
{
	pthread_t tid[10];

	// create console reader thread
	if((errno = pthread_create(&(tid[0]), NULL, &console_reader, NULL))!=0){
		fprintf(stderr,"Error creating console reader thread. Error Code:%d %s\n",errno,strerror(errno) );
		exit(EXIT_FAILURE);
	}

	// create video capture thread
	if((errno = pthread_create(&(tid[1]), NULL, &video_capture, NULL))!=0){
		fprintf(stderr,"Error creating video capture thread. Error Code:%d %s\n",errno,strerror(errno) );
		exit(EXIT_FAILURE);
	}

	// wait for threads to exit
	for(int i=0;i<10;i++){
		pthread_join(tid[i],NULL);
	}

	printf("Thank You. Bye!\n");

	return 0;
}



