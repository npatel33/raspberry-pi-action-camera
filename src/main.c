#include <stdio.h>
#include "recorder/gstreamer.h"

/**
* @fn main(int argc, char *argv[])
* @brief main entry function
*/
int main(int argc, char *argv[])
{
    /* Initialize GStreamer */
    gst_init(&argc, &argv);
	gstreamer_setup();

	return 0;
}
