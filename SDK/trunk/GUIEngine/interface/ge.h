/*!	\file ge.h
	\author Michael Olsen
	\date 09/Dec/2004
	\date 09/Dec/2004
*/

#ifndef _GE_INCLUDED
#define _GE_INCLUDED

// Needed by SRect
#if WIN32
struct tagRECT;
typedef tagRECT RECT;
#endif	// WIN32

// BaseUtilities
#include "bu.h"
// Data Engine
#include "de.h"

namespace k2s {

/*! \namespace
 * \brief Namespace for GUI engine
 *
 * This namespace is for the GUI engine, and misc. GUI related helper classes
*/
namespace ge {

// Forwards
class IWindow;

// Structs
#include "SPos.h"
#include "SSize.h"
#include "SRect.h"
#include "SRGB.h"
#include "SRGBA.h"
#include "SScrollPos.h"
#include "SEventGeneric.h"

// Enums
#include "EMouseMsg.h"

// Utilities
#include "geUtilities.h"

// Context
#include "IApplicationCallback.h"
#include "IContextCallback.h"
#include "IContext.h"
#include "IInContext.h"
#include "ISetContext.h"

// Events
#include "IEventReceiver.h"

// Resources
#include "IResource.h"
#include "IBitmapResource.h"
#include "IBitmapResourceText.h"
#include "IFontResource.h"
#include "IRawResource.h"
#include "IResourceManager.h"

// Window
#include "IWindow.h"

// Drawing primitives
#include "IDrawPrimitives.h"

// Controls
#include "IControl.h"
#include "IMouseTrap.h"
#include "ICustomControlCallback.h"
#include "ICustomControl.h"
#include "IURL.h"
#include "IBitmap.h"
#include "IText.h"
#include "ITextField.h"
#include "IButton.h"
#include "IMultiStateButton.h"
#include "IKnob.h"
#include "IKnob2.h"
#include "ISlider.h"
#include "IPopupMenu.h"
#include "ILine.h"
#include "ILED.h"
#include "IRect.h"
#include "IInvert.h"
#include "IBrowser.h"
#include "IBrowserCallback.h"
#include "IFFT.h"
#include "I2D.h"
#include "ITextList.h"
#include "IListBox.h"

// Panes
#include "IPane.h"
#include "IRadioButtons.h"
#include "IDropDownListBox.h"
#include "IScrollPane.h"
#include "IDialogCallback.h"
#include "IDialog.h"

// More controls
#include "IScrollBar.h"
#include "IEQDisplayCallback.h"
#include "IEQDisplay.h"
#include "ISpinner.h"
#include "IPeakMeter.h"
#include "IMultiStateDropDownListBox.h"
#include "IPaneSwitcher.h"
#include "IPanner.h"

// Utils
#include "IInvalidater.h"
#include "IWaitCursor.h"

// Standard OS dialogs
#include "ISaveAsDialog.h"
#include "IOpenDialog.h"
#include "IPickDirDialog.h"


}	// namespace ge

}	// namespace k2s

#endif	_GE_INCLUDED

