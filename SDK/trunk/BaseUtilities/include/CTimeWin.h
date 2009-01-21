/*!	\file CTimeWin.h
	\author Michael Olsen
	\date 06/May/2005
	\date 06/May/2005
*/


/*! \class CTimeWin
 * \brief Win32 implementation of ITime
*/
class CTimeWin : public virtual ITime
{
public:
	//! Constructor
	CTimeWin();
	//! Destructor
	virtual ~CTimeWin();

	//! IDestructable override
	virtual void Destroy();

protected:
};






