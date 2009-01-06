
#include "KSOS.h"


CParmGlobal::CParmGlobal( tint32 iID, tint32 iMinValue, tint32 iMaxValue, tint32 iValue, tint32 iSection)
	:  miID(iID), miMinValue(iMinValue), miMaxValue(iMaxValue), miDefaultValue(iValue), miValue(iValue), miSection(iSection)
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

	switch(miSection){
	
		
		case giSectionGUI:
			gpApplication->UpdateGUIData(miID, miValue);
			break;

		case giSectionGlobal:
			gpApplication->UpdateEngineData(miID, miValue);
			break;
			
		case giSection_Master:
			gpApplication->UpdateMasterData(miID, miValue);
			break;
	
		case giSection_AUX1:
			gpApplication->UpdateAUX1Data(miID, miValue);
			break;
	
		case giSection_AUX2:
			gpApplication->UpdateAUX2Data(miID, miValue);
			break;
			
		default:{
	
			if (miSection >= giSection_First_Track && miSection <= giSection_Last_Track)
			{
				gpApplication->UpdateTrackData(miID, miValue, miSection - giSection_First_Track);
			}
			else if (miSection >= giSection_First_Buss && miSection <= giSection_Last_Buss)
			{
				gpApplication->UpdateBussData(miID, miValue, miSection - giSection_First_Buss );
			}
			break;
		}
	}

}


