NAME := Lib_MGKERNEL

$(NAME)_SOURCES :=	mgDMG.c \
					deviceObject.c \
					mgKernel.c \
					sceneObject.c \
					timerObject.c \
			
GLOBAL_INCLUDES += 	.


$(NAME)_COMPONENTS := moorgenCloud/sysAdapter/mgSys \
					moorgenCloud/moorgenMQTT \
					moorgenCloud/moorgenService \
					