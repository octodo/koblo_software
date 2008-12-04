/*!	\file CWindow.cpp
	\author Michael Olsen
	\date 22/Dec/2005
	\date 22/Dec/2005
*/


#include "dspeInternalOS.h"


CWindow::CWindow()
{
}

CWindow::~CWindow()
{
}

IWindow* IWindow::Create()
{
	return dynamic_cast<IWindow*>(new CWindow());
}

void CWindow::Destroy()
{
	delete dynamic_cast<CWindow*>(this);
}

void CWindow::Init(EWindow Window, tfloat32 fData, tuint32 uiSize)
{
	muiDataSize = uiSize;

	mpfData.Allocate(muiDataSize);

	switch(Window) {
		case WindowBlackmanHarris:
			{
				tfloat64 fTheta = 0.0;
				tfloat64 fThetaInc = (2.0 * fPi) / uiSize;
				tuint32 ui;
				for (ui = 0; ui < uiSize; ui++, fTheta += fThetaInc) {
					mpfData[ui] = (tfloat32)(0.35875 - (0.48829 * cos(fTheta)) + (0.14128 * cos(2.0 * fTheta)) - (0.01168 * cos(3.0 * fTheta)));
				}
			}
			break;

		case WindowHann:
			{
				tfloat64 fTheta = 0.0;
				tfloat64 fThetaInc = (2.0 * fPi) / uiSize;
				tuint32 ui;
				for (ui = 0; ui < uiSize; ui++, fTheta += fThetaInc) {
					mpfData[ui] = (tfloat32)(0.5 * (1.0 - cos(fTheta)));
				}
			}
			break;
	}
}

void CWindow::Apply(tfloat32* pfSamples)
{
	tfloat32* pfData = mpfData.Get();

	for (tuint32 ui = 0; ui < muiDataSize; ui++) {
		pfSamples[ui] *= pfData[ui];
	}
}





