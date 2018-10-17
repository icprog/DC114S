#include "mgOS.h"
#include "moorgenService.h"
#include "moorgenWifiConfig.h"

#define CONFIG_TimeOut                90000 /**< EasyLink timeout 60 seconds. */

#define CONFIG_ConnectWlan_Timeout    10000 /**< Connect to wlan after configured by easylink.
                                                   Restart easylink after timeout: 20 seconds. */


static int wifi_status;
static mg_semaphore_t station_up_sem = NULL;
static mg_semaphore_t config_connect_sem = NULL;
static mg_semaphore_t config_sem = NULL;

static system_context_t  *inContext=NULL;		
static mg_thread_t softap_thread_handler = NULL;

static int config_success = 0; 



static void mgNotify_WifiStatusHandler(WiFiEvent event, system_context_t * const inContext)
{
      switch (event) {
      case NOTIFY_STATION_UP:

        if(config_connect_sem!=NULL)		//���Ĭ����wifi���ã�ģ������APģʽ��������config_connect_sem�ź���
        {
            inContext->flashContentInRam.micoSystemConfig.configured = allConfigured;
            mico_system_context_update( &inContext->flashContentInRam ); //Update Flash content
            
            mg_rtos_set_semaphore( &config_connect_sem ); //Notify Easylink thread
        }
      
        if(station_up_sem!=NULL)
        {
            mico_rtos_set_semaphore( &station_up_sem );
        }
        wifi_status=MG_WIFI_STA_CONNECTED;

        moorgen_service_led_set(LED_WIFI_CONNECTED);
        break;
      case NOTIFY_STATION_DOWN:
        wifi_status=MG_WIFI_STA_DISCONNECTED;
        moorgen_service_led_set(LED_WIFI_CONNECTING);
        break;
      case NOTIFY_AP_UP:
        wifi_status=MG_WIFI_AP_START;
        break;
      case NOTIFY_AP_DOWN:
        wifi_status=MG_WIFI_AP_STOP;
        break;
      default:
        break;
      }
      return;
}


void mg_wifi_softap_start()
{
    network_InitTypeDef_st wNetConfig;
    unsigned char mac[8] = {0};

    
    wlan_get_mac_address(mac);		//��ȡ�豸mac ��ַ
    
    memset( &wNetConfig, 0, sizeof(network_InitTypeDef_st) );
    wNetConfig.wifi_mode = Soft_AP;
    snprintf( wNetConfig.wifi_ssid, 32, "%s_%02x%02x_%s",
               CONFIG_FACTORY,mac[4],mac[5] , CONFIG_DEVICE_TYPE );
    strcpy( (char*) wNetConfig.wifi_key, "" );
    strcpy( (char*) wNetConfig.local_ip_addr, "10.10.2.1" );
    strcpy( (char*) wNetConfig.net_mask, "255.255.255.0" );
    strcpy( (char*) wNetConfig.gateway_ip_addr, "10.10.2.1" );
    wNetConfig.dhcpMode = DHCP_Server;
    micoWlanStart( &wNetConfig );
    
    mgos_log("Establish soft ap: %s.....", wNetConfig.wifi_ssid);

}




void mg_wifi_config_suc(char *ssid, char *password)
{


    mico_rtos_lock_mutex( &inContext->flashContentInRam_mutex );

	inContext->flashContentInRam.micoSystemConfig.easyLinkByPass = EASYLINK_BYPASS_NO;

	//ssid
	strncpy(inContext->flashContentInRam.micoSystemConfig.ssid, ssid, 32);
	inContext->flashContentInRam.micoSystemConfig.channel = 0;
	memset( inContext->flashContentInRam.micoSystemConfig.bssid, 0x0, 6 );
	inContext->flashContentInRam.micoSystemConfig.security = SECURITY_TYPE_AUTO;

	//password
	strncpy( inContext->flashContentInRam.micoSystemConfig.key,password, 64);
	strncpy( inContext->flashContentInRam.micoSystemConfig.user_key, password, 64 );
	inContext->flashContentInRam.micoSystemConfig.keyLength = strlen( inContext->flashContentInRam.micoSystemConfig.key );
	inContext->flashContentInRam.micoSystemConfig.user_keyLength = strlen( inContext->flashContentInRam.micoSystemConfig.key );
   	 inContext->flashContentInRam.micoSystemConfig.dhcpEnable = true;

	inContext->flashContentInRam.micoSystemConfig.dhcpEnable = DHCP_Client;	//����Ӧע������ΪDHCP_Client,Ϊ֮����ΪSTATIONģʽ
    	mico_rtos_unlock_mutex( &inContext->flashContentInRam_mutex );

	config_success=1;

    

    mg_rtos_set_semaphore( &config_sem ); //Notify Easylink thread


}

void mg_wifi_config_failed()
{

        mgos_log("mg_wifi_config_failed");
        moorgen_service_led_set(LED_WIFI_CONNECTING);

        mgos_log("Roll back to previous settings");
        MICOReadConfiguration( inContext );
        if(strlen(inContext->flashContentInRam.micoSystemConfig.ssid)!=0)
        {
            inContext->flashContentInRam.micoSystemConfig.configured = allConfigured;
            mico_system_context_update( &inContext->flashContentInRam);   //inContext

            system_connect_wifi_normal( inContext);	//inContext
        }

        else {

            mgos_log("Wi-Fi power off");
            micoWlanPowerOff( );
        }

}



