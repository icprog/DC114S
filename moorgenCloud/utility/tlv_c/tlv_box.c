/*
 *  COPYRIGHT NOTICE  
 *  Copyright (C) 2015, Jhuster, All Rights Reserved
 *  Author: Jhuster(lujun.hust@gmail.com)
 *  
 *  https://github.com/Jhuster/TLV
 *   
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.  
 */
#include "mico.h"
#include <stdio.h>
#include <string.h>
#include "tlv_box.h"
#define BASIC_DATA
#define BASIC_TYPE
static OSStatus make_tlv_tag(unsigned char *buffer, int tag, int *poffset);
static OSStatus make_tlv_length(unsigned char *buffer, int length, int *poffset);
static OSStatus make_tlv_data(unsigned char *buffer, unsigned char* data, int datalen, int *poffset);
static int  get_tlv_tag(unsigned char *buffer, int *poffset, int size_remain);
static int  get_tlv_length(unsigned char *buffer, int *poffset, int size_remain);
int tlv_box_putobject(tlv_box_t *box,int type,void *value,int length);

#define tlv_log(format, ...)  //custom_log("tlv", format, ##__VA_ARGS__)

static void tlv_box_release_tlv(value_t value)
{
    tlv_t *tlv = (tlv_t *)value.value;
    free(tlv->value);
    free(tlv);
}

tlv_box_t *tlv_box_create()
{
    tlv_box_t* box = (tlv_box_t*)malloc(sizeof(tlv_box_t));
    box->m_list = key_list_create(tlv_box_release_tlv);
    box->m_serialized_buffer = NULL;
    box->m_serialized_bytes = 0;
    return box;
}

tlv_box_t *tlv_box_parse(unsigned char *buffer,int buffersize)
{

    tlv_box_t *box = tlv_box_create();

    unsigned char *cached = (unsigned char*)malloc(buffersize);
    memcpy(cached,buffer,buffersize);

/*
	for (int i=0; i<buffersize; i++)
	{
		tlv_log("%s, get cached %x", __func__, (unsigned char)cached[i]);
	}
*/

    int offset = 0, length = 0;
    while (offset < buffersize) {
	/* renjian
        int type = (*(int *)(cached+offset));
        offset += sizeof(int);
        int length = (*(int *)(cached+offset));
        offset += sizeof(int);
        tlv_box_putobject(box,type,cached+offset,length);
        offset += length;
        */
        int type = get_tlv_tag(cached+offset, &offset, buffersize-offset);
	require(offset<buffersize, exit);
	//tlv_log("get type is %x", type);
	length = get_tlv_length(cached+offset, &offset, buffersize-offset);
	//tlv_log("get length is %d", length);
	require(offset<buffersize, exit);
	tlv_box_putobject(box, type, cached+offset, length);
	//tlv_log("get data is %x", cached[offset]);
	offset += length;
	
    }

    box->m_serialized_buffer = cached;
    box->m_serialized_bytes  = buffersize;

exit:
    return box;
}

int tlv_box_destroy(tlv_box_t *box)
{
    key_list_destroy(box->m_list);
    if (box->m_serialized_buffer != NULL) {
        free(box->m_serialized_buffer);        
    }

    free(box);

    return 0;
}

unsigned char *tlv_box_get_buffer(tlv_box_t *box)
{
    return box->m_serialized_buffer;
}

int tlv_box_get_size(tlv_box_t *box)
{
    return box->m_serialized_bytes;
}

int tlv_box_putobject(tlv_box_t *box,int type,void *value,int length)
{
    if (box->m_serialized_buffer != NULL) {
        return -1;
    }

    tlv_t *tlv = (tlv_t *)malloc(sizeof(tlv_t));
    tlv->type = type;
    tlv->length = length;
    tlv->value = (unsigned char *)malloc(length);
    memcpy(tlv->value,value,length);

    value_t object;
    object.value = tlv;

    if (key_list_add(box->m_list,type,object) != 0) {
        return -1;
    }    

	if ((type & 0xf00) == 0xf00)
	{
		box->m_serialized_bytes += 2;
	}
	else if ((type & 0xff00) == 0 && (type & 0xf) != 0xf)
	{
		box->m_serialized_bytes += 1;
	}
	else
	{
		tlv_log("Tag range err!");
		return -1;
	}

	if (length < 128)
	{
		box->m_serialized_bytes += 1;
	}
	else if (length <= 0xff)
	{
		box->m_serialized_bytes += 2;
	}
	else if (length <= 0xffff)
	{
		box->m_serialized_bytes += 3;
	}
	else
	{
		tlv_log("The length is too long!");
		return -1;
	}

	box->m_serialized_bytes += length;
    
    return 0;
}

