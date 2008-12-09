/*!	\file CBitmapResource.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "geInternalOS.h"


CBitmapResource::CBitmapResource(tint iNrOfFrames, tint iNrOfVariants)
	: miBitsPerPixel(0), miNrOfFrames(iNrOfFrames), miNrOfVariants(iNrOfVariants), mppDC(NULL)
{
	mResType = eResTypeTga;
}

CBitmapResource::~CBitmapResource()
{
	if (mppDC) {
		for (tint i = 0; i < miNrOfFrames * miNrOfVariants; i++) {
			mppDC[i]->Destroy();
		}

		delete[] mppDC;
	}
}

IBitmapResource* IBitmapResource::Create(tint iNrOfFrames, tint iNrOfVariants)
{
	return dynamic_cast<IBitmapResource*>(new CBitmapResource(iNrOfFrames, iNrOfVariants));
}

void CBitmapResource::Destroy()
{
	delete dynamic_cast<CBitmapResource*>(this);
}

void CBitmapResource::Load(IWindow* pWnd, IFile* pFile, tint iSize, tint32 iResID)
{
	CResource::Load(pWnd, pFile, iSize, iResID);

	miBytesPerRow = 0;

	LoadTga();

	tint iNrOfBitmaps = miNrOfFrames * miNrOfVariants;
	mppDC = new IDC*[iNrOfBitmaps];

	tint iWidthPerFrame = mSize.iCX / miNrOfFrames;
	tint iHeightPerVariant = mSize.iCY / miNrOfVariants;

	tint iVariant;
	for (iVariant = 0; iVariant < miNrOfVariants; iVariant++) {
		tint iFrame;
		for (iFrame = 0; iFrame < miNrOfFrames; iFrame++) {
			// The DC we're working on
			tint iIndex = iVariant * miNrOfFrames + iFrame;

			mppDC[iIndex] = IDC::Create();
			mppDC[iIndex]->Init(pWnd,
				dynamic_cast<IBitmapResource*>(this),
				iWidthPerFrame * iFrame,
				iHeightPerVariant * iVariant);
		}
	}
}

void CBitmapResource::GetSize(SSize& rSize) const
{
	rSize = mSize;
}

tint CBitmapResource::GetWidth() const
{
	return mSize.iCX;
}

tint CBitmapResource::GetHeight() const
{
	return mSize.iCY;
}

tint CBitmapResource::GetBytesPerRow() const
{
	return miBytesPerRow;
}

void CBitmapResource::GetRealSize(SSize& rSize) const
{
	rSize = mSize;

	rSize.iCX /= miNrOfFrames;
	rSize.iCY /= miNrOfVariants;
}

tint CBitmapResource::GetBitsPerPixel() const
{
	return miBitsPerPixel;
}

tint CBitmapResource::GetNrOfFrames()
{
	return miNrOfFrames;
}

tint CBitmapResource::GetNrOfVariants()
{
	return miNrOfVariants;
}

void CBitmapResource::Draw(IWindow* pWnd, const SRect& rctUpdate, const SRect& rctDraw, tint iFrame, tbool bGhost)
{
	tint iVariant = 0;

	if (bGhost) {
		iVariant = 1;
	}

	mppDC[iVariant * miNrOfFrames + iFrame]->Draw(pWnd, rctUpdate ,rctDraw);
}

// Stuff used by Targa loader

#define TGA_TYPE_MAPPED 1
#define TGA_TYPE_COLOR 2
#define TGA_TYPE_GRAY 3
#define TGA_TYPE_MAPPED_RLE 9
#define TGA_TYPE_COLOR_RLE 10
#define TGA_TYPE_GRAY_RLE 11

#define TGA_DESC_ABITS 0x0f
#define TGA_DESC_HORIZONTAL 0x10
#define TGA_DESC_VERTICAL 0x20

#define TGA_SIGNATURE "TRUEVISION-XFILE"

#pragma pack(1)
struct tga_header {
	tchar idLength;
	tchar colorMapType;

	tuchar imageType;

	// Color Map Specification.
	// We need to separately specify high and low bytes to avoid endianness and alignment problems.
	tuchar colorMapIndexLo, colorMapIndexHi;
	tuchar colorMapLengthLo, colorMapLengthHi;
	tuchar colorMapSize;

	// Image Specification
	tuchar xOriginLo, xOriginHi;
	tuchar yOriginLo, yOriginHi;

	tuchar widthLo, widthHi;
	tuchar heightLo, heightHi;

	tuchar bpp;

	// Image descriptor.
	// 3-0: attribute bpp
	// 4:   left-to-right ordering
	// 5:   top-to-bottom ordering
	// 7-6: zero
	tuchar descriptor;
};

struct tga_footer {
	tint32 extensionAreaOffset;
	tint32 developerDirectoryOffset;
	tchar signature[16];
	tchar dot;
	tchar null;
};
#pragma pack()


void CBitmapResource::LoadTga()
{
	if (size_t(miDataSize) < sizeof(tga_footer) + sizeof(tga_header)) {
		// Error in file format
		return;
	}

	// Check footer
	tga_footer* pFooter = (tga_footer*)(&mpcData[miDataSize - sizeof(tga_footer)]);
	pFooter->extensionAreaOffset = CPSwap(pFooter->extensionAreaOffset);
	pFooter->developerDirectoryOffset = CPSwap(pFooter->developerDirectoryOffset);
	if (strcmp((const char*)pFooter->signature, "TRUEVISION-XFILE.") != 0) {
		// Hmmm, not 100% sure what that might mean
		return;
	}

	// Check header
	tga_header* pHeader = (tga_header*)mpcData;
	if (pHeader->imageType != TGA_TYPE_COLOR) {
		// We only support TGA_TYPE_COLOR image types
		return;
	}
	if ((pHeader->bpp !=24) && (pHeader->bpp != 32)) {
		// Only 24 bit (rgb) or 32 bit (rgb+a) supported
		return;
	}
	miBitsPerPixel = pHeader->bpp;
//	if ((pHeader->descriptor & TGA_DESC_ABITS) != 8) {
//		return;
//	}

	tbool bHorzReversed = (pHeader->descriptor & TGA_DESC_HORIZONTAL) != 0;
	tbool bVertReversed = !(pHeader->descriptor & TGA_DESC_VERTICAL);
	if (bHorzReversed) {
		// No support for horz reversed, but should be easy to fix if neccasary
		return;
	}

	tint32 lWidth = (pHeader->widthHi << 8) | pHeader->widthLo;
	tint32 lHeight = (pHeader->heightHi << 8) | pHeader->heightLo;

	mSize = SSize(lWidth, lHeight);
	miBytesPerRow = lWidth * 4;
	tint iBytesPerRowSource = lWidth * (miBitsPerPixel / 8);

	if (miBitsPerPixel == 24) {
		// bgr
		tuchar* pchDest = new tuchar[lHeight * miBytesPerRow];
		if (pchDest == NULL) {
			return;
		}

		tuchar* pchBits = (tuchar*)(mpcData + sizeof(tga_header));
		if (bVertReversed == false) {
			for (tint lY = 0; lY < lHeight; lY++) {
				tuchar* pSrc = &(pchBits[lY * iBytesPerRowSource]);
				tuchar* pDest = &(pchDest[lY * miBytesPerRow]);
				for (tint lX = 0; lX < lWidth; lX++) {
#ifdef WIN32
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					*pDest++ = 0;
#endif	// WIN32
#ifdef _Mac
					*pDest++ = pSrc[2];
					*pDest++ = pSrc[1];
					*pDest++ = *pSrc;
					*pDest++ = 0;
					pSrc += 3;
#endif	// _Mac
				}
			}
		}
		else {
			for (tint lY = 0; lY < lHeight; lY++) {
				tuchar* pSrc = &(pchBits[(lHeight - lY - 1) * iBytesPerRowSource]);
				tuchar* pDest = &(pchDest[lY * miBytesPerRow]);
				for (tint lX = 0; lX < lWidth; lX++) {
#ifdef WIN32
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					*pDest++ = 0;
#endif	// WIN32
#ifdef _Mac
					*pDest++ = pSrc[2];
					*pDest++ = pSrc[1];
					*pDest++ = *pSrc;
					*pDest++ = 0;
					pSrc += 3;
#endif	// _Mac
				}
			}
		}

		delete[] mpcData;
		mpcData = (tuchar*)pchDest;
	}
	if (miBitsPerPixel == 32) {
		// bgra
		tuchar* pchDest = new tuchar[lHeight * miBytesPerRow];
		if (pchDest == NULL) {
			return;
		}

		tuchar* pchBits = (tuchar*)(mpcData + sizeof(tga_header));
		if (bVertReversed == false) {
			memcpy(pchDest, pchBits, lHeight * miBytesPerRow);
		}
		else {
			for (tint lY = 0; lY < lHeight; lY++) {
				memcpy(&(pchDest[lY * miBytesPerRow]), &(pchBits[(lHeight - lY - 1) * miBytesPerRow]), miBytesPerRow);
			}
		}

		delete[] mpcData;
		mpcData = (tuchar*)pchDest;
	}
}