void mg_wifi_info_get(char *ssid, char *ip , char *bssid)
{

    IPStatusTypedef outNetpara;
    
    mgos_log("mg_wifi_info_get");

    strncpy(ssid,inContext->flashContentInRam.micoSystemConfig.ssid,32);


    if(micoWlanGetIPStatus (&outNetpara, Station)==kNoErr)
    {
        strncpy(ip,outNetpara.ip,16);
    }
    
    



}


int mg_wifi_status_get()
{
    return wifi_status;
}



void mg_softap_thread( uint32_t args )
{
    OSStatus err = kNoErr;

    mgos_log("mg_softap_thread");

    mg_rtos_init_semaphore( &config_sem, 1 );
    mg_rtos_init_semaphore( &config_connect_sem, 1 );

    moorgen_service_led_set(LED_WIF_CONFIG);


restart:

    config_success = 0;		//��־λ: ��û���óɹ�

    micoWlanSuspend( );			//�ر�wifi ����
    mg_rtos_thread_msleep( 200 );


    mg_wifi_softap_start();		//����AP�ȵ�, ssid :  connector_mac[4]mac[5]_22000001   passward :   ��

    moorgen_wifi_config_start();	//����UDP�߳� , ����������Ϣ����: msgType , ssid    ,   passward���������豸���
    								//�������ֻ�APP��������ģ���ssid,password, dhcp_client �Ȳ���(CONFIG_REG,��WIFI_SET)
    while( mg_rtos_get_semaphore( &config_sem, 0 ) == kNoErr );	//EasyLink timeout 60 seconds
    mg_rtos_get_semaphore( &config_sem, CONFIG_TimeOut );		//�ȴ�60s  EasyLink����

    micoWlanSuspendSoftAP();	//�˳�APģʽ


    if ( config_success == 1 ) {		//���֮ǰ�˳�APģʽ���������ɹ�

        mico_thread_sleep(1);

        moorgen_wifi_config_stop();	//ɾ��mg_config_thread(config_client_thread���) UDP�����̣߳��ر�udp_config.socket

        moorgen_service_led_set(LED_WIFI_CONNECTING);

        system_connect_wifi_normal(inContext );	//STATIONģʽ������wifi

        /* Wait for station connection */
        while ( mg_rtos_get_semaphore( &config_connect_sem, 0 ) == kNoErr );
        err = mg_rtos_get_semaphore( &config_connect_sem, CONFIG_ConnectWlan_Timeout );	//�ȴ�20s wifi �Ƿ����ӳɹ�

        
           
        /*SSID or Password is not correct, module cannot connect to wlan, so restart EasyLink again*/
        require_noerr_action_string( err, restart, micoWlanSuspend(), "Re-start easylink softap mode" );

        goto exit;
    }
    else /* EasyLink failed */
    {
        mg_wifi_config_failed();		//AP����ʧ�ܣ��ָ�Ĭ��ssid  password ȥ����wifi

    }

exit:

    moorgen_wifi_config_stop();

    mg_rtos_deinit_semaphore( &config_sem );
    mg_rtos_deinit_semaphore( &config_connect_sem );
    softap_thread_handler = NULL;
    config_sem=NULL;
    config_connect_sem=NULL;
    mico_rtos_delete_thread( NULL );
}



void  mg_wifi_start()
{
//	net_para_st para;
	
    mgos_log("mg_wifi_start");
    inContext =system_context();

     
    mg_rtos_init_semaphore(&station_up_sem,1);
    mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED, (void *)mgNotify_WifiStatusHandler, (void *) inContext );

    //moorgen_sys_info_update();

    if( inContext->flashContentInRam.micoSystemConfig.configured == unConfigured){	//Ĭ��û������wifi
        system_log("Empty configuration. Starting configuration mode...");
        mico_rtos_create_thread( &softap_thread_handler, MG_APPLICATION_PRIORITY, "CONFIG AP",
                                       mg_softap_thread, 0x1000, (mico_thread_arg_t) 0 );
   }
   else
   {
        system_log("Available configuration. Starting Wi-Fi connection...");
        moorgen_service_led_set(LED_WIFI_CONNECTING);
        system_connect_wifi_fast(inContext );
   }

    
   
    mico_rtos_get_semaphore( &station_up_sem, MG_WAIT_FOREVER );  //�ȴ������ȵ�ɹ�(��mgNotify_WifiStatusHandler��)


    moorgen_sys_info_update();


    if(station_up_sem != NULL )
    {
        mico_rtos_deinit_semaphore( &station_up_sem );
        station_up_sem=NULL;
    }
/*
	micoWlanGetIPStatus(&para, Station);
	mgos_log("wifi dhcp : %d",para.dhcp);
	mgos_log("wifi dhcp : %s",para.ip);
	mgos_log("wifi dhcp : %s",para.gate);
*/

    //mg_rtos_thread_sleep(2);

}


