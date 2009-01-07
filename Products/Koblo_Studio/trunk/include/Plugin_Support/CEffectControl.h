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


class CEffectControl : public virtual ge::ICustomControlCallback
{
public:
	CEffectControl(ge::ICustomControl* pControl, kspi::IGUI* pPlugInGUI);

	virtual ~CEffectControl();

	//! ICustomControlCallback override
	virtual void OnDraw(const ge::SRect &rUpdate);
	//! ICustomControlCallback override
	virtual tbool OnMouse(ge::EMouseMsg MouseMsg, const ge::SPos& Pos);
	//! ICustomControlCallback override
	virtual tbool OnKeyDown(ge::EKey Key);
	//! ICustomControlCallback override
	virtual void OnTimer();
	//! ICustomControlCallback override
	virtual void OnValueChange();
	//! ICustomControlCallback override
	virtual void SetSize(const ge::SSize& Size);
	//! ICustomControlCallback override
	virtual void SetPos(const ge::SPos& Pos);

	virtual void Init();

protected:
	ge::ICustomControl* mpControl;

	kspi::IGUI* mpPlugInGUI;

	tbool mbMouseCaptured;
};