int tlv_box_put_char(tlv_box_t *box,int type,char value)
{
    return tlv_box_putobject(box,type,&value,sizeof(char));
}

int tlv_box_put_short(tlv_box_t *box,int type,short value)
{
    return tlv_box_putobject(box,type,&value,sizeof(short));
}

int tlv_box_put_int(tlv_box_t *box,int type,int value)
{
#if (BIGEDIAN == 1)
	value = htonl(value);
#endif
    return tlv_box_putobject(box,type,&value,sizeof(int));
}

int tlv_box_put_long(tlv_box_t *box,int type,long value)
{
#if (BIGEDIAN == 1)
	value = htonl(value);
#endif	
    return tlv_box_putobject(box,type,&value,sizeof(long));
}

int tlv_box_put_longlong(tlv_box_t *box,int type,long long value)
{
#if (BIGEDIAN == 1)
	value = htonl(value);
#endif
    return tlv_box_putobject(box,type,&value,sizeof(long long));
}

int tlv_box_put_float(tlv_box_t *box,int type,float value)
{
    return tlv_box_putobject(box,type,&value,sizeof(float));
}

int tlv_box_put_double(tlv_box_t *box,int type,double value)
{
    return tlv_box_putobject(box,type,&value,sizeof(double));
}

int tlv_box_put_string(tlv_box_t *box,int type,char *value)
{
    return tlv_box_putobject(box,type,value,strlen(value)+1);
}

int tlv_box_put_bytes(tlv_box_t *box,int type,unsigned char *value,int length)
{
    return tlv_box_putobject(box,type,value,length);
}

int tlv_box_put_object(tlv_box_t *box,int type,tlv_box_t *object)
{
    return tlv_box_putobject(box,type,tlv_box_get_buffer(object),tlv_box_get_size(object));
}

int tlv_box_serialize(tlv_box_t *box)
{
	OSStatus err = kGeneralErr;
    if (box->m_serialized_buffer != NULL) {
        goto exit;
    }

    int offset = 0;
    unsigned char* buffer = (unsigned char*)malloc(box->m_serialized_bytes);    
	memset(buffer, 0, box->m_serialized_bytes);
    key_list_foreach(box->m_list,node) {
        tlv_t *tlv = (tlv_t *)node->value.value; 

	// TLV serialize
	err = make_tlv_tag(buffer+offset, tlv->type, &offset);
	require_noerr(err, exit);
	err = make_tlv_length(buffer+offset, tlv->length, &offset);
	require_noerr(err, exit);
	err = make_tlv_data(buffer+offset, tlv->value, tlv->length, &offset);
	require_noerr(err, exit);
    }

    box->m_serialized_buffer = buffer;

    return 0;
exit:
	return -1;
}

static OSStatus make_tlv_tag(unsigned char *buffer, int tag, int *poffset)
{
	int result = tag;
	if ((result & 0xff00) != 0)
	{
		if ((result & 0x0f00) == 0xf00 )
		{
			// tag length is two
			buffer[0] = (unsigned char)((result >> 8) & 0xff);
			buffer[1] = (unsigned char)(result & 0xff);
			*poffset += 2;
		}
		else
		{
			tlv_log("tag type err!");
			return kGeneralErr;
		}
	}
	else
	{
		if ((result & 0xf) != 0xf)
		{
			// tag length is one
			buffer[0] = (unsigned char)(result & 0xff);
			*poffset += 1;
		}
		else
		{
			tlv_log("tag type err");
			return  kGeneralErr;
		}
	}
	
	return kNoErr;
}

static int  get_tlv_tag(unsigned char *buffer, int *poffset, int size_remain)
{
	int type = -1;

	if (size_remain <= 0)
	{
		return -1;
	}

	//tlv_log("buffer[0] is %x, buffer[1] is %x", buffer[0], buffer[1]);
	if ((buffer[0] & 0xff) != 0)
	{
		if ((buffer[0] & 0x0f) == 0xf)
		{
			if (size_remain <= 1)
			{
				return -1;
			}
			// tag length is two
			type = (int)((buffer[0] << 8) | (buffer[1] &0xff));
			*poffset += 2;
		}
		else
		{
			if (size_remain <= 0)
			{
				return -1;
			}
			// tag lenght is one
			type = (int)(buffer[0] & 0xff);
			*poffset += 1;
		}
	}
	else
	{
		return -1;
	}
	
	return type;
}

