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



class CScrollBar;

/*! \class CScrollPane
 * \brief Implementation of IScrollPane
*/
class CScrollPane : public virtual IScrollPane, public virtual CPane, public virtual CEventReceiver, public virtual ITimerCallback
{
public:
	//! Constructor
	CScrollPane();
	//! Destructor
	virtual ~CScrollPane();

	//! IDestructable override
	virtual void Destroy();

	//! IScrollPaneHorizontal override
	virtual void Init(tint32 iID, tint32 iTimerID);
	//! IScrollPaneHorizontal override
	virtual void SetScrollPos(const SScrollPos& ScrollPos, bool bRedraw);
	//! IScrollPaneHorizontal override
	virtual void GetScrollPos(SScrollPos& ScrollPos);
	//! IScrollPane override
	virtual tbool SlideTo(const SPos& posTarget, tint32 iMethod, tint32 iPctJump, tint32 iPctSlowest);
	//! IScrollPane override
	virtual void SetAutomaticResizeOfSubPane(tbool bEnabled = true, EResizeMode Mode = ResizeAbsolute, tfloat fValue = 1.0f);
	//! IScrollPane override
	virtual void SetMinSizeOfSubPane(SSize sizeMin);
	//! IScrollPane override
	virtual void GetSizeOfSubPane(SSize& sizeSub);
	//! IScrollPane override
	virtual void AddSlaveScrollPane(IScrollPane* pSlave, EResizeMode eMode);
	//! IScrollPane override
	virtual tbool RemoveSlaveScrollPane(IScrollPane* pSlave);

	//! CControl override
	virtual void SetSize(const SSize& Size);

	//! CPane override
	virtual void AddControl(IControl* pToAdd, const SPos& Pos);
	//! CPane override
	virtual void RemoveControl(IControl* pToRemove);
	//! CPane override
	virtual void SetSize(const SSize& Size, tbool bRedraw = false);
	//! CPane override
	virtual IControl* GetControl(tint32 iControlID);
	//! CPane override
	virtual void OnDraw(const SRect &rUpdate);
	//! CPane override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! CPane override
	virtual void GetMousePos(SPos& rPos) const;
	//! CPane override
	virtual void SetControlValue(tint32 iControlID, tint32 iValueNew, tbool bNotify = false);

	//! CEventReceiver override
	virtual void EventValueChange(IControl *pSender, tint32 iValueNew);

	void SetScrollBar(CScrollBar* pScrollBar) {mpScrollBar = pScrollBar;}

	//! IScrollPane implementation
	virtual void SuppressUpdatePositions(tbool bSuppress);

protected:
	CScrollBar* mpScrollBar;

	//! Returns the sub pane which all controls are inside
	/*!
		\return IPane: Sub pane which all controls are inside
	*/
	IPane* GetSubPane() {return dynamic_cast<IPane*>(*(mControls.begin()));}

	//! Overload
	const IPane* GetSubPane() const {return dynamic_cast<IPane*>(*(mControls.begin()));}

	// Lasse, out-comment 2008-05-09 - redundant value, "faster pussycat, kill! kill!"
	/*
	//! Current scroll position
	SScrollPos mScrollPos;
	*/
	// .. Lasse

	// Lasse, added 2007-08-29 -> -31 and 2007-09-11
	//! ITimerCallback implementation
	virtual void OnTimer();
	//! ITimerCallback implementation
	virtual void OnTimer(tint32 iTimerID);
	//! Sliding?
	tbool mbIsSliding;
	//! Timer
	ITimer* mpTimer;
	//! Unique timer ID
	tint32 miTimerID;
	//! Target for slide operation
	SPos mposTarget;
	SSize msizeSlideJump;
	SSize msizeSlideSlowest;
	//! Sliding method (reserved for future use)
	tint32 miSlideMethod;
	//tint32 miSliderPreventDisasterLevel;
	//! Calculate next jump according to method
	static int CalcJump(tint32 iMethod, tint32 iDistance, tint32 iJump, tint32 iSlowest);
	// .. Lasse

	// Lasse, added 2008-05-13
	struct SSlave {
		IScrollPane* mpSlave;
		EResizeMode meMode;

		SSlave(IScrollPane* pSlave, EResizeMode eMode)
		{ mpSlave = pSlave; meMode = eMode; };
	};
	std::list<SSlave> mlistSlaves;

	virtual void UpdateSlaves();
	// .. Lasse
};

