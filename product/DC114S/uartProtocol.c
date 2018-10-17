/**
******************************************************************************
* @file    uartProtocol.c 
* @author  Nicky Zhang
* @version V1.0.0
* @date    23-Oct-2017
* @brief   This file create a UART recv thread.
******************************************************************************
*
*  Copyright (c) 2015 DOOYA Inc.

******************************************************************************
*/

/*---------------------------------------------------------------------------*/
/* Includes*/
/*---------------------------------------------------------------------------*/

#include "uartProtocol.h"
#include "product.h"

#define UART_TIMER_MS 500

#define UART_RECV_TIMEOUT                   300
#define UART_RING_BUFFER_LENGTH             1024
   
#define STACK_SIZE_UART_RECV_THREAD         0x400

#define LIST_BIT_FULL   0xffffffff
#define LIST_HEAD_BIT   0

#define uart_log(M, ...)                     //custom_log("uartProtocol", M, ##__VA_ARGS__)

static volatile int motor_run_sta;
static mico_timer_t uart_timer;


volatile ring_buffer_t  rx_buffer;
volatile uint8_t        rx_data[UART_RING_BUFFER_LENGTH];
static IS_UART_OB       uart_ob;
DEVICE_STATUS    dev_status;

static mico_mutex_t list_mutex;

static LIST_SEND_DATA *head=NULL;
static LIST_SEND_DMG  *plistData;


static void  listLock()
{
    mico_rtos_lock_mutex(&list_mutex);

}

static void listUnlock()
{
    mico_rtos_unlock_mutex(&list_mutex);
}


static void  sendListInit(void)
{

    plistData=(LIST_SEND_DMG *)calloc(1,sizeof(LIST_SEND_DMG));
    plistData->total=LIST_SEND_ID_MAX;
    plistData->bitSet[0]=0x01;
    head=&plistData->data[LIST_HEAD_BIT];
    head->next=0;
    head->prev=0;

    mico_rtos_init_mutex(&list_mutex);
}


static UINT8 sendIDGet()
{
    UINT8 i,j;

    UINT8 ret;
    UINT32 value = 0;

    if(plistData->inUse >= plistData->total)
    {
	    return LIST_SEND_ID_INVALID;
	}
       
    
    for(i = 0; i < LIST_SEND_ID_BIT; i++)
    {
        if(plistData->bitSet[i] != LIST_BIT_FULL)
        {
            break;
        }
    }

    if(i >= LIST_SEND_ID_BIT)
    {
        return LIST_SEND_ID_INVALID;
    }

    for(j = 0; j < 32; j ++)
    {
        value = 0x01 << j;
	    if(0 == (plistData->bitSet[i] & value))
	    {
	        break;
	    }
    }

	ret = i*32 + j;

   	if(ret>=LIST_SEND_ID_MAX)
	{
		return LIST_SEND_ID_INVALID;
	}

    plistData->bitSet[i] |= value;
    plistData->inUse ++;
    
    return ret;
}



static void sendIDSet(UINT8 dataID)
{
    UINT32 value = 0x01;
    UINT8 iii,jjj;

    if(dataID >= LIST_SEND_ID_MAX)
    {
        return ;
    }


    iii = dataID/32;
    jjj = dataID%32;


    value <<= jjj;
    value = ~value;
	
    plistData->bitSet[iii] &= value;
    if(plistData->inUse>0)
        plistData->inUse --;
    memset(&plistData->data[dataID], 0, sizeof(LIST_SEND_DATA));
	
}




int sendListAdd(UINT8 *data,UINT8 dataLen)
{
    UINT8 id=LIST_SEND_ID_INVALID;
    LIST_SEND_DATA *cur=NULL;
    LIST_SEND_DATA *end=NULL;

    listLock();

    id=sendIDGet();
    if(id==LIST_SEND_ID_INVALID)
    {   
        listUnlock();
        return -1;
    }

    cur=&plistData->data[id];
    end=&plistData->data[head->prev];

    cur->next=LIST_HEAD_BIT;
    cur->prev=head->prev;
    head->prev=id;
    end->next=id;
    cur->datalen=dataLen;
    memcpy(cur->data,data,LIST_DATA_LEN);
    listUnlock();
    return 0;
}


static int sendListDel(UINT8 id)
{
    LIST_SEND_DATA *cur=NULL;

    listLock();

    cur=&plistData->data[id];
	plistData->data[cur->prev].next=cur->next;
    plistData->data[cur->next].prev=cur->prev;
    sendIDSet(id);

    listUnlock();

	return 0;
}

