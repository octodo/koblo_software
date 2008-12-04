// ResComp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

#pragma pack(push, 1)
struct SResourceInfo {
	long lID;
	long lType;
	long lFileOffset;
	long lSize;
};
#pragma pack(pop)

struct SResource {
	string sFileName;
	string sDefineName;
	long lSize;
	long lID;
	long lType;
};

// Current version
#define VERSION 1

#define ID_0 1000

long lCurId = ID_0;

#define RESTYPE_UNKNOWN 0
#define RESTYPE_TGA 1
#define RESTYPE_FNT 2
#define RESTYPE_BIN 3

const long lNrOfKnownExt = 4;
char apszKnownExts[lNrOfKnownExt][20] = {{".tga"}, {".fnt"}, {".ico"}, {".icns"}};
tint32 aiKnownExt2ResType[lNrOfKnownExt] = { 
	RESTYPE_TGA, RESTYPE_FNT, 
	RESTYPE_BIN, RESTYPE_BIN
};


IChunk* pChunkList;
IChunk* pChunkResources;
IChunkFile* pChunkFile;

#ifdef _Mac
	//#define _write fwrite
#endif

bool WriteDefineHeader(int iFileOut, list<SResource*>& ResourceList)
{
	list<SResource*>::iterator it;
	for (it = ResourceList.begin(); it != ResourceList.end(); it++) {
		SResource* pResource = (*it);

		char psz[256];
		sprintf(psz, "#define %s %d\n", pResource->sDefineName.c_str(), pResource->lID);
		_write(iFileOut, psz, strlen(psz));
	}

	return true;
}

bool WriteFile()
{
	pChunkFile = IChunkFile::Create();
	tint32 iFileVersion = 1;
	pChunkFile->Open((const tchar*)"res.k2s", IFile::FileCreate, iFileVersion);

	pChunkFile->SetChunk(pChunkList);
	pChunkFile->SetChunk(pChunkResources);

	pChunkList->Destroy();
	pChunkResources->Destroy();

	pChunkFile->Destroy();

	return true;
}

bool WriteResources(list<SResource*>& ResourceList)
{
	// Calculate total size
	long lSize = 0;
	list<SResource*>::iterator it;
	for (it = ResourceList.begin(); it != ResourceList.end(); it++) {
		SResource* pResource = (*it);
		lSize += pResource->lSize;
	}

	char* p = new char[lSize];

	long lIndexCur = 0;
	for (it = ResourceList.begin(); it != ResourceList.end(); it++) {
		SResource* pResource = (*it);
		int iFileIn = _open(pResource->sFileName.c_str(), _O_BINARY | _O_SEQUENTIAL | _O_RDONLY);
		if (iFileIn == -1) {
			printf("Error opening resource file\n");
			return false;
		}

		char* pch = p + lIndexCur;

		if (_read(iFileIn, pch, pResource->lSize) != pResource->lSize) {
			printf("Error reading resource file %s\n", pResource->sFileName.c_str());
			delete[] pch;
			_close(iFileIn);
			return false;
		}

		_close(iFileIn);

		lIndexCur += pResource->lSize;
	}

	// Create resources chunk
	pChunkResources = IChunk::Create((void*)p, lSize, giChunkResources);

	delete[] p;

	return true;
}

bool BuildHeader(list<SResource*>& ResourceList)
{
	char* p = new char[(sizeof(int) * 2) + (ResourceList.size() * sizeof(SResource))];
	int* pi = (int*)p;
	pi[0] = VERSION;
	pi[1] = ResourceList.size();

	SResourceInfo* pInfo = (SResourceInfo*)(p + (sizeof(int) * 2));
	long lOffset = 0;
	list<SResource*>::iterator it;
	int i = 0;
	for (it = ResourceList.begin(); it != ResourceList.end(); it++, i++) {
		SResource* pResource = (*it);
		pInfo[i].lID = pResource->lID;
		pInfo[i].lType = pResource->lType;
		pInfo[i].lFileOffset = lOffset;
		pInfo[i].lSize = pResource->lSize;

		lOffset += pInfo[i].lSize;
	}

	// Create resource list chunk
	pChunkList = IChunk::Create((void*)p, (sizeof(int) * 2) + (ResourceList.size() * sizeof(SResourceInfo)), giChunkResourceList);
	delete[] p;

	return true;
}

bool ValidateFiles(list<SResource*>& ResourceList)
{
	list<SResource*>::iterator it;
	for (it = ResourceList.begin(); it != ResourceList.end(); it++) {
		int iFile = _open((*it)->sFileName.c_str(), _O_BINARY | _O_SEQUENTIAL | _O_RDONLY);
		if (iFile == -1) {
			printf("Error locating %s\n", (*it)->sFileName.c_str());
			return false;
		}
		long lFileSize = _lseek(iFile, 0, SEEK_END);
		(*it)->lSize = lFileSize;
		_close(iFile);
	}

	return true;
}

long FindType(string sFileName)
{
	long lIndex = sFileName.find_last_of('.');
	if (lIndex == string::npos) {
		return RESTYPE_UNKNOWN;
	}

	string sTmp = sFileName.substr(lIndex);
	char* pch = new char[1024];
	strcpy(pch, sTmp.c_str());
	strlwr(pch);

	for (long l = 0; l < lNrOfKnownExt; l++) {
		if (strcmp(pch, apszKnownExts[l]) == 0) {
			delete[] pch;
			return aiKnownExt2ResType[l];
		}
	}

	delete[] pch;
	return RESTYPE_UNKNOWN;
}

