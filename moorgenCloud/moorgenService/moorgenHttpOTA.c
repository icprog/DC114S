#include "mgOS.h"
#include "moorgenHttpOTA.h"
#include "mgMqttService.h"
#include "moorgenService.h"

#define hota_log(M, ...) //custom_log("HTTP OTA", M, ##__VA_ARGS__)
#define hota_log_trace() custom_log_trace("HTTP OTA")

//#define TMP_BUF_LEN         4608
//#define MAX_OTA_RECV_SIZE   4096

#define TMP_BUF_LEN         8704
#define MAX_OTA_RECV_SIZE   8192

#define OTA_LED_TRIGGER_INTERVAL 1000 

#define STACK_SIZE_OTA_TCP_SERVER_THREAD   0x400


static http_file_info_t  otaFileInfo;
static char g_buf_send[200];//发送数据暂存区

static int is_mg_ota_established=0;



static int ota_socket_connect()
{
    int retry_cnt = 3;
    int sockfd;
    struct sockaddr_in addr;

//hota_log("ota_socket_connect");

    while (retry_cnt--) {
        sockfd= socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd >= 0)
            break;
        /* Wait some time to allow some sockets to get released */
        mg_rtos_thread_sleep(1);
    }
    
    if (sockfd < 0) {
        hota_log("Unable to create hota socket to stop server");
        return -1;
    }
    

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(otaFileInfo.httpPort);		//80
    addr.sin_addr.s_addr=inet_addr(otaFileInfo.httpIp);



 //   hota_log("OTA connecting ...");
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        hota_log("OTA Server close error. tcp connect failed %s:%d", otaFileInfo.httpIp, otaFileInfo.httpPort);
        close(sockfd);
        return -1;
    }


    return sockfd;

}


static int ota_socket_send(int socketfd,uint8_t *sendbuf, int sendlen)
{
    int       bytes = 0;

    uint8_t     error=0;

    while(sendlen > 0)
    {
        bytes = send(socketfd, sendbuf, sendlen, 0);
        if(bytes < 0)
        {
            error++;
            if(error>=3)
                return -1;

            mg_rtos_thread_msleep(100);
        }
        else
        {
            sendlen -= bytes;
            sendbuf += bytes;
        }
    }
    
    return 0;

}


 

//acquire msg :  body
static int ota_http_get_content_length(char *revbuf)
{
    char *p1 = NULL, *p2 = NULL;
    int HTTP_Body = 0;

    p1 = strstr(revbuf,"Content-Length");
    if(p1 == NULL)
        return 0;
    else
    {
        p2 = p1+strlen("Content-Length")+ 2; 
        HTTP_Body = atoi(p2);
        return HTTP_Body;
    }

}
//acquire msg:  head + body
static int ota_http_get_recv_length(char *revbuf)
{
    char *p1 = NULL;
    int HTTP_Body = 0;
    int HTTP_Head = 0;


    HTTP_Body = ota_http_get_content_length(revbuf);
    if(HTTP_Body == -1)
        return -1;

    p1=strstr(revbuf,"\r\n\r\n");
    if(p1==NULL)
        return -1;
    else
    {
        HTTP_Head = p1- revbuf +4;			//数据:  \r\n\r\n + 消息体
        return HTTP_Body+HTTP_Head;
    }


}




//从HTTP 服务器获取要下载的数据
static int ota_http_recv(int sockfd,char *buf_recv)
{
    int ret;
    int recvlen=0;
    int downloadlen = 0;


    fd_set fds;
    struct timeval   t;

     //       hota_log("ota_http_recv");


    while(1)
    {

        t.tv_sec = 10;
        t.tv_usec = 0;

    	FD_ZERO(&fds);
    	FD_SET(sockfd, &fds);

		if(select(sockfd+1, &fds, NULL, NULL, &t)<=0)
		{
			hota_log("ERR:select fail");
			return -1;
		}

        ret = recv(sockfd,otaFileInfo.filebuf+recvlen,TMP_BUF_LEN,0);

        if(ret <= 0)
        {
            hota_log("ERR:recv fail");
            return ret;
        }
    

        if(recvlen == 0)
        {
        //    hota_log("recv len = %d", ret);
            downloadlen = ota_http_get_recv_length((char *)otaFileInfo.filebuf);	//acquire msg body  length
          //  hota_log("downloadlen = %d",downloadlen);
            if(downloadlen>TMP_BUF_LEN)		//>8704
            {
                hota_log("ERR:downloadlen>TMP_BUF_LEN");
                return -1;
            }
        }   


        recvlen += ret;
       // hota_log("total recvlen = %d\n",recvlen);

        if(downloadlen == recvlen)
            break;


    }

    return recvlen;

}

