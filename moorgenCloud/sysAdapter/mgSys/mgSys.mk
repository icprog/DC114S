

NAME := Lib_mgsys_c


$(NAME)_SOURCES := 	mgOS.c \
					mgEeprom.c \
					mgSecure.c \
					mgWifi.c \



GLOBAL_INCLUDES := 	. \


$(NAME)_COMPONENTS := moorgenCloud/moorgenService \

