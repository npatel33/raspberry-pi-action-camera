EXEC_NAME = rpi_action_cam

INC_DIRS = -Isrc/recorder

SRC_FILES = src/main.c \
			src/recorder/gstreamer.c\

all:
	gcc $(SRC_FILES) $(INC_DIRS) -o $(EXEC_NAME) -g -Wdeprecated-declarations `pkg-config --cflags --libs gstreamer-1.0`

