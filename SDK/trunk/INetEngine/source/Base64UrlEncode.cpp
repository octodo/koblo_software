
//////////////////////////////////////////////////////////////////////
//
// Base64UrlEncode.cpp
//
// Developed by Claus Riber/CRC Data ApS based on Cyotec solution
//
//
// Associated Files
// - CyoEncode.h
// - CyoEncode.cpp
// - CyoDecode.h
// - CyoDecode.cpp
//
//////////////////////////////////////////////////////////////////////

#include "ineInternalOS.h"


#define fillchar '='

//namespace Base64UrlEncode
//{
unsigned long Base64UrlEncode::EncodeGetLength( unsigned long size )
{
	return CyoEncode::Base64EncodeGetLength(size);
}

unsigned long Base64UrlEncode::Encode( void* dest, const void* src, unsigned long size ) {
	//remove fillchars
	unsigned long encodelength = CyoEncode::Base64Encode(dest, src, size);
	while (((char*) dest)[encodelength-1] == fillchar) encodelength--;
	//change special chars
	// Lasse - bug-fix 2007-06-28, accessing memory not owned
	//for (char *p = (char*) dest+encodelength; p >= dest; p--) {
	for (char *p = (char*) dest+encodelength-1; p >= dest; p--) {
	// .. Lasse
		if (*p == '+') *p = '*';
		if (*p == '/') *p = '-';
	}
	return encodelength;
}

unsigned long Base64UrlEncode::DecodeGetLength( unsigned long size )
{
	return CyoDecode::Base64DecodeGetLength(((size + 3) >> 2) << 2);
}

unsigned long Base64UrlEncode::Decode( void* dest, const void* src, unsigned long size )
{
	//append fillchars
	unsigned long buffersize = ((size + 3) >> 2) << 2; //set to multible of 4 at least size
	char *buffer = new char[buffersize]; 

	// Lasse - optimized 2007-06-28
	//memset(buffer, fillchar, buffersize); //pad with fillchar - not optimal
	//memcpy(buffer, src, size);
	memcpy(buffer, src, size);
	unsigned long lPad = buffersize - size;
	if (lPad > 0) {
		// Pad the last bytes with fillchar
		memset(buffer+size, fillchar, lPad);
	}
	// .. Lasse
	//change special chars
	// Lasse - bug-fix 2007-06-28, accessing memory not owned
	//for (char *p = buffer+size; p >= buffer; p--) {
	for (char *p = buffer+size-1; p >= buffer; p--) {
	// .. Lasse
		if (*p == '*') *p = '+';
		if (*p == '-') *p = '/';
	}
	// Lasse - bug-fix 2007-06-28, memory leak
	//return CyoDecode::Base64Decode(dest, buffer, buffersize);
	unsigned long ulReturn = CyoDecode::Base64Decode(dest, buffer, buffersize);
	delete[] buffer;
	return ulReturn;
	// .. Lasse
}

//}

