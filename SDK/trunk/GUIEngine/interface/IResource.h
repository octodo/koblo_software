/*!	\file IResource.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class IResource
 * \brief Resource, which can be loaded from resource file.
 *
 * Only use the resource manager to load resource files, never load them manually
*/
class IResource : public virtual IDestructable, public virtual IRefCountable
{
public:
	//! Resource type
	enum EResourceType {
		//! Unknown resource type. This is the default, and is used to identify problems in the code
		eResTypeUnknown = 0,
		//! Targe (.tga) resource type. This is used for bitmaps.
		eResTypeTga,
		//! Font (.fnt) resource type. This is used for fonts, and is actually a renamed targa (.tga)
		eResTypeFnt,
		//! Raw binary resource. Application must handle appropriately
		eResTypeRaw
	};

	//! Load resource from resource file. Note that the files current position must be the beginning of the resource data
	/*!
		\param pWnd [in]: Window resource will be used in (This is neccasary because of DirectX)
		\param pFile [in]: File to read resource from
		\param iSize [in]: Size of resource (in bytes).
		\param iResID [in]: ID of resource
	*/
	virtual void Load(IWindow* pWnd, IFile* pFile, tint iSize, tint32 iResID) = 0;

	//! Returns resource ID
	/*!
		\return tint32: Resource ID
	*/
	virtual tint32 GetResID() const = 0;

	//! Returns resource data
	/*!
		\return tuchar: Pointer to resource data
	*/
	virtual tuchar* GetData() = 0;

	//! Returns resource type
	/*!
		\return EResourceType: Resource type
	*/
	virtual EResourceType GetResType() const = 0;
};