//acquire  file name , End with   '/'
static int ota_get_file_name()
{
    int len;
    char *p;
    int offset;
    p = strstr(otaFileInfo.url,"http://");
    if(p == NULL)
    {
        offset = 0;
    }
    else
    {
        offset = strlen("http://");
    }

    len = strlen(otaFileInfo.url);			//acqurie  URL  length
    while(otaFileInfo.url[offset]!='\0')
    {
        if(otaFileInfo.url[offset] == '/')
            break;
        offset ++;
    }
    if(offset== len)
    {
        hota_log("url not contain '/'");
        return -1;
    }
    else
    {
        strcpy(otaFileInfo.filename,otaFileInfo.url+offset);
        hota_log("filename=%s\n",otaFileInfo.filename);
        return 0;
    }
}


static int ota_get_ip_port()
{
    char *p = NULL;
    int offset = 0;
    char DOMAIN_NAME[128];
    struct hostent *site;
    char **pptr = NULL;
    struct in_addr in_addr;

    p = strstr(otaFileInfo.url,"http://");
    if(p == NULL)
    {
        offset = 0;
    }
    else
    {
        offset = strlen("http://");
    }

    p = strchr(otaFileInfo.url+offset,'/');
    if(p == NULL)
    {
        hota_log("url:%s format error",otaFileInfo.url);
        return -1;
        
    }
    else
    {

        memset(DOMAIN_NAME,0x0,sizeof(DOMAIN_NAME));
        memcpy(DOMAIN_NAME,otaFileInfo.url+offset,(p-otaFileInfo.url-offset));	//在URL中获取域名
        p = strchr(DOMAIN_NAME,':');
        if(p == NULL)
        {
            site = gethostbyname(DOMAIN_NAME);	//通过域名获取 ip 相关信息

        	if(site==NULL) 
            {
                printf("DOMAIN_NAME error!\n");
                return -1;
            }
       
     //       strcpy(otaFileInfo.httpIp,DOMAIN_NAME);
            otaFileInfo.httpPort=80;
            pptr=site->h_addr_list;			//得到ip 地址(网络字节序)
            in_addr.s_addr = *(uint32_t *)(*pptr);
            strcpy(otaFileInfo.httpIp, inet_ntoa(in_addr));

            hota_log("ip=%s,port=%d\n",otaFileInfo.httpIp,otaFileInfo.httpPort);//debug info

            return 1;

        }
        else
        {    


            *p = '\0';
            site = gethostbyname(DOMAIN_NAME);

        	if(site==NULL) 
            {
                printf("DOMAIN_NAME error!\n");
                return -1;
            }
            otaFileInfo.httpPort=atoi(p+1);
            pptr=site->h_addr_list;
            in_addr.s_addr = *(uint32_t *)(*pptr);
            strcpy(otaFileInfo.httpIp, inet_ntoa(in_addr));
            hota_log("ip=%s,port=%d\n",otaFileInfo.httpIp,otaFileInfo.httpPort);//debug info

            return 2;

        }


    }

}

static void http_package_url_get_file(char *range)
{
    char buf[64];
    memset(g_buf_send,0x0,sizeof(g_buf_send));         
    sprintf(g_buf_send, "GET %s",otaFileInfo.filename);

    
    strcat(g_buf_send," HTTP/1.1\r\n");
    strcat(g_buf_send, "Host: ");
    strcat(g_buf_send, otaFileInfo.httpIp);
    //strcat(g_buf_send, ":");
    //strcat(g_buf_send, otaFileInfo.httpPort);
    
    sprintf(buf, "\r\nRange: bytes=%s",range);
    strcat(g_buf_send,buf);
    strcat(g_buf_send, "\r\nKeep-Alive: 200");
    strcat(g_buf_send,"\r\nConnection: Keep-Alive\r\n\r\n");
    

}

//(HEAD) (otaFileInfo.filename) (HTTP/1.1\r\n) (Host:) ( otaFileInfo.httpIp) (\r\nConnection: Keep-Alive\r\n\r\n)
static void http_package_url_get_filesize()
{
    
    memset(g_buf_send,0x0,sizeof(g_buf_send));         
    sprintf(g_buf_send, "HEAD %s",otaFileInfo.filename);

    strcat(g_buf_send," HTTP/1.1\r\n");
    strcat(g_buf_send, "Host: ");
    strcat(g_buf_send, otaFileInfo.httpIp);
    //strcat(g_buf_send, ":");
    //strcat(g_buf_send, otaFileInfo.httpPort);
    strcat(g_buf_send,"\r\nConnection: Keep-Alive\r\n\r\n");

}



