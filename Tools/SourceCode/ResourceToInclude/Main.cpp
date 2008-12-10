
#ifndef _WIN32
	#include <stdio.h>
	#define _Mac
	#include <Carbon/Carbon.h>
	#include "bu.h"
#endif // #ifndef _WIN32

#include "RTIOS.h"

//#define _L_TEST
#ifdef _L_TEST
int counter[]={1,1,1,1};

struct S_Test {
	tint32 miSize;
	void* mpContents;
	S_Test() {
		miSize = 0;
		mpContents = (void*)counter;
	};
};
#endif // #ifdef _L_TEST


int main(int argc, char* argv[])
{
	if (argc <= 2) {
		printf("Syntax: ResourceToInclude <generated-file.inc> <resource> [<resource> ...]\n");
		return 1;
	}
	tchar pszfOut[1024];
	IFile::PathToOS2(argv[1], pszfOut);
	CAutoDelete<IFile> fOut(IFile::Create());
	fOut->Open(pszfOut, IFile::FileCreate);
	for (int i=2; i<argc; i++) {
		tchar pszfIn[1024];
		IFile::PathToOS2(argv[i], pszfIn);
		printf("%02d: %s\n", i, pszfIn);
		CAutoDelete<IFile> fIn(IFile::Create());
		fIn->Open(pszfIn, IFile::FileRead);

		tchar pszNameBuff[1024];
		tint32 iSlash = sprintf(pszNameBuff, pszfIn);
		while (
			(iSlash > 0)
			&& 
			((pszNameBuff[iSlash] != '/') && (pszNameBuff[iSlash] != ':') && (pszNameBuff[iSlash] != '\\'))
			)
		{
			char c = pszNameBuff[iSlash];
			if (
				((c >= 'A') && (c <= 'Z'))
				||
				((c >= 'a') && (c <= 'z'))
				||
				((c >= '0') && (c <= '9'))
				||
				(c == '\0')
			);
			else
				pszNameBuff[iSlash] = '_';
			iSlash--;
		}
		tchar* pszName = pszNameBuff + iSlash + 1;
		tchar pszLine[1024];
#ifdef _WIN32
		tchar NL[3] = "\r\n";
#else _WIN32
		tchar NL[2] = "\n";
#endif // #else _WIN32
		tint32 iInSize = (tint32)fIn->GetSizeWhenOpened();
		tbool bLargeFile = (iInSize > 0x1000);

		tint iLineLen;

		if (bLargeFile) {
			// For large input files we must output signed int 32 arrays

			// Load the data
			tint32 iSizeDiv16x4 =  (iInSize / (16 * sizeof(tuint32))) + 1;
			tint32 iSizeArray = iSizeDiv16x4 * 16;
			tchar* pszInBuff = (tchar*)(new tuint32[iSizeArray]);
			if (iInSize > 0)
				fIn->Read(pszInBuff, iInSize);

			for (tint iSwapped = 0; iSwapped < 2; iSwapped++) {
				// First pre-processor directive stating whether we're a Motorola CPU or not
				iLineLen = sprintf(pszLine, "%s%s%s", NL, (iSwapped != 0) ? "#ifdef _Mac_PowerPC" : "#ifndef _Mac_PowerPC", NL);
				fOut->Write(pszLine, iLineLen);

				// Then the array variable
				iLineLen = sprintf(pszLine, "static const tint32 _gai_%s[] = {%s", pszName, NL);
				fOut->Write(pszLine, iLineLen);
				
				// Write data
				tuint32* puiSrc = (tuint32*)pszInBuff;
				for (tint i = 0; i < iSizeDiv16x4; i++) {
					tchar* pcDst = pszLine;
					tint iChars;
					iChars = sprintf(pcDst, "\t");
					pcDst += iChars;
					iLineLen = iChars;
					
#ifdef _Mac_PowerPC
					const tint32 kiWhenToSwap = 0;
#else _Mac_PowerPC
					const tint32 kiWhenToSwap = 1;
#endif // _Mac_PowerPC

					for (tint i2 = 0; i2 < 16; i2++) {
						tuint32 ul = *puiSrc++;
						if (iSwapped == kiWhenToSwap) {
							ul = (((ul>>24) & 0x000000FFL) | ((ul>>8) & 0x0000FF00L) | ((ul<<8) & 0x00FF0000L) | ((ul<<24) & 0xFF000000L));
						}
						if (ul == 0x80000000L)
							iChars = sprintf(pcDst, "(tint32)0x80000000L,");
						else
							iChars = sprintf(pcDst, "%d,", ul);
						iLineLen += iChars;
						pcDst += iChars;
					}

					iChars = sprintf(pcDst, "%s", NL);
					iLineLen += iChars;
					fOut->Write(pszLine, iLineLen);
				}

				// End of array
				iLineLen = sprintf(pszLine, "\t0%s};%s", NL, NL);
				fOut->Write(pszLine, iLineLen);

				// End of preprocessor directive
				iLineLen = sprintf(pszLine, "#endif%s%s", NL, NL);
				fOut->Write(pszLine, iLineLen);
			}

			iLineLen = sprintf(pszLine, "const tint32 giSize_%s = %u;%s", pszName, iInSize, NL);
			fOut->Write(pszLine, iLineLen);
			iLineLen = sprintf(pszLine, "const void* gp_%s = (const void*)_gai_%s;%s%s", pszName, pszName, NL, NL);
			fOut->Write(pszLine, iLineLen);
		}

		if (!bLargeFile) {
			// Small file - output as string, 16 chars a line

			iLineLen = sprintf(pszLine, "const tint32 giSize_%s = %u;%s", pszName, iInSize, NL);
			fOut->Write(pszLine, iLineLen);
			iLineLen = sprintf(pszLine, "const void* gp_%s = (const void*)%s", pszName, NL);
			fOut->Write(pszLine, iLineLen);

			tint32 iSizeDiv16 =  (iInSize / 16) + 1;
			tint32 iSizeArray = iSizeDiv16 * 16;
			tchar* pszInBuff = new tchar[iSizeArray];
			if (iInSize > 0)
				fIn->Read(pszInBuff, iInSize);
			tchar* pcSrc = pszInBuff;
			for (tint i = 0; i < iSizeDiv16; i++) {
				tchar* pcDst = pszLine;
				tint iChars;
				iChars = sprintf(pcDst, "\t\t\t\"");
				pcDst += iChars;
				iLineLen = iChars;
				unsigned char ucLast = 1;
				for (tint i2=0; i2<16; i2++) {
					unsigned char uc = (unsigned char)*pcSrc++;
					if (uc < 32) {
						if (ucLast == 0) {
							pcDst -= 2;
							iLineLen -= 2;
						}
						switch (uc) {
							case 7: iChars = sprintf(pcDst, "\\a"); break;
							case 8: iChars = sprintf(pcDst, "\\b"); break;
							case 9: iChars = sprintf(pcDst, "\\t"); break;
							case 10: iChars = sprintf(pcDst, "\\n"); break;
							case 13: iChars = sprintf(pcDst, "\\r"); break;
							default: iChars = sprintf(pcDst, "\\%03o", uc); break;
						}
					}
					else if (uc == '\\')
						iChars = sprintf(pcDst, "\\\\");
					else if (uc == '"')
						iChars = sprintf(pcDst, "\\\"");
					else if ((uc >= '0') && (uc <= '9')) {
						iChars = 1;
						*pcDst = (tchar)uc;
					}
					else if (uc <= 127) {
						if (ucLast == 0) {
							pcDst -= 2;
							iLineLen -= 2;
						}
						iChars = 1;
						*pcDst = (tchar)uc;
					}
					else {
						if (ucLast == 0) {
							pcDst -= 2;
							iLineLen -= 2;
						}
						iChars = sprintf(pcDst, "\\%03o", uc);
					}
					ucLast = uc;
					pcDst += iChars;
					iLineLen += iChars;
				}
				if (ucLast == 0) {
					pcDst -= 2;
					iLineLen -= 2;
				}
				iChars = sprintf(pcDst, "\"%s", NL);
				iLineLen += iChars;
				fOut->Write(pszLine, iLineLen);
			}

			delete[] pszInBuff;
			pszInBuff = NULL;

			iLineLen = sprintf(pszLine, "\t\t;%s\t};%s} gs_%s;%s%s", NL, NL, pszName, NL, NL);
			fOut->Write(pszLine, iLineLen);
		}
	}
	printf("done\n");
	//printf("iSize == %d, strlen() => %d\n", gs_KobloStudio_ico.iSize, strlen(gs_KobloStudio_ico.pszContents));

	return 0;
}