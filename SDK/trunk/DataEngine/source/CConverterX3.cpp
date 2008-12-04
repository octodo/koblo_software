

#include "deInternalOS.h"


CConverterX3::CConverterX3(tint32 iParmMin, tint32 iParmMax, tuint uiParmDecimals)
	: CConverter(iParmMin, iParmMax)
{
	mType = TypeX3;

	mfDecimalMul = pow((tfloat64)10, (tint)uiParmDecimals);

	tfloat64 fVolumeMax = iParmMax / mfDecimalMul;
	mfReference = exp(log(1 / fVolumeMax) / 4);
}

CConverterX3::~CConverterX3()
{
}

IConverter* IConverter::CreateX3(tint32 iParmMin, tint32 iParmMax, tuint iStrength)
{
	return dynamic_cast<IConverter*>(new CConverterX3(iParmMin, iParmMax, iStrength));
}

void CConverterX3::Destroy()
{
	delete dynamic_cast<CConverterX3*>(this);
}

tfloat64 CConverterX3::Parm2Normalised(tint32 iParm) const
{
	tfloat64 fParm = iParm / mfDecimalMul;

	tfloat64 fRet;
	if (fParm < 1) {
		if (fParm == 0) {
			fRet = 0;
		}
		else {
			fRet = exp(log(fParm) / 3) * mfReference;
		}
	}
	else {
		fRet = exp(log(fParm) / 4) * mfReference;
	}

	return fRet;
}

tint32 CConverterX3::Normalised2Parm(tfloat64 fNormalised) const
{
	tfloat64 dRet;

	if (fNormalised < mfReference) {
		tfloat64 d = fNormalised / mfReference;
		dRet = d * d * d;
	}
	else {
		tfloat64 d = fNormalised / mfReference;
		dRet = d * d * d * d;
	}

	return (tint32)(dRet * mfDecimalMul);
}

tint32 CConverterX3::Parm2GUI(tint32 iParm) const
{
	tfloat64 fValue = Parm2Normalised(iParm);

	tint32 iMax = gControlMax - gControlMin;

	return Float2Int(fValue * iMax) + gControlMin;
}

tint32 CConverterX3::GUI2Parm(tint32 iGUI) const
{
	tint32 iTmp = iGUI - gControlMin;
	tint32 iMax = gControlMax - gControlMin;
	tfloat64 f = (tfloat64)iTmp / iMax;

	return Normalised2Parm(f);
}





