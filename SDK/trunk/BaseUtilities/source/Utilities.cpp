/*!	\file Utilities.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 18/Feb/2005
*/

#include "buInternalOS.h"


#ifdef _Mac

namespace k2s {

void ToUnix(tchar* psz)
{
	std::string sDest("/Volumes");
	std::string sSrc(psz);

	tint iStringLength = sSrc.size();
	tint iIndex;
	for (iIndex = 0; iIndex < iStringLength; iIndex++) {
		tchar psz[2];
		psz[1] = 0;

		if (sSrc[iIndex] == ':') {
			psz[0] = '/';
		}
		else {
			psz[0] = sSrc[iIndex];
		}

		sDest += std::string((tchar*)psz);
	}
	
	strcpy(psz, sDest.c_str());
}

void PascalToCString(StringPtr pasStr, char* cStr)
{
	long ix = 0; 

	while (ix < pasStr[0]) {
		*cStr = pasStr[ix+1];
		cStr++;
		ix++;
	}

	*cStr = 0;
}

void PathNameFromDirID(long dirID, short vRefNum, char* fullPathName)
{
	CInfoPBRec	block;
	Str255		directoryName;
	OSErr		err;
	char		tmps[255];

	fullPathName[0] = '\0';

	block.dirInfo.ioDrParID = dirID;
	block.dirInfo.ioNamePtr = directoryName;

	do {
		block.dirInfo.ioVRefNum = vRefNum;
		block.dirInfo.ioFDirIndex = -1;
		block.dirInfo.ioDrDirID = block.dirInfo.ioDrParID;
		err = PBGetCatInfo(&block, false);

		PascalToCString(directoryName,tmps);
		strcat(tmps,":");
		strcat(tmps,fullPathName);
		strcpy(fullPathName,tmps);
	} while ((err == noErr) && (block.dirInfo.ioDrDirID != 2));
}

void FSPToCPath(FSSpecPtr fspp, char* fPath)
{
	tchar aFName[256];

	PascalToCString((unsigned char*)(fspp->name), (char*)aFName);
	PathNameFromDirID(fspp->parID,fspp->vRefNum,fPath);
	strcat((char*)fPath, (const char*)aFName);
}

void CToPascalString(char* cStr, StringPtr pasStr)
{
	pasStr[0] = 0;
	while (cStr[pasStr[0]])
	{
		pasStr[pasStr[0]+1] = cStr[pasStr[0]];
		pasStr[0]++;
	}

}

void	GetPartOfPath	(char *path, char *partOfPath, char **restOfPath)
{
	long	i = 0, j= 0;
	long	length = strlen(path);

	if (path[0] == ':')
		i++;
	while (path[i] != ':' && i < length)
	{
		partOfPath[j] = path[i];
		i++;
		j++;
	}
	if (i == length)
		*restOfPath = 0L;
	else
		*restOfPath = &(path[i]);
	partOfPath[j] = 0;
}

OSErr		GetDirID		(FSSpec *folderSpec, long *dirID)
{
	OSErr		theErr	= noErr;
	CInfoPBRec	catInfo;
	memset((Ptr) & catInfo, 0, sizeof(CInfoPBRec));

	catInfo.dirInfo.ioNamePtr 	= folderSpec->name;
	catInfo.dirInfo.ioVRefNum 	= folderSpec->vRefNum;
	catInfo.dirInfo.ioDrDirID 	= folderSpec->parID;

	theErr = PBGetCatInfo(&catInfo,false);

	if (theErr == noErr)
		*dirID 	= catInfo.dirInfo.ioDrDirID;

	return (theErr);
}

OSErr	GetVolID							(FSSpec *spec)
{
	HVolumeParam	volumeParam;
	Str255			name;
	OSErr			anErr = noErr;
	long			index = 1;
	long			found = 0;

	while (!found && !anErr)
	{
		memset((void *)&volumeParam, 0, sizeof(HVolumeParam));
		volumeParam.ioNamePtr = name;
		name[0] = 0;
		volumeParam.ioVolIndex = index;
		anErr = PBHGetVInfoSync((HParmBlkPtr )&volumeParam);
		if (!anErr)
		{
			if (EqualString(name, spec->name, true, true))
			{
				found = 1;
				spec->vRefNum = volumeParam.ioVRefNum;
				spec->parID = fsRtParID;
			}
			else
				index++;
		}
	}
	return anErr;
}

void FileNameToFSSpec(const char *name, FSSpec *aSpec)
{
	char	pathPart[64], *restOfPath;
	long	dirID;
	FSSpec	spec;
	OSErr	anErr;

	char localName[2048];


	strcpy(localName, name);

	restOfPath = localName;

	GetPartOfPath(restOfPath, pathPart, &restOfPath);
	CToPascalString(pathPart, spec.name);
	spec.vRefNum = 0;
	spec.parID = 0;
	anErr = GetVolID(&spec);
	if (!anErr)
	{
		while (restOfPath)
		{
			GetPartOfPath(restOfPath, pathPart, &restOfPath);
			anErr = GetDirID(&spec, &dirID);
			spec.name[0] = 0;
			spec.parID = dirID;
	        CToPascalString(pathPart, spec.name);
		}
	}
	if (!anErr)
		*aSpec = spec;
}

void stlstr2str255(Str255& rs255, std::string s)
{
	rs255[0] = s.size();
	strcpy((char*)(&(rs255[1])), s.c_str());
}


}

