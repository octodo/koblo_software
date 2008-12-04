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


// Forwards
class CKVerbPlugIn;

class CParmGlobal : public virtual de::IParameter
{
public:
	CParmGlobal(CKVerbPlugIn* pPlugIn, tint32 iID, tint32 iMinValue, tint32 iMaxValue, tint32 iValue = 0);

	virtual ~CParmGlobal();

	virtual void Destroy();

	// IParameter override
	virtual tint32 GetID() const;
	// IParameter override
	virtual tint32 GetDefaultValue() const;
	// IParameter override
	virtual tint32 GetValue() const;
	// IParameter override
	virtual void SetValue(tint32 iValueNew, tbool bCallOnChange = true);
	// IParameter override
	virtual tint32 GetMinValue() const;
	// IParameter override
	virtual tint32 GetMaxValue() const;
	// IParameter override
	virtual void OnChange();

protected:
	CKVerbPlugIn* mpPlugIn;
	tint32 miID;
	tint32 miDefaultValue;
	tint32 miValue;
	tint32 miMinValue;
	tint32 miMaxValue;
};
