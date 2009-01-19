/*!	\file CFileCP.h
	\author Michael Olsen
	\date 22/Apr/2005
	\date 22/Apr/2005
*/


/*! \class CFileCP
 * \brief Implementation of crossplatform part of IFile (ReadCP / WriteCP)
*/
class CFileCP : public virtual IFile
{
public:
	//! Constructor
	CFileCP();
	//! Destructor
	virtual ~CFileCP();

	//! IFile override
	virtual tuint64 ReadCP(tint32* p, tuint64 iSize);
	//! IFile override
	virtual tuint64 WriteCP(tint32* p, tuint64 iSize);
	//! IFile override
	virtual tuint64 ReadCP(tfloat32* p, tuint64 iSize);
	//! IFile override
	virtual tuint64 WriteCP(tfloat32* p, tuint64 iSize);

protected:
};