#endif	// _Mac


// Currently this is not working (mo)
/*#ifdef _DEBUG

//! Header for allocated memory
#pragma pack(push, 1)
struct SMemoryHeader
{
	tint iBlockSize;
	void* pAddressReturned;
};
#pragma pack(pop)

//! Array of allocated blocks
SMemoryHeader** gppAllocated = NULL;
//! Amount of memory blocks currently allocated
tint giMemoryBlocksAllocated = 0;
//! Amount of memory blocks currently free
tint giMemoryBlocksFree = 0;
//! Amount of memory blocks to allocate at a time
const tint giMemoryBlocksToAllocate = 1000;
//! Total amount of memory currently allocated
tint giMemoryAllocated = 0;

void* AllocateMem(size_t s)
{
	if (s == 0) {
		return NULL;
	}

	if (giMemoryBlocksAllocated == 0) {
		// Allocate initial amount
		gppAllocated = (SMemoryHeader**)malloc(giMemoryBlocksToAllocate * sizeof(void*));
		giMemoryBlocksAllocated = giMemoryBlocksToAllocate;
		giMemoryBlocksFree = giMemoryBlocksToAllocate;

		memset((void*)gppAllocated, 0, giMemoryBlocksToAllocate * sizeof(void*));
	}

	if (giMemoryBlocksFree == 0) {
		// Realloc to make more space
		gppAllocated = (SMemoryHeader**)realloc((void*)gppAllocated, (giMemoryBlocksAllocated + giMemoryBlocksToAllocate) * sizeof(void*));

		memset((void*)(gppAllocated + giMemoryBlocksAllocated), 0, giMemoryBlocksToAllocate * sizeof(void*));

		giMemoryBlocksAllocated += giMemoryBlocksToAllocate * sizeof(void*);
		giMemoryBlocksFree = giMemoryBlocksToAllocate;
	}

	void* p = malloc(s + sizeof(SMemoryHeader));
	SMemoryHeader* pHeader = (SMemoryHeader*)p;
	p = (void*)(((char*)p) + sizeof(SMemoryHeader));

	pHeader->iBlockSize = s;
	pHeader->pAddressReturned = p;

	giMemoryAllocated += s;

	// Find free space
	// This cannot fail (in theory), because we know there's a free entry, so it's ok to make an endless loop and break out of it when we found a free entry
	tint iIndex = 0;
	while (1) {
		if (gppAllocated[iIndex] == NULL) {
			gppAllocated[iIndex] = pHeader;
			giMemoryBlocksFree--;
			break;
		}
		iIndex++;
	}

	return p;
}

void DeAllocateMem(void* p)
{
	if (p == NULL) {
		return;
	}

	p = (void*)(((char*)p) - sizeof(SMemoryHeader));

	SMemoryHeader* pHeader = (SMemoryHeader*)p;
	giMemoryAllocated -= pHeader->iBlockSize;

	// Find the block and remove it from array
	tint iIndex = 0;
	while(1) {
		if (gppAllocated[iIndex] == pHeader) {
			gppAllocated[iIndex] = NULL;
			giMemoryBlocksFree++;
			break;
		}
		iIndex++;
	}

	free(pHeader);
}

void* operator new(size_t s)
{
	return AllocateMem(s);
}

void operator delete(void* p)
{
	DeAllocateMem(p);
}

void* operator new[](size_t s)
{
	return AllocateMem(s);
}

void operator delete[](void* p)
{
	DeAllocateMem(p);
}

void ReportMemoryLeaks()
{
	// Index of last leaked block
	tint iLeakedIndex = -1;
	// Count number of leaked blocks
	tint iNrOfLeakedBlocks = 0;
	tint iIndex;
	for (iIndex = 0; iIndex < giMemoryBlocksAllocated; iIndex++) {
		if (gppAllocated[iIndex] != NULL) {
			iNrOfLeakedBlocks++;
			iLeakedIndex = iIndex;
		}
	}

	if (iNrOfLeakedBlocks != 0) {
#ifdef WIN32
		char psz[128];
		sprintf(psz, "Memory leaks: %d\nLast block leaked:\nSize: %d\nAddress: %d",
			iNrOfLeakedBlocks,
			gppAllocated[iLeakedIndex]->iBlockSize,
			(int)(gppAllocated[iLeakedIndex]->pAddressReturned));
		::MessageBox(NULL, psz, NULL, MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
#endif
	}
}

#else	// _DEBUG
*/
/*
void* operator new(size_t s)
{
	void* p = malloc(s);

	return p;
}

void operator delete(void* p)
{
	free(p);
}

void* operator new[](size_t s)
{
	void* p = malloc(s);

	return p;
}

void operator delete[](void* p)
{
	free(p);
}

void ReportMemoryLeaks()
{
	// Release version is empty
}
*/

