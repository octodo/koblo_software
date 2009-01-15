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





class CX3DisplayConverter : public virtual de::IDisplayConverter
{
public:
	CX3DisplayConverter();

	virtual ~CX3DisplayConverter();

	virtual void Destroy();

	//! IDisplayConverter override
	virtual void Value2String(tint32 iValue, tchar* psz);
	//! IDisplayConverter override
	virtual tint32 String2Value(const tchar* psz) throw (IException*);
	//! IDisplayConverter override
	virtual void SetPostFix(const tchar* psz);
	//! IDisplayConverter override
	virtual void AddFixedString(tint32 iValue, const tchar* psz);

protected:
	de::IDisplayConverter* mpConverter;
};



