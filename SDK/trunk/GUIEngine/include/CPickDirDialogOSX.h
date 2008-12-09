/*!	\file CPickDirDialogOSX.h
	\author Michael Olsen
	\date 15/Jun/2005
	\date 15/Jun/2005
*/

/*! \class CPickDirDialogOSX
 * \brief OSX implementation of IPickDirDialog
*/
class CPickDirDialogOSX : public virtual IPickDirDialog
{
public:
	//! Constructor
	CPickDirDialogOSX();
	//! Destructor
	virtual ~CPickDirDialogOSX();

	//! IDestructable override
	virtual void Destroy();

	//! IPickDirDialog override
	virtual void DoDialog(tchar* pszPathOut, const tchar* pszPath);

protected:
};

