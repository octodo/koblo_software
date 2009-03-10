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

//!!! TO ASK is there always only one channel in a sound?
//#define MAX_CHANNELS_IN_SOUND 0

/*! \brief CSample process on CWave_File \
 *  trim start and end \
 */

class CXML_Hand_Shakes:
public virtual CKSUUID
{
public:
	CXML_Hand_Shakes();
	
	virtual ~CXML_Hand_Shakes();
	
	tbool Set_Take_Handshake(std::string sXML);
	
	tbool Set_Branch_Permission_XML(std::string sXML);
	
	//! pass revision handshake returned from koblo.com
	virtual void Pass_Branch_Revision(std::string sXML ); 
	
	//! pass permission handshake returned from koblo.com
	virtual void Pass_Branch_Permissions(std::string sXML ); 
	
	
	
	


	

protected:
	
	//**************** Take Handshake passer *****************
	//! read the  Take DOM file 
	void Set_Take_Info( TiXmlNode* pParent );
	
	//! read the  Take tag file 
	void Pass_Take_Tags( TiXmlNode* pParent );
	
	//! read the  Take tag file 
	void Pass_Take_Tag( TiXmlNode* pParent );
	
	//! read the  take sample tag 
	void Pass_Take_Sample( TiXmlNode* pParent );
	
	//! read the  take description tag 
	void Pass_Take_Description( TiXmlNode* pParent );
	
	//! read the  take audio/ ogg url tag 
	void Pass_Take_Audio( TiXmlNode* pParent );
	
	//! read the  take audio/ mp3 url tag 
	void Pass_Take_MP3( TiXmlNode* pParent );
	
	//! read the  take Author tag 
	void Pass_Take_Author( TiXmlNode* pParent );
	
	std::string msTake_UUID;
	
	std::string msTake_Audio_URL;
	
	std::string msTake_MP3_URL;
	
	virtual void Update_Take_Data();
	
	//**************** Take Handshake passer *****************
	
	std::string msSampleUUID;
	
	//  TinyXml document
	TiXmlDocument* mpXMLHand_Shake;
	
	CMutex mHands_Shake_Mutex;
	
//	void Pass_Branch_Revision (TiXmlNode* pParent);
	
	void Read_Branch_Revision_Tag(TiXmlNode* pParent);
	
//	void Read_Branch_Revision_Object(TiXmlNode* pParent);
	
	//! pass the branch tag
	void Read_Branch_Tag(TiXmlNode* pParent);
	
	//! pass the permission tags
	void Read_Permissions_Tag(TiXmlNode* pParent);
	
	//! pass the permission tags
	void Read_Permissions_Object(TiXmlNode* pParent);
	
	//! read permission
	tbool mbRead_Permission;
	
	//! write permission
	tbool mbWrite_Permission;
	
	tuint32 uiState;
	
	
	
	
	

	
//	void Read_Branch_Permission_Object(TiXmlNode* pParent);
	
	
	
};