static int ota_http_get_file_size()
{

    int rcvLen;
    int ret=0;
    int sockfd;

    sockfd=ota_socket_connect();		//连接 http 服务器
    if(sockfd<0) return 0;

    http_package_url_get_filesize();	//(HEAD) (otaFileInfo.filename) (HTTP/1.1\r\n) (Host:) ( otaFileInfo.httpIp) (\r\nConnection: Keep-Alive\r\n\r\n)
											
    if(ota_socket_send(sockfd, (uint8_t *)g_buf_send, strlen(g_buf_send))<0)	//发送
    {           
        close(sockfd);
        return 0;
    }
   
    rcvLen=recv( sockfd, otaFileInfo.filebuf,TMP_BUF_LEN, 0);

    if(rcvLen <= 0)
    {
        hota_log("ERR:recv fail GetFileSize()");
        close(sockfd);
        return 0;
    } 
  //  hota_log("%s",otaFileInfo.filebuf);


    ret=ota_http_get_content_length((char *) otaFileInfo.filebuf);	//获取msg  body  length

    close(sockfd);
    return ret;

}

static int ota_get_file()
{
    int count;
    char range[32];
    int i,j;
    int sockfd=-1;
    int ret = 0;
    char *p = NULL;
    uint32_t filelength=otaFileInfo.filelen;
    int recvLen=0;
    uint8_t percent=0;
    uint8_t tmp=0;
    uint8_t recv_error=0;
 
    hota_log("ota_get_file");


	//得到要下载count 次才能全部下载完成
    count = (filelength%MAX_OTA_RECV_SIZE)?(filelength/MAX_OTA_RECV_SIZE +1):(filelength/MAX_OTA_RECV_SIZE);
    hota_log("conut:%d",count);

    for(i=0;i<count;i++)
    {

        hota_log("%d",i);

    	for(j=0;j<3;j++)
    	{
			sockfd=ota_socket_connect();	//尝试去连接http服务器
			{
				if(sockfd>=0) break;

			}
			mg_rtos_thread_sleep(1);
    	}

    	if(sockfd<0) return 0;

        if((i == (count-1))&&(filelength%MAX_OTA_RECV_SIZE))		//这次最后一次下载，并且下载的数据不足8192
        {
            sprintf(range,"%ld-%ld",i*MAX_OTA_RECV_SIZE,filelength-1);	//最后多余(不满8192)的数据范围
            recvLen=filelength-i*MAX_OTA_RECV_SIZE;					//这次要下载(接收)的数据长度

        }
        else
        {
            sprintf(range,"%ld-%ld",i*MAX_OTA_RECV_SIZE,(i+1)*MAX_OTA_RECV_SIZE-1);	//下载的范围
            recvLen=MAX_OTA_RECV_SIZE;											//这次要下载(接收)的数据长度

        }

        http_package_url_get_file(range);										//组合要下载数据的 格式报文
   //     hota_log("%s",g_buf_send);

        if(ota_socket_send(sockfd, (uint8_t *)g_buf_send, strlen(g_buf_send))<0)   //发送报文
        {
            ret= -1;
            goto OTA_EXIT;
        }

        memset(otaFileInfo.filebuf,0,TMP_BUF_LEN);               	// TMP_BUF_LEN (8704) 必须大于  MAX_OTA_RECV_SIZE (8192)                        
        ret = ota_http_recv(sockfd,(char *)otaFileInfo.filebuf);	//返回从http下载的数据长度
/*
        if(ret < 0)
            break;*/
        if(ret <= 0 )					//接收出错
        {
            close(sockfd);
            sockfd=-1;
          //  sockfd = hotaSocketConnect();
            i--;
            recv_error ++;

            if(recv_error>=3)
                return 0;
                
            continue;
        }
        recv_error=0;					//下载(接受)正常，判断是否有 起始符:  \r\n\r\n
        p = strstr((char *)otaFileInfo.filebuf,"\r\n\r\n");
        if(p == NULL)
        {
            hota_log("ERR:g_buf_recv not contain end flag");
            ret= -1;
            goto OTA_EXIT;
        }
        else						//接收正常，且含有起始符 \r\n\r\n
        {

            if(mg_ota_flash_write(&otaFileInfo.flashaddr,(uint8_t *)(p+4),recvLen)<0)		//将下载的数据写入flash中
            {																		//去除起始头部\r\n\r\n
                ret= -1;
                hota_log("ERR:hotaSaveFile");

                goto OTA_EXIT;
            }
		/* 计算已经下载的百分表，并上报MQTT */
            if(i==(count-1)) tmp=10;
            else
            {
                tmp= ((100*i)/(count-1))/10;
            }
            
            
            if(percent!=tmp)
            {
                percent=tmp;
                hota_log("percent:%d",percent*10);

                mqtt_rate_push_report(otaFileInfo.mac,otaFileInfo.deviceType,percent*10);
            }


            

            
        }
        close(sockfd);
        sockfd=-1;
    }
    

OTA_EXIT:
	if(sockfd>=0)
		close(sockfd);
    if(ret<0)
        return 0;
    else
        return filelength;

}




