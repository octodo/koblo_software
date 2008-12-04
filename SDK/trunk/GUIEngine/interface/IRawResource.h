/*!	\file IRawResource.h
	\author Lasse Steen Bohnstedt
	\date 2008-06-09
*/

/*! \class IRawResource
 * \brief Interface for raw binary resource data
*/
class IRawResource : public virtual IResource
{
public:
	//! Create IRawResource
	static IRawResource* Create();

	//! Returns size of raw data (only interesting for binary data)
	/*!
		\return tint32: Size in bytes of raw resource data
	*/
	virtual tint32 GetDataSize() = 0;

	//! Load resource from resource file. Note that the files current position must be the beginning of the resource data
	/*!
		\param pWnd [in]: Window resource will be used in (This is neccasary because of DirectX)
		\param pFile [in]: File to read resource from
		\param iSize [in]: Size of resource (in bytes).
		\param iResID [in]: ID of resource
	*/
	virtual void Load(IWindow* pWnd, IFile* pFile, tint iSize, tint32 iResID) = 0;
};

