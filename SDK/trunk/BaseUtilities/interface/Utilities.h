/*!	\file Utilities.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

#ifdef WIN32
#define PACKED
#endif	// WIN32
#ifdef _Mac
#define PACKED __attribute__ ((packed))
#endif	// _Mac


#ifdef _Mac
#ifdef __LITTLE_ENDIAN__
#define _Mac_Intel
#else	// __LITTLE_ENDIAN__
#define _Mac_PowerPC
#endif	// __LITTLE_ENDIAN__
#endif	// _Mac


#ifdef _DEBUG
	#ifdef WIN32
		#define debug_break __asm int 3
	#else
//		#define debug_break ThrowIfNil_(0);
		#define debug_break
	#endif
	#undef ASSERT
	#define ASSERT(n) if(!(n)) debug_break
#else
	#define debug_break
	#define ASSERT(n)
#endif

//! Rounds floating point value to closest integer
inline float Round(float f) {
	return f < 0 ? f - 0.5f : f + 0.5f;
}
//! \overload
inline double Round(double d) {
	return d < 0 ? d - 0.5 : d + 0.5;
}

//! Rounds floating point value to closest integer and returns integer
inline tint Float2Int(float f)
{
	return (tint)Round(f);
}
//! \overload
inline tint Float2Int(double d)
{
	return (tint)Round(d);
}

//! Swaps 2 items (with intermediary storage)
template<class _T> inline void Swap(_T& a, _T& b) {
	_T Tmp = a;
	a = b;
	b = Tmp;
}


#ifdef _Mac

#define max(a, b) ((a) > (b) ? (a) : (b))

#define min(a, b) ((a) < (b) ? (a) : (b))

#endif

#ifdef _WIN32
//! On systems that are not little-endian, swaps the bytes as neccasary, and returns new value. On little-endian systems returns the same value.
inline tuint32 CPSwap(tuint32 ul)
{
	return ul;
}
//! \overload
inline tint32 CPSwap(tint32 l)
{
	return l;
}
//! \overload
inline tuint16 CPSwap(tuint16 us)
{
	return us;
}
//! \overload
inline tint16 CPSwap(tint16 s)
{
	return s;
}
//! \overload
inline tfloat CPSwap(tfloat32 f)
{
	return f;
}
#endif	// _WIN32

#ifdef _Mac_Intel
inline tuint32 CPSwap(tuint32 ul)
{
	return ul;
}
inline tint32 CPSwap(tint32 l)
{
	return l;
}
inline tuint16 CPSwap(tuint16 us)
{
	return us;
}
inline tint16 CPSwap(tint16 s)
{
	return s;
}
inline tfloat CPSwap(tfloat32 f)
{
	return f;
}
#endif	// _Mac_Intel

#ifdef _Mac_PowerPC
inline tuint32 CPSwap(tuint32 ul)
{
	return (((ul>>24) & 0x000000FFL) | ((ul>>8) & 0x0000FF00L) | ((ul<<8) & 0x00FF0000L) | ((ul<<24) & 0xFF000000L));
}
inline tint32 CPSwap(tint32 l)
{
	return (((l>>24) & 0x000000FFL) | ((l>>8) & 0x0000FF00L) | ((l<<8) & 0x00FF0000L) | ((l<<24) & 0xFF000000L));
}
inline tuint16 CPSwap(tuint16 us)
{
	return (((us>>8) & 0x00FF) |  ((us<<8) & 0xFF00));
}
inline tint16 CPSwap(tint16 s)
{
	return (((s>>8) & 0x00FF) |  ((s<<8) & 0xFF00));
}
inline tfloat CPSwap(tfloat32 f)
{
	tuint32* pul = (tuint32*)&f;
	*pul = ((((*pul)>>24) & 0x000000FFL) | (((*pul)>>8) & 0x0000FF00L) | (((*pul)<<8) & 0x00FF0000L) | (((*pul)<<24) & 0xFF000000L));
	tfloat* pf = (tfloat*)pul;
	return *pf;
}
#endif	// _Mac_PowerPC

//! Opens a browser at the given address
void OpenBrowser(const tchar* pszAddress);

//! Starts an application with the given path name (full path + file name)
void StartApplication(const tchar* pszPathName);

//! Opens a document with default application
void OpenDocument(const tchar* pszPathName);

// Misc OSX utilities
#ifdef _Mac

//! Converts an internal path to a Unix path. For internal use only. Note that the output string is 7 characters longer than the input string.
void ToUnix(tchar* psz);

void PascalToCString(StringPtr pasStr, char* cStr);

void PathNameFromDirID(long dirID, short vRefNum, char* fullPathName);

void FSPToCPath(FSSpecPtr fspp, char* fPath);

void CToPascalString(char* cStr, StringPtr pasStr);

void GetPartOfPath(char *path, char *partOfPath, char **restOfPath);

OSErr GetDirID(FSSpec *folderSpec, long *dirID);

OSErr GetVolID(FSSpec *spec);

void FileNameToFSSpec(const char *name, FSSpec *aSpec);

void stlstr2str255(Str255& rs255, std::string s);

#endif	// _Mac

}	// namespace k2s

// Stuff which are on msvc, but not on xcode
#ifdef _Mac
int stricmp(const char* pszA, const char* pszB);
int strnicmp(const char* pszA, const char* pszB, size_t iCount);
#endif	// _Mac

namespace k2s {
	
	
	void PszEnumString2List(const tchar* psz, tchar cDelimiter, std::list<std::string>& rlistOfStrings);
	
	//! generates a UUID
	void Gen_UUID( tchar* pszUUID, tuint32 uiBuffer_Size);



/*
// New and delete has to be outside the namespace
}	// namespace k2s

void* operator new(size_t);
void operator delete(void*);

void* operator new[](size_t);
void operator delete[](void*);

//! Report memory leaks
void ReportMemoryLeaks();

// Enter namespace again
namespace k2s {
*/