/*#endif	// _DEBUG else
*/


namespace k2s {


#ifdef WIN32
void OpenBrowser(const tchar* pszAddress)
{
	::ShellExecute(0, "open", (const char*)pszAddress, NULL, NULL, SW_SHOW);
}
#endif	// WIN32

#ifdef _Mac
#include <InternetConfig.h>
void OpenBrowser(const tchar* pszAddress)
{
	OSStatus err;
	ICInstance inst;
	long startSel;
	long endSel;

	if(ICStart(&inst, '????') == noErr) {
		std::string spUrl = std::string((const char*)pszAddress);
		spUrl.insert(0, 1, (char)spUrl.size());
		const char* spUrlStr = spUrl.c_str();

		startSel = 0;
		endSel = spUrlStr[0];
		err = ICLaunchURL(inst, "\p", (char*)&spUrlStr[1], spUrlStr[0], &startSel, &endSel);

		ICStop( inst );
	}
}
#endif	// _Mac

#ifdef WIN32
void StartApplication(const tchar* pszPathName)
{
	std::string sPath((char*)pszPathName);
	CFile::PathToOS(sPath);

	::ShellExecute(0, "open", sPath.c_str(), NULL, NULL, SW_SHOW);
}

void OpenDocument(const tchar* pszPathName)
{
	StartApplication(pszPathName);
}
#endif	// WIN32

#ifdef _Mac
#include <CFUrl.h>
void StartApplication(const tchar* pszPathName)
{
	std::string sPath((char*)pszPathName);
	CFile::PathToOS(sPath);

	FSSpec Spec;
	FileNameToFSSpec(sPath.c_str(), &Spec);

	FSRef Ref;
	OSStatus err;
	err = ::FSpMakeFSRef(
		&Spec,
		&Ref);

	if (err == noErr) {
		err = ::LSOpenFSRef(
			&Ref,
			NULL);
	}
}

void OpenDocument(const tchar* pszPathName)
{
	StartApplication(pszPathName);
}
#endif	// _Mac


#ifdef _Mac

}	// namespace k2s

int stricmp(const char* pszA, const char* pszB)
{
	std::string sA;
	tint iLength = strlen(pszA);
	tint iIndex;
	for (iIndex = 0; iIndex < iLength; iIndex++) {
		if (pszA[iIndex] >= 'A' && pszA[iIndex] <= 'Z') {
			char psz[2];
			psz[0] = pszA[iIndex] - ('A' - 'a');
			psz[1] = 0;
			sA += std::string((const char*)psz);
		}
		else {
			char psz[2];
			psz[0] = pszA[iIndex];
			psz[1] = 0;
			sA += std::string((const char*)psz);
		}
	}

	std::string sB;
	iLength = strlen(pszB);
	for (iIndex = 0; iIndex < iLength; iIndex++) {
		if (pszB[iIndex] >= 'A' && pszB[iIndex] <= 'Z') {
			char psz[2];
			psz[0] = pszB[iIndex] - ('A' - 'a');
			psz[1] = 0;
			sB += std::string((const char*)psz);
		}
		else {
			char psz[2];
			psz[0] = pszB[iIndex];
			psz[1] = 0;
			sB += std::string((const char*)psz);
		}
	}
	
	return sA.compare(sB);
}

