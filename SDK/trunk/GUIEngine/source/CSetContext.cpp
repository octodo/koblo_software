/*!	\file CSetContext.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "geInternalOS.h"


CSetContext::CSetContext()
{
}

CSetContext::~CSetContext()
{
}

void CSetContext::SetContext(ge::IContext* pContext)
{
	mpContext = pContext;
}



