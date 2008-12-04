/*!	\file IURL.h
	\author Michael Olsen
	\date 19/Oct/2005
	\date 19/Oct/2005
*/

/*! \class IURL
 * \brief Invisible control, which opens browser with URL when clicked.
*/
class IURL : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IURL
	static IURL* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pszURL [in]: URL to use
	*/
	virtual void Init(tint32 iID, const tchar* pszURL) = 0;

	//! Changes the cursor to a pointing hand when the mouse is over the control
	virtual void ActivateHandCursor() = 0;
};