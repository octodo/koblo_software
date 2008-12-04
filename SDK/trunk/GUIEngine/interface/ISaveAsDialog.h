/*!	\file ISaveAsDialog.h
	\author Michael Olsen
	\date 05/Apr/2005
	\date 05/Apr/2005
*/

/*! \class ISaveAsDialog.h
 * \brief Modal OS standard dialogue.
*/
class ISaveAsDialog : public virtual IDestructable
{
public:
	//! Create ISaveAsDialog
	static ISaveAsDialog* Create();

	//! Sets the file type to save

	//! Select how the dialog handles bundle files (currently OS X only). Must be called before DoDialog. <br>If this method is not called, bundles are ignored per default
	/*!
	 \param iBundleBehaviour [in]: 0 => ignore bundles (they appear greyed in dialog), <br>1 => handle bundles as files (even though they really are folders), <br>2 => browse inside bundles (like it was a normal folder).
	 */
	virtual void SetBundleBehaviour(tint32 iBundleBehaviour) = 0;
	
	//! Creates and runs the dialog
	/*!
		\param pszPathNameOut [out]: Selected file name, or "" if error (or cancel). Should be preallocated with minimum 512 characters
		\param pszPathName [in]: Initial path name
		\param pszFileType [in]: Type of file (example: "*.k2spreset")
		\param pszFileTypeDescription [in]: Description of file type (example: "KeyToSound Presets (*.k2spreset)")
		\param pszDefaultName [in]: Default name of file, or NULL if none. If it contains a path too this will override pszPathName.
	*/
	virtual void DoDialog(tchar* pszPathNameOut, const tchar* pszPathName, const tchar* pszFileType, const tchar* pszFileTypeDescription, const tchar* pszDefaultName = NULL) = 0;
};
