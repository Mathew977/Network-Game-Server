#pragma once

//PACKING

//Pack bytes
#define PackBytes(bf, bytes, len) \
	memcpy(bf, bytes, len); \
	bf += len;

//Pack long
#define PackLong(bf, l) \
    *bf++ = (l >> 24) & 0xFF; \
    *bf++ = (l >> 16) & 0xFF; \
    *bf++ = (l >> 8) & 0xFF; \
    *bf++ = l & 0xFF;    

//Pack short
#define PackShort(bf, s) \
    *bf++ = (s >> 8) & 0xFF; \
    *bf++ = s & 0xFF;    

//Pack byte
#define PackByte(bf, b) \
    *bf++ = b;    

// UNPACKING

//Unpack bytes
#define UnpackBytes(bf, bytes, len) \
    memcpy(bytes, bf, len); \
    bf += len;

//Unpack long
#define UnpackLong(bf, l) \
    l = (*bf++ << 24); \
    l += (*bf++ << 16); \
    l += (*bf++ << 8); \
    l += *bf++;

//Unpack short
#define UnpackShort(bf, s) \
    s = (*bf++ << 8); \
    s += *bf++;

//Unpack byte
#define UnpackByte(bf, b) \
    b = *bf++;