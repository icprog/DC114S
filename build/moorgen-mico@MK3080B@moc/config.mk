MiCO_SDK_MAKEFILES           		+= ./mico-os/libraries/drivers/MiCODriverMFiAuth/MiCODriverMFiAuth.mk .//moorgenCloud/utility/tlv_c/tlv_c.mk .//moorgenCloud/sysAdapter/mgMqtt/mgMqtt.mk ./mico-os/platform/GCC/GCC.mk ./mico-os/libraries/utilities/json_c/json_c.mk ./mico-os/MiCO/security/Sodium/Sodium.mk ./mico-os/MiCO/security/SRP_6a/SRP_6a.mk ./mico-os/MiCO/system/easylink/aws/aws.mk ./mico-os/MiCO/system/easylink/MFi_WAC/MFi_WAC.mk ./mico-os/MiCO/system/qc_test/qc_test.mk ./mico-os/libraries/drivers/sensor/infrared_adc/infrared_adc.mk ./mico-os/libraries/drivers/sensor/light_adc/light_adc.mk ./mico-os/libraries/drivers/sensor/APDS9930/APDS9930.mk ./mico-os/libraries/drivers/sensor/DHT11/DHT11.mk ./mico-os/libraries/drivers/sensor/BME280/BME280.mk ./mico-os/libraries/drivers/rgb_led/P9813/P9813.mk ./mico-os/libraries/drivers/motor/dc_motor/dc_motor.mk ./mico-os/libraries/drivers/keypad/gpio_button/gpio_button.mk ./mico-os/libraries/drivers/display/VGM128064/VGM128064.mk .//moorgenCloud/moorgenKernel/moorgenKernel.mk .//moorgenCloud/moorgenMQTT/moorgenMQTT.mk ./mico-os/platform/MCU/MX1290/MX1290.mk ./mico-os/libraries/utilities/utilities.mk ./mico-os/MiCO/security/security.mk ./mico-os/MiCO/system/system.mk ./mico-os/MiCO/core/core.mk ./mico-os/MiCO/net/mocIP/mico/mico.mk ./mico-os/MiCO/RTOS/mocOS/mico/mico.mk ./mico-os/libraries/drivers/MiCOKit_EXT2/MiCOKit_EXT2.mk .//product/DC114S/DC114S.mk .//moorgenCloud/moorgenService/moorgenService.mk .//moorgenCloud/sysAdapter/mgSys/mgSys.mk ./mico-os/MiCO/MiCO.mk ./mico-os/MiCO/security/TLS/mocSSL/mocSSL.mk ./mico-os/MiCO/net/mocIP/mocIP.mk ./mico-os/MiCO/RTOS/mocOS/mocOS.mk ./mico-os/board/MK3080B/MK3080B.mk .//moorgen-mico/moorgen-mico.mk
TOOLCHAIN_NAME            		:= GCC
MiCO_SDK_LDFLAGS             		+= -Wl,--gc-sections -Wl,--cref -Wl,-wrap,_malloc_r -Wl,-wrap,free -Wl,-wrap,realloc -Wl,-wrap,malloc -Wl,-wrap,calloc -Wl,-wrap,_free_r -Wl,-wrap,_realloc_r -mthumb -mcpu=cortex-m4 -Wl,-A,thumb -mlittle-endian -nostartfiles -Wl,--defsym,__STACKSIZE__=800 -L ./platform/MCU/MX1290/GCC --specs=nano.specs -u _printf_float -mfpu=fpv4-sp-d16 -mfloat-abi=hard -L ./mico-os/board/MK3080B
RESOURCE_CFLAGS					+= -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mlittle-endian
MiCO_SDK_LINK_SCRIPT         		+= ./mico-os/platform/MCU/MX1290/GCC/app_with_moc.ld
MiCO_SDK_LINK_SCRIPT_CMD    	 	+= -Wl,-T ./mico-os/platform/MCU/MX1290/GCC/app_with_moc.ld
MiCO_SDK_PREBUILT_LIBRARIES 	 	+= ./mico-os/MiCO/system/easylink/MFi_WAC/Lib_MFi_WAC.Cortex-M4F.GCC.release.a ./mico-os/MiCO/system/easylink/aws/aws.Cortex-M4F.GCC.release.a ./mico-os/MiCO/security/SRP_6a/Lib_SRP6a.Cortex-M4F.GCC.release.a ./mico-os/MiCO/security/Sodium/Lib_Sodium.Cortex-M4F.GCC.release.a
MiCO_SDK_CERTIFICATES       	 	+= 
MiCO_SDK_PRE_APP_BUILDS      		+= 
MiCO_SDK_LINK_FILES          		+=                                                                            
MiCO_SDK_INCLUDES           	 	+=                                                  -I.//moorgenCloud/utility/tlv_c/. -I.//moorgenCloud/sysAdapter/mgMqtt/. -I.//moorgenCloud/sysAdapter/mgMqtt/MQTTPacket -I./mico-os/libraries/utilities/json_c/. -I./mico-os/MiCO/security/Sodium/inc -I./mico-os/MiCO/security/SRP_6a/inc -I./mico-os/MiCO/system/easylink/aws/. -I./mico-os/MiCO/system/qc_test/. -I./mico-os/libraries/drivers/sensor/infrared_adc/. -I./mico-os/libraries/drivers/sensor/light_adc/. -I./mico-os/libraries/drivers/sensor/APDS9930/. -I./mico-os/libraries/drivers/sensor/DHT11/. -I./mico-os/libraries/drivers/sensor/BME280/. -I./mico-os/libraries/drivers/rgb_led/P9813/. -I./mico-os/libraries/drivers/motor/dc_motor/. -I./mico-os/libraries/drivers/keypad/gpio_button/. -I./mico-os/libraries/drivers/display/VGM128064/. -I.//moorgenCloud/moorgenKernel/. -I.//moorgenCloud/moorgenMQTT/. -I./mico-os/platform/MCU/MX1290/. -I./mico-os/platform/MCU/MX1290/.. -I./mico-os/platform/MCU/MX1290/../.. -I./mico-os/platform/MCU/MX1290/../../include -I./mico-os/platform/MCU/MX1290/../../GCC -I./mico-os/platform/MCU/MX1290/../../Cortex-M4 -I./mico-os/platform/MCU/MX1290/../../Cortex-M4/CMSIS -I./mico-os/platform/MCU/MX1290/interface -I./mico-os/platform/MCU/MX1290/peripherals -I./mico-os/libraries/utilities/. -I./mico-os/MiCO/security/Curve25519 -I./mico-os/MiCO/security/SHAUtils -I./mico-os/MiCO/core/. -I./mico-os/MiCO/core/../../platform/MCU/MX1290/moc -I./mico-os/MiCO/net/mocIP/mico/. -I./mico-os/MiCO/RTOS/mocOS/mico/. -I./mico-os/libraries/drivers/MiCOKit_EXT2/. -I./mico-os/libraries/drivers/MiCOKit_EXT2/.. -I.//product/DC114S/. -I.//moorgenCloud/moorgenService/. -I.//moorgenCloud/sysAdapter/mgSys/. -I./mico-os/MiCO/. -I./mico-os/MiCO/system -I./mico-os/MiCO/system/include -I./mico-os/MiCO/security -I./mico-os/MiCO/RTOS/mocOS/.. -I./mico-os/board/MK3080B/. -I./mico-os/include -I./moorgen-mico -I.
MiCO_SDK_DEFINES             		+=                    -D__FPU_PRESENT -DCONFIG_CPU_MX1290 -DMOC=1 -DTLS_SSL_mocSSL=1 -DmocSSL_VERSION=\"v1.0.0\" -DNETWORK_mocIP=1 -DmocIP_VERSION=\"v1.0.0\" -DRTOS_mocOS=1 -DmocOS_VERSION=\"v1.0.0\" -DCRLF_STDIO_REPLACEMENT -DCONFIG_DEVICE_TYPE=\"22000005\" -DCONFIG_SINGLE_DEVICE -DCONFIG_PRODUCT=\"DC114S\" -DCONFIG_VERSION=\"0.2.0\" -DCONFIG_FACTORY=\"Connector\" -DCONFIG_CHANNEL_CODE=\"6b05985c-6be7-439b-8f6b-9e0011452110\" -DMQTT_PORT_SLL=3453 -DCONFIG_MQTT_CA_EU -DMQTT_DOMAIN_NAME=\"connectoreu.dooya.com\"
COMPONENTS                		:= App_moorgen-mico Board_MK3080 mocOS mocIP mocSSL MiCO Lib_mgsys_c Lib_MOORGEN_SERVICE Lib_PRODUCT_c Lib_MiCOKit_EXT_MK3080B MiCO_mocOS_Interface MiCO_mocIP_Interface Lib_MiCO_Kernel Lib_MiCO_System Lib_MiCO_Security Lib_Utilities MX1290 Lib_MGMQTT_SERVICE Lib_MGKERNEL Lib_VGM128064_MK3080B Lib_gpio_button_MK3080B Lib_dc_motor_MK3080B Lib_RGB_P9813_MK3080B Sensor_BME280_MK3080B Sensor_DHT11_MK3080B Sensor_APDS9930_MK3080B Sensor_Light_adc_MK3080B Sensor_Infrared_Reflective_MK3080B Lib_MiCO_System_QC Lib_MiCO_System_WAC aws Lib_MiCO_Security_SRP_6a Lib_MiCO_Security_Sodium Lib_JSON_C GCC Lib_mgmqtt_client_c Lib_TLV_C Lib_MFi_Auth_MK3080B
BUS                       		:= MK3080
IMAGE_TYPE                		:= ram
NETWORK_FULL              		:= 
RTOS_FULL                 		:= mocOS
PLATFORM_DIRECTORY        		:= ./mico-os/board/MK3080B
APP_FULL                  		:= ./moorgen-mico
NETWORK                   		:= 
RTOS                      		:= mocOS
MODULE                  			:= 3080B
PLATFORM                  		:= MK3080B
HOST_MCU_FAMILY                  	:= MX1290
USB                       		:= 
APP                       		:= moorgen-mico
HOST_OPENOCD              		:= MX1290
JTAG              		        := jlink_swd
HOST_ARCH                 		:= Cortex-M4F
NO_BUILD_BOOTLOADER           	:= 
NO_BOOTLOADER_REQUIRED         	:= 
App_moorgen-mico_LOCATION         := .//moorgen-mico/
Board_MK3080_LOCATION         := ./mico-os/board/MK3080B/
mocOS_LOCATION         := ./mico-os/MiCO/RTOS/mocOS/
mocIP_LOCATION         := ./mico-os/MiCO/net/mocIP/
mocSSL_LOCATION         := ./mico-os/MiCO/security/TLS/mocSSL/
MiCO_LOCATION         := ./mico-os/MiCO/
Lib_mgsys_c_LOCATION         := .//moorgenCloud/sysAdapter/mgSys/
Lib_MOORGEN_SERVICE_LOCATION         := .//moorgenCloud/moorgenService/
Lib_PRODUCT_c_LOCATION         := .//product/DC114S/
Lib_MiCOKit_EXT_MK3080B_LOCATION         := ./mico-os/libraries/drivers/MiCOKit_EXT2/
MiCO_mocOS_Interface_LOCATION         := ./mico-os/MiCO/RTOS/mocOS/mico/
MiCO_mocIP_Interface_LOCATION         := ./mico-os/MiCO/net/mocIP/mico/
Lib_MiCO_Kernel_LOCATION         := ./mico-os/MiCO/core/
Lib_MiCO_System_LOCATION         := ./mico-os/MiCO/system/
Lib_MiCO_Security_LOCATION         := ./mico-os/MiCO/security/
Lib_Utilities_LOCATION         := ./mico-os/libraries/utilities/
MX1290_LOCATION         := ./mico-os/platform/MCU/MX1290/
Lib_MGMQTT_SERVICE_LOCATION         := .//moorgenCloud/moorgenMQTT/
Lib_MGKERNEL_LOCATION         := .//moorgenCloud/moorgenKernel/
Lib_VGM128064_MK3080B_LOCATION         := ./mico-os/libraries/drivers/display/VGM128064/
Lib_gpio_button_MK3080B_LOCATION         := ./mico-os/libraries/drivers/keypad/gpio_button/
Lib_dc_motor_MK3080B_LOCATION         := ./mico-os/libraries/drivers/motor/dc_motor/
Lib_RGB_P9813_MK3080B_LOCATION         := ./mico-os/libraries/drivers/rgb_led/P9813/
Sensor_BME280_MK3080B_LOCATION         := ./mico-os/libraries/drivers/sensor/BME280/
Sensor_DHT11_MK3080B_LOCATION         := ./mico-os/libraries/drivers/sensor/DHT11/
Sensor_APDS9930_MK3080B_LOCATION         := ./mico-os/libraries/drivers/sensor/APDS9930/
Sensor_Light_adc_MK3080B_LOCATION         := ./mico-os/libraries/drivers/sensor/light_adc/
Sensor_Infrared_Reflective_MK3080B_LOCATION         := ./mico-os/libraries/drivers/sensor/infrared_adc/
Lib_MiCO_System_QC_LOCATION         := ./mico-os/MiCO/system/qc_test/
Lib_MiCO_System_WAC_LOCATION         := ./mico-os/MiCO/system/easylink/MFi_WAC/
aws_LOCATION         := ./mico-os/MiCO/system/easylink/aws/
Lib_MiCO_Security_SRP_6a_LOCATION         := ./mico-os/MiCO/security/SRP_6a/
Lib_MiCO_Security_Sodium_LOCATION         := ./mico-os/MiCO/security/Sodium/
Lib_JSON_C_LOCATION         := ./mico-os/libraries/utilities/json_c/
GCC_LOCATION         := ./mico-os/platform/GCC/
Lib_mgmqtt_client_c_LOCATION         := .//moorgenCloud/sysAdapter/mgMqtt/
Lib_TLV_C_LOCATION         := .//moorgenCloud/utility/tlv_c/
Lib_MFi_Auth_MK3080B_LOCATION         := ./mico-os/libraries/drivers/MiCODriverMFiAuth/
App_moorgen-mico_SOURCES          += main.c
Board_MK3080_SOURCES          += platform.c
mocOS_SOURCES          += 
mocIP_SOURCES          += 
mocSSL_SOURCES          += mico/mico_ssl.c
MiCO_SOURCES          += 
Lib_mgsys_c_SOURCES          += mgOS.c mgEeprom.c mgSecure.c mgWifi.c 
Lib_MOORGEN_SERVICE_SOURCES          += moorgenService.c moorgenWifiConfig.c moorgenLocalServer.c moorgenHttpOTA.c mgBase64.c 
Lib_PRODUCT_c_SOURCES          += product.c dt72Led.c uartProtocol.c factoryTest.c 
Lib_MiCOKit_EXT_MK3080B_SOURCES          += micokit_ext_mfg.c micokit_ext.c motion_sensor.c temp_hum_sensor.c
MiCO_mocOS_Interface_SOURCES          += mico_rtos.c ../../mico_rtos_common.c
MiCO_mocIP_Interface_SOURCES          += mico_socket.c
Lib_MiCO_Kernel_SOURCES          += mico_config.c moc_main.c ../../platform/MCU/MX1290/moc/moc_api.c 
Lib_MiCO_System_SOURCES          += mico_system_init.c mico_system_monitor.c mico_system_notification.c mico_system_para_storage.c mico_system_time.c mico_system_power_daemon.c mico_filesystem.c system_misc.c  command_console/mico_cli.c config_server/config_server_menu.c config_server/config_server.c easylink/system_easylink_delegate.c easylink/system_easylink_wac.c easylink/system_easylink.c easylink/system_easylink_usr.c easylink/system_easylink_softap.c easylink/system_easylink_monitor.c easylink/system_aws.c easylink/internal/easylink_bonjour.c mdns/mico_mdns.c mdns/system_discovery.c tftp_ota/tftp_ota.c tftp_ota/tftpc.c
Lib_MiCO_Security_SOURCES          += Curve25519/curve25519-donna.c  SHAUtils/hkdf.c SHAUtils/hmac.c SHAUtils/sha1.c SHAUtils/sha224-256.c SHAUtils/sha384-512.c SHAUtils/usha.c
Lib_Utilities_SOURCES          += AESUtils.c RingBufferUtils.c StringUtils.c LinkListUtils.c SocketUtils.c CheckSumUtils.c HTTPUtils.c TimeUtils.c TLVUtils.c URLUtils.c
MX1290_SOURCES          += ../../Cortex-M4/crt0_GCC.c ../../Cortex-M4/platform_core.c ../platform_nsclock.c ../moc_platform_common.c moc/moc_adapter.c
Lib_MGMQTT_SERVICE_SOURCES          += mgMqttService.c mgMqttDevice.c mgMqttScene.c mgMqttTimer.c 
Lib_MGKERNEL_SOURCES          += mgDMG.c deviceObject.c mgKernel.c sceneObject.c timerObject.c 
Lib_VGM128064_MK3080B_SOURCES          += AsciiLib.c oled.c
Lib_gpio_button_MK3080B_SOURCES          += button.c
Lib_dc_motor_MK3080B_SOURCES          += dc_motor.c
Lib_RGB_P9813_MK3080B_SOURCES          += hsb2rgb_led.c rgb_led.c
Sensor_BME280_MK3080B_SOURCES          += bme280_user.c bme280.c
Sensor_DHT11_MK3080B_SOURCES          += DHT11.c
Sensor_APDS9930_MK3080B_SOURCES          += APDS9930.c
Sensor_Light_adc_MK3080B_SOURCES          += light_sensor.c
Sensor_Infrared_Reflective_MK3080B_SOURCES          += infrared_reflective.c
Lib_MiCO_System_QC_SOURCES          += qc_test.c internal/qc_test_tcpip.c internal/qc_test_wlan.c internal/qc_test_ble.c internal/qc_test_cli.c
Lib_MiCO_System_WAC_SOURCES          += 
aws_SOURCES          += 
Lib_MiCO_Security_SRP_6a_SOURCES          += 
Lib_MiCO_Security_Sodium_SOURCES          += 
Lib_JSON_C_SOURCES          += arraylist.c json_debug.c json_object.c json_tokener.c json_util.c linkhash.c printbuf.c
GCC_SOURCES          += mem_newlib.c math_newlib.c stdio_newlib.c time_newlib.c eabi.c
Lib_mgmqtt_client_c_SOURCES          += mgMQTTClient.c mgMQTT.c MQTTPacket/MQTTConnectClient.c MQTTPacket/MQTTConnectServer.c MQTTPacket/MQTTDeserializePublish.c MQTTPacket/MQTTFormat.c MQTTPacket/MQTTPacket.c MQTTPacket/MQTTSerializePublish.c MQTTPacket/MQTTSubscribeClient.c MQTTPacket/MQTTSubscribeServer.c MQTTPacket/MQTTUnsubscribeClient.c MQTTPacket/MQTTUnsubscribeServer.c 
Lib_TLV_C_SOURCES          += key_list.c tlv_box.c
Lib_MFi_Auth_MK3080B_SOURCES          += MiCODriverMFiAuth.c
App_moorgen-mico_CHECK_HEADERS    += 
Board_MK3080_CHECK_HEADERS    += 
mocOS_CHECK_HEADERS    += 
mocIP_CHECK_HEADERS    += 
mocSSL_CHECK_HEADERS    += 
MiCO_CHECK_HEADERS    += 
Lib_mgsys_c_CHECK_HEADERS    += 
Lib_MOORGEN_SERVICE_CHECK_HEADERS    += 
Lib_PRODUCT_c_CHECK_HEADERS    += 
Lib_MiCOKit_EXT_MK3080B_CHECK_HEADERS    += 
MiCO_mocOS_Interface_CHECK_HEADERS    += 
MiCO_mocIP_Interface_CHECK_HEADERS    += 
Lib_MiCO_Kernel_CHECK_HEADERS    += 
Lib_MiCO_System_CHECK_HEADERS    += 
Lib_MiCO_Security_CHECK_HEADERS    += 
Lib_Utilities_CHECK_HEADERS    += 
MX1290_CHECK_HEADERS    += 
Lib_MGMQTT_SERVICE_CHECK_HEADERS    += 
Lib_MGKERNEL_CHECK_HEADERS    += 
Lib_VGM128064_MK3080B_CHECK_HEADERS    += 
Lib_gpio_button_MK3080B_CHECK_HEADERS    += 
Lib_dc_motor_MK3080B_CHECK_HEADERS    += 
Lib_RGB_P9813_MK3080B_CHECK_HEADERS    += 
Sensor_BME280_MK3080B_CHECK_HEADERS    += 
Sensor_DHT11_MK3080B_CHECK_HEADERS    += 
Sensor_APDS9930_MK3080B_CHECK_HEADERS    += 
Sensor_Light_adc_MK3080B_CHECK_HEADERS    += 
Sensor_Infrared_Reflective_MK3080B_CHECK_HEADERS    += 
Lib_MiCO_System_QC_CHECK_HEADERS    += 
Lib_MiCO_System_WAC_CHECK_HEADERS    += 
aws_CHECK_HEADERS    += 
Lib_MiCO_Security_SRP_6a_CHECK_HEADERS    += 
Lib_MiCO_Security_Sodium_CHECK_HEADERS    += 
Lib_JSON_C_CHECK_HEADERS    += 
GCC_CHECK_HEADERS    += 
Lib_mgmqtt_client_c_CHECK_HEADERS    += 
Lib_TLV_C_CHECK_HEADERS    += 
Lib_MFi_Auth_MK3080B_CHECK_HEADERS    += 
App_moorgen-mico_INCLUDES         := 
Board_MK3080_INCLUDES         := 
mocOS_INCLUDES         := 
mocIP_INCLUDES         := 
mocSSL_INCLUDES         := 
MiCO_INCLUDES         := 
Lib_mgsys_c_INCLUDES         := 
Lib_MOORGEN_SERVICE_INCLUDES         := 
Lib_PRODUCT_c_INCLUDES         := 
Lib_MiCOKit_EXT_MK3080B_INCLUDES         := 
MiCO_mocOS_Interface_INCLUDES         := 
MiCO_mocIP_Interface_INCLUDES         := 
Lib_MiCO_Kernel_INCLUDES         := 
Lib_MiCO_System_INCLUDES         := -I./mico-os/MiCO/system/command_console -I./mico-os/MiCO/system/easylink/internal
Lib_MiCO_Security_INCLUDES         := 
Lib_Utilities_INCLUDES         := 
MX1290_INCLUDES         := 
Lib_MGMQTT_SERVICE_INCLUDES         := 
Lib_MGKERNEL_INCLUDES         := 
Lib_VGM128064_MK3080B_INCLUDES         := 
Lib_gpio_button_MK3080B_INCLUDES         := 
Lib_dc_motor_MK3080B_INCLUDES         := 
Lib_RGB_P9813_MK3080B_INCLUDES         := 
Sensor_BME280_MK3080B_INCLUDES         := 
Sensor_DHT11_MK3080B_INCLUDES         := 
Sensor_APDS9930_MK3080B_INCLUDES         := 
Sensor_Light_adc_MK3080B_INCLUDES         := 
Sensor_Infrared_Reflective_MK3080B_INCLUDES         := 
Lib_MiCO_System_QC_INCLUDES         := -I./mico-os/MiCO/system/qc_test/internal
Lib_MiCO_System_WAC_INCLUDES         := 
aws_INCLUDES         := 
Lib_MiCO_Security_SRP_6a_INCLUDES         := 
Lib_MiCO_Security_Sodium_INCLUDES         := 
Lib_JSON_C_INCLUDES         := 
GCC_INCLUDES         := 
Lib_mgmqtt_client_c_INCLUDES         := 
Lib_TLV_C_INCLUDES         := 
Lib_MFi_Auth_MK3080B_INCLUDES         := 
App_moorgen-mico_DEFINES          := 
Board_MK3080_DEFINES          := 
mocOS_DEFINES          := 
mocIP_DEFINES          := 
mocSSL_DEFINES          := 
MiCO_DEFINES          := 
Lib_mgsys_c_DEFINES          := 
Lib_MOORGEN_SERVICE_DEFINES          := 
Lib_PRODUCT_c_DEFINES          := 
Lib_MiCOKit_EXT_MK3080B_DEFINES          := 
MiCO_mocOS_Interface_DEFINES          := 
MiCO_mocIP_Interface_DEFINES          := 
Lib_MiCO_Kernel_DEFINES          := 
Lib_MiCO_System_DEFINES          := -DCONFIG_MICO_AWS
Lib_MiCO_Security_DEFINES          := 
Lib_Utilities_DEFINES          := 
MX1290_DEFINES          := 
Lib_MGMQTT_SERVICE_DEFINES          := 
Lib_MGKERNEL_DEFINES          := 
Lib_VGM128064_MK3080B_DEFINES          := 
Lib_gpio_button_MK3080B_DEFINES          := 
Lib_dc_motor_MK3080B_DEFINES          := 
Lib_RGB_P9813_MK3080B_DEFINES          := 
Sensor_BME280_MK3080B_DEFINES          := 
Sensor_DHT11_MK3080B_DEFINES          := 
Sensor_APDS9930_MK3080B_DEFINES          := 
Sensor_Light_adc_MK3080B_DEFINES          := 
Sensor_Infrared_Reflective_MK3080B_DEFINES          := 
Lib_MiCO_System_QC_DEFINES          := 
Lib_MiCO_System_WAC_DEFINES          := 
aws_DEFINES          := 
Lib_MiCO_Security_SRP_6a_DEFINES          := 
Lib_MiCO_Security_Sodium_DEFINES          := 
Lib_JSON_C_DEFINES          := 
GCC_DEFINES          := 
Lib_mgmqtt_client_c_DEFINES          := 
Lib_TLV_C_DEFINES          := 
Lib_MFi_Auth_MK3080B_DEFINES          := 
App_moorgen-mico_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Board_MK3080_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
mocOS_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
mocIP_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
mocSSL_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
MiCO_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_mgsys_c_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MOORGEN_SERVICE_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_PRODUCT_c_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCOKit_EXT_MK3080B_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
MiCO_mocOS_Interface_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
MiCO_mocIP_Interface_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_Kernel_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_System_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_Security_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_Utilities_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" -Wno-char-subscripts
MX1290_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" -Wno-implicit-function-declaration -Wno-unused-variable -Wno-implicit-function-declaration -Wno-unused-variable
Lib_MGMQTT_SERVICE_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MGKERNEL_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_VGM128064_MK3080B_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" -Wno-missing-braces
Lib_gpio_button_MK3080B_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_dc_motor_MK3080B_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_RGB_P9813_MK3080B_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Sensor_BME280_MK3080B_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Sensor_DHT11_MK3080B_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Sensor_APDS9930_MK3080B_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Sensor_Light_adc_MK3080B_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Sensor_Infrared_Reflective_MK3080B_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_System_QC_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_System_WAC_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
aws_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_Security_SRP_6a_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_Security_Sodium_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_JSON_C_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" -Wno-unused-value
GCC_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_mgmqtt_client_c_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_TLV_C_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MFi_Auth_MK3080B_CFLAGS           :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard    -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
App_moorgen-mico_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Board_MK3080_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
mocOS_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
mocIP_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
mocSSL_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
MiCO_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_mgsys_c_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MOORGEN_SERVICE_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_PRODUCT_c_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCOKit_EXT_MK3080B_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
MiCO_mocOS_Interface_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
MiCO_mocIP_Interface_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_Kernel_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_System_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_Security_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_Utilities_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
MX1290_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MGMQTT_SERVICE_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MGKERNEL_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_VGM128064_MK3080B_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_gpio_button_MK3080B_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_dc_motor_MK3080B_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_RGB_P9813_MK3080B_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Sensor_BME280_MK3080B_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Sensor_DHT11_MK3080B_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Sensor_APDS9930_MK3080B_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Sensor_Light_adc_MK3080B_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Sensor_Infrared_Reflective_MK3080B_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_System_QC_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_System_WAC_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
aws_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_Security_SRP_6a_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MiCO_Security_Sodium_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_JSON_C_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
GCC_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_mgmqtt_client_c_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_TLV_C_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
Lib_MFi_Auth_MK3080B_CXXFLAGS         :=                      -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard  -mlittle-endian                 -DDEBUG -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DMiCO_SDK_VERSION_MAJOR=3 -DMiCO_SDK_VERSION_MINOR=6 -DMiCO_SDK_VERSION_REVISION=0 -DBUS=\"MK3080\" -I./build/moorgen-mico@MK3080B@moc/resources/ -DPLATFORM=\"MK3080B\" 
App_moorgen-mico_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Board_MK3080_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
mocOS_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
mocIP_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
mocSSL_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
MiCO_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_mgsys_c_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MOORGEN_SERVICE_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_PRODUCT_c_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MiCOKit_EXT_MK3080B_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
MiCO_mocOS_Interface_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
MiCO_mocIP_Interface_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MiCO_Kernel_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MiCO_System_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MiCO_Security_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_Utilities_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
MX1290_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MGMQTT_SERVICE_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MGKERNEL_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_VGM128064_MK3080B_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_gpio_button_MK3080B_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_dc_motor_MK3080B_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_RGB_P9813_MK3080B_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Sensor_BME280_MK3080B_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Sensor_DHT11_MK3080B_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Sensor_APDS9930_MK3080B_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Sensor_Light_adc_MK3080B_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Sensor_Infrared_Reflective_MK3080B_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MiCO_System_QC_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MiCO_System_WAC_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
aws_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MiCO_Security_SRP_6a_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MiCO_Security_Sodium_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_JSON_C_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
GCC_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_mgmqtt_client_c_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_TLV_C_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
Lib_MFi_Auth_MK3080B_ASMFLAGS         :=                      -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard                   -ggdb
App_moorgen-mico_RESOURCES        := 
Board_MK3080_RESOURCES        := 
mocOS_RESOURCES        := 
mocIP_RESOURCES        := 
mocSSL_RESOURCES        := 
MiCO_RESOURCES        := 
Lib_mgsys_c_RESOURCES        := 
Lib_MOORGEN_SERVICE_RESOURCES        := 
Lib_PRODUCT_c_RESOURCES        := 
Lib_MiCOKit_EXT_MK3080B_RESOURCES        := 
MiCO_mocOS_Interface_RESOURCES        := 
MiCO_mocIP_Interface_RESOURCES        := 
Lib_MiCO_Kernel_RESOURCES        := 
Lib_MiCO_System_RESOURCES        := 
Lib_MiCO_Security_RESOURCES        := 
Lib_Utilities_RESOURCES        := 
MX1290_RESOURCES        := 
Lib_MGMQTT_SERVICE_RESOURCES        := 
Lib_MGKERNEL_RESOURCES        := 
Lib_VGM128064_MK3080B_RESOURCES        := 
Lib_gpio_button_MK3080B_RESOURCES        := 
Lib_dc_motor_MK3080B_RESOURCES        := 
Lib_RGB_P9813_MK3080B_RESOURCES        := 
Sensor_BME280_MK3080B_RESOURCES        := 
Sensor_DHT11_MK3080B_RESOURCES        := 
Sensor_APDS9930_MK3080B_RESOURCES        := 
Sensor_Light_adc_MK3080B_RESOURCES        := 
Sensor_Infrared_Reflective_MK3080B_RESOURCES        := 
Lib_MiCO_System_QC_RESOURCES        := 
Lib_MiCO_System_WAC_RESOURCES        := 
aws_RESOURCES        := 
Lib_MiCO_Security_SRP_6a_RESOURCES        := 
Lib_MiCO_Security_Sodium_RESOURCES        := 
Lib_JSON_C_RESOURCES        := 
GCC_RESOURCES        := 
Lib_mgmqtt_client_c_RESOURCES        := 
Lib_TLV_C_RESOURCES        := 
Lib_MFi_Auth_MK3080B_RESOURCES        := 
App_moorgen-mico_MAKEFILE         := .//moorgen-mico/moorgen-mico.mk
Board_MK3080_MAKEFILE         := ./mico-os/board/MK3080B/MK3080B.mk
mocOS_MAKEFILE         := ./mico-os/MiCO/RTOS/mocOS/mocOS.mk
mocIP_MAKEFILE         := ./mico-os/MiCO/net/mocIP/mocIP.mk
mocSSL_MAKEFILE         := ./mico-os/MiCO/security/TLS/mocSSL/mocSSL.mk
MiCO_MAKEFILE         := ./mico-os/MiCO/MiCO.mk
Lib_mgsys_c_MAKEFILE         := .//moorgenCloud/sysAdapter/mgSys/mgSys.mk
Lib_MOORGEN_SERVICE_MAKEFILE         := .//moorgenCloud/moorgenService/moorgenService.mk
Lib_PRODUCT_c_MAKEFILE         := .//product/DC114S/DC114S.mk
Lib_MiCOKit_EXT_MK3080B_MAKEFILE         := ./mico-os/libraries/drivers/MiCOKit_EXT2/MiCOKit_EXT2.mk
MiCO_mocOS_Interface_MAKEFILE         := ./mico-os/MiCO/RTOS/mocOS/mico/mico.mk
MiCO_mocIP_Interface_MAKEFILE         := ./mico-os/MiCO/net/mocIP/mico/mico.mk
Lib_MiCO_Kernel_MAKEFILE         := ./mico-os/MiCO/core/core.mk
Lib_MiCO_System_MAKEFILE         := ./mico-os/MiCO/system/system.mk
Lib_MiCO_Security_MAKEFILE         := ./mico-os/MiCO/security/security.mk
Lib_Utilities_MAKEFILE         := ./mico-os/libraries/utilities/utilities.mk
MX1290_MAKEFILE         := ./mico-os/platform/MCU/MX1290/MX1290.mk
Lib_MGMQTT_SERVICE_MAKEFILE         := .//moorgenCloud/moorgenMQTT/moorgenMQTT.mk
Lib_MGKERNEL_MAKEFILE         := .//moorgenCloud/moorgenKernel/moorgenKernel.mk
Lib_VGM128064_MK3080B_MAKEFILE         := ./mico-os/libraries/drivers/display/VGM128064/VGM128064.mk
Lib_gpio_button_MK3080B_MAKEFILE         := ./mico-os/libraries/drivers/keypad/gpio_button/gpio_button.mk
Lib_dc_motor_MK3080B_MAKEFILE         := ./mico-os/libraries/drivers/motor/dc_motor/dc_motor.mk
Lib_RGB_P9813_MK3080B_MAKEFILE         := ./mico-os/libraries/drivers/rgb_led/P9813/P9813.mk
Sensor_BME280_MK3080B_MAKEFILE         := ./mico-os/libraries/drivers/sensor/BME280/BME280.mk
Sensor_DHT11_MK3080B_MAKEFILE         := ./mico-os/libraries/drivers/sensor/DHT11/DHT11.mk
Sensor_APDS9930_MK3080B_MAKEFILE         := ./mico-os/libraries/drivers/sensor/APDS9930/APDS9930.mk
Sensor_Light_adc_MK3080B_MAKEFILE         := ./mico-os/libraries/drivers/sensor/light_adc/light_adc.mk
Sensor_Infrared_Reflective_MK3080B_MAKEFILE         := ./mico-os/libraries/drivers/sensor/infrared_adc/infrared_adc.mk
Lib_MiCO_System_QC_MAKEFILE         := ./mico-os/MiCO/system/qc_test/qc_test.mk
Lib_MiCO_System_WAC_MAKEFILE         := ./mico-os/MiCO/system/easylink/MFi_WAC/MFi_WAC.mk
aws_MAKEFILE         := ./mico-os/MiCO/system/easylink/aws/aws.mk
Lib_MiCO_Security_SRP_6a_MAKEFILE         := ./mico-os/MiCO/security/SRP_6a/SRP_6a.mk
Lib_MiCO_Security_Sodium_MAKEFILE         := ./mico-os/MiCO/security/Sodium/Sodium.mk
Lib_JSON_C_MAKEFILE         := ./mico-os/libraries/utilities/json_c/json_c.mk
GCC_MAKEFILE         := ./mico-os/platform/GCC/GCC.mk
Lib_mgmqtt_client_c_MAKEFILE         := .//moorgenCloud/sysAdapter/mgMqtt/mgMqtt.mk
Lib_TLV_C_MAKEFILE         := .//moorgenCloud/utility/tlv_c/tlv_c.mk
Lib_MFi_Auth_MK3080B_MAKEFILE         := ./mico-os/libraries/drivers/MiCODriverMFiAuth/MiCODriverMFiAuth.mk
App_moorgen-mico_PRE_BUILD_TARGETS:= 
Board_MK3080_PRE_BUILD_TARGETS:= 
mocOS_PRE_BUILD_TARGETS:= 
mocIP_PRE_BUILD_TARGETS:= 
mocSSL_PRE_BUILD_TARGETS:= 
MiCO_PRE_BUILD_TARGETS:= 
Lib_mgsys_c_PRE_BUILD_TARGETS:= 
Lib_MOORGEN_SERVICE_PRE_BUILD_TARGETS:= 
Lib_PRODUCT_c_PRE_BUILD_TARGETS:= 
Lib_MiCOKit_EXT_MK3080B_PRE_BUILD_TARGETS:= 
MiCO_mocOS_Interface_PRE_BUILD_TARGETS:= 
MiCO_mocIP_Interface_PRE_BUILD_TARGETS:= 
Lib_MiCO_Kernel_PRE_BUILD_TARGETS:= 
Lib_MiCO_System_PRE_BUILD_TARGETS:= 
Lib_MiCO_Security_PRE_BUILD_TARGETS:= 
Lib_Utilities_PRE_BUILD_TARGETS:= 
MX1290_PRE_BUILD_TARGETS:= 
Lib_MGMQTT_SERVICE_PRE_BUILD_TARGETS:= 
Lib_MGKERNEL_PRE_BUILD_TARGETS:= 
Lib_VGM128064_MK3080B_PRE_BUILD_TARGETS:= 
Lib_gpio_button_MK3080B_PRE_BUILD_TARGETS:= 
Lib_dc_motor_MK3080B_PRE_BUILD_TARGETS:= 
Lib_RGB_P9813_MK3080B_PRE_BUILD_TARGETS:= 
Sensor_BME280_MK3080B_PRE_BUILD_TARGETS:= 
Sensor_DHT11_MK3080B_PRE_BUILD_TARGETS:= 
Sensor_APDS9930_MK3080B_PRE_BUILD_TARGETS:= 
Sensor_Light_adc_MK3080B_PRE_BUILD_TARGETS:= 
Sensor_Infrared_Reflective_MK3080B_PRE_BUILD_TARGETS:= 
Lib_MiCO_System_QC_PRE_BUILD_TARGETS:= 
Lib_MiCO_System_WAC_PRE_BUILD_TARGETS:= 
aws_PRE_BUILD_TARGETS:= 
Lib_MiCO_Security_SRP_6a_PRE_BUILD_TARGETS:= 
Lib_MiCO_Security_Sodium_PRE_BUILD_TARGETS:= 
Lib_JSON_C_PRE_BUILD_TARGETS:= 
GCC_PRE_BUILD_TARGETS:= 
Lib_mgmqtt_client_c_PRE_BUILD_TARGETS:= 
Lib_TLV_C_PRE_BUILD_TARGETS:= 
Lib_MFi_Auth_MK3080B_PRE_BUILD_TARGETS:= 
App_moorgen-mico_PREBUILT_LIBRARY := 
Board_MK3080_PREBUILT_LIBRARY := 
mocOS_PREBUILT_LIBRARY := 
mocIP_PREBUILT_LIBRARY := 
mocSSL_PREBUILT_LIBRARY := 
MiCO_PREBUILT_LIBRARY := 
Lib_mgsys_c_PREBUILT_LIBRARY := 
Lib_MOORGEN_SERVICE_PREBUILT_LIBRARY := 
Lib_PRODUCT_c_PREBUILT_LIBRARY := 
Lib_MiCOKit_EXT_MK3080B_PREBUILT_LIBRARY := 
MiCO_mocOS_Interface_PREBUILT_LIBRARY := 
MiCO_mocIP_Interface_PREBUILT_LIBRARY := 
Lib_MiCO_Kernel_PREBUILT_LIBRARY := 
Lib_MiCO_System_PREBUILT_LIBRARY := 
Lib_MiCO_Security_PREBUILT_LIBRARY := 
Lib_Utilities_PREBUILT_LIBRARY := 
MX1290_PREBUILT_LIBRARY := 
Lib_MGMQTT_SERVICE_PREBUILT_LIBRARY := 
Lib_MGKERNEL_PREBUILT_LIBRARY := 
Lib_VGM128064_MK3080B_PREBUILT_LIBRARY := 
Lib_gpio_button_MK3080B_PREBUILT_LIBRARY := 
Lib_dc_motor_MK3080B_PREBUILT_LIBRARY := 
Lib_RGB_P9813_MK3080B_PREBUILT_LIBRARY := 
Sensor_BME280_MK3080B_PREBUILT_LIBRARY := 
Sensor_DHT11_MK3080B_PREBUILT_LIBRARY := 
Sensor_APDS9930_MK3080B_PREBUILT_LIBRARY := 
Sensor_Light_adc_MK3080B_PREBUILT_LIBRARY := 
Sensor_Infrared_Reflective_MK3080B_PREBUILT_LIBRARY := 
Lib_MiCO_System_QC_PREBUILT_LIBRARY := 
Lib_MiCO_System_WAC_PREBUILT_LIBRARY := ./mico-os/MiCO/system/easylink/MFi_WAC/Lib_MFi_WAC.Cortex-M4F.GCC.release.a
aws_PREBUILT_LIBRARY := ./mico-os/MiCO/system/easylink/aws/aws.Cortex-M4F.GCC.release.a
Lib_MiCO_Security_SRP_6a_PREBUILT_LIBRARY := ./mico-os/MiCO/security/SRP_6a/Lib_SRP6a.Cortex-M4F.GCC.release.a
Lib_MiCO_Security_Sodium_PREBUILT_LIBRARY := ./mico-os/MiCO/security/Sodium/Lib_Sodium.Cortex-M4F.GCC.release.a
Lib_JSON_C_PREBUILT_LIBRARY := 
GCC_PREBUILT_LIBRARY := 
Lib_mgmqtt_client_c_PREBUILT_LIBRARY := 
Lib_TLV_C_PREBUILT_LIBRARY := 
Lib_MFi_Auth_MK3080B_PREBUILT_LIBRARY := 
MiCO_SDK_UNIT_TEST_SOURCES   		:=                                                                            
ALL_RESOURCES             		:= 
INTERNAL_MEMORY_RESOURCES 		:= 
EXTRA_TARGET_MAKEFILES 			:=   ./mico-os/makefiles/mico_moc_targets.mk
APPS_START_SECTOR 				:=  
BOOTLOADER_FIRMWARE				:=  
ATE_FIRMWARE				        :=  
APPLICATION_FIRMWARE				:=  
PARAMETER_1_IMAGE					:=  
PARAMETER_2_IMAGE					:=  
FILESYSTEM_IMAGE					:=  
WIFI_FIRMWARE						:=  
BT_PATCH_FIRMWARE					:=  
MiCO_ROM_SYMBOL_LIST_FILE 		:= 
MiCO_SDK_CHIP_SPECIFIC_SCRIPT		:=                                      
MiCO_SDK_CONVERTER_OUTPUT_FILE	:=                                      
MiCO_SDK_FINAL_OUTPUT_FILE 		:=                                      
MiCO_RAM_STUB_LIST_FILE 			:= 
MOC_KERNEL_BIN_FILE 				:= ./mico-os/resources/moc_kernel/3080B/kernel.bin
MOC_APP_OFFSET 				:= 0x75000
