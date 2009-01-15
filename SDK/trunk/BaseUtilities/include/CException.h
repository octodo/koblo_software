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

/*! \class CException
 * \brief Implementation of IException
*/
class CException : public virtual IException
{
public:
	//! Constructor
	CException(EType Type, EReason Reason, const tchar* pszFile, tint32 iLine, const tchar* pszDate, const tchar* pszDescription = NULL);
	//! Destructor
	virtual ~CException();

	//! IDestructable override
	virtual void Destroy();

	//! IException override
	virtual EType GetType();
	//! IException override
	virtual const EType GetType() const;
	//! IException override
	virtual EReason GetReason();
	//! IException override
	virtual const EReason GetReason() const;
	//! IException override
	virtual const tchar* GetDescription() const;
	//! IException override
	virtual const tchar* GetFullDescription() const;

protected:
	EType mType;
	EReason mReason;
	std::string msDescription;
	std::string msFile;
	std::string msLine;
	std::string msDate;
	std::string msFullDescription;

	//! Builds full description string
	virtual void MakeFullDescription();

	//! Appends type text to string
	/*!
		\parm rsString [in/out]: String to be appended
	*/
	virtual void GetTypeText(std::string& rsString);

	//! Appends reason text to string
	/*!
		\parm rsString [in/out]: String to be appended
	*/
	virtual void GetReasonText(std::string& rsString);
};






