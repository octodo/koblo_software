/*!	\file CEQDisplay.cpp
	\author Michael Olsen
	\date 15/Dec/2005
	\date 15/Dec/2005
*/


#include "geInternalOS.h"


CEQDisplay::CEQDisplay()
	: muiNrOfBands(0), mpCallback(NULL), mpfBufferCurveBand(NULL), mpfBufferCurveTotal(NULL),
	mpDrawPrimitives(NULL),
	mfdBMin(-24.0f), mfdBMax(12.0f), miControlIDDot1(-1), miControlIDDotSpan(-1)
{
	SetAutomaticResize(false);
}

CEQDisplay::~CEQDisplay()
{
	if (mpDrawPrimitives) {
		mpDrawPrimitives->Destroy();
	}

	if (mpfBufferCurveBand) {
		delete[] mpfBufferCurveBand;
	}
	if (mpfBufferCurveTotal) {
		delete[] mpfBufferCurveTotal;
	}
}

IEQDisplay* IEQDisplay::Create()
{
	return dynamic_cast<IEQDisplay*>(new CEQDisplay());
}

void CEQDisplay::Destroy()
{
	delete dynamic_cast<CEQDisplay*>(this);
}

void CEQDisplay::Init(tint iID, IWindow* pWindow, IResourceManager* pResMan, tint32 iDotResID, tint32 iControlIDDot1, tint32 iControlIDDotSpan, tint iDotOffsetX, tint iDotOffsetY, tuint uiNrOfBands, SRGB Colour)
{
	CControl::Init(iID);

	miControlIDDot1 = iControlIDDot1;
	miControlIDDotSpan = iControlIDDotSpan;

	miDotOffsetX = iDotOffsetX;
	miDotOffsetY = iDotOffsetY;

	muiNrOfBands = uiNrOfBands;
	if (muiNrOfBands) {
		tuint uiBand;
		for (uiBand = 0; uiBand < muiNrOfBands; uiBand++) {
			I2D* p2D = I2D::Create();
			// Lasse: *Exterminated*!
			//p2D->Init(guiDotID1 + uiBand, pResMan->GetBitmap(pWindow, iDotResID + uiBand, 1, 1));
			p2D->Init(iControlIDDot1 + uiBand*iControlIDDotSpan, pResMan->GetBitmap(pWindow, iDotResID + uiBand, 1, 1));
			// .. Lasse
			p2D->AddListener(dynamic_cast<IEventReceiver*>(this));

			AddControl(dynamic_cast<IControl*>(p2D), SPos(-miDotOffsetX, -miDotOffsetY));
		}
	}

	mpDrawPrimitives = IDrawPrimitives::Create();
	mpDrawPrimitives->Init(pWindow);

	mColour = Colour;
}

void CEQDisplay::Init2(tint iID, IWindow* pWindow, tuint uiNrOfBands, SRGB Colour)
{
	CControl::Init(iID);

	muiNrOfBands = uiNrOfBands;

	mpDrawPrimitives = IDrawPrimitives::Create();
	mpDrawPrimitives->Init(pWindow);

	mColour = Colour;
}

void CEQDisplay::SetCallback(IEQDisplayCallback* pCallback, tint32 iCallbackUserDefined)
{
	mpCallback = pCallback;
	miCallbackUserDefined = iCallbackUserDefined;
}

void CEQDisplay::SetdBMinAndMax(tfloat fdBMin, tfloat fdBMax)
{
	mfdBMin = fdBMin;
	mfdBMax = fdBMax;

	CControl::Redraw();
}

void CEQDisplay::SetSize(const SSize& Size, tbool bRedraw)
{
	CPane::SetSize(Size, bRedraw);

	if (mpfBufferCurveBand) {
		delete[] mpfBufferCurveBand;
	}
	mpfBufferCurveBand = new tfloat32[GetSize().iCX];
	if (mpfBufferCurveTotal) {
		delete[] mpfBufferCurveTotal;
	}
	mpfBufferCurveTotal = new tfloat32[GetSize().iCX];

	std::list<IControl*>::iterator it = mControls.begin();
	for (; it != mControls.end(); it++) {
		IControl* pControl = *it;
		pControl->SetSize(GetSize() + SSize(miDotOffsetX * 2, miDotOffsetY * 2));
	}
}

