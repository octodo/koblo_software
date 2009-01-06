


#include "KSOS.h"


CKSBaseGUI::CKSBaseGUI(CBasePlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CBaseGUI(pPlugIn, pParmMan)
{

}

tbool CKSBaseGUI::OnKeyDown(ge::EKey Key)
{
	return dynamic_cast<CKSPlugIn*>(gpApplication)->OnKeyDown(Key);
}









