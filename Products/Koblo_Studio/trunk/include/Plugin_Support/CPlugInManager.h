
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
// along with the Koblo SDK. If not, see <http://www.gnu.org/licenses/>.


class CKSPlugInGUI;

class CPlugInManager : public virtual kspi::IHost
{
public:
	CPlugInManager();
	
	virtual ~CPlugInManager();
	
	//! IHost override
	virtual void KSPI_CALL OnAutomationUpdate(kspi::IPlugIn* pCaller, tint32 iParameterIndex, tint32 iValue) {}
	//! IHost override
	virtual tfloat32 KSPI_CALL GetCurrentBPM() {return 0.0f;}
	//! IHost override
	virtual kspi::IHost::EType KSPI_CALL GetType() {return kspi::IHost::HostTypeStandAlone;}
	//! IHost override
	virtual void KSPI_CALL ActivateWindow(tint32 iIndex) {}
	//! IHost override
	virtual void KSPI_CALL HideWindow(tint32 iIndex) {}
	//! IHost override
	virtual tint32 KSPI_CALL IsWindowVisible(tint32 iIndex) { return false; }
	//! IHost override
	virtual void KSPI_CALL RedrawWindow(kspi::IGUI* pCaller, tint32 iX, tint32 iY, tint32 iCX, tint32 iCY) {}
	//! IHost override
	virtual tint32 KSPI_CALL DoesWindowHaveToolbar(tint32 iIndex) {return 1;}
	
	//! Scans the system for plug-ins
	virtual void Init();
	
	struct SPlugInInfo {
		SPlugInInfo()
		: puiInputs(NULL), puiOutputs(NULL), puiSideChains(NULL) {}
		
		~SPlugInInfo() {
			if (puiSideChains) {
				delete[] puiInputs;
				delete[] puiOutputs;
				delete[] puiSideChains;
			}
		}
		
		enum EPlugInType {
			PlugInTypeEffect = 0,
			PlugInTypeSynth
		};
		
		EPlugInType PlugInType;
		
		std::string sPathName;
		tint32 iPlugInIndex;
		
		tuint32 uiChannelConfigurations;
		tuint32* puiInputs;
		tuint32* puiOutputs;
		tuint32* puiSideChains;
		
		tuint32 uiCompanyID;
		std::string sCompanyName;
		tuint32 uiProductID;
		std::string sProductName;
	};
	
	tint32 GetNrOfPlugIns() const {return (tint32)mPlugIns.size();}
	
	SPlugInInfo* GetPlugInInfo(tint32 iIndex) {std::list<SPlugInInfo*>::const_iterator it = mPlugIns.begin(); for (; iIndex; iIndex--) {it++;} return *it;}
	
	//! A plug-in handle uniquely identifies a plug-in instance
	typedef tint32 PlugInHandle;
	
	//! Value which means "no plug-in"
	const static PlugInHandle mInvalidHandleValue;
	
	//! Loads a new instance of a plug-in with a set of ids
	/*!
	 \param iCompanyID [in]; Company ID of plug-in
	 \param iProductID [in]; Product ID of plug-in
	 */
	PlugInHandle LoadPlugIn(tint32 iCompanyID, tint32 iProductID, tint32 iChannel, tint32 iInsertIndex);
	
	//! Loads a new instance of a plug-in with a given index (into our list)
	/*!
	 \param iIndex [in]; Index of plug-in
	 */
	PlugInHandle LoadPlugIn(tint32 iIndex, tint32 iChannel, tint32 iInsertIndex);
	
	//! Unloads instance of plug-in
	/*!
	 \param Handle [in]: Handle of plug-in to unload
	 */
	void UnloadPlugIn(PlugInHandle Handle, tint32 iChannel, tint32 iInsertIndex);
	
	//! Creates a new GUI for the plug-in
	/*!
	 \param Handle [in]: Handle of plug-in to create GUI for
	 \param iIndex [in]: Index of GUI to create
	 \return kspi::IGUI*: Pointer to KSPI GUI instance
	 */
	kspi::IGUI* CreateGUI(PlugInHandle Handle, tint32 iIndex);
	
	kspi::IPlugIn* GetPlugInFromHandle(PlugInHandle Handle);
	
	void OpenGUI(tint32 iChannel, tint32 iInsertIndex);
	
	void Close_GUI(PlugInHandle Handle, tint32 iChannel, tint32 iInsertIndex);
	
	void WindowClosed(tint32 iChannel, tint32 iInsertIndex);
	
	//! get vendor name
	std::string GetPlugInVendor(tint32 iInsertId);
	//! get plug-in name
	std::string GetPlugInName(tint32 iInsertId);
	
protected:
//	CKSPlugIn* mpPlugIn;
	
	std::list<SPlugInInfo*> mPlugIns;
	
	std::list<kspi::IPlugIn*> mLoadedPlugIns;
	
	struct SLoadedPlugInInfo {
		kspi::IPlugIn* pPlugIn;
		tbool bGUILoaded;
		CBaseGUI* pBaseGUI;
		CKSPlugInGUI* pPlugInGUI;
		kspi::IGUI* pGUI;
	};
	
	std::map<tint32, SLoadedPlugInInfo> mPlugInMap;
};
