// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.


#include "ge.h"

#include <Windows.h>

#ifdef _DXGUI
#include <ddrawex.h>
#include <windowsx.h>
#include <d3d9.h>
#endif

#ifdef WIN32
// Inheritance via dominance
#pragma warning(disable: 4250)
// Identifier truncated to 255 characters
#pragma warning(disable: 4786)
#endif

// DSP Engine
#include "dspe.h"

namespace k2s {

namespace ge {

// Context
#include "CContext.h"
#ifdef WIN32
	#include "CContextWin.h"
#endif	// WIN32
#ifdef _Mac
	#include "CContextOSX.h"
#endif
#include "CInContext.h"
#include "CSetContext.h"

// Interface utilities
#include "CEventReceiver.h"

// Device contexts
#include "IDC.h"
#include "CMemoryDC.h"
//#include "IDCFont.h"
#ifdef WIN32
	#ifdef _DXGUI
	#else	// _DXGUI
		#include "CMemoryDCGDI.h"
	#endif	// _DXGUI else
#endif	// WIN32
#ifdef _Mac
	#include "CMemoryDCQT.h"
#endif	// _Mac

// Resources
#include "CResource.h"
#include "CBitmapResource.h"
//#include "CBitmapResourceText.h"
#include "CFontResource.h"
#include "CRawResource.h"
#include "CResourceManager.h"

// System specific resources
#ifdef _DXGUI
//#include "CBitmapResourceDX.h"
//#include "CBitmapResourceDXText.h"
//#include "CFontResourceDX.h"
#else	// _DXGUI
//#include "CBitmapResourceGDI.h"
//#include "CBitmapResourceTextGDI.h"
//#include "CFontResourceGDI.h"
#endif	// _DXGUI else

// Window
#include "CWindow.h"
#ifdef WIN32
	#include "CWindowWin.h"
	#ifdef _DXGUI
		#include "CWindowWinDX.h"
	#else	// _DXGUI
		#include "CWindowWinGDI.h"
	#endif	// _DXGUI else
#endif	// WIN32
#ifdef _Mac
	#include "CWindowOSX.h"
	#include "CWindowOSXQT.h"
#endif	// _Mac

// Drawing primitives
#ifdef WIN32
#include "CDrawPrimitivesGDI.h"
#else	// WIN32
#include "CDrawPrimitivesQT.h"
#endif	// WIN32

// Controls
#include "CControl.h"
#include "CCustomControl.h"
#include "CBitmap.h"
#include "CURL.h"
#include "CKnob.h"
#include "CKnob2.h"
#include "CSlider.h"
#include "CText.h"
#include "CTextField.h"
#include "CLine.h"
#include "CLED.h"
#include "CRect.h"
#include "CInvert.h"
#include "CBrowser.h"
#include "CFFT.h"
#include "C2D.h"
#include "CTextList.h"
#include "CListBox.h"
#include "CMouseTrap.h"

// Panes
#include "CPane.h"
#include "CRadioButtons.h"
#include "CDropDownListBox.h"
#include "CDialog.h"

// More controls
#include "CButton.h"
#include "CMultiStateButton.h"
#include "CPopupMenu.h"
#include "CScrollPane.h"
#include "CScrollBar.h"
#include "CEQDisplay.h"
#include "CSpinner.h"
#include "CPeakMeter.h"
#include "CMultiStateDropDownListBox.h"
#include "CPaneSwitcher.h"
#include "CPanner.h"

// Utils
#include "CInvalidater.h"
#ifdef WIN32
#include "CWaitCursorWin.h"
#else
#include "CWaitCursorOSX.h"
#endif

// Standard OS dialogs
#ifdef WIN32
#include "CSaveAsDialogWin.h"
#include "COpenDialogWin.h"
#include "CPickDirDialogWin.h"
#else
#include "CSaveAsDialogOSX.h"
#include "COpenDialogOSX.h"
#include "CPickDirDialogOSX.h"
#endif


}	// namespace ge

}	// namespace k2s


using namespace k2s;

using namespace ge;
