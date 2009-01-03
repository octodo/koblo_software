// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
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

//! Called when a buffer of samples needs to be filled
/*!
 \param iID [in]: ID of callback as when SetCallback was called.
 \param pfOut [out]: Buffer to be filled
 \param iCount [in]: Number of samples to fill in buffer
 */

/*! \class CKSXML_Sign_In
  \brief class that check if the computer is online and if KS holds a valid username and password 
 \ based on TinyXml www.sourceforge.net/projects/tinyxml
 */

class CKSPlugIn;


class CKSXML_Sign_In
{
	
private:
	CKSPlugIn* mpKSPlugIn;
	
	//! loads username and password from pref file returns false if there is no pref file with a usernames
	virtual tbool Load_User_Name_And_Password();
	
	
public:
	
	CKSXML_Sign_In(CKSPlugIn* pKSPlugIn);
	~CKSXML_Sign_In();
	
	//! creates a new "empty" project on koblo.com
	/*! 
	 \ use user id and password
	 \ pasrses the XML handshake in to the internal data structure
	 */ 
	tbool Sign_In_Handshake();
	
	//! called when users upload or sign in
	virtual tbool Get_User_Name_And_Password();
	
	//! called when ok is pressed in the sign in dialog is "remember me" is checked
	virtual void Save_User_Name_And_Password();
	
	// Called when the user select sign out from the setup menu
	virtual void Sign_Out();
	
	//! remove username and password from disk
	virtual void Remove_Username_And_Password();
	
};

