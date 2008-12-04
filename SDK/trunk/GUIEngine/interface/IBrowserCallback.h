/*!	\file IBrowserCallback.h
	\author Michael Olsen
	\date 29/Mar/2005
	\date 29/Mar/2005
*/

/*! \class IBrowserCallback
 * \brief Callback used by IBrowser, to determine what items to show
 *
 *	Inherites from IDestructable, so that IBrowser can destroy it in its destructor
*/
class IBrowserCallback : public virtual IDestructable
{
public:
	//! Called to browse a folder
	/*!
		\param pszPath [in]: Path to folder to browse, starting from root. Each subdirectory is marked by a single backslash (\). Root folder is an empty string
		\param pItems [out]: Array to return items browsed
		\param iItemCount [in/out]: On input: Number of items allocated in pItems. On output: Number of items returned in pItems
	*/
	virtual void Browse(tchar* pszPath, IBrowser::SItem* pItems, tint32& iItemCount) = 0;
};
