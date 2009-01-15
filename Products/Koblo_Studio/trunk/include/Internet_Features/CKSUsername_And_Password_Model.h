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

//! Called when a buffer of samples needs to be filled
/*!
 \param iID [in]: ID of callback as when SetCallback was called.
 \param pfOut [out]: Buffer to be filled
 \param iCount [in]: Number of samples to fill in buffer
 */

/*! \class CKSUsername_And_Password_Model
  \brief class that check if the computer is online and if KS holds a valid username and password 
 \ based on TinyXml www.sourceforge.net/projects/tinyxml
 */

class CKSApplication;


class CKSUsername_And_Password_Model
{
	
	
public:
	
	CKSUsername_And_Password_Model();
	~CKSUsername_And_Password_Model();
	
	//! check it the computer is online
	tbool Computer_Is_Online();
	
	//! load username and password from memory
	virtual tbool Get_User_Name_And_Password();
	
	//! load username and password from pref file
	virtual tbool Load_User_Name_And_Password();
	
	//! called when ok is pressed in the sign in dialog is "remember me" is checked
	virtual void Save_User_Name_And_Password();
	
	//! called when the user select "clear username and password" from the setup menu
	virtual void Clear_Username_And_Password();
	
	//! remove username and password from pref file
	virtual void Remove_Username_And_Password();
	
	//! called from the file menu opens the sign in dialog
	virtual void Open_Username_And_Password_Dialog();
	
	//! called when the user presses "Ok" in the sign in dialog
	void On_Username_And_Password_OK();
	
	private:
	
};

