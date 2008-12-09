/*!	\file IPeakMeter.h
	\author Lasse Steen Bohnstedt
	\date 19/Sep/2006
*/

/*! \class IPeakMeter.h
 * \brief IPeakMeter consists of two bitmaps, 1 background (lights off) and 1 front (lights ON)
 *
 *	After creating the IPeakMeter you must add at least one bitmap to its implicit IPane.
 *  The bitmap added *last* is the one that's resized.
 *
 *  Default growing direction for meter is "Up".
 *  If direction "Down" or "Right" is used you MUST add any LEDs OFF images first (if needed), then the foreground (with LEDs ON).
 *  For directions "Up" or "Left" you MUST add any LEDs ON images first (if needed), then the background (with LEDs OFF).
*/
class IPeakMeter : public virtual IPane
{
public:
	//! Create IPeakMeter
	static IPeakMeter* Create();

	enum EDirection {
		Up,
		Down,
		Left,
		Right
	};

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param iSteps [in]: Indicates how many steps the meter is divided into
		\param eDir [in]: Indicates in which direction the peek meter should grow (Up/Down/Left/Right).
		\param iSpacing0 [in]: How many pixel lines in the bitmap to "ignore" for level 0.000 (silence)
		\param iSpacing1 [in]: How many pixel lines in the bitmap to "ignore" for level 1.000 (full volume)
	*/
	virtual void Init(tint32 iID, tint32 iSteps, EDirection eDir = Up, tint32 iSpacing0 = 0, tint32 iSpacing1 = 0) = 0;

	//! Set the direction in which meter grows
	/*!
		\param eDir [in]: Indicates in which direction the peek meter should grow (Up/Down/Left/Right).
	*/
	virtual void SetGrowingDirection(EDirection eDir = Up) = 0;

	//! Set meter value
	/*!
		\param fValue [in]: Accepts values from 0.000 (all lights off) to 1.000 (all lights ON)
	*/
	virtual void SetFloatValue(tfloat fValue, tbool bNotify = true) = 0;
	
	//! Set meter value, but only if higher than latest value x decay.
	/*!
		\param fValue [in]: Accepts values from 0.000 (all lights off) to 1.000 (all lights ON)
		\param fDecayMul [in]: Factor to multiply latest value with (0.000 - 1.000) for compare
	 */
	virtual void SetFloatValueWithDecay(tfloat fValue, tfloat fDecayMul, tbool bNotify = true) = 0;

};
