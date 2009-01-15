
#include "VChorusOS.h"


CParmGlobal::CParmGlobal(CVChorusPlugIn* pPlugIn, tint32 iID, tint32 iMinValue, tint32 iMaxValue, tint32 iValue)
	: mpPlugIn(pPlugIn), miID(iID), miMinValue(iMinValue), miMaxValue(iMaxValue), miDefaultValue(iValue), miValue(iValue)
{
	// Force parameter within min/max range by calling SetValue(...)
	SetValue(miValue, false);
}

CParmGlobal::~CParmGlobal()
{
}

void CParmGlobal::Destroy()
{
	delete dynamic_cast<CParmGlobal*>(this);
}

tint32 CParmGlobal::GetID() const
{
	return miID;
}

tint32 CParmGlobal::GetDefaultValue() const
{
	return miDefaultValue;
}

tint32 CParmGlobal::GetValue() const
{
	return miValue;
}

tint32 CParmGlobal::GetMinValue() const
{
	return miMinValue;
}

tint32 CParmGlobal::GetMaxValue() const
{
	return miMaxValue;
}

void CParmGlobal::SetValue(tint32 iValueNew, tbool bCallOnChange)
{
	if (iValueNew < miMinValue) {
		iValueNew = miMinValue;
	}
	if (iValueNew > miMaxValue) {
		iValueNew = miMaxValue;
	}

	miValue = iValueNew;

	if (bCallOnChange) {
		OnChange();
	}
}

void CParmGlobal::OnChange()
{
	mpPlugIn->UpdateEngineData(miID, miValue);
}