static UINT8  sendListIDGetNext(UINT8 list_id)
{

    if(plistData->inUse==0) return LIST_HEAD_BIT;

    return plistData->data[list_id].next;

}


static void uartSendData()
{
    UINT8 currentID=0;
    
    
    currentID=sendListIDGetNext(currentID);
    if(LIST_HEAD_BIT==currentID) return;
/*
    printf("uartSendData\n\r");
    int i;
    for(i=0;i<plistData->data[currentID].datalen;i++)
        printf("%2.2x ",plistData->data[currentID].data[i]);
    printf("\n\r");
*/
    MicoUartSend(UART_FOR_APP,plistData->data[currentID].data,plistData->data[currentID].datalen);
    sendListDel(currentID);

}

void uartWifiTestinRep()
{

    
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET]=0xff;
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+1]=0xff;
    
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+2]=0x00;
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+3]=0x03;
    
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+4]=0x18;
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+5]=0x00;
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+6]=0x19;

        
        uart_ob.uSendLen =7;

        MicoUartSend(UART_FOR_APP, uart_ob.uSendBuffer,7);

 
}


static unsigned int calccrc(unsigned char crcbuf,unsigned int crc)
{
	unsigned char i;
	unsigned char chk;
	crc=crc ^ crcbuf;
	for(i=0;i<8;i++)
	{
		chk=crc&1;
		crc=crc>>1;
		crc=crc&0x7fff;
		if (chk==1)
		crc=crc^0xa001;
		crc=crc&0xffff;
	}
	return crc;
}

static unsigned int qioucrc16(unsigned int crc,unsigned char *buf,unsigned int x)
{
		unsigned char hi,lo;
		unsigned int i;
		for (i=0;i<x;i++)
		{
			crc=calccrc(*buf,crc);
			buf++;
		}
		hi=crc%256;
		lo=crc/256;
		crc=(hi<<8)|lo;
		return crc;
}


static void devUartUpdate(UINT8 *data,UINT16 len)
{
    UINT8 loc=0;
    UINT8 update=0;
  //  dev_log("devUartUpdate");

    if(dev_status.currentPosition!=data[loc])
    {
        dev_status.currentPosition=data[loc];
        update=1;
    }
    loc++;

    if(dev_status.direction!=data[loc])
    {
        dev_status.direction=data[loc];
        update=1;
    }
    loc++;

    if(dev_status.manual!=data[loc])
        dev_status.manual=data[loc];
    loc++;

    if(data[loc]==0 || data[loc]==0x04)
    {//??/???
        if(uart_ob.motorRun!=0)
            uart_ob.motorRun--;
        else
        {
            if(dev_status.status!=0)
            {
                if(dev_status.currentPosition<=100)
                    dev_status.targetPosition=dev_status.currentPosition;
                dev_status.status=0;
                dev_status.app=0;
                update=1;


            }

        }
    }
    else
    {
        if(dev_status.status!=data[loc])
        {
            dev_status.status=data[loc];
            uart_ob.motorRun=4;
            if(dev_status.app==0)
            {
                if(dev_status.status==0x01)
                    dev_status.targetPosition=100;
                else if(dev_status.status==0x02)
                    dev_status.targetPosition=0;
            }
            update=1;

        }
    }
    loc++;

    if(dev_status.angle!=data[loc])
        dev_status.angle=data[loc];
    loc++;
    
    if(dev_status.angleDirection!=data[loc])
        dev_status.angleDirection=data[loc];
    loc++;
    if(dev_status.anglePara!=data[loc])
        dev_status.anglePara=data[loc];
    loc++;
    
    if(dev_status.trip!=data[loc])
        dev_status.trip=data[loc];

/*
    printf("#####################\r\n");
    dev_log("targetPosition:%d\n",dev_status.targetPosition);
    dev_log("currentPosition:%d\n",dev_status.currentPosition);
    dev_log("direction:%d\n",dev_status.direction);
    dev_log("manual:%d\n",dev_status.manual);
    dev_log("status:%d\n",dev_status.status);
    dev_log("angle:%d\n",dev_status.angle);
    dev_log("anglePara:%d\n",dev_status.anglePara);
    dev_log("res:%d\n",dev_status.res);
    dev_log("trip:%d\n",dev_status.trip);
    printf("#####################\r\n");
*/
    if(update)
        curatin_device_report();
}

