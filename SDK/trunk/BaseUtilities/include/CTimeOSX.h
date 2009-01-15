/*!	\file CTimeOSX.h
	\author Michael Olsen
	\date 06/May/2005
	\date 06/May/2005
*/


/*! \class CTimeOSX
 * \brief OSX implementation of ITime
*/
class CTimeOSX : public virtual ITime
{
public:
	//! Constructor
	CTimeOSX();
	//! Destructor
	virtual ~CTimeOSX();

	//! IDestructable override
	virtual void Destroy();

protected:
};






