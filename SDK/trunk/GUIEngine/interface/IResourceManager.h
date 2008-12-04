/*!	\file IResourceManager.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class IResourceManager
 * \brief The resource manager keeps track of resources
 *
 * Only 1 resource manager exist per context.<br>Resources are automatically deleted when their reference count reaches 0
*/
class IResourceManager : public virtual IDestructable, public virtual IInContext, public virtual ISetContext
{
public:
	//! Create resource manager
	static IResourceManager* Create();

	//! Adds a resource file to the list of files<br>Remember to add all needed resources before doing anything else<br>File is automatically destroyed with resource manager
	/*!
		\param pFile [in]: Chunk file to add
	*/
	virtual void AddResourceFile(IChunkFile *pFile) = 0;

	//! Get bitmap resource with given ID, and if neccasary load it first
	/*!
		\param pWnd [in]: Window resource will work in (neccasary because of DirectX)
		\param iResID [in]: Resource ID of resource to load
		\param iNrOfFrames [in]: Number of frames in bitmap
		\param iNrOfVariants [in]: Number of variants in bitmap
		\return IBitmapResource: Pointer to bitmap resource, or NULL if not found
	*/
	virtual IBitmapResource* GetBitmap(IWindow* pWnd, tint32 iResID, int iNrOfFrames, int iNrOfVariants) = 0;

	//! Get font resource with given ID, and if neccasary load it first
	/*!
		\param pWnd [in]: Window resource will work in (neccasary because of DirectX)
		\param iResID [in]: Resource ID of resource to load
		\param iNrOfChars [in]: Number of charactes in font. Must be equal to or lower than giMaxNrOfCharsInFont
		\param piCharSizes [in]: Pointer to array of iNrOfChars size, with size for each character. Characters not present should be set to 0.
		\param TextColour [in]: Text colour for font
		\return IFontResource: Pointer to font resource, or NULL if not found
	*/
	virtual IFontResource* GetFont(IWindow* pWnd, tint32 iResID, tint iNrOfChars, const tint* piCharSizes, const SRGB& TextColour) = 0;

	//! Get a resource as raw binary data
	/*!
		\param iResID [in]: Resource ID of resource to load
		\return IResource*: Pointer to binary resource, or NULL if not found
	*/
	virtual IRawResource* GetRawResource(tint32 iResID) = 0;
};




