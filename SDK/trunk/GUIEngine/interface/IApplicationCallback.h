/*!	\file IApplicationCallback.h
	\author Michael Olsen
	\date 10/March/2008
	\date 10/March/2008
*/

/*! \class IApplicationCallback
 * \brief
*/
class IApplicationCallback
{
public:
	//! Fires when user clicks menu item
	/*!
		\param pszString [in]: OS dependent content. Mac OS X: a zero-terminated string. Windows: points to a resource ID of type tint32 (note! value range only 1->65535) that uniquely identifies the menu entry
	*/
	virtual void OnMenuEvent(const tchar* pszString) = 0;

	virtual tbool OnInit(tint32 iIndex) = 0;

	//! Window size has changed
	/*!
		\param SizeNew [in]: New size
	*/
	virtual void SizeChanged(void* pWnd, SSize SizeNew) = 0;

	virtual void WindowClosed(void* pWnd) = 0;
};



