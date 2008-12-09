

#include "KSOS.h"

CX3DisplayConverter::CX3DisplayConverter()
{
	mpConverter = de::IDisplayConverter::Create(1, 5, 1);
	mpConverter->SetPostFix("dB");
}

CX3DisplayConverter::~CX3DisplayConverter()
{
	mpConverter->Destroy();
}

void CX3DisplayConverter::Destroy()
{
	delete dynamic_cast<CX3DisplayConverter*>(this);
}

void CX3DisplayConverter::Value2String(tint32 iValue, tchar* psz)
{
	if (iValue == 0) {
		strcpy(psz, "-Inf.");
	}
	else {
		tfloat64 f = iValue / 10000.0;
		f = log10(f) * 20;
		f = f * 10;
		iValue = (tint32)(f + 0.5);

		mpConverter->Value2String(iValue, psz);
	}
}

tint32 CX3DisplayConverter::String2Value(const tchar* psz) throw (IException*)
{
	tint32 iRet = mpConverter->String2Value(psz);

	tfloat64 f = iRet / 10.0;
	f = pow(10, f / 20.0);
	f = f * 10000;
	iRet = (tint32)(f + 0.5);

	return iRet;
}

void CX3DisplayConverter::SetPostFix(const tchar* psz)
{
	mpConverter->SetPostFix(psz);
}

void CX3DisplayConverter::AddFixedString(tint32 iValue, const tchar* psz)
{
	mpConverter->AddFixedString(iValue, psz);
}
