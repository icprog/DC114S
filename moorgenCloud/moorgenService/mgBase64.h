#ifndef _mgBase64_H__
#define _mgBase64_H__

char * mgbase64_encode(char * bindata, char * base64, int binlength );

int mgbase64_decode(char * base64, char * bindata );



#endif