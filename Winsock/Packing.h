#pragma once
#include "pch.h"
/*
MSG_STRUCT
An application may have many types of messages that need to be
sent/received. A good idea is to start each message
with a code to indicate the message structure being sent and
a length field containing the number of bytes in the message
structure.
*/
/*
Note: The packing of structures is dependant on the compiler,
fields may be aligned on word or long word boundaries.
The 'packing bytes' have to be removed by the packing process.
*/
typedef struct
{
	unsigned short code;
	unsigned short length;
	unsigned char username[20];
	int intValue;                 // 32 bit (signed)
	int intValue2;
	unsigned short datalen;
	char data[100];
	char data2[100];

} MSG_STRUCT;

using namespace std;
// Macros for packing and unpacking the structure
// to a buffer.

// PACKING

#define PackBytes(bf, bytes, len) \
    memcpy(bf, bytes, len); \
    bf += len;

// PackLong is equivalent to htonl but writes the
// u_long into a buffer.
// Note: You could do the equivalent
//#define PackLong(bf, l) \
//    *((unsigned long *)bf) = htonl(l); \
//    bf += sizeof(unsigned long);
// However, this would not be portable, some processors
// would crash if bf was not word/long word aligned
// when trying to use it as a unsigned long.
// Note: the terms long and short in htonl, htons, etc,
// are historical artifacts from the Digital VAX BSD distribution.
// In this context, long = 32 bits and short = 16 bits
// 
#define PackLong(bf, l) \
    *bf++ = (l >> 24) & 0xFF; \
    *bf++ = (l >> 16) & 0xFF; \
    *bf++ = (l >> 8) & 0xFF; \
    *bf++ = l & 0xFF;    

#define PackShort(bf, s) \
    *bf++ = (s >> 8) & 0xFF; \
    *bf++ = s & 0xFF;    

#define PackByte(bf, b) \
    *bf++ = b;    

// UNPACKING

#define UnpackBytes(bf, bytes, len) \
    memcpy(bytes, bf, len); \
    bf += len;

// UnpackLong is equivalent to ntohl but reads the
// buffer into a u_long.
// Note: You could do the equivalent
//#define UnpackLong(bf, l) \
//    l = ntohl(*((unsigned long *)bf)); \
//    bf += sizeof(unsigned long);
// However, this would not be portable, some processors
// would crash if bf was not word/long word aligned
// when trying to use it as a unsigned long.
// 
#define UnpackLong(bf, l) \
    l = (*bf++ << 24); \
    l += (*bf++ << 16); \
    l += (*bf++ << 8); \
    l += *bf++;

#define UnpackShort(bf, s) \
    s = (*bf++ << 8); \
    s += *bf++;

#define UnpackByte(bf, b) \
    b = *bf++;

// Could use a code to distinguish between different
// types of data being sent e.g. bullet = 1, player = 2,
// new level = 3, etc.
#define MSGCODE 0x0001

// Function to pack the message structure into a
// provided buffer
unsigned short PackMsg(MSG_STRUCT *msg, unsigned char *buff)
{
	unsigned short msglen;
	unsigned char *lenptr;
	//unsigned char *startptr = buff;

	PackShort(buff, msg->code);
	lenptr = buff;      // Add total length later
	buff += sizeof(unsigned short);
	PackBytes(buff, msg->username, 20);
	PackLong(buff, msg->intValue);
	PackLong(buff, msg->intValue2);
	PackShort(buff, msg->datalen);
	PackBytes(buff, msg->data, msg->datalen);
	PackBytes(buff, msg->data2, msg->datalen);

	// Calculate the correct message length.
	// =====================================
	// You may hard code the length of the header
	// and add the extra data bytes (msg->datalen)
	// at the end.
	// 37 + msg->datalen
	// This is quite often the technique used,
	// but since we are incrementing the buffer
	// pointer each time we add a byte we can
	// simply calculate the length from the pointers. 
	// Because they are unsigned char* pointers, subtracting
	// them gives us the difference in the addresses in bytes.      
	//msglen = (unsigned short)(buff-startptr); 

	// Here we add all the size of all the fields together
	// to make it clear what we want.   
	msglen = sizeof(unsigned short)
		+ sizeof(unsigned short)
		+ sizeof(msg->username)
		+ sizeof(unsigned long)
		+ sizeof(unsigned short)
		+ 1
		+ sizeof(unsigned long) // int
		+ sizeof(unsigned short)
		+ msg->datalen;

	PackShort(lenptr, msglen);

	return msglen;
}

// Unpack the provided buffer into the provided
// message structure
void UnpackMsg(MSG_STRUCT *msg, unsigned char *buff)
{
	UnpackShort(buff, msg->code);
	UnpackShort(buff, msg->length);
	UnpackBytes(buff, msg->username, 20);
	UnpackLong(buff, msg->intValue);
	UnpackLong(buff, msg->intValue2);
	UnpackShort(buff, msg->datalen);
	UnpackBytes(buff, msg->data, msg->datalen);
	UnpackBytes(buff, msg->data2, msg->datalen);
}