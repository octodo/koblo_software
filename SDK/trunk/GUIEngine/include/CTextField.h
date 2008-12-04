/*!	\file CText.h
	\author Michael Olsen
	\date 01/Jan/2005
	\date 01/Jan/2005
*/

/*! \class CText
 * \brief Implementation of ITextField
*/
class CTextField : public virtual ITextField, public virtual CControl
{
public:
	//! Constructor
	CTextField();
	//! Destructor
	virtual ~CTextField();

	//! IDestructable override
	virtual void Destroy();

	//! IControl override
	virtual void OnDraw(const SRect &rUpdate);
	//! IControl override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! IControl override
	virtual tbool OnKeyDown(EKey Key);
	//! IControl override
	virtual void OnTimer();
	//! IControl override
	virtual void SetText(const tchar* pszText);
	//! IControl override
	virtual void GetText(tchar* pszText);
	//! IControl override
	virtual void SetConvertedValue(tint32 iNew, tbool bNotify);

	//! ITextField override
	virtual void Init(tint iID, IFontResource* pFont);
	//! ITextField override
	virtual void ChangeFont(IFontResource* pFont);
	//! ITextField override
	virtual void SetHorzAlignment(EHorizontalAlignment Alignment);
	//! ITextField override
	virtual void SetVertAlignment(EVerticalAlignment Alignment);
	//! ITextField override
	virtual void EnableTextLimiting(tbool bEnable = true);
	//! ITextField override
	virtual void MakeEditable();
	//! ITextField override
	virtual void SetClicksOutsideAsSuccess();
	//ITextField override
	virtual void SetTextFieldSize(ge::SSize Size);
	//ITextField override
	virtual void SetMaxNumberLines(tint iMaxLines = -1);

protected:
	//! Rect to draw inside
	ge::SRect mRect;
	//! Current text
	std::string msTextCur;

	//! Current text while editing
	std::string msTextCurEditing;

	//! Current horizontal alignment
	EHorizontalAlignment mAlignmentHorz;
	//! Current vertical alignment
	EVerticalAlignment mAlignmentVert;

	//! If true text limiting is on, otherwise it's off
	tbool mbLimiting;

	//! If true, text is editable, otherwise it's not editable
	tbool mbEditable;

	//! If true, we're currently editing, otherwise not
	tbool mbEditing;

	//! Limitation on number off lines.
	tint miMaxLines;

	//! If we're editing, this is the position of the cursor
	tint miCursorPos;

	//! Selection types for editing
	enum ESelectionType
	{
		//! No selection
		SelectionNone = 0,

		//! All selected
		SelectionAll
	};

	//! If we're editing, this is the current selection type
	ESelectionType mSelectionType;

	//! If true the text cursor is currently on, otherwise it's off (blinking)
	tbool mbCursorOn;

	//! When it reaches 0 the cursor should toggle (blinking)
	tint32 miCursorCounter;

	//! Time when we were in timer last
	tuint32 muiTimerLast;

	//! If true a click outside the control will end editing as if pressed return. Otherwise it will end as if pressed cancel.
	tbool mbClicksOutsideIsSuccess;

	//! How fast the cursor blinks in ms (fyll cycle ON-OFF = 2 x the value)
	tint miCursorBlinkRateMs;

	//! Do text limiting
	/*!
		\param rs [in/out]: Text to limit. Will be changed if neccasary
	*/
//	virtual void DoLimiting(std::string& rs);

	/*! Get Line, 
	\takes a string input and clips the start off it 
	\returns the Y possition of the next line	
		\param strIn [in]: offset in characters
		\param strIn [in]: Text to shoop a line from
		\param strIn [out]: line off text without the offset
	*/
	virtual tint GetLine(tint iOffSet, std::string& strIn, std::string& strOut );

	//! Draw lines
	/*!
		\param rs [in]: Text draw in lines
	*/
	virtual void DrawLines(std::string& strIn, const SRect &rUpdate);

	//! Draw Cursor
	/*!
		\param rs [in]: Text draw in lines
	*/
	virtual void DrawCursor(std::string& strIn,SRect Rect, const SRect &rUpdate);

	//! Called to start editing
	virtual void StartEditing();

	//! Called to end editing
	/*!
		\param bOK [in]: If true the user didn't cancel, and the text should be updated
	*/
	virtual void StopEditing(tbool bOK);

	//! Returns the draw rect for a given text
	/*!
		\param rRect [out]: Drawing rect for text
		\param sText [in]: Text to get drawing rect for
	*/
	virtual void GetDrawRect(SRect& rRect, const std::string& sText);

	//! Returns the index into text based on x offset (pixels)
	/*!
		\param sText [in]: Text to get index into
		\param iOffset [in]: Offset in pixels
		\return tint: Character index
	*/
	virtual tint GetCharacterIndex(const std::string& sText, tint iOffset);




};
