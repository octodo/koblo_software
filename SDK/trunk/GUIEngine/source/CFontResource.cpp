/*!	\file CFontResource.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "geInternalOS.h"


CFontResource::CFontResource()
	: mpiLength(NULL), mpiOffset(NULL), miNrOfChars(256), CBitmapResource(1, 1)
{
	mResType = eResTypeFnt;
}

CFontResource::~CFontResource()
{
	if (mpiLength) {
		delete[] mpiLength;
	}
	if (mpiOffset) {
		delete[] mpiOffset;
	}

	if (mppDC) {
		tint iChar;
		for (iChar = 0; iChar < miNrOfChars; iChar++) {
			mppDC[iChar]->Destroy();
		}

		delete[] mppDC;
		mppDC = NULL;
	}
}

IFontResource* IFontResource::Create()
{
	return dynamic_cast<IFontResource*>(new CFontResource());
}

void CFontResource::Destroy()
{
	delete dynamic_cast<CFontResource*>(this);
}

void CFontResource::Load_Deprecated(IWindow* pWnd, IFile* pFile, tint iSize, tint32 iResID, tint iNrOfChars, const tint* piCharSizes, const SRGB& TextColour)
{
	mColour = TextColour;

	CResource::Load(pWnd, pFile, iSize, iResID);

	miBytesPerRow = 0;

	LoadTga();

	miNrOfChars = iNrOfChars;
	mppDC = new IDC*[miNrOfChars];

	mpiLength = new tint[miNrOfChars];
	memset(mpiLength, 0, miNrOfChars * sizeof(tint));

	mpiOffset = new tint[miNrOfChars];
	memset(mpiOffset, 0, miNrOfChars * sizeof(tint));

	tint iOffsetCur = 0;
	tint iIndex;
	for (iIndex = 0; iIndex < miNrOfChars; iIndex++) {
		// Length of the character we're working on this iteration
		tint iLength = piCharSizes[iIndex];

		// Set the length
		mpiLength[iIndex] = iLength;
		// Set the offset
		mpiOffset[iIndex] = iOffsetCur;

		// Calculate offset for next iteration
		iOffsetCur += iLength;
	}

	// Change the graphics from grayscale to coloured RGB with alpha
	miBitsPerPixel = 32;
	const tuchar* pData = GetData();

	for (tint iY = 0; iY < mSize.iCY; iY++) {
		// Our source pointer
		tuchar* p = (tuchar*)pData;
		// Increment for current y position
		p += iY * mSize.iCX * 4;

		for (tint iX = 0; iX < mSize.iCX; iX++) {
			tint iAlpha = *p;

#ifdef WIN32
			*p++ = TextColour.u.gun.b;
			*p++ = TextColour.u.gun.g;
			*p++ = TextColour.u.gun.r;
			*p++ = (tuchar)iAlpha;
#endif	// WIN32
#ifdef _Mac
			*p++ = TextColour.u.gun.b;
			*p++ = TextColour.u.gun.g;
			*p++ = TextColour.u.gun.r;
			*p++ = (tuchar)iAlpha;
#endif	// _Mac
		}
	}

	tint iChar;
	tint iWidthIndex = 0;
	for (iChar = 0; iChar < miNrOfChars; iChar++) {
		// The DC we're working on
		tint iIndex = iChar;

		mppDC[iIndex] = IDC::Create();
		mppDC[iIndex]->Init(pWnd,
			dynamic_cast<IBitmapResource*>(this),
			iWidthIndex,
			0);

		iWidthIndex += mpiLength[iIndex];
	}

	// Lasse - added 2007-06-01
	miTopLine = 0;
	miBaseLine = mSize.iCY - 1;
	// .. Lasse
} // CFontResource::Load_Deprecated(...)


void CFontResource::Load(IWindow* pWnd, IFile* pFile, tint iSize, tint32 iResID, tint iNrOfChars, const tint* piCharSizes, const SRGB& TextColour)
{
	if (piCharSizes != NULL) {
		Load_Deprecated(pWnd, pFile, iSize, iResID, iNrOfChars, piCharSizes, TextColour);
		return;
	}

	mColour = TextColour;

	CResource::Load(pWnd, pFile, iSize, iResID);

	miBytesPerRow = 0;

	LoadTga();

#ifdef _DEBUG
	if (miBitsPerPixel != 32) {
		tchar pszFontFile[512];
		pFile->GetPathName(pszFontFile);
		tchar pszMsg[1024];
		sprintf(pszMsg, "The font bitmap doesn't contain an alpha channel (%s).", pszFontFile);
		throw IException::Create(IException::TypeGeneric, IException::ReasonGeneric, EXCEPTION_INFO, pszMsg);
	}
#endif // #ifdef _DEBUG

	const tuchar* pData = GetData();

	miNrOfChars = 0; //iNrOfChars;
	mpiOffset = new tint[giMaxNrOfCharsInFont]; //tint[miNrOfChars];
	memset(mpiOffset, 0, giMaxNrOfCharsInFont * sizeof(tint)); //miNrOfChars * sizeof(tint));
	mpiLength = new tint[giMaxNrOfCharsInFont]; //tint[miNrOfChars];
	memset(mpiLength, 0, giMaxNrOfCharsInFont * sizeof(tint)); //miNrOfChars * sizeof(tint));

	// Count actual number of chars
	{
		tint iX = 0, iY = 0;
		miTopLine = miBaseLine = -1;
		tint* piData = (tint*)pData;
		tint* piOfs = mpiOffset + 32;
		tint* piLens = mpiLength + 32;
		tint iXStart = 0;
		tint** piaryOffsets = new tint*[mSize.iCY];
		for (iY = 0; iY < mSize.iCY; iY++) {
			// Initialize for 
			piaryOffsets[iY] = piData + iY * mSize.iCX;
		}
		for (iX = 0; iX < mSize.iCX; iX++) {
			for (iY = 0; iY < mSize.iCY; iY++) {
				tint* piPixels = piaryOffsets[iY]++;
				tuchar* pucAlpha = (tuchar*)piPixels;
				pucAlpha += 3;	// Alpha channel byte
				//piPixels++; // Advance to next pixel
				if (*pucAlpha == 0) {
					// We found a char delimiter mark in alpha channel

					if (miNrOfChars == 0) {
						// This is very first char delimiter

						if (miTopLine < 0) {
							// First pixel of very first char delimiter
							miTopLine = miBaseLine = iY;
						}
						else {
							miBaseLine = iY;
						}
					}
					else {
						// Not the first char delimiter

						// Advance after delimiter
						miNrOfChars++;
						*piOfs++ = iXStart;
						*piLens++ = iX + 1 - iXStart;
						iXStart = iX + 1;

						// Don't look further down this Y-axis
						iY = mSize.iCY;
					}
				}
			}
			if ((miNrOfChars == 0) && (miTopLine >= 0)) {
				// Done with first delimiter

				// Advance after first delimiter
				miNrOfChars++;
				*piOfs++ = iXStart;
				*piLens++ = iX + 1 - iXStart;
				iXStart = iX + 1;
			}
		}

		// Always skip first 32 chars
		miNrOfChars += 32;
	}

#ifdef _DEBUG
	// Verify number of chars in font
	{
		if (iNrOfChars <= 32) {
			// Totally generic font - 
			if ((miNrOfChars <= 32) || (miNrOfChars > giMaxNrOfCharsInFont)) {
				tchar pszMsg[1024];
				sprintf(pszMsg,
					"Unexpected nb of chars in font (expected between 32 and %d, found %d ( - 32 = %d))\nRessource-ID: %d",
					giMaxNrOfCharsInFont, miNrOfChars, miNrOfChars - 32,
					iResID
				);
				throw IException::Create(IException::TypeGeneric, IException::ReasonGeneric, EXCEPTION_INFO, pszMsg);
			}
		}
		else {
			// Generic font with fixed number of chars

			if (miNrOfChars != iNrOfChars) {
				tchar pszFontFile[512];
				pFile->GetPathName(pszFontFile);
				tchar pszMsg[1024];
				sprintf(pszMsg,
					"Unexpected nb of chars in font (expected %d, found %d)\nRessource-ID: %d",
					iNrOfChars, miNrOfChars,
					iResID
				);
				throw IException::Create(IException::TypeGeneric, IException::ReasonGeneric, EXCEPTION_INFO, pszMsg);
			}
		}
	}
#endif // #ifdef _DEBUG

	mppDC = new IDC*[miNrOfChars];


	// Change the graphics from grayscale to coloured RGB with alpha
	miBitsPerPixel = 32;

	for (tint iY = 0; iY < mSize.iCY; iY++) {
		// Our source pointer
		tuchar* p = (tuchar*)pData;
		// Increment for current y position
		p += iY * mSize.iCX * 4;

		for (tint iX = 0; iX < mSize.iCX; iX++) {
			// Read blue channel only and then treat it as alpha channel
			tuchar ucAlphaFromBlueChannel = *p;

#ifdef WIN32
			*p++ = TextColour.u.gun.b;
			*p++ = TextColour.u.gun.g;
			*p++ = TextColour.u.gun.r;
			*p++ = ucAlphaFromBlueChannel;
#endif	// WIN32
#ifdef _Mac
			*p++ = TextColour.u.gun.b;
			*p++ = TextColour.u.gun.g;
			*p++ = TextColour.u.gun.r;
			*p++ = ucAlphaFromBlueChannel;
#endif	// _Mac
		}
	}

	tint iChar;
	tint iWidthIndex = 0;
	for (iChar = 0; iChar < miNrOfChars; iChar++) {
		// The DC we're working on
		tint iIndex = iChar;

		mppDC[iIndex] = IDC::Create();
		mppDC[iIndex]->Init(pWnd,
			dynamic_cast<IBitmapResource*>(this),
			iWidthIndex,
			0);

		iWidthIndex += mpiLength[iIndex];
	}
} // CFontResource::Load(...)


tint CFontResource::GetLength(tint iIndex) const
{
	return mpiLength[iIndex];
}

tint CFontResource::GetOffset(tint iIndex) const
{
	return mpiOffset[iIndex];
}

tint CFontResource::GetTextWidth(const tchar* psz) const
{
	tint iStrLength = strlen((char*)psz);
	tint iWidth = 0;
	for (tint i = 0; i < iStrLength; i++) {
		iWidth += GetLength((tuchar)psz[i]);
	}

	return iWidth;
}

tint CFontResource::GetTextHeight() const
{
	return GetHeight();
}

tint CFontResource::GetFontTopLine() const
{
	return miTopLine;
}


tint CFontResource::GetFontBaseLine() const
{
	return miBaseLine;
}


void CFontResource::DrawTextInMemory(const tchar* pszText, const SPos& Pos, const SRGB& Colour, tchar* pDestBuffer, tint iBytesPerRow)
{
	SSize SizeThis;
	GetSize(SizeThis);
	tint iCharacter;
	tint iTextSize = strlen((char*)pszText);
	tint iDestinationXOffset = 0;
	for (iCharacter = 0; iCharacter < iTextSize; iCharacter++) {
		// ascii value
		tint iAscii = pszText[iCharacter];

		// X offset for character
		tint iOffsetX = mpiOffset[iAscii];

		// Do the blending
		for (tint iY = 0; iY < SizeThis.iCY; iY++) {
			// Our source pointer
			const tuchar* pSrc = GetData();
			// Increment for character
			pSrc += (iOffsetX * 4);
			// Increment for current y position
			pSrc += iY * GetWidth() * 4;

			// Our destination pointer
			tuchar* pDest = (tuchar*)pDestBuffer;
			// Increment for current destination x position
			pDest += (Pos.iX + iDestinationXOffset) * 4;
			// Increment for current y position
			pDest += ((Pos.iY + iY) * iBytesPerRow);

			for (tint iX = 0; iX < mpiLength[iAscii]; iX++) {
				tint iAlpha = *pSrc;

				*pDest++ = (((Colour.u.gun.b * iAlpha) >> 8) +
					((*pDest * (255 - iAlpha)) >> 8));

				*pDest++ = (((Colour.u.gun.g * iAlpha) >> 8) +
					((*pDest * (255 - iAlpha)) >> 8));

				*pDest++ = (((Colour.u.gun.r * iAlpha) >> 8) +
					((*pDest * (255 - iAlpha)) >> 8));

				pDest++;

				pSrc += 4;
			}
		}

		iDestinationXOffset += mpiLength[iAscii];
	}
}

void CFontResource::Draw(IWindow* pWnd, const SRect& rctUpdate, const SRect& rctDraw, const std::string& sText)
{
	SPos TextPos;
	TextPos.iX = rctDraw.iX;
	TextPos.iY = rctDraw.iY;

	tint iTextLength = sText.size();
	for (tint iIndex = 0; iIndex < iTextLength; iIndex++) {
		tint iAscii = (unsigned char)sText[iIndex];
		if ((iAscii >= miNrOfChars) || (iAscii < 32)) {
			iAscii = (miNrOfChars > 127) ? 127 : miNrOfChars - 1;
		}
		tint iCharLengthPixels = mpiLength[iAscii];

		mppDC[iAscii]->Draw(pWnd, rctUpdate, SRect(TextPos, SSize(iCharLengthPixels, mSize.iCY)));

		TextPos.iX += iCharLengthPixels;
	}
}



