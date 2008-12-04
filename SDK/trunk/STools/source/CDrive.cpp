

#include "SToolsInternalOS.h"

IDrive* IDrive::Create()
{
	return dynamic_cast<IDrive*>(new CDrive());
}

void CDrive::Destroy()
{
	delete dynamic_cast<CDrive*>(this);
}

CDrive::CDrive()
{

}

CDrive::~CDrive()
{

}

void CDrive::ProcessHarch(tfloat32* pfData, tfloat32 pfDrive, tint lC)
{
	
	for (long l = 0; l < lC; l++) {
		
		tfloat32 fIn			=	pfData[l];
		
		tfloat32* fPtr		=	&fIn;
		tint32*	iPtr		=	(tint32*)fPtr;
		// Store sign
		tint32 iSignBuff	=	*iPtr & 0x80000000;
		// Clear sign
		*iPtr				&=	0x7FFFFFFF;
		// Drive
		fIn					=	((pfDrive+1.0f) * fIn) /(1.0f + pfDrive * fabs(fIn));
		// Set sign
		*iPtr				|=	iSignBuff;
		pfData[l]			= fIn;
	}
	
}

void CDrive::ProcessHarch(tfloat32* pfOut0,tfloat32*pfOut1, tfloat32 pfDrive, tint lC)
{
	ProcessHarch(pfOut0,  pfDrive, lC);
	ProcessHarch(pfOut1,  pfDrive, lC);
	
	
}

void CDrive::ProcessSoft(tfloat32* pfData, tfloat32 pfDrive, tint lC)
{
	
	for (long l = 0; l < lC; l++) {
		
		float fOut	=	pfData[l] * pfDrive;

		fOut		=	1.5f * fOut - 0.5f * fOut*fOut*fOut;
	//	fOut		=	1.5f * fOut - 0.5f * fOut*fOut*fOut;
	//	fOut		=	1.5f * fOut - 0.5f * fOut*fOut*fOut;
	
		pfData[l] = fOut;
	}	
}