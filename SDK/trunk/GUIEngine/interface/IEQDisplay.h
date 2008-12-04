/*!	\file IEQDisplay.h
	\author Michael Olsen
	\date 18/Nov/2005
	\date 18/Nov/2005
*/

/*! \class IEQDisplay
 * \brief An interface for a eq display with moveable "dots" and eq curve
*/
class IEQDisplay : public virtual IPane, public virtual IDestructable
{
public:
	//! Create IEQDisplay
	static IEQDisplay* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pWindow [in]: Window control will work in
		\param pResMan [in]: Resource manager to get resources from
		\param iDotResID [in]: Resource ID of the first "dot". Each other dot (one for each band) should follow in series.
		\param iDotOffsetX [in]: Offset in dot bitmaps into center position for x direction
		\param iDotOffsetY [in]: Offset in dot bitmaps into center position for y direction
		\param uiNrOfBands [in]: Number of bands. Must be 1 or higher.
		\param Colour [in]: Colour of curve. Defaults to 128, 192, 64
	*/
	virtual void Init(tint iID, IWindow* pWindow, IResourceManager* pResMan, tint32 iDotResID, tint32 iControlIDDot1, tint32 iControlIDDotSpan, tint iDotOffsetX, tint iDotOffsetY, tuint uiNrOfBands, SRGB Colour = SRGB(128, 192, 64)) = 0;

	//! Initializer for displays without "dots", i.e. a passive control
	/*!
		\param iID [in]: Control ID
		\param pWindow [in]: Window control will work in
		\param uiNrOfBands [in]: Number of bands. Must be 1 or higher.
		\param Colour [in]: Colour of curve.
	*/
	virtual void Init2(tint iID, IWindow* pWindow, tuint uiNrOfBands, SRGB Colour) = 0;

	//! Sets the callback
	/*!
		\param pCallback [in]: Callback to set. Cannot be NULL
		\param iCallbackUserDefined [in]: User defined value, passed with the callback.
	*/
	virtual void SetCallback(IEQDisplayCallback* pCallback, tint32 iCallbackUserDefined) = 0;

	//! Sets the min and max dB. Can safely be called more than once. If never called the values defaults to -24dB to +12dB.
	/*!
		\param fdBMin [in]: New min dB of display
		\param fdBMax [in]. New max dB of display
	*/
	virtual void SetdBMinAndMax(tfloat fdBMin, tfloat fdBMax) = 0;

	//! Sets info for 1 band
	/*!
		\param uiBand [in]: Band to set info for
		\param bEnabled [in]: True if band is enabled (visible), otherwise false
		\param fFreqIndex [in]: Index (0..1) of frequency
		\param fdBIndex [in]: Index (0..1) of dB
	*/
	virtual void SetBandInfo(tuint uiBand, tbool bEnabled, tfloat32 fFreqIndex, tfloat32 fdBIndex) = 0;

	//! Force an update (and thus redraw)
	virtual void Update() = 0;
};
