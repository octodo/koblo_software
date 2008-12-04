

#include "deInternalOS.h"


CConverterLinear::CConverterLinear(tint32 iParmMin, tint32 iParmMax)
	: CConverter(iParmMin, iParmMax)
{
	mType = TypeLinear;
}

CConverterLinear::~CConverterLinear()
{
}

IConverter* IConverter::CreateLinear(tint32 iParmMin, tint32 iParmMax)
{
	return dynamic_cast<IConverter*>(new CConverterLinear(iParmMin, iParmMax));
}

void CConverterLinear::Destroy()
{
	delete dynamic_cast<CConverterLinear*>(this);
}

tfloat64 CConverterLinear::Parm2Normalised(tint32 iParm) const
{
	tfloat64 fParm = iParm;

	tfloat64 fTmp = fParm - miParmMin;
	tfloat64 fMax = miParmMax - miParmMin;
	tfloat64 fValue = fTmp / fMax;

	return fValue;
}

tint32 CConverterLinear::Normalised2Parm(tfloat64 fNormalised) const
{
	tfloat64 fMax = miParmMax - miParmMin;

	return Float2Int((fNormalised * fMax) + miParmMin);
}

tint32 CConverterLinear::Parm2GUI(tint32 iParm) const
{
	tfloat64 fValue = Parm2Normalised(iParm);

	tint32 iMax = gControlMax - gControlMin;

	return Float2Int(fValue * iMax) + gControlMin;
}

tint32 CConverterLinear::GUI2Parm(tint32 iGUI) const
{
	tint32 iTmp = iGUI - gControlMin;
	tint32 iMax = gControlMax - gControlMin;
	tfloat64 fTmp = (tfloat64)iTmp / iMax;

	return Normalised2Parm(fTmp);
}



