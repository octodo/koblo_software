

#include "deInternalOS.h"


CConverterEnum::CConverterEnum(tint32 iParmMax)
	: CConverter(0, iParmMax)
{
	mType = TypeEnum;
}

CConverterEnum::~CConverterEnum()
{
}

IConverter* IConverter::CreateEnum(tint32 iParmMax)
{
	return dynamic_cast<IConverter*>(new CConverterEnum(iParmMax));
}

void CConverterEnum::Destroy()
{
	delete dynamic_cast<CConverterEnum*>(this);
}

tfloat64 CConverterEnum::Parm2Normalised(tint32 iParm) const
{
	tint iNrOfSteps = miParmMax + 1;
	tfloat64 fIndex = iParm / (tfloat64)(iNrOfSteps - 1);

	return fIndex;
}

tint32 CConverterEnum::Normalised2Parm(tfloat64 fNormalised) const
{
	return Float2Int(fNormalised * miParmMax);
}

tint32 CConverterEnum::Parm2GUI(tint32 iParm) const
{
	tfloat64 fValue = Parm2Normalised(iParm);

	tint32 iMax = gControlMax - gControlMin;

	return Float2Int(fValue * iMax) + gControlMin;
}

tint32 CConverterEnum::GUI2Parm(tint32 iGUI) const
{
	tint32 iTmp = iGUI - gControlMin;
	tint32 iMax = gControlMax - gControlMin;
	tfloat64 fTmp = (tfloat64)iTmp / iMax;

	return Normalised2Parm(fTmp);
}