static int ota_http_download_file()
{

    if(ota_get_ip_port()<0)		//通过域名，得到http  IP信息
        return 0;

    if(ota_get_file_name()<0)	//获取filename， /xxxx
        return 0;


    otaFileInfo.filelen=ota_http_get_file_size();	//获取要下载的数据body length
    if(otaFileInfo.filelen == 0)
        return 0;
    hota_log("file_len:%ld",otaFileInfo.filelen);


    otaFileInfo.flashaddr = 0;

    if(mg_ota_begin()<0)		//擦除OTA分区
        return 0;
    return ota_get_file();		//下载


}



void ota_finished(int result)
{

    hota_log("ota_finished");
    hota_log("result:%d",result);
    
    mqtt_ota_update_rsp(result,NULL);

    moorgen_service_led_set(LED_LAST);

    switch(result) {
    case MG_OTA_UPDATE_SUCCESS:
        mg_rtos_thread_sleep(3);		
        mg_os_reboot();					//更新成功，重启系统
        break;
    case MG_OTA_CHECK_FAILURE:
        break;
    case MG_OTA_NO_NEW_VERSION:
        break;
    case MG_OTA_UPDATE_FAILURE:
        hota_log("ERROR!! download_file error");
        break;
    case MG_OTA_MD5_FAIL:
        hota_log("ERROR!! md5 error");
        break;
    case MG_OTA_NO_MEM:
        hota_log("ERROR!! Can't get enough memory");
        break;
    case MG_OTA_NO_FILE:
        break;
    default:
        break;
    }
}


void ota_thread(mg_thread_arg_t arg)
{

    uint32_t filelen; 


    moorgen_is_ota_set(1);		//moorgen_sys_info.is_ota=set_flag; 表示OTA 线程开启


    if(strstr(otaFileInfo.url,moorgen_module_type_get())==NULL)	//url包含 模块类型?
    {
        otaFileInfo.result=MG_OTA_CHECK_FAILURE;
        goto exit;
    }
    
   
    otaFileInfo.filebuf= (uint8_t*)malloc(TMP_BUF_LEN);		//申请filebuf 内存空间，存放一次下载的数据
    if (otaFileInfo.filebuf == NULL) {
        otaFileInfo.result=MG_OTA_NO_MEM;
        goto exit;
    }
    moorgen_service_led_set(LED_HUB_UPDATE);

    
    

    if((filelen=ota_http_download_file ())==0)	//OTA 下载固件
    {


        otaFileInfo.result=MG_OTA_UPDATE_FAILURE;
        goto exit;
    }

	//到此，OTA已下载成功,并成功写入到flash中
    hota_log("tftp download image finished, OTA bin len %ld", filelen);

    if(mg_ota_end(otaFileInfo.filelen,otaFileInfo.md5_recv)<0)		//MD5校验
    {
        otaFileInfo.result=MG_OTA_MD5_FAIL;
        goto exit;
    }


    otaFileInfo.result=MG_OTA_UPDATE_SUCCESS;

 exit:

    is_mg_ota_established = 0;				//没有建立OTA 连接
    if(otaFileInfo.filebuf!=NULL)
        free(otaFileInfo.filebuf);				//释放内存空间
    moorgen_is_ota_set(0);				//标记OTA 线程是否存活
    ota_finished(otaFileInfo.result);			
	mg_rtos_delete_thread(NULL );		//删除OTA 线程

 
}


void moorgen_ota_server_start(char *url,char *md5Str,char *mac ,char *deviceType)
{
    uint8_t i;
    char str[4];
    hota_log("ota_server_start");
    hota_log("url:%s",url);
    hota_log("md5:%s",md5Str);

    if( is_mg_ota_established )
	{
        return;
	}

  	is_mg_ota_established = 1;



    memset(&otaFileInfo,0,sizeof(otaFileInfo));

//acquire  md5
    for(i=0;i<16;i++)
    {
        str[0]=md5Str[i*2];
        str[1]=md5Str[i*2+1];
        str[2]='\0';
        otaFileInfo.md5_recv[i]=strtol(str, NULL, 16);	//字符串转成16进制，以NULL(\0)为结束符

    }
	
    strncpy(otaFileInfo.url,url,200);				//acquire url
    strncpy(otaFileInfo.mac,mac,20);				//acuire mac
    strncpy(otaFileInfo.deviceType,deviceType,16);	//acquire deviceType

    
    
    mg_rtos_create_thread(NULL, MG_APPLICATION_PRIORITY, "OTA Server", ota_thread, 0x2000,  (uint32_t)NULL );
}