int strnicmp(const char* pszA, const char* pszB, size_t iCount)
{
	std::string sA;
	tint iLength = strlen(pszA);
	if (iLength > iCount) {
		iLength = iCount;
	}
	tint iIndex;
	for (iIndex = 0; iIndex < iLength; iIndex++) {
		if (pszA[iIndex] >= 'A' && pszA[iIndex] <= 'Z') {
			char psz[2];
			psz[0] = pszA[iIndex] - ('A' - 'a');
			psz[1] = 0;
			sA += std::string((const char*)psz);
		}
		else {
			char psz[2];
			psz[0] = pszA[iIndex];
			psz[1] = 0;
			sA += std::string((const char*)psz);
		}
	}

	std::string sB;
	iLength = strlen(pszB);
	if (iLength > iCount) {
		iLength = iCount;
	}
	for (iIndex = 0; iIndex < iLength; iIndex++) {
		if (pszB[iIndex] >= 'A' && pszB[iIndex] <= 'Z') {
			char psz[2];
			psz[0] = pszB[iIndex] - ('A' - 'a');
			psz[1] = 0;
			sB += std::string((const char*)psz);
		}
		else {
			char psz[2];
			psz[0] = pszB[iIndex];
			psz[1] = 0;
			sB += std::string((const char*)psz);
		}
	}
	
	return sA.compare(sB);
}

namespace k2s {

#endif	// _Mac

	void PszEnumString2List(const tchar* psz, tchar cDelimiter, std::list<std::string>& rlistOfStrings)
	{
		// Translate delimited string to std::list
		{
			const tchar* pszIx0 = psz;
			const tchar* pszIx1 = pszIx0;
			while (*pszIx1) {
				pszIx1++;
				if ((*pszIx1 == '\0') || (*pszIx1 == cDelimiter)) {
					tint iLen = pszIx1 - pszIx0;
					tchar* psz = new tchar[iLen + 1];
					memcpy(psz, pszIx0, iLen);
					psz[iLen] = '\0';
					rlistOfStrings.push_back(psz);
					delete[] psz;
					pszIx0 = pszIx1 + 1;
				}
			}
		}
	}
	
	
	
	void Gen_UUID( tchar* pszUUID, tuint32 uiBuffer_Size)
	{


		std::string sUUID	= "";
		char psz[4];
		
		CFUUIDRef uuid = CFUUIDCreate (NULL );
		CFUUIDBytes uuid_bytes = CFUUIDGetUUIDBytes(uuid);
		
		sprintf(psz, "%02x", uuid_bytes.byte0 ); 	
		sUUID += psz;
		sprintf(psz, "%02x", uuid_bytes.byte1 ); 	
		sUUID += psz;
		sprintf(psz, "%02x", uuid_bytes.byte2 ); 
		sUUID += psz;
		sprintf(psz, "%02x", uuid_bytes.byte3 ); 	sUUID += psz;
		sprintf(psz, "-%02x", uuid_bytes.byte4 ); 	sUUID += psz;
		sprintf(psz, "%02x", uuid_bytes.byte5 ); 	sUUID += psz;
		sprintf(psz, "-%02x", uuid_bytes.byte6 ); 	sUUID += psz;
		sprintf(psz, "%02x", uuid_bytes.byte7 ); 	sUUID += psz;
		sprintf(psz, "-%02x", uuid_bytes.byte8 ); 	sUUID += psz;
		sprintf(psz, "%02x", uuid_bytes.byte9 ); 	sUUID += psz;
		sprintf(psz, "-%02x", uuid_bytes.byte10); 	sUUID += psz;
		sprintf(psz, "%02x", uuid_bytes.byte11); 	sUUID += psz;
		sprintf(psz, "%02x", uuid_bytes.byte12); 	sUUID += psz;
		sprintf(psz, "%02x", uuid_bytes.byte13); 	sUUID += psz;
		sprintf(psz, "%02x", uuid_bytes.byte14); 	sUUID += psz;
		sprintf(psz, "%02x", uuid_bytes.byte15); 	sUUID += psz;
		

		uiBuffer_Size = sUUID.size();
		strcpy(pszUUID, sUUID.c_str());
		
		
		
	}
}




