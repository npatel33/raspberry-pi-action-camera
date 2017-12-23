#include "recorder.h"

/**
 * @file recorder.c
 * @brief Contains routines to use recorder
 *
 * Recorder uses gstreamer library to capture video.
 *
 * @author Abhimanyu Chopra, Nisarg Patel
 */

/**
 * Table of supported resolution and fps
 */
static const unsigned int rec_modes[][3] = {
    {1920, 1080, 30},
    {1280, 720, 60}
};

/**
 * @fn recorder_init(void)
 * @brief ### Initializes recorder ###
 *
 * Initialization of recorder involves setting up gstreamer pipeline
 * and necessary components like v4l2src, muxer etc.
 *
 * @return return code (success or fail)
 */ 
int recorder_init(Recorder *recorder)
{
    GstElement *camera, *caps_filter, *video_parser,
               *video_enc, *video_writer;
    GstCaps *caps;
    char caps_str[256];
    guint bus_watch_id;
    char fname[32];
    uint8_t rc;

    /**
     * Default setting : 1920x1080 @ 30 fps
     * recorder state IDLE
     */
    recorder->resolution[0] = rec_modes[0][0];
    recorder->resolution[1] = rec_modes[0][1];
    recorder->fps = rec_modes[0][2];
    recorder->state = RECORDER_IDLE;
    g_mutex_init(&recorder->mutex_state);

    /**
     * create pipeline
     */
    recorder->pipeline = gst_pipeline_new("my-pipeline");
    if (!recorder->pipeline) {
        g_print("Failed to create pipeline!\n");
        return -1;
    }

    /**
     * create application elements
     */
    camera = gst_element_factory_make("v4l2src", "camera");
    video_enc = gst_element_factory_make("qtmux", "video_enc");
    video_writer = gst_element_factory_make("filesink", "video_writer");
    video_parser = gst_element_factory_make("h264parse", "video_parser");
    caps_filter = gst_element_factory_make("capsfilter", "caps_filter");

    if (!camera || !video_parser || !video_enc || 
            !video_writer || !caps_filter) {
        g_print("Failed to create one or more elements!\n");
        return -1;
    }

    /**
     * set ouput image location
     */
    rc = get_new_fname(fname);
    if(rc) {
        g_print("Failed to get file name!\n");
        return -1;
    }
    g_object_set(G_OBJECT (video_writer), "location", fname, NULL);
    g_object_set(G_OBJECT (video_writer), "sync", TRUE, NULL);

    sprintf(caps_str, "video/x-h264,framerate=%d/1,width=%d,height=%d",
            recorder->fps, recorder->resolution[0], recorder->resolution[1]);
    
    caps = gst_caps_from_string(caps_str);

    g_object_set(G_OBJECT (caps_filter), "caps", caps, NULL);

    /*
     * add elements to pipeline
     */
    gst_bin_add_many(GST_BIN (recorder->pipeline), camera, caps_filter, video_parser,
            video_enc, video_writer, NULL);

    if (!gst_element_link_many (camera, caps_filter, video_parser, 
                video_enc, video_writer, NULL)) {
        g_print ("Failed to link elements\n");
        return -1;
    }

    return 0;

}

/**
 * @fn recorder_start(Recorder *recorder)
 * @brief ### Starts recorder ###
 */
int recorder_start(Recorder *recorder)
{

    g_mutex_lock(&recorder->mutex_state);

    g_print("Recorder state: %d\n", recorder->state);
    /**
     * Capture only if recorder is stopped or paused
     */
    if (recorder->state == RECORDER_IDLE ||
        recorder->state == RECORDER_STOPPED || 
        recorder->state == RECORDER_PAUSED) {

        gst_element_set_state(recorder->pipeline, GST_STATE_PLAYING); 
        
        recorder->state = RECORDER_STARTED;
    }
    else {
        g_print("Bad state : Recorder can't be started!\n");
        g_mutex_unlock(&recorder->mutex_state);
        return -1;
    }

    g_mutex_unlock(&recorder->mutex_state);

    return 0;
}


/**
 * @fn recorder_pause(Recorder *recorder)
 * @brief ### Pauses recorder ###
 */
int recorder_pause(Recorder *recorder)
{
    g_mutex_lock(&recorder->mutex_state);

    /**
     * pause capture
     */
    if (recorder->state == RECORDER_STARTED) {

        gst_element_set_state(recorder->pipeline, GST_STATE_PAUSED);
        recorder->state = RECORDER_PAUSED;
    }
    else {
        g_print("Bad state : Recorder can't be paused!\n");
        g_mutex_unlock(&recorder->mutex_state);
        return -1;
    }

    g_mutex_unlock(&recorder->mutex_state);

    return 0;
}

/**
 * @fn recorder_stop(Recorder *recorder)
 * @brief ### Stops recorder ###
 */
int recorder_stop(Recorder *recorder)
{

    g_mutex_lock(&recorder->mutex_state);

    if (recorder->state == RECORDER_STARTED ||
        recorder->state == RECORDER_PAUSED) {

        /* Send EOS signal to pipeline
         * 
         * Without this signal final video may get corrupted
         */
        gst_element_send_event(recorder->pipeline, gst_event_new_eos());

        /*!
         * Release pipeline
         */
        gst_element_set_state(recorder->pipeline, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT (recorder->pipeline));

        recorder->state == RECORDER_STOPPED;
    }
    else {
        g_print("Bad state : Recorder can't be stopped!\n");
        g_mutex_unlock(&recorder->mutex_state);
        return -1;
    }

    g_mutex_unlock(&recorder->mutex_state);
    return 0;
}
/**
 * @fn get_new_fname(char *fname)
 * @brief generates new and unique file name for video files
 * @param[out] fname file name string
 * @return returns success(0) or failure(1)
 */
uint8_t get_new_fname(char *fname)
{
    time_t curr_time;
    struct tm *local_time;
    const uint8_t AUTO_FNAME_MAX_SIZE = 32;

    curr_time = time(NULL);
    if (curr_time == ((time_t)-1)) {
        perror("Failed to obtain time!!\n");
        return 1;
    }

    local_time = localtime(&curr_time);
    if (local_time == NULL) {
        perror("Failed to obtain local time!!\n");
        return 1;
    }

    snprintf(fname, AUTO_FNAME_MAX_SIZE, "mov_%02d%02d%d_%02d%02d%02d.mp4",
            local_time->tm_mon,
            local_time->tm_mday,
            local_time->tm_year + 1900,
            local_time->tm_hour,
            local_time->tm_min,
            local_time->tm_sec); 

    return 0;
}
