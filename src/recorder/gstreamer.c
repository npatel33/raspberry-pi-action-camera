#include "gstreamer.h"

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
