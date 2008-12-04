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



class CConverter : public virtual IConverter
{
public:
	CConverter(tint32 iParmMin, tint32 iParmMax);
	virtual ~CConverter();

	//! IConverter override
	virtual EType GetType() const {return mType;}
	//! IConverter override
	virtual tuint GetStrength() const {return 0;}

protected:
	tint32 miParmMin;
	tint32 miParmMax;

	EType mType;
};

