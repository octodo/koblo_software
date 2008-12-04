/*!	\file CContext.cpp
	\author Michael Olsen
	\date 09/Dec/2004
	\date 12/Jan/2005
*/


#include "geInternalOS.h"


CContext::CContext() : mpCallback(NULL), mpAppCallback(NULL)
{
}

CContext::~CContext()
{
}

void CContext::SetCallback(IContextCallback* pCallback)
{
	mpCallback = pCallback;
}

void CContext::SetApplicationCallback(IApplicationCallback* pCallback)
{
	mpAppCallback = pCallback;
}

IContextCallback* CContext::GetCallback()
{
	return mpCallback;
}

const IContextCallback* CContext::GetCallback() const
{
	return mpCallback;
}

