/*!	\file IUniqueID.h
	\author Michael Olsen
	\date 17/Jan/2005
	\date 17/Jan/2005
*/

/*! \class IUniqueID
 * \brief Interface for unique ID generator
 *
 * An unique ID generator can generate unique IDs of the type tuint32. When a previously used ID is not in use anymore the generator should be notified (ReleaseID()). The ID's are considered unique as far as the same ID is not returned twice from the same object (unless it has been released in between).
*/
class IUniqueID : public virtual IDestructable
{
public:
	//! Creates IUniqueID
	/*!
		uiMinValue [in]: Min value to generate as ID
	*/
	static IUniqueID* Create(tuint32 uiMinValue = 0);

	//! Gets new unique ID
	/*!
		\return tuint32: New unique ID
	*/
	virtual tuint32 GetID() = 0;

	//! Release previously retured ID
	/*
		\parm iID [in]: Previous unique ID, now being returned ("decommisioned")
	*/
	virtual void ReleaseID(tuint32 iID) = 0;
};




