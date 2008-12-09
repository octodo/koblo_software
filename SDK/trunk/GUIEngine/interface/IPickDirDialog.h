/*!	\file IPickDirDialog.h
	\author Michael Olsen
	\date 15/Jun/2005
	\date 15/Jun/2005
*/

/*! \class IPickDirDialog.h
 * \brief Modal OS standard dialogue.
*/
class IPickDirDialog : public virtual IDestructable
{
public:
	//! Create IPickDirDialog
	static IPickDirDialog* Create();

	//! Sets the file type to save

	//! Creates and runs the dialog
	/*!
		\param pszPathOut [out]: Selected path, or "" if error (or cancel). Should be preallocated with minimum 512 characters
		\param pszPath [in]: Initial path
	*/
	virtual void DoDialog(tchar* pszPathOut, const tchar* pszPath) = 0;
};