void CEQDisplay::SetBandInfo(tuint uiBand, tbool bEnabled, tfloat32 fFreqIndex, tfloat32 fdBIndex)
{
	tfloat32 fXIndex = fFreqIndex;
	tfloat32 fYIndex = 1 - fdBIndex;

	tint32 iXPositions = dynamic_cast<I2D*>(*(mControls.begin()))->GetXPositions();
	tint32 iYPositions = dynamic_cast<I2D*>(*(mControls.begin()))->GetYPositions();

	tint32 iXPos = Float2Int(fXIndex * (iXPositions - 1));
	tint32 iYPos = Float2Int(fYIndex * (iYPositions - 1));

	tint32 iValue = iXPos << 16 | iYPos;

	// Get the control
	std::list<IControl*>::iterator it = mControls.begin();
	tuint uiIndex = uiBand;
	while (uiIndex) {
		it++;
		uiIndex--;
	}
	IControl* pControl = *it;

	pControl->SetValue(iValue, false);
	pControl->SetVisible(bEnabled);
}

void CEQDisplay::Update()
{
	CControl::Redraw();
}

void CEQDisplay::EventValueChange(IControl* pSender, tint32 iValueNew)
{
	I2D* p2D = dynamic_cast<I2D*>(pSender);
	if (p2D) {
		// Sender is a 2D
		tuint uiBand = (pSender->GetID() - miControlIDDot1) / miControlIDDotSpan;

		tint32 iValue = pSender->GetValue();
		tint32 iXPos = iValue >> 16;
		tint32 iYPos = iValue & 0xffff;

		tint32 iXPositions = dynamic_cast<I2D*>(*(mControls.begin()))->GetXPositions();
		tint32 iYPositions = dynamic_cast<I2D*>(*(mControls.begin()))->GetYPositions();

		tfloat32 fXIndex = (tfloat32)iXPos / (iXPositions - 1);
		tfloat32 fYIndex = (tfloat32)iYPos / (iYPositions - 1);
		fYIndex = 1 - fYIndex;

		tint32 piData[4];
		piData[0] = 0;
		piData[1] = uiBand;
		((tfloat32*)piData)[2] = fXIndex;
		((tfloat32*)piData)[3] = fYIndex;
		NotifyGeneric((void*)piData);

		CControl::Redraw();
	}
}

void CEQDisplay::EventGeneric(IControl* pSender, void* pEventData)
{
	I2D* p2D = dynamic_cast<I2D*>(pSender);
	if (p2D) {
		// Sender is a 2D
		tuint uiBand = (pSender->GetID() - miControlIDDot1) / miControlIDDotSpan;

		tint32* piMessage = (tint32*)pEventData;
		if (piMessage[0] == 0) {
			tint32 iDiff = piMessage[1];

			tint32 piData[3];
			piData[0] = 1;
			piData[1] = uiBand;
			piData[2] = iDiff;
			NotifyGeneric((void*)piData);
		}
	}
}

void CEQDisplay::OnDraw(const SRect &rUpdate)
{
	ASSERT(mpCallback != NULL);

	memset(mpfBufferCurveTotal, 0, GetSize().iCX * sizeof(tfloat32));
	tuint uiBand;
	for (uiBand = 0; uiBand < muiNrOfBands; uiBand++) {
		mpCallback->GetCurve(miCallbackUserDefined, uiBand, mpfBufferCurveBand, GetSize().iCX);

		tint iIndex;
		for (iIndex = 0; iIndex < GetSize().iCX; iIndex++) {
			mpfBufferCurveTotal[iIndex] += mpfBufferCurveBand[iIndex];
		}
	}

//	const tfloat32 fdBMax = 12.0f;
//	const tfloat32 fdBMin = -24.0f;

	tint iIndex;
	for (iIndex = 0; iIndex < GetSize().iCX; iIndex++) {
		tfloat32 fdB = mpfBufferCurveTotal[iIndex];
		//fdB = min(fdB, mfdBMax);
		//fdB = max(fdB, mfdBMin);
		if ((fdB>=mfdBMin)&&(fdB<=mfdBMax)) {
			tfloat32 fIndex = (fdB - mfdBMin) / (mfdBMax - mfdBMin);
			tint32 iYPos = Float2Int((GetSize().iCY - 1) * (1 - fIndex));

			SPos Pos(GetPos() + SPos(iIndex, iYPos));

			mpDrawPrimitives->DrawPoint(rUpdate, Pos, mColour);
		}
	}

	CPane::OnDraw(rUpdate);
}


