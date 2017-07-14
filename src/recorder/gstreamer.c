#include "gstreamer.h"

/**
* @file gstreamer.c
* @brief Contains routines to initialize and use gstreamer
* @author Nisarg Patel
*/


/**
* @fn bus_callback (GstBus *bus, GstMessage *msg, gpointer data)
* @brief callback function for different bus activity
* @param bus pointer to the bus variable
* @param msg pointer to the bus message
* @param data bus data
*/
static gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer data)
{
    g_print("Got %s message\n", GST_MESSAGE_TYPE_NAME(msg));

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            g_main_loop_quit(loop);
            break;

        default:
            break;
    }
	return 0;
}

/**
* @fn gstreamer_setup(void)
* @brief sets up gstreamer elements like pipeline and other elements
* @return return code (success or fail)
*/ 
int gstreamer_setup(void)
{
    
	/* create pipeline  */
    pipeline = gst_pipeline_new("my-pipeline");
    if (!pipeline) {
        g_print("Failed to create pipeline!\n");
        return -1;
    }

    /* add watch for bus messages */
    bus = gst_pipeline_get_bus(GST_PIPELINE (pipeline));
    bus_watch_id = gst_bus_add_watch(bus, bus_callback, NULL);
    gst_object_unref(bus);
    
	/* create application elements */
    camera = gst_element_factory_make("v4l2src", "camera");
	video_converter = gst_element_factory_make("videoconvert", "video_converter");
    img_enc = gst_element_factory_make("jpegenc", "img_enc");
	video_enc = gst_element_factory_make("avimux", "video_enc");
    video_writer = gst_element_factory_make("filesink", "video_writer");
    caps_filter = gst_element_factory_make("capsfilter", "caps_filter");

    if (!camera || !video_converter || !img_enc || !video_enc || 
			!video_writer || !caps_filter) {
        g_print("Failed to create one or more elements!\n");
        return -1;
    }
    
    //g_object_set(G_OBJECT (camera), "num-buffers", 1, NULL);

    /* set ouput image location */
    g_object_set(G_OBJECT (video_writer), "location", "capture.avi", NULL);
    
    caps = gst_caps_from_string("video/x-raw,framerate=30/1");

    g_object_set(G_OBJECT (caps_filter), "caps", caps, NULL);
    
	/* add elements to pipeline */
    gst_bin_add_many(GST_BIN (pipeline), camera, caps_filter, video_converter, img_enc,
			video_enc, video_writer, NULL);

    if (!gst_element_link_many (camera, caps_filter, video_converter, img_enc, 
				video_enc, video_writer, NULL)) {
        g_print ("Failed to link elements\n");
        return -1;
    }
    
    /* start capture */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    /* Release pipeline */
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT (pipeline));
    g_main_loop_unref(loop);

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
	const uint8_t AUTO_FNAME_MAX_SIZE = 20;

	char *str;

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

	snprintf(fname, AUTO_FNAME_MAX_SIZE, "mov_%d%d%d_%d%d%d",
			local_time->tm_mon,
			local_time->tm_mday,
			local_time->tm_year + 1900,
			local_time->tm_hour,
			local_time->tm_min,
			local_time->tm_sec); 
	
	return 0;
}