static OSStatus make_tlv_length(unsigned char *buffer, int length, int *poffset)
{
	if (length < 128)
	{
		buffer[0] = (unsigned char)(length & 0xff);	
		*poffset += 1;
		//tlv_log("length is %x", buffer[0]);
	}
	else
	{
		if (length <= 0xff)
		{
			buffer[0] = 0x81;
			buffer[1] = (unsigned char)(length & 0xff);
			*poffset += 2;
		}
		else if (length <= 0xffff)
		{
			buffer[0] = 0x82;
			buffer[1] = (unsigned char)((length >> 8) & 0xff);
			buffer[2] = (unsigned char)(length &  0xff);
			*poffset += 3;
		}
		else if (length <= 0xffffff)
		{
			buffer[0] = 0x83;
			buffer[1] = (unsigned char)((length >> 16) & 0xff);
			buffer[2] = (unsigned char)((length >> 8) & 0xff);
			buffer[3] = (unsigned char)(length &  0xff);
			*poffset += 4;
		}
		else
		{
			tlv_log("length is too long!");
			return kGeneralErr;
		}
	}
	return kNoErr;
}

static int  get_tlv_length(unsigned char *buffer, int *poffset, int size_remain)
{
	int length = -1;

	if (size_remain <=0)
	{
		return -1;
	}
	
	if ((buffer[0] >> 7) != 0)
	{
		// get data length
		if (buffer[0] == 0x81)
		{
			if (size_remain < 2)
			{
				return -1;
			}
			
			*poffset += 2;
			length = (int)buffer[1];
		}
		else if (buffer[0] == 0x82)
		{
			if (size_remain < 3)
			{
				return -1;
			}
			*poffset += 3;
			length = (int)(buffer[1] << 8 | buffer[2]);
		}
	}
	else
	{
		length = (int)(buffer[0] & 0x7f);
		*poffset += 1;
	}
	return length;
}

static OSStatus make_tlv_data(unsigned char *buffer, unsigned char* data, int datalen, int *poffset)
{
	//tlv_log("data is %s", data);
	memcpy(buffer, data, datalen);
	*poffset += datalen;
	//tlv_log("make tlv data is %s", buffer);
	return kNoErr;
}

int tlv_box_get_char(tlv_box_t *box,int type,char *value)
{
    value_t object;
    if (key_list_get(box->m_list,type,&object) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *)object.value;
    *value = (*(char *)(tlv->value));
    return 0;
}

int tlv_box_get_short(tlv_box_t *box,int type,short *value)
{
    value_t object;
    if (key_list_get(box->m_list,type,&object) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *)object.value;
    *value = (*(short *)(tlv->value));
    return 0;
}

int tlv_box_get_int(tlv_box_t *box,int type,int *value)
{
    value_t object;
    if (key_list_get(box->m_list,type,&object) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *)object.value;
    *value = (*(int *)(tlv->value));

#if (BIGEDIAN == 1)
	*value = ntohl(*value);
#endif
    return 0;
}

int tlv_box_get_long(tlv_box_t *box,int type,long *value)
{
    value_t object;
    if (key_list_get(box->m_list,type,&object) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *)object.value;
    *value = (*(long *)(tlv->value));

#if (BIGEDIAN == 1)
	*value = ntohl(*value);
#endif

    return 0;
}

int tlv_box_get_longlong(tlv_box_t *box,int type,long long *value)
{
    value_t object;
    if (key_list_get(box->m_list,type,&object) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *)object.value;
    *value = (*(long long *)(tlv->value));

#if (BIGEDIAN == 1)
	*value = ntohl(*value);
#endif
    return 0;
}

int tlv_box_get_float(tlv_box_t *box,int type,float *value)
{
    value_t object;
    if (key_list_get(box->m_list,type,&object) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *)object.value;
    *value = (*(float *)(tlv->value));
    return 0;
}

int tlv_box_get_double(tlv_box_t *box,int type,double *value)
{
    value_t object;
    if (key_list_get(box->m_list,type,&object) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *)object.value;
    *value = (*(double *)(tlv->value));
    return 0;
}

int tlv_box_get_string(tlv_box_t *box,int type,char *value,int* length)
{
    return tlv_box_get_bytes(box,type,(unsigned char *)value,length);
}

int tlv_box_get_bytes(tlv_box_t *box,int type,unsigned char *value,int* length)
{
    value_t object;
    if (key_list_get(box->m_list,type,&object) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *)object.value;
    if (*length < tlv->length) {
        return -1;
    }
    memset(value,0,*length);
    *length = tlv->length;
    memcpy(value,tlv->value,tlv->length);
    return 0;
}

int tlv_box_get_bytes_ptr(tlv_box_t *box,int type,unsigned char **value,int* length)
{
    value_t object;
    if (key_list_get(box->m_list,type,&object) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *)object.value;
    *value  = tlv->value;
    *length = tlv->length;
    return 0;
}

int tlv_box_get_object(tlv_box_t *box,int type,tlv_box_t **object)
{
    value_t value;
    if (key_list_get(box->m_list,type,&value) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *)value.value;
    *object = (tlv_box_t *)tlv_box_parse(tlv->value,tlv->length);
    return 0;
}