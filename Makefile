EXEC_NAME = rpi_action_cam

INC_DIRS = -Isrc/recorder \
	   -Isrc/server \
	   -Isrc/hw

COMMON_SRC = src/main.c \
	    src/recorder/recorder.c \
	    src/hw/buttons.c

# default compilation is development specific
# preprocessor variable DEVEL is set
default:
	gcc $(COMMON_SRC) $(DEVEL_SRC) $(INC_DIRS) -DDEVEL -o $(EXEC_NAME) -g -Wdeprecated-declarations \
	-lwiringPi `pkg-config --cflags --libs gstreamer-1.0 glib-2.0 gio-2.0`

# final compilation is suitable for production code
final:
	gcc $(COMMON_SRC) $(PROD_SRC) $(INC_DIRS) -o $(EXEC_NAME) -g -Wdeprecated-declarations \
	-lwiringPi `pkg-config --cflags --libs gstreamer-1.0 glib-2.0 gio-2.0`
