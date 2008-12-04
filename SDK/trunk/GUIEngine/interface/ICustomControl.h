/*!	\file ICustomControl.h
	\author Michael Olsen
	\date 18/Mar/2005
	\date 18/Mar/2005
*/

/*! \class ICustomControl
 * \brief This interface is used when a user of the GUIEngine wants to make a custom control
*/
class ICustomControl : public virtual IControl
{
public:
	//! Create ICustomControl
	static ICustomControl* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pCallback [in]: Callback that will receive calls for events
		\param pBitmap [in]: Bitmap resource to use, or NULL. A non-NULL bitmap will be managed by the control and thus DecRefCount will automatically be called at destruction time
	*/
	virtual void Init(tint32 iID, ICustomControlCallback* pCallback, IBitmapResource* pBitmap = NULL) = 0;
};
