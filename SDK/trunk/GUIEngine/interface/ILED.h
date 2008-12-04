/*!	\file ILED.h
	\author Max Grønlund
	\date 17/July/2005
	\date 17/July/2005
*/

/*! \class ILED
 * \brief An interface for a generic LED,
*/
class ILED : public virtual IControl, public virtual IDestructable
{
public:
	//! Create ILED
	static ILED* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param iMin [in]: Minimum vslue
		\param iMax [in]: Maximum value
		\param Size [in]: Size off control
		\param pParentWindow [in]: Window to draw in
		\param RGB [in]: RGB Color
	*/
	virtual void Init(tint32 iID, tint32 iMin, tint32 iMax, SSize Size,IWindow* pParentWindow, SRGB RGB) = 0;

	//! Set
	/*!
		\param iValue [in]: Value to draw

	*/
	virtual void Set(tint32 iValue) = 0;
};