static void devUartTypeUpdate(UINT8 type)
{

  //  dev_log("devUartTypeUpdate");
  //  dev_log("type:%d\n",type);

    dev_status.type=type;
}


static void uartDataParse(UINT8 *data,UINT16 len)
{
    UINT8 *pData=&data[UART_PROT_ADDR_OFFSET+2];
  // dev_log("uartDataParse\r\n");

    if((data[UART_PROT_CMD_OFFSET]==UPDATE_CMD)&&(data[UART_PROT_ADDR_OFFSET]==CURRENT_ADDR))//?????????
    {
            devUartUpdate(pData,len-8);
    }
    else if(data[UART_PROT_CMD_OFFSET]==READ_CMD)//?????
    {

        if(data[UART_PROT_ADDR_OFFSET]==CURRENT_ADDR)
        {
            devUartUpdate(pData,len-8);
        }
        else if(data[UART_PROT_ADDR_OFFSET]==TYPE_ADDR)
        {
            devUartTypeUpdate(pData[0]);
        }

    }


}






static int uartProtProcess(UINT8* data, int dataLen)
{
    UINT8  loc=0;
    UINT16 len=0;
    UINT16  CRC_buf=0;
/*
    UINT8  i;

    printf("uartRecvData\r\n");
    for(i=0;i<dataLen;i++)
        printf("%2.2x ",data[i]);
    printf("\r\n");
*/
    while(dataLen>=UART_PROT_MIN_LEN)
    {
        loc=0;
        if((data[loc++]==0x55)&&(data[loc++]==0xfe)&&(data[loc++]==0xfe))
        {
            if(data[UART_PROT_CMD_OFFSET]==READ_CMD)//??????16???豸???????9
            {
                if(data[UART_PROT_ADDR_OFFSET]==CURRENT_ADDR)
                    len=16;
                else if(data[UART_PROT_ADDR_OFFSET]==TYPE_ADDR)
                {
                    len=9;
                }
                else
                {
                    data +=1;
                    dataLen -=1;
                    continue;  
                }
            }
            else if((data[UART_PROT_CMD_OFFSET]==UPDATE_CMD)&&(data[UART_PROT_ADDR_OFFSET]==CURRENT_ADDR))//?????
            {

                  len=16;
            }
            else
            {
        
                data +=1;
                dataLen -=1;
                continue;
            }

            
            if(len>dataLen)
                break;

            
            CRC_buf=qioucrc16(0xffff,&data[0],(len-2));
            if((data[len-2]!=(UINT8)(CRC_buf>>8))||(data[len-1]!=(UINT8)(CRC_buf)))
            {
                dev_log("uart crc error");
                loc =1;
            }
            else
            {
                uartDataParse(data,len);
                loc=len;
            }
            
        }
        else if((data[0]==0xff)&&(data[1]==0xff)&&(data[2]==0x00)&&(data[3]==0x03)&&(data[4]==0x17))
        {
            uartWifiTestinRep();
            product_fac_test();   
            loc=7;
        }
        else
        {
          loc=1;
        }



        data +=loc;
        dataLen -=loc;

    }

   // printf("dataLen:%2.2x\r\n",dataLen);
    return dataLen;
}

#if 0
static int uartProtProcess(UINT8* data, int dataLen)
{
    UINT8  loc=0;
    UINT16 len=0;
    UINT16  CRC_buf=0;

    UINT8  i;

    printf("uartRecvData\r\n");
    for(i=0;i<dataLen;i++)
        printf("%2.2x ",data[i]);
    printf("\r\n");

    while(dataLen>=UART_PROT_MIN_LEN)
    {

        
        if((data[loc++]==0x55)&&(data[loc++]==0xfe)&&(data[loc++]==0xfe))
        {
            if(data[UART_PROT_CMD_OFFSET]==READ_CMD)//??????16???豸???????9
            {
                if(data[UART_PROT_ADDR_OFFSET]==CURRENT_ADDR)
                    len=16;
                else if(data[UART_PROT_ADDR_OFFSET]==TYPE_ADDR)
                {
                    len=9;
                }
                else
                {
                    data +=1;
                    dataLen -=1;
                    continue;  
                }
            }
            else if((data[UART_PROT_CMD_OFFSET]==UPDATE_CMD)&&(data[UART_PROT_ADDR_OFFSET]==CURRENT_ADDR))//?????
            {

                  len=16;
            }
            
            else
            {
        
                data +=1;
                dataLen -=1;
                continue;
            }

            
            if(len>dataLen)
                break;

            
            CRC_buf=qioucrc16(0xffff,&data[0],(len-2));
            if((data[len-2]!=(UINT8)(CRC_buf>>8))||(data[len-1]!=(UINT8)(CRC_buf)))
            {
                dev_log("uart crc error");
                loc =1;
            }
            else
            {
                uartDataParse(data,len);
                loc=len;
            }
            
        }
        else if((data[0]==0xff)&&(data[1]==0xff)&&(data[2]==0x00)&&(data[3]==0x03)&&(data[6]==0x01))
        {
            uartWifiTestinRep();
            product_fac_test();   
            loc=7;
        }
        
        else
        {
          loc=1;
        }



        data +=loc;
        dataLen -=loc;

    }

   // printf("dataLen:%2.2x\r\n",dataLen);
    return dataLen;
}