bool BuildList(int iFileIn, list<SResource*>& ResourceList)
{
	long lFileSize = _lseek(iFileIn, 0, SEEK_END);
	_lseek(iFileIn, 0, SEEK_SET);
	char* pch = new char[lFileSize];
	if (_read(iFileIn, pch, lFileSize) != lFileSize) {
		printf("Error reading from input file\n");
		return false;
	}

	char* pchLine = new char[1024];
	char* pchFileName = new char[1024];
	char* pchID = new char[1024];

	long lCharsLeft = lFileSize;
	while (lCharsLeft > 0) {
		// Get one line into pchLine
		char* pchTmp = pchLine;
		// Lasse, modified 2007-12-14 - allow for Unix end-line sequences
		//while((*pch != 13) && (lCharsLeft)) {
		while((*pch != 13) && (*pch != 10) && (lCharsLeft)) {
		// .. Lasse
			*pchTmp++ = *pch++;
			lCharsLeft--;
		}
		*pchTmp = 0;
		// Skip newline
		// Lasse, modified 2007-12-14 - allow for Unix end-line sequences
		/*
		pch += 2;
		lCharsLeft -= 2;
		*/
		while ((lCharsLeft > 0) && (*pch == 13) || (*pch == 10))
		{
			pch++;
			lCharsLeft--;
		}
		// .. Lasse
		char* pchLineOrg = pchLine;
		if (*pchLine == 0) {
			continue;
		}

		// Lasse, added 2007-12-14
		// Skip initial space and tab
		while ((*pchLine == ' ') || (*pchLine == '\t'))
			pchLine++;
		// Skip entire comment line
		if (*pchLine == '#')
			continue;
		// .. Lasse

		// Get filename into pchFileName
		pchTmp = pchFileName;
		while ((*pchLine != ' ') && (*pchLine != '\t')) {
			*pchTmp++ = *pchLine++;
		}
		// Syntax error check
		if (*pchLine == 0) {
			printf("Syntax error in input file - unexpected EOL\n");
			return false;
		}
		// Make zero-terminated string
		*pchTmp = 0;
		// Skip spaces
		while ((*pchLine == ' ') || (*pchLine == '\t')) {
			pchLine++;
		}
		// Syntax error check
		if (*pchLine == 0) {
			printf("Syntax error in input file - unexpected EOL\n");
			return false;
		}

		// Get id into pchID
		pchTmp = pchID;
		while((*pchLine != ' ') && (*pchLine != '\t') && (*pchLine != 0)) {
			*pchTmp++ = *pchLine++;
		}
		*pchTmp = 0;
		pchLine = pchLineOrg;

		// Parse ID and save info
		SResource* pResource = new SResource;
		pResource->sFileName = pchFileName;
		pResource->sDefineName = pchID;
		pResource->lID = lCurId++;
		pResource->lType = FindType(pResource->sFileName);
		if (pResource->lType == RESTYPE_UNKNOWN) {
			printf("Warning: unknown resource type for %s\n", pResource->sFileName.c_str());
		}
		ResourceList.insert(ResourceList.end(), pResource);
	}

	delete[] pchLine;
	delete[] pchFileName;
	delete[] pchID;

	return true;
}


void PrintUsage()
{
	printf("Key2Sound Resource Compiler version 1.0\nCopyright (c) 2004 Key2Sound\nAll rights reserved world-wide\n");
}

int main(int argc, char* argv[])
{
	PrintUsage();

	if (argc > 1) {
		getchar();
		return 1;
	}

	int iFileIn = _open("res.list", _O_BINARY | _O_SEQUENTIAL | _O_RDONLY);
	if (iFileIn == -1) {
		printf("Couldn't open input file (res.list)\n");
		getchar();
		return 2;
	}

	// Lasse, modified 2007-12-13 - use Unix style newlines
	/*
	int iFileHeader = _open("Resources.h", _O_TEXT | _O_CREAT | _O_SEQUENTIAL | _O_TRUNC | _O_WRONLY, _S_IREAD | _S_IWRITE);
	*/
	int iFileHeader = _open("Resources.h", _O_BINARY | _O_CREAT | _O_SEQUENTIAL | _O_TRUNC | _O_WRONLY, _S_IREAD | _S_IWRITE);
	// .. Lasse
	if (iFileHeader == -1) {
		printf("Couldn't open resource header file\n");
		getchar();
		return 2;
	}

	printf("Building list...\n");
	list<SResource*> ResourceList;
	if (BuildList(iFileIn, ResourceList) == false) {
		getchar();
		return 3;
	}

	printf("Validating files...\n");
	if (ValidateFiles(ResourceList) == false) {
		getchar();
		return 4;
	}

	printf("Building resource list chunk...\n");
	if (BuildHeader(ResourceList) == false) {
		getchar();
		return 5;
	}

	printf("Building resource chunk...\n");
	if (WriteResources(ResourceList) == false) {
		getchar();
		return 6;
	}

	printf("Writing file...\n");
	if (WriteFile() == false) {
		getchar();
		return 7;
	}

	printf("Writing C++ header...\n");
	if (WriteDefineHeader(iFileHeader, ResourceList) == false) {
		getchar();
		return 7;
	}

	_close(iFileHeader);
	_close (iFileIn);
	printf("Done. Press enter to exit\n");
	getchar();

	list<SResource*>::iterator it;
	for (it = ResourceList.begin(); it != ResourceList.end(); it++) {
		delete (*it);
	}
	ResourceList.clear();

	return 0;
}

