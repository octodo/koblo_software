

#include "SToolsInternalOS.h"

IClipper* IClipper::Create()
{
	return dynamic_cast<IClipper*>(new CClipper());
}

void CClipper::Destroy()
{
	delete dynamic_cast<CClipper*>(this);
}

CClipper::CClipper()
{
	
}

CClipper::~CClipper()
{
	
}

void CClipper::Process(tfloat fClip, tfloat32* pfLeft, tfloat32* pfRight, tint lC)
{
	
	
	for (long i = 0; i < lC; i++) {
		
		tfloat32 fInL	=	pfLeft[i];
		tfloat32 fInR	=	pfRight[i];
		
		// CLipper
		if(fInL > fClip)			fInL = fClip;
		else if(fInL < -fClip)		fInL = -fClip;
		
		if(fInR > fClip)			fInR = fClip;
		else if(fInR < -fClip)		fInR = -fClip;
		 // Copy to output
		pfLeft[i]	= fInL;
		pfRight[i]	= fInR;
		 
	}	
}