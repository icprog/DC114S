#include "stdint.h"
#include "stdio.h"
#include "mgOS.h"
#include "AESUtils.h"


void mgAesEncrypt(uint8_t *in,uint8_t *out,uint8_t *key,int length)
{
    Aes aes;    
    int i,k,j;

    AesSetKey( &aes, key, 16, NULL, AES_ENCRYPTION );


    k=length/16;
    j=length%16;
    if(j!=0)
        k +=1;
        
    for(i=0;i<k;i++)
    {
        AesEncryptDirect(&aes,&out[16*i],&in[16*i]);
    }

}




void  mgAesDecrypt(uint8_t *in,uint8_t*out,uint8_t *key, int length)
{
    Aes aes;    
    int i,k,j;

    AesSetKey( &aes, key, 16, NULL, AES_DECRYPTION );


    k=length/16;
    j=length%16;
    if(j!=0)
        k +=1;
    for(i=0;i<k;i++)
    {
        AesDecryptDirect(&aes,&out[16*i],&in[16*i]);
    }

}


/*
void mgAesTest()
{
    uint8_t buffer[16]={0};
    uint8_t buffer2[16]={0};
    int i;
    sprintf(buffer,"hello world");

    mgAesEncrypt(buffer,buffer2,testkey,16);

    
    for(i=0;i<16;i++)
        printf("%02x ",(uint8_t)buffer2[i]);
    printf("\n\r");


    memset(buffer,0,16);

    mgAesDecrypt(buffer2,buffer,testkey,16);

    for(i=0;i<16;i++)
        printf("%02x ",(uint8_t)buffer[i]);
    printf("\n\r");

    mgos_log("mgAesTest:%s",buffer);
}
*/
