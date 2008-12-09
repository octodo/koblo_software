

#include "deInternalOS.h"


CConverterTrueLog::CConverterTrueLog(tint32 iParmMin, tint32 iParmMax)
	: CConverter(iParmMin, iParmMax)
{
	mType = TypeTrueLog;
}

CConverterTrueLog::~CConverterTrueLog()
{
}

IConverter* IConverter::CreateTrueLog(tint32 iParmMin, tint32 iParmMax)
{
	return dynamic_cast<IConverter*>(new CConverterTrueLog(iParmMin, iParmMax));
}

void CConverterTrueLog::Destroy()
{
	delete dynamic_cast<CConverterTrueLog*>(this);
}

tfloat64 CConverterTrueLog::Parm2Normalised(tint32 iParm) const
{
	return log(iParm / (tfloat64)miParmMin) / log((tfloat)miParmMax / miParmMin);
}

tint32 CConverterTrueLog::Normalised2Parm(tfloat64 fNormalised) const
{
	return Float2Int(exp(fNormalised * log((tfloat64)miParmMax / miParmMin)) * miParmMin);
}

tint32 CConverterTrueLog::Parm2GUI(tint32 iParm) const
{
	tfloat64 fValue = Parm2Normalised(iParm);

	tint32 iMax = gControlMax - gControlMin;

	return Float2Int(fValue * iMax) + gControlMin;
}

tint32 CConverterTrueLog::GUI2Parm(tint32 iGUI) const
{
	tint32 iTmp = iGUI - gControlMin;
	tint32 iMax = gControlMax - gControlMin;
	tfloat64 f = (tfloat64)iTmp / iMax;

	return Normalised2Parm(f);
}





