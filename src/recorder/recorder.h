#ifndef RECORDER_GSTREAMER_H
#define RECORDER_GSTREAMER_H

/**
 * @file gstreamer.h
 * @brief Gstreamer functions and variable declarations
 * @author Abhimanyu Chopra, Nisarg Patel
 */
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <gst/gst.h>

/**
 * @brief ### Recorder structure ###
 *
 */
typedef struct Recorder {
    GstElement *pipeline;
    GMainLoop *loop;
} Recorder;


uint8_t get_new_fname(char *fname);
int recorder_init(Recorder *recorder);
int recorder_start(Recorder *recorder);
int recorder_pause(Recorder *recorder);
int recorder_stop(Recorder *recorder);

#endif
