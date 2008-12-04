

#include "deInternalOS.h"


CConverterLog::CConverterLog(tint32 iParmMin, tint32 iParmMax, tuint iStrength)
	: CConverter(iParmMin, iParmMax), miStrength(iStrength)
{
	mType = TypeLog;
}

CConverterLog::~CConverterLog()
{
}

IConverter* IConverter::CreateLog(tint32 iParmMin, tint32 iParmMax, tuint iStrength)
{
	return dynamic_cast<IConverter*>(new CConverterLog(iParmMin, iParmMax, iStrength));
}

void CConverterLog::Destroy()
{
	delete dynamic_cast<CConverterLog*>(this);
}

tfloat64 CConverterLog::Parm2Normalised(tint32 iParm) const
{
	tfloat64 fParm = iParm;

	tfloat64 fTmp = fParm - miParmMin;
	tfloat64 fMax = miParmMax - miParmMin;
	tfloat64 fValue = fTmp / fMax;

	return log10((tfloat64)((fValue * (miStrength - 1)) + 1)) / log10((tfloat64)miStrength);
}

tint32 CConverterLog::Normalised2Parm(tfloat64 fNormalised) const
{
	tfloat64 f = fNormalised;

	tfloat64 fTmp = (pow(10.0, f * log10((tfloat64)miStrength)) - 1) / (miStrength - 1);
	if (fTmp > 0.999999) fTmp = 1.0;

	tfloat64 fMax = miParmMax - miParmMin;

	return Float2Int((fTmp * fMax) + miParmMin);
}

tint32 CConverterLog::Parm2GUI(tint32 iParm) const
{
	tfloat64 fValue = Parm2Normalised(iParm);

	tint32 iMax = gControlMax - gControlMin;

	return Float2Int(fValue * iMax) + gControlMin;
}

tint32 CConverterLog::GUI2Parm(tint32 iGUI) const
{
	tint32 iTmp = iGUI - gControlMin;
	tint32 iMax = gControlMax - gControlMin;
	tfloat64 f = (tfloat64)iTmp / iMax;

	return Normalised2Parm(f);
}





