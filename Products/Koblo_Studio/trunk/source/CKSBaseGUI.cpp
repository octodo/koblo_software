


#include "KSOS.h"


CKSBaseGUI::CKSBaseGUI( de::IParameterManager* pParmMan)
	: CBaseGUI(gpApplication, pParmMan)
{

}

tbool CKSBaseGUI::OnKeyDown(ge::EKey Key)
{
	gpApplication->OnKeyDown(Key);
}