#endif

static void uartRecvData(UINT8* inBuf, int inBufLen)
{

    int datalen;


    if( MicoUartRecv( UART_FOR_APP, inBuf, inBufLen, UART_RECV_TIMEOUT) == kNoErr){
        datalen=inBufLen;

    }
    else{
        datalen = MicoUartGetLengthInBuffer(UART_FOR_APP);
        if(datalen){
            MicoUartRecv(UART_FOR_APP, inBuf, datalen, UART_RECV_TIMEOUT);

        }
        else 
            return;
    }

    
    uart_ob.uRecvLen +=datalen;

    //CODE   
    //DEAL PROT
    datalen=uartProtProcess(uart_ob.uRecvBuffer,uart_ob.uRecvLen );
    if(datalen>0)
    {
        if(datalen!=uart_ob.uRecvLen)
        {
            memmove(uart_ob.uRecvBuffer,&uart_ob.uRecvBuffer[uart_ob.uRecvLen-datalen],datalen);
            uart_ob.uRecvLen=datalen;
        }
    }
    else
        uart_ob.uRecvLen=0;
  
}

void uartRecv_thread(void *inContext)
{
    mico_thread_sleep(5);

    devUartTypeRead();
    devUartStatusRead(CURRENT_ADDR,8);

    while(1) {
    
        uartRecvData(uart_ob.uRecvBuffer+uart_ob.uRecvLen, UART_RECV_BUFFER_LEN-uart_ob.uRecvLen);
//UART_RECV_TIMEOUT 300ms


    }  

    
}



static void uart_timer_handler( void* arg )
{
    
        
    uartSendData();

    if(uart_ob.motorRun>0)
        devUartStatusRead(CURRENT_ADDR,8);


}




void devUartInit()
{

    uart_log("%s: start", __func__);
//    OSStatus err = kNoErr;
//    mico_uart_config_t uart_config;
//    mico_Context_t* mico_context=mico_system_context_get();;
    sendListInit();

    memset(&dev_status,0,sizeof(DEVICE_STATUS));

    memset(&uart_ob,0,sizeof(IS_UART_OB));
    /*UART receive thread*/
/*
    uart_config.baud_rate    = 9600;
    uart_config.data_width   = DATA_WIDTH_8BIT;
    uart_config.parity       = NO_PARITY;
    uart_config.stop_bits    = STOP_BITS_1;
    uart_config.flow_control = FLOW_CONTROL_DISABLED;

    ring_buffer_init  ( (ring_buffer_t *)&rx_buffer, (uint8_t *)rx_data, UART_RING_BUFFER_LENGTH );
    MicoUartInitialize( UART_FOR_APP, &uart_config, (ring_buffer_t *)&rx_buffer );
    err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "UART Recv", uartRecv_thread, STACK_SIZE_UART_RECV_THREAD, 0 );
    require_noerr(err, exit);

    
    mico_init_timer(&uart_timer,UART_TIMER_MS, uart_timer_handler, NULL);
    mico_start_timer(&uart_timer);
    

exit:
*/
  return;
}


