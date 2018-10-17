NAME := Lib_PRODUCT_c


$(NAME)_SOURCES := 	product.c \
					dt72Led.c \
					uartProtocol.c \
					factoryTest.c \
			


GLOBAL_INCLUDES := 	. \


$(NAME)_COMPONENTS := moorgenCloud/moorgenKernel \
				      moorgenCloud/moorgenService \
				      moorgenCloud/sysAdapter/mgSys \
