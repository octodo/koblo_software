/*!	\file BaseTypes.h
	\author Michael Olsen
	\date 09/Dec/2004
	\date 09/Dec/2004
*/

//! \typedef Signed integer, min. 16-bit
typedef int tint;

//! \typedef Unsigned integer, min. 16-bit
typedef unsigned int tuint;

/*! \typedef
 * \brief Signed 32-bit integer
 *
 * Use this when excactly 32-bit, or min. 32-bit, is required
*/
typedef int tint32;

/*! \typedef
 * \brief Unsigned 32-bit integer
 *
 * Use this when excactly 32-bit, or min. 32-bit, is required
*/
typedef unsigned int tuint32;

/*! \typedef
 * \brief Signed 16-bit integer
 *
 * Use this when excactly 16-bit, or min. 16-bit, is required
*/
typedef short tint16;

/*! \typedef
 * \brief Unsigned 16-bit integer
 *
 * Use this when excactly 16-bit, or min. 16-bit, is required
*/
typedef unsigned short tuint16;

/*! \typedef
 * \brief Signed 64-bit integer
 *
 * Use this when excactly 64-bit, or min. 64-bit, is required
 * <br>Note, it may not be present on all platforms (is present on Win32 and MacOSX)
*/
#ifdef WIN32
typedef __int64 tint64;
#endif
#ifdef _Mac
typedef long long tint64;
#endif

/*! \typedef
 * \brief Unsigned 64-bit integer
 *
 * Use this when excactly 64-bit, or min. 64-bit, is required
 * <br>Note, it may not be present on all platforms (is present on Win32 and MacOSX)
*/
#ifdef WIN32
typedef unsigned __int64 tuint64;
#endif
#ifdef _Mac
typedef unsigned long long tuint64;
#endif

/*! \typedef
 * \brief Signed 8-bit integer (char)
 *
 * Use this when excactly 8-bit is required
*/
typedef signed char tschar;

/*! \typedef
 * \brief Unsigned 8-bit integer (char)
 *
 * Use this when excactly 8-bit is required
*/
typedef unsigned char tuchar;

/*! \typedef
 * \brief 8-bit integer (char) signed or unsigned
 *
 * Use this when excactly 8-bit is required
*/
typedef char tchar;

/*! \typedef
 * \brief Boolean
 *
 * There's no guarantee regarding size, and thus shouldn't be used in external data structures
*/
typedef bool tbool;

//! \typedef float, min. 32-bit
typedef float tfloat;

/*! \typedef
 * \brief 32-bit float
 *
 * Use this when excactly 32-bit, or min. 32-bit, is required
*/
typedef float tfloat32;

/*! \typedef
 * \brief 64-bit float
 *
 * Use this when excactly 64-bit, or min. 64-bit, is required
*/
typedef double tfloat64;




