NAME := Lib_MOORGEN_SERVICE

$(NAME)_SOURCES :=	moorgenService.c \
					moorgenWifiConfig.c \
					moorgenLocalServer.c \
					moorgenHttpOTA.c \
					mgBase64.c \
			
GLOBAL_INCLUDES += 	. 
			


$(NAME)_COMPONENTS := moorgenCloud/sysAdapter/mgSys \
					  moorgenCloud/moorgenMQTT \
					  moorgenCloud/moorgenKernel \
					