void devUartCtrl(UINT8 addr , UINT8 *data , UINT8 datalen)
{
	UINT16 CRC_buf;
    UINT8 retry_cnt =5;
    
    uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET]=0x55;
    uart_ob.uSendBuffer[UART_PROT_ID_OFFSET]=0xfe;
    uart_ob.uSendBuffer[UART_PROT_ID_OFFSET+1]=0xfe;
    uart_ob.uSendBuffer[UART_PROT_CMD_OFFSET]=CTRL_CMD;
    uart_ob.uSendBuffer[UART_PROT_ADDR_OFFSET]=addr;
    uart_ob.uSendLen =5;
    if(datalen>0)
    {
        memcpy(&uart_ob.uSendBuffer[UART_PROT_DATA_OFFSET],data,datalen);
        uart_ob.uSendLen +=2;
    }
    
    CRC_buf=qioucrc16(0xffff,&uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET],uart_ob.uSendLen);
    uart_ob.uSendBuffer[uart_ob.uSendLen++]=(UINT8)(CRC_buf>>8);
    uart_ob.uSendBuffer[uart_ob.uSendLen++]=(UINT8)(CRC_buf);

    while(retry_cnt--)
    {
        if(sendListAdd(uart_ob.uSendBuffer,uart_ob.uSendLen)==0)
            break;
        mico_thread_msleep(100);

    }

}


void devUartStatusRead(UINT8 addr , UINT8 offset)
{
	UINT16 CRC_buf;
    UINT8 retry_cnt =5;
    
    uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET]=0x55;
    uart_ob.uSendBuffer[UART_PROT_ID_OFFSET]=0xfe;
    uart_ob.uSendBuffer[UART_PROT_ID_OFFSET+1]=0xfe;
    uart_ob.uSendBuffer[UART_PROT_CMD_OFFSET]=READ_CMD;
    uart_ob.uSendBuffer[UART_PROT_ADDR_OFFSET]=addr;
    uart_ob.uSendBuffer[UART_PROT_DATA_OFFSET]=offset;
    uart_ob.uSendLen =6;

    
    CRC_buf=qioucrc16(0xffff,&uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET],uart_ob.uSendLen);
    uart_ob.uSendBuffer[uart_ob.uSendLen++]=(UINT8)(CRC_buf>>8);
    uart_ob.uSendBuffer[uart_ob.uSendLen++]=(UINT8)(CRC_buf);

    while(retry_cnt--)
    {
        if(sendListAdd(uart_ob.uSendBuffer,uart_ob.uSendLen)==0)
            break;
        mico_thread_msleep(100);

    }

}



void devUartStatusWrite(UINT8 addr , UINT8 offset,UINT8 *data)
{
	UINT16 CRC_buf;
    UINT8 retry_cnt =5;
    
    uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET]=0x55;
    uart_ob.uSendBuffer[UART_PROT_ID_OFFSET]=0xfe;
    uart_ob.uSendBuffer[UART_PROT_ID_OFFSET+1]=0xfe;
    uart_ob.uSendBuffer[UART_PROT_CMD_OFFSET]=WRITE_CMD;
    uart_ob.uSendBuffer[UART_PROT_ADDR_OFFSET]=addr;
    uart_ob.uSendBuffer[UART_PROT_DATA_OFFSET]=offset;
    uart_ob.uSendLen =6;

    
    CRC_buf=qioucrc16(0xffff,&uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET],uart_ob.uSendLen);
    uart_ob.uSendBuffer[uart_ob.uSendLen++]=(UINT8)(CRC_buf>>8);
    uart_ob.uSendBuffer[uart_ob.uSendLen++]=(UINT8)(CRC_buf);

    while(retry_cnt--)
    {
        if(sendListAdd(uart_ob.uSendBuffer,uart_ob.uSendLen)==0)
            break;
        mico_thread_msleep(100);

    }

}



void devUartTypeRead(void)
{
    devUartStatusRead(TYPE_ADDR,1);

}

void devUartTypeWrite(UINT8 type)
{
    devUartStatusWrite(TYPE_ADDR,1,&type);
    devUartStatusRead(CURRENT_ADDR,8);

}




//0xff 0xff 0x01 0x02 0x01
void devUartWifiTestSuc()
{
  //      UINT8 retry_cnt =5;

    
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET]=0xff;
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+1]=0xff;
    
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+2]=0x00;
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+3]=0x04;
    
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+4]=0x13;
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+5]=0x00;
        uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+6]=0x02;
		uart_ob.uSendBuffer[UART_PROT_BEGIN_OFFSET+7]=0x17;
        uart_ob.uSendLen =8;

		MicoUartSend(UART_FOR_APP, uart_ob.uSendBuffer,uart_ob.uSendLen);
        /*while(retry_cnt--)
        {
            if(sendListAdd(uart_ob.uSendBuffer,uart_ob.uSendLen)==0)
                break;
            mico_thread_msleep(100);
    
        }*/

 
}



