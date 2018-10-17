NAME := Lib_MGMQTT_SERVICE

$(NAME)_SOURCES :=	mgMqttService.c \
					mgMqttDevice.c \
					mgMqttScene.c \
					mgMqttTimer.c \
					
			
GLOBAL_INCLUDES += 	.


$(NAME)_COMPONENTS := moorgenCloud/sysAdapter/mgMqtt \
					moorgenCloud/utility/tlv_c \
					moorgenCloud/sysAdapter/mgSys \
					moorgenCloud/moorgenKernel \
					moorgenCloud/moorgenService \
					product/$(project)