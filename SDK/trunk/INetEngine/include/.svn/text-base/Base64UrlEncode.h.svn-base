
//////////////////////////////////////////////////////////////////////
//
// Base64UrlEncode.h
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
// This version uses "modified Base64 for URL" where no padding '=' will be used,
// and the '+' and '/' characters of standard Base64 are respectively replaced by 
// '*' and '-', so that using URL encoders/decoders is no longer necessary and has
// no impact on the length of the encoded value
//
//////////////////////////////////////////////////////////////////////

#ifndef __BASE64URLCODE_H
#define __BASE64URLCODE_H

//namespace Base64UrlEncode
//{
class Base64UrlEncode {
public:
    static unsigned long EncodeGetLength( unsigned long size );
    static unsigned long Encode( void* dest, const void* src, unsigned long size );

    static unsigned long DecodeGetLength( unsigned long size );
    static unsigned long Decode( void* dest, const void* src, unsigned long size );
};
//}

#endif //__BASE64URLCODE_H
