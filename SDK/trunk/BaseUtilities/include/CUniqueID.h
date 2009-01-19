/*!	\file CUniqueID.h
	\author Michael Olsen
	\date 17/Jan/2005
	\date 17/Jan/2005
*/


/*! \class CUniqueID
 * \brief Implementation of IUniqueID
*/
class CUniqueID : public virtual IUniqueID
{
public:
	//! Constructor
	CUniqueID(tuint32 uiMinValue = 0);
	//! Destructor
	virtual ~CUniqueID();

	//! IDestructable override
	virtual void Destroy();

	//! IUniqueID override
	virtual tuint32 GetID();
	//! IUniqueID override
	virtual void ReleaseID(tuint32 iID);

protected:
	//! Min value to use as ID
	tuint32 muiMinValue;

	//! Map of ID's in use. The first value is used to find our value, the second is a dummy (not used). I.e. if we can locate a map entry based on an ID that means we've used the ID, otherwise we havn't.
	std::map<tuint32, tuint32> mIDMap;
};






