/*!	\file CInContext.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "geInternalOS.h"


CInContext::CInContext() : mpContext(NULL)
{
}

CInContext::~CInContext()
{
}

ge::IContext* CInContext::GetContext()
{
	return mpContext;
}

const ge::IContext* CInContext::GetContext() const
{
	return mpContext;
}




