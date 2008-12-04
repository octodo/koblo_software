/*!	\file CDialog.cpp
	\author Michael Olsen
	\date 05/Apr/2005
	\date 05/Apr/2005
*/


#include "geInternalOS.h"


CDialog::CDialog()
	: mpCallback(NULL)
{
	SetAutomaticResize(false);
}

CDialog::~CDialog()
{
	if (mpCallback) {
		mpCallback->Destroy();
	}
}

IDialog* IDialog::Create()
{
	return dynamic_cast<IDialog*>(new CDialog());
}

void CDialog::Destroy()
{
	delete dynamic_cast<CDialog*>(this);
}

void CDialog::Init(tint32 iID)
{
	CPane::Init(iID);

	SetVisible(false);
}

void CDialog::SetCallback(IDialogCallback* pCallback)
{
	// If there's a previous callback we have a memory leak
	ASSERT(mpCallback == NULL);

	mpCallback = pCallback;
}

IDialogCallback* CDialog::GetCallback()
{
	return mpCallback;
}

const IDialogCallback* CDialog::GetCallback() const
{
	return mpCallback;
}

void CDialog::AddControl(IControl* pToAdd, const SPos& Pos, EButtonType ButtonType)
{
	// Call inherited
	CPane::AddControl(pToAdd, Pos);

	if (ButtonType == ButtonTypeClose) {
		pToAdd->AddListener(dynamic_cast<IEventReceiver*>(this));

		miCloseIDs.push_back(pToAdd->GetID());
	}
}

void CDialog::EventValueChange(IControl *pSender, tint32 /*iValueNew*/)
{
	tint32 iID = pSender->GetID();

	std::list<tint32>::const_iterator it = miCloseIDs.begin();
	for (; it != miCloseIDs.end(); it++) {
		if ((*it) == iID) {
			// We found one of the close ids, so close dialog
			Hide(iID);
			break;
		}
	}
}

void CDialog::Show()
{
	dynamic_cast<CWindow*>(GetParentWindow())->GetMouseFocus(dynamic_cast<IControl*>(this), false);

	SetVisible(true);
}

void CDialog::Hide(tint32 iID)
{
	SetVisible(false);

	dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();

	if (mpCallback) {
		mpCallback->OnClose(dynamic_cast<IDialog*>(this), iID);
	}
}

