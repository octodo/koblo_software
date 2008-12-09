

#include "SToolsInternalOS.h"

ISoftDistortion* ISoftDistortion::Create()
{
	return dynamic_cast<ISoftDistortion*>(new CSoftDistortion());
}

void CSoftDistortion::Destroy()
{
	delete dynamic_cast<CSoftDistortion*>(this);
}

CSoftDistortion::CSoftDistortion()
{

}

CSoftDistortion::~CSoftDistortion()
{

}

void CSoftDistortion::Process(tfloat32* pfLeft, tfloat32* pfRight, tint lC)
{
	
	
	for (long i = 0; i < lC; i++) {
		tfloat32 fInL	=	pfLeft[i];
		tfloat32 fInR	=	pfRight[i];
		pfLeft[i]		=	1.5f * fInL - 0.5f * fInL * fInL * fInL;
		pfRight[i]		=	1.5f * fInR - 0.5f * fInR * fInR * fInR;
	}	
}