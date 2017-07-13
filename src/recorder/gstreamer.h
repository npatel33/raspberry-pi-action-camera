#ifndef RECORDER_GSTREAMER_H
#define RECORDER_GSTREAMER_H

/**
* @file gstreamer.h
* @brief gstreamer functions and variable declarations
* @author Nisarg Patel
*/
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <gst/gst.h>

GstElement *pipeline;
GstElement *camera, *caps_filter, *img_enc, *img_writer;
GstCaps *caps;
GstBus *bus;
guint bus_watch_id;
GMainLoop *loop;

uint8_t get_new_fname(char *fname);
int gstreamer_setup(void);

#endif
