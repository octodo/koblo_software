/*!	\file CPeakMeter.cpp
	\author Lasse Steen Bohnstedt
	\date 19/Sep/2006
*/


#include "geInternalOS.h"


CPeakMeter::CPeakMeter()
{
	SetAutomaticResize(false);

	miSteps = 0;
	meDir = Up;
	mSizeOriginalOfLastBitmap = ge::SSize(0,0);
	
	//mfPeakLatest = 0.0;
	// Lasse, let's try this 2008-06-17
	mfPeakLatest = 1.0;
} // constructor


CPeakMeter::~CPeakMeter()
{
} // destructor


IPeakMeter* IPeakMeter::Create()
{
	return dynamic_cast<IPeakMeter*>(new CPeakMeter());
} // Create

void CPeakMeter::Destroy()
{
	delete dynamic_cast<CPeakMeter*>(this);
} // Destroy


void CPeakMeter::Init(tint32 iID, tint32 iSteps, ge::IPeakMeter::EDirection eDir /*= IPeakMeter::EDirection::Up*/, tint32 iSpacing0 /*= 0*/, tint32 iSpacing1 /*= 0*/)
{
	CControl::Init(iID);

	miSteps = iSteps;
	miSpacing0 = iSpacing0;
	miSpacing1 = iSpacing1;
	miPixelLines = -1;
	SetGrowingDirection(eDir);
} // Init(tint32, tint32, IPeakMeter::EDirection)


void CPeakMeter::SetGrowingDirection(ge::IPeakMeter::EDirection eDir /*= IPeakMeter::EDirection::Up*/)
{
	meDir = eDir;

	// Force a redraw (if possible)
	miValue = -1;
	SetValue(GetValue());
} // CPeakMeter::SetGrowingDirection(IPeakMeter::EDirection)


void CPeakMeter::SetFloatValue(tfloat fValue, tbool bNotify /*= true*/)
{
	int iRounded = Float2Int(fValue * miSteps);

	SetValue(iRounded, bNotify);
} // CPeakMeter::SetFloatValue(tfloat)


void CPeakMeter::SetFloatValueWithDecay(tfloat fValue, tfloat fDecayMul, tbool bNotify /*= true*/)
{
	mfPeakLatest *= fDecayMul;
	if (fValue > mfPeakLatest)
		mfPeakLatest = fValue;
	SetFloatValue(mfPeakLatest, bNotify);
} // CPeakMeter::SetFloatValueWithDecay(tfloat)


