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



class CConverterEnum : public virtual CConverter
{
public:
	CConverterEnum(tint32 iParmMax);
	virtual ~CConverterEnum();

	// IDestructable
	virtual void Destroy();

	// IConverter
	virtual tint32 Parm2GUI(tint32 iParm) const;
	virtual tint32 GUI2Parm(tint32 iGUI) const;
	virtual tfloat64 Parm2Normalised(tint32 iParm) const;
	virtual tint32 Normalised2Parm(tfloat64 fNormalised) const;

protected:
};

