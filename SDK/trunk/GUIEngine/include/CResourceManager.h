// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.


/*! \class CResourceManager
 * \brief Implementation of IResourceManager
*/
class CResourceManager : public virtual IResourceManager, public virtual CInContext, public virtual CSetContext
{
public:
	//! Constructor
	CResourceManager();
	//! Destructor
	virtual ~CResourceManager();

	//! IDestructable override
	virtual void Destroy();

	//! IResourceManager override
	virtual void AddResourceFile(IChunkFile *pFile);
	//! IResourceManager override
	virtual IBitmapResource* GetBitmap(IWindow* pWnd, tint32 iResID, tint iNrOfFrames, tint iNrOfVariants);
	//! IResourceManager override
	virtual IFontResource* GetFont(IWindow* pWnd, tint32 iResID, tint iNrOfChars, const tint* piCharSizes, const SRGB& TextColour);
	//! IResourceManager override
	virtual IRawResource* GetRawResource(tint32 iResID);

protected:
	#if WIN32
	#pragma pack(push, 1)
	#endif

	/*! \class SResourceInfo
	 * \brief Struct with info data about 1 resource. Used to keep a list of available resources
	 */
	struct SResourceInfo {
		//! ID
		tint32 iID;
		//! Type
		tint32 iType;
		//! Offset into file (bytes)
		tint32 iFileOffset;
		//! Size in file (bytes)
		tint32 iSize;
		//! Pointer to file. Note, this must come last in struct, because the above values are read from chunk
		IFile* pFile;
	};

	#if WIN32
	#pragma pack(pop)
	#endif

	//! List of available resources
	std::list<SResourceInfo> mIndexHeaders;
	//! List of loaded resources
	std::list<IResource*> mResources;

	//! Call to find resource with given ID
	/*!
		\param iResID [in]: Resource ID
		\return IResource: Pointer to resource, or NULL if not found
	*/
	IResource* FindResource(tint32 iResID);

	//! Call to find resource with given ID and RGB (used to find font with same ID and colour)
	/*!
		\param iResID [in]: Resource ID
		\param RGB [in]: Resource colour
		\return IResource: Pointer to resource, or NULL if not found
	*/
	IResource* FindResource(tint32 iResID, const SRGB& RGB);
};