void CPeakMeter::SetValue(tint32 iNew, tbool bNotify /*= true*/)
{
	// Sanitize value
	if (iNew<0)
		iNew = 0;
	else if (iNew>=miSteps)
		iNew = miSteps;

	// Don't redraw if value hasn't changed
	if (iNew==GetValue())
		return;

	// Maybe redraw (if possible)
	if (mControls.size() > 0) {
		// Find last bitmap
		IControl* pBitmapLast = NULL;
		std::list<IControl*>::iterator it = mControls.end();
		it--;
		pBitmapLast = *it;

		// Lasse, added 2007-12-17
		tint32 iSpacing0Pct;
		tint32 iSpacing100Pct;
		// .. Lasse

		// Find logical size of index 1 bitmap (we don't know yet if it's width or height)
		tint32 iLogicalSize = 0;
		if ((meDir == Up)||(meDir == Left)) {
			// Growing Up or Left: background is on top and size should be inverted
			iLogicalSize = miSteps - iNew;

			// Lasse, added 2007-12-17
			// Invert spacing pixel lines as well
			iSpacing0Pct = miSpacing1;
			iSpacing100Pct = miSpacing0;
			// .. Lasse
		}
		else {
			// Growing Down or Right: foreground is on top and size is straight foreward
			iLogicalSize = iNew;

			// Lasse, added 2007-12-17
			// So is spacing pixel lines
			iSpacing0Pct = miSpacing0;
			iSpacing100Pct = miSpacing1;
			// .. Lasse
		}

		// Set size of last bitmap
		ge::SSize sizeNew = mSizeOriginalOfLastBitmap;
		if ((meDir == Up)||(meDir == Down)) {
			// Growing Up or Down: Manipulate Y-value of size (height)

			// Lasse, modified 2007-12-17
			//sizeNew.iCY = mSizeOriginalOfLastBitmap.iCY*iLogicalSize/miSteps;
			if (iLogicalSize == 0)
				sizeNew.iCY = 0;
			else if (iLogicalSize == miSteps)
				sizeNew.iCY = mSizeOriginalOfLastBitmap.iCY;
			else
				sizeNew.iCY = ((miPixelLines * iLogicalSize) / miSteps) + iSpacing0Pct;
			// .. Lasse
		}
		else {
			// Growing Left or Right: Manipulate X-value of size (width)

			// Lasse, modified 2007-12-17
			//sizeNew.iCX = mSizeOriginalOfLastBitmap.iCX*iLogicalSize/miSteps;
			if (iLogicalSize == 0)
				sizeNew.iCX = 0;
			else if (iLogicalSize == miSteps)
				sizeNew.iCX = mSizeOriginalOfLastBitmap.iCX;
			else
				sizeNew.iCX = ((miPixelLines * iLogicalSize) / miSteps) + iSpacing0Pct;
			// .. Lasse
		}
		pBitmapLast->SetSize(sizeNew);
	}


	// Save value
	if (mControls.size() > 0) {
		CControl::SetValue(iNew, bNotify);
	}
	else {
		miValue = iNew;
	}
} // CPeakMeter::SetValue(tint32, tbool)


void CPeakMeter::SetParentPane(IPane* pPane)
{
	CPane::SetParentPane(pPane);

	// Lasse: Inherited from CRadioButton.cpp
	// (mo) Hack. To automatically set the radio button size we set it to the same size as it's parent pane
	SSize Size;
	GetParentPane()->GetSize(Size);
	SetSize(Size);
}

void CPeakMeter::ControlUpdateAll()
{
	NotifyValueChange();
}

void CPeakMeter::AddControl(IControl* pToAdd, const SPos& Pos)
{
	// Lasse 2006-10-16: Why check for number of controls? Just accept anything
	//ASSERT(mControls.size()<2);
	// .. Lasse

	// Call inherited
	CPane::AddControl(pToAdd, Pos);

	// Save original size of last bitmap for later use
	pToAdd->GetSize(mSizeOriginalOfLastBitmap);

	// Lasse, added 2007-12-17
	// Calculate pixel lines that move with steps
	if ((meDir == Up)||(meDir == Down)) {
		// Growing Up or Down: Manipulate Y-value of size (height)
		miPixelLines = mSizeOriginalOfLastBitmap.iCY - (miSpacing0 + miSpacing1);
	}
	else {
		// Growing Left or Right: Manipulate X-value of size (width)
		miPixelLines = mSizeOriginalOfLastBitmap.iCX - (miSpacing0 + miSpacing1);
	}
	// .. Lasse

	// Lasse, removed 2007-12-17 - caused unexpected behaviour for more than one added bitmap
	/*
	// Make sure we show a correctly initialized peek meter
	SetValue(GetValue(), false);
	*/
	// .. Lasse

	// Add ourselves as listener
//	pToAdd->AddListener(dynamic_cast<IEventReceiver*>(this));
}

/*
void CPeakMeter::EventValueChange(IControl *pSender, tint32 iValueNew)
{
	// If the value is not 1 it means the button was not turned on, so we need to turn it back on
	if (iValueNew != 1) {
		pSender->SetValue(1);
	}
	else {
		// The value was 1, so the button was turned on.
		// Find the index, and set our value. This will turn off the other buttons
		tint iIndex = 0;
		std::list<IControl*>::iterator it = mControls.begin();
		while ((*it) != pSender) {
			iIndex++;
			it++;
		}

		tint32 iValue = GetValue();
		if (iIndex==0)
			iValue++;
		else
			iValue--;
		SetValue(iValue, true);
	}
}
*